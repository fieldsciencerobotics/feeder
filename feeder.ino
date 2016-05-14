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

#include <Arduino.h>
#include <cc430f5137.h>
#include "HardwareSerial.h"
#include "cc430f5137.h"
#include "wiring.h"
#include "PololuStepper.h"
#include "Plunger.h"
#include "VoltageDivider.h"
#include "regtable.h"
#include "product.h"
#include "swap.h"
#include "register.h"
#include "actions.h"
#include "byteops.h"

int endstop_front_pin = 1;
int endstop_back_pin = 0;
int fsr_meat_pin = 12;
int fsr_perch_pin = 11;

int MEAT_LOADED_THRESH = 200;
int MEAT_DROPPED_DIFF = 100;
int PERCH_OCCUPIED_THRESH = 2000;
int PERCH_VACANT_DIFF = 2500;
int MAX_MEAT_PIECES = 15;
int MOTOR_DIRECTION = 1;

const float MILLIVOLT = 1000.0;

PerchEvent perch_state = PerchVacant;
ActionEvent feeder_state = Stopped;
HeartBeat hb_state = InitA;
uint8_t meat_pieces_left = MAX_MEAT_PIECES;
int address = 0;
int address_max = 128;

PololuStepper stepper;
Plunger plunger;
VoltageDivider d1;
VoltageDivider d2;
VoltageDivider d3;

void setup() {
  WDTCTL = WDTPW + WDTHOLD; //Stop watchdog timer
  
  //Setup end stops
  pinMode(endstop_front_pin, INPUT);
  pinMode(endstop_back_pin, INPUT);
  	
  //Setup interupts
  attachInterrupt(endstop_front_pin, endstop_front_high, RISING);
  attachInterrupt(endstop_back_pin, endstop_back_high, RISING);
    
  swap.init();
  swap.getRegister(REGI_PRODUCTCODE)->getData();

  //Load default values from info memory
  reg_to_int(swap.getRegister(REGI_MEAT_LOADED_THRESH), MEAT_LOADED_THRESH);
  reg_to_int(swap.getRegister(REGI_MEAT_DROPPED_DIFF), MEAT_DROPPED_DIFF);
  reg_to_int(swap.getRegister(REGI_PERCH_OCCUPIED_THRESH), PERCH_OCCUPIED_THRESH);
  reg_to_int(swap.getRegister(REGI_PERCH_VACANT_DIFF), PERCH_VACANT_DIFF);
  reg_to_int(swap.getRegister(REGI_MOTOR_DIRECTION), MOTOR_DIRECTION);
  reg_to_int(swap.getRegister(REGI_MAX_MEAT_PIECES), MAX_MEAT_PIECES);
  load_state(feeder_state, meat_pieces_left);
  
  //Attach other devices
  stepper.attach(14, 15, 18, 17, 16, MOTOR_DIRECTION);
  d1.attach(10, 10000, 3300);
  d2.attach(9, 10000, 5600);
  d3.attach(8, 10000, 10000);
  plunger.attach(2);
  plunger.pull_up(2);
      
  //Setup timer for ADC call
  TA1CCR0 =  12500; //  hz
  TA1CCTL0 = CCIE;
  TA1CTL = TASSEL_2 + MC_1 + ID_3;
  _BIS_SR(LPM0_bits + GIE); //LPM0_bits
}

const void action_callback(int action_id)
{ 
  switch (action_id)
  {
    case Stop:
      stop_callback(); 
      break;
    case Reset:
      reset_callback(); 
      break;
    case PrimeMeat:
      prime_meat_callback(); 
      break;
    case DropMeat:
      drop_meat_callback(); 
      break;    
    default:
      break;
  }  
}

void stop_callback()
{  
  stepper.stop();
  feeder_state = Stopped;
  save_state(feeder_state, meat_pieces_left);
  swap.getRegister(REGI_ACTION_EVENT)->getData();
}

void reset_callback()
{
  int val = digitalRead(endstop_back_pin); //to check border case of reset when already reset
  
  if(!val)
  {    
    stepper.reverse();
    feeder_state = ResetStarted;
    save_state(feeder_state, meat_pieces_left);
    swap.getRegister(REGI_ACTION_EVENT)->getData();
  }
  else
  {
    meat_pieces_left = MAX_MEAT_PIECES;
    save_state(feeder_state, meat_pieces_left);
    swap.getRegister(REGI_MEAT_PIECES_LEFT)->getData();
  }
}

void prime_meat_callback()
{
  int val = digitalRead(endstop_front_pin); //to check border case of reset when empty
  
  if(!val && meat_pieces_left > 0)
  {    
    stepper.forward();
    feeder_state = PrimeMeatStarted;
    save_state(feeder_state, meat_pieces_left);
    swap.getRegister(REGI_ACTION_EVENT)->getData();
  }
}

void drop_meat_callback()
{
  if(feeder_state == PrimeMeatFinished && meat_pieces_left > 0)
  {
    feeder_state = DropMeatStarted;
    save_state(feeder_state, meat_pieces_left);
    swap.getRegister(REGI_ACTION_EVENT)->getData();
  }
}

//Called by pin interrupt when front endstop high
void endstop_front_high()
{
  if(feeder_state == PrimeMeatStarted)
  {
    stepper.stop();
    feeder_state = MeatEmpty;
    save_state(feeder_state, meat_pieces_left);
    swap.getRegister(REGI_ACTION_EVENT)->getData();
  }
}

//Called by pin interrupt when back endstop high
void endstop_back_high()
{  
  if(feeder_state == ResetStarted)
  {
    stepper.stop();
    feeder_state = ResetFinished;
    
    meat_pieces_left = MAX_MEAT_PIECES;
    save_state(feeder_state, meat_pieces_left);
    swap.getRegister(REGI_MEAT_PIECES_LEFT)->getData();
    
    swap.getRegister(REGI_ACTION_EVENT)->getData();
  }
}

void perch_callback()
{
  REGISTER* reg = swap.getRegister(REGI_PERCH_EVENT);
  int perch_fsr = analogRead(fsr_perch_pin);
  
  if(perch_state == PerchVacant && perch_fsr > PERCH_OCCUPIED_THRESH)
  {
    perch_state = PerchOccupied;    
    reg->getData();
  }
  else if(perch_state == PerchOccupied && perch_fsr < (PERCH_OCCUPIED_THRESH - PERCH_VACANT_DIFF))
  {
    perch_state = PerchVacant;
    reg->getData();
  }
}

int num_plunger_tries = 0;
int plunger_tries_thresh = 2;

void meat_sensor_callback()
{
  int meat_fsr = analogRead(fsr_meat_pin);
  
  //Priming finished logic
  if(feeder_state == PrimeMeatStarted && meat_fsr > abs(MEAT_LOADED_THRESH))
  {    
    stepper.stop();
    feeder_state = PrimeMeatFinished;
    save_state(feeder_state, meat_pieces_left);
    swap.getRegister(REGI_ACTION_EVENT)->getData();
  }
  
  //Meat dropping logic
  if(feeder_state == DropMeatStarted)
  {  
    if(num_plunger_tries == 0 || (meat_fsr > (abs(MEAT_LOADED_THRESH) - abs(MEAT_DROPPED_DIFF)) && num_plunger_tries < plunger_tries_thresh))
    {
      plunger.push_down(2);
      plunger.pull_up(3);
      num_plunger_tries += 1;
    }
    else
    {    
        plunger.pull_up();        
        feeder_state = DropMeatFinished;
        meat_pieces_left -= 1;
        save_state(feeder_state, meat_pieces_left); //Update and save num pieces of meat left
        
        num_plunger_tries = 0;
        swap.getRegister(REGI_ACTION_EVENT)->getData();
        swap.getRegister(REGI_MEAT_PIECES_LEFT)->getData();
        prime_meat_callback();
    }
  }
}

unsigned long bat_count = 0;
unsigned long hb_count = 0;
unsigned long BAT_NOTIF_THRESH = 7500; //60 seconds in hz
unsigned long HB_INIT_THRESH = 1250; //10 seconds in hz

// Interrupt routine, reads analogue force sensitive resitors and executes logic based on their values
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0 (void) 
{
  perch_callback();
  meat_sensor_callback();  

  if(!stepper.is_active())
  {
    if(bat_count > BAT_NOTIF_THRESH)
    {
      swap.getRegister(REGI_BATTERY_VOLTAGE)->getData();
      bat_count = 0;
    }
    
    if (hb_state == InitA && hb_count > HB_INIT_THRESH)
    {
      hb_state = InitB;
      hb_count = 0;
      swap.getRegister(REGI_HEART_BEAT)->getData();
    }
    else if (hb_state == InitB && hb_count > HB_INIT_THRESH)
    {
      hb_state = InitA;
      hb_count = 0;
      swap.getRegister(REGI_HEART_BEAT)->getData();
    }
  }

  hb_count++;
  bat_count++;
}

void save_state(ActionEvent feeder_state, uint8_t meat_left)
{
  STORAGE nvMem;
  uint8_t value[2];
  value[0] = feeder_state;
  value[1] = meat_left;

  uint8_t deflt[2];
  deflt[0] = 255;
  deflt[1] = 255;
  nvMem.write(deflt, INFOMEM_SECTION_C, address, sizeof(value));
  
  address = (address + 2) % address_max;
  nvMem.write(value, INFOMEM_SECTION_C, address, sizeof(value));
}

void load_state(ActionEvent &feeder_state, uint8_t &meat_left)
{
  STORAGE nvMem;
  
  for (int i = 0 ; i < address_max; i+=2) {
    uint8_t value[2];
    nvMem.read(value, INFOMEM_SECTION_C, i, sizeof(value));

    if(value[0] != 255)
    {
      address = i;
      feeder_state = ActionEvent(value[0]);
      meat_left = value[1];
      break;
    }
  }
}

void loop(){}

