// Page 0 (Clock / Main)
  NextionText txtStd(nex, 0, 1, "tStd");
  NextionText txtMin(nex, 0, 6, "tMin");
  NextionText txtSek(nex, 0, 2, "tSek");
  NextionText txtDate(nex, 0, 7, "tDate");
  NextionText txtTemp(nex, 0, 8, "tActTemp");
  NextionText txtWind(nex, 0, 39, "tActWind");
  NextionHotspot touchAlarm(nex, 0, 36, "m0");
  NextionPage pgMain(nex, 0, 0, "page0");
  NextionButton btnForecast(nex, 0, 8, "b0");
  
// Page 1 (Alarm)
  NextionPage pgAlarmSetup(nex, 1, 0, "page1");
  NextionDualStateButton btW1On(nex, 1, 11, "bt0");
  NextionDualStateButton btW1Wd(nex, 1, 10, "bt1");
  NextionDualStateButton btW1Sa(nex, 1, 12, "bt2");
  NextionDualStateButton btW1Su(nex, 1, 13, "bt3");
  NextionDualStateButton btW2On(nex, 1, 17, "bt4");
  NextionDualStateButton btW2Wd(nex, 1, 18, "bt5");
  NextionDualStateButton btW2Sa(nex, 1, 19, "bt6");
  NextionDualStateButton btW2Su(nex, 1, 20, "bt7");
  NextionDualStateButton btW3On(nex, 1, 24, "bt8");
  NextionDualStateButton btW3Wd(nex, 1, 25, "bt9");
  NextionDualStateButton btW3Sa(nex, 1, 26, "bt10");
  NextionDualStateButton btW3Su(nex, 1, 27, "bt11");
  NextionDualStateButton btW4On(nex, 1, 31, "bt12");
  NextionDualStateButton btW4Wd(nex, 1, 32, "bt13");
  NextionDualStateButton btW4Sa(nex, 1, 33, "bt14");
  NextionDualStateButton btW4Su(nex, 1, 34, "bt15");
  NextionNumber nW1Std(nex, 1, 2, "nW1Std");
  NextionNumber nW1Min(nex, 1, 3, "nW1Min");
  NextionNumber nW2Std(nex, 1, 14, "nW2Std");
  NextionNumber nW2Min(nex, 1, 15, "nW2Min");
  NextionNumber nW3Std(nex, 1, 21, "nW3Std");
  NextionNumber nW3Min(nex, 1, 22, "nW3Min");
  NextionNumber nW4Std(nex, 1, 28, "nW4Std");
  NextionNumber nW4Min(nex, 1, 29, "nW4Min");
  NextionButton btnSave(nex, 1, 7, "bSave");
  NextionButton btnMain1(nex, 1, 35, "bMain");
  
//Page 2 (Keypad)
  NextionPage pgKeypad(nex, 2, 0, "keypdC");
  
// Page 3 (Alarm active)
  NextionPage pgAlarm(nex, 3, 0, "page2");
  NextionButton btnSnooze(nex, 3, 2, "bSnooze");
  NextionButton btnQuitt(nex, 3, 3, "bQuitt");

// Page 4 (Weather forecast)
  NextionButton btnMain4(nex, 4, 23, "bMain");
  NextionButton btnSettings(nex, 4, 2, "bSettings");
  NextionText txtTime1(nex, 4, 21, "t2");
  NextionText txtTemp1(nex, 4, 22, "t20");
  NextionText txtHumidity1(nex, 4, 24, "t5");
  NextionText txtWind1(nex, 4, 25, "t21");
  NextionText txtRain1(nex, 4, 26, "t22");
  NextionText txtTime2(nex, 4, 30, "t26");
  NextionText txtTemp2(nex, 4, 31, "t27");
  NextionText txtHumidity2(nex, 4, 32, "t28");
  NextionText txtWind2(nex, 4, 33, "t29");
  NextionText txtRain2(nex, 4, 34, "t30");
  NextionText txtTime3(nex, 4, 35, "t31");
  NextionText txtTemp3(nex, 4, 36, "t32");
  NextionText txtHumidity3(nex, 4, 37, "t33");
  NextionText txtWind3(nex, 4, 38, "t34");
  NextionText txtRain3(nex, 4, 39, "t35");
  NextionText txtTime4(nex, 4, 40, "t36");
  NextionText txtTemp4(nex, 4, 41, "t37");
  NextionText txtHumidity4(nex, 4, 42, "t38");
  NextionText txtWind4(nex, 4, 43, "t39");
  NextionText txtRain4(nex, 4, 44, "t40");
  NextionText txtTotalRain(nex, 4, 45, "t41");
    
// Page 5 (Settings)
  NextionText txtLux(nex, 5, 3, "txtLux");
  NextionNumber nActVol(nex, 5, 19, "n2");
  NextionNumber nActMp3(nex, 5, 9, "n0");
  NextionNumber nTotMp3(nex, 5, 16, "n1");
  NextionNumber nActEQ(nex, 5, 20, "n3");
  NextionButton btnMain5(nex, 5, 2, "b1");
  NextionButton btnPlay(nex, 5, 5, "b0");  
  NextionButton btnStop(nex, 5, 6, "b2");
  NextionButton btnPrev(nex, 5, 8, "b4");
  NextionButton btnNext(nex, 5, 7, "b3");
  NextionButton btnVolP(nex, 5, 11, "b11");
  NextionButton btnVolM(nex, 5, 12, "b12");  
    
class Handler : public INextionCallback
{
public:
  Handler()
      : INextionCallback()
  {
  }
  void handleNextionEvent(NextionEventType type, INextionTouchable *widget)
  {
    if (type == NEX_EVENT_PUSH)
    {
      if (widget == &btnSave){
        saveAlarms();        
      }
      
      if (widget == &btnQuitt){
        Serial.println("Quitt");
        for (byte i=0; i<=3; i++){
          if (almStatus[i] &B00010000 == 16){     // Alarm Active
            bitWrite(almStatus[i],6,1);           // Set quitt
            bitWrite(almStatus[i],4,0);           // Reset active
            myDFPlayer.stop();                     //Stop mp3
            bAlarm = 0;
            refreshClockPage();
          }
        }                
      }
      
      if (widget == &btnSnooze){
       Serial.println("Snozze");  
        for (byte i=0; i<=3; i++){
          if (almStatus[i] &B00010000 == 16){     // Alarm Active
            bitWrite(almStatus[i],5,1);           // Set snozze
            time_t tSnooze = now();
            refreshClockPage();
            Serial.println(minute(tSnooze));
          }
        }  
      }
    }
    
    else if (type == NEX_EVENT_POP)
    { 
      if (widget == &touchAlarm){
        actPage = 1;
        displayAlarms();
      }     
      if (widget == &btnMain1 || widget == &btnMain4 || widget == &btnMain5){
        actPage = 0;
        refreshClockPage();       
      }
      if (widget == &btnForecast){
        actPage = 4;
        refreshForecastPage();       
      }
      if (widget == &btnSettings){
        refreshSetupPage();
      }
      if (widget == &btnNext){
        myDFPlayer.next();  //Play next mp3
        refreshSetupPage();
      }
      if (widget == &btnPrev){
        myDFPlayer.previous();  //Play previous mp3
        refreshSetupPage();
      }
      if (widget == &btnPlay){
        myDFPlayer.play(1); //Play the first mp3
        refreshSetupPage();
      }
      if (widget == &btnStop) myDFPlayer.pause();
      if (widget == &btnVolP){
        myDFPlayer.volumeUp(); //Volume Up
        nActVol.setValue(myDFPlayer.readVolume());
      }
      if (widget == &btnVolM){
        myDFPlayer.volumeDown(); //Volume Down
        nActVol.setValue(myDFPlayer.readVolume());
      }
    }
  }
};

Handler handle;

void atachTouch(){                      // All touchebles and button in HMI
  btnSave.attachCallback(&handle);
  btnSnooze.attachCallback(&handle);
  btnQuitt.attachCallback(&handle);
  touchAlarm.attachCallback(&handle);
  btnMain1.attachCallback(&handle);
  btnForecast.attachCallback(&handle);
  btnSettings.attachCallback(&handle);
  btnMain4.attachCallback(&handle);
  btnMain5.attachCallback(&handle);
  btnPlay.attachCallback(&handle);
  btnStop.attachCallback(&handle);
  btnPrev.attachCallback(&handle);
  btnNext.attachCallback(&handle);
  btnVolP.attachCallback(&handle);
  btnVolM.attachCallback(&handle);
}

void refreshSetupPage(){
  nActVol.setValue(myDFPlayer.readVolume());
  nTotMp3.setValue(myDFPlayer.readFileCounts()); //read all file counts in SD card
  nActMp3.setValue(myDFPlayer.readCurrentFileNumber()); //read current play file number  
}

void LongToChar2(float floatVal,int select, int decimal);

void refreshForecastPage(){
  char cDate[6];
  sprintf(cDate,"%02d:%02d", hour(forecastTime[0]), minute(forecastTime[0]));
  txtTime1.setText(cDate);
  LongToChar2(temp[0],1,1);
  LongToChar2(humidity[0],2,0);
  LongToChar2(wind[0]*3.6,3,1);
  LongToChar2(rain[0],4,1);

  sprintf(cDate,"%02d:%02d", hour(forecastTime[1]), minute(forecastTime[1]));
  txtTime2.setText(cDate);
  LongToChar2(temp[1],5,1);
  LongToChar2(humidity[1],6,0);
  LongToChar2(wind[1]*3.6,7,1);
  LongToChar2(rain[1],8,1);

  sprintf(cDate,"%02d:%02d", hour(forecastTime[2]), minute(forecastTime[2]));
  txtTime3.setText(cDate);
  LongToChar2(temp[2],9,1);
  LongToChar2(humidity[2],10,0);
  LongToChar2(wind[2]*3.6,11,1);
  LongToChar2(rain[2],12,1);

  sprintf(cDate,"%02d:%02d", hour(forecastTime[3]), minute(forecastTime[3]));
  txtTime4.setText(cDate);
  LongToChar2(temp[3],13,1);
  LongToChar2(humidity[3],14,0);
  LongToChar2(wind[3]*3.6,15,1);
  LongToChar2(rain[3],16,1);
  
  LongToChar2(totalRain,17,1);
}

void LongToChar2(float floatVal,int select, int decimal){    
  String stringVal = ""; 
  String sDecPart = ""; 

  int intPart = int(floatVal);
  long decPart = pow(10,decimal) *(floatVal-intPart);              //Change to match the number of decimal places you need
                                                      
  if(decPart>0)sDecPart = String(decPart);            //return the decimal part of float number if it is available 
  else if(decPart<0)sDecPart = String((-1)*decPart);  //if negative, multiply by -1
  else if(decPart==0)sDecPart = String("0");             //return 0 if decimal part of float number is not available
     
  if (decimal > 0) stringVal+=String(intPart)+ "." + sDecPart;         //combining both whole and decimal part in string with a fullstop between them 
  if (decimal == 0) stringVal+=String(intPart);
  char charVal[stringVal.length()+1];                   //initialise character array to store the values
  stringVal.toCharArray(charVal,stringVal.length()+1);  //passing the value of the string to the character array

  if (select ==1) txtTemp1.setText(charVal);     //display character array
  if (select ==2) txtHumidity1.setText(charVal);
  if (select ==3) txtWind1.setText(charVal);
  if (select ==4) txtRain1.setText(charVal);
  if (select ==5) txtTemp2.setText(charVal);     //display character array
  if (select ==6) txtHumidity2.setText(charVal);
  if (select ==7) txtWind2.setText(charVal);
  if (select ==8) txtRain2.setText(charVal);
  if (select ==9) txtTemp3.setText(charVal);     //display character array
  if (select ==10) txtHumidity3.setText(charVal);
  if (select ==11) txtWind3.setText(charVal);
  if (select ==12) txtRain3.setText(charVal);
  if (select ==13) txtTemp4.setText(charVal);     //display character array
  if (select ==14) txtHumidity4.setText(charVal);
  if (select ==15) txtWind4.setText(charVal);
  if (select ==16) txtRain4.setText(charVal);
  if (select ==17) txtTotalRain.setText(charVal);
}

