// Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip

#define enableWire
int VL61Xmynumdist = 0;

#include <Adafruit_VL6180X.h>
Adafruit_VL6180X vlx = Adafruit_VL6180X();
//#define I2C_ADDRESS 0x29

void startVL6180() {
  int status, tried = 0;
  do {
    status = vlx.begin();
    //Serial.printf("Adafruit VL6180x Status: %u \n",status);
    status = vlx.getAddress();                              // Dirty hack to see if chip answers as .begin not works!!
    if (status != 00) delay(100);
      tried++;
  } while ((status != 41) && (tried < 10));

  if (status != 41) {                                       // Dirty hack to see if chip answers as .begin not works!!
    Serial.printf("Failed to start VL6180x sensor after %d tries\n", tried);
    MySensors += "Error VL6180x";
  } else {
    Serial.printf("VL6180x sensor started after %d tries\n", tried);
    numdist++;
    VL61Xmynumdist = numdist;
    MySensors += "& VL61 ";
  }
}

void readVL6180() {
  String numh = String(VL61Xmynumdist -1);              
  String topic = String(mqtt_out_sen) + "/dist/" + numh;
  String topic2 = String(mqtt_out_sen) + "/diqu/" + numh;
  MySensors += "VLd" + numh + "- ";

  Serial.println("HELLO");
  delay(2000);

  // 1. Erst Lux (ALS Single-Shot, ~100ms)
  float lux = vlx.readLux(VL6180X_ALS_GAIN_1);

  Serial.println("HELLO2");
  delay(2000);


  // 2. Dann Range (Single-Shot, ~30ms)
  dist = vlx.readRange();

  Serial.println("HELLO3");
  delay(2000);

  if (dist >=254){ dist = 255; }
  else if(dist <= 39){ dist = 0; }
  mqttclient.publish(topic.c_str(), String(dist).c_str(), false);
  uint8_t Rstatus = vlx.readRangeStatus();  // ← NACH readRange()

  switch (Rstatus){
    case VL6180X_ERROR_NONE:{
      qual = "";
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_ECEFAIL:{
      qual = "ECE failure";
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_NOCONVERGE:{
      qual = "No convergence";
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_RANGEIGNORE:{
      qual = "Ignoring range";
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_SNR:{
      qual = "Signal/Noise error";
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_RAWUFLOW:{
      qual = "Raw underflow";
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_RAWOFLOW:{
      qual = "Raw overflow";
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_RANGEUFLOW:{
      qual = "Range underflow";
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_RANGEOFLOW:{
      qual = "Range overflow";
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
      break;}
    default: {
      qual = "System error";
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
      break;}
  }

  #if defined(TEST)
    Serial.printf("VL6180 Lux: %.2f\tRange: %d mm\tStatus: %d = ", lux, dist, Rstatus);

    if (Rstatus == VL6180X_ERROR_NONE) {
      Serial.println(" OK");
    } else if ((Rstatus >= VL6180X_ERROR_SYSERR_1) && (Rstatus <= VL6180X_ERROR_SYSERR_5)) {
      Serial.println(" System error");

    } else if (Rstatus == VL6180X_ERROR_ECEFAIL) {
      Serial.println(" ECE failure");

    } else if (Rstatus == VL6180X_ERROR_NOCONVERGE) {
      Serial.println(" No convergence");

    } else if (Rstatus == VL6180X_ERROR_RANGEIGNORE) {
      Serial.println(" Ignoring range");

    } else if (Rstatus == VL6180X_ERROR_SNR) {
      Serial.println(" Signal/Noise error");

    } else if (Rstatus == VL6180X_ERROR_RAWUFLOW) {
      Serial.println(" Raw underflow");

    } else if (Rstatus == VL6180X_ERROR_RAWOFLOW) {
      Serial.println(" Raw overflow");

    } else if (Rstatus == VL6180X_ERROR_RANGEUFLOW) {
      Serial.println(" Range underflow");

    } else if (Rstatus == VL6180X_ERROR_RANGEOFLOW) {
      Serial.println(" Range overflow");
    } else {
      Serial.print (" ");
    }
  #endif
}