#define enableWire

#include <AHTxx.h>
AHTxx aht(AHTXX_ADDRESS_X38, AHT2x_SENSOR);; //sensor address, sensor type
//#define I2C_ADDRESS 0x38

void startAHT() {
  int status, tried = 0;

  do {
    status = aht.begin();
    if (status != 1) {
      delay(50);
    }
    tried++;
  } while ((status != 1) && (tried < 10));

  if (status != 1) {
    Serial.printf("Failed to start AHT21 sensor after %d tries\n", tried);
    MySensors += "Error AHT21";
  } else {
    Serial.printf("AHT21 sensor started after %d tries\n", tried);
  }
}

void readAHT(){
  //Serial.printf("AHT21 Status: %d\n",aht.getStatus());
  if (!aht.getStatus()) {
    hum = aht.readHumidity();
    hum = hum + humAHTcorr;     
    temp = aht.readTemperature();
    temp = temp + temAHTcorr;
    #if defined(TEST)
      Serial.printf("AHT21 Temp: %f  Hum: %f\n", temp, hum);
    #endif
    mqttclient.publish(mqtt_out_hum0, String(hum).c_str(), false);
    mqttclient.publish(mqtt_out_tem0, String(temp).c_str(), false);
    MySensors = MySensors + " & " + "AHT21"; 
  }
  else { 
    #if defined(TEST)
      mqttclient.publish(out_status, "-6" ,false);
    #else
      Serial.println("AHT21 read ERROR!");
      mqttclient.publish(out_status, "AHT10 ERROR" ,false);
    #endif
  }
}