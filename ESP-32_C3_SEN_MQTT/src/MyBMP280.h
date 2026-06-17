    // Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip

int BMP28mynumpress = 0;
int BMP28mynumtemp = 0;

#define enableWire

#include <Adafruit_BMP280.h>
//#define I2C_ADDRESS 0x76
Adafruit_BMP280 bmp280;


void startBMP28() {
  int status, tried = 0;
  do {
    status = bmp280.begin(0x76, BMP280_CHIPID);
    Serial.printf("BMP280 with sensorID %i started after %d tries\n", bmp280.sensorID(), tried);
    if (status != 1) { 
      delay(50); 
    }
    tried++; } 
  while ((status != 1) && (tried < 10));
    
  if (status != 1) {
    Serial.printf("Failed to start BMP280 sensor after %d tries\n", tried);
    MySensors += "Error BMP280"; } 
  else {
    Serial.printf("BMP280 with sensorID %i started after %d tries\n", bmp280.sensorID(), tried);
    numpress++; numtemp++;
    BMP28mynumpress = numpress;
    BMP28mynumtemp = numtemp;
    MySensors += "BMP280  ";
    bmp280.setSampling (
    Adafruit_BMP280::MODE_NORMAL,  // Operating Mode
    Adafruit_BMP280::SAMPLING_X16,          // Temperature oversampling
    Adafruit_BMP280::SAMPLING_X16,          // Pressure oversampling
    Adafruit_BMP280::FILTER_X16,            // Filtering
    Adafruit_BMP280::STANDBY_MS_4000        // Standby time
    ); 
  }
}

void readBMP28(){
  int status, tried = 0;
  do {
    status = bmp280.getStatus();
    if (status != 1) { 
      delay(35);
      #if defined(TEST)
        Serial.printf("\nBMP280 read temperature try %i", tried);
      #endif 
    }
    tried++; } 
  while (!bmp280.getTemperatureSensor() && (tried < 10));
  temp = bmp280.readTemperature();
  tried = 0;
  do {
    status = bmp280.getStatus();
    if (status != 1) { 
      delay(35);
      #if defined(TEST)
        Serial.printf(" pressure read try %i\n", tried);
      #endif 
    }
    tried++; } 
  while (!bmp280.getPressureSensor() && (tried < 10));
  press = bmp280.readPressure()/ 100;  // change pascal to mBar

  String numh = String(BMP28mynumtemp-1);              
  String topic = String(mqtt_out_sen) + "/temp/" + numh;
  mqttclient.publish(topic.c_str(), String(temp).c_str(), false);
  MySensors += "BM28<t" + numh;

  numh = String(BMP28mynumpress-1);              
  topic = String(mqtt_out_sen) + "/press/" + numh;
  mqttclient.publish(topic.c_str(), String(press).c_str(), false);
  MySensors += "p" + numh + "> ";
  #if defined(TEST)
    Serial.printf("BMP280 Temp: %f C & Press: %f mbar\n", temp, press);
  #endif 

}
