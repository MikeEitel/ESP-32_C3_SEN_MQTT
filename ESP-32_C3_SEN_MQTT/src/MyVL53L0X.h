// Used pins
// GND            GND
// 3V3            Internal 3.3V
// XSHUT_PIN      XIAO-C3 board pin D3 to stop tofl chip
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip

#define enableWire
int VL53L0mynumdist = 0;

#include <Adafruit_VL53L0X.h>


Adafruit_VL53L0X lox = Adafruit_VL53L0X();
//#define I2C_ADDRESS 29

void startVL53L0() {
  // XSHUT aktivieren
  pinMode(XSHUT_PIN, OUTPUT);
  digitalWrite(XSHUT_PIN, LOW);
  delay(100);
  digitalWrite(XSHUT_PIN, HIGH);
  delay(100);
  int status, tried = 0;
  do {
    #if defined(TEST)
      status = lox.begin(VL53L0X_I2C_ADDR, true);
    #else
      status = lox.begin(VL53L0X_I2C_ADDR);
    #endif
//    Serial.printf("ENS Status: %u \n",status);
    if (status != true) delay(50);
      tried++;
  } while ((status != 1) && (tried < 10));

  if (!status) {
    Serial.println("Failed to boot VL53L0X");}
  else {
    numdist++;
    VL53L0mynumdist = numdist;
    MySensors += "VL53L0  ";
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
  String numh = String(VL53L0mynumdist -1);              
  String topic = String(mqtt_out_sen) + "/dist/" + numh;
  String topic2 = String(mqtt_out_sen) + "/diqu/" + numh;
  #if defined(TEST)
    lox.rangingTest(&measure, true);
    // Serial.printf("\n");
  #else 
    lox.rangingTest(&measure, false);
    dist = measure.RangeMilliMeter;
    if (dist >=499){ dist = 499; } else if(dist <= 39){ dist = 0; }  // Restrict range
    // Publish readings
    mqttclient.publish(topic.c_str(), String(dist).c_str(), false);
    // Serial.printf("%u tofl\n", dist);
    switch (measure.RangeStatus){
    case 0:{
      qual = "";
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
      break;}
    case 1:{
    //  Serial.printf("%u\tSignal schwach/verrauscht\n", dist);
      qual = "schwach";
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
      break;}    
    case 2:{
    //  Serial.printf("%u\tKein Ziel gefunden\n", dist);
      qual = "sehr weit";
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
      break;}
    case 3:{
    //  Serial.printf("%u\tObjekt zu nah am Sensor\n", dist);
      dist = 0;
      qual = "zu nahe";
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
      break;}
    case 248:{
      Serial.printf("\n\nSENSOR RESET\n\n\n");
      qual = "Sensor RESET";
      mqttclient.publish(out_status, "VL53L0 ERROR" ,false);
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
      startVL53L0();
      break;}
    default:{
    //  Serial.printf("%u\tOut of Range mit Status %u\t%f\n", dist, measure.RangeStatus);
      qual = "zu weit";
      mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
    }
  }
  #endif
  MySensors += "LX<d" + numh + "> ";
}
