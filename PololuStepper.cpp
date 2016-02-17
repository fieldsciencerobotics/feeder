/*
The MIT License (MIT)

Copyright (c) 2015 Field Science Robotics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "PololuStepper.h"

bool PololuStepper::motor_high = false;
int PololuStepper::step_pin;

PololuStepper::PololuStepper()
{
  this->active = false;
}

void PololuStepper::attach(int ms1_pin, int ms2_pin, int ms3_pin, int step_pin, int dir_pin, int dir)
{  
  this->ms1_pin = ms1_pin;
  this->ms2_pin = ms2_pin;
  this->ms3_pin = ms3_pin;
  PololuStepper::step_pin = step_pin;
  this->dir_pin = dir_pin;
  this->dir = dir;
    
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

bool PololuStepper::is_active()
{
  return this->active;
}

void PololuStepper::forward()
{
  digitalWrite(this->dir_pin, this->dir);
  this->isforward = true;
  this->update_dir_pin();
  start_timer();
}
  
void PololuStepper::reverse()
{
  digitalWrite(this->dir_pin, this->dir);
  this->isforward = false;
  this->update_dir_pin();
  start_timer();
}

void PololuStepper::stop()
{
  this->stop_timer();
}


void PololuStepper::update_dir_pin()
{
  if(this->isforward)
  {
    digitalWrite(this->dir_pin, this->dir);
  }
  else
  {
    digitalWrite(this->dir_pin, !this->dir);
  }
}


void PololuStepper::set_motor_direction(int dir)
{
  this->dir = dir;  
  this->update_dir_pin();
}
  
void PololuStepper::start_timer()
{
  this->active = true;
  TA0CCR0 =  300;
  TA0CCTL0 = CCIE;
  TA0CTL = TASSEL_2 + MC_1 + ID_3;  
}

void PololuStepper::stop_timer()
{
  this->active = false;
  TA0CCTL0 = 0;
  TA0CCR0 = 0;
}
  

