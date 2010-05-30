

unsigned int mclock=0, clock=0, timeout=0x1000;


#include "config.h"
#include "timer.h"


static char timer1_wrapped;

void timer1_init (void) {
  
  clock=mclock=0;
  
  //CCR0 = 0x100;                         // clock divider of 256
  CCR0 = 0x100;
  TACTL = TASSEL_1 + TACLR + MC_3 + TAIE;
  
  timer1_wrapped = 0;  
  timer1_go();
}


//! Set the timeout value of timer1.
void timer1_set (unsigned short tout) {
  clock=mclock=timer1_wrapped=0;
  timeout=tout;
  timer1_init();
}

void timer1_go () {
  WDTCTL = WDTPW + WDTHOLD;             // Stop WDT
  TACTL = TASSEL1 + TACLR;              // SMCLK, clear TAR
  CCTL0 = CCIE;                         // CCR0 interrupt enabled
  //CCR0 = 0x200;                         //clock divider
  TACTL |= MC_3;
  _EINT();                              // Enable interrupts 
  
  //MSP430
  CCTL0 |= CCIE;                        // CCR0 interrupt enabled
  _EINT();                              // Enable interrupts 
}

void timer1_sleep () {
  //mclock = clock = timer1_wrapped= 0;
  __bis_SR_register(CPUOFF + GIE);
  while (clock<timeout){
    //TODO Enter an LPM here.
  }
  timer1_wrapped=0;
}

void sleep_jiffies (unsigned long jiffies) {
  
  timer1_set (jiffies);
  timer1_sleep ();
  
  /* Software Code
  volatile long i=jiffies+2, j=10;
  while(j--){
    i=jiffies;
    while(i--);
  }
  */
}

void sleep_2ms (void) {
  sleep_jiffies (2 * TIMER1_JIFFIES_PER_MS);
}

void msleep(unsigned long i){
  if(i>0x8000)
    while(1)
      LED1_LIT;
  sleep_jiffies (i * TIMER1_JIFFIES_PER_MS);
}


// Timer A0 interrupt service routine
interrupt(TIMERA0_VECTOR) Timer_A (void){
  clock++;
  
  if(!clock)
    mclock++;
  if(clock>timeout){
    timer1_wrapped++;
    
    //Power up the CPU.
    __bic_SR_register_on_exit(CPUOFF);
  }
  return;
}
