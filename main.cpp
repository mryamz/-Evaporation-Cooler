#include <dht.h>//Temp and humidity sensor lib
#include <LiquidCrystal.h> //LCD lib
#include <Arduino.h>
#include <Servo.h>//Servo lib

//Following Declerations are needed to run the state machine tracking button
int stateButtonPin = 1; //Digital Pin 3
int currentState = 0; //0 = off state, 1 = idle state
int stateButtonTime = 0; //This is used to debouce the on off button signals
int stateButtonLastTime = 0; //This is used to debouce the on off button signals
int yellowLEDPin = 14; //Digital Pin 14
int greenLEDPin = 15; //Digital Pin 13

//Following declerations are needed to run the water level sensor
int waterSensorPin = A0; //Analog pin
int waterLevel; //numerical water level

//Following declerations are needed to run the LCD monitor                  
LiquidCrystal lcd (4,5,6,7,8,9); // Define LCD display pins RS, E, D4, D5, D6, D7: Note these are digita; pin 4-9

//The following declerations are the pins needed to run the servo and fan
int servoPin = 13; //The pin going to the servo is digital pin 13
int servoButtonPin = 0; //The pin for the wire going from the button to the arduino is digital pin 2, not digital pin 0
int fanPosition = 0;//This value keeps track of what angle the servo is at
unsigned long servoButtonTime = 0;//This is used to debounce the servo input
unsigned long servoButtonLastTime = 0;//This is used to debounce the servo input
Servo motor;//This is the servo motor variable 

//The following declerations are needed to run the humidity and Temp sensor
#define DHT11_PIN 12 //The pin for this sensor is Digital pin 12
dht DHT;
float currentTemp;
float currentHumid;


void setup() 
{
  initalizeState(); //Function to intialize pin for on/off button
  intializeServo();//Function to intialize all servo parts
  initalizeLCD();//Function to intialize all the parts needed for the LCD monitor
  Serial.begin(9600);//Begin Serial communication
}
void loop() 
{
  int chk = DHT.read11(DHT11_PIN); //This just needs to be at the start of every loop or the temp and humidity sensor doesnt work
  setState();
  Serial.println(currentState);//used for debugging
  Serial.println(waterLevel);//used for debugging
}


void setState()
{
  if(currentState == 0) //Off and disabled
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("OFF");
    delay(1000);
    lcd.clear();
    digitalWrite(yellowLEDPin, HIGH); //This code needs to be streamlined for all the 4 LEDs
    digitalWrite(greenLEDPin, LOW);//This code needs to be streamlined for all the 4 LEDs
    detachInterrupt(servoButtonPin);//Stops the servo intrrupt from working
  }
  if(currentState == 1) //On and idle
  {
    attachInterrupt(servoButtonPin, servoButton, RISING);//Reattach the interrupt after changing states
    digitalWrite(greenLEDPin, HIGH);//This code needs to be streamlined for all the 4 LEDs
    digitalWrite(yellowLEDPin, LOW);//This code needs to be streamlined for all the 4 LEDs
    updateTempAndHumid();//Gets and sets the global variable "currentTemprature" to the current temp
    updateLCD();//This function new readings from updateTempAndHumid()
    updateWaterLevel();//This function gets the current water level reading
  }
}
void initalizeState()
{
  pinMode(yellowLEDPin,OUTPUT); //State LED
  pinMode(greenLEDPin, OUTPUT);//State LED
  attachInterrupt(stateButtonPin,updateStateISR, RISING); //Attach the on off state inttrupt routine
}
void updateStateISR()
{
  stateButtonTime = millis(); //Used for button debounce
  if(stateButtonTime - stateButtonLastTime > 250)//Used for button debounce
  {
    currentState++; //change the state
    if(currentState > 1) //if the state exceeds 1, reset it back to 0 *NOTE* may need to change functionality when adding the running and error states
    {
      currentState = 0;
    }
  }
  stateButtonLastTime = stateButtonTime;//Used for button debounce
}


//This funtion updates the global variable waterLevel
void updateWaterLevel()
{
  waterLevel = analogRead(waterSensorPin); //Read the current water level reading from analog pin 0 (A0)
}


//This function updates the LCD with new sensor readings
void updateLCD()
{
  lcd.setCursor(0,0);//Begin writing at the first line, first spot
  lcd.print("Current Temp: ");
  lcd.print(currentTemp);
  lcd.setCursor(0,1);//Begin writing at the second line, first spot
  lcd.print("Current Humid: ");
  lcd.print(currentHumid);
}
//This function initalizes the LCD variables and displays a welcome message
void initalizeLCD()
{
  lcd.begin(16,2); //Set the variable lcd to be a 16x2
  lcd.setCursor(0,0);//Begin writing at the first line, first spot
  lcd.print("Swamp Cooler");                                   
  lcd.setCursor(0,1);//Begin writing at the second line, first spot
  lcd.print("Simulation");
  delay(3000);                                             
  lcd.clear();                                              
}


//Delays are needed in this function or else we get garbage values for the temp sensor. DHT is assigned to the digital pin 12 on the board
void updateTempAndHumid()
{
  delay(575);
  currentTemp = DHT.temperature; //update current temp global variable
  currentHumid = DHT.humidity;//update current humidity global variable
  delay(575);
}


void intializeServo()
{
 
  pinMode(servoPin, OUTPUT);//Set digital pin 13 to output
  motor.attach(13);//Attach the motor variable to pin 13
  motor.write(fanPosition);//Write the intial servo position (0);
  attachInterrupt(servoButtonPin, servoButton, RISING);//Create the interrupt for digital pin 2(that is zero internally), name the ISR function, and trigger the change on the rising edge only
}
//This function adds 90 degrees to the servos current position until it reaches 180 degrees and then resets back down to zero, it also uses time stamps and the milli function to debouce the button
void servoButton()
{
  servoButtonTime = millis();
  if (servoButtonTime - servoButtonLastTime > 250)
  {
    fanPosition += 90;
    if(fanPosition > 180)
      {
        fanPosition = 0;
      }
    motor.write(fanPosition);
    servoButtonLastTime = servoButtonTime;
  }
}