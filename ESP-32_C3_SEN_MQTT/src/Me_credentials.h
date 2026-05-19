/*Made by Mike Eitel to store device credenitals on one place
  No rights reserved when private use. Otherwise contact author.

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
  and associated documentation files. The above copyright notice and this permission notice shall 
  be included in all copies or substantial portions of the software.
*/

// Wifi acess definitions of network to connect to  
#include      <..\..\Networksecrets.h>                  // This file contains the secret private network credentials

#define MyIP "033"                                      // Device ID

//Wifi definitions of this device 
IPAddress staticIP(192,168,167,atoi(MyIP));             // This device has IP
IPAddress subnet(255,255,255,0);                        // Network subnet size
IPAddress gateway(192,168,167,1);                       // IOT Network router IP

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
#define mqtt_out_hum0   iamclient "/sen/hum/0"          // This is send to MQTT
#define mqtt_out_hum1   iamclient "/sen/hum/1"          // This is send to MQTT
#define mqtt_out_tem0   iamclient "/sen/temp/0"         // This is send to MQTT
#define mqtt_out_tem1   iamclient "/sen/temp/1"         // This is send to MQTT
#define mqtt_out_tem2   iamclient "/sen/temp/2"         // This is send to MQTT
#define mqtt_out_tem3   iamclient "/sen/temp/3"         // This is send to MQTT
#define mqtt_out_tem4   iamclient "/sen/temp/4"         // This is send to MQTT
#define mqtt_out_tem5   iamclient "/sen/temp/5"         // This is send to MQTT
#define mqtt_out_tem6   iamclient "/sen/temp/6"         // This is send to MQTT
#define mqtt_out_tem7   iamclient "/sen/temp/7"         // This is send to MQTT
#define mqtt_out_tem8   iamclient "/sen/temp/8"         // This is send to MQTT
#define mqtt_out_tem9   iamclient "/sen/temp/9"         // This is send to MQTT
#define mqtt_out_angle  iamclient "/sen/angle"          // Angle position is send to MQTT
#define mqtt_out_co2    iamclient "/sen/co2"            // eCO2 air quality is send to MQTT
#define mqtt_out_tvoc   iamclient "/sen/tvoc"           // TVOC air quality is send to MQTT
#define mqtt_out_bmppr  iamclient "/sen/press"          // Air pressure is send to MQTT
#define mqtt_out_dist   iamclient "/sen/dist"           // Distance is send to MQTT
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

#define enableDHT                                       // Enable DHT sensor      
#define enableDS                                        // Enable DS sensors      
#define enableAHT                                       // Enable AHT21 T&H sensor  
#define enableCCS                                       // Enable CCS811 tvoc sensor  
#define enableENS                                       // Enable ENS160 tvoc sensor  
#define enableVL53L0                                    // Enable LV53L0X tofl sensor
#define enableVL6180                                    // Enable VL6180X tofl sensor


float humDHTcorr =        0.0;                          // Humidity DHT sensor error correction
float humAHTcorr =        0.0;                          // Humidity AHT sensor error correction
float temDHTcorr =        0.0;                          // Temperatur DHT sensor error correction
float temAHTcorr =       -6.0;                          // Temperatur AHT sensor error correction

// Constant how often the mqtt message is send
#if defined(TEST)
  long readinterval =  3000;                            // Interval at which device does MQTT reading
  long sendinterval =  6000;                            // Interval at which sensor data is send via mqtt
#else
  long readinterval = 3000; //30000;                            // Interval at which to publish sensor readings
  long sendinterval = 6000; //449000;                           // Interval at which sensor data is send via mqtt 
#endif