#include <dht.h>//Temp and humidity sensor lib
#include <LiquidCrystal.h> //LCD lib
#include <Arduino.h>
#include <Servo.h>//Servo lib

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

int x = 0;
void setup() 
{
  intializeServo();//Function to intialize all servo parts
  initalizeLCD();//Function to intialize all the parts needed for the LCD monitor
  Serial.begin(9600);//Begin Serial communication
}

void loop() 
{
  int chk = DHT.read11(DHT11_PIN); //This just needs to be at the start of every loop or the temp and humidity sensor doesnt work
  updateTempAndHumid();//Gets and sets the global variable "currentTemprature" to the current temp
  updateLCD();//This function new readings from updateTempAndHumid()
}

void updateLCD()
{
  
  lcd.setCursor(0,0);//Begin writing at the first line, first spot
  lcd.print("Current Temp: ");
  lcd.print(currentTemp);
  lcd.setCursor(0,1);//Begin writing at the second line, first spot
  lcd.print("Current Humid: ");
  lcd.print(currentHumid);
}


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
}