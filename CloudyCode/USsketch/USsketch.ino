/**
 * Controller software for the Smart Umbrella Stand
 * 
 * Author: Giacomo Del Rio
 */

#define DEBUG_BUILD  // Undefine this for release compiling

#include <tuple>
#include <vector>
#include "SPI.h"
#include "SD.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "DrawBitmap.h"
#include "WifiConnector.h"
#include "IBeaconDetector.h"
#include "BeaconsFinder.h"
#include "USUtils.h"
#include "DueTimer.h"
#include "Switch.h"

#define TFT_CS_PIN 2                    // TFT Chip Select pin
#define TFT_DC_PIN 3                    // TFT data/command
#define TFT_LITE_PIN 4                  // TFT data/command
#define SD_CS_PIN  5                    // SD card chip select
#define PIEZO_PIN 13                    // Piezo pin
#define BTN_UP_PIN 31                   // Arrow up button
#define BTN_MIDDLE_PIN 33               // Middle button
#define BTN_DOWN_PIN 35                 // Arrow down button
#define PIR_SENSOR_PIN 39               // PIR sensor
#define ESP_RESET_PIN 43                // ESP8266 reset pin
#define HOOK_LEFT_PIN 47                // Left hook
#define HOOK_RIGHT_PIN 45               // Right hook

#define STAY_ON_MSECS 50000                // Time to stay on after a motion detection (msecs)
#define BEACONS_RANGE 0.5                  // Range in which beacons are considered alive (meters)
#define BEACONS_RETAIN_TIME 5000           // Once a beacon has been discovered, consider it alive for this time (msecs)
#define REMINDER_IDLE_TIME 10000           // Once a reminder has been displayed, don't show it again for this time (msecs)
#define MIN_REMINDER_REQUEST_INTERVAL 3000 // Minimum time between two consecutive reminders request
#define WIFI_SSID "arduino"      // SSID of the wifi to connect to
#define WIFI_PASSWORD "wifimodule123"

using namespace std;

enum ControllerState{
  DISCOVERY,
  SHOW_FORECAST,
  ACQUIRE_REMINDERS,
  SHOW_REMINDERS
};

/****************** GLOBAL VARIABLES ******************/

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS_PIN, TFT_DC_PIN);  // 2.8" 320x240 TFT display
BitmapDrawer bmpDrawer = BitmapDrawer(&tft, 40);  // Allows to draw .bmp images stored in the SD card
WifiConnector wifiConnector(Serial3);  // Communicate with server via wifi
IBeaconDetector beaconDetector(Serial1);  // Detect active iBeacons
BeaconsFinder activeBeacons(beaconDetector, BEACONS_RANGE, BEACONS_RETAIN_TIME); // Maintain a list of active beacons in range 

// Debouncers
Switch btnUp =     Switch(BTN_UP_PIN,     INPUT_PULLUP, LOW, 50,  300, 250, 40);
Switch btnMiddle = Switch(BTN_MIDDLE_PIN, INPUT_PULLUP, LOW, 50,  300, 250, 40);
Switch btnDown =   Switch(BTN_DOWN_PIN,   INPUT_PULLUP, LOW, 50,  300, 250, 40);
Switch hookLeft =  Switch(HOOK_LEFT_PIN,  INPUT_PULLUP, LOW, 200, 300, 250, 80);
Switch hookRight = Switch(HOOK_RIGHT_PIN, INPUT_PULLUP, LOW, 200, 300, 250, 80);

Forecast forecast;  // Hold the forecast received by the web service
vector<Reminder> reminders;  // Reminders received by the web service
vector<tuple<UUID, size_t, unsigned long>> remindersFilter; // Remember already displayed reminders
int actualDisplayedReminder = -1;   // Tells which reminder is currently displayed (-1 if any)
int desiredDisplayedReminder = -1;  // Tells which reminder is ought to be displayed (-1 if any)

// Seconds to stay on before return to DISCOVERY state. Set to STAY_ON_MSECS in
// case of motion detection by the PIR sensor
int stayOnRemainingMSecs = 0;

// Hold the time in ms in which the previous main loop begins and lasts
unsigned long lastMainLoopTime = 0;
int           lastMainLoopDuration = 0;

// Hold the time in which the last request for reminders was sent
unsigned long lastReminderRequest = 0;

ControllerState state = DISCOVERY;  // State of the controller


/****************** MAIN FUNCTIONS ******************/

/* Poll and debounce buttons and hooks
*/
void pollAllButtons()
{
  hookLeft.poll();
  hookRight.poll();
  btnUp.poll();
  btnMiddle.poll();
  btnDown.poll();
}

/* Read PIR sensor and update the stayOnRemainingMSecs variable
*/
void updateStayOnRemainingMSecs()
{
  if( digitalRead(PIR_SENSOR_PIN) ){
    stayOnRemainingMSecs = STAY_ON_MSECS;
  }else{
    stayOnRemainingMSecs -= lastMainLoopDuration;
    if(stayOnRemainingMSecs < 0)
      stayOnRemainingMSecs = 0;
  }
}

/* Returns true if the reminder has already been displayed within REMINDER_IDLE_TIME
*/
bool isAlreadyDisplayed( size_t reminderHash, UUID& uuid ){
  auto flt = begin(remindersFilter);
  while( flt != end(remindersFilter) ){
    if( millis() - get<2>(*flt) > REMINDER_IDLE_TIME ){
      // Expired filter
      flt = remindersFilter.erase(flt);
    }else{
      // Active filter
      if( uuid == get<0>(*flt) &&  reminderHash == get<1>(*flt) )
        return true; // Already displayed within REMINDER_IDLE_TIME
      flt++;
    }
  }

  return false;
}

/* Insert the actual displayed reminder into the list of the already displayed reminders
*/
void discardActualDisplayedReminder(){
  remindersFilter.push_back( 
    make_tuple( reminders[actualDisplayedReminder].uuid, 
                reminders[actualDisplayedReminder].hash(),
                millis() ) );
  reminders.erase(reminders.begin() + actualDisplayedReminder);
  desiredDisplayedReminder = min(actualDisplayedReminder, (int)reminders.size() -1);
  actualDisplayedReminder = -1;
  lowSingleBeep(PIEZO_PIN);
  delay(300);
}

/* Prepare the peripherals and setup variables
*/
void setup() {
  #ifdef DEBUG_BUILD
    Serial.begin(9600);  // Debug
  #endif //DEBUG_BUILD

  // Set pins
  pinMode(TFT_LITE_PIN, OUTPUT);
  pinMode(PIEZO_PIN, OUTPUT);
  pinMode(ESP_RESET_PIN, OUTPUT);
  pinMode(PIR_SENSOR_PIN, INPUT);

  // Rest the ESP8266
  analogWrite(ESP_RESET_PIN, LOW);
  delay(30);
  analogWrite(ESP_RESET_PIN, HIGH);
  delay(500);

  // Initialize TFT display
  tft.begin();         
  tft.setRotation(3);  // Set proper landscape orientation
  analogWrite(TFT_LITE_PIN, 0); // Turn off backlight
  yield();

  //Initialize SD card reader
  if (!SD.begin(SD_CS_PIN)){
    DEBUG("Failed SD card reader initialization!");
  }
  yield();

  // Initialize Beacon Detector
  Serial1.begin(57600);
  beaconDetector.initialize();

  // Initialize WifiConnector serial
  Serial3.begin(115200);
  while( !Serial3 ){
    delay(10);
  }
  DEBUG("Serial3 ready");
  //wifiConnector.connect(WIFI_SSID, WIFI_PASSWORD);

  // Setup timer interrupt to poll and debounce buttons
  Timer1.attachInterrupt(pollAllButtons).setFrequency(5).start();
}


/* Main loop
*/
void loop() {
  lastMainLoopDuration = millis() - lastMainLoopTime;
  lastMainLoopTime = millis();

  updateStayOnRemainingMSecs();
  
  switch(state){
    case DISCOVERY:
      activeBeacons.scan();

      // --> SHOW_FORECAST
      if(stayOnRemainingMSecs > 0){
        //wifiConnector.getForecast(forecast);
        getDemoForecast(forecast);
        drawForecastOnScreen(&tft, bmpDrawer, forecast);
        analogWrite(TFT_LITE_PIN, 255);
        state = SHOW_FORECAST;
      }
      break;
      
    case SHOW_FORECAST:
      activeBeacons.scan();

      // --> ACQUIRE_REMINDERS
      if(activeBeacons.getBeaconsInRange().size() > 0 &&
         (millis() - lastReminderRequest) > MIN_REMINDER_REQUEST_INTERVAL){
        state = ACQUIRE_REMINDERS;
        break;
      }

      // --> DISCOVERY
      if(stayOnRemainingMSecs <= 0){
        clearScreen(&tft);
        analogWrite(TFT_LITE_PIN, 0);
        state = DISCOVERY;
      }
      break;
      
    case ACQUIRE_REMINDERS:{
      // Acquire reminders
      lastReminderRequest = millis();
      vector<Reminder> tmp_rem;
      reminders.clear();
      const vector<UUID>& uuids = activeBeacons.getBeaconsInRange();
      for( auto uuid: uuids ){
         tmp_rem.clear();
         char str[25]; sprintUUID(str, uuid.uuid); DEBUG(str);
         getDemoReminders(tmp_rem, uuid);
         //wifiConnector.getReminders(tmp_rem, uuid);

        // Filter reminders
        for( auto r: tmp_rem )
          if( !isAlreadyDisplayed(r.hash(), uuid) ){
              if( r.hookNumber == 0 ||
                  r.hookNumber == 1 && !hookLeft.on() ||
                  r.hookNumber == 2 && !hookRight.on() )
                reminders.push_back(r);
          }
      }

      if(reminders.size() > 0){
        // --> SHOW_REMINDERS
        doubleBeep(PIEZO_PIN);
        actualDisplayedReminder = -1;
        desiredDisplayedReminder = 0;
        state = SHOW_REMINDERS;
        DEBUG(reminders.size());
      }else{
        // --> SHOW_FORECAST
        state = SHOW_FORECAST;
      }
      break;
    }
      
    case SHOW_REMINDERS:
      // Display selected reminder
      if(actualDisplayedReminder != desiredDisplayedReminder){
        drawReminderOnScreen(&tft, bmpDrawer, reminders, desiredDisplayedReminder);
        actualDisplayedReminder = desiredDisplayedReminder;
      }

      if( btnUp.on() ){
        desiredDisplayedReminder = max(desiredDisplayedReminder - 1, 0);
      }else if( btnDown.on() ){
        desiredDisplayedReminder = min(desiredDisplayedReminder + 1, (int)reminders.size() -1);
      }else if( btnMiddle.on() ){
        discardActualDisplayedReminder();
      }else if( hookLeft.on() && reminders[actualDisplayedReminder].hookNumber == 1){
        discardActualDisplayedReminder();
      }else if( hookRight.on() && reminders[actualDisplayedReminder].hookNumber == 2){
        discardActualDisplayedReminder();
      }

      if( reminders.size() == 0 ){
        // --> SHOW_FORECAST
        drawForecastOnScreen(&tft, bmpDrawer, forecast);
        state = SHOW_FORECAST;
      }else if( stayOnRemainingMSecs <= 0 ){
        // --> DISCOVERY
        clearScreen(&tft);
        analogWrite(TFT_LITE_PIN, 0);
        state = DISCOVERY;
      }
      break;
  }
}

