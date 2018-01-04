// #include <Arduino.h>
#include "gg_debug.hpp"
#include "gg_gps.hpp"
#include <NeoGPS_cfg.h>
#include <ublox/ubxGPS.h>
#include <GPSport.h>
#include <Streamers.h>

const unsigned int ACQ_DOT_INTERVAL = 500UL;

//--------------------------
GPS::GPS(Stream * device) : ubloxGPS(device) {
  state = GETTING_STATUS;
}

//--------------------------
void GPS::start_running() {

  if (!enable_msg( ublox::UBX_NAV, ublox::UBX_NAV_STATUS )){
    D(DEBUG_PORT.println(F("enable UBX_NAV_STATUS failed!"));)
  } else {
    D(DEBUG_PORT.println(F("enabled UBX_NAV_STATUS"));)
  }

  if (!enable_msg( ublox::UBX_NAV, ublox::UBX_NAV_TIMEGPS )) {
    D(DEBUG_PORT.println(F("enable UBX_NAV_TIMEGPS failed!"));)
  } else {
    D(DEBUG_PORT.println(F("enabled UBX_NAV_TIMEGPS"));)
  }

  if (!enable_msg(ublox::UBX_NAV, ublox::UBX_NAV_TIMEUTC)) {
    D(DEBUG_PORT.println(F("enable UBX_NAV_TIMEUTC failed!"));)
  } else {
    D(DEBUG_PORT.println(F("enabled UBX_NAV_TIMEUTC"));)
  }


  if (!enable_msg(ublox::UBX_NAV, ublox::UBX_NAV_POSLLH)) {
    D(DEBUG_PORT.println(F("enable UBX_NAV_POSLLH failed!"));)
  } else {
    D(DEBUG_PORT.println(F("enabled UBX_NAV_POSLLH"));)
  }

  #if (defined(GPS_FIX_SPEED) | defined(GPS_FIX_HEADING)) & defined(UBLOX_PARSE_VELNED)
    if (!enable_msg(ublox::UBX_NAV, ublox::UBX_NAV_VELNED)) {
      D(DEBUG_PORT.println(F("enable UBX_NAV_VELNED failed!"));)
    } else {
      D(DEBUG_PORT.println(F("enabled UBX_NAV_VELNED"));)
    }
  #endif

  #if defined(UBLOX_PARSE_DOP)
    if (!enable_msg(ublox::UBX_NAV, ublox::UBX_NAV_DOP)) {
      D(DEBUG_PORT.println(F("enable UBX_NAV_DOP failed!"));)
    } else {
      D(DEBUG_PORT.println(F("enabled UBX_NAV_DOP"));)
    }
  #endif

  #if (defined(GPS_FIX_SATELLITES) | defined(NMEAGPS_PARSE_SATELLITES)) & defined(UBLOX_PARSE_SVINFO)
    if (!enable_msg(ublox::UBX_NAV, ublox::UBX_NAV_SVINFO)) {
      D(DEBUG_PORT.println(F("enable UBX_NAV_SVINFO failed!"));)
    } else {
      D(DEBUG_PORT.println(F("enabled UBX_NAV_SVINFO"));)
    }
  #endif

  state = GETTING_STATUS;
} // start_running

//--------------------------
void GPS::get_status() {
  static bool acquiring = false;

  if (fix().status == gps_fix::STATUS_NONE) {
    static uint32_t dotPrint;

    if (!acquiring) {
      acquiring = true;
      dotPrint = millis();
      DEBUG_PORT.print(F("Getting STATUS..."));
    } else if (millis() - dotPrint > ACQ_DOT_INTERVAL) {
      dotPrint = millis();
      DEBUG_PORT << '.';
    }

  } else {
    if (acquiring) DEBUG_PORT << '\n';
    DEBUG_PORT << F("Acquired status: ") << (uint8_t) fix().status << '\n';
    state = GETTING_LEAP_SECONDS;
    // state = RUNNING;
  }
} // get_status

//--------------------------
void GPS::get_leap_seconds() {
  static bool acquiring = false;
  static uint32_t dotPrint;

  if (GPSTime::leap_seconds != 0) {
    if (acquiring) DEBUG_PORT << '\n';
    DEBUG_PORT << F("Acquired leap seconds: ") << GPSTime::leap_seconds << '\n';
    state = GETTING_UTC;
  } else {
    if (!acquiring) {
      acquiring = true;
      dotPrint = millis();
      DEBUG_PORT.print(F("Getting leap seconds..."));
    } else if (millis() - dotPrint > ACQ_DOT_INTERVAL) {
      dotPrint = millis();
      DEBUG_PORT << '.';
    }
  }
} // get_leap_seconds

//--------------------------
void GPS::get_utc() {
  static bool acquiring = false;
  static uint32_t dotPrint;

  lock();
  bool            safe = is_safe();
  NeoGPS::clock_t sow  = GPSTime::start_of_week();
  NeoGPS::time_t  utc  = fix().dateTime;
  unlock();

  if (safe && (sow != 0)) {
    if (acquiring) DEBUG_PORT << '\n';
    DEBUG_PORT << F("Acquired UTC: ") << utc << '\n';
    DEBUG_PORT << F("Acquired Start-of-Week: ") << sow << '\n';
    // go to running state
    state = RUNNING;
  } else {
    if (!acquiring) {
      acquiring = true;
      dotPrint = millis();
      DEBUG_PORT.print(F("Getting UTC..."));
    } else if (millis() - dotPrint > ACQ_DOT_INTERVAL) {
      dotPrint = millis();
      DEBUG_PORT << '.';
    }
  }
} // get_utc

//--------------------------
bool GPS::running() {
  switch (state) {
    case GETTING_STATUS      : get_status      (); break;
    case GETTING_LEAP_SECONDS: get_leap_seconds(); break;
    case GETTING_UTC         : get_utc         (); break;
  }
  return (state == RUNNING);
} // running
