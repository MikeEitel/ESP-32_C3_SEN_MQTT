// Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip

#define enableWire
int BH17mynumlight = 0;

#include <BH1750.h>                     // Standard library for BH17
//#define I2C_ADDRESS (nc) 0x23 (VCC) 0x5C
BH1750 bh17;


void startBH17(){
  bool status = false;
  int tried = 0;
  do {
    status = bh17.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23);
    #if defined(TEST)
      Serial.printf("SHT31 Status: %x\n", status);
    #endif
    if (status != 1) delay(500);
    tried++;
  } while ((!status) && (tried < 10));
  if (!status) {
    Serial.printf("Failed to start BH1750 sensor after %d tries\n", tried);
    MySensors += "Error BH1750";
  } else {
    Serial.printf("BH1750 sensor started after %d tries\n", tried);
    numlight++;
    BH17mynumlight = numlight;
    MySensors += "BH1750  ";
  }
}

void readBH17(){
  if (!bh17.measurementReady()) {
    mqttclient.publish(out_status, "BH1750 Error" ,false);
    Serial.print("BH1750 Read error\n");}
  else {
    light = bh17.readLightLevel();
    #if defined(TEST)
      Serial.printf("BH1750 light: %f\n", light);
    #endif
    // Publish readings
    String numh = String(BH17mynumlight -1);              
    String topic = String(mqtt_out_sen) + "/light/" + numh;
    mqttclient.publish(topic.c_str(), String(light).c_str(), false);
    MySensors += "BH17<l" + numh + "> ";
  }
}
