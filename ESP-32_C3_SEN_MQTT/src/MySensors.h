#ifndef MYSENSORS_H
#define MYSENSORS_H


#if defined(enableDHT)                        // One wire based DHT temperatur and humidity sensors
  #include "MyDHTxx.h"
#endif
#if defined(enableDS)                        // One wire based DS temperatur  sensors
  #include "MyDSx.h"
#endif
#if defined(enableAHT)                        // Temperatur and humidity sensor
  #include "MyAHT2x.h"
#endif  
#if defined(enableCCS)                        // TVOC and C02 sensor via CJMCU-811 board
  #include "MyCCS811.h"
 #endif  
#if defined(enableENS)                         // TVOC and C02 multigas sensor
  #include "MyENS160.h"
#endif  
#if defined(enableVL53L0)                      // TOFL sensor ca. 40 - 500
  #include "MyVL53L0X.h"
#endif
#if defined(enableVL6180)                      // TOFL sensor ca. 40 - 255
  #include "MyVL6180.h"
#endif
#if defined(enableLM75)                        // Temperatur sensor
  #include "LM75_Derived.h"
#endif

void readvalues() {   
  MySensors = "";
  #if defined(enableDHT)          // Sensor reading DHT
    readDHT();   
  #endif                 
  #if defined(enableDS)           // Sensors reading DS
    readDS();   
  #endif                 
  #if defined(enableAHT)
    readAHT();                    // Sensor reading AHT
  #endif
  #if defined(enableCCS)
    readCCS();                    // Sensor reading tcov CCS
  #endif
  #if defined(enableENS)
    readENS();                    // Sensor reading tcov ENS
  #endif
  #if defined(enableVL53L0)
    readVL53L0();                 // Sensor reading tofl VL53
  #endif
  #if defined(enableVL6180)
    readVL6180();                 // Sensor reading tofl VL61
  #endif
  
  #if defined(enableLM75)         // Temperatur sensor LM75
    readLM75();
  #endif
}





#endif // MYSENSORS_H