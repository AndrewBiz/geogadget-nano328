#include <Arduino.h>
#include <ublox/ubxGPS.h>
#include <GPSport.h>
#include <U8x8lib.h>
#include <AltSoftSerial.h>
#define GPS_PORT_NAME "AltSoftSerial(RX pin 8, TX pin 9)"
#include <PinButton.h>

#include "gg_version.hpp"
#include "gg_int.hpp"
#include "gg_debug.hpp"
#include "gg_gps.hpp"
#include "gg_sd.hpp"
#include "gg_format.hpp"

AltSoftSerial gpsPort;

const uint8_t modeButtonPin = 2;
PinButton modeButton(modeButtonPin);

static GPS gps(&gpsPort);
static gps_fix fix;

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8;
bool display_is_sleeping = false;

/*****************************************************************************
   Display in diff modes
*****************************************************************************/
// TODO: move to gg_display
void clear_display() {
  u8x8.clear();
}

const uint8_t number_spin_steps = 8;
const char spin_step[number_spin_steps] PROGMEM = {'|','/','-','\\','|','/','-','\\'};

void displaydata_init(const NMEAGPS & gps, const gps_fix & fix) {
  static uint32_t spin_phase_time = 0;
  static uint8_t spin_phase = 0;

  char _buf[11];

  u8x8.setFont(u8x8_font_artossans8_r);

  u8x8.setCursor(0, 0);
  u8x8.print(F("   Geo-Gadget   "));
  u8x8.setCursor(0, 1);
  u8x8.print(F("     v" GG_VERSION "     "));

  u8x8.setCursor(0, 2);
  u8x8.print(F("getting signal "));
  u8x8.setCursor(15, 2);
  if (millis() - spin_phase_time > 100) {
    // spin phases clockwise: "|/-\|/-\"
    spin_phase_time = millis();
    u8x8.print((char)pgm_read_byte(&(spin_step[spin_phase++])));
    if (spin_phase >= number_spin_steps) spin_phase = 0;
  }
  u8x8.setCursor(0, 4);
  u8x8.print(F("STA: "));
  u8x8.print(fix.status);

  u8x8.setCursor(0, 5);
  if (not fix.valid.date) u8x8.setInverseFont(1);
  u8x8.print(F("YMD: "));
  u8x8.print(
    format_date( _buf, '.',
      fix.dateTime.full_year(fix.dateTime.year),
      fix.dateTime.month,
      fix.dateTime.date
    )
  );
  u8x8.setInverseFont(0);

  u8x8.setCursor(0, 6);
  if (not fix.valid.time) u8x8.setInverseFont(1);
  u8x8.print(F("HMS: "));
  u8x8.print(
    format_time( _buf, ':',
      fix.dateTime.hours,
      fix.dateTime.minutes,
      fix.dateTime.seconds
    )
  );
  u8x8.setInverseFont(0);
}

void displaydata(const NMEAGPS & gps, const gps_fix & fix) {
  char _buf[12];

  u8x8.setFont(u8x8_font_artossans8_r);

  if (not fix.valid.status) u8x8.setInverseFont(1);
  u8x8.setCursor(0, 0);
  u8x8.print(F("STA: "));
  u8x8.print(fix.status);
  u8x8.setInverseFont(0);

  u8x8.setCursor(0, 1);
  if (not fix.valid.date) u8x8.setInverseFont(1);
  u8x8.print(F("YMD: "));
  u8x8.print(
    format_date( _buf, '.',
      fix.dateTime.full_year(fix.dateTime.year),
      fix.dateTime.month,
      fix.dateTime.date
    )
  );
  u8x8.setInverseFont(0);

  u8x8.setCursor(0, 2);
  if (not fix.valid.time) u8x8.setInverseFont(1);
  u8x8.print(F("HMS: "));
  u8x8.print(
    format_time( _buf, ':',
      fix.dateTime.hours,
      fix.dateTime.minutes,
      fix.dateTime.seconds
    )
  );
  u8x8.setInverseFont(0);

  if (not fix.valid.location || fix.status == gps_fix::STATUS_NONE)
    u8x8.setInverseFont(1);
  u8x8.setCursor(0, 3);
  u8x8.print(F("LAT: "));
  dtostrf(fix.latitude(), 11, 6, _buf);  /* 11 width, 6 precision */
  u8x8.print(_buf);

  u8x8.setCursor(0, 4);
  u8x8.print(F("LON: "));
  dtostrf(fix.longitude(), 11, 6, _buf);  /* 11 width, 6 precision */
  u8x8.print(_buf);
  u8x8.setInverseFont(0);

  if (not fix.valid.altitude) u8x8.setInverseFont(1);
  u8x8.clearLine(5);
  u8x8.setCursor(0, 5);
  u8x8.print(F("ALT: "));
  u8x8.print(int(trunc(fix.altitude())));
  u8x8.print(F(" m"));
  u8x8.setInverseFont(0);

  u8x8.clearLine(6);
  u8x8.setCursor(0, 6);
  u8x8.print(F("ERR: "));
  u8x8.print(int(trunc(fix.lat_err())));
  u8x8.print(F(","));
  u8x8.print(int(trunc(fix.lon_err())));
  u8x8.print(F(","));
  u8x8.print(int(trunc(fix.alt_err())));
  u8x8.print(F(" m"));

  u8x8.setCursor(0, 7);
  u8x8.print(F("LOG: "));
  u8x8.print(gg_file_name);
}

//--------------------------
// TODO: move to gg_gps
static void configNMEA(uint8_t val) {
  for (uint8_t i=NMEAGPS::NMEA_FIRST_MSG; i<=NMEAGPS::NMEA_LAST_MSG; i++) {
    ublox::configNMEA( gps, (NMEAGPS::nmea_msg_t) i, val );
  }
}

//--------------------------
// TODO: move to gg_gps
static void disableUBX() {
  gps.disable_msg( ublox::UBX_NAV, ublox::UBX_NAV_STATUS );
  gps.disable_msg( ublox::UBX_NAV, ublox::UBX_NAV_TIMEGPS );
  gps.disable_msg( ublox::UBX_NAV, ublox::UBX_NAV_TIMEUTC );
  gps.disable_msg( ublox::UBX_NAV, ublox::UBX_NAV_VELNED );
  gps.disable_msg( ublox::UBX_NAV, ublox::UBX_NAV_POSLLH );
  gps.disable_msg( ublox::UBX_NAV, ublox::UBX_NAV_DOP );
  gps.disable_msg( ublox::UBX_NAV, ublox::UBX_NAV_SVINFO );
}

//--------------------------
void setup() {
  DEBUG_PORT.begin(9600);
  while (!DEBUG_PORT)
    ;

  u8x8.begin();

  DEBUG_PORT.println(F("Geo-Gadget v" GG_VERSION));
  D(DEBUG_PORT << F("fix object size = ") << sizeof(gps.fix()) << '\n';)
  D(DEBUG_PORT << F("GPS object size = ") << sizeof(gps) << '\n';)
  DEBUG_PORT.flush();

  // GPS device initializing
  gpsPort.begin(9600);
  D(DEBUG_PORT.println(F("Looking for GPS device on " GPS_PORT_NAME));)
  // TODO: put to start_running:
  configNMEA(0);
  disableUBX();

  gps.start_running();
  bool running = false;
  do {
    if (gps.available(gpsPort)) {
      fix = gps.read();
      running = gps.running();
    }
    displaydata_init(gps, fix);
    // _dumpPort(gpsPort, DEBUG_PORT, 1500);
  } while (not running);

  // SD card initializing
  setup_sd(gps, fix);

  D(trace_header(DEBUG_PORT);)
  clear_display();
}

//--------------------------
void loop() {
  modeButton.update();
  if (modeButton.isSingleClick()) {
    display_is_sleeping = !display_is_sleeping;
    if (display_is_sleeping) {
      u8x8.setPowerSave(1); //activates power save
    } else {
      u8x8.setPowerSave(0); //deactivates power save
    }
  }
  if (gps.available(gpsPort)) {
    fix = gps.read();
    D(trace_all(DEBUG_PORT, gps, fix);)

    displaydata(gps, fix);

    log_fix(gps, fix);
  }
}
