void data_in_Nex()
{
  if(Serial2.available())
  {
    data_from_Nex = "";
    delay(30);
    while(Serial2.available())
    {
      data_from_Nex += char(Serial2.read());
    }
    if(data_from_Nex == "mstLit")
    {
      xSemaphoreTake(Semaphore, 300);
      mLightState = !mLightState;
      xSemaphoreGive(Semaphore);
    }
    if(data_from_Nex == "mstFan")
    {
      mfanState = !mfanState;
    }
    if(data_from_Nex == "dvLamp")
    {
      MQTTclient.publish((base_mqtt_topic + "/dLamp").c_str(),"toggle",true); 
    }
    if(data_from_Nex == "maLamp")
    {
      MQTTclient.publish((base_mqtt_topic + "/mLamp").c_str(),"toggle",true); 
    }
    if(data_from_Nex == "zzz")
    {
      MQTTclient.publish((base_mqtt_topic + "/mLamp").c_str(),"snz",true);    //turns off auto lamps
      debugln("Motion zzz off");
      snoozeActive = 1;
      motionOveride = 1;
      roomMotion = 0;
      snoozeCounter  = 0;
      detectedCounter = 0;
      snoozeLength = snoozeLength + 10800;
      debug("snoozeLength ");
      debugln(snoozeLength);
      if(!saveConfig())
      {
        debugln("didnt save");
      }
    }
    if(data_from_Nex == "xzzz")
    {
      xSemaphoreTake(Semaphore, portMAX_DELAY);
      mLightPrev = 1;
      mLightState = 1;
      xSemaphoreGive(Semaphore);
      
      WriteNum("mstrLt.picc",mLightState);   //write status to Nextion   
      MQTTclient.publish((base_mqtt_topic + "/mLightfb").c_str(),String(mLightState).c_str(),true);
        if(!saveConfig())
        {
          debugln("didnt save");
        }
      MQTTclient.publish((base_mqtt_topic + "/mLightfb").c_str(),"m",true);
      
      digitalWrite(lightTogglePin,0);
      delay(500);
      digitalWrite(lightTogglePin,1);
            
      MQTTclient.publish((base_mqtt_topic + "/mLamp").c_str(),"snzDn",true);    //turns off auto lamps
      debugln("Motion Xzzz");
      snoozeActive = 0;
      motionOveride = 0;
      roomMotion = 0;
      snoozeCounter  = 0;
      detectedCounter = 0;
      snoozeLength = 0;
      timeLeft = 0;
      debug("snoozeLength ");
      if(!saveConfig())
      {
        debugln("didnt save");
      }
    }

//    if(data_from_Nex == "xzzz")
//    {
//      debugln("lightOn");
//      digitalWrite(lightOnPin,0);
//      delay(500);
//      digitalWrite(lightOnPin,1);
//    }
    
    data_from_Nex = "";
  }
}
