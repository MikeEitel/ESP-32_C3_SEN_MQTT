#include <DHT.h>                              // Standard library for DHT
DHT dht(DHT_PIN,DHTtyp);                      // Allows to have different types of DHT's on "first channel"
//#define maxspi_DELAY    7                   // in ca. microseconds 


void startDHT(){
  // Power reset the DHT to conteract startup bug of some devices
  pinMode(DHT_POWER, OUTPUT);
  digitalWrite(DHT_POWER, LOW);
  delay(100);
  digitalWrite(DHT_POWER, HIGH);                  // Initialize DHT by power on
  dht.begin();
  MySensors = "DHT & "; 
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
    mqttclient.publish(mqtt_out_tem0, String(temp).c_str(), false);
    MySensors = MySensors + "DT";
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
    mqttclient.publish(mqtt_out_hum0, String(hum).c_str(), false);
    MySensors = MySensors + "H ";
  }

  #if defined(TEST)
    Serial.printf("DHT T: %f C\tF: %f %%\n",temp , hum);
  #endif
} 
