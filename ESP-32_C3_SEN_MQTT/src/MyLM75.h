// Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip

int LM75mynumtemp = 0;

#define enableWire

#include <LM75_derived.h>                     // Standard library for LM75
//#define I2C_ADDRESS 0x48
Generic_LM75 lm75(0x48);


void startLM75(){
  temp = lm75.readTemperatureC(); 
  Serial.printf("LM75 aktive %fC\n",temp);
  MySensors += "LM75  "; 
  numtemp++;
  LM75mynumtemp = numtemp;
}

void readLM75(){
  temp = lm75.readTemperatureC();
  if (isnan(temp)){
    #if defined(TEST)
      mqttclient.publish(out_status, "-8" ,false);
    #else
      mqttclient.publish(out_status, "LM75 Temperatur ERROR" ,false);
    #endif
    }
  else{ 
    // Publish temperature readings with unique MQTT topics per sensor
    String numh = String(LM75mynumtemp -1);              // Starting mqtt at /0
    String topic = String(mqtt_out_sen) + "/temp/" + numh;
    mqttclient.publish(topic.c_str(), String(temp).c_str(), false);
    MySensors += "LM<t" + numh + "> ";
  }

  #if defined(TEST)
    Serial.printf("LM75 T: %f C\n",temp);
  #endif
} 
