// Modified by Mike Eitel to read temperatur and humidity by M5-C3U based board
// All special modified libraries are stored in the via platformio.ini defined libraries directory !!!
// Took some time to get the right versions etc. working flawless, so this unusual approach is choosen deliberately.
// No rights reserved when used non commercial, otherwise contact author.

// XIAO-ESP32-C3 pinout 
// D0   GPIO2   A0/D0    =  2     Used for powering DHT to have it resetable
// D1   GPIO3   A1/D1    =  3     Used for DHT data
// D2   GPIO4   A2/D2    =  4     Used for DS18B20 sensors
// D3   GPIO5   A3/D3    =  5     Chip select for tofl chips
// D4   GPIO6   SDA/D4   =  6     Data for I2C Data
// D5   GPIO7   SCL/D5   =  7     Clock for I2C Clock
// D6   GPIO21  TX/D6    = 21     Not used

// 5V                             Supply 5V
// GND                            GND
// 3V3                            Internal 3.3V
// D10  GPIO10  MOSI/D10  = 10    
// D9   GPIO3   MISO/D9   =  9    BOOT button on board
// D8   GPIO4   SCK/D8    =  8    Boot option
// D7   GPIO5   RX/D7/SS  = 20    Not used


#define Me     // If defined ( Me .. MeIOT .. LU ..  Rhy ) use private network for testing, otherwise use IOT standard
//#define TEST    // Testmodus
#define OTA       // If defined use OTA programming posibility

#include <cstdlib>
#include <WiFi.h>
//#include <SPI.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>

// Setup the background classes  
WiFiClient   espClient;                       // Get Wifi access
PubSubClient mqttclient;                      // MQTT protokol handler


// This files contain the device definitions for different networks
// In the credential files the fixed IP has to be defined
#if defined(Me)                       
    #include <Me_credentials.h>      
#elif defined(MeIOT)
    #include <MeIOT_credentials.h>     
#elif defined(MeTEST)
    #include <MeTEST_credentials.h>
#else
    #include <credentials.h>         
#endif

// Define used pins
#define DHT_POWER      2                      // Board pin D0 to supply power to the DHT sensor
#define DHT_PIN        3                      // Board pin D1 to connect data of DHT sensor
#define DS_PIN         4                      // Board pin D2 to connect data of DS18B20 sensors
#define XSHUT_PIN      5                      // Board pin D3 to stop tofl chip
#define SDA            6                      // Board pin D4 connect data of I2C chip
#define SCL            7                      // Board pin D5 connect clock of I2C chip

// Declare global variables and constants
unsigned long currentMillis;                  // Actual timer 
unsigned long prevRMQTTMillis = 2764472319;   // Stores last MQTT time value was published 2764472319->FASTER START
unsigned long prevSMQTTMillis = 2764472319;   // Stores last MQTT time value was published 2764472319->FASTER START
unsigned long prevMinMillis = 2764472319;     // Stores last minutes time value ->  2764472319->FASTER START
const int readSensorsinterval  = 3000;        // How often the sensors are read

String Sendme;                                // Used for clear text messages in MQTT
String MySensors;                             // A list of usable sensors
char debug_buf[96];                           // Placeholder for MQTT text messages
int receivedlenght;                           // How long is mqtt message
char lastreceived;                            // Stores the last received status
char receivedChar[10];                        //  = "";
bool received;                                // Actual received status
int watchdogW = 1;                            // Counter if there is no wifi connection
int watchdogM = 1;                            // Counter if there is no MQTT connection
int mqttstatus;                               // Helper to see whats going on
bool watchdog = true;                         // Signal via mqtt that device is still ok
bool statusreset = false;                     // Used to minimize error 0 sendouts
int looped = 1;                               // Loop counter as debug helper

//bool toomany = false;                         // Too many DS18 sensors connected
int tstatus = 0;                              // Status of Max6675 readout
int dscounter = 1;                            // Helper to seperate DS18 sensors
int dscounted = 0;                            // Readable value

int numsensors = 0;                           // Number of existing sensors
int numhum = 0;                               // Counter for existing humidity sensors
int numtemp = 0;                              // Counter for existing temperatur sensors
int numco2 = 0;                               // Counter for existing co2 sensors
int numtvoc = 0;                              // Counter for existing tvoc sensors
int numpress = 0;                             // Counter for existing presure sensors
int numdist = 0;                              // Counter for existing distance sensors
int numdeg = 0;                               // Counter for existing degree sensors
int numlight = 0;                             // Counter for existing light sensors
int numaccel = 0;                             // Counter for acceleration sensors
int nummag = 0;                               // Counter for magnet sensors

float ENScortemp = -1;                        // Possible ENS160 correction via a measured temp
float ENScorhum = -1;                         // Possible ENS160 correction via a measured humidity
 
float hum = -1;                               // Measured humidity
float temp = -1;                              // Measured temperature
uint16_t co2 = -1;                            // Measured air co2 ppm
uint16_t tvoc = -1;                           // Measured air quality
uint16_t dist = -1;                           // Measured distance
float press = -1;                             // Measured pressure
float angle = -1;                             // Measured angle
float light = -1;                             // Measured light
String qual = "";                             // Quality of an measure
String accel = "";                            // Measure acceration
float X = 0;                                    // measured value for X 
float Y = 0;                                    // measured value for Y
float Z = 0;                                    // measured value for Z

//#include <MySensors.h>

///////////////////////////////////////////////////////// Sensors ///////////////////////////////////
#if defined(enableDHT)                        // One wire based DHT temperatur and humidity sensors
  #include "MyDHTxx.h"
#endif
#if defined(enableDS)                         // One wire based DS temperatur  sensors
  #include "MyDSx.h"
#endif
#if defined(enableLM75)                       // Temperatur and humidity sensor
  #include "MyLM75.h"
#endif  
#if defined(enableAHT)                        // Temperatur and humidity sensor
  #include "MyAHT2x.h"
#endif  
#if defined(enableAGS10)                      // TVOC sensor via AGS10 board
  #include "MyAGS10.h"
 #endif  
#if defined(enableCCS)                        // TVOC and C02 sensor via CJMCU-811 board
  #include "MyCCS811.h"
 #endif  
#if defined(enableSGP30)                      // TVOC and C02 multigas sensor
  #include "MySGP30.h"
#endif 
#if defined(enableSHT31)                      // TVOC and C02 multigas sensor
  #include "MySHT31.h"
#endif 
#if defined(enableENS)                        // TVOC and C02 multigas sensor
  #include "MyENS160.h"
#endif  
#if defined(enableBMP18)                      // Temperature and barometric pressure sensor
  #include "MyBMP180.h"
#endif
#if defined(enableBMP28)                      // Temperature and barometric pressure sensor
  #include "MyBMP280.h"
#endif
#if defined(enableBME28)                      // Temperature, humidity and barometric pressure sensor
  #include "MyBME280.h"
#endif
#if defined(enableBMP39)                      // Temperature and barometric pressure sensor
  #include "MyBMP390.h"
#endif
#if defined(enableVL53L0)                     // TOFL sensor ca. 40 - 500
  #include "MyVL53L0X.h"
#endif
#if defined(enableVL6180)                     // TOFL sensor ca. 40 - 255
  #include "MyVL6180.h"
#endif
#if defined(enableMLX)                        // Touchless temp sensor
  #include "MyMLX90614.h"
#endif
#if defined(enableAS56)                       // Angle sensor 
  #include "MyAS5600.h"
#endif
#if defined(enableBH17)                       // Light sensor 
  #include "MyBH1750.h"
#endif
#if defined(enableVEML)                       // Light sensor 
  #include "MyVEML7700.h"
#endif
#if defined(enableLISD)                       // Acceleration sensor 
  #include "MyLIS3DH.h"
#endif
#if defined(enableLISM)                       // Magnet sensor 
  #include "MyLIS3MDL.h"
#endif
#if defined(enableMPU65)
  #include "MyMPU6500.h"                       // 6 axis sensor
#endif

///////////////////////////////////////////////////////// Sensors ///////////////////////////////////

void setup_wifi() {
  WiFi.disconnect(false);                     // Stack reset ohne SSID zu löschen
  delay(100);
  WiFi.hostname(iamclient);
  WiFi.config(staticIP, gateway, subnet);
  
  Serial.println("");
  Serial.print("Try connect to: ");
  Serial.println(wifi_ssid);
  Serial.print("With IP address: ");
  Serial.println(WiFi.localIP());

  WiFi.begin(wifi_ssid, wifi_password);
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  while ((WiFi.status()!= WL_CONNECTED) && (watchdogW <= WiFi_timeout)) {
    delay(250);
    Serial.print(".");
    Serial.print(watchdogW);
    watchdogW++;
    }
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("No connection -> Restart");
      WiFi.disconnect(true);
      delay(500);
      ESP.restart();
    }
  else {
    Serial.println(""); Serial.println("");
    Serial.print("Successfull connected Wifi with RSSI: "); 
    Serial.println(WiFi.RSSI());
   }
}

#if defined(OTA)                                    // Augment by OTA posibility
  #include <ArduinoOTA.h>
  bool otaInProgress = false;

void setupOTA(){                                    
    ArduinoOTA.end();                               // Bestehenden OTA-Server beenden falls aktiv
    delay(100);
    ArduinoOTA.setPort(3232);                       // Port defaults to 3232
    // Hostname defaults to esp32-[ChipID] Set this BEFORE calling begin()
    ArduinoOTA.setHostname(iamclient);              // Choose a unique name
    // No authentication by default Password can be set with MD5 hash as well MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    ArduinoOTA.setPassword("admin");

    // OTA event handlers
    ArduinoOTA.onStart([]() {
      otaInProgress = true;
      mqttclient.disconnect();                      // ← MQTT Socket freigeben vor OTA
      espClient.stop();                             // ← WiFiClient Socket explizit schliessen
      delay(100);
      Serial.println("OTA Started");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("OTA Progress: %u%%\n", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf(" IP:%s Port:%u\n", ArduinoOTA.getHostname().c_str(), 3232);
        Serial.printf("OTA Error[%u]: ", error);
        if      (error == OTA_AUTH_ERROR)    Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)   Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)     Serial.println("End Failed");
        
    });

    ArduinoOTA.onEnd([]() {
        otaInProgress = false;
        Serial.println("OTA End");
    });
    ArduinoOTA.begin();
    Serial.println("OTA Initialized");
  }

void checkOTA(){
    ArduinoOTA.handle();
  }
#endif

void reconnect() {
  delay(100);
  // Loop until we're reconnected to MQTT server
  while (!mqttclient.connected() && (watchdogM <= mqtt_timeout)) {
    mqttclient.clearWriteError();                   // Cleaning MQTT write buffer
    mqttclient.flush();                             // Cleaning MQTT data buffer
    mqttstatus = mqttclient.state();                // Decoding of MQTT status 
    Serial.println("");
    Serial.print("Attempting MQTT connection try Nr. ");
    Serial.println(watchdogM);
  
    const char *reason;
    switch (mqttstatus) {
      case -4 : {reason = "MQTT server didn't respond within the keepalive time"; break;}
      case -3 : {reason = "MQTT network connection was broken"; break;}
      case -2 : {reason = "MQTT network connection failed"; break;}
      case -1 : {reason = "MQTT client is disconnected cleanly"; break;}
      case  0 : {reason = "MQTT client is connected"; break;}
      case  1 : {reason = "MQTT server doesn't support the requested version of MQTT"; break;}
      case  2 : {reason = "MQTT server rejected the client identifier"; break;}
      case  3 : {reason = "MQTT server was unable to accept the connection"; break;}
      case  4 : {reason = "MQTT username/password were rejected"; break;}
      case  5 : {reason = "MQTT client was not authorized to connect"; break;}
      default: {   }                                // Wrong               
   }
    Serial.println(reason);
    if (mqttclient.connect(iamclient, mqtt_user, mqtt_password)) {
      Serial.print("MQTT connected as: ");
      Serial.println(iamclient);
      Serial.println("");
      watchdogM = 1;
      // Send status after start
      #if defined(TEST)                               // Send status after start
        mqttclient.publish(out_status, "1" ,false);
      #else
        mqttclient.publish(out_status, "MQTT Connected" ,false);
        delay(500);
      #endif
    }
    Serial.print("MQTT with RSSI: ");   Serial.println(WiFi.RSSI());
    Serial.println("Retry MQTT in 4 seconds ... ");
    // Wait 4 seconds before retrying with OTA-frindlye wait loop
    for (int i = 0; i < 40; i++) {
      delay(100);
      #if defined(OTA)
        checkOTA();
      #endif
    }
    watchdogM++;
    Serial.println("_");
    }
    if (watchdogM >= mqtt_timeout) {
    Serial.println("NO MQTT available -> RESTART");
    RestartDevice();                                  // REBOOT of the system !!!!!
  }
}

void callback(char* topic, uint8_t* payload, unsigned int length) {
  receivedlenght = length;
  Serial.println("");
  Serial.print("Message for [");
  Serial.print(topic);
  Serial.print("] arrived = L:(");
  Serial.print(length);
  Serial.print(")->");
  for (unsigned int i = 0; i < length; i++) {
    receivedChar[i] = payload[i];    
    Serial.print(receivedChar[i]);                  //     Received from mqtt text
  }
  Serial.println();
  #if defined(TEST)
      mqttclient.publish(out_status, "3" ,false);
    #else
      Sendme = "Command  ";
      Sendme = Sendme + (receivedChar[0]);
      Sendme = Sendme + " received";
      mqttclient.publish(out_status, (String(Sendme).c_str()) ,false);
      statusreset = true;
      delay(1000);
  #endif
 
  switch (receivedChar[0]) {          // Detecition what command is detected 
    case '?' : {                       // request parameters
     Sendme = "RSSI: ";
      Sendme = Sendme + (WiFi.RSSI());
      mqttclient.publish(out_param,   (String(Sendme).c_str()), false);       // Send the wifi signal strenght
      mqttclient.publish(out_sensors, (String(MySensors).c_str()), false);    // Send list of the usable sensors
      readvalues();
      scanI2Cbus();                                                           // Also show bus devices on Serial
      break;
    }
    // Received from mqtt new update rate 
    case 'U' : {     
      int Va; 
      Va = x2i(receivedChar,1,2);                                             
      if (Va <= 5){ Va = 5; };
      Sendme = "Update : ";
      Sendme = Sendme + (Va);
      mqttclient.publish(out_status, (String(Sendme).c_str()), false);        // Send the wifi signal strenght
      readinterval = Va *  500;
      sendinterval = Va * 1000;
      break;
    }
    // I2C Bus scan 
    case 'O' : {                       // restart device
      mqttclient.publish(out_status, "Serialprint I2C requested", false);
      scanI2Cbus();
      break;
      }
     // RESTART command 
    case 'X' : {                       // restart device
      mqttclient.publish(out_status, "Restart requested", false);
      RestartDevice();
      break;
      }
    default: {                        // Wrong command
      #if defined(TEST)
        mqttclient.publish(out_status, "-1" ,false);
      #else
        mqttclient.publish(out_status, "No valid command" ,false);
        statusreset = true;
        delay(1000);
      #endif
      break;
    }
  }
}

void RestartDevice(){
  mqttclient.publish(out_status, "Restart Device", false);
  Serial.println("Restart Device");
  delay(1000);
  mqttclient.disconnect();
  Serial.println("MQTT disconnect");
  delay(1000);
  WiFi.disconnect(true);                                                  // true = auch gespeicherte SSID löschen
  Serial.println("Wifi disconnect");
  delay(1000);
  ESP.restart();                                                          // REMOTE RESTART
}

void readvalues() {   
  MySensors = "S: " + String(numsensors) + "-> ";
  #if defined(enableDHT)          // Sensor reading DHT
    readDHT();   
  #endif                 
  #if defined(enableDS)           // Sensors reading DS
    readDS();   
  #endif                 
  #if defined(enableLM75)
    readLM75();                    // Sensor reading LM75
  #endif  
  #if defined(enableAHT)
    readAHT();                    // Sensor reading AHT
  #endif
  #if defined(enableMLX)
    readMLX();                    // Sensor reading MLX
  #endif 
  #if defined(enableAGS10)
    readAGS10();                  // Sensor reading AGS10
  #endif
  #if defined(enableCCS)
    readCCS();                    // Sensor reading CSS811
  #endif
  #if defined(enableENS)
    readENS();                    // Sensor reading ENS160
  #endif   
  #if defined(enableSGP30)
    readSGP30();                  // Sensor reading SGP30
  #endif   
  #if defined(enableSHT31)
    readSHT31();                  // Sensor reading SHT31
  #endif  
  #if defined(enableBMP18)
    readBMP18();                  // Sensor reading BMP180
  #endif
  #if defined(enableBMP28)
    readBMP28();                  // Sensor reading BMP280
  #endif
  #if defined(enableBME28)
    readBME28();                  // Sensor reading BME280
  #endif
  #if defined(enableBMP39)
    readBMP39();                  // Sensor reading BMP280
  #endif
   #if defined(enableVL53L0)
    readVL53L0();                 // Sensor reading tofl VL53
  #endif
  #if defined(enableVL6180)
    readVL6180();                 // Sensor reading tofl VL61
  #endif
  #if defined(enableAS56)
    readAS56();                   // Sensor reading angle AS5600
  #endif
  #if defined(enableBH17)
    readBH17();                   // Sensor reading light BH1750
  #endif  
  #if defined(enableVEML)
    readVEML();                   // Sensor reading light VEML7700
  #endif
  #if defined(enableLISD)
    readLISD();                   // Sensor reading acceleration LIS3DH
  #endif
  #if defined(enableLISM)
    readLISM();                   // Sensor reading magnet LIS3MDL
  #endif
  #if defined(enableMPU65)
    readMPU65();                   // Sensor reading 6 axis MPU 6500 board
  #endif
}

// Helper function for substring to hex conversion 
int x2i(const char *s, int from_a, int to_b) {
  char temp[33];                                    // Enough for 32-bit hex + null terminator
  int len = to_b - from_a + 1;
  strncpy(temp, s + from_a, len);
  temp[len] = '\0';
  return (int)strtol(temp, nullptr, 16);
}

// Helper function to identify I2C devices
void scanI2Cbus(){
  Serial.print("\nI2C devices: \t");
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0)
      Serial.printf(" 0x%02X\t", addr);
  }
  Serial.print("\nNo more I2C devices:\n");
}

// XXXXXXXXXXXXXXXXXXXXXXXX PROGRAM  START XXXXXXXXXXXXXXXXXXXXXXX

void setup() {
  // Initialize debug output and wifi and preset mqtt
  Serial.begin(115200);
  delay(1000);
  Serial.println("");
  Serial.println("");
  Serial.print("I am: ");
  Serial.print(iamclient);
  Serial.println("");

  setup_wifi(); // Start the wifi connection
  delay(100);
  #if defined(OTA)                                  
    setupOTA();                                     // Start OTA posibility
  #endif

  mqttclient.setClient(espClient);
  mqttclient.setServer(mqtt_server, mqtt_port);
  mqttclient.setCallback(callback);
  mqttclient.setKeepAlive(61);      // MQTT_KEEPALIVE : keepAlive interval in seconds. Override setKeepAlive()
  mqttclient.setSocketTimeout(63);  // MQTT_SOCKET_TIMEOUT: socket timeout interval in Seconds. Override setSocketTimeout()
  reconnect();                      // Start the mqtt connection would read first meassures with 0
  mqttclient.subscribe(in_topic);   // Listen to the mqtt inputs

  #if defined(enableWire)
    Wire.begin(SDA, SCL);
    Wire.setTimeOut(200); 
    delay(500);
    Serial.println("Wire started");
	  #if defined(TEST)
      scanI2Cbus();
    #endif
  #endif
  
  #if defined(enableDHT)        // Initialize the DHT sensor
    startDHT();
  #endif
  #if defined(enableDS)         // Initialize the DS sensors
    startDS();
  #endif
  #if defined(enableLM75)       // Initialize the VL6180 sensor
    startLM75();
  #endif
  #if defined(enableAHT)        // Initialize the AHT21 sensor
    startAHT();
  #endif
  #if defined(enableMLX)        // Initialize the MLX90614 sensor
    startMLX();
  #endif
  #if defined(enableAGS10)      // Initialize the AGS10 sensor
    startAGS10();
  #endif
  #if defined(enableCCS)        // Initialize the CCS811 sensor
    startCSS();
  #endif
  #if defined(enableENS)        // Initialize the ENS160 sensor
    startENS();
  #endif   
  #if defined(enableSGP30)      // Initialize the SGP30 sensor
    startSGP30();
  #endif  
  #if defined(enableSHT31)      // Initialize the SHT31 sensor
    startSHT31();
  #endif  
  #if defined(enableBMP18)      // Initialize the BMP180 sensor
    startBMP18();
  #endif  
  #if defined(enableBMP28)      // Initialize the BMP280 sensor
    startBMP28();
  #endif  
  #if defined(enableBME28)      // Initialize the BME280 sensor
    startBME28();
  #endif
  #if defined(enableBMP39)      // Initialize the BMP390 sensor
    startBMP39();
  #endif
  #if defined(enableVL53L0)     // Initialize the VL53L0 sensor
    startVL53L0();
  #endif
  #if defined(enableVL6180)     // Initialize the VL6180 sensor
    startVL6180();
  #endif
  #if defined(enableAS56)       // Initialize the AS5600 sensor
    startAS56();                  
  #endif
  #if defined(enableBH17)       // Initialize the BH1750 sensor
    startBH17();     
  #endif
  #if defined(enableVEML)       // Initialize the VEML7700 sensor
    startVEML();         
  #endif
  #if defined(enableLISD)       // Initialize the LIS3DH sensor
    startLISD();              
  #endif
  #if defined(enableLISM)       // Initialize the LIS3MDL sensor
    startLISM();                
  #endif
  #if defined(enableMPU65)       // Initialize the MPU 6050 board
    startMPU65();                 
  #endif

  numsensors = numtemp + numhum + numco2 + numtvoc + numpress + numdist + numdeg + numlight + numaccel + nummag;
  Serial.printf("\n%i Sensors for Temp: %i, Humid: %i, Co2: %i, Tvoc: %i, Press: %i, Dist: %i, Angle: %i, Light: %i, Acell.: %i Magn.: %i\n",
                numsensors, numtemp, numhum, numco2, numtvoc, numpress, numdist, numdeg, numlight, numaccel, nummag);
  mqttclient.publish(out_sensors, (String(MySensors).c_str()), false); // Send list of the usable sensors.
}

//  This is the Main loop
void loop() {                             
  #if defined(OTA)                                  // Augment  OTA posibility
    checkOTA();
  #endif
  if (otaInProgress) return;                        // ← Während OTA alles andere stoppen  

  unsigned long currentMillis = millis();
  // Every X number of seconds (interval = x milliseconds) it reads a new MQTT message
  if (currentMillis - prevRMQTTMillis >= readinterval) {
    prevRMQTTMillis = currentMillis;

    if (WiFi.status() == WL_CONNECTED) { Serial.print("+");}
    else {
      WiFi.begin(wifi_ssid, wifi_password);
      Serial.println("Try Wifi reconnect "); 
    }
    
    if (!mqttclient.connected()) {
      reconnect();                                  // In case no mqtt it will reconnect
      mqttclient.subscribe(in_topic);
      Serial.print("Go in loop with MQTT topic: " );
      Serial.println(in_topic);
      #if defined(TEST)
        mqttclient.publish(out_status, "2" ,false);
      #else
        mqttclient.publish(out_status, "Reconnected" ,false);
      #endif
      statusreset = true;
    }
    mqttclient.loop();                              // Request if there is a message
    watchdog = !watchdog;                           // Create toggeling watchdog signal
    mqttclient.publish(out_watchdog, String(watchdog).c_str() ,false); 

    if (statusreset){
      statusreset = false;
      delay(1000);
      #if defined(TEST)
        mqttclient.publish(out_status, "0" ,false);
      #else
        mqttclient.publish(out_status, "Normal" ,false);
      #endif
    }

    // Here are the sensordata send via mqtt.
    // It is intentional that a send is only done within receive interval. 
    // (To avoid unchecked connection adding with buffer problems)
    if (currentMillis - prevSMQTTMillis >= sendinterval) {
      prevSMQTTMillis = currentMillis;
      #if defined(TEST)
        Serial.printf("Free Heap: %d\n\n", ESP.getFreeHeap());  
      #endif
      readvalues();
/*
      Serial.println();
      #if defined(enableDHT)
        Serial.printf("T: %.1fC\tH: %0.f%%\t", temp, hum);
      #endif
      #if defined(enableDS)
        Serial.printf("T: %.1fC\t", temp);
      #endif
      #if defined(enableAHT)
        Serial.printf("T: %.1fC\tH: %0.f%%\t", temp, hum);
      #endif
      #if defined(enableCCS)
        Serial.printf("CO2: %uppm\tTVOC: %uppb\t", co2, tvoc);
      #endif
      #if defined(enableENS)
        Serial.printf("CO2: %uppm\tTVOC: %uppb\t", co2, tvoc);
      #endif 
      #if defined(enableVL53L0)
        Serial.printf("Distance: %umm\t%s\t", dist, qual);
      #endif    
      #if defined(enableVL6180)
        Serial.printf("Distance: %u\t", dist);
      #endif         
      Serial.printf("\n\n");    
      */
    }
  }
  #if defined(TEST)
  // mqttclient.publish(out_topic, String(looped).c_str() ,false); // Only use that when in doubts of loop speed
    looped++;
  #endif
  delay(50);
}