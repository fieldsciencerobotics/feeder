#ifndef byteops_h
#define byteops_h

#include "Arduino.h"
#include "register.h"

/**
 * Loads a two byte array register into an integer
 */

const void reg_to_int(REGISTER* reg, int& out, int start_i=0, int end_i=1)
{
  out = reg->value[start_i] << 8;
  out |= reg->value[end_i];
}

const void byte_to_int(byte *value, int& out, int start_i=0, int end_i=1)
{
  out = value[start_i] << 8;
  out |= value[end_i];
}

/**
 * Loads an integer into a two byte array
 */

const void int_to_reg(REGISTER* reg, int in, int start_i=0, int end_i=1)
{
  reg->value[start_i] = (in >> 8) & 0xFF;
  reg->value[end_i] = in & 0xFF;
}



#endif
