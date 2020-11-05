// Smart Home Node by oL0ck
// Node just ask sensors and shows the result on the internal web page
// Allows simply get data form sensors via web-api request
// overlockg@gmail.com

// WARNING this code is out of date, does not supported anymore, just like sketchbook
// Anyway, it work in Arduino 1.8x, with esp32dev libs installed, if it needed for someone

#include <Preferences.h>        // ROM RW native esp32 library
#include <WiFi.h>               // wifi module
#include <MHZ19_uart.h>         // MH-Z19 spectro-CO2 meter, over softserial
#include <RBD_Timer.h>          // program timer for excluding delay`s
#include <Wire.h>               // wire bus library 
                                   // Wire i2c bus for wemos lolin v 1.0.0 board using 17,22 PINS, works good
#include <HTU21D.h>             // SI7020 temp and hummidity sensor, over wire lib
#include  "ping.h"              // get it from esp82 community for gateway ping wifi test
#include <ESPmDNS.h>            // bonjur zeroconf server-client, for name node in OTA resolving
#include <WiFiUdp.h>            // UDP transport for OTA, not similar in ping
#include <ArduinoOTA.h>         // OTA - firmware over wifi
#include "SparkFunCCS811.h"     // CCS811 FluedOrganicConcentration, like eCO2(probably) or whiskey(:
  #define CCS811_ADDR 0x5B         // Default I2C Address or \#define CCS811_ADDR 0x5A\ alternate I2C Address
#include <FS.h>                 /// This block
#include <AsyncTCP.h>           /// used for
#include <ESPAsyncWebServer.h>  /// Async WebServer


// Hostname defaults to esp3232-[MAC]
// ArduinoOTA.setHostname("myesp32");
// No authentication by default
// ArduinoOTA.setPassword("admin");
// Password can be set with it's md5 value as well
// MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
// ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

HTU21D myHTU21D(HTU21D_RES_RH12_TEMP14);  // SI7020
CCS811 mySensor(CCS811_ADDR);             // CCS811
MHZ19_uart mhz19;                         // MH-z19
AsyncWebServer server(80);                // AsyncWebInit
Preferences Config;                       // NVS Storage for startup config and logging


RBD::Timer timer;                // timer for scenners request
RBD::Timer timerwifi;            // timer for wifi check cycle
RBD::Timer timerwifiinit;        // timer wifi init timeout
RBD::Timer timercominit;         // timer for usb-com init timeout
RBD::Timer timerwatchdogsensors; // how frequently check sensors
RBD::Timer timermhz19calibrate;  // for MH-z19 calibrate

const String ver = "SH-node rev 2.0";                           // Release name
const char* ssid     = "SSidHere";                    // Ap name
const char* password = "WpaSecret";   // Ap password
const int SCANSENSORS = 1000;                               // period requesting data from sensors...
const int WATCHDOGCOUNT = 64;                                // if counter is over esp will reboot, increase every SCANSENSORS &or WIFICHECKTIMER secs, if problems found
const int WIFICHECKTIMER = 60000;                           // period of gateway ping for check wifi
const int WIFIINITTIMER = 120000;                           // waitin until wifi linked at startup
const int COMUNTILREADY = 15000;                            // wait 15 secs for USB com connected
const int SENSORSWDCHECK = 10000;                           // check sensors every N seconds
const String LEGEND[11] = {                                 // Legend string for sensors
  "All","MH-z19 CO2","MH-z19 Temp",
  "SI7020 Hummidity","SI7020 Temp",
  "Update CCS Hum and Temp","CCS Measuring",
  "CCS811 CO2","CCS811 tVOC",
  "Emon","debug"};
const int MHZ19CALIBTIMER = 10000;                          // time wait for calibrating
const int MHZ19CALIBCOUNTS = 10;                            // repeat times calibrating

String NODENAME = "SH-Node";                    // it needs if ROM dose not respond
String NODEIP = "";                             // get current Ip in str
bool sensorsona[11] = {0,1,1,1,1,1,1,1,1,1,1};  // Turn every sensor`s parameter on-off, 0 index turns all sensors, 10 turn`s off debug to serial
char linebuf[80];                               // web parse buff
int co2ppm=0;                                   // MH-z19 CO2 storing
int csstvoc=0;                                  // CCS811 tVOC storing
int cssco2ppm=0;                                // CCS811 CO2 storing(calculated from tVOC)
int temp=0;
float sihummidity=0;      // SI7020 hummidity
float sitemp=0;           // SI7020 temperature
int watchdog=0;           // main watchdog counter
int wifiok=0;             // wifi checks controlled param
int sscount=0;            // SensorSelect counter
unsigned int cyclesc;     // Reboot times couter
int rx_pin = 18;          // SoftSerial rx pin no for MH-z19
int tx_pin = 19;          // SoftSerial tx pin no for MH-z19
int mhz19calib = 0;       // MH-z19 calibrating status
String ccsstatus;         // CCS sensor main status
String ccsintstatus;      // CCS sensor internal status
int apic[] = {0,0,0,0};   // min, max, avg, counter

int Fco2ppm = 401;       // Web filter varablies
float Fsihummidity = 41; // suppressing
float Fsitemp = 21;      // peak parameter
int Fcssco2ppm = 401;    // from sensors
int Fcsstvoc = 4;        // and-or use defaults
  
void setup() {
  TimersInit();
  Serial.begin(115200);                               // usb-serial init
  while(!Serial || !timercominit.isExpired()) {
    ;                                                 // wait for timeout or connected, in some cases it will wait infinity... so i use timeout
    }
  ConfigInit();                                       // rean NVS and fill current enviroment, node name, calibration, reboot counter etc...
  SensorsInit();                                      // do whatever need to init sensors selected in NVS or current config, maybe recalled if calibration changes
  delay(500);                                         // here we must wait for sensors get ready, but we just wait(:
  WifiInit();                                         // just connect to wifi or reboot if timed out, AP mode enabled after
  OTAinit();                                          // ===OTA=== init
  ArduinoOTA.begin();                                 // OTA begin, wireless firmware update... magics...    
  AsyncWebInit();                                     // AsyncWebServer Init and start, hooks used there, cannot change until reboot...
  };
 
void loop() {
  ArduinoOTA.handle();                //  ===OTA===
  SensorsAsk();                       // acking sensors
  WatchDog();                         // WatchDog watching
  }
