#include <Arduino.h>
#include <Servo.h>

unsigned volatile char* portB = (unsigned char*) 0x25;
unsigned volatile char* ddrB =  (unsigned char*) 0x24;
unsigned volatile char* pinB = (unsigned char*) 0x23;
unsigned volatile char* sreg = (unsigned char*) 0x5F;
unsigned volatile char* eicrb = (unsigned char*) 0x6A;
unsigned volatile char* eimsk = (unsigned char*) 0x3D;

int water_level_value; // not wet when 0, fully wet when 350
int water_level_pin = A5; // USING analogue PIN 5 hll

int position=0;
Servo x;




enum STATES {
  LOW_WATER
}

STATES currentState;

void OnLowWaterState() {
  currentState = STATES.LOW_WATER;
}

void setup() {
  *sreg = 0b10000000;
  pinMode(*ddrB, OUTPUT);
  pinMode(2,INPUT);
  x.attach(9);
  x.write(position);
  *eicrb = 0b00000011;
  *eimsk = 0x10;
}

void updateWaterLevelReading() {
  water_level_value = analogRead(water_level_pin); /// returns a max value of 350 when full
  if(water_level_value < 25 && currentState != STATES.LOW_WATER) {
    OnLowWaterState(); // tap this function once if we need to change states to low water
  }
}

void loop() 
{
  updateWaterLevelReading();
  delay(500);
}

ISR(INT4_vect)
{
  delay(500);
  position += 90;
  delay(500);
  if(position > 190)
  {
    position = 0;
  }
  delay(10000);
  x.write(position);




  delay(500);
}