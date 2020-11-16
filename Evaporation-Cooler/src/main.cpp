#include <Arduino.h>
#include <Servo.h>

unsigned volatile char* portB = (unsigned char*) 0x25;
unsigned volatile char* ddrB =  (unsigned char*) 0x24;
unsigned volatile char* pinB = (unsigned char*) 0x23;
unsigned volatile char* sreg = (unsigned char*) 0x5F;
unsigned volatile char* eicrb = (unsigned char*) 0x6A;
unsigned volatile char* eimsk = (unsigned char*) 0x3D;
int position=0;
Servo x;
void setup() {
  *sreg = 0b10000000;
  pinMode(*ddrB, OUTPUT);
  pinMode(2,INPUT);
  x.attach(9);
  x.write(position);
  *eicrb = 0b00000011;
  *eimsk = 0x10;
}

void loop() 
{
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