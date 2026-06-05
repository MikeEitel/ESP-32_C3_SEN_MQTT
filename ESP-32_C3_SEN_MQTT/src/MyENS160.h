// Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip

#define enableWire
int ENSmynumco2 = 0 ;
int ENSmynumtvoc = 0 ;

#include <SparkFun_ENS160.h> // ENS160 library
SparkFun_ENS160 ens; //0x53..ENS160+AHT21
//#define I2C_ADDRESS 0x53

void startENS(){
  int status, tried = 0;
  do {
    status = ens.begin(0x53);
    // Serial.printf("ENS Status: %u \n",status);
    if (status != 1) delay(500);
      tried++;
  } while ((status != 1) && (tried < 10));

  if (status != 1) {
    Serial.printf("Failed to start ENS160 sensor after %d tries\n", tried);
    MySensors += "Error ENS160";
  } else {
    Serial.printf("ENS160 sensor started after %d tries\n", tried);
    numco2++; numtvoc++;
    ENSmynumco2 = numco2;
    ENSmynumtvoc = numtvoc;
    MySensors += "& ENS ";
  }
  ens.setOperatingMode(SFE_ENS160_STANDARD);
}

void readENS(){  
  if (ENScortemp != -1){
    ens.setTempCompensationCelsius(ENScortemp); 
    ens.setRHCompensationFloat(ENScorhum);
    #if defined(TEST)  
      snprintf(debug_buf, sizeof(debug_buf), "ENS160 Modus: %d\tFlags: %d\n", ens.getOperatingMode(), ens.getFlags());
      mqttclient.publish(mqtt_debug, debug_buf, false);
    #endif
  }
  if (ens.getFlags() == 0xff) {
    mqttclient.publish(out_status, "ENS160 Error" ,false);
  }
  if (ens.getFlags()!=3) {
    co2 = ens.getECO2();
    tvoc = ens.getTVOC();  
    #if defined(TEST)
      Serial.printf("ENS160 Modus: %d\tFlags: %d\teCO2: %u\tTVOC: %u\n",ens.getOperatingMode(),ens.getFlags(), co2, tvoc);
      //Serial.printf("ENS160 eCO2: %u\tTVOC: %u\n", co2, tvoc);
    #endif
    // Publish readings
    String numh = String(ENSmynumco2 -1);              
    String topic = String(mqtt_out_sen) + "/co2/" + numh;
    mqttclient.publish(topic.c_str(), String(co2).c_str(), false);
    MySensors += "ENS160<co" + numh;
    numh = String(ENSmynumtvoc -1);              
    topic = String(mqtt_out_sen) + "/tvoc/" + numh;
    mqttclient.publish(topic.c_str(), String(tvoc).c_str(), false);
    MySensors += "tv" + numh + "> ";
  }
  else { 
    #if defined(TEST)
      mqttclient.publish(out_status, "-5" ,false);
    #else
      Serial.println("ENS160 read ERROR!");
      mqttclient.publish(out_status, "ENS160 ERROR" ,false);
    #endif
  }
}
