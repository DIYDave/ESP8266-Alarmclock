/*
void JsonParseWeather(const char * jsonString){                // Parsing the JSON String from Server
      Serial.println(jsonString);
  DynamicJsonBuffer  jsonBuffer;                        // StaticJsonBuffer<3000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(jsonString);  
  if (!root.success()) {                                // Test if parsing succeeds.
    Serial.println("parseObject() failed");
    return;
  }
  else{  
      String weatherAct = root["weather"][0]["main"];
      String weather = root["list"][0]["weather"][0]["main"];
    if (weatherAct.length() != 0){
      String descriptionAct = root["weather"][0]["description"];
      windAct = root["wind"]["speed"];
      tempAct = root["main"]["temp"];
      String iconAct = root["weather"][0]["icon"];
      if(debug)Serial.print("Weather Main: ");       
      if(debug)Serial.println(weatherAct);  
      if(debug)Serial.print("Weather Desc: ");       
      if(debug)Serial.println(descriptionAct);  
      if(debug)Serial.print("Icon: ");       
      if(debug)Serial.println(iconAct);  
      if(debug)Serial.print("Wind: ");       
      if(debug)Serial.println(windAct);  
      if(debug)Serial.print("Temp: ");       
      if(debug)Serial.println(tempAct); 
      if(debug)Serial.println();   
      lastWasForecast =0;   
      if (weatherOK == 1) weatherInterval = 60 * 1000 * 10;   // If Response OK, set intervall to 30Min 
      weatherOK = 0;
    }
    else if  (weather.length() != 0){
      totalRain = 0;
       if(debug)Serial.print("Rain: ");
    for (int i=0; i <= 3; i++){
      String weather = root["list"][i]["weather"][0]["main"];
      if (weather = "Rain") {
        rain[i] = root["list"][i]["rain"]["3h"];
        totalRain += rain[i];
        if(debug)Serial.print(rain[i]);  
        if(debug)Serial.print(", ");           
      }      
    }
        if(debug)Serial.println();
        if(debug)Serial.print("Wind: ");
    for (int i=0; i <= 3; i++){
        wind[i] = root["list"][i]["wind"]["speed"];
        if(debug)Serial.print(wind[i]); 
        if(debug)Serial.print(", ");             
    }
        if(debug)Serial.println();
        if(debug)Serial.print("Temp: ");
    for (int i=0; i <= 3; i++){
        temp[i] = root["list"][i]["main"]["temp"];
        if(debug)Serial.print(temp[i]); 
        if(debug)Serial.print(", ");        
    }
    lastWasForecast = 1;
    if(debug)Serial.println();   
    weatherOK = 1;                         
    }
  }      
}



void  printWiFidetails(){
  Serial.println();
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("signal strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.println();
  Serial.println();
}



//  char cDate[50];
//  sprintf(cDate,"%02d.%02d.%04d  %02d:%02d:%02d", day(webTime), month(webTime), year(webTime), hour(webTime), minute(webTime), second(webTime) );
//  Serial.println(cDate);


// Print all alarm status
  for (byte i=0; i<=3; i++){
    char cDate[50];   
    sprintf(cDate,"Wecker: %d  Aktiv: %d  Snooz: %d  Quitt: %d",  i, bitRead(almStatus[i],4),bitRead(almStatus[i],5), bitRead(almStatus[i],6)  );
    Serial.println(cDate);
    memset(cDate, 0, sizeof cDate); // Clear Char Array 
  }


*/


