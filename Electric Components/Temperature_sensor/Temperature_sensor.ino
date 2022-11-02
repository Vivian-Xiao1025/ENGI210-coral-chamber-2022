#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 11 

//declaring the constants for the sensor 
const int ENA_A_PIN = 7; 
const int IN1_PIN = 5; 
const int IN2_PIN = 6; 

// code: https://create.arduino.cc/projecthub/iotboys/how-to-use-ds18b20-water-proof-temperature-sensor-2adecc 
// schematics: https://arduinogetstarted.com/tutorials/arduino-temperature-sensor 

OneWire oneWire(ONE_WIRE_BUS); 

DallasTemperature sensors(&oneWire); 

float Celcius=0; 
float Fahrenheit=0; 

void setup(void) { 
  Serial.begin(9600); 

  //starts the sensor 
  sensors.begin(); 

  //declaring the state of the pins for the pump 
  pinMode(ENA_A_PIN, OUTPUT); 
  pinMode(IN1_PIN, OUTPUT); 
  pinMode(IN2_PIN, OUTPUT); 
} 

void loop(void) { 

  //getting the temperature values 
  sensors.requestTemperatures(); 
  Celcius=sensors.getTempCByIndex(0); 
  Fahrenheit=sensors.toFahrenheit(Celcius); 
  Serial.print(" C "); 
  Serial.print(Celcius); 
  Serial.print(" F "); 
  Serial.println(Fahrenheit); 
  delay(1000); 

  // driving the pump 
  digitalWrite(IN1_PIN, LOW); 
  digitalWrite(IN2_PIN, HIGH); 
  digitalWrite(ENA_A_PIN, 250); 
}
