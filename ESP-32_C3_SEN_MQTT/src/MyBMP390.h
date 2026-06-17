// Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip

int BMP39mynumpress = 0;
int BMP39mynumtemp = 0;

#define enableWire

#include <Adafruit_I2CDevice.h>
#include <Adafruit_BMP3XX.h>
//#define I2C_ADDRESS 0x77
Adafruit_BMP3XX bmp390;


void startBMP39() {
  int status, tried = 0;
  do {
    status = bmp390.begin_I2C();
    if (status != 1) {
      delay(50); 
    }
    tried++; } 
  while ((status != 1) && (tried < 10));
    
  if (status != 1) {
    Serial.printf("Failed to start BMP390 sensor after %d tries\n", tried);
    MySensors += "Error BMP390"; } 
  else {
    Serial.printf("BMP390 sensor started after %d tries\n", tried);
    numpress++; numtemp++;
    BMP39mynumpress = numpress;
    BMP39mynumtemp = numtemp;
    MySensors += "BMP390  ";
    bmp390.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp390.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp390.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp390.setOutputDataRate(BMP3_ODR_50_HZ);
  }
}

void readBMP39() {
  int status, tried = 0;
  do {
    status = bmp390.performReading();
    if (status != 1) { 
      delay(35);
      #if defined(TEST)
        Serial.printf("\nBMP390 reading try %i", tried);
      #endif 
    }
    tried++; 
  } 
  while (!bmp390.performReading() && (tried < 10));
  temp = bmp390.readTemperature();
  press = bmp390.readPressure()/ 100;  // change pascal to mBar

  String numh = String(BMP39mynumtemp-1);              
  String topic = String(mqtt_out_sen) + "/temp/" + numh;
  mqttclient.publish(topic.c_str(), String(temp).c_str(), false);
  MySensors += "BM39<t" + numh;

  numh = String(BMP39mynumpress-1);              
  topic = String(mqtt_out_sen) + "/press/" + numh;
  mqttclient.publish(topic.c_str(), String(press).c_str(), false);
  MySensors += "p" + numh + "> ";
  #if defined(TEST)
    Serial.printf("BMP390 Temp: %f C & Press: %f mbar\n", temp, press);
  #endif 
}