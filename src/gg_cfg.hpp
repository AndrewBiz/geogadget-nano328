#ifndef GG_CFG_HPP
#define GG_CFG_HPP

#include <stdint.h>

// SNR limit to consider the SAT is good (used only for display, no effect on GPS measurements)
const uint8_t SAT_GOOD_SNR = 20;

const uint32_t LOGGING_DISPLAY_INTERVAL = 20000; // ms, after this period of time GG goes to normal mode (display off)

// GPS fix rates
const uint16_t NORMAL_RATE = 5000; //ms = 1 tick per 5 sec
const uint16_t FAST_RATE = 1000;   //ms = 1 tick per 1 sec = 1Hz

const uint16_t NOFIX_LOGGING_INTERVAL = 300; //sec. log to SD every 5 min in nofix conditions

// Uncomment if you want save more power taking UBLOX device to cyclyc power save mode
// if you use UBLOX power save mode the GG current flow can be less than 20mA - this makes some power banks to switch off
// #define UBLOX_POWER_SAVE_MODE

// const uint32_t GPS_UART_BAUDRATE_FAST = 57600UL;  // no-work for NeoICSerial ! baudrate btw GPS device and MCU
const uint32_t GPS_UART_BAUDRATE_FAST = 19200UL;  // baudrate btw GPS device and MCU
// const uint32_t GPS_UART_BAUDRATE_FAST = 9600UL;  // baudrate btw GPS device and MCU

// Log file base name.  Must be six characters or less.
#define FILE_BASE_NAME "GGTRK"

#endif
