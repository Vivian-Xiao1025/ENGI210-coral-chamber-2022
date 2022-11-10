// temperature sensor 
#include <OneWire.h>
#include <DallasTemperature.h>
// LCD
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

#define ONE_WIRE_BUS 11 
#define HEATER_PIN 2
hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip

//declaring the constants for the sensor 
const int ENA_A_PIN = 3; 
const int IN1_PIN = 5; 
const int IN2_PIN = 6; 
//declaring the constants for the heater
const int HEAT_PIN = 2;
const int ENA_B_PIN = 10; 
const int IN3_PIN = 8; 
const int IN4_PIN = 9; 
// LCD geometry
const int LCD_COLS = 16;
const int LCD_ROWS = 2;
const byte degreeSymbol = "B11011111";

// code: https://create.arduino.cc/projecthub/iotboys/how-to-use-ds18b20-water-proof-temperature-sensor-2adecc 
// schematics: https://arduinogetstarted.com/tutorials/arduino-temperature-sensor 

OneWire oneWire(ONE_WIRE_BUS); 

DallasTemperature sensors(&oneWire); 

float Celcius=0; 
float Fahrenheit=0; 

void setup(void) { 
  // LCD status
  int status;

  Serial.begin(9600); 

  //starts the sensor 
  sensors.begin(); 

  //declaring the state of the pins for the pump 
  pinMode(ENA_A_PIN, OUTPUT); 
  pinMode(IN1_PIN, OUTPUT); 
  pinMode(IN2_PIN, OUTPUT); 
  pinMode(HEAT_PIN, OUTPUT);

  //declaring the state of the pins for the heater 
  pinMode(ENA_B_PIN, OUTPUT); 
  pinMode(IN3_PIN, OUTPUT); 
  pinMode(IN4_PIN, OUTPUT); 

  // LCD
  status = lcd.begin(LCD_COLS, LCD_ROWS);
	if(status) // non zero status means it was unsuccesful
	{
		// hd44780 has a fatalError() routine that blinks an led if possible
		// begin() failed so blink error code using the onboard LED if possible
		hd44780::fatalError(status); // does not return
	}
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
  analogWrite(ENA_A_PIN, 4); 

  // // driving the heater
  // digitalWrite(IN3_PIN, LOW); 
  // digitalWrite(IN4_PIN, HIGH); 
  // analogWrite(ENA_B_PIN, 250); 

  // heater control
  if (Celcius < 24.5) {
    digitalWrite(IN3_PIN, LOW); 
    digitalWrite(IN4_PIN, HIGH); 
    analogWrite(ENA_B_PIN, 250); 
    lcd.setCursor(9, 1);
    lcd.print("ON");    
  }
  else if (Celcius > 25){
    digitalWrite(IN3_PIN, LOW); 
    digitalWrite(IN4_PIN, LOW); 
    lcd.setCursor(9, 1);
    lcd.print("OFF");
  }

	// Print a message to the LCD
  lcd.setCursor(0, 0);
  lcd.print(Celcius);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0, 1);
	lcd.print(Fahrenheit);
  lcd.print((char)223);
  lcd.print("F");
  lcd.setCursor(9, 0);
  lcd.print("Heater");
}
