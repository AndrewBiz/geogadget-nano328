#include "gg_cfg.hpp"
#include "gg_display.hpp"
#include "gg_version.hpp"
#include "gg_format.hpp"

const uint8_t number_spin_steps = 8;
const char spin_step[number_spin_steps] PROGMEM = {'|','/','-','\\','|','/','-','\\'};
const char format_sta_sat[] PROGMEM = "ST %1d SAT: %02d(%02d)";


// Definition section
void GG_Display::init() {
  _display.begin();
};

void GG_Display::clear() {
  _display.clear();
}

void GG_Display::sleep() {
  _display.setPowerSave(1); //activates power save on display
};

void GG_Display::wakeup() {
  _display.setPowerSave(0); //deactivates power save on display
};

void GG_Display::_sta_sat(const NMEAGPS & gps, const gps_fix & fix) {
  char _buf[17];
  uint8_t sats_ok = 0;

  for (uint8_t i=0; i < gps.sat_count; i++) {
    if ((gps.satellites[i].tracked) && (gps.satellites[i].snr > SAT_GOOD_SNR)) {
      sats_ok++;
    }
  }
  sprintf_P(_buf, format_sta_sat,
    fix.status,
    sats_ok,
    fix.satellites
  );
  _display.print(_buf);
}

void GG_Display::_ymd(const NMEAGPS & gps, const gps_fix & fix) {
  char _buf[17];
  _display.print(F("YMD: "));
  _display.print(
    format_date( _buf, '-',
      fix.dateTime.full_year(fix.dateTime.year),
      fix.dateTime.month,
      fix.dateTime.date
    )
  );
}

void GG_Display::_hms(const NMEAGPS & gps, const gps_fix & fix) {
  char _buf[17];
  _display.print(F("HMS: "));
  _display.print(
    format_time( _buf, ':',
      fix.dateTime.hours,
      fix.dateTime.minutes,
      fix.dateTime.seconds
    )
  );
}

void GG_Display::show_init_screen(const NMEAGPS & gps, const gps_fix & fix) {
  static uint32_t spin_phase_time = 0;
  static uint8_t spin_phase = 0;

  _display.setFont(u8x8_font_artossans8_r);

  _display.setCursor(3, 0);
  _display.print(F("Geo-Gadget"));
  _display.setCursor(3, 1);
  _display.print(F("v " GG_VERSION));

  _display.setCursor(7, 2);
  // _display.print(F("getting signal"));
  // _display.setCursor(15, 2);
  if (millis() - spin_phase_time > 100) {
    // spin phases clockwise: "|/-\|/-\"
    spin_phase_time = millis();
    _display.print((char)pgm_read_byte(&(spin_step[spin_phase++])));
    if (spin_phase >= number_spin_steps) spin_phase = 0;
  }

  _display.setCursor(0, 4);
  _sta_sat(gps, fix);

  if ((!fix.valid.time) || (!fix.valid.date)) _display.setInverseFont(1);
  _display.setCursor(0, 5);
  _ymd(gps, fix);
  _display.setCursor(0, 6);
  _hms(gps, fix);
  _display.setInverseFont(0);
}


void GG_Display::show_main_screen(const NMEAGPS & gps, const gps_fix & fix, char* gg_file_name) {
  char _buf[17];

  _display.setFont(u8x8_font_artossans8_r);

  _display.setCursor(0, 0);
  _sta_sat(gps, fix);

  if ((!fix.valid.time) || (!fix.valid.date)) _display.setInverseFont(1);
  _display.setCursor(0, 1);
  _ymd(gps, fix);
  _display.setCursor(0, 2);
  _hms(gps, fix);
  _display.setInverseFont(0);

  _display.setCursor(0, 3);
  if (!fix.valid.location || fix.status == gps_fix::STATUS_NONE)
    _display.setInverseFont(1);
  _display.print(F("LAT: "));
  _display.print(format_location(_buf, fix.latitudeL()));

  _display.setCursor(0, 4);
  _display.print(F("LON: "));
  _display.print(format_location(_buf, fix.longitudeL()));
  _display.setInverseFont(0);

  _display.setCursor(0, 5);
  if (!fix.valid.altitude) _display.setInverseFont(1);
  static const char fmt_dsp_alt[] PROGMEM = "ALT: %+6d m";
  sprintf_P(_buf,
    fmt_dsp_alt,
    int(fix.altitude())
  );
  _display.print(_buf);
  _display.setInverseFont(0);

  _display.setCursor(0, 6);
  static const char fmt_dsp_err[] PROGMEM = "ERR: %4d,%4d m";
  sprintf_P(_buf,
    fmt_dsp_err,
    int(fix.lat_err()),
    int(fix.alt_err())
  );
  _display.print(_buf);

  _display.setCursor(0, 7);
  _display.print(F("LOG: "));
  _display.print(gg_file_name);
}
