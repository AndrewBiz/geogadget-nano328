#ifndef GG_DISPLAY_HPP
#define GG_DISPLAY_HPP

#include <ublox/ubxGPS.h>
#include "gg_debug.hpp"


/*****************************************************************************
   Display in diff modes
*****************************************************************************/
// TODO: inline functions
// static inline void init_display();
// static inline void clear_display();
// static inline void display_sleep();
// static inline void display_wakeup();

void init_display();
void clear_display();
void display_sleep();
void display_wakeup();


void display_sta_sat(const NMEAGPS & gps, const gps_fix & fix);
void display_hms(const NMEAGPS & gps, const gps_fix & fix);
void displaydata_init(const NMEAGPS & gps, const gps_fix & fix);
void displaydata(const NMEAGPS & gps, const gps_fix & fix, char* gg_file_name);

#endif
