/*
 *  This sketch makes an HTTP GET request to an host.
 *
 *  The instruction on how to setup the ESP8266 are here:
 *    http://www.allaboutcircuits.com/projects/breadboard-and-program-an-esp-01-circuit-with-the-arduino-ide/
 *  
 *  JSON library is here:
 *    https://github.com/bblanchon/ArduinoJson
 *    
 *    {"current":{"summary":"Clear","icon":"clear-day","date":"22.05.2016","time":"18:37","temp":18.7,"windSpeed":2.31,"humidity":74,"precProb":1},"nextHour":{"summary":"Clear","icon":"clear-day","date":"22.05.2016","time":"20:00","temp":17.29,"windSpeed":1.77,"humidity":82,"precProb":2},"today":{"summary":"Rain starting tonight.","icon":"rain","min":9.73,"max":19.36,"from":{"year":2016,"month":"05","day":"22","hour":18,"min":"00"},"to":{"year":2016,"month":"05","day":"23","hour":0,"min":"00"},"precProb":1,"WindSpeed":1.7316666666666667,"Humidity":95}}
 *    
 *    CONNECT|arduino|wifimodule123
 *    CONNECT|mob-67016|4b4y-g9bz-diye-xt7t
 *    
 *    192.168.43.251|3000|/todayForecast
 *    94.198.253.25|3000|/todayForecast
 *    94.198.253.25|3000|/reminder/1
 *    
 *  Forecast I/O
 *    
 *  Author: Giacomo Del Rio
 */

#include <ESP8266WiFi.h>

#define LINE_TERMINATOR '\n'


void connectToWiFi(char* connectCommand){
  char* command  = strtok(connectCommand, "|");
  char* ssid = strtok(NULL, "|");
  char* password = strtok(NULL, "|");

  //printSerialLine("Connecting...");
  //printSerialLine(command);
  //printSerialLine(ssid);
  //printSerialLine(password);

  if( strcmp(command, "CONNECT") != 0 )
    return;  // Not the connect command
  
  WiFi.begin(ssid, password);
  while( WiFi.status() != WL_CONNECTED ){
    delay(500);
    Serial.write("Not connected");
  }
  //Serial.println("Connected. IP address: ");  
  //Serial.println(WiFi.localIP());
}

int readSerialLine(char* buf, int buflen) {
  int index = 0;
 
  while( Serial.available() > 0 ){
    char rc = Serial.read();

    if( rc != LINE_TERMINATOR ){
      buf[index] = rc;
      index++;
      if( index >= buflen ){
        index = buflen - 1;
      }
    }else{
      buf[index] = '\0';
    }
  }
}

void printSerialLine(const char* buf) {
  Serial.print(buf);
  Serial.print(LINE_TERMINATOR);
}

String httpGet(char* host, int port, const char* url){
  // Connect to client
  WiFiClient client;
  if( !client.connect(host,port) ){
    return String();  // Error connecting
  }
  //Serial.println("Connesso");

  // Send request
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  //Serial.println("Richiesta inviata");

  // Wait response
  unsigned long timeout = millis();
  while( client.available() == 0 ){
    if( millis() - timeout > 5000 ){
      client.stop();
      return String();  // Timeout
    }
  }
  //Serial.println("Dati pronti");
  
  // Read status
  String buffer = client.readStringUntil('\r');
  if( strstr(buffer.c_str(), "200") == NULL )
    return String();  // Error

  // Skip header response
  buffer = client.readStringUntil('\r');
  while( buffer.length() > 1 ){
    buffer = client.readStringUntil('\r');
  }

  return (client.readStringUntil('\r')).substring(1);
}

String executeRequest(char* command){
  char* host = strtok(command, "|");
  int   port = atoi(strtok(NULL, "|"));
  char* url  = strtok(NULL, "|");
  return httpGet(host, port, url);
}

void waitSerial()
{
  while( !Serial.available() )
    delay(100);
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // Wait for the CONNECT command
  waitSerial();
  char buffer[2048];
  readSerialLine(buffer, 2048);
  connectToWiFi(buffer);
}

void loop(){
  char buffer[2048];
  waitSerial();
  readSerialLine(buffer, 2048);
  String result = executeRequest(buffer);
  printSerialLine(result.c_str());
}
