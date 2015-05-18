
#ifndef PololuStepper_h
#define PololuStepper_h

#include <Arduino.h>
#include "HardwareSerial.h"

class PololuStepper
{
  public:
    PololuStepper();
    void attach(int ms1_pin, int ms2_pin, int ms3_pin, int step_pin, int dir_pin);
    void forward();
    void reverse();
    void stop();
  private:
    int ms1_pin, ms2_pin, ms3_pin, dir_pin;
    static bool motor_high;
    static int step_pin;
    void start_timer();
    void stop_timer();
    __interrupt void Timer0_A0();
};

#endif
