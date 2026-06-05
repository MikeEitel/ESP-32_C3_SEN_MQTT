// Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip

int AGS10mynumtvoc = 0;

#define enableWire

#include <MeAGS10.h>
Me_AGS10 ags10 = Me_AGS10();
//    #define I2C_ADDRESS 0x1A


void startAGS10(){
  ags10.begin();
  if (ags10.readVersion() <= 1){
    Serial.print("AGS10 ERROR\n");
    MySensors += "Error AGS10";} 
  else {
    Serial.printf("AGS10 sensor started\n");
    numtvoc++;
    AGS10mynumtvoc = numtvoc;
    MySensors += "AGS10  ";
  }
}

void readAGS10(){
  tvoc = ags10.readTVOC();
  String numh = String(AGS10mynumtvoc -1);              
  String topic = String(mqtt_out_sen) + "/tvoc/" + numh;
  mqttclient.publish(topic.c_str(), String(tvoc).c_str(), false);
  MySensors += "AG<tv" + numh + "> ";
  #if defined(TEST)
    Serial.printf("AGS10 TVOC: %i\n",tvoc);
  #endif
}