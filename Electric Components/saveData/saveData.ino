#include <SPI.h>
#include <SD.h>
// temperature sensor 
#include <OneWire.h>
#include <DallasTemperature.h>

// Schematics of the Micro SD card to Arduino Mega: 
// GND - GND, VCC - 5V, MOSO - 50, MOSI - 51, SCK - 52, CS - 53
// to read data from SD card into a chart, do https://create.arduino.cc/projecthub/electropeak/sd-card-module-with-arduino-how-to-read-write-data-37f390
// next steps: get time from the internet and store that info as well

File myFile;

#define ONE_WIRE_BUS 11 
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire); 

float Celcius=0; 
float Fahrenheit=0; 
unsigned long previousTime = millis(); // millisecond
long timeInterval = 60000; // one minute
int Minute=0;

void save_temperature() {
  // opens the file one file at a time, in our case, only the temperature file
  myFile = SD.open("temp.txt", FILE_WRITE);
  if (myFile) {
    myFile.println(String(Minute) + " min: " + String(Celcius) + " C " + String(Fahrenheit) + " F");
    // close the file:
    myFile.close();
    Serial.println("stored: " + String(Celcius) + " C " + String(Fahrenheit) + " F");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

}

void setup() {

  //starts the sensor 
  sensors.begin(); 

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
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
}

void loop() {
  //getting the temperature values 
  sensors.requestTemperatures(); 
  Celcius=sensors.getTempCByIndex(0); 
  Fahrenheit=sensors.toFahrenheit(Celcius); 
  Serial.print(" C "); 
  Serial.print(Celcius); 
  Serial.print(" F "); 
  Serial.println(Fahrenheit); 

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

  delay(1000); 
}



