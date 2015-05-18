#include "Plunger.h"
#include "HardwareSerial.h"

int const Plunger::down_pulse = 1200;
int const Plunger::up_pulse = 1800;
int const Plunger::period = 20000;

Plunger::Plunger()
{
  
}

void Plunger::attach(int pin)
{
  this->servo_pin = pin;
  pinMode(this->servo_pin, OUTPUT);
}

void Plunger::push_down(float seconds)
{   
 // Serial.println("Pushing down");
  
  int cycles = 50 * seconds;
  for(int i = 0; i < cycles; i++)
  {    
    const int remaining_time = Plunger::period - Plunger::down_pulse;
    digitalWrite(this->servo_pin, HIGH);
    delayMicroseconds(Plunger::down_pulse);
    digitalWrite(this->servo_pin, LOW);
    delayMicroseconds(remaining_time);
  }
}
  
void Plunger::pull_up(float seconds)
{
 // Serial.println("Pulling up");
  
  int cycles = 50 * seconds;
  for(int i = 0; i < cycles; i++)
  {    
    const int remaining_time = Plunger::period - Plunger::up_pulse;
    digitalWrite(this->servo_pin, HIGH);
    delayMicroseconds(Plunger::up_pulse);
    digitalWrite(this->servo_pin, LOW);
    delayMicroseconds(remaining_time);
  }
}


