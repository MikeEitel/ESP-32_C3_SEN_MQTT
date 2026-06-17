// Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip
//

#define enableWire
int SHTmynumtemp = 0 ;
int SHTmynumhum = 0 ;

#include "Adafruit_SHT31.h"
// #define I2C_ADDRESS 0x44
Adafruit_SHT31 sht = Adafruit_SHT31();

void startSHT31(){
  bool status = false;
  int tried = 0;
  do {
    status = sht.begin();
    #if defined(TEST)
      Serial.printf("SHT Startstatus: %u \n",status);
    #endif
    if (status != 1) delay(500);
      tried++;
  } while ((!status) && (tried < 10));
  if (!status) {
    Serial.printf("Failed to start SHT31 sensor after %d tries\n", tried);
    MySensors += "Error SHT31";
  } else {
    Serial.printf("SHT31 sensor started after %d tries\n", tried);
    numtemp++; numhum++;
    SHTmynumtemp = numtemp;
    SHTmynumhum = numhum;
    MySensors += "SHT31  ";
  }
  #if defined(TEST)
    Serial.printf("SHT31 Status: %x\n", sht.readStatus());
  #endif  
}

void readSHT31(){
  temp = sht.readTemperature();
  hum = sht.readHumidity();
  if (isnan(temp) || isnan(hum))  {
    mqttclient.publish(out_status, "SHT31 Error" ,false);
    Serial.print("SHT Read error\n"); }
  else {
    #if defined(TEST)
      Serial.printf("SHT31 Temp: %f\tHumidity: %f\n", temp, hum);
    #endif
    // Publish readings
    String numh = String(SHTmynumtemp -1);              
    String topic = String(mqtt_out_sen) + "/temp/" + numh;
    mqttclient.publish(topic.c_str(), String(temp).c_str(), false);
    MySensors += "SHT<t" + numh;
    numh = String(SHTmynumhum -1);              
    topic = String(mqtt_out_sen) + "/hum/" + numh;
    mqttclient.publish(topic.c_str(), String(hum).c_str(), false);
    MySensors += "h" + numh + "> ";
  }

}
