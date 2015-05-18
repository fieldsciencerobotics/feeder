#ifndef VoltageDivider_h
#define VoltageDivider_h

#include <Arduino.h>

class VoltageDivider
{
  public:
    VoltageDivider();
    void attach(int pin, int r1, int r2, int max_adc=4096, float vcc=3.3);
    float read_voltage();
    
  private:
    int pin, r1, r2, max_adc;
    float denominator, vcc;
};

#endif
