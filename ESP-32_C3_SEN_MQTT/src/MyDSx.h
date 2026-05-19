#include <DS18B20.h>                // Standard library for DS18B20 x
#if defined(TEST)
  #include <DallasTemperature.h>
#endif
DS18B20 ds(DS_PIN);                 // Set the "second channel" for pure DS18 devices


void startDS(){
  dscounted = ds.getNumberOfDevices();
  #if defined(TEST)
    Serial.printf("Found %u DS18 Senors\n",dscounted);
  #endif

  if (dscounted == 0){
    #if defined(TEST)
      mqttclient.publish(out_status, "-4" ,false);
    #else
      mqttclient.publish(out_status, "DS18B20 Temperatur unavailable" ,false);
    #endif
    }
  else  {
    MySensors = MySensors + " & " + dscounted + " DS18";
    Serial.printf("Found %s\n",MySensors.c_str());
  }
}

void readDS(){
  ds.resetSearch();
  dscounter = 0; 
  while (ds.selectNext()) {
    temp = ds.getTempC();
    if (isnan(temp)){
      #if defined(TEST)
        mqttclient.publish(out_status, "-4" ,false);
        Serial.println("DS18B20 Temperatur unavailable");
      #else
        Serial.println("DS18B20 Temperatur unavailable");
        mqttclient.publish(out_status, "DS18B20 Temperatur unavailable" ,false);
      #endif
    }
    #if defined(TEST)
      uint8_t addr[8]; // Array für die Adresse
      ds.getAddress(addr); 
      Serial.printf("DScounter: %u\tAddress: ", dscounter);   
      for (uint8_t i = 0; i < 8; i++) {
        if (addr[i] < 16) Serial.print("0");
        Serial.print(addr[i], HEX);
      }
      Serial.printf("\tTemperatur: %.2f C\n", temp);
      dscounter++;
    #else
        Serial.print(".");
    #endif
    // Serial.printf("Count: %u\n", dscounter);
    switch (dscounter){
      case 0:{
        mqttclient.publish(mqtt_out_tem1, String(temp).c_str(), false);}
      break;
      case 1:{
        mqttclient.publish(mqtt_out_tem2, String(temp).c_str(), false);}
      break;
      case 2:{
        mqttclient.publish(mqtt_out_tem3, String(temp).c_str(), false);}
      break;
      case 3:{
        mqttclient.publish(mqtt_out_tem3, String(temp).c_str(), false);}
      break;
      case 4:{
        mqttclient.publish(mqtt_out_tem3, String(temp).c_str(), false);}
      break;
      default:{
        Serial.println("DS18 adressing error. Too many sensors");
        Serial.printf("\tTemperatur: %.2f C\n", temp);
      }
    }
    dscounter++; 
  }
  MySensors = MySensors + " & " + (dscounted+1) + " DS18B20";
}