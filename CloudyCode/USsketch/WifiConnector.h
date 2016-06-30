/*
 * Use an ESP8266 to send request to the server.
 * 
 * Author: Giacomo Del Rio
*/

#define DEBUG_BUILD  // Undefine this for release compiling

#ifndef WIFI_CONNECTOR_H_
#define WIFI_CONNECTOR_H_

#ifdef DEBUG_BUILD
#  define DEBUG(x) Serial.println(x)
#else
#  define DEBUG(x) do {} while (0)
#endif

#include <vector>
#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include "USUtils.h"

#define LINE_TERMINATOR '\n'
#define SERIAL_BUFFER_LENGHT 2048
#define HOST "192.168.43.251"
#define PORT 3000
#define FORECAST_URL "/todayForecast"
#define REMINDER_URL "/reminder"

using namespace std;

class WifiConnector {
  public:
    WifiConnector(HardwareSerial& espSerial);
    void connect(const char* ssid, const char* password);
    void getForecast(Forecast& forecast);
    void getReminders(vector<Reminder>& reminder, const UUID& uuid);
  
  private:
    HardwareSerial& espSerial;                   // Serial attached to ESP8266
    char commBuffer[SERIAL_BUFFER_LENGHT];       // Buffer for serial communication with ESP8266
    int getIcon(const char* icon);

    void httpGetRequest(const char* request);    // Make an HTTP get request to the server
    void writeSerialLine(const char* buf);       // Write a string to the serial attached to the ESP8266
    int  readSerialLine();                       // Read a line from the serial attached to the ESP8266 and put the content into commBuffer
};

#endif  //WIFI_CONNECTOR_H_
