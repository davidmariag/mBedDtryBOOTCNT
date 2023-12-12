void loop0(void *pvParameters) //loop para procesos del dosificador en Core0
{
  while (1)
  {

    data_in_Nex();    
    delay(10);
    //start button loop
    if(firstLoop == 0)
    {
      if(mLightState != mLightPrev)
      {
        if(mLightState == 0)
        {
          debugln("lightOff");
          digitalWrite(lightTogglePin,0);
          delay(500);
          digitalWrite(lightTogglePin,1);
        }
        if(mLightState == 1)
        {
          debugln("lightOn");
          digitalWrite(lightTogglePin,0);
          delay(500);
          digitalWrite(lightTogglePin,1);
        }
        xSemaphoreTake(Semaphore, portMAX_DELAY);
        //xSemaphoreTake(Semaphore, 300);
        mLightPrev = mLightState;
        xSemaphoreGive(Semaphore);
        
        WriteNum("mstrLt.picc",mLightState);   //write status to Nextion   
        MQTTclient.publish((base_mqtt_topic + "/mLightfb").c_str(),String(mLightState).c_str(),true);
        if(!saveConfig())
        {
          debugln("didnt save");
        }
        debugln("state changed in loop");
        MQTTclient.publish((base_mqtt_topic + "/mLightfb").c_str(),"m",true);
      }


      if(mfanState != mFanPrev)
      {
        if(mfanState == 0)
        {
          debugln("fan off");
          digitalWrite(fanOffPin,mfanState);
          timer.setTimeout(500L,[]()
          {
            debugln("fan off2");
            digitalWrite(fanOffPin,!mfanState);
          });         
        }
        if(mfanState == 1)
        {
          debugln("fan on");
          digitalWrite(fanOnPin,!mfanState);
          timer.setTimeout(500L,[]()
          {
            debugln("fan on2");
            digitalWrite(fanOnPin,mfanState);
          });   
        }
        mFanPrev = mfanState;
        WriteNum("tmFan.en",mfanState);
        MQTTclient.publish((base_mqtt_topic + "/mFanfb").c_str(),String(mfanState).c_str(),true);
        if(!saveConfig())
        {
          debugln("didnt save");
        }
      }
      ///try saving remaining time for snooze if active
      if(snoozeActive == 1)      //save snooze time every 10s
      {
        unsigned long prevsnzSave;
        if((millis() - prevsnzSave) > 60029)
        {
          debugln("gottillheresaveloop");
          if(!saveConfig())
          {
            debugln("didnt save");
          }
          debugln("snooze active so saving every minute");
          prevsnzSave = millis();
        }
      }
    }
  }
}
