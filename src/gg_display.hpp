#ifndef GG_DISPLAY_HPP
#define GG_DISPLAY_HPP

#include <U8x8lib.h>
#include <ublox/ubxGPS.h>
#include "gg_debug.hpp"


/*****************************************************************************
   Display Class
*****************************************************************************/
class GG_Display {
public:
  void init();
  void clear();
  void sleep();
  void wakeup();

  void show_init_screen(const NMEAGPS & gps, const gps_fix & fix);
  void show_main_screen(const NMEAGPS & gps, const gps_fix & fix, char* gg_file_name);
  void show_error_screen(const uint8_t error_type, PGM_P error_str);

private:
  U8X8_SSD1306_128X64_NONAME_HW_I2C _display;

  void _sta_sat(const NMEAGPS & gps, const gps_fix & fix);
  void _ymd(const NMEAGPS & gps, const gps_fix & fix);
  void _hms(const NMEAGPS & gps, const gps_fix & fix);
};

#endif
