/*Made by Mike Eitel to store device credenitals on one place
  No rights reserved when private use. Otherwise contact author.

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
  and associated documentation files. The above copyright notice and this permission notice shall 
  be included in all copies or substantial portions of the software.
*/

// Wifi acess definitions of network to connect to  
#define wifi_ssid "xxx"
#define wifi_password "yyy"

#define MyIP "033"                                      // Device ID

//Wifi definitions of this device 
//IPAddress staticIP(192,168,x,atoi(myIP));            // IOT device IP
IPAddress staticIP(1,1,1,atoi(MyIP));      // REPLACE !!!!
IPAddress subnet(255,255,255,0);                        // Network subnet size
//IPAddress gateway(192,168,x,y);                       // Network router IP
IPAddress gateway(1,1,1,1);                // REPLACE !!!!

// Raspberri Pi Mosquitto MQTT Broker definitions
#define mqtt_server    "192.168.167.20"                 // IOT MQTT server IP
#define mqtt_user      "admin"
#define mqtt_password  "admin"
#define mqtt_port      1883
#define WiFi_timeout    101                             // How many times to try before give up
#define mqtt_timeout     11                             // How many times to try before try Wifi reconnect

// MQTT Topics
#define mytype          "esp/C3-SEN-"                   // Client Typ
#define iamclient       mytype MyIP                     // Client name 
#define in_topic        iamclient "/command"            // This common input is received from MQTT
#define out_param       iamclient "/signal"             // Wifi signal strength is send to MQTT
#define out_status      iamclient "/status"             // This is a general message send to MQTT
#define out_watchdog    iamclient "/watchdog"           // A watchdog bit send to MQTT
#define out_sensors     iamclient "/sensors"            // This is a list of usable external sensors send to MQTT
#define out_topic       iamclient "/loop"               // This helper debug variable can be send to MQTT
#define mqtt_debug      iamclient "/debug"              // This is send to MQTT   Debug only
#define mqtt_out_sen    iamclient "/sen"                // This is a generic starter descriptor to send to MQTT 
#define mqtt_out_qual   iamclient "/sen/qual"           // Measurement quality is send to MQTT

// Errors send as values in test mode
// error =  -7    
// error =  -6    No ATH
// error =  -5    No tvoc sensors
// error =  -4    No DS sensors
// error =  -3    Humidity sensor not read
// error =  -2    Temperatur sensor not read
// error =  -1    Wrong command received
// error =   0    Normal status
// error =   1    MQTT first time connected
// error =   2    MQTT Reconnect succesfull
// error =   3    Command received

// Allowed are:  DHT 11  or   DHT 12  or   DHT 22  (AM2302), AM2321   or   DHT 21 (AM2301)
#define DHTtyp   DHT22                                  // See above

#define enableDHT                                       // Enable DHT T&H sensor      
#define enableDS                                        // Enable multiple DS18 T sensors      
#define enableLM75                                      // I2C_ADDRESS 0x48   Enable LM75 T sensor  
#define enableAHT                                       // I2C_ADDRESS 0x38   Enable AHT21 T&H sensor  
#define enableSHT31                                     // I2C_ADDRESS 0x44   Enable SHT31 T&H sensor  
#define enableMLX                                       // I2C_ADDRESS 0x5A   Enable MLX90614 nontouch T sensor
#define enableAGS10                                     // I2C_ADDRESS 0x1A   Enable AGS10 tvoc sensor  
#define enableCCS                                       // I2C_ADDRESS (nc)-0x5A (VCC)-0x5B   Enable CCS811 tvoc sensor  
//#define enableENS                                       // I2C_ADDRESS 0x53   Enable ENS160 tvoc sensor  
//#define enableSGP30                                     // I2C_ADDRESS 0x58   Enable SPG30 tvoc sensor  ATTENTION !!! SENSIBLE TO BUS LOAD
//#define enableBMP18   // EITHER BMP/E-3xx               // I2C_ADDRESS 0x77   Enable BMP180 T & Baropress sensor  
#define enableBMP39   // OR BMP 180                     // I2C_ADDRESS 0x77   Enable BMP/E-3XX T&H & Baropress sensor  
//#define enableBMP28   // EITHER BMP 280                 // I2C_ADDRESS 0x76   Enable BMP280 T & Baropress sensor  
#define enableBME28   // OR BME 280                     // I2C_ADDRESS 0x76   Enable BME280 T&H & Baropress sensor  
#define enableVL53L0                                    // I2C_ADDRESS 0x29   Enable LV53L0X tofl sensor
//#define enableVL6180  // EITHER AS5600                  // I2C_ADDRESS 0x36   Enable VL6180X tofl sensor
#define enableAS56    // OR VL6180                      // I2C_ADDRESS 0x36   Enable AS5600 angle sensor
#define enableBH17                                      // I2C_ADDRESS (nc)-0x23 (VCC)-0x5C   Light sensor BH1750                     
#define enableVEML                                      // I2C_ADDRESS 0x10   Light sensor VEML7700
#define enableMPU65                                     // I2C_ADDRESS (nc)-0x68 (VCC)-0x69  6 axis sensor board MPU6500

// Not succesfull tested
//#define enableLISM                                      // I2C_ADDRESS 0x1C  Enable magnet sensor LIS3MDL
//#define enableLISD                                      // I2C_ADDRESS 0x19  Enable acceleration sensor LIS3DTHR
//

float humDHTcorr =        0.0;                          // Humidity DHT sensor error correction
float humAHTcorr =        0.0;                          // Humidity AHT sensor error correction
float temDHTcorr =        0.0;                          // Temperatur DHT sensor error correction
float temAHTcorr =        0.0;                          // Temperatur AHT sensor error correction

// Constant how often the mqtt message is send
#if defined(TEST)
  long readinterval =  3000;                            // Interval at which device does MQTT reading
  long sendinterval =  6000;                            // Interval at which sensor data is send via mqtt
#else
  long readinterval = 3000; //30000;                            // Interval at which to publish sensor readings
  long sendinterval = 6000; //449000;                           // Interval at which sensor data is send via mqtt 
#endif