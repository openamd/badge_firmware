/*! \file led.c
  \author Travis Goodspeed
  \brief LED Startup Animations for TNH
*/

#include "timer.h"
#include "config.h"

void led_startup(){
  int i;
  
  for(i=0;i<5;i++){
    //Cycle three LEDs.
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
  }
  
  //Dim everything for low power mode.
  LED1_DIM;
  LED2_DIM;
  LED3_DIM;
  
}
