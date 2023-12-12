void RSSIcheck(void *pvParameters) //loop para procesos del dosificador en Core0
{
  while (1)
  {
    if((millis() - prevMillisRSSI)>600013)
    {      
      char rrsiStringBuff[5];
      sprintf(rrsiStringBuff,"page0.minT.txt=\"%i\"\xFF\xFF\xFF", (WiFi.RSSI()));
      Serial2.print(rrsiStringBuff);  
          
      debug("gonna print RSSI ");
      debugln(WiFi.RSSI());
      
//check inside temp
      floatTemp = bme.readTemperature();
      intHumid = bme.readHumidity();
      WriteStr("page0.inT.txt", String(floatTemp,1));
      WriteStr("page0.inH.txt", String (intHumid));
      debug("temp read ");
      debugln(floatTemp);
        
      String humStr = String(intHumid);
      String tempStr = String (floatTemp,1); // or dht.readTemperature(true) for Fahrenheit
      String combinedStr = tempStr + "," + humStr;
      debugln(combinedStr);
      MQTTclient.publish((base_mqtt_topic + "/Temp").c_str(),String(combinedStr).c_str(),true);  
      combinedStr = " ";
      prevMillisRSSI = millis();
    }
  }
}
