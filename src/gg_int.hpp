#include <NeoGPS_cfg.h>
#include <ublox/ubxGPS.h>
//------------------------------------------------------------
// Check that the config files are set up properly

#ifndef NMEAGPS_DERIVED_TYPES
  #error You must "#define NMEAGPS_DERIVED_TYPES" in NMEAGPS_cfg.h!
#endif

#if !defined(UBLOX_PARSE_STATUS) & !defined(UBLOX_PARSE_TIMEGPS) & \
    !defined(UBLOX_PARSE_TIMEUTC) & !defined(UBLOX_PARSE_POSLLH) & \
    !defined(UBLOX_PARSE_VELNED) & !defined(UBLOX_PARSE_SVINFO)  & \
    !defined(UBLOX_PARSE_DOP)

  #error No UBX binary messages enabled: no fix data available for fusing.

#endif

#if defined(UBLOX_PARSE_DOP) & \
    ( !defined(GPS_FIX_HDOP) & \
      !defined(GPS_FIX_VDOP) & \
      !defined(GPS_FIX_PDOP) )
  #warning UBX DOP message is enabled, but all GPS_fix DOP members are disabled.
#endif

#ifndef NMEAGPS_RECOGNIZE_ALL
  //  Resetting the messages with ublox::configNMEA requires that
  //    all message types are recognized (i.e., the enum has all
  //    values).
  #error You must "#define NMEAGPS_RECOGNIZE_ALL" in NMEAGPS_cfg.h!
#endif
