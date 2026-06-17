# ESP-32_C3_SEN   WORK IN PROGRESS
XIAO ESP32-C3 based to measure a lot of sensors. Until now:

One wire DHT T&H sensor      
One wire multiple DS18B20 T sensors      
I2C_ADDRESS 0x48  LM75 T sensor  
I2C_ADDRESS 0x38  AHT21 T&H sensor  
I2C_ADDRESS 0x44  SHT31 T&H sensor  
I2C_ADDRESS 0x5A  MLX90614 nontouch T sensor
I2C_ADDRESS 0x1A  AGS10 tvoc sensor  
I2C_ADDRESS (nc)-0x5A (VCC)-0x5B  CCS811 tvoc sensor  
I2C_ADDRESS 0x53  ENS160 tvoc sensor  
I2C_ADDRESS 0x58  SPG30 tvoc sensor  ATTENTION !!! SENSIBLE TO BUS LOAD
I2C_ADDRESS 0x77  BMP180 T & Baropress sensor  
I2C_ADDRESS 0x77  BMP/E-3XX T&H & Baropress sensor  
I2C_ADDRESS 0x76  BMP280 T & Baropress sensor  
I2C_ADDRESS 0x76  BME280 T&H & Baropress sensor  
I2C_ADDRESS 0x29  LV53L0X tofl sensor
I2C_ADDRESS 0x36  VL6180X tofl sensor
I2C_ADDRESS 0x36  AS5600 angle sensor
I2C_ADDRESS (nc)-0x23 (VCC)-0x5C   Light sensor BH1750                     
I2C_ADDRESS 0x10  VEML7700 Light sensor 
I2C_ADDRESS (nc)-0x68 (VCC)-0x69  6 axis sensor board MPU6500
I2C_ADDRESS 0x1C  LIS3MDL magnet sensor 
I2C_ADDRESS 0x19  LIS3DTHR acceleration sensor 

SPI MAX6675		  Thermocouple type-K  sensor
SPI MAX31856	  Thermocouple multi type sensor

The measured results are transfered via mqtt.

SEE Readme.pdf in _DOC


In the credentials.h  you define what sensoric to use plus address and network / mqtt parameters.. 
This device can read all sensors in the same device at the same time.

As these devices are used also in multiple networks in the .ino it is defined whitch credential file for a network is to use.

I dislike DHCP in static IOT networks and the last IP digit is also used to define the mqtt name. Easier for debugging. 
So in a correct credential file only the one number in   #define myIP "100"   is enough to compile another device.
