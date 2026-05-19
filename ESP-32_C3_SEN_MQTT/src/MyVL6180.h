#define enableWire

#include <Adafruit_VL6180X.h>
Adafruit_VL6180X vlx = Adafruit_VL6180X();
//#define I2C_ADDRESS 0x29

String ThisSensor = "";

void startVL6180() {
  int status, tryed = 0;
  do {
    status = vlx.begin();
    //Serial.printf("Adafruit VL6180x Status: %u \n",status);
    status = vlx.getAddress();                              // Dirty hack to see if chip answers as .begin not works!!
    if (status != 00) delay(100);
      tryed++;
  } while ((status != 41) && (tryed < 10));

  if (status != 41) {                                       // Dirty hack to see if chip answers as .begin not works!!
    Serial.printf("Failed to start VL6180x sensor after %d tries\n", tryed);
    MySensors += "Error VL6180x";
  } else {
    Serial.printf("VL6180x sensor started after %d tries\n", tryed);
    ThisSensor = " & VL6180"; 
  }
}

void readVL6180() {
  // 1. Erst Lux (ALS Single-Shot, ~100ms)
  float lux = vlx.readLux(VL6180X_ALS_GAIN_1);
  MySensors = MySensors + ThisSensor;   
  // 2. Dann Range (Single-Shot, ~30ms)
  dist = vlx.readRange();
  if (dist >=254){ dist = 255; }
  else if(dist <= 39){ dist = 0; }
  mqttclient.publish(mqtt_out_dist, String(dist).c_str(), false);
  uint8_t Rstatus = vlx.readRangeStatus();  // ← NACH readRange()

  switch (Rstatus){
    case VL6180X_ERROR_NONE:{
      qual = "";
      mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_ECEFAIL:{
      qual = "ECE failure";
      mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_NOCONVERGE:{
      qual = "No convergence";
      mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_RANGEIGNORE:{
      qual = "Ignoring range";
      mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_SNR:{
      qual = "Signal/Noise error";
      mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_RAWUFLOW:{
      qual = "Raw underflow";
      mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_RAWOFLOW:{
      qual = "Raw overflow";
      mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_RANGEUFLOW:{
      qual = "Range underflow";
      mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
      break;}
    case VL6180X_ERROR_RANGEOFLOW:{
      qual = "Range overflow";
      mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
      break;}
    default: {
      qual = "System error";
      mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
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