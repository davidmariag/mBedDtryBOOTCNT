void ambientCheck(void *pvParameters) //loop para procesos del dosificador en Core0
{
  while (1)
  {
    if((millis() - prevMillisAmb)>503)
    {
      //debugln("checkambient");
      ambientL = map(analogRead(ambientPin), 0.0f, 1000.0f, 1, 100);
      //ambientL = map(analogRead(ambientPin), 0, 1500, 1, 100);
      //ambientL =(analogRead(ambientPin));
      //debug("ambient ");
      //debugln(ambientL);
      backLightStr = "";
      
      if(ambientL < backLight - 5)
      {
        backLight = backLight - 2;
        if(backLight <= 6 && backLight >=2 )
        {
          backLight = 1;
        }
        Serial2.print(F("dim="));
        Serial2.print(String(backLight));
        Serial2.print(F("\xFF\xFF\xFF"));
      }
    
      if(ambientL > backLight + 5)
      {
        backLight = backLight + 2;
        //backLightStr = String (backLight);
        Serial2.print(F("dim="));
        Serial2.print(String(backLight));
        Serial2.print(F("\xFF\xFF\xFF"));
      }

 
      if (firstTime == 0)  //make sure aquired time first before looping
      {
        //WriteStr("page0.maxT.txt", "1st");
        if((timeinfo.tm_hour >= 7) && (timeinfo.tm_hour <= 22) && (motionOveride == 0))
        {
          //debugln(timeinfo.tm_hour);
          //Serial.print("MasterLight ");
          //Serial.println(masterLight);
          detect =(digitalRead(motionSensor));
      
          if(detect == 1)
          {
            detectedCounter = 0;              //resets timer every time there is motion
      
            if(roomMotion == 0)       //if motion flag inactive
            {
              roomMotion = 1;         //sets motion flag active
              debug("roomMotion ");
              debugln(roomMotion);
              
              if(ambientL < 10 && mLightState == 0)
              {
                debugln("turningONLIGHT movement");
                xSemaphoreTake(Semaphore, portMAX_DELAY);
                mLightState = 1;
                xSemaphoreGive(Semaphore);
              }
            }
          }
        
          //motion pin LOW but counting to delay turning light off
          if(detect == 0 && roomMotion == 1)
          {
            detectedCounter ++;
            //debug("no movement ");
            //debugln(detectedCounter);
          }
        
          //motion timer turns motion off after 500 rounds of no motion
          if(detectedCounter >= 600)   //loop runs every 500ms 
          {
            roomMotion = 0;
        
            if(mLightState == 1)
            {
              debugln("turningofflight no movement");
              xSemaphoreTake(Semaphore, portMAX_DELAY);
              mLightState = 0;
              xSemaphoreGive(Semaphore);
            }
          }
        }
      }
      if(snoozeActive == 1)           //check so counter isn't incrementing unless being used
      {
        snoozeCounter ++;             //try too use counter for snooze function
        if(snoozeCounter >= snoozeLength)
        {
          motionOveride = 0;
          snoozeActive = 0;
          snoozeLength = 0;
          snoozeCounter = 0;
          WriteNum("page1.t2.txt", snoozeLength);
          MQTTclient.publish((base_mqtt_topic + "/mLampfb").c_str(),"snzdone",true);
        }
//        debug("snoozeLeft ");
        
        timeLeft = snoozeLength-snoozeCounter;
//        debugln(timeLeft);
      }
/////////////doesnt work to save here either
//      int ambientLoopcount;
//      ambientLoopcount ++;
//      if(ambientLoopcount >= 50)
//      {
//        ambientLoopcount = 0;
//        if(snoozeActive == 1)      //save snooze time every 10s
//        {
//          debugln("gottillhereintimefunct");
//          if(!saveConfig())
//          {
//            debugln("didnt save");
//          }
//          debugln("snooze active so saving every minute");
//        }
//      }
      prevMillisAmb = millis();
    }
  }
}
