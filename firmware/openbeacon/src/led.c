/*! \file led.c
  \author Travis Goodspeed
  \brief LED Startup Animations for TNH
*/

#include "timer.h"
#include "config.h"

void led_startup(){
  volatile register i;
  
  LED1_LIT;
  msleep(10);
  LED2_LIT;
  msleep(10);
  LED3_LIT;
  msleep(10);
  LED1_DIM;
  msleep(10);
  LED2_DIM;
  msleep(10);
  LED3_DIM;
  msleep(10);
  
  
  LED1_DIM;
  LED2_DIM;
  LED3_DIM;
  
}
