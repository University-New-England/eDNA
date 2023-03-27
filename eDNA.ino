// Library section
#include <Wire.h>
//LCD SETUP
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header
//DEPTH SENSOR SETUP
#include "MS5837.h"
//Adalogger SETUP
#include <RTClib.h>
#include <SPI.h>
#include <SD.h>


// Object instantiation, Constants and variables
RTC_PCF8523 rtc;
hd44780_I2Cexp lcd;
MS5837 sensor;
// SD card chip select pin number
const int chipSelect = 11;
int finit=0;
String filename="";
//Solenoid Pin
int solenoidPin = 27;

void setup() {
  Serial.begin(115200);
 
 //LCD SETUP
 lcd.begin(16, 2);
 
 // Turn on the backlight.
  lcd.backlight();
  lcd.clear();
// Starts SD card communication
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
// Set the clock Uncomment run once and the comment
//Takes the Date and Time according to the computer attached
// and uses that to program the RTC. Must compile and upload

//rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

//Solenoid Setup
pinMode(solenoidPin, OUTPUT);

       
//DEPTH SENSOR SETUP
 Wire.begin();
 
 // Initialize pressure sensor
 // Returns true if initialization was successful
 // We can't continue with the rest of the program unless we can initialize the sensor
 while (!sensor.init()) {
   Serial.println("Pressure Sensor Init failed!");
   
   delay(5000);
 }
  sensor.setModel(MS5837::MS5837_30BA);
 sensor.setFluidDensity(997); // kg/m^3 (997 freshwater, 1029 for seawater)
}
 
void loop() {

// creates a now data structure reading date time from RTC board
DateTime now = rtc.now();  
 // This section here creates a filename with the date appended to it
//filename 8 character limit
// The if creates the file name once when the loop starts so after                
    if(finit==0){                                       
                           
   filename +="D";                                  
   filename +=now.day();                      
   filename +=now.hour();                    
   filename +=now.minute();              
   filename +=".csv";
  Serial.println(filename);                          
   finit=1;                                                
    }
//Depth Sensor
 // Update pressure and temperature readings
 sensor.read();
 
 //LCD
 lcd.begin(16, 2);   //added to fix ocassional intermittent connection
  lcd.clear();
 lcd.setCursor(0, 0);
   lcd.print(sensor.depth());
   lcd.print("m");
 lcd.setCursor(0,2);
   String time1="";
   time1 +=now.hour();
   time1 +=":";
   time1 +=now.minute();
   time1 +=":";
   time1 +=now.second();
   lcd.print(time1);
String dataString = "";
dataString += now.month();
dataString += "/";
dataString += now.day();
dataString += "/";
dataString += now.year();
dataString += " ";
dataString += now.hour();
dataString += ":";
dataString += now.minute();
dataString += ":";
dataString += now.second();
dataString += ", ";
dataString += sensor.pressure();  //mbar
dataString += ", ";
dataString += sensor.depth();      // m
//dataString += ", ";
//dataString += sensor.getTempCByIndex(0);
// This section writes to the SD card
// open filename to write
File dataFile = SD.open(filename, FILE_WRITE);

if (dataFile) {
dataFile.println(dataString);
dataFile.close();
// print to the serial port too:
Serial.println(dataString);
}
else {
Serial.println("error opening datalog.txt");
}
if (sensor.read(sensor.depth() == 1)) {
  digitalWrite(solenoidPin, HIGH); //Switch Solenoid ON
  delay(20000);                    //Wait 20 Second
  digitalWrite(solenoidPin, LOW); //Switch Solenoid OFF
}
 delay(1000);
}
