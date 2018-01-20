#include "gg_format.hpp"
#include <Arduino.h>

const char format_date_solid[] PROGMEM = "%04d%02d%02d";
const char format_date_dash[] PROGMEM =  "%04d-%02d-%02d";

//--------------------------
char* format_date(char* buf, char ftype, uint16_t year, uint8_t month, uint8_t date) {
  const char* _fmt;
  switch (ftype) {
    case '-': _fmt = format_date_dash; break;
    default : _fmt = format_date_solid; break;
  }
  sprintf_P(buf, _fmt,
    year,
    month,
    date
  );
  return buf;
}

const char format_time_solid[] PROGMEM = "%02d%02d%02d";
const char format_time_colon[] PROGMEM = "%02d:%02d:%02d";

//--------------------------
char* format_time(char* buf, char ftype, uint8_t hours, uint8_t minutes, uint8_t seconds) {
  const char* _fmt;
  switch (ftype) {
    case ':': _fmt = format_time_colon; break;
    default : _fmt = format_time_solid; break;
  }
  sprintf_P(buf, _fmt,
    hours,
    minutes,
    seconds
  );
  return buf;
}

//----Location format section ---------------------
const int32_t location_SCALE = 10000000L;

int location_whole(int32_t val) {
    return (int) (val / location_SCALE);
}

unsigned long location_frac(int32_t val) {
    return (unsigned long) ((val % location_SCALE) / 10);
}

const char fmt_location_int32_p[] PROGMEM =  "+%03d.%06lu";
const char fmt_location_int32_n[] PROGMEM =  "-%03d.%06lu";

//--------------------------
char* format_location(char* buf, int32_t val) {
  const char* _fmt;
  if (val >= 0) {
    _fmt = fmt_location_int32_p;
  } else {
    _fmt = fmt_location_int32_n;
    val = -val;
  }
  sprintf_P(buf,
    _fmt,
    location_whole(val),
    location_frac(val)
  );
  return buf;
}
