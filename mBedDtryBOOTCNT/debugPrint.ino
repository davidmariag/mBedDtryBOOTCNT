void debugPrint(void *pvParameters) //loop para procesos del dosificador en Core0
{
  while (1)
  {
    if((millis() - prevMillisDebug)>1087)
    {
      sprintf(debug1,"page1.t0.txt=\"HR:%u_MOvrrd:%u_DtctCntr:%u_RmMtn:%u\"\xFF\xFF\xFF",timeinfo.tm_hour,motionOveride,detectedCounter,roomMotion);
      Serial2.print(debug1);
      sprintf(debug2,"page1.t2.txt=\"zzActive:%u_zzLeft:%u_snz:%u\"\xFF\xFF\xFF",snoozeActive,timeLeft,snoozeLength);
      Serial2.print(debug2);
      sprintf(debug3,"page1.debug.txt=\"ambient:%u_bckLite:%u_mlight:%u\"\xFF\xFF\xFF",ambientL,backLight,mLightState);
      Serial2.print(debug3);      
      prevMillisDebug = millis();
      //WriteNum("page1.debug.txt", mLightState);
    }
  }
}
