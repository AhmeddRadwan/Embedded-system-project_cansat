#include <Adafruit_Sensor.h> //ADXL345
#include <Wire.h>  // Wire library - used for I2C communication
#include <DHT.h>//Arduino to NodeMCU Lib
#include <SoftwareSerial.h> //Send data From mega to NodeMCU
#include <ArduinoJson.h>


int ADXL345 = 0x53; // The ADXL345 sensor I2C address
//Initialise Arduino to NodeMCU (5=Rx & 6=Tx)
SoftwareSerial nodemcu(24, 26);

//Initialisation of DHT11 Sensor
#define DHTPIN 22
DHT dht(DHTPIN, DHT11);
float temp;
float hum;
float x;
float y;
void setup() {
  Wire.begin(); // Initiate the Wire library
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  Serial.begin(9600);
  dht.begin();
  nodemcu.begin(9600);

}

void loop() {
     StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();
 Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true);
  
  //Assign collected data to JSON Object
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  data["humidity"] = hum;
  data["temperature"] = temp; 
  x=( Wire.read()| Wire.read() << 8);
  x=x/256;
  y=( Wire.read()| Wire.read() << 8);
  y=y/256;
  data["x-axis"] = x;
  data["y-axis"] = y; 
  
  //Send data to NodeMCU
  data.printTo(nodemcu);
  jsonBuffer.clear();

  delay(1000);
}
