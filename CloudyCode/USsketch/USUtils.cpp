/* 
 *  Implementation of USUtils.h
*/

#include "USUtils.h"

const char* iconsFilename[] = {
  "sunny.bmp",    // 0
  "rain.bmp",     // 1
  "cloudy.bmp",   // 2
  "foggy.bmp",    // 3
  "msunny.bmp",   // 4
  "pcloudy.bmp",  // 5
  "psunny.bmp",   // 6
  "rshowers.bmp", // 7
  "sleet.bmp",    // 8
  "snow.bmp",     // 9
  "tstorms.bmp",  // 10
  "unknown.bmp",  // 11
  "clear.bmp"     // 12
};

size_t Reminder::hash(){
  // Fowler–Noll–Vo hash function
  size_t h = 2166136261;

  for(char* it = recipientName; *it; ++it){
    h = (h * 16777619) ^ *it;
  }

  for(char* it = description; *it; ++it){
    h = (h * 16777619) ^ *it;
  }

  h = (h * 16777619) ^ hookNumber;

  return h;
}

void clearScreen(Adafruit_ILI9341* tft){
  tft->fillScreen(ILI9341_WHITE);
}

void drawForecastOnScreen(Adafruit_ILI9341* tft, BitmapDrawer bmpDrawer, const Forecast& forecast){
  char buf[6];
  
  // Prepare screen
  tft->fillScreen(ILI9341_WHITE);
  tft->drawLine(4, 28, 315, 28, ILI9341_GREEN);

  // Location name
  tft->setFont(&FreeSansBold12pt7b);  
  tft->setTextColor(ILI9341_BLACK);  
  tft->setTextSize(1);
  tft->setCursor(2, 26);
  tft->print(forecast.locationName);

  // Current date
  tft->setFont();
  tft->setTextSize(1);
  tft->setCursor(190, 20);
  tft->print(forecast.currentDate);

  // Current time
  tft->setTextSize(2);
  tft->setCursor(258, 13);
  tft->print(forecast.currentTime);

  //tft->drawLine(70, 28, 70, 239, ILI9341_RED);
  //tft->drawLine(153, 28, 153, 239, ILI9341_RED);
  //tft->drawLine(237, 28, 237, 239, ILI9341_RED);

  // Legend column
  tft->setTextSize(1);
  tft->setCursor(3, 125);
  tft->print("Rain prob.");
  tft->setCursor(3, 150);
  tft->print("T Min-Max");
  tft->setCursor(3, 175);
  tft->print("Wind m/s");
  tft->setCursor(3, 200);
  tft->print("Humidity");
  tft->drawLine(72, 108, 317, 108, ILI9341_GREEN);
  tft->drawLine(72, 223, 317, 223, ILI9341_GREEN);
  
  // Now
  bmpDrawer.draw(iconsFilename[forecast.lines[0].icon], 86, 35);
  tft->setFont(&FreeMono9pt7b);
  tft->setTextSize(1);
  tft->setCursor(94, 100);
  tft->print("Now");
  tft->setCursor(83, 131);
  sprintf(buf, "%d%%", forecast.lines[0].rainProbablilty);
  tft->print(buf);
  tft->setCursor(83, 156);
  sprintf(buf, "%d-%d", forecast.lines[0].tempMin, forecast.lines[0].tempMax);
  tft->print(buf);
  tft->setCursor(83, 181);
  sprintf(buf, "%d", forecast.lines[0].windSpeed);
  tft->print(buf);
  tft->setCursor(83, 206);
  sprintf(buf, "%d%%", forecast.lines[0].humidity);
  tft->print(buf);

  // 1Hour
  bmpDrawer.draw(iconsFilename[forecast.lines[1].icon], 169, 35);
  tft->setFont(&FreeMono9pt7b);
  tft->setTextSize(1);
  tft->setCursor(161, 100);
  tft->print("1 Hour");
  tft->setCursor(165, 131);
  sprintf(buf, "%d%%", forecast.lines[1].rainProbablilty);
  tft->print(buf);
  tft->setCursor(165, 156);
  sprintf(buf, "%d-%d", forecast.lines[1].tempMin, forecast.lines[0].tempMax);
  tft->print(buf);
  tft->setCursor(165, 181);
  sprintf(buf, "%d", forecast.lines[1].windSpeed);
  tft->print(buf);
  tft->setCursor(165, 206);
  sprintf(buf, "%d%%", forecast.lines[1].humidity);
  tft->print(buf);

  // Today
  bmpDrawer.draw(iconsFilename[forecast.lines[2].icon], 252, 35);
  tft->setFont(&FreeMono9pt7b);
  tft->setTextSize(1);
  tft->setCursor(250, 100);
  tft->print("Today");
  tft->setCursor(248, 131);
  sprintf(buf, "%d%%", forecast.lines[2].rainProbablilty);
  tft->print(buf);
  tft->setCursor(248, 156);
  sprintf(buf, "%d-%d", forecast.lines[2].tempMin, forecast.lines[0].tempMax);
  tft->print(buf);
  tft->setCursor(248, 181);
  sprintf(buf, "%d", forecast.lines[2].windSpeed);
  tft->print(buf);
  tft->setCursor(248, 206);
  sprintf(buf, "%d%%", forecast.lines[2].humidity);
  tft->print(buf);
}

void drawReminderOnScreen(Adafruit_ILI9341* tft, BitmapDrawer bmpDrawer, const vector<Reminder>& reminder,
                          int actualReminder){
  // Prepare screen
  tft->fillScreen(ILI9341_WHITE);

  // Reminder Icon
  if(reminder[actualReminder].hookNumber == 0)
    bmpDrawer.draw("reminder.bmp", 20, 10);
  else
    bmpDrawer.draw("umbrella.bmp", 20, 10);
    
  // Reminder person name
  tft->drawLine(70, 67, 260, 67, ILI9341_GREEN);
  tft->setFont(&FreeSansBold12pt7b);  
  tft->setTextColor(ILI9341_BLACK);  
  tft->setTextSize(1);
  tft->setCursor(70, 65);
  tft->print(reminder[actualReminder].recipientName);

  // Reminder text (splitted in 20 chars rows)
  tft->setFont(&FreeMono9pt7b);
  tft->setTextSize(1);
  drawMultilineText(tft, reminder[actualReminder].description, 23, 18, 90, 21);

  // Arrows
  if( actualReminder > 0 )
    tft->fillTriangle( 290, 24,  314, 24,  302, 6, ILI9341_RED);
  else
    tft->drawTriangle( 290, 24,  314, 24,  302, 6, tft->color565(200, 200, 200));
    
  if( actualReminder < reminder.size() - 1)
    tft->fillTriangle( 290, 214, 314, 214, 302, 232, ILI9341_RED);
  else
    tft->drawTriangle( 290, 214, 314, 214, 302, 232, tft->color565(200, 200, 200));

  // Ok button
  tft->fillCircle( 302, 120, 12, tft->color565(100, 200, 100));
  tft->setFont();
  tft->setTextSize(1);
  tft->setCursor(297, 117);
  tft->setTextColor(ILI9341_WHITE); 
  tft->print("OK");
}

void drawMultilineText(Adafruit_ILI9341* tft, const char* text, int rowLen, int x, int y, int rowSpace){
  char buf[rowLen+1];
  int curRow = 0;
  int dscLen = strlen(text);
  int lastSpace = -1, lastSpaceInRow = 0;
  int i = 0, j = 0;
  while( i < dscLen ){
    /*DEBUG("BEGIN: multiline text");
    DEBUG(text[i]);
    DEBUG(" ");
    DEBUG(i);
    DEBUG(" ");
    DEBUG(lastSpace);
    DEBUG("END: multiline text");*/

    if(text[i] == ' '){
      lastSpace = i;
      lastSpaceInRow = j;
    }
      
    buf[j++] = text[i++];
    
    if(j >= rowLen){
      if(text[i] == ' '){
        lastSpace = i;
        lastSpaceInRow = 0;
        i++;
        buf[j] = 0;
      }else if(lastSpace == i-1){
        lastSpace = i-1;
        lastSpaceInRow = 0;
        buf[j] = 0;
      }else{
        if(lastSpaceInRow == 0)
          return; // Can't print this
        buf[lastSpaceInRow] = 0;
        i = lastSpace + 1;
        lastSpaceInRow = 0;
      }
      
      tft->setCursor(x, y + curRow * rowSpace);
      tft->print(buf);
      j = 0;
      curRow++;
    }
  }
  if(j > 0){
    buf[j] = 0;
    tft->setCursor(x, y + curRow * rowSpace);
    tft->print(buf);
  }
}

void doubleBeep(int piezoPin){
  analogWrite(piezoPin, 200);
  delay(300);
  analogWrite(piezoPin, 0);
  delay(100);
  analogWrite(piezoPin, 200);
  delay(300);
  analogWrite(piezoPin, 0);
}

void lowSingleBeep(int piezoPin){
  analogWrite(piezoPin, 80);
  delay(300);
  analogWrite(piezoPin, 0);
}

void sprintUUID(char* buf, const uint8_t* uuid){
  sprintf(buf, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
          uuid[0], uuid[1], uuid[2], uuid[3],
          uuid[4], uuid[5],
          uuid[6], uuid[7],
          uuid[8], uuid[9],
          uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
}

void getDemoForecast(Forecast& forecast){
  sprintf(forecast.locationName, "%s", "Lugano");
  sprintf(forecast.currentDate, "%s", "Thu 2 Jun");
  sprintf(forecast.currentTime, "%s", "15:50");
  forecast.lines[0].icon = 1;
  forecast.lines[0].rainProbablilty = 100;
  forecast.lines[0].tempMin = 17;
  forecast.lines[0].tempMax = 18;
  forecast.lines[0].windSpeed = 0;
  forecast.lines[0].humidity = 30;
  forecast.lines[1].icon = 1;
  forecast.lines[1].rainProbablilty = 100;
  forecast.lines[1].tempMin = 17;
  forecast.lines[1].tempMax = 18;
  forecast.lines[1].windSpeed = 0;
  forecast.lines[1].humidity = 40;
  forecast.lines[2].icon = 1;
  forecast.lines[2].rainProbablilty = 100;
  forecast.lines[2].tempMin = 14;
  forecast.lines[2].tempMax = 15;
  forecast.lines[2].windSpeed = 0;
  forecast.lines[2].humidity = 50;
}

void getDemoReminders(vector<Reminder>& reminders, const UUID& uuid){
  Reminder rem;
  
  static uint8_t greyBeaconUUID[] = {0xB9, 0x40, 0x7F, 0x30, 0xF5, 0xF8, 0x46, 0x6E,
                                     0xAF, 0xF9, 0x25, 0x55, 0x6B, 0x57, 0xFE, 0x6A};
  static uint8_t blueBeaconUUID[] = {0xB9, 0x40, 0x7F, 0x30, 0xF5, 0xF8, 0x46, 0x6E, 
                                     0xAF, 0xF9, 0x25, 0x55, 0x6B, 0x57, 0xFE, 0x6D};

  if( equal(greyBeaconUUID, greyBeaconUUID + 16, uuid.uuid) ){
    rem.uuid = uuid;
    sprintf(rem.recipientName, "%s", "Giuseppe");
    sprintf(rem.description, "%s", "It's gonna rain. Don't forget your umbrella!");
    rem.hookNumber = 1;
    reminders.push_back(rem);

    rem.uuid = uuid;
    sprintf(rem.recipientName, "%s", "Giuseppe");
    sprintf(rem.description, "%s", "Take your lunch.");
    rem.hookNumber = 0;
    reminders.push_back(rem);
  }else if( equal(blueBeaconUUID, blueBeaconUUID + 16, uuid.uuid) ){
    rem.uuid = uuid;
    sprintf(rem.recipientName, "%s", "Giacomo");
    sprintf(rem.description, "%s", "Buy the pizza!");
    rem.hookNumber = 0;
    reminders.push_back(rem);
    
    rem.uuid = uuid;
    sprintf(rem.recipientName, "%s", "Giacomo");
    sprintf(rem.description, "%s", "It's gonna rain. Take your umbrella!");
    rem.hookNumber = 2;
    reminders.push_back(rem);
  }
}

