#define enableWire

#include <SparkFun_ENS160.h> // ENS160 library
SparkFun_ENS160 ens; //0x53..ENS160+AHT21
//#define I2C_ADDRESS 0x53

void startENS(){
  int status, tryed = 0;
  do {
    status = ens.begin(0x53);
    // Serial.printf("ENS Status: %u \n",status);
    if (status != 1) delay(500);
      tryed++;
  } while ((status != 1) && (tryed < 10));

  if (status != 1) {
    Serial.printf("Failed to start ENS160 sensor after %d tries\n", tryed);
    MySensors += "Error ENS160";
  } else {
    Serial.printf("ENS160 sensor started after %d tries\n", tryed);
  }
  ens.setOperatingMode(SFE_ENS160_STANDARD);
}

void readENS(){  
  if (temp != -1){
    ens.setTempCompensationCelsius(temp); 
    ens.setRHCompensationFloat(hum);
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
    mqttclient.publish(mqtt_out_co2, String(co2).c_str(), false);
    mqttclient.publish(mqtt_out_tvoc, String(tvoc).c_str(), false);
    MySensors = MySensors + " & ENS160"; 
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
