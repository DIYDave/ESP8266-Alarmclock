#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>          // Manage auto connect to WiFi an fall back to AP   //https://github.com/tzapu/WiFiManager
#include <ArduinoOTA.h>           // Library for uploading sketch over the air (OTA)
#include <EEPROM.h> 
#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"
#include <TimeLib.h>
#include <DS3232RTC.h>        //http://github.com/JChristensen/DS3232RTC
#include <Wire.h>             //http://arduino.cc/en/Reference/Wire
#include <ArduinoJson.h>          // Library for parsing Json Strings
#include <Nextion.h>
#include <NextionPage.h>
#include <NextionText.h>
#include <NextionPicture.h>
#include <NextionNumber.h>
#include <NextionButton.h>
#include <NextionDualStateButton.h>
#include <NextionHotspot.h>

#define Addr 0x4A               // MAX44009 I2C address is 0x4A(74)

const char* Wochentag[] = {"Fehler","Sonntag", "Montag", "Dienstag","Mittwoch", "Donnerstag", "Freitag","Samstag" };
const char* Monate[] = {"Fehler","Januar", "Februar", "März","April", "Mai", "Juni", "juli", "August", "Septempber", "Oktober", "November","Dezember" };
static time_t tLast;
time_t t;
time_t webTime;
int moonPhase;
int WeatherIcon = 8;
int lastsecond, actPage;
boolean bAlarm;
boolean weatherOK, forecastOK, webTimeOK;     // 1 if answere from server OK
uint8_t  almStd[4], almMin[4], almStatus[4], almSound[4]; //almStatus: b0 = on/off; b1=Mo-Fr; b2=Sa; b3=So; b4=activ; b5=snooze; b6=Quitt; b7=; 

unsigned long lastNexPoll;                      // letzte Serverabfrage
unsigned long nexPolInterval =  250;

// JSON Variables
String JSONtext;                                  // String to hold answere from openweathermap
int endResponse         = 0;                      // Verwendung für parsen
boolean startJson       = false;                  // Verwendung für parsen

// OpenWeatherMap settings
const char weatherServer[30] = "api.openweathermap.org";    // Address for openweathermap 
String nameOfCity = "Pfäffikon,CH";  //7286788      // City to receive weather forecast from
String apiID = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";  // Personal API from openweathermap.org
String weatherType;                                 // Forecast data of weather
unsigned long lastWeatherTime = 0;                      // letzte Serverabfrage
unsigned long weatherInterval =  2500;              // Intervall für Wetter-Server abfrage (nur erste nach Kaltstart)
unsigned long lastForecastTime = 0;                      // letzte Serverabfrage
unsigned long forecastInterval = 2500;              // Intervall für Wetter-Server abfrage (nur erste nach Kaltstart)
float rain[4], wind[4], temp[4], humidity[4];             
time_t forecastTime[4];    
float tempAct, windAct;                             // Actual weather data
String weatherAct, descriptionAct, iconAct2;                  // Actual weather data                  
float totalRain;                                    // Sum of rain in the forecast
bool readForecast, readWeather, lastWasForecast;    // Just one at one time possible

// Timezonedb,com settings
const char timeServer[30] = "api.timezonedb.com";       // Address for timezonedb.org
String timeZone = "Europe/Zurich";                    // name of Timezone
String timeApiID = "XXXXXXXXXXXXXXXXXXXXXXXXXXX";       // Personal API from timezonedb.org
unsigned long lastWebTime;                            // letzte Serverabfrage
unsigned long webTimeInterval =  2500;                // Intervall for Time request (only after cold start)
int timeOffset;

char serverString[30];
bool clientInUse;                                 // verhindert gleichzeitige Anfragen an Server

void saveAlarms();
void refreshClockPage();
void displayAlarms();
void refreshForecastPage();                  // Function in "nextion.h"
void refreshSetupPage();

/// Create Instances ///////////
SoftwareSerial nextionSerial(D5, D6); // RX, TX
SoftwareSerial serialMP3(D7, D8); // RX, TX

DFRobotDFPlayerMini myDFPlayer;

Nextion nex(nextionSerial);
#include"nextion.h"         // all the components stuff and the handler

WiFiClient client;         // Client für Verbindung mit Wetterserver

void setup(void) /////////////////////////////////////////////////////////////////////////////////////////////////
{  /////////////////////////////// Setup /////////////////////////////////////////////////////////////////////////
  Serial.begin(57600);
  Serial.println();

  // Setup and start Nexton Display
  delay(300);
  nextionSerial.begin(57600);
  nex.init();

  // attach touch-widgets to handler (Handler in file "nextion.h")
  atachTouch();
  
  // synchronize Time library with the external RTC every five minutes by default.
  setSyncProvider(RTC.get);
  if (timeStatus() != timeSet) {
    Serial.println(" FAIL!");
  }

  // Setup and start I2c Bus for RTC and MAX44009
  Wire.begin(D2, D1); //sda,scl
  //Setup MAX44009 Light sensor
  Wire.beginTransmission(Addr);   // Select configuration register
  Wire.write(0x02);               // Continuous mode, Integration time = 800 ms
  Wire.write(0x40);
  Wire.endTransmission();         // Stop I2C transmission

  // Read from EEPROM
  EEPROM.begin(50);
  readFromFlash();

  // Setup and start MP3 Player
  serialMP3.begin(9600);
  if (!myDFPlayer.begin(serialMP3)) {  //Use softwareSerial to communicate with mp3.
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  myDFPlayer.volume(13);                //Set volume value. From 0 to 30
  //myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.EQ(DFPLAYER_EQ_BASS);

 // Setup an start WiFi
  WiFiManager wifiManager;
  IPAddress _ip = IPAddress(192, 168, 0, 8);
  IPAddress _gw = IPAddress(192, 168, 0, 1);
  IPAddress _sn = IPAddress(255, 255, 255, 0);
  
  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);
  //wifiManager.setAPCallback(configModeCallback);        // Call routine to show infos when in Config Mode
  //wifiManager.resetSettings(); // For test only

  if (!wifiManager.autoConnect("Wecker")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(2000);
    ESP.reset();
    delay(3000);
  }
  
  //if you get here you have connected to the WiFi
  ArduinoOTA.begin();
       
  pinMode(16, OUTPUT);
  delay(1000);
  digitalWrite(16, 1);

  nextionSerial.print("page 1");        // Show alarmscreen // pgAlarm.show(); dosn't work !
  printEnd();
  delay(200);
  displayAlarms();

}  ///////////////////// End Setup //////////////////////////////////////////////////////////////////////////


void loop(void) ////////////////////////////////////////////////////////////////////////////////////////////
{  /////////////////////////// Loop ////////////////////////////////////////////////////////////////////////
  ArduinoOTA.handle();
  nex.poll();   // Poll input handler for Nextion HMI
  t = now();    // Set to current time
  
  if (second(t) != lastsecond){ // && actPage == 0  // Time has change and Clock-Page active
    displaySecond();
    checkForAlarm(t);
    readLightSens();
    lastsecond =second(t);
    
    if (second(t) == 0) {
      float c = RTC.temperature() / 4.;
      float f = c * 9. / 5. + 32.;
    }   
  }

  if (minute(t) != minute(tLast)&& actPage == 0){   // Time has change and Clock-Page active) {
    tLast = t;
    refreshClockPage();
  }
  
  if ((millis() - lastForecastTime > forecastInterval) && !clientInUse) {
      clientInUse = 1; 
      weatherType = "forecast";
      forecastOK = 0;
      weatherRequest();                                      // Request forecast to openweathermap.com
      lastForecastTime = millis();
  }
  
  if ((millis() - lastWeatherTime > weatherInterval) && !clientInUse) {
    clientInUse = 1; 
    weatherType = "weather";
    weatherOK = 0;
    weatherRequest();                                      // Request forecast to openweathermap.com
    lastWeatherTime = millis();      
  }

  if ((millis() - lastWebTime > webTimeInterval)  && !clientInUse) {  
    clientInUse = 1; 
    webTimeOK = 0;
    timeRequest();                                      // Request forecast to openweathermap.com
    lastWebTime = millis(); 
  }
  
  while (client.available()) {                            // connectetd with Server     
    //String line = client.readStringUntil('\r'); Serial.print(line); 
    readJson (client.read());                          // Call function and give received char to it
  }  
}  //////////////////// End Loop /////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Online Functions ///////////////////////////////////////////////////////
void weatherRequest() {                                    // makes a HTTP request to OpenWeather.com
  client.stop();                                       // close any connection before send a new request.  This will free the socket on the WiFi
  if (client.connect(weatherServer, 80)) {             // if there's a successful connection:  
    client.println("GET /data/2.5/" +  weatherType + "?q=" + nameOfCity + "&APPID=" + apiID + "&mode=json&units=metric&cnt=4 HTTP/1.1");
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    //Serial.println("Server Connected");
  }
  else {
    // if you couldn't make a connection:
    Serial.println("Weather connection failed");
    weatherInterval =  5000; 
    forecastInterval = 7000;
  }
  clientInUse = 0;
}

void timeRequest() {                                    // makes a HTTP request to Timezonedb.com
  client.stop();                                       // close any connection before send a new request.  This will free the socket on the WiFi
  if (client.connect("api.timezonedb.com", 80)) {             // if there's a successful connection:  
    client.println("GET /v2/get-time-zone?key=" + timeApiID + "&format=json&by=countryName&by=zone&zone=" + timeZone + " HTTP/1.1");
    client.println("Host: api.timezonedb.com");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    //Serial.println("Server Connected");
  }
  else {
    // if you couldn't make a connection:
    Serial.println("Time connection failed");
    webTimeInterval = 10000;
  }
  clientInUse = 0;
}


void readJson (char c){                                 // Make String from JSON-Stream
  if (c == '{') {
    startJson = true;                                   // set startJson true to indicate json message has started
    endResponse++;                                      // For each { increment
  }
  if (c == '}') {
    endResponse--;                                      // For each } decrement
  }
  if (startJson == true) {
    JSONtext += c;
  }
  if (endResponse == 0 && startJson == true) {          // If equal { and } the json is complettly read
    JsonParse(JSONtext.c_str());                        // parse string "text" in parseJson function
    JSONtext = "";                                      // clear text string for the next time
    startJson = false;                                  // set startJson to false to indicate that a new message has not yet started
  }
}


void JsonParse(const char * jsonString){                    // Parsing the JSON String from Server
  //Serial.println(jsonString);
  DynamicJsonBuffer  jsonBuffer;                            // StaticJsonBuffer<3000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(jsonString);  
  if (!root.success()) {                                    // Test if parsing succeeds.
    Serial.println("parseObject() failed");
    return;
  }
  else{  
    String weatherAct = root["weather"][0]["main"];
    String weather = root["list"][0]["weather"][0]["main"];
    String swebTime = root["status"];
   
 // Parse actual Weather
    if (weatherAct.length() != 0){                                // Actual weather
      String descriptionAct = root["weather"][0]["description"];
      windAct = root["wind"]["speed"];
      tempAct = root["main"]["temp"];     
      String iconAct = root["weather"][0]["icon"];
      iconAct2 = iconAct;                                         // Pass to global label      
      weatherInterval = 60 * 1000 * 15;           // If Response OK, set intervall to 20Min 
      Serial.print("Weather OK");
      Serial.print(" millis: ");   
      Serial.println(millis()); 
      weatherOK =1;
      displayWeather();
    }
    
 // Parse Weather forecast
    else if  (weather.length() != 0){                             // Weather forecast
      totalRain = 0;
      for (int i=0; i <= 3; i++){
        rain[i] = 0; 
        String weather = root["list"][i]["weather"][0]["main"];
        if (weather = "Rain") {
          rain[i] = root["list"][i]["rain"]["3h"];
          totalRain += rain[i];       
        } 
      }
      for (int i=0; i <= 3; i++){
        temp[i] = root["list"][i]["main"]["temp"]; 
        wind[i] = root["list"][i]["wind"]["speed"]; 
        humidity[i] = root["list"][i]["main"]["humidity"]; 
        forecastTime[i] = 0;
        forecastTime[i] = root["list"][i]["dt"];   
        forecastTime[i] += timeOffset;    
      }
        forecastOK = 1;
        forecastInterval = 60 * 1000 * 17;          // If Response OK, set intervall to 20Min
        displayWeather();
        Serial.print("Forecast OK  ");  
        Serial.print(" millis: ");   
        Serial.println(millis()); 
    }

 // Parse Time
    else if (swebTime = "OK") {                                   // Web Time
      webTime = root["timestamp"];                                //  Time in correct timezone
      timeOffset = root["gmtOffset"];                             // Time zone offset from GMT used for weather forecast times
      if (webTime > 1494800000){                                  // Time is plausible
        RTC.set(webTime);                                         // Set RTC to Webtime
        setSyncProvider(RTC.get);                                 // Set ESP time to RTC time
        webTimeInterval = 1000 * 60 * 65;                         // Set updatetime to 70 Min.
        webTimeOK = 1;                                            // 
        displayTime();
        Serial.print("WebTime OK ");
        Serial.print(" millis: ");   
        Serial.println(millis());  
      }
    }
  }       
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// End Of Online Functions ///////////////////////////////////////////////////////


void readLightSens(){
  unsigned int data[2];
  Wire.beginTransmission(Addr);   // Start I2C Transmission
  Wire.write(0x03);               // Select data register
  Wire.endTransmission();         // Stop I2C transmission
  Wire.requestFrom(Addr, 2);      // Request 2 bytes of data
  if (Wire.available() == 2) {        // Read 2 bytes of data (luminance msb, luminance lsb)
    data[0] = Wire.read();
    data[1] = Wire.read();
  }
  // Convert the data to lux
  int exponent = (data[0] & 0xF0) >> 4;
  int mantissa = ((data[0] & 0x0F) << 4) | (data[1] & 0x0F);
  float luminance = pow(2, exponent) * mantissa * 0.045;
  LongToChar(luminance,1,2);
  int dim = (16.8 * log(luminance))-3.112;  // calculate logaritmic value for LCD dim.
  if (luminance < 1.5) dim = 1;
  else if (luminance > 240) dim = 100;
  nextionSerial.print("dim=");      // Show alarmscreen // pgAlarm.show(); dosn't work !
  nextionSerial.print(dim);
  printEnd(); 
}

void LongToChar(float floatVal,int select, int decimal){      //I have copy this cool function from a forum and modify a bit. Unfortunately I do not know where. Many Thanks
  String stringVal = ""; 
  String sDecPart = ""; 

  int intPart = int(floatVal);
  long decPart = pow(10,decimal) *(floatVal-intPart);              //I am multiplying by 100 assuming that the foat values will have a maximum of 2 decimal places. 
                                                      //Change to match the number of decimal places you need
  if(decPart>0)sDecPart = String(decPart);            //return the decimal part of float number if it is available 
  else if(decPart<0)sDecPart = String((-1)*decPart);  //if negative, multiply by -1
  else if(decPart==0)sDecPart = String("0");             //return 0 if decimal part of float number is not available
     
  stringVal+=String(intPart)+ "." + sDecPart;         //combining both whole and decimal part in string with a fullstop between them 
  char charVal[stringVal.length()+1];                   //initialise character array to store the values
  stringVal.toCharArray(charVal,stringVal.length()+1);  //passing the value of the string to the character array
  if (select ==1) txtLux.setText(charVal); //display character array
  if (select ==2) txtTemp.setText(charVal);
  if (select ==3) txtWind.setText(charVal);
}


void checkForAlarm(time_t t){ 
  byte Alarmdays;
  if(weekday(t)==1) Alarmdays = B00001000;          // Alarm on and su
  else if(weekday(t)==7) Alarmdays = B00000100;          // Alarm on and Sa
  else if(weekday(t)>1 && weekday(t)<7) Alarmdays = B00000010;          // Alarm on and Mo-Fr
  
//almStatus: b7=; b6=Quitt; b5=snooze; b4=activ; b3=So; b2=Sa; b1=Mo-Fr; b0=on/off;  
  for (byte i=0; i<=3; i++){
    if (hour(t) * 60 + minute(t) == almStd[i] * 60 + almMin[i]){
      if ((almStatus[i] & (Alarmdays + B01000001)) == Alarmdays + 1){   // right day + not Quitt and alarm on
        bitWrite(almStatus[i],4,1);             // Set alarm to active
        bAlarm = 1;                             // Global Alarm On     
        if (!pgAlarm.isShown()){                // If alarmscreen not shown
          nextionSerial.print("page 3");        // Show alarmscreen // pgAlarm.show(); dosn't work !
          printEnd();
          delay(10);
          //myDFPlayer.enableLoopAll(); 
          //myDFPlayer.loop(1);
          myDFPlayer.volume(10); 
          myDFPlayer.randomAll();
          //myDFPlayer.play(2);                   //Play mp3
        }    
      }
      else{
        bitWrite(almStatus[i],4,0);           // Reset alarm active
        bAlarm = 0;                           // Global Alarm On
        if ((almStatus[i] & (Alarmdays + B00000001)) != Alarmdays + 1){ // not the right day + Not Alarm on
          bitWrite(almStatus[i],6,0);         // Reset Quitt     
        }
      }
    }
    else{
      if (hour(t) * 60 + minute(t) != almStd[i] * 60 + almMin[i]){  
        bitWrite(almStatus[i],6,0);             // Reset Quitt     
      }
    }
  }  
}

void refreshClockPage(){
  displayTime();
  displayDate();
  displayMoon();
  displayWeather();
}
  
void displaySecond(){
  char cTime[3];
  leading0(second(t)).toCharArray(cTime, 3);      // Add leading 0 and convert Int to Char Array
  txtSek.setText(cTime);  
  memset(cTime, 0, sizeof cTime);             // Clear Char Array    
}

void displayTime(){
  char cTime[3]; 
  leading0(hour(t)).toCharArray(cTime, 3);        // Add leading 0 and convert Int to Char Array
  txtStd.setText(cTime);                        // Write to HMI
  leading0(minute(t)).toCharArray(cTime, 3); 
  txtMin.setText(cTime);  
  leading0(second(t)).toCharArray(cTime, 3); 
  txtSek.setText(cTime);  
  memset(cTime, 0, sizeof cTime);             // Clear Char Array 
  if (webTimeOK){
    nextionSerial.print("ptOK.pic=21"); printEnd();
  }
  else{
    nextionSerial.print("ptOK.pic=20"); printEnd();
  }
}


void displayDate(){
  char cDate[30];   
  sprintf(cDate,"%s %02d.%s %04d ",  Wochentag[weekday(t)], day(t),Monate[month(t)], year(t));
  txtDate.setText(cDate);
  memset(cDate, 0, sizeof cDate); // Clear Char Array 
}


void displayMoon(){ 
  moonPhase = moon_phase(year(t), month(t), day(t)); 
  if (moonPhase <=7 && moonPhase >=0){
    nextionSerial.print("pMoon.pic=");
    nextionSerial.print(moonPhase); // Call function, return int 0..7
     printEnd();
  }
}

void displayWeather(){ 
  //Serial.println("disp weather ");
  //Serial.println(iconAct2);
   if (iconAct2 == "01d" || iconAct2 == "01n")  WeatherIcon = 8;
   if (iconAct2 == "02d" || iconAct2 == "02n")  WeatherIcon = 9;
   if (iconAct2 == "03d" || iconAct2 == "03n")  WeatherIcon = 10;
   if (iconAct2 == "04d" || iconAct2 == "04n")  WeatherIcon = 11;
   if (iconAct2 == "09d" || iconAct2 == "09n")  WeatherIcon = 12;
   if (iconAct2 == "10d" || iconAct2 == "10n")  WeatherIcon = 13;
   if (iconAct2 == "11d" || iconAct2 == "11n")  WeatherIcon = 14;
   if (iconAct2 == "13d" || iconAct2 == "13n")  WeatherIcon = 15;  
   //  Serial.println(WeatherIcon);
  nextionSerial.print("pWeather.pic=");
  nextionSerial.print(WeatherIcon);
  printEnd();
  LongToChar(tempAct,2,1);
  LongToChar(windAct*3.6,3,1);
    if (weatherOK && forecastOK){
    nextionSerial.print("pwOK.pic=23"); printEnd();
  }
  else{
    nextionSerial.print("pwOK.pic=22"); printEnd();
  }
}


String leading0(int val){       // Add leading zero if val < 10
  String sTemp ="";
  if (val < 10){
     sTemp= "0";
  }
    sTemp += val;
    return sTemp;
}
  

int moon_phase(int y, int m, int d){
  // calculates the age of the moon phase(0 to 7)
  // there are eight stages, 0 is full moon and 4 is a new moon
  double jd = 0; // Julian Date
  double ed = 0; //days elapsed since start of full moon
  int b= 0;
  jd = julianDate(y,m,d);
  //jd = julianDate(1972,1,1); // used to debug this is a new moon
  jd = int(jd - 2244116.75); // start at Jan 1 1972
  jd /= 29.53; // divide by the moon cycle    
  b = jd;
  jd -= b; // leaves the fractional part of jd
  ed = jd * 29.53; // days elapsed this month
  String nfm = String((int(29.53 - ed))); // days to next full moon
  //Serial.println(nfm);
  b = jd*8 +0.5;
  b = b & 7; 
  return b;
   
}
double julianDate(int y, int m, int d){
// convert a date to a Julian Date}
  int mm,yy;
  double k1, k2, k3;
  double j;
  
  yy = y- int((12-m)/10);
  mm = m+9;
  if(mm >= 12) {
    mm = mm-12;
  }
  k1 = 365.25 *(yy +4172);
  k2 = int((30.6001 * mm) + 0.5);
  k3 = int((((yy/100) + 4) * 0.75) -38);
  j = k1 +k2 + d + 59;
  j = j-k3; // j is the Julian date at 12h UT (Universal Time)
  return j;
}

void printEnd(){                // Print Endchar to Nextion
  nextionSerial.write(0xff);
  nextionSerial.write(0xff);
  nextionSerial.write(0xff);
}

void displayAlarms(){                       //Set all components on alarm page
  nW1Std.setValue(almStd[0]);
  nW1Min.setValue(almMin[0]);
  nW2Std.setValue(almStd[1]);
  nW2Min.setValue(almMin[1]);
  nW3Std.setValue(almStd[2]);
  nW3Min.setValue(almMin[2]);
  nW4Std.setValue(almStd[3]);
  nW4Min.setValue(almMin[3]);
  btW1On.setActive(bitRead(almStatus[0],0));
  btW1Wd.setActive(bitRead(almStatus[0],1));
  btW1Sa.setActive(bitRead(almStatus[0],2));
  btW1Su.setActive(bitRead(almStatus[0],3));
  btW2On.setActive(bitRead(almStatus[1],0));
  btW2Wd.setActive(bitRead(almStatus[1],1));
  btW2Sa.setActive(bitRead(almStatus[1],2));
  btW2Su.setActive(bitRead(almStatus[1],3));
  btW3On.setActive(bitRead(almStatus[2],0));
  btW3Wd.setActive(bitRead(almStatus[2],1));
  btW3Sa.setActive(bitRead(almStatus[2],2));
  btW3Su.setActive(bitRead(almStatus[2],3));
  btW4On.setActive(bitRead(almStatus[3],0));
  btW4Wd.setActive(bitRead(almStatus[3],1));
  btW4Sa.setActive(bitRead(almStatus[3],2));
  btW4Su.setActive(bitRead(almStatus[3],3));
}

void saveAlarms(){
  for (byte i=0; i<=1; i++){  // Because I had some problem with communication, I run this twice.
    almStd[0] = nW1Std.getValue();
    almMin[0] = nW1Min.getValue();
    almStd[1] = nW2Std.getValue();
    almMin[1] = nW2Min.getValue();
    almStd[2] = nW3Std.getValue();
    almMin[2] = nW3Min.getValue();
    delay(20);                // Because I had some problem with communication, I add this delay.
    almStd[3] = nW4Std.getValue();
    almMin[3] = nW4Min.getValue();
    bitWrite(almStatus[0],0,btW1On.isActive());
    bitWrite(almStatus[0],1,btW1Wd.isActive());
    bitWrite(almStatus[0],2,btW1Sa.isActive());
    bitWrite(almStatus[0],3,btW1Su.isActive());
    bitWrite(almStatus[1],0,btW2On.isActive());
    delay(20);
    bitWrite(almStatus[1],1,btW2Wd.isActive());
    bitWrite(almStatus[1],2,btW2Sa.isActive());
    bitWrite(almStatus[1],3,btW2Su.isActive());
    bitWrite(almStatus[2],0,btW3On.isActive());
    bitWrite(almStatus[2],1,btW3Wd.isActive());
    delay(20);
    bitWrite(almStatus[2],2,btW3Sa.isActive());
    bitWrite(almStatus[2],3,btW3Su.isActive());
    bitWrite(almStatus[3],0,btW4On.isActive());
    bitWrite(almStatus[3],1,btW4Wd.isActive());
    bitWrite(almStatus[3],2,btW4Sa.isActive());
    bitWrite(almStatus[3],3,btW4Su.isActive());
    delay(20);
  }
  saveToFlash();  
  btnSave.setText("Done");
  btnSave.setForegroundColour(NEX_COL_BLACK);
  btnSave.setBackgroundColour(NEX_COL_GREEN);
}

void saveToFlash(){
  byte e = 0;
  for (byte i = 0; i <= 3; i++){  
    EEPROM.write(e++, almStd[i]); 
    EEPROM.write(e++, almMin[i]); 
    EEPROM.write(e++, almStatus[i]); 
    EEPROM.write(e++, almSound[i]);    
  }
  EEPROM.commit(); 
}

void readFromFlash(){
  byte e = 0;
  for (byte i = 0; i <= 3; i++){
    almStd[i] = EEPROM.read(e++);   
    almMin[i] = EEPROM.read(e++); 
    almStatus[i] = EEPROM.read(e++);   
    almSound[i] = EEPROM.read(e++);  
  }
}
