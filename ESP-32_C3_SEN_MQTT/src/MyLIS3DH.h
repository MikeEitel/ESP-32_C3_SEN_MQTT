// Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip


#define enableWire
int LISDmynumaccel = 0;

#include <LIS3DHTR.h>                   // Seeed library for LIS3DH
//#define I2C_ADDRESS 0x19
LIS3DHTR<TwoWire> lisd; 

void startLISD(){
  bool status = false;
  int tried = 0;
  String Setup = "";

  lisd.begin(Wire, 0x19);
  do {
    status = lisd.isConnection();
    Serial.printf("LIS3DH Status: %u \n",status);
    if (status != 1) delay(50);
      tried++;
  } while ((!status) && (tried < 10));
  if (!status) {
    Serial.printf("Failed to start LIS3DH sensor after %d tries\n", tried);
    MySensors += "Error LIS3DH";}
  else {
  //  lisd.setOutputDataRate(LIS3DHTR_DATARATE_50HZ);
  //  lisd.setHighSolution(true); 
  //  lisd.setPowerMode(POWER_MODE_NORMAL);
    
    Serial.printf("LIS3DH sensor started after %d tries\n", tried);
    numaccel++;
    LISDmynumaccel = numaccel;
    MySensors += "LIS3DH  ";
  }
}

void readLISD(){
/*  //String qual = " ", q1 = " ", q2 = " ";
//  if (lisd.x == 0){
//    mqttclient.publish(out_status, "LIS3DH Error" ,false);
//    Serial.print("LIS3DH Read error\n");
//  }
//  else {
  X = lisd.getAccelerationX();    
  Y = lisd.getAccelerationY();    
  Z = lisd.getAccelerationZ();
  /*
  uint8_t x_l = readSingleKlonRegister(0x28);
  uint8_t x_h = readSingleKlonRegister(0x29);
  uint8_t y_l = readSingleKlonRegister(0x2A);
  uint8_t y_h = readSingleKlonRegister(0x2B);
  uint8_t z_l = readSingleKlonRegister(0x2C);
  uint8_t z_h = readSingleKlonRegister(0x2D);
  int16_t rawX = (int16_t)((x_h << 8) | x_l);
  int16_t rawY = (int16_t)((y_h << 8) | y_l);
  int16_t rawZ = (int16_t)((z_h << 8) | z_l);
  X = (float)rawX / 16000.0;    
  Y = (float)rawY / 16000.0;    
  Z = (float)rawZ / 16000.0;
*/
    #if defined(TEST)
      Serial.printf("LIS3DH X: %f Y: %f Z: %f\n", X);
    #endif
    // Publish readings
    String numh = String(LISDmynumaccel -1);              
    String topic = String(mqtt_out_sen) + "/Accel/" + numh;
    mqttclient.publish(topic.c_str(), String(light).c_str(), false);
    MySensors += "LISD<a" + numh + "> ";
    #if defined(TEST)
      //Serial.printf("\nGain: %s  Integration Time: %s ms Value: %f", q1, q2, light);
    #endif
    //qual = "LIS3D Gain: " + q1 + " time: "+ q2;
    //String topic2 = String(mqtt_out_sen) + "/diqu/" + numh;
    //mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
//  }
}