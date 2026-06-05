// Used pins
// GND            GND
// 3V3            Internal 3.3V
// SDA            XIAO-C3 board pin D4 connect data of I2C chip
// SCL            XIAO-C3 board pin D5 connect clock of I2C chip

int MLXmynumtemp = 0;

#define enableWire

#include <Adafruit_MLX90614.h>                 // Nontouch temp sensor
#define I2C_ADDRESS 0x5A
Adafruit_MLX90614 mlx90614 = Adafruit_MLX90614();

void startMLX(){
  if (!mlx90614.begin(I2C_ADDRESS,&Wire)) { 
    Serial.println("Error connecting to MLX sensor. Check wiring."); }
  else{
    delay(100);
    Serial.print("MLX90614 Emissivity = "); 
    Serial.println(mlx90614.readEmissivity());
    MySensors += "MLX90614  "; 
    numtemp++; 
    MLXmynumtemp = numtemp;
    numtemp++;
  }
}

void readMLX(){
  int a= mlx90614.readAmbientTempF();  // Just to activate the chip ??
  float tempa = mlx90614.readAmbientTempC();
  float tempo = mlx90614.readObjectTempC();
  #if defined(TEST)
    Serial.printf("MXL90614 Ambient= %f*C and Object= %f*C\n", tempa, tempo);
  #endif
  if (isnan(tempa)){
    #if defined(TEST)
      mqttclient.publish(out_status, "-8" ,false);
    #else
      mqttclient.publish(out_status, "MLX90614 Temperatur ERROR" ,false);
    #endif
    }
  else{ 
    // Publish temperature readings with unique MQTT topics per sensor
    String numh = String(MLXmynumtemp -1);              // Starting mqtt at /0
    String numh2 = String(MLXmynumtemp);              // One more
    String topic = String(mqtt_out_sen) + "/temp/" + numh;
    mqttclient.publish(topic.c_str(), String(tempa).c_str(), false);
    topic = String(mqtt_out_sen) + "/temp/" + numh2;
    mqttclient.publish(topic.c_str(), String(tempo).c_str(), false);
    MySensors += "MLX<t" + numh + numh2 + "> ";
  }

  #if defined(TEST)
    Serial.printf("MLX Ta: %f C To: %f C\n",tempa,tempo);
  #endif
} 
