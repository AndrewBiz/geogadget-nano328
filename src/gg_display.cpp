#include <U8x8lib.h>
#include "gg_cfg.hpp"
#include "gg_display.hpp"
#include "gg_version.hpp"
#include "gg_format.hpp"

const uint8_t number_spin_steps = 8;
const char spin_step[number_spin_steps] PROGMEM = {'|','/','-','\\','|','/','-','\\'};
const char format_sta_sat[] PROGMEM = "ST %1d SAT: %02d(%02d)";

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8;

void init_display() {
  u8x8.begin();
};

void clear_display() {
  u8x8.clear();
}

void display_sleep() {
  u8x8.setPowerSave(1); //activates power save on display
};

void display_wakeup() {
  u8x8.setPowerSave(0); //deactivates power save on display
};

void display_sta_sat(const NMEAGPS & gps, const gps_fix & fix) {
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
  u8x8.print(_buf);
}

void display_ymd(const NMEAGPS & gps, const gps_fix & fix) {
  char _buf[17];
  u8x8.print(F("YMD: "));
  u8x8.print(
    format_date( _buf, '-',
      fix.dateTime.full_year(fix.dateTime.year),
      fix.dateTime.month,
      fix.dateTime.date
    )
  );
}

void display_hms(const NMEAGPS & gps, const gps_fix & fix) {
  char _buf[17];
  u8x8.print(F("HMS: "));
  u8x8.print(
    format_time( _buf, ':',
      fix.dateTime.hours,
      fix.dateTime.minutes,
      fix.dateTime.seconds
    )
  );
}

void displaydata_init(const NMEAGPS & gps, const gps_fix & fix) {
  char _buf[17];
  static uint32_t spin_phase_time = 0;
  static uint8_t spin_phase = 0;

  u8x8.setFont(u8x8_font_artossans8_r);

  u8x8.setCursor(3, 0);
  u8x8.print(F("Geo-Gadget"));
  u8x8.setCursor(4, 1);
  u8x8.print(F("v" GG_VERSION));

  u8x8.setCursor(0, 2);
  u8x8.print(F("getting signal"));
  u8x8.setCursor(15, 2);
  if (millis() - spin_phase_time > 100) {
    // spin phases clockwise: "|/-\|/-\"
    spin_phase_time = millis();
    u8x8.print((char)pgm_read_byte(&(spin_step[spin_phase++])));
    if (spin_phase >= number_spin_steps) spin_phase = 0;
  }

  u8x8.setCursor(0, 4);
  display_sta_sat(gps, fix);

  if ((!fix.valid.time) || (!fix.valid.date)) u8x8.setInverseFont(1);
  u8x8.setCursor(0, 5);
  display_ymd(gps, fix);
  u8x8.setCursor(0, 6);
  display_hms(gps, fix);
  u8x8.setInverseFont(0);
}


void displaydata(const NMEAGPS & gps, const gps_fix & fix, char* gg_file_name) {
  char _buf[17];

  u8x8.setFont(u8x8_font_artossans8_r);

  u8x8.setCursor(0, 0);
  display_sta_sat(gps, fix);

  if ((!fix.valid.time) || (!fix.valid.date)) u8x8.setInverseFont(1);
  u8x8.setCursor(0, 1);
  display_ymd(gps, fix);
  u8x8.setCursor(0, 2);
  display_hms(gps, fix);
  u8x8.setInverseFont(0);

  u8x8.setCursor(0, 3);
  if (!fix.valid.location || fix.status == gps_fix::STATUS_NONE)
    u8x8.setInverseFont(1);
  u8x8.print(F("LAT: "));
  u8x8.print(format_location(_buf, fix.latitudeL()));

  u8x8.setCursor(0, 4);
  u8x8.print(F("LON: "));
  u8x8.print(format_location(_buf, fix.longitudeL()));
  u8x8.setInverseFont(0);

  u8x8.setCursor(0, 5);
  if (!fix.valid.altitude) u8x8.setInverseFont(1);
  static const char fmt_dsp_alt[] PROGMEM = "ALT: %+6d m";
  sprintf_P(_buf,
    fmt_dsp_alt,
    int(fix.altitude())
  );
  u8x8.print(_buf);
  u8x8.setInverseFont(0);

  u8x8.setCursor(0, 6);
  static const char fmt_dsp_err[] PROGMEM = "ERR: %4d,%4d m";
  sprintf_P(_buf,
    fmt_dsp_err,
    int(fix.lat_err()),
    int(fix.alt_err())
  );
  u8x8.print(_buf);

  u8x8.setCursor(0, 7);
  u8x8.print(F("LOG: "));
  u8x8.print(gg_file_name);
}
