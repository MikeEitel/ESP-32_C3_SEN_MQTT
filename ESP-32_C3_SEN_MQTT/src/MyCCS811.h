// Used pins
// GND            GND
// 3V3            Internal 3.3V
//    IMPORTANT   The WAK pin must be connected to GND
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip

#define enableWire
int CSSmynumco2 = 0;
int CSSmynumtvoc = 0;

#include <DFRobot_CCS811.h>
DFRobot_CCS811 ccs811;
//#define I2C_ADDRESS 0x5A

void startCSS(){
  int status, tried = 0;
  do {
    status = ccs811.begin();
    if (status != 1) {
      delay(50);
    }
    tried++;
  } while ((status != 0) && (tried < 10));

  if (status != 0) {
    Serial.printf("Failed to start CCS811 sensor after %d tries\n", tried);
    MySensors += "Error CCS811";
  } else {
    Serial.printf("CCS811 sensor started after %d tries\n", tried);
    ccs811.checkDataReady();
    numco2++; numtvoc++;
    CSSmynumco2 = numco2;
    CSSmynumtvoc = numtvoc;
    MySensors += "CSS811  ";
  }
}

void readCCS() {  
  if(ccs811.checkDataReady()){
    co2 = ccs811.getCO2PPM();
    tvoc= ccs811.getTVOCPPB();
    #if defined(TEST)
      Serial.printf("CCS811 CO2: %i ppm  and TVOC: %i  ppb\n", co2, tvoc);
    #endif
    // Publish readings
    String numh = String(CSSmynumco2 -1);              
    String topic = String(mqtt_out_sen) + "/co2/" + numh;
    mqttclient.publish(topic.c_str(), String(co2).c_str(), false);
    MySensors += "CSS<co" + numh;
    numh = String(CSSmynumtvoc -1);              
    topic = String(mqtt_out_sen) + "/tvoc/" + numh;
    mqttclient.publish(topic.c_str(), String(tvoc).c_str(), false);
    MySensors += "tv" + numh + "> ";
  }
  else { 
    #if defined(TEST)
      mqttclient.publish(out_status, "-5" ,false);
    #else
      Serial.println("CCS811 read ERROR!");
      mqttclient.publish(out_status, "CCS811 ERROR" ,false);
    #endif
  }
}
