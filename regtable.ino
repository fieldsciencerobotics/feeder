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

#include "product.h"
#include "regtable.h"
#include "byteops.h"


/**
 * Define common registers
 */
 
DEFINE_COMMON_REGISTERS()

const void set_meat_loaded_thresh(byte rId, byte *value);
const void set_meat_dropped_thresh(byte rId, byte *value);
const void set_motor_direction(byte rId, byte *value);
const void set_max_meat_pieces(byte rId, byte *value);
const void set_action(byte rId, byte *value);
const void send_action_event(byte rId);
const void send_battery_voltage(byte rId);
const void send_meat_pieces_left(byte rId);
const void send_perch_event(byte rId);

/*
 * Define custom registers
 */
 
//Define custom registers
static byte dt_action[2];
REGISTER reg_action(dt_action, sizeof(dt_action), NULL, &set_action);

static byte dt_action_event[2];
REGISTER reg_action_event(dt_action_event, sizeof(dt_action_event), &send_action_event, NULL);

static byte dt_meat_loaded_thresh[2];
REGISTER reg_meat_loaded_thresh(dt_meat_loaded_thresh, sizeof(dt_meat_loaded_thresh), NULL, &set_meat_loaded_thresh, SWDTYPE_OTHER, 0, INFOMEM_SECTION_B);

static byte dt_meat_dropped_diff[2];
REGISTER reg_meat_dropped_diff(dt_meat_dropped_diff, sizeof(dt_meat_dropped_diff), NULL, &set_meat_dropped_diff, SWDTYPE_OTHER, 2, INFOMEM_SECTION_B);

static byte dt_perch_occupied_thresh[2];
REGISTER reg_perch_occupied_thresh(dt_perch_occupied_thresh, sizeof(dt_perch_occupied_thresh), NULL, &set_perch_occupied_thresh, SWDTYPE_OTHER, 6, INFOMEM_SECTION_B);

static byte dt_perch_vacant_diff[2];
REGISTER reg_perch_vacant_diff(dt_perch_vacant_diff, sizeof(dt_perch_vacant_diff), NULL, &set_perch_vacant_diff, SWDTYPE_OTHER, 4, INFOMEM_SECTION_B);

static byte dt_motor_direction[2];
REGISTER reg_motor_direction(dt_motor_direction, sizeof(dt_motor_direction), NULL, &set_motor_direction, SWDTYPE_OTHER, 8, INFOMEM_SECTION_B);

static byte dt_max_meat_pieces[2];
REGISTER reg_max_meat_pieces(dt_max_meat_pieces, sizeof(dt_max_meat_pieces), NULL, &set_max_meat_pieces, SWDTYPE_OTHER, 10, INFOMEM_SECTION_B);

static byte dt_heart_beat[2];
REGISTER reg_heart_beat(dt_heart_beat, sizeof(dt_heart_beat), &send_heart_beat, NULL);

static byte dt_ack_heart_beat[2];
REGISTER reg_ack_heart_beat(dt_ack_heart_beat, sizeof(dt_ack_heart_beat), NULL, &set_ack_heart_beat);

static byte dt_config[12];
REGISTER reg_config(dt_config, sizeof(dt_config), &send_config, NULL);

static byte dt_get_config[2];
REGISTER reg_get_config(dt_get_config, sizeof(dt_get_config), NULL, &set_get_config);

static byte dt_battery_voltage[6];
REGISTER reg_battery_voltage(dt_battery_voltage, sizeof(dt_battery_voltage), &send_battery_voltage, NULL);

static byte dt_meat_pieces_left[2];
REGISTER reg_meat_pieces_left(dt_meat_pieces_left, sizeof(dt_meat_pieces_left), &send_meat_pieces_left, NULL);

static byte dt_perch_event[2];
REGISTER reg_perch_event(dt_perch_event, sizeof(dt_perch_event), &send_perch_event, NULL);


/**
 * Table of registers
 */
 
DECLARE_REGISTERS_START()
  &reg_action, //11
  &reg_action_event, //12
  &reg_meat_loaded_thresh, //13 
  &reg_meat_dropped_diff,  //14
  &reg_perch_occupied_thresh, //15
  &reg_perch_vacant_diff, //16
  &reg_motor_direction, //17
  &reg_max_meat_pieces, //18
  &reg_heart_beat, //19
  &reg_ack_heart_beat, //20
  &reg_config, //21
  &reg_get_config, //22
  &reg_battery_voltage, //23
  &reg_meat_pieces_left, //24
  &reg_perch_event //25
DECLARE_REGISTERS_END()


/**
 * Define common getter/setter callback functions
 */
 
DEFINE_COMMON_CALLBACKS()


/**
 * Define custom getter/setter callback functions
 */

const void set_meat_loaded_thresh(byte rId, byte *value)
{
  int val;
  byte_to_int(value, val);

  if(val >= 10 && val < 4096)
  {
    memcpy(regTable[rId]->value, value, sizeof(regTable[rId]->value));
    reg_to_int(swap.getRegister(REGI_MEAT_LOADED_THRESH), MEAT_LOADED_THRESH);
  }
}

const void set_meat_dropped_diff(byte rId, byte *value)
{
  int val;
  byte_to_int(value, val);

  if(val >= 10 && val < 4096)
  {  
    memcpy(regTable[rId]->value, value, sizeof(regTable[rId]->value));
    reg_to_int(swap.getRegister(REGI_MEAT_DROPPED_DIFF), MEAT_DROPPED_DIFF);
  }
}

const void set_perch_occupied_thresh(byte rId, byte *value)
{
  int val;
  byte_to_int(value, val);

  if(val >= 10 && val < 4096)
  {  
    memcpy(regTable[rId]->value, value, sizeof(regTable[rId]->value));
    reg_to_int(swap.getRegister(REGI_PERCH_OCCUPIED_THRESH), PERCH_OCCUPIED_THRESH);
  }
}

const void set_perch_vacant_diff(byte rId, byte *value)
{
  int val;
  byte_to_int(value, val);

  if(val >= 10 && val < 4096)
  {  
    memcpy(regTable[rId]->value, value, sizeof(regTable[rId]->value)); 
    reg_to_int(swap.getRegister(REGI_PERCH_VACANT_DIFF), PERCH_VACANT_DIFF);
  }
}

const void set_motor_direction(byte rId, byte *value)
{
  int val;
  byte_to_int(value, val);

  if(val >= 0 && val <= 1)
  {  
    memcpy(regTable[rId]->value, value, sizeof(regTable[rId]->value));
    reg_to_int(swap.getRegister(REGI_MOTOR_DIRECTION), MOTOR_DIRECTION);
    stepper.set_motor_direction(MOTOR_DIRECTION);
  }
}

const void set_max_meat_pieces(byte rId, byte *value)
{
  int val;
  byte_to_int(value, val);

  if(val >= 0 && val <= 244)
  {
    memcpy(regTable[rId]->value, value, sizeof(regTable[rId]->value));
    reg_to_int(swap.getRegister(REGI_MAX_MEAT_PIECES), MAX_MEAT_PIECES);
  }
}

const void set_ack_heart_beat(byte rId, byte *value)
{
  hb_state = Ack;
  hb_count = 0;
  memcpy(regTable[rId]->value, value, sizeof(regTable[rId]->value));
  swap.getRegister(REGI_HEART_BEAT)->getData();
  swap.getRegister(REGI_ACTION_EVENT)->getData();
  swap.getRegister(REGI_MEAT_PIECES_LEFT)->getData();
}

const void send_heart_beat(byte rId)
{
  int_to_reg(swap.getRegister(REGI_HEART_BEAT), hb_state);
}

const void set_action(byte rId, byte *value)
{
  memcpy(regTable[rId]->value, value, sizeof(regTable[rId]->value));
  int action_id = -1;
  reg_to_int(swap.getRegister(REGI_ACTION), action_id);
  action_callback(action_id);
}

const void send_config(byte rId)
{
  REGISTER* reg = swap.getRegister(REGI_CONFIG);
  int_to_reg(reg, MEAT_LOADED_THRESH, 0, 1);
  int_to_reg(reg, MEAT_DROPPED_DIFF, 2, 3);
  int_to_reg(reg, PERCH_OCCUPIED_THRESH, 4, 5);
  int_to_reg(reg, PERCH_VACANT_DIFF, 6, 7);
  int_to_reg(reg, MOTOR_DIRECTION, 8, 9);
  int_to_reg(reg, MAX_MEAT_PIECES, 10, 11);
}

const void set_get_config(byte rId, byte *value)
{
  swap.getRegister(REGI_CONFIG)->getData();
}

const void send_perch_event(byte rId)
{
  int_to_reg(swap.getRegister(REGI_PERCH_EVENT), perch_state);
}

const void send_action_event(byte rId)
{
  int_to_reg(swap.getRegister(REGI_ACTION_EVENT), feeder_state);
}

const void send_battery_voltage(byte rId)
{
  float d1_val = d1.read_voltage();
  float d2_val = d2.read_voltage(); 
  float d3_val = d3.read_voltage();
  
  int cell_1_mv = int((d1_val - d2_val) * MILLIVOLT);
  int cell_2_mv = int((d2_val - d3_val) * MILLIVOLT);
  int cell_3_mv = int(d3_val * MILLIVOLT);

  REGISTER* reg = swap.getRegister(REGI_BATTERY_VOLTAGE);
  int_to_reg(reg, cell_1_mv, 0, 1);
  int_to_reg(reg, cell_2_mv, 2, 3);
  int_to_reg(reg, cell_3_mv, 4, 5);
}

const void send_meat_pieces_left(byte rId)
{
  int_to_reg(swap.getRegister(REGI_MEAT_PIECES_LEFT), meat_pieces_left);
}
