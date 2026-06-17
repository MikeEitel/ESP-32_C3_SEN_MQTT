// Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip
//
// THIS SENSOR HAS NOT A GOOD FANOUT. NOT USABLE WITH LARGER NUMBER OF I2C DEVICES
//


#define enableWire
int SGPmynumco2 = 0 ;
int SGPmynumtvoc = 0 ;

#include <SparkFun_SGP30.h> // SGP130 library
SGP30 sgp; 
//#define I2C_ADDRESS 0x58

void startSGP30(){
  bool status = false;
  int tried = 0;
  do {
    status = sgp.begin();
    Serial.printf("SGP Status: %u \n",status);
    if (status != 1) delay(500);
      tried++;
  } while ((!status) && (tried < 10));
  if (!status) {
    Serial.printf("Failed to start SGP30 sensor after %d tries\n", tried);
    MySensors += "Error SGP30";
  } else {
    Serial.printf("SGP30 sensor %llX started after %d tries\n", sgp.serialID, tried);
    numco2++; numtvoc++;
    SGPmynumco2 = numco2;
    SGPmynumtvoc = numtvoc;
    MySensors += "SGP30  ";
  }
  sgp.initAirQuality();
}

void readSGP30(){
  if (sgp.measureAirQuality()){
    mqttclient.publish(out_status, "SGP30 Error" ,false);
    Serial.print("SGP Read error\n");}
  else {
    co2 = sgp.CO2;
    tvoc = sgp.TVOC; 
    #if defined(TEST)
      Serial.printf("SGP30 eCO2: %u\tTVOC: %u\n", co2, tvoc);
    #endif
    // Publish readings
    String numh = String(SGPmynumco2 -1);              
    String topic = String(mqtt_out_sen) + "/co2/" + numh;
    mqttclient.publish(topic.c_str(), String(co2).c_str(), false);
    MySensors += "SGP<co" + numh;
    numh = String(SGPmynumtvoc -1);              
    topic = String(mqtt_out_sen) + "/tvoc/" + numh;
    mqttclient.publish(topic.c_str(), String(tvoc).c_str(), false);
    MySensors += "tv" + numh + "> ";
  }
}
