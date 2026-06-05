    // Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip

int BME28mynumpress = 0;
int BME28mynumtemp = 0;
int BME28mynumhum = 0;

#define enableWire

#include <Adafruit_BME280.h>
//#define I2C_ADDRESS 0x76
Adafruit_BME280 bme280;


void startBME28() {
  int status, tried = 0;
  do {
    status = bme280.begin(0x76);
    if (status != 1) { 
      delay(50); 
    }
    tried++; } 
  while ((status != 1) && (tried < 10));
    
  if (status != 1) {
    Serial.printf("Failed to start BME280 sensor after %d tries\n", tried);
    MySensors += "Error BME280"; } 
  else {
    Serial.printf("BME280 with sensorID %i started after %d tries\n", bme280.sensorID(), tried);
    numpress++; numtemp++; numhum++;
    BME28mynumpress = numpress;
    BME28mynumtemp = numtemp;
    BME28mynumhum = numhum;
    MySensors += "BME280  ";
    bme280.setSampling (
      Adafruit_BME280::MODE_FORCED,
      Adafruit_BME280::SAMPLING_X1, // temperature
      Adafruit_BME280::SAMPLING_X1, // pressure
      Adafruit_BME280::SAMPLING_X1, // humidity
      Adafruit_BME280::FILTER_OFF 
    ); 
  }
}

void readBME28(){
  temp = bme280.readTemperature();
  press = bme280.readPressure()/ 100;  // change pascal to mBar
  hum = bme280.readHumidity();

  String numh = String(BME28mynumtemp-1);              
  String topic = String(mqtt_out_sen) + "/temp/" + numh;
  mqttclient.publish(topic.c_str(), String(temp).c_str(), false);
  MySensors += "BME280<t" + numh;

  numh = String(BME28mynumpress-1);              
  topic = String(mqtt_out_sen) + "/press/" + numh;
  mqttclient.publish(topic.c_str(), String(press).c_str(), false);
  MySensors += "p" + numh;

  numh = String(BME28mynumhum-1);              
  topic = String(mqtt_out_sen) + "/hum/" + numh;
  mqttclient.publish(topic.c_str(), String(hum).c_str(), false);
  
  MySensors += "h" + numh + "> ";
  #if defined(TEST)
    Serial.printf("BME280 Temp: %f C & Humid: %f %% & Press: %f mbar\n", temp, hum, press);
  #endif 

}
