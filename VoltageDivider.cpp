#include "VoltageDivider.h"

VoltageDivider::VoltageDivider()
{

}

void VoltageDivider::attach(int pin, int r1, int r2, int max_adc, float vcc)
{
  this->pin = pin;
  this->r1 = r1;
  this->r2 = r2;
  this->denominator = (float)r2 / (r1 + r2);
  this->vcc = vcc;
  this->max_adc = max_adc;
}

//Returns cell voltage
float VoltageDivider::read_voltage()
{  	
  int value = analogRead(this->pin);
  float voltage = ((float)value / this->max_adc * this->vcc) / this->denominator;
  return voltage;
} 


//void LipoMonitor::set_balance_pins(int bal_41_pin, int bal_31_pin, int bal_21_pin)
//{
//  this->bal_41_pin = bal_41_pin;
//  this->bal_31_pin = bal_31_pin;
//  this->bal_21_pin = bal_21_pin;
//}
//
//void LipoMonitor::set_resistors(int r1, int r2, int r3, int r4, int r5, int r6)
//{
//  this->r1 = r1;
//  this->r2 = r2;
//  this->r3 = r3;
//  this->r4 = r4;
//  this->r5 = r5;
//  this->r6 = r6;
//
//  
//  this->bal_31_den = (float)r4 / (r3 + r4);
//  this->bal_21_den = (float)r6 / (r5 + r6);
//}

