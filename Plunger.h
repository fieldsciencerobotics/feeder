
#ifndef Plunger_h
#define Plunger_h

#include <Arduino.h>

class Plunger
{
  public:
    Plunger();
    void push_down(float seconds=1.0);
    void pull_up(float seconds=1.0);
    void attach(int pin);
    void stop();
  private:
    static const int down_pulse;
	static const int up_pulse;
	static const int period;
	int servo_pin;
};

#endif
