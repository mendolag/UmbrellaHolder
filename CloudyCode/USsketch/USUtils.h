/*
 * Umbrella Stand Utility file
 * 
 * Contains common data structures and auxiliary functions
 * 
 * Author: Giacomo Del Rio
 */

#ifndef USUTILS_H_
#define USUTILS_H_

//#define DEBUG_BUILD

#include <vector>
#include "DrawBitmap.h"
#include "Fonts/FreeSansBold12pt7b.h"
#include "Fonts/FreeMono9pt7b.h"

// Debugging Macros
#ifdef DEBUG_BUILD
#  define DEBUG(x) Serial.println(x)
#else
#  define DEBUG(x) do {} while (0)
#endif

using namespace std;

// Maps numbers to icons filename
extern const char* iconsFilename[];

// UUID of an iBeacon
struct UUID{
  uint8_t uuid[16];

  UUID(){};  
  UUID(uint8_t* uuid){
    memcpy(this->uuid, uuid, 16);
  }
  
  bool operator==(const UUID &other) const{
    return equal(uuid, uuid + 16, other.uuid);
  }
};

// Reminder. A reminder with hookNumber==0 will be always shown, a
// reminder with hookNumber>0 will be shown only if teh corresponding
// hook is not empty (has something attached).
struct Reminder{
  UUID uuid;               // UUID of the beacon associated to this reminder
  char recipientName[20];  // Name of the person addressed to this reminder
  char description[100];   // The text associated to this reminder
  int hookNumber;          // 0: no hook, 1: left hook, 2: right hook

  size_t hash();  // Returns an hash of this reminder
};

// Forecast relative to a specific period.
struct ForecastLine{
  int icon;
  int rainProbablilty;
  int tempMin;
  int tempMax;
  int windSpeed;
  int humidity;
};

// Global forecast that can hold multiple ForecastLine
struct Forecast{
  char locationName[21];   // Location name of the forecasts. Eg: Lugano
  char currentDate[11];    // Current date. Eg: Mon 24 Jan
  char currentTime[6];     // Current time. Eg: 12:32
  ForecastLine lines[3];   // 0: now, 1: 1 hour, 2: Today
};

// Clear the screen
void clearScreen(Adafruit_ILI9341* tft);

// Draws a forecast on the screen
void drawForecastOnScreen(Adafruit_ILI9341* tft, BitmapDrawer bmpDrawer, const Forecast& forecast);

// Draws a reminder on the screen. It will display also the arrows if there are more than
// a reminder (totalReminders>0)
void drawReminderOnScreen(Adafruit_ILI9341* tft, BitmapDrawer bmpDrawer, const vector<Reminder>& reminder,
                          int actualReminder);

// Draws a long text splitted into lines
void drawMultilineText(Adafruit_ILI9341* tft, const char* text, int rowLen, int x, int y, int rowSpace);

// Play a double beep
void doubleBeep(int piezoPin);
void lowSingleBeep(int piezoPin);

// Debug functions
void sprintUUID(char* buf, const uint8_t* uuid);
void getDemoForecast(Forecast& forecast);
void getDemoReminders(vector<Reminder>& reminder, const UUID& uuid);

#endif //USUTILS_H_
