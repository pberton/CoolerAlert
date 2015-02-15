/*
*/

#include <Wire.h> //I2C needed for sensors
#include "MPL3115A2.h" //Pressure sensor
#include "HTU21D.h" //Humidity sensor

MPL3115A2 myPressure; //Create an instance of the pressure sensor
HTU21D myHumidity; //Create an instance of the humidity sensor

//Hardware pin definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// digital I/O pins
const byte STAT1 = 7;
const byte STAT2 = 8;

// analog I/O pins
const byte REFERENCE_3V3 = A3;
const byte LIGHT = A1;
const byte BATT = A2;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
float humidity = 0; // [%]
float tempc = 0; // [temperature C]
float rainin = 0; // [rain inches over the past hour)] -- the accumulated rainfall in the past 60 min
float pressure = 0;
float batt_lvl = 11.8; //[analog value from 0 to 1023]
float light_lvl = 455; //[analog value from 0 to 1023]

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  
  Serial.begin(9600);
  //Serial.println("Weather Shield setup");

  pinMode(STAT1, OUTPUT); //Status LED Blue
  pinMode(STAT2, OUTPUT); //Status LED Green
 
  pinMode(REFERENCE_3V3, INPUT);
  pinMode(LIGHT, INPUT);

  //Configure the pressure sensor
  myPressure.begin(); // Get sensor online
  myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags 

  //Configure the humidity sensor
  myHumidity.begin();
  
  //Serial.println("Weather Shield online!");
}

// the loop function runs over and over again forever
void loop() {
 
  digitalWrite(STAT1, HIGH); //Blink stat LED
 
  //Go calc all the various sensors
  calcWeather(); 
  
  //Report all readings every second
  printWeather();
    
  digitalWrite(STAT1, LOW); //Turn off stat LED
  
  digitalWrite(STAT2, HIGH);    // turn the LED off by making the voltage LOW
  delay(1000);              // wait for a second
  digitalWrite(STAT2, LOW);    // turn the LED off by making the voltage LOW
}


//Calculates each of the variables that wunderground is expecting
void calcWeather()
{
  //Calc humidity
  humidity = myHumidity.readHumidity();
  float temp_h = myHumidity.readTemperature();
  //Serial.print(" TempH:");
  //Serial.print(temp_h, 2);
  tempc = temp_h;
  
  //Calc temp from pressure sensor
  float temp_p = myPressure.readTemp();
  //Serial.print(" TempP:");
  //Serial.print(temp_p, 2);
  //tempc = temp_p;
  
  //Calc pressure
  pressure = myPressure.readPressure();

  //Calc light level
  light_lvl = get_light_level();

  //Calc battery level
  batt_lvl = get_battery_level();
}

//Returns the voltage of the light sensor based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
float get_light_level()
{
  float operatingVoltage = analogRead(REFERENCE_3V3);

  float lightSensor = analogRead(LIGHT);
  
  operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V
  
  lightSensor = operatingVoltage * lightSensor;
  
  return(lightSensor);
}


//Returns the voltage of the raw pin based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
//Battery level is connected to the RAW pin on Arduino and is fed through two 5% resistors:
//3.9K on the high side (R1), and 1K on the low side (R2)
float get_battery_level()
{
  float operatingVoltage = analogRead(REFERENCE_3V3);

  float rawVoltage = analogRead(BATT);
  
  operatingVoltage = 3.30 / operatingVoltage; //The reference voltage is 3.3V
  
  rawVoltage = operatingVoltage * rawVoltage; //Convert the 0 to 1023 int to actual voltage on BATT pin
  
  rawVoltage *= 4.90; //(3.9k+1k)/1k - multiple BATT voltage by the voltage divider to get actual system voltage
  
  return(rawVoltage);
}

//Prints the various variables directly to the port
//I don't like the way this function is written but Arduino doesn't support floats under sprintf
void printWeather()
{
  Serial.print("{\"humidity\":");
  Serial.print(humidity, 2);
  Serial.print(",\"temp\":");
  Serial.print(tempc, 2);
  Serial.print(",\"pressure\":");
  Serial.print(pressure, 2);
  Serial.print(",\"batt_lvl\":");
  Serial.print(batt_lvl, 2);
  Serial.print(",\"light_lvl\":");
  Serial.print(light_lvl, 2);
  Serial.println("}");
}

