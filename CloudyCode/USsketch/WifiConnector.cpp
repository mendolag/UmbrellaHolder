/* 
 *  Implementation of WifiConnector.h
*/

#include "WifiConnector.h"
#include <ArduinoJson.h>

WifiConnector::WifiConnector(HardwareSerial& espSerial)
  : espSerial(espSerial) {
}

void WifiConnector::connect(const char* ssid, const char* password)
{
   String connectCmd = String("CONNECT|") + ssid + "|" + password;
   writeSerialLine(connectCmd.c_str());
   delay(100);
   DEBUG(connectCmd);
}

void WifiConnector::getForecast(Forecast& forecast)
{
  String forecastCmd = String(HOST) + "|" + PORT + "|" + FORECAST_URL;
  DEBUG(forecastCmd);
  httpGetRequest(forecastCmd.c_str());
  DEBUG("print buffer");
  DEBUG(commBuffer);
  StaticJsonBuffer<SERIAL_BUFFER_LENGHT> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(commBuffer);
// Test if parsing succeeds.
  if (!root.success()) {
     Serial.println("parseObject() failed");
  return;
  }

  // Forecast relative to a specific period.
  char t[6];
  char d[11];
  sprintf(forecast.locationName, "%s", "Lugano");
  String dtemp=root["date"];
  dtemp.toCharArray(d,11);
  sprintf(forecast.currentDate, "%s", d);
  String ttemp=root["time"];
  ttemp.toCharArray(t,6);
  sprintf(forecast.currentTime, "%s", t);
  forecast.lines[0].icon = getIcon(root["current"]["icon"]);
  forecast.lines[0].rainProbablilty = root["current"]["precProb"];
  forecast.lines[0].tempMin = root["current"]["temp"];
  forecast.lines[0].tempMax = root["current"]["temp"];
  forecast.lines[0].windSpeed = root["current"]["windSpeed"];
  forecast.lines[0].humidity = root["current"]["humidity"];
  forecast.lines[1].icon = getIcon(root["nextHour"]["icon"]);
  forecast.lines[1].rainProbablilty = root["nextHour"]["precProb"];
  forecast.lines[1].tempMin = root["nextHour"]["temp"];
  forecast.lines[1].tempMax = root["nextHour"]["temp"];
  forecast.lines[1].windSpeed = root["nextHour"]["windSpeed"];
  forecast.lines[1].humidity = root["nextHour"]["humidity"];
  forecast.lines[2].icon = getIcon(root["today"]["icon"]);
  forecast.lines[2].rainProbablilty = root["today"]["precProb"];
  forecast.lines[2].tempMin = root["today"]["min"];
  forecast.lines[2].tempMax = root["today"]["max"];
  forecast.lines[2].windSpeed = root["today"]["windSpeed"];
  forecast.lines[2].humidity = root["today"]["humidity"];
};
  
int WifiConnector::getIcon(const char* icon){
  //  "sunny.bmp",    // 0
  //  "rain.bmp",     // 1
  //  "cloudy.bmp",   // 2
  //  "foggy.bmp",    // 3
  //  "msunny.bmp",   // 4
  //  "pcloudy.bmp",  // 5
  //  "psunny.bmp",   // 6
  //  "rshowers.bmp", // 7
  //  "sleet.bmp",    // 8
  //  "snow.bmp",     // 9
  //  "tstorms.bmp",  // 10
  //  "unknown.bmp",  // 11
  //  "clear.bmp"     // 12
  int n=0;
  if(strcmp(icon,"clear-day")==0){n=0;}
  else if(strcmp(icon,"clear-night")==0){n=12;}
  else if(strcmp(icon,"rain")==0){n=1;}
  else if(strcmp(icon,"cloudy")==0){n=2;}
  else if(strcmp(icon,"fog")==0){n=3;}
  else if(strcmp(icon,"wind")==0){n=4;}
  else if(strcmp(icon,"partly-cloudy-night")==0){n=5;}
  else if(strcmp(icon,"partly-cloudy-day")==0){n=6;}
  else if(strcmp(icon,"showers")==0){n=7;}
  else if(strcmp(icon,"sleet")==0){n=8;}
  else if(strcmp(icon,"snow")==0){n=9;}
  else if(strcmp(icon,"thundestorm")==0){n=10;}
  else{n=11;}
  
  return n;
}

void WifiConnector::getReminders(vector<Reminder>& reminders, const UUID& uuid)
{
  char uuidbuf[40]="B9407F30-F5F8-466E-AFF9-25556B57FE6D";
  //sprintUUID(uuidbuf, uuid.uuid);
  String forecastCmd = String(HOST) + "|" + PORT + "|" + REMINDER_URL + "/" + uuidbuf;
  httpGetRequest(forecastCmd.c_str());
  DEBUG( "WifiConnector::getReminders: json object received is" );
  DEBUG( commBuffer );
  
  // Parse JSON and populate reminder
  StaticJsonBuffer<SERIAL_BUFFER_LENGHT> jsonBuffer;
  JsonArray& root = jsonBuffer.parseArray(commBuffer);
  if( !root.success() ){
    DEBUG("WifiConnector::getReminders: parseObject() failed");
    return;
  }

  DEBUG("WifiConnector::getReminders: number of reminders returned");
  DEBUG(root.size());
  char userA[19];
  for( int i=0; i<=root.size(); i++ ){    
    Reminder rem;
    rem.uuid=uuid;
  
    // String utemp=root[i]["username"];
    //  utemp.toCharArray(rem.recipientName,19);
    //  DEBUG(utemp);
      //sprintf(rem.recipientName, "%s", userA);
    //strncpy(rem.recipientName,utemp, 19);
    //rem.recipientName[19] = 0;
    DEBUG((const char*)root[i]["username"]);
    DEBUG((const char*)root[i]["description"]);
    strcpy(rem.recipientName, static_cast<const char*>(root[i]["username"]));
    rem.recipientName[19] = 0;
    strcpy(rem.description, static_cast<const char*>(root[i]["description"]));
    rem.description[99] = 0;
    rem.hookNumber = root[i]["hook"];;
    reminders.push_back(rem);
  }
}

void WifiConnector::httpGetRequest(const char* request)
{
  DEBUG( "WifiConnector::httpGetRequest: Send request");
  DEBUG( request );
  // Make call
  writeSerialLine(request);
  delay(10);

  DEBUG("WifiConnector::httpGetRequest: Wait for serial");

  // Wait for the response
  while( !espSerial.available() )
    delay(100);

  DEBUG("WifiConnector::httpGetRequest: Serial available");

  // Read response
  readSerialLine();
}



void WifiConnector::writeSerialLine(const char* buf)
{
  espSerial.print(buf);
  espSerial.print(LINE_TERMINATOR);
}

int WifiConnector::readSerialLine()
{
  int index = 0;
  String s1="";
  int counter=0;
  commBuffer[index] = '\0';
  while( espSerial.available()  ){
   while(!(s1.endsWith("}}")||s1.endsWith("}]")||counter>=10)){
   DEBUG("s1read");
   delay(10);
   counter++;
   s1 = espSerial.readStringUntil('\n');
   DEBUG(s1);
    }
    }
   
   if(s1.length()!=0){
    s1.toCharArray(commBuffer, 2048);
   }
  return index;
}
