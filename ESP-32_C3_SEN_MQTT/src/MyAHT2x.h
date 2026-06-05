// Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip

int ATHmynumhum = 0;
int ATHmynumtemp = 0;

#define enableWire

#include <AHTxx.h>
AHTxx aht(AHTXX_ADDRESS_X38, AHT2x_SENSOR); //sensor address, sensor type
//#define I2C_ADDRESS 0x38

void startAHT() {
  int status, tried = 0;
  do {
    status = aht.begin();
    if (status != 1) { 
      delay(50); 
    }
    tried++; } 
  while ((status != 1) && (tried < 10));

  if (status != 1) {
    Serial.printf("Failed to start AHT21 sensor after %d tries\n", tried);
    MySensors += "Error AHT21";} 
  else {
    Serial.printf("AHT21 sensor started after %d tries\n", tried);
    numhum++; numtemp++;
    ATHmynumhum = numhum;
    ATHmynumtemp = numtemp;
    MySensors += "ATH2x  ";
  }
}

void readAHT(){
  hum = aht.readHumidity(AHTXX_FORCE_READ_DATA); 
  temp = aht.readTemperature(AHTXX_FORCE_READ_DATA); 
  hum = hum + humAHTcorr;
  temp = temp + temAHTcorr;

  // Fehlermessung abfangen (AHTxx gibt bei Fehlern oft AHTXX_ERROR_VAL zurück)
  if (temp == AHTXX_ERROR || hum == AHTXX_ERROR) {
    #if defined(TEST)
      mqttclient.publish(out_status, "-6" ,false);
    #else
      Serial.println("AHT20 read ERROR!");
      mqttclient.publish(out_status, "AHT20 ERROR" ,false);
    #endif
    return; // Funktion abbrechen bei Fehler
  }
  #if defined(TEST)
    Serial.printf("AHT21 Temp: %f  Hum: %f\n", temp, hum);
  #endif

  // Publish temperature readings
  String numh = String(ATHmynumtemp -1);              
  String topic = String(mqtt_out_sen) + "/temp/" + numh;
  mqttclient.publish(topic.c_str(), String(temp).c_str(), false);
  MySensors += "AH<t" + numh;

  // Publish humidity readings
  numh = String(ATHmynumhum -1);              
  topic = String(mqtt_out_sen) + "/hum/" + numh;
  mqttclient.publish(topic.c_str(), String(hum).c_str(), false);
  MySensors += "h" + numh + "> ";
}