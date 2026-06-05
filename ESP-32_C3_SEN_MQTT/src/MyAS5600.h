// Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip

int AS56mynumdeg = 0;

#define enableWire

#include <AS5600.h>     // Seeed Studio runs without errors
// #define I2C_ADDRESS 0x36
#define enableWire
AMS_5600 as5600;

void startAS56(){
  int sensorReady = 0; int tried = 0;
  do {
    sensorReady = as5600.getAddress();
    if (sensorReady <= -1) {
      delay(35);
      #if defined(TEST)
        Serial.printf("\nAS5600 read address try %i", tried);
      #endif 
    }
    tried++; } 
  while ((sensorReady <= 1) && (tried < 10));
  if (sensorReady != 0x36 ){
    Serial.print("AS5600 ERROR\n");
    MySensors += "Error AS5600";} 
  else {
    Serial.printf("AS5600 sensor started\n");
    numdeg++;
    AS56mynumdeg = numdeg;
    MySensors += "AS5600  ";
    #if defined(TEST)
      Serial.print("AS5600 found at: "); Serial.println(as5600.getAddress());
    #endif
  }
}

void readAS56(){
  angle = as5600.getRawAngle() * 0.087890625;
  #if defined(TEST)
    Serial.printf("AS5600 Angle: %f deg with Magnitude: %f \n", angle,as5600.getMagnitude());
  #endif
  String numh = String(AS56mynumdeg - 1);              
  String topic = String(mqtt_out_sen) + "/deg/" + numh;
  mqttclient.publish(topic.c_str(), String(angle).c_str(), false);
  MySensors += "AS56L<a" + numh+ "> ";
}