#include <dht.h>//Temp and humidity sensor lib
#include <LiquidCrystal.h> //LCD lib
#include <Arduino.h>
#include <Servo.h>//Servo lib

volatile unsigned char *portB = (unsigned char*) 0x25;
volatile unsigned char *ddrB = (unsigned char*) 0x24;
volatile unsigned char *pinB = (unsigned char*) 0x23;

volatile unsigned char *portH = (unsigned char*) 0x102;
volatile unsigned char *ddrH = (unsigned char*) 0x101;
volatile unsigned char *pinH = (unsigned char*) 0x100;

volatile unsigned char *portG = (unsigned char*) 0x34;
volatile unsigned char *ddrG = (unsigned char*) 0x33;
volatile unsigned char *pinG = (unsigned char*) 0x32;

volatile unsigned char *portE = (unsigned char*) 0x2E;
volatile unsigned char *ddrE = (unsigned char*) 0x2D;
volatile unsigned char *pinE = (unsigned char*) 0x2C;

volatile unsigned char *portJ = (unsigned char*) 0x105;
volatile unsigned char *ddrJ = (unsigned char*) 0x104;
volatile unsigned char *pinJ = (unsigned char*) 0x103;

volatile unsigned char *portF = (unsigned char*) 0x31;
volatile unsigned char *ddrF = (unsigned char*) 0x30;
volatile unsigned char *pinF = (unsigned char*) 0x2F;


//Following Declerations are needed to run the state machine tracking button
int stateButtonPin = 1; //Digital Pin 3
int currentState = 0; //0 = off state, 1 = idle state
int stateButtonTime = 0; //This is used to debouce the on off button signals
int stateButtonLastTime = 0; //This is used to debouce the on off button signals
int yellowLEDPin = 14; //Digital Pin 14
int greenLEDPin = 15; //Digital Pin 15
int blueLEDPin = 16; //Digital Pin 16
int redLEDPin = 17; //Digital Pin 17

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
int fanPin = 11;
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
  updateTempAndHumid(); //Get intial readings
  updateWaterLevel(); //Get intial readings
  Serial.begin(9600);//Begin Serial communication
}
void loop() 
{
  int chk = DHT.read11(DHT11_PIN); //This just needs to be at the start of every loop or the temp and humidity sensor doesnt work
  checkState(); //Checks and sets the currentState variable
  setState(); //calls functions needed for the determined state
}



void setLED(int red, int blue, int yellow, int green) //Sets the Leds based on a binary passed in
{
  digitalWrite(greenLEDPin,green);
  digitalWrite(redLEDPin,red);
  digitalWrite(yellowLEDPin,yellow);
  digitalWrite(blueLEDPin,blue);
}

void checkState()//Monitors the active state based on project parameters
{
  if(currentState != 0)//If the ISR on/off button has not been pressed
  {
    if(waterLevel < 250) //Highest priority state condition
    {
      currentState = 3;
    }
    else if(currentTemp > 22) //Second priority state condition
    {
      currentState = 2;
    }
    else //If both other state conditions are in check stand by in idle mode
    {
      currentState = 1;
    }
  }
}
/*void updateTime(){
  if(serialTracker == 1)
  {
    serialTracker = 0;
    Serial.print(time(thyme));
  }
} */
void setState() //Different functions based on state
{

  if(currentState == 0) //Off and disabled
  {
    setLED(0,0,1,0); //Set the yellow LED
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("OFF");
    delay(500);
    lcd.clear();
    detachInterrupt(servoButtonPin);//Stops the servo interrupt from working
    //updateTime();
    digitalWrite(fanPin, 0);
  } 
  if(currentState == 1) //On and idle
  {
    //updateTime();
    setLED(0,0,0,1); //Set the green LED
    attachInterrupt(servoButtonPin, servoButton, RISING);//Reattach the interrupt after changing states
    updateTempAndHumid();//Gets and sets the global variable "currentTemprature" to the current temp
    updateLCD();//This function new readings from updateTempAndHumid()
    updateWaterLevel();//This function gets the current water level reading
    digitalWrite(fanPin, 0);
  }
  if(currentState == 2) //Fan running/temp to high
  {
    setLED(0,1,0,0); //Set the Blue LED
    updateTempAndHumid(); //Check to see if temp is back in accpetable ranges
    updateWaterLevel(); //See if the water level is too low, which would overide this state and send the machine to the error state
    digitalWrite(fanPin, 1);
  }
  if(currentState == 3) //Error State
  {
    setLED(1,0,0,0); //Set the Red LED
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Error!");
    lcd.setCursor(0,1);
    lcd.print("Add More Water");
    updateWaterLevel(); //Only check the water level until issue is resolved
    detachInterrupt(servoButtonPin); //Stops the servo from working
    digitalWrite(fanPin, 0);
  }
}
void initalizeState()
{
  *ddrJ = 0b00000011;//State LED for yellow and green
  *ddrH = 0b00000011;//State LED for Red and Blue
  *ddrB = 0b00100000; //Set the fan
  attachInterrupt(stateButtonPin,updateStateISR, RISING); //Attach the on off state inttrupt routine
}
void updateStateISR()
{
  stateButtonTime = millis(); //Used for button debounce
  if(stateButtonTime - stateButtonLastTime > 250)//Used for button debounce
  {
    currentState++; //change the state
    //serialTracker++;
    if(currentState > 1) //if the state exceeds 1, reset it back to 0 
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
  delay(2000);                                             
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
 
  *portB = 0b00000000;//Set digital pin 13 to output
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