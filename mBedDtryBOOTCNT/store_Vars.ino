bool loadConfig() 
{
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
    debugln("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    debugln("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  debug("loading file ");

  
  StaticJsonDocument<1024> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    debugln("Failed to parse config file");
    return false;
  }

  
  mLightState = doc["mLight"];
  motionOveride = doc["mO"];
  snoozeActive = doc["snzA"];
  snoozeLength = doc["snzL"];
  snoozeCounter = doc["snzC"];
    xSemaphoreTake(Semaphore, 100);
  bootCnt = doc["bootC"];
  wifiCnt = doc["wifiC"];
  mLightPrev = mLightState;
    xSemaphoreGive(Semaphore);
  WriteNum("mstrLt.picc",mLightState);   //write status to Nextion   
  MQTTclient.publish((base_mqtt_topic + "/mLightfb").c_str(),String(mLightState).c_str(),true);

  timeLeft = snoozeLength-snoozeCounter;
//  debugln(snoozeCounter);  
  return true;
}

bool saveConfig() 
{
  StaticJsonDocument<1024> doc;
  doc["mLight"] = mLightState;
  doc["mO"] = motionOveride;
  doc["snzA"] = snoozeActive;
  doc["snzL"] = snoozeLength;
  doc["snzC"] = snoozeCounter;
  doc["bootC"] = bootCnt;
  doc["wifiC"] = wifiCnt;
  
  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
    debugln("Failed to open config file for writing");
    return false;
  }

  debug("saving file ");
  debugln(snoozeCounter); 
  
  serializeJson(doc, configFile);
  return true;
}
