
void timeCheck(void *pvParameters) //loop para procesos del dosificador en Core0
{
  while (1)
  {
    if(WiFi.status() == WL_CONNECTED )
    {
      if((millis()- prevMillisTm) > 60037)
      {
        if(!getLocalTime(&timeinfo))
        {
          Serial.println("Failed to obtain time");
          return;
        }
        //debugln("first time done");
        debug("weekday ");
        debugln(timeinfo.tm_wday);  //weekDay
        if(timeinfo.tm_hour != 6)     //got time from server
        {
          firstTime = 0;
        }
        
        char timeStringBuff[50]; //50 chars should be enough
        if(timeinfo.tm_hour >= 12)
        {
          strftime(timeStringBuff, sizeof(timeStringBuff), "%l:%M PM", &timeinfo);
          debugln(timeStringBuff);
        }
        else
        {
            strftime(timeStringBuff, sizeof(timeStringBuff), "%l:%M AM", &timeinfo);
            debugln(timeStringBuff);
        }
        prevMillisTm = millis();
        MQTTclient.publish((base_mqtt_topic + "/time").c_str(),timeStringBuff,true);     // Uptime in seconds     
        WriteStr("page0.time.txt",timeStringBuff);
/////////////can not save from this core i guess
//        if(snoozeActive == 1)
//        {
//          debugln("gottillhereintimefunct");
//          if(!saveConfig())
//          {
//            debugln("didnt save");
//          }
//          debugln("snooze active so saving every minute");
//        }
      }
    }
  }
}
