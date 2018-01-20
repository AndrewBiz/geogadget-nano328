// #include <Arduino.h>
#include "gg_debug.hpp"
#include "gg_gps.hpp"
#include <NeoGPS_cfg.h>
#include <ublox/ubxGPS.h>
#include <GPSport.h>

const unsigned int ACQ_DOT_INTERVAL = 500UL;

//--------------------------
GPS::GPS(Stream* device) : ubloxGPS(device) {
  state = GETTING_SIGNAL;
}

//--------------------------
void GPS::start_running() {
  // disabling NMEA 'spam'
  for (uint8_t i=NMEAGPS::NMEA_FIRST_MSG; i<=NMEAGPS::NMEA_LAST_MSG; i++) {
    ublox::configNMEA( *this, (NMEAGPS::nmea_msg_t) i, 0 );
  }
  // initially disabling all messages
  disable_msg(ublox::UBX_NAV, ublox::UBX_NAV_STATUS);
  disable_msg(ublox::UBX_NAV, ublox::UBX_NAV_TIMEGPS);
  disable_msg(ublox::UBX_NAV, ublox::UBX_NAV_TIMEUTC);
  disable_msg(ublox::UBX_NAV, ublox::UBX_NAV_VELNED);
  disable_msg(ublox::UBX_NAV, ublox::UBX_NAV_POSLLH);
  disable_msg(ublox::UBX_NAV, ublox::UBX_NAV_DOP);
  disable_msg(ublox::UBX_NAV, ublox::UBX_NAV_SVINFO);

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

  state = GETTING_SIGNAL;
} // start_running

//--------------------------
void GPS::get_signal() {
  static bool acquiring = false;
  static uint32_t dotPrint;

  lock();
  bool              safe = is_safe();
  NeoGPS::clock_t   sow  = GPSTime::start_of_week();
  gps_fix::status_t status = fix().status;
  unlock();

  if (safe && (sow != 0) && (status != gps_fix::STATUS_NONE)) {
    D(if (acquiring) DEBUG_PORT << '\n';)
    D(DEBUG_PORT << F("Acquired Satus: ") << status << '\n';)
    D(DEBUG_PORT << F("Acquired Start-of-Week: ") << sow << '\n';)
    // go to running state
    state = RUNNING;
  } else {
    if (!acquiring) {
      acquiring = true;
      dotPrint = millis();
      D(DEBUG_PORT.print(F("Getting signal..."));)
    } else if (millis() - dotPrint > ACQ_DOT_INTERVAL) {
      dotPrint = millis();
      DEBUG_PORT.print(F("."));
    }
  }
} // get_signal


//--------------------------
bool GPS::running() {
  switch (state) {
    case GETTING_SIGNAL      : get_signal      (); break;
  }
  return (state == RUNNING);
} // running
