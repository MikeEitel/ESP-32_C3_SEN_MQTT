// Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip

#define enableWire
int VEMLmynumlight = 0;

#include "Adafruit_VEML7700.h"                    // Standard library for VEML7700
//#define I2C_ADDRESS 0x10
Adafruit_VEML7700 veml = Adafruit_VEML7700();


void startVEML(){
  bool status = false;
  int tried = 0;
  do {
    status = veml.begin();
    #if defined(TEST)
      Serial.printf("VEML7700 Status: %u \n",status);
    #endif  
    if (status != 1) delay(500);
      tried++;
  } while ((!status) && (tried < 10));
  if (!status) {
    Serial.printf("Failed to start VEML7700 sensor after %d tries\n", tried);
    MySensors += "Error VEML7700";
  } else {
    Serial.printf("VEML7700 sensor started after %d tries\n", tried);
    numlight++;
    VEMLmynumlight = numlight;
    MySensors += "VEML7700  ";
  }
}



void readVEML(){
  String qual = " ", q1 = " ", q2 = " ";
  if (veml.readALS(true)==0){
    mqttclient.publish(out_status, "VEML7700 Error" ,false);
    Serial.print("VEML7700 Read error\n");}
  else {
    light = veml.readLux(VEML_LUX_AUTO);
    #if defined(TEST)
      Serial.printf("VEML7700 light: %f\n", light);
    #endif
    // Publish readings
    String numh = String(VEMLmynumlight -1);              
    String topic = String(mqtt_out_sen) + "/light/" + numh;
    mqttclient.publish(topic.c_str(), String(light).c_str(), false);
    MySensors += "VEML<l" + numh + "> ";
    switch (veml.getGain()) {
      case VEML7700_GAIN_1:   q1= "1";    break;
      case VEML7700_GAIN_2:   q1= "2";    break;
      case VEML7700_GAIN_1_4: q1= "1/4";  break;
      case VEML7700_GAIN_1_8: q1= "1/8";  break;
      }
    switch (veml.getIntegrationTime()) {
      case VEML7700_IT_25MS:  q2=  "25"; break;
      case VEML7700_IT_50MS:  q2=  "50"; break;
      case VEML7700_IT_100MS: q2= "100"; break;
      case VEML7700_IT_200MS: q2= "200"; break;
      case VEML7700_IT_400MS: q2= "400"; break;
      case VEML7700_IT_800MS: q2= "800"; break;
      }
    #if defined(TEST)
      Serial.printf("\nGain: %s  Integration Time: %s ms Value: %f", q1, q2, light);
    #endif
    qual = "VEML7700 Gain: " + q1 + " time: "+ q2;
    String topic2 = String(mqtt_out_sen) + "/diqu/" + numh;
    mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
  }
}