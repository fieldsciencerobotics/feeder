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

#ifndef regtable_h
#define regtable_h

#include "Arduino.h"
#include "register.h"
#include "commonregs.h"


/**
 * Register indicies
 */
 
DEFINE_REGINDEX_START()
  REGI_ACTION,
  REGI_ACTION_EVENT,
  REGI_MEAT_LOADED_THRESH,
  REGI_MEAT_DROPPED_DIFF,
  REGI_PERCH_OCCUPIED_THRESH,
  REGI_PERCH_VACANT_DIFF,
  REGI_MOTOR_DIRECTION,
  REGI_MAX_MEAT_PIECES,
  REGI_HEART_BEAT,
  REGI_ACK_HEART_BEAT,  
  REGI_CONFIG,
  REGI_GET_CONFIG,
  REGI_BATTERY_VOLTAGE,
  REGI_MEAT_PIECES_LEFT,
  REGI_PERCH_EVENT
DEFINE_REGINDEX_END()

#endif
