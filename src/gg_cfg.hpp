#ifndef GG_CFG_HPP
#define GG_CFG_HPP

#include <stdint.h>

// SNR limit to consider the SAT is good
const uint8_t SAT_GOOD_SNR = 20;

// GPS fix rates
const uint16_t  NORMAL_RATE = 5000; //ms = 1 tick per 5 sec
const uint16_t  FAST_RATE = 1000;   //ms = 1 tick per 1 sec = 1Hz

// Log file base name.  Must be six characters or less.
#define FILE_BASE_NAME "GGTRK"

#endif
