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

#ifndef PololuStepper_h
#define PololuStepper_h

#include <Arduino.h>
#include "HardwareSerial.h"

class PololuStepper
{
  public:
    PololuStepper();
    void attach(int ms1_pin, int ms2_pin, int ms3_pin, int step_pin, int dir_pin, int dir);
    void forward();
    void reverse();
    void stop();
    bool is_active();
    void set_motor_direction(int dir);
  private:
    int ms1_pin, ms2_pin, ms3_pin, dir_pin, dir; //dir, 0 is forward, 1 is reverse
    static bool motor_high;
    bool isforward;
    bool active;
    static int step_pin;
    void start_timer();
    void stop_timer();
    void update_dir_pin();
    __interrupt void Timer0_A0();
};

#endif
