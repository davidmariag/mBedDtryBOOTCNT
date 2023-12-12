void connect_to_MQTT()
{
  debugln("Connecting to MQTT...");
  if(MQTTclient.connect(mqtt_client_id.c_str(), mqtt_username, mqtt_password, (base_mqtt_topic + "/Status").c_str(),0, 0, "Dead"))   //c_str(),0, 1, "Dead"))
  {
    delay(500);
    // We get here if the connection was successful... 
    debugln("MQTT Connected");
       
    
    // ... and then re/subscribe to the watched topics
    MQTTclient.subscribe((base_mqtt_topic + "/mLight").c_str());   // Watch the .../LED topic for incoming MQTT messages
    MQTTclient.subscribe((base_mqtt_topic + "/mFan").c_str());   // Watch the .../LED topic for incoming MQTT messages
    MQTTclient.subscribe((base_mqtt_topic + "/States").c_str());   // Watch the .../LED topic for incoming MQTT messages 
    MQTTclient.subscribe((base_mqtt_topic + "/tempOut").c_str());   // Watch the .../LED topic for incoming MQTT messages 
    MQTTclient.subscribe((base_mqtt_topic + "/humOut").c_str());   // Watch the .../LED topic for incoming MQTT messages 
    MQTTclient.subscribe((base_mqtt_topic + "/windS").c_str());   // Watch the .../LED topic for incoming MQTT messages 
    MQTTclient.subscribe((base_mqtt_topic + "/windD").c_str());   // Watch the .../LED topic for incoming MQTT messages 
    MQTTclient.subscribe((base_mqtt_topic + "/dLamp").c_str());   // Watch the .../LED topic for incoming MQTT messages 
    MQTTclient.subscribe((base_mqtt_topic + "/mLamp").c_str());   // Watch the .../LED topic for incoming MQTT messages 
    MQTTclient.subscribe((base_mqtt_topic + "/GTB").c_str());   // Watch the .../LED topic for incoming MQTT messages 
    MQTTclient.subscribe((base_mqtt_topic + "/dLampfb").c_str());
    MQTTclient.subscribe((base_mqtt_topic + "/mLampfb").c_str());
 
     // Add other watched topics in here...  <--------------------- THIS IS THE BIT THAT YOU'LL FORGET TO DO!!

    // We can update the /Status topic to "Alive" now...
    
    MQTTclient.publish((base_mqtt_topic + "/Status").c_str(),"Alive",true);  //was true

  } 
  else
  {
    // We get here if the connection failed...   
    debug(F("MQTT Connection FAILED, Return Code = "));
    debugln(MQTTclient.state());
 //   debugln(); 
    /*
    MQTTclient.state return code meanings...
    -4 : MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time
    -3 : MQTT_CONNECTION_LOST - the network connection was broken
    -2 : MQTT_CONNECT_FAILED - the network connection failed
    -1 : MQTT_DISCONNECTED - the client is disconnected cleanly
     0 : MQTT_CONNECTED - the client is connected
     1 : MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT
     2 : MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier
     3 : MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection
     4 : MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected
     5 : MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect * 
     */
  }

}

void MQTTcallback(char* topic, byte* payload, unsigned int length)
{
  debug(F("Message arrived ["));
  debug(topic);
  debug(F("] ["));
  for (int i=0;i<length;i++)
  {
    debug((char)payload[i]);
  }
  debugln(F("]"));    
  
  msg_topic = String((char *)topic);
  msg_payload = String((char *)payload);
  msg_payload.remove(length); // Trim any unwanted characters off the end of the string
  // We now have two string variables, 'msg_topic' and 'msg_payload' that we can use in 'if' statements below... 
  
  if (msg_topic==base_mqtt_topic + "/GTB" )    //go to bed routine
  {
    xSemaphoreTake(Semaphore, 300);
    mLightState = 0;
    xSemaphoreGive(Semaphore);
    snoozeActive = 1;
    motionOveride = 1;
    snoozeCounter  = 0;
    roomMotion = 0;
    detectedCounter = 0;
    snoozeLength = snoozeLength + 10800;
  }
  
  if (msg_topic==base_mqtt_topic + "/tempOut" )
  {
     WriteStr("page0.outT.txt", msg_payload);
  }

  if (msg_topic==base_mqtt_topic + "/humOut" )
  {
     WriteStr("page0.outH.txt", msg_payload);
  }

  if (msg_topic==base_mqtt_topic + "/windS" )
  {
     WriteStr("page0.windS.txt", msg_payload);
  }

  if (msg_topic==base_mqtt_topic + "/windD" )
  {
     int windDeg = atoi(msg_payload.c_str());
     if(windDeg < 23 | windDeg > 338)
     {      
       WriteNum("b0.picc",0);       //n
       debugln("wind 1"); 
     }
     else if(windDeg < 68)
     {
       WriteNum("b0.picc",1);       //se      
       debugln("wind 2");
     }
     else if(windDeg < 113)
     {
       WriteNum("b0.picc",3);         //e     
       debugln("wind 3");   
     }
     else if(windDeg < 158)
     {
       WriteNum("b0.picc",4);         //se      
       debugln("wind 4");    
     }
     else if(windDeg < 203)
     {
       WriteNum("b0.picc",5);     //s
       debugln("wind 5");   
     }
     else if(windDeg < 248)
     {
       WriteNum("b0.picc",6);    //sw
       debugln("wind 6");    
     }
     else if(windDeg < 293)
     {
       WriteNum("b0.picc",7);    //w
       debugln("wind 7");  
     }
     else
     {
       WriteNum("b0.picc",8);     //ne
       debugln("wind 8");    
     }
  }
    
  if (msg_topic==base_mqtt_topic + "/mFan" )
  {
    mfanButton();
  }

  if (msg_topic==base_mqtt_topic + "/mLight" )
  {
    mlightButton();    //decides how to handle message
  }
  
  if (msg_topic==base_mqtt_topic + "/mLampfb" )
  {
    if(msg_payload == "0")
    {
      WriteNum("page0.maLamp.picc",0);
    }
    if(msg_payload == "1")
    {
      WriteNum("page0.maLamp.picc",1);
    }
  }

  if (msg_topic==base_mqtt_topic + "/dLampfb" )
  {
    if(msg_payload == "0")
    {
      WriteNum("page0.dvLamp.picc",0);
    }
    if(msg_payload == "1")
    {
      WriteNum("page0.dvLamp.picc",1);
    }
  }
  
  if (msg_topic==base_mqtt_topic + "/States")
  {
    if (msg_payload=="Alive") 
      {
        //put all feed back topics here
        //debugln("Alive feed back triggered");
        firstLoop = 0;
        MQTTclient.publish((base_mqtt_topic + "/mLightfb").c_str(),String(mLightState).c_str(),true);
        MQTTclient.publish((base_mqtt_topic + "/mFanfb").c_str(),String(mfanState).c_str(),true);      
      }
  }
  // Handle messages from other topics in here,
  // DON'T FORGET to subscribe to the topic in void MQTT_Connect()
  msg_payload = "m";
} // End of void MQTTcallback
