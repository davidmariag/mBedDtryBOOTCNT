void mlightButton()   //sets state based on input from MQTT
{
  if (msg_payload == "toggle")
  {
    debugln(mLightState);
    xSemaphoreTake(Semaphore, 100);
    mLightState = !mLightState;
    xSemaphoreGive(Semaphore);
    debugln(mLightState);
  }
  if (msg_payload == "1")
  {
    xSemaphoreTake(Semaphore, 100);
    mLightState = 1;
    xSemaphoreGive(Semaphore);
  }
  if (msg_payload == "0")
  {
    xSemaphoreTake(Semaphore, 100);
    mLightState = 0;
    xSemaphoreGive(Semaphore);
  }
}

void mfanButton()   //sets state based on input from MQTT
{
  if (msg_payload == "toggle")
  {
    debugln(mfanState);
    mfanState = !mfanState;
  }
  if (msg_payload == "1")
  {
    mfanState = 1;
  }
  if (msg_payload == "0")
  {
    mfanState = 0;
  }
}
