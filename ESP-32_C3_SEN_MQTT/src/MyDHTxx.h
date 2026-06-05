// Used pins 
// GND            GND
// DHT_POWER      XIAO-C3 board pin D0 to supply power to the DHT sensor
// DHT_PIN        XIAO-C3 board pin D1 to connect data of DHT sensor

int DTHmynumhum = 0;
int DTHmynumtemp = 0;

#include <DHT.h>                              // Standard library for DHT
DHT dht(DHT_PIN,DHTtyp);                      // Allows to have different types of DHT's on "first channel"

void startDHT(){
  // Power reset the DHT to conteract startup bug of some devices
  pinMode(DHT_POWER, OUTPUT);
  digitalWrite(DHT_POWER, LOW);
  delay(500);
  digitalWrite(DHT_POWER, HIGH);                  // Initialize DHT by power on
  dht.begin();
  MySensors = "DHT  "; 
  numhum++; numtemp++;
  DTHmynumhum = numhum;
  DTHmynumtemp = numtemp;
}

void readDHT(){
  temp = dht.readTemperature();
  if (isnan(temp)){
    #if defined(TEST)
      mqttclient.publish(out_status, "-2" ,false);
    #else
      mqttclient.publish(out_status, "DHT Temperatur ERROR" ,false);
    #endif
    statusreset = true;

    digitalWrite(DHT_POWER, LOW);                 // Reset DHT via power
    delay(1000);                                   // Give sensor time
    digitalWrite(DHT_POWER, HIGH);                // Initialize DHT via power
    }
  else{ 
    temp = temp + temDHTcorr;
    // Publish temperature readings with unique MQTT topics per sensor
    String numh = String(DTHmynumtemp -1);              // Starting mqtt at /0
    String topic = String(mqtt_out_sen) + "/temp/" + numh;
    mqttclient.publish(topic.c_str(), String(temp).c_str(), false);
    MySensors += "DHT<t" + numh;
  }

  hum = dht.readHumidity();
  if (isnan(hum)) {
    #if defined(TEST)
      mqttclient.publish(out_status, "-3" ,false);
    #else
      mqttclient.publish(out_status, "DHT Humidity ERROR" ,false);
    #endif
    statusreset = true;
    MySensors = "";
    digitalWrite(DHT_POWER, LOW);                 // Reset DHT via power
    delay(1000);
    digitalWrite(DHT_POWER, HIGH);                // Initialize DHT
    }
  else
    {
    // Correct sensor selfwarming and impossible humidity > 100%
    hum = hum + humDHTcorr;
    if (hum >= 100){
      hum = 100;
    }
    // Publish temperature readings with unique MQTT topics per sensor
    String numh = String(DTHmynumhum -1);              // Starting mqtt at /0
    String topic = String(mqtt_out_sen) + "/hum/" + numh;
    mqttclient.publish(topic.c_str(), String(hum).c_str(), false);
    MySensors += "h" + numh + "> ";
  }

  #if defined(TEST)
    Serial.printf("DHT T: %f C\tF: %f %%\n",temp , hum);
  #endif
} 
