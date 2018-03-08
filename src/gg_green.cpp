#include "gg_green.hpp"
#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h> //Needed for powering down perihperals such as the ADC/TWI and Timers

const uint8_t BTN_INT = 0;   // button attached to PD2 - INT0
const uint8_t PPS_INT = 1;   // GPS PPS attached to PD3 - INT1

volatile bool int_btn_event = false;
volatile bool int_pps_event = false;


// here the interrupt is handled after wakeup
void wakeUpNow_PPS() {
  sleep_disable();   // to avoid  going to sleep with detached interrupt
  detachInterrupt(PPS_INT);
  detachInterrupt(BTN_INT);
  int_pps_event = true;
}

// here the interrupt is handled after wakeup
void wakeUpNow_BTN() {
  sleep_disable();   // to avoid  going to sleep with detached interrupt
  detachInterrupt(PPS_INT);
  detachInterrupt(BTN_INT);
  int_btn_event = true;
}
//--------------------------
void CPU_sleepNow() {
  int_btn_event = false;
  int_pps_event = false;
   /* The 5 different modes are:
   * SLEEP_MODE_IDLE -the least power savings
   * SLEEP_MODE_ADC
   * SLEEP_MODE_PWR_SAVE
   * SLEEP_MODE_STANDBY
   * SLEEP_MODE_PWR_DOWN -the most power savings
   */

  set_sleep_mode(SLEEP_MODE_STANDBY); // sleep mode is set here

  noInterrupts();

  PCICR = 0x00;  /// PinChange interrupt 0-1-2 disable

  sleep_enable(); // enables the sleep bit in the mcucr register

  power_adc_disable();
  // power_spi_disable();
  // power_timer0_disable();
  // power_timer1_disable();
  // power_timer2_disable();
  // power_twi_disable();
  sleep_bod_disable();

  attachInterrupt(BTN_INT, wakeUpNow_BTN, LOW);
  attachInterrupt(PPS_INT, wakeUpNow_PPS, LOW);

  interrupts();

  sleep_cpu(); // here the device is actually put to sleep!!
  // ...ZZZZZZZ.....
  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  sleep_disable(); // first thing after waking from sleep:
  // power_all_enable();
  // power_spi_enable(); //After wake up, power up peripherals
  // power_timer0_enable();
  // power_timer1_enable();
  // power_timer2_enable();
}
