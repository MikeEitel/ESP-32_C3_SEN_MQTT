#define enableWire

#include <DFRobot_CCS811.h>
DFRobot_CCS811 ccs811;
#define I2C_ADDRESS 0x5A

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
  }
}

void readCCS() {  
  if(ccs811.checkDataReady()){
    co2 = ccs811.getCO2PPM();
    tvoc= ccs811.getTVOCPPB();
    #if defined(TEST)
      Serial.print("CCS811  CO2: ");  Serial.print(co2);
      Serial.print(" ppm  and  TVOC: "); Serial.print(tvoc);Serial.println(" ppb");
    #endif
    mqttclient.publish(mqtt_out_co2, String(co2).c_str(), false);
    mqttclient.publish(mqtt_out_tvoc, String(tvoc).c_str(), false);
    MySensors = MySensors + " & CCS811"; 
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
