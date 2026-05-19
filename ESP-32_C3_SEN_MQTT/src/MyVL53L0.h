#define enableWire

#include <Adafruit_VL53L0X.h>
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
//#define I2C_ADDRESS 

void startVL53L0() {
  // XSHUT aktivieren
  pinMode(XSHUT_PIN, OUTPUT);
  digitalWrite(XSHUT_PIN, LOW);
  delay(100);
  digitalWrite(XSHUT_PIN, HIGH);
  delay(100);
  int status, tryed = 0;
  do {
    #if defined(TEST)
      status = lox.begin(VL53L0X_I2C_ADDR, true);
    #else
      status = lox.begin(VL53L0X_I2C_ADDR);
    #endif
//    Serial.printf("ENS Status: %u \n",status);
    if (status != true) delay(50);
      tryed++;
  } while ((status != 1) && (tryed < 10));

  if (!status) {
    Serial.println("Failed to boot VL53L0X");
  }
  // Small (.01) more sensitive for fare or dark objects. Standard is 0.25. 
  lox.setLimitCheckValue(VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.1 * 65536));
  // Sigma-Limit (Standard 18) allows more noise in measurement
  lox.setLimitCheckValue(VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(32 * 65536));
  // More time to summup measurements
  lox.setMeasurementTimingBudgetMicroSeconds(100000);

  // start continuous ranging
  #if defined(TEST)
    Serial.println("VL53L0X API Continuous Ranging\n\n");
    lox.startRangeContinuous();;
    delay(500);
  #else
    Serial.println("VL53L0X API Singleshot Ranging\n\n");
    lox.startMeasurement();
  #endif
}

void readVL53L0(){
  VL53L0X_RangingMeasurementData_t measure;
  MySensors = MySensors + " & " + "VL53L0"; 
  #if defined(TEST)
    lox.rangingTest(&measure, true);
    // Serial.printf("\n");
  #else 
    lox.rangingTest(&measure, false);
    dist = measure.RangeMilliMeter;
    if (dist >=499){ dist = 499; }
    else if(dist <= 39){ dist = 0; }
    mqttclient.publish(mqtt_out_dist, String(dist).c_str(), false);
    switch (measure.RangeStatus){
      case 0:{
      //  Serial.printf("%u\tOk\n", dist);
        qual = "";
        mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
        break;}
      case 1:{
      //  Serial.printf("%u\tSignal schwach/verrauscht\n", dist);
        qual = "schwach";
        mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
        break;}    
      case 2:{
      //  Serial.printf("%u\tKein Ziel gefunden\n", dist);
        qual = "sehr weit";
        mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
        break;}
      case 3:{
      //  Serial.printf("%u\tObjekt zu nah am Sensor\n", dist);
        dist = 0;
        qual = "zu nahe";
        mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
        break;}
      case 248:{
        Serial.printf("\n\nSENSOR RESET\n\n\n");
        qual = "Sensor RESET";
        mqttclient.publish(out_status, "VL53L0 ERROR" ,false);
        mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
        startVL53L0();
        break;}
      default:{
      //  Serial.printf("%u\tOut of Range mit Status %u\t%f\n", dist, measure.RangeStatus);
        qual = "zu weit";
        mqttclient.publish(mqtt_out_qual, String(qual).c_str(), false);
      }
    }
  #endif
}
