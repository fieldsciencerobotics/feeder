#ifndef regtable_h
#define regtable_h

#include "Arduino.h"
#include "register.h"
#include "commonregs.h"

/**
 * Register indexes
 */
DEFINE_REGINDEX_START()
  REGI_DROP_MEAT,
  REGI_RESET,
  REGI_DROP_MEAT_STATE,
  REGI_RESET_STATE,
  REGI_REQ_BATT_VOLTAGES,
  REGI_BATT_VOLT,
  REGI_PERCH_TRIGGERED
DEFINE_REGINDEX_END()

#endif
