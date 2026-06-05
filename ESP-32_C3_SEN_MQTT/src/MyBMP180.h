// Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip

int BMP18mynumpress = 0;
int BMP18mynumtemp = 0;

#define enableWire

#include <BMP180I2C.h>
//#define I2C_ADDRESS 0x77
BMP180I2C bmp180(0x77);


void startBMP18() {
  int status, tried = 0;
  do {
    status = bmp180.begin();
    if (status != 1) { 
      delay(50); 
    }
    tried++; } 
  while ((status != 1) && (tried < 10));
    
  if (status != 1) {
    Serial.printf("Failed to start BMP180 sensor after %d tries\n", tried);
    MySensors += "Error BMP180"; } 
  else {
    Serial.printf("BMP180 sensor started after %d tries\n", tried);
    numpress++; numtemp++;
    BMP18mynumpress = numpress;
    BMP18mynumtemp = numtemp;
    MySensors += "BMP180  ";
    bmp180.resetToDefaults();
    bmp180.setSamplingMode(BMP180MI::MODE_UHR);
  }
}

void readBMP18() {
  int status, tried = 0;
  do {
    status = bmp180.measureTemperature();
    if (status != 1) { 
      delay(35);
      #if defined(TEST)
        Serial.printf("\nBMP180 read temperature try %i", tried);
      #endif 
    }
    tried++; } 
  while (!bmp180.hasValue() && (tried < 10));
  temp = bmp180.getTemperature();
  tried = 0;
  do {
    status = bmp180.measurePressure();
    if (status != 1) { 
      delay(35);
      #if defined(TEST)
        Serial.printf(" pressure read try %i\n", tried);
      #endif 
    }
    tried++; } 
  while (!bmp180.hasValue() && (tried < 10));
  press = bmp180.getPressure()/ 100;  // change pascal to mBar

  String numh = String(BMP18mynumtemp-1);              
  String topic = String(mqtt_out_sen) + "/temp/" + numh;
  mqttclient.publish(topic.c_str(), String(temp).c_str(), false);
  MySensors += "B18<t" + numh;

  numh = String(BMP18mynumpress-1);              
  topic = String(mqtt_out_sen) + "/press/" + numh;
  mqttclient.publish(topic.c_str(), String(press).c_str(), false);
  MySensors += "p" + numh + "> ";
  #if defined(TEST)
    Serial.printf("BMP180 Temp: %f C & Press: %f mbar\n", temp, press);
  #endif 
}