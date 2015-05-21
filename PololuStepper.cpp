#include "PololuStepper.h"

bool PololuStepper::motor_high = false;
int PololuStepper::step_pin;

PololuStepper::PololuStepper()
{

}

void PololuStepper::attach(int ms1_pin, int ms2_pin, int ms3_pin, int step_pin, int dir_pin)
{  
  this->ms1_pin = ms1_pin;
  this->ms2_pin = ms2_pin;
  this->ms3_pin = ms3_pin;
  PololuStepper::step_pin = step_pin;
  this->dir_pin = dir_pin;
    
  pinMode(this->ms1_pin, OUTPUT);
  pinMode(this->ms2_pin, OUTPUT);
  pinMode(this->ms3_pin, OUTPUT);
  pinMode(PololuStepper::step_pin, OUTPUT);
  pinMode(this->dir_pin, OUTPUT);

  digitalWrite(this->ms1_pin, HIGH);
  digitalWrite(this->ms2_pin, HIGH);
  digitalWrite(this->ms3_pin, HIGH);
  digitalWrite(PololuStepper::step_pin, LOW);
  digitalWrite(this->dir_pin, LOW);
} 

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void PololuStepper::Timer0_A0 (void)
{  
  //Serial.println("Doing work");
  
  if(!PololuStepper::motor_high)
  {
    digitalWrite(PololuStepper::step_pin, HIGH);
    PololuStepper::motor_high = true;    
  }
  else
  {
    digitalWrite(PololuStepper::step_pin, LOW);
    PololuStepper::motor_high = false;
  }
}

void PololuStepper::forward()
{
  digitalWrite(this->dir_pin, LOW);
  start_timer();
}
  
void PololuStepper::reverse()
{
  digitalWrite(this->dir_pin, HIGH);
  start_timer();
}

void PololuStepper::stop()
{
  this->stop_timer();
}
  
void PololuStepper::start_timer()
{
  TA0CCR0 =  300;
  TA0CCTL0 = CCIE;
  TA0CTL = TASSEL_2 + MC_1 + ID_3;  
}

void PololuStepper::stop_timer()
{
  TA0CCTL0 = 0;
  TA0CCR0 = 0;
}
  

