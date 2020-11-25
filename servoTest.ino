#include <Servo.h>

//We need to install the servo library
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
  
}

ISR(INT4_vect)
{
  
  position += 90;
  if(position >= 180)
  {
    position = 0;
  }
  x.write(position);
}