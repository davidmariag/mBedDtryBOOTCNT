#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"  //esp32 brownout disable
#include "LittleFS.h"
#include "FS.h"
#include <ArduinoJson.h>
#include <PubSubClient.h>     // https://github.com/knolleary/pubsubclient
//#include <ESP8266WiFi.h>  //esp8266
#include <WiFi.h>       //esp32
//#include <ESP8266mDNS.h>  //esp8266
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SimpleTimer.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define DEBUG 0

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

//#define WIFI_TASK_CORE_ID 1

//#define MQTT_KEEPALIVE 15  changed in library default is 15
// Wi-Fi variables...
const char *ssid =                "***arm"; 
const char *pass =                "***1";


// MQTT Server Setting variables...
IPAddress mqtt_server_ip          (192,168,1,157);                     // IP Address for the MQTT Server...
const int mqtt_port =             1883;                                  // Port for the MQTT Server...
const char* mqtt_username =       "d*";                            // MQTT Server username...
const char* mqtt_password =       "m*";                            // MQTT Server password...
String base_mqtt_topic =          "mBed";    // Start of the MQTT Topic name used by this device
String mqtt_client_id;                                                   // Will hold unique ID, so use ChipID will be assigned in void setup

String msg_payload;
String msg_topic;

String data_from_Nex;
//#define remoteGrn 4
#define RXD2 16    
#define TXD2 17
//pins 21 and 22 SDA and SCL respectively
#define fanOnPin 26       //white wire
#define fanOffPin 18      //green wire
///#define lightOnPin 19     //yellow wire  23  
#define lightTogglePin 19    //blue wire
#define ambientPin 36     //ADC1_0 36 light sense  10K pho to GRN (other to 3.3V)
#define motionSensor 39 //PIR

#define SEALEVELPRESSURE_HPA (1013.25)

int motionOveride = 0;           //used for
bool detect = 0;
int detectedCounter = 0;
int roomMotion = 0;
int snoozeActive = 0;
int snoozeCounter = 0;
int snoozeLength = 0;

int bootCnt;
int wifiCnt;
//filesave vars
String DBRecord [1];
int totalRecords = 1;
String stored_Vars;
const char* filename = "/vars.txt";
char debug3[32];
char debug2[32];
char debug1[32];

// Create instances of the Wi-Fi, MQTT and SimpleTimer objects...
WiFiClient My_WiFi_Client;
PubSubClient MQTTclient(My_WiFi_Client);
SimpleTimer timer;
volatile int mLightState = 0;  
volatile int mLightPrev = 0;
volatile int mfanState = 0;
volatile int mFanPrev = 3;
int heaterState = 0;
float roomTemp = 32.l;

int ambientL;
String backLightStr;
int backLight = 1;
int timeLeft;
unsigned long prevMillisWifi;
unsigned long prevMillisMQTT;
unsigned long prevMillisDebug;
unsigned long prevMillisAmb;  //for ambientloop
unsigned long prevMillisTm;
unsigned long prevMillisRSSI;
static volatile int firstTime = 1;
static volatile int firstLoop = 1;
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -21600;
const int   daylightOffset_sec = 3600;
struct tm timeinfo;

Adafruit_BME280 bme;
static volatile float floatTemp;
static volatile int intHumid;

#define FORMAT_LITTLEFS_IF_FAILED true //first time only

SemaphoreHandle_t Semaphore;
SemaphoreHandle_t SemaphoreFan;

void connect_to_Wifi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    //WriteStr("page0.maxT.txt", "discon");
    debug("Connecting to ");
    debug(ssid);
    debugln("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      return;
    }
  }

  wifiCnt ++;
  WriteStr("page1.t3.txt", String(wifiCnt));
  saveConfig();
  debug("IP Address = ");
  debugln(WiFi.localIP());
  debugln("WiFi Connected");
  connect_to_MQTT();
} 

void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  xTaskCreatePinnedToCore(loop0, "loop0", 4096, NULL, 0, NULL, 0);
  xTaskCreatePinnedToCore(loop1, "loop1", 4096, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(ambientCheck, "ambientCheck", 2000, NULL, 0, NULL, 0);
  xTaskCreatePinnedToCore(timeCheck, "timeCheck", 3000, NULL, 0, NULL, 1);
  xTaskCreatePinnedToCore(RSSIcheck, "RSSIcheck", 2500, NULL, 0, NULL, 0);
  xTaskCreatePinnedToCore(debugPrint, "debugPrint", 2500, NULL, 0, NULL, 0);
 
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  Semaphore = xSemaphoreCreateMutex();
  SemaphoreFan = xSemaphoreCreateMutex();
    
  Serial.begin(115200);
  Serial2.begin(250000, SERIAL_8N1, RXD2, TXD2);

  WiFi.mode(WIFI_STA); 
 
  pinMode(lightTogglePin,OUTPUT);
  digitalWrite(lightTogglePin,HIGH);
  pinMode(fanOffPin,OUTPUT);
  pinMode(fanOnPin,OUTPUT);
  digitalWrite(fanOffPin,HIGH);
  digitalWrite(fanOnPin,HIGH);

  pinMode(motionSensor, INPUT);
  
  if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
  {
    debugln("An Error has occurred while mounting LittleFS");
    return;
  }
//  saveConfig();
  if(!loadConfig())
  {
    debugln("didn't load");
  }
  

  bootCnt++;
  if(!saveConfig())
  {
    debugln("didnt save");
  }

  WriteStr("page0.maxT.txt", String(bootCnt));
    
  connect_to_Wifi();
  
//createDir(LittleFS, "/mydir");              //firsttime esp32 littleFS
//listDir(LittleFS, "/", 3);                  //uncomment under store_Vars 
  debugln("setup");

//esp8266  mqtt_client_id = String(ESP.getChipId());
  MQTTclient.setServer(mqtt_server_ip, mqtt_port);
  MQTTclient.setCallback(MQTTcallback);           // This function is called automatically whenever a message arrives on a subscribed topic

  bme.begin(0x76);
  ArduinoOTA.setHostname("mBedDtryBoot");
  ArduinoOTA.begin();    

}

void loop()
{
  vTaskDelete (NULL);
}

void loop1(void *pvParameters) //loop para procesos del dosificador en Core0
{
  while (1)
  {
    if((WiFi.status() != WL_CONNECTED)&& (millis()- prevMillisWifi) >= 4999)
    {
      prevMillisWifi = millis();
      debugln("tring to connect wifi");
      connect_to_Wifi();
    }
    else
    {  
      if (!MQTTclient.connected() && (millis() - prevMillisMQTT) >= 5009)     // Note that MQTTclient.connected() will still return 'true' until the MQTT keepalive timeout has expired (around 35 seconds for my setup) 
      {
        prevMillisMQTT = millis();
        debug("loop not mqtt");
        connect_to_MQTT();
      }
      else
      {
        MQTTclient.loop();
        //debugln("looping MQTT");
      }
    }
    timer.run();
    ArduinoOTA.handle(); 
  }
}
