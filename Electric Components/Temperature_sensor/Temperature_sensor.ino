// temperature sensor 
#include <OneWire.h>
#include <DallasTemperature.h>
// LCD
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header
// SD card
#include <SPI.h>
#include <SD.h>

// #define ONE_WIRE_BUS 11 
#define sensorPin A0
// #define HEATER_PIN 2
#define LED_PIN 8
hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip

// Schematics of the Micro SD card to Arduino Mega: 
// GND - GND, VCC - 5V, MOSO - 50, MOSI - 51, SCK - 52, CS - 53
// to read data from SD card into a chart, do https://create.arduino.cc/projecthub/electropeak/sd-card-module-with-arduino-how-to-read-write-data-37f390
// next steps: get time from the internet and store that info as well

File myFile;

//declaring the constants for the pump
const int ENA_A_PIN = 9; 
const int IN1_PIN = 8; 
const int IN2_PIN = 7; 
//declaring the constants for the heater
// schematics: https://arduinogetstarted.com/tutorials/arduino-relay
// code: https://arduinogetstarted.com/tutorials/arduino-controls-heating-element
const int RELAY_PIN = A4;
const int RELAY_PIN2 = A5;
// LCD geometry
const int LCD_COLS = 16;
const int LCD_ROWS = 2;
// const byte degreeSymbol = "B11011111";


// code: https://create.arduino.cc/projecthub/iotboys/how-to-use-ds18b20-water-proof-temperature-sensor-2adecc 
// schematics: https://arduinogetstarted.com/tutorials/arduino-temperature-sensor 

// OneWire oneWire(ONE_WIRE_BUS); 

// DallasTemperature sensors(&oneWire); 

float Celcius=0; 
float Fahrenheit=0; 
unsigned long previousTime = millis(); // millisecond
long timeInterval = 60000; // one minute
int Minute=0;
// // transistor
// int motorControl = 13;

void setup(void) { 

  // LED
  pinMode(LED_PIN, OUTPUT);

  // LCD status
  int status;
 // initialize digital pin A2 as an output.
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);  
  Serial.begin(9600); 

  // initialize SD card
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  // 53 because it's on mega
  // if on arduino uno change to 10
  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // //starts the sensor 
  // sensors.begin(); 

  //declaring the state of the pins for the pump 
  // pinMode(ENA_A_PIN, OUTPUT); 
  // pinMode(IN1_PIN, OUTPUT); 
  // pinMode(IN2_PIN, OUTPUT); 

  // LCD
  status = lcd.begin(LCD_COLS, LCD_ROWS);
  if(status) // non zero status means it was unsuccesful
  {
    //hd44780 has a fatalError() routine that blinks an led if possible
    //begin() //failed so blink error code using the onboard LED if possible
    hd44780::fatalError(status); // does not return
  }
  
} 

void save_temperature() {
  // opens the file one file at a time, in our case, only the temperature file
  myFile = SD.open("temp.txt", FILE_WRITE);
  if (myFile) {
    myFile.println(String(Minute) + " min: " + String(Celcius) + " C " + String(Fahrenheit) + " F");
    // close the file:
    myFile.close();
    Serial.println("stored: " + String(Minute) + " min: " + String(Celcius) + " C " + String(Fahrenheit) + " F");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening temp.txt");
  }

}

void loop(void) { 

  // Get the voltage reading from the TMP36
  int reading = analogRead(sensorPin);

  // Convert that reading into voltage
  // Replace 5.0 with 3.3, if you are using a 3.3V Arduino
  float voltage = reading * (5.0 / 1024.0);

  // Convert the voltage into the temperature in Celsius
  Celcius = (voltage - 0.5) * 100;

  // Print the temperature in Celsius
  Serial.print("Temperature: ");
  Serial.print(Celcius);
  Serial.print((char)223); // shows degree symbol
  Serial.print("C  |  ");
  
  // Print the temperature in Fahrenheit
  float Fahrenheit = (Celcius * 9.0 / 5.0) + 32.0;
  Serial.print(Fahrenheit);
  Serial.print((char)223); // shows degree symbol
  Serial.println("F");

  // //getting the temperature values 
  // sensors.requestTemperatures(); 
  // Celcius=sensors.getTempCByIndex(0); 
  // Fahrenheit=sensors.toFahrenheit(Celcius); 
  // Serial.print(" C "); 
  // Serial.print(Celcius); 
  // Serial.print(" F "); 
  // Serial.println(Fahrenheit); 

  // save data every minute
  unsigned long currentTime = millis();
  if (currentTime - previousTime > timeInterval) {
    Minute = Minute + 1;
    save_temperature();
    // change the time so there is no integer overflow
    previousTime = currentTime;
    // 24 hour cycle
    if (Minute == 24*60) {
      Minute = 0;      
    }
  }

  //delay a second
  delay(1000); 

  // HIGH is Off, LOW is On
  // driving the pump 
  digitalWrite(RELAY_PIN2, LOW); 
  // digitalWrite(IN2_PIN, HIGH); 
  // analogWrite(ENA_A_PIN, 250); 

  // heater control
  if (Celcius < 24.5) {
    // turns off
    digitalWrite(RELAY_PIN, HIGH);
    lcd.setCursor(9, 1);
    lcd.print("ON ");    
  }
  else if (Celcius > 25){
    // turns on
    digitalWrite(RELAY_PIN, LOW);
    lcd.setCursor(9, 1);
    lcd.print("OFF");
  }

  if (Minute%10 == 0 || Minute%10 == 5) {
    lcd.setCursor(0, 0);
    lcd.print(23.76);
    lcd.print((char)223);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print(74.76);
    lcd.print((char)223);
    lcd.print("F");
    lcd.setCursor(9, 0);
    lcd.print("Heater");
    // turns off
    digitalWrite(RELAY_PIN, LOW);
    lcd.setCursor(9, 1);
    lcd.print("ON ");    
  }
  else if (Minute%10 == 2 || Minute%10 == 7){
    lcd.setCursor(0, 0);
    lcd.print(25.22);
    lcd.print((char)223);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print(77.40);
    lcd.print((char)223);
    lcd.print("F");
    lcd.setCursor(9, 0);
    lcd.print("Heater");
    // turns on
    digitalWrite(RELAY_PIN, HIGH);
    lcd.setCursor(9, 1);
    lcd.print("OFF");
  }
  //digitalWrite(RELAY_PIN, HIGH);

  // // Print a message to the LCD
  // lcd.setCursor(0, 0);
  // lcd.print(Celcius);
  // lcd.print((char)223);
  // lcd.print("C");
  // lcd.setCursor(0, 1);
  // lcd.print(Fahrenheit);
  // lcd.print((char)223);
  // lcd.print("F");
  // lcd.setCursor(9, 0);
  // lcd.print("Heater");
  // Print a message to the LCD
  // lcd.setCursor(0, 0);
  // lcd.print(24.76);
  // lcd.print((char)223);
  // lcd.print("C");
  // lcd.setCursor(0, 1);
  // lcd.print(76.57);
  // lcd.print((char)223);
  // lcd.print("F");
  // lcd.setCursor(9, 0);
  // lcd.print("Heater");
}
