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

bool perch_triggered = false;
bool pushing_meat = false;
bool clearing_meat = false;
bool resetting = false;
bool meat_dropped = false;
bool device_reset = false;

int endstop_front_pin = 1;
int endstop_back_pin = 0;
int fsr_meat_pin = 12;
int fsr_perch_pin = 11;

const int MEAT_SENSED_THRESHOLD = 200;
const int MEAT_CLEARED_THRESHOLD = 100;
const int PERCH_TRIGGERED_THRESHOLD = 500;
const int PERCH_CLEAR_THRESHOLD = 200;

PololuStepper stepper;
Plunger plunger;
VoltageDivider d1;
VoltageDivider d2;
VoltageDivider d3;


DECLARE_COMMON_CALLBACKS()
DEFINE_COMMON_REGISTERS()

//Define custom registers
static byte dt_drop_meat[1];
REGISTER reg_drop_meat(dt_drop_meat, sizeof(dt_drop_meat), NULL, &set_push_meat);

static byte dt_reset[1];
REGISTER reg_reset(dt_reset, sizeof(dt_reset), NULL, &set_reset);

static byte dt_drop_meat_state[1];
REGISTER reg_drop_meat_state(dt_drop_meat_state, sizeof(dt_drop_meat_state), &updt_drop_meat_state, NULL);

static byte dt_reset_state[1];
REGISTER reg_reset_state(dt_reset_state, sizeof(dt_reset_state), &updt_reset_state, NULL);

static byte dt_req_batt_volt[1];
REGISTER reg_req_batt_volt(dt_req_batt_volt, sizeof(dt_req_batt_volt), NULL, &set_req_batt_volt);
                      
static byte dt_batt_volt[6];
REGISTER reg_batt_volt(dt_batt_volt, sizeof(dt_batt_volt), &updt_batt_volt, NULL);

static byte dt_perch_triggered[1];
REGISTER reg_perch_triggered(dt_perch_triggered, sizeof(dt_perch_triggered), &updt_perch_triggered, NULL);
                     
DECLARE_REGISTERS_START()
  &reg_drop_meat,  // 11
  &reg_reset,  // 12
  &reg_drop_meat_state, //13
  &reg_reset_state, //14
  &reg_req_batt_volt, //15
  &reg_batt_volt,  // 16
  &reg_perch_triggered,  // 17
DECLARE_REGISTERS_END()

DEFINE_COMMON_CALLBACKS()


void setup() {
  WDTCTL = WDTPW + WDTHOLD; //Stop watchdog timer
  Serial.begin(9600);
  
  //Setup end stops
  pinMode(endstop_front_pin, INPUT);
  pinMode(endstop_back_pin, INPUT);
  	
  //Setup interupts
  attachInterrupt(endstop_front_pin, endstop_front_high, RISING);
  attachInterrupt(endstop_back_pin, endstop_back_high, RISING);
  
  //Attach other devices
  stepper.attach(14, 15, 18, 17, 16);
  d1.attach(10, 10000, 3300);
  d2.attach(9, 10000, 5600);
  d3.attach(8, 10000, 10000);
  plunger.attach(2);
  plunger.pull_up(2);
      
  //Setup timer for ADC call
  TA1CCR0 =  12500; //10hz
  TA1CCTL0 = CCIE;
  TA1CTL = TASSEL_2 + MC_1 + ID_3;
  
  swap.init(); // Initialize SWAP registers
  swap.getRegister(REGI_PRODUCTCODE)->getData(); // Transmit product code
  swap.getRegister(REGI_RESET_STATE)->getData();
  swap.getRegister(REGI_DROP_MEAT_STATE)->getData();
  swap.getRegister(REGI_PERCH_TRIGGERED)->getData();
  swap.getRegister(REGI_BATT_VOLT)->getData();

  _BIS_SR(LPM0_bits + GIE); //LPM0_bits
}

//Start dropping meat
const void set_push_meat(byte rId, byte *junk)
{
  Serial.println("Starting to push meat");  
  memcpy(regTable[rId]->value, junk, sizeof(regTable[rId]->value));
  
  if(!pushing_meat and !resetting)
  {    
    pushing_meat = true;
    clearing_meat = false;
    device_reset = false;
    meat_dropped = false;
    swap.getRegister(REGI_DROP_MEAT_STATE)->getData();
    swap.getRegister(REGI_RESET_STATE)->getData();
    stepper.forward();
  }
}

//Reset device
const void set_reset(byte rId, byte *junk)
{  
  Serial.println("Resetting");
  memcpy(regTable[rId]->value, junk, sizeof(regTable[rId]->value));
  
  if(!resetting and !pushing_meat)
  {    
    resetting = true;
    device_reset = false;
    swap.getRegister(REGI_RESET_STATE)->getData();
    stepper.reverse();
  }
}

//Updates register whether feeder succeeded in dropping meat
const void updt_drop_meat_state(byte rId)
{
  regTable[rId]->value[0] = meat_dropped;
}

//Updates register whether feeder successfully reset itself
const void updt_reset_state(byte rId)
{
  regTable[rId]->value[0] = device_reset; 
}

//Send updated battery voltages to server
const void set_req_batt_volt(byte rId, byte *junk)
{
  Serial.println("Requesting battery voltages");
  memcpy(regTable[rId]->value, junk, sizeof(regTable[rId]->value));
  swap.getRegister(REGI_BATT_VOLT)->getData();
}

//Updates register with battery cell voltages
const void updt_batt_volt(byte rId)
{
  float d1_val = d1.read_voltage();
  float d2_val = d2.read_voltage(); 
  float d3_val = d3.read_voltage();
  
  float cell_1 = d1_val - d2_val;
  float cell_2 = d2_val - d3_val;
  float cell_3 = d3_val;
    
  Serial.print("Voltages: ");
  Serial.print(d1_val);
  Serial.print(", ");
  Serial.print(d2_val);
  Serial.print(", ");
  Serial.print(d3_val);
  Serial.println("");
  
  int cell_1_int = int(cell_1*100.0);
  int cell_2_int = int(cell_2*100.0);
  int cell_3_int = int(cell_3*100.0);
  
  regTable[rId]->value[0] = (cell_1_int >> 8) & 0xFF;
  regTable[rId]->value[1] = cell_1_int & 0xFF;
  
  regTable[rId]->value[2] = (cell_2_int >> 8) & 0xFF;
  regTable[rId]->value[3] = cell_2_int & 0xFF;
  
  regTable[rId]->value[4] = (cell_3_int >> 8) & 0xFF;
  regTable[rId]->value[5] = cell_3_int & 0xFF;  
}

//Updates register with current perch triggered value
const void updt_perch_triggered(byte rId)
{
  regTable[rId]->value[0] = perch_triggered;
}

//Called by pin interrupt when front endstop high
void endstop_front_high()
{
  Serial.println("Front end stop activated");
  
  if(pushing_meat)
  {
    stepper.stop();
    pushing_meat = false;
    clearing_meat = true;
  }
}

//Called by pin interrupt when back endstop high
void endstop_back_high()
{
  Serial.println("Back end stop activated");
  
  if(resetting)
  {
    stepper.stop();
    resetting = false;
    device_reset = true;
    swap.getRegister(REGI_RESET_STATE)->getData(); //Send confirmation
  }
}

// Interrupt routine, reads analogue force sensitive resitors and executes logic based on their values
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0 (void)
{  
  /*    
    MEAT
  */
  
  int meat_fsr = analogRead(fsr_meat_pin);  
  
  //Meat sensing logic
  if(pushing_meat and meat_fsr > MEAT_SENSED_THRESHOLD)
  {    
    stepper.stop();
    pushing_meat = false;
    clearing_meat = true;
    Serial.println("Meat in place");
  }
  
  //Meat clearing logic
  if(clearing_meat)
  {
    if(meat_fsr < MEAT_CLEARED_THRESHOLD)
    {
      clearing_meat = false;
      meat_dropped = true;
      swap.getRegister(REGI_DROP_MEAT_STATE)->getData();
      plunger.pull_up();
      Serial.println("Meat cleared");
    }
    else
    {
      Serial.println("Attempting to clear meat");
      plunger.push_down(2);
      plunger.pull_up(3);
      plunger.push_down(2);
      plunger.pull_up(3);
    }
  }
  
  /*    
    PERCH
  */
  
  int perch_fsr = analogRead(fsr_perch_pin);
  
  if(!perch_triggered && perch_fsr > PERCH_TRIGGERED_THRESHOLD)
  {  
    Serial.println("Perch triggered");
    perch_triggered = true;
    swap.getRegister(REGI_PERCH_TRIGGERED)->getData();
  }
  else if(perch_triggered && perch_fsr < PERCH_CLEAR_THRESHOLD)
  {
     Serial.println("Perch clear");
     perch_triggered = false;
     swap.getRegister(REGI_PERCH_TRIGGERED)->getData();
  }    
}

void loop(){}




