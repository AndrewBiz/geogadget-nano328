#include <Arduino.h>
//======================================================================
//  Program: gg.cpp - GeoGadget based on UBLOX GPS receiver
#define GG_VERSION "0.1.0"
//======================================================================

#include <ublox/ubxGPS.h>
#include <GPSport.h>
// TODO ! get rid of:
#include <Streamers.h>
#include <U8x8lib.h>
#include <AltSoftSerial.h>
#define GPS_PORT_NAME "AltSoftSerial(RX pin 8, TX pin 9)"
#include <PinButton.h>

#include "gg_int.hpp"
#include "gg_debug.hpp"
#include "gg_gps.hpp"
#include "gg_sd.hpp"

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
void clear_display() {
  u8x8.clear();
}

void displaydata(const NMEAGPS &gps, const gps_fix &fix) {
  u8x8.setFont(u8x8_font_artossans8_r);

  u8x8.setCursor(0, 0);
  u8x8.print(F("STA: "));
  if (fix.valid.status)
    u8x8.print(fix.status);

  u8x8.setCursor(0, 1);
  uint16_t d = 0;
  u8x8.print(F("YMD: "));
  u8x8.print(fix.dateTime.full_year(fix.dateTime.year));
  u8x8.print(F("."));
  d = fix.dateTime.month;
  if (d < 10) u8x8.print(F("0")); u8x8.print(d);
  u8x8.print(F("."));
  d = fix.dateTime.date;
  if (d < 10) u8x8.print(F("0")); u8x8.print(d);
  // if (fix.valid.date)
  //   u8x8.print(F("  "));
  // else
  //   u8x8.print(F(" ?"));
  u8x8.setCursor(0, 2);
  u8x8.print(F("HMS: "));
  d = fix.dateTime.hours;
  if (d < 10) u8x8.print(F("0")); u8x8.print(d);
  u8x8.print(F(":"));
  d = fix.dateTime.minutes;
  if (d < 10) u8x8.print(F("0")); u8x8.print(d);
  u8x8.print(F(":"));
  d = fix.dateTime.seconds;
  if (d < 10) u8x8.print(F("0")); u8x8.print(d);
  // if (fix.valid.time)
  //   u8x8.print(F("  "));
  // else
  //   u8x8.print(F(" ?"));

  char _float_buf11[12];
  u8x8.setCursor(0, 3);
  u8x8.print(F("LAT: "));
  dtostrf(fix.latitude(), 11, 6, _float_buf11);  /* 11 width, 6 precision */
  u8x8.print(_float_buf11);

  u8x8.setCursor(0, 4);
  u8x8.print(F("LON: "));
  dtostrf(fix.longitude(), 11, 6, _float_buf11);  /* 11 width, 6 precision */
  u8x8.print(_float_buf11);

  u8x8.setCursor(0, 5);
  u8x8.print(F("ALT: "));
  u8x8.print(int(trunc(fix.altitude())));
  u8x8.print(F(" m"));

  u8x8.clearLine(6);
  u8x8.setCursor(0, 6);
  u8x8.print(F("ERR: "));
  if (fix.valid.lat_err)
    u8x8.print(int(trunc(fix.lat_err())));
  else
    u8x8.print(F("?"));
  u8x8.print(F(","));
  if (fix.valid.lon_err)
    u8x8.print(int(trunc(fix.lon_err())));
  else
    u8x8.print(F("?"));
  u8x8.print(F(","));
  if (fix.valid.alt_err)
    u8x8.print(int(trunc(fix.alt_err())));
  else
    u8x8.print(F("?"));
  u8x8.print(F(" m"));

  u8x8.setCursor(0, 7);
  u8x8.print(F("LOG: "));
  u8x8.print(gg_file_name);
}

//--------------------------
static void configNMEA(uint8_t rate) {
  for (uint8_t i=NMEAGPS::NMEA_FIRST_MSG; i<=NMEAGPS::NMEA_LAST_MSG; i++) {
    ublox::configNMEA( gps, (NMEAGPS::nmea_msg_t) i, rate );
  }
}

//--------------------------
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

  DEBUG_PORT.println(F("GeoGadget V" GG_VERSION));
  D(DEBUG_PORT << F("fix object size = ") << sizeof(gps.fix()) << '\n';)
  D(DEBUG_PORT << F("GPS object size = ") << sizeof(gps) << '\n';)
  D(DEBUG_PORT.println(F("Looking for GPS device on " GPS_PORT_NAME));)
  DEBUG_PORT.flush();

  // GPS device initializing
  gpsPort.begin(9600);
  // TODO: put to start_running:
  configNMEA(0);
  disableUBX();

  // D(DEBUG_PORT.println(F("\n----------gps.START_RUNNING----------"));)
  gps.start_running();
  // _dumpPort(gpsPort, DEBUG_PORT, 1500UL);

  // int pstate = gps.state;
  bool running = false;
  do {
    if (gps.available(gpsPort)) {
      fix = gps.read();
      running = gps.running();
      // if (gps.state != pstate) {
      //   DEBUG_PORT.println(F("\n++++++++++gps.state CHANGED++++++++++"));
      //   _dumpPort(gpsPort, DEBUG_PORT, 1500UL);
      //   pstate = gps.state;
      // }
    }
  } while (not running);

  // SD card initializing
  setup_sd(gps, fix);

  D(trace_header(DEBUG_PORT);)
}

//--------------------------
void loop() {
  modeButton.update();
  if (modeButton.isSingleClick()) {
    // DEBUG_PORT.println(F("Button SingleClick!"));
    display_is_sleeping = !display_is_sleeping;
    // DEBUG_PORT.print(F("new display sleeping mode = "));
    // DEBUG_PORT.println(display_is_sleeping);
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
