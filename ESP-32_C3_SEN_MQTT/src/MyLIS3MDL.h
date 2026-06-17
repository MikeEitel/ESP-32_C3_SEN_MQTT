// Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip


#define enableWire
int LISMmynummag = 0;

#include <Adafruit_LIS3MDL.h>                   // Seeed library for LIS3MDL
//#define I2C_ADDRESS 0x1C
Adafruit_LIS3MDL lism; 

void startLISM(){
  bool status = false;
  int tried = 0;
  String Setup = "";

  status = lism.begin_I2C();
  do {
    //status = lism.();
    Serial.printf("LIS3DH Status: %u \n",status);
    if (status != 1) delay(50);
      tried++;
  } while ((!status) && (tried < 10));
  if (!status) {
    Serial.printf("Failed to start LIS3DH sensor after %d tries\n", tried);
    MySensors += "Error LIS3DH";}
  else {
    lism.setPerformanceMode(LIS3MDL_MEDIUMMODE);
    Serial.print("Performance mode set to: ");
    switch (lism.getPerformanceMode()) {
      case LIS3MDL_LOWPOWERMODE: Serial.println("Low"); break;
      case LIS3MDL_MEDIUMMODE: Serial.println("Medium"); break;
      case LIS3MDL_HIGHMODE: Serial.println("High"); break;
      case LIS3MDL_ULTRAHIGHMODE: Serial.println("Ultra-High"); break;
    }
    Serial.printf("LIS3DH sensor started after %d tries\n", tried);
    nummag++;
    LISMmynummag = nummag;
    MySensors += "LIS3DH  ";
  }
}

void readLISM(){
  //String qual = " ", q1 = " ", q2 = " ";
  lism.read();
  if ( 1 == 2) {
    mqttclient.publish(out_status, "LIS3DH Error" ,false);
    Serial.print("LIS3DH Read error\n");
    }
  else {
    X = lism.x_gauss;    
    Y = lism.y;    
    Z = lism.z;
  sensors_event_t event; 

    #if defined(TEST)
      Serial.printf("LIS3DH X: %f Y: %f Z: %f\n", X);
  lism.getEvent(&event);
  /* Display the results (magnetic field is measured in uTesla) */
  Serial.print("\tX: "); Serial.print(event.magnetic.x);
  Serial.print(" \tY: "); Serial.print(event.magnetic.y); 
  Serial.print(" \tZ: "); Serial.print(event.magnetic.z); 
  Serial.println(" uTesla ");
    #endif


    // Publish readings
    String numh = String(LISMmynummag -1);              
    String topic = String(mqtt_out_sen) + "/Mag/" + numh;
    mqttclient.publish(topic.c_str(), String(light).c_str(), false);
    MySensors += "LISM<a" + numh + "> ";
    #if defined(TEST)
      //Serial.printf("\nGain: %s  Integration Time: %s ms Value: %f", q1, q2, light);
    #endif
    //qual = "LIS3D Gain: " + q1 + " time: "+ q2;
    //String topic2 = String(mqtt_out_sen) + "/diqu/" + numh;
    //mqttclient.publish(topic2.c_str(), String(qual).c_str(), false);
  }
}