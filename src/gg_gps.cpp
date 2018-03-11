#include "gg_cfg.hpp"
#include "gg_debug.hpp"
#include "gg_gps.hpp"
#include <NeoGPS_cfg.h>
#include <ublox/ubxGPS.h>
#include <GPSport.h>

const unsigned int ACQ_DOT_INTERVAL = 500UL;

//--------------------------
// UBLOX device constants section

const unsigned char ubx_cfg_prt_fast[] PROGMEM = {
  0x06, 0x00,             // ID
  0x14, 0x00,             // 20b
  0x01,                   // portID
  0x00,                   // r0
  0x00, 0x00,             // txReady
  0xD0, 0x08, 0x00, 0x00, // mode => 0000 1000 (no parity) 1101(8bit reserved)0000
  // baudRate  80 25 00 00 baudrate = 0x25 0x80 = 9600low byte
  (uint8_t) GPS_UART_BAUDRATE_FAST,
    (uint8_t) (GPS_UART_BAUDRATE_FAST >> 8),
      (uint8_t) (GPS_UART_BAUDRATE_FAST >> 16),
        (uint8_t) (GPS_UART_BAUDRATE_FAST >> 24),
  0x01, 0x00,             // inProtoMask   0x01 = UBX only
  0x01, 0x00,             // outProtoMask  0x01 = UBX only
  0x00, 0x00,             // flags
  0x00, 0x00              // r5
};


#if defined(UBLOX_POWER_SAVE_MODE)
const uint32_t GPS_SEARCH_PERIOD = 20000; // ms, 20 sec
const uint16_t GPS_MIN_ACQ_TIME = 0; // s

const unsigned char ubx_cfg_pm2_cyclic[] PROGMEM = {
  0x06, 0x3B,             // ID CFG-PM2
  0x2C, 0x00,             // len = 002C=44b
  0x01,                   // version
  0x06,                   // reserved1
  0x00,                   // r2
  0x00,                   // r3
  0x00, 0x98, 0x02, 0x00, // flags: 0000 0000 0000 0010:1001 1000 0000 0000 ()
  // update Period (e.g. 5000ms = 0x1388 => 0x88, 0x13, 0x00, 0x00)
  (uint8_t) NORMAL_RATE,  // low byte
    (uint8_t) (NORMAL_RATE >> 8),
      0x00,
        0x00,
  // searchPeriod (e.g. 10000ms = 0x2710 => 0x10, 0x27, 0x00, 0x00)
  (uint8_t) GPS_SEARCH_PERIOD,  // low byte
    (uint8_t) (GPS_SEARCH_PERIOD >> 8),
      (uint8_t) (GPS_SEARCH_PERIOD >> 16),
        (uint8_t) (GPS_SEARCH_PERIOD >> 24),
  0x00, 0x00, 0x00, 0x00, // gridOffset
  0x00, 0x00,             // onTime
  // minAcqTime
  (uint8_t) GPS_MIN_ACQ_TIME,  // low byte
    (uint8_t) (GPS_MIN_ACQ_TIME >> 8),
  0x2C, 0x01,             // r4
  0x00, 0x00,             // r5
  0x4F, 0xC1, 0x03, 0x00, // r6
  0x86, 0x02, 0x00, 0x00, // r7
  0xFE,                   // r8
  0x00,                   // r9
  0x00, 0x00,             // r10
  0x64, 0x40, 0x01, 0x00  // r11
};

const unsigned char ubx_cfg_rxm_power_save[] PROGMEM = {
  0x06, 0x11,   // ID CFG-RXM
  0x02, 0x00,   // len = 2b
  0x08,         // reserved = 08
  0x01          // lpMode (01 = PowerSave)
};
#endif

const unsigned char ubx_cfg_rxm_power_max[] PROGMEM = {
  0x06, 0x11,   // ID CFG-RXM
  0x02, 0x00,   // len = 2b
  0x08,         // reserved = 08
  0x00          // lpMode (00 = Max Power)
};

// CFG-TP5
#if defined(UBLOX_POWER_SAVE_MODE)
  const uint32_t GPS_PULSE_NOFIX_PERIOD = 7000000; // us, 7 sec (for cyclyc mode)
#else
  const uint32_t GPS_PULSE_NOFIX_PERIOD = 10000000; // us, 10 sec (for normal power mode)
#endif
const uint32_t GPS_PULSE_FIX_PERIOD_NORMAL = (uint32_t)NORMAL_RATE * 1000; // us, 5 sec
const uint32_t GPS_PULSE_FIX_PERIOD_FAST = (uint32_t)FAST_RATE * 1000; // us, 1 sec
const uint32_t GPS_PULSE_NOFIX_LEN = 1000; // us, 0.001 sec
const uint32_t GPS_PULSE_FIX_LEN = 1000;     // us, 0.001 sec

const unsigned char ubx_cfg_tp5_power_save[] PROGMEM = {
  0x06, 0x31,   // ID CFG-TP5
  0x20, 0x00,   // len = 32b
  0x00,         // tpIdx 0 = TIMEPULSE
  0x01,         // r1
  0x00, 0x00,   // r2
  0x32, 0x00,   // antCableDelay 0x0032 = 50ns
  0x00, 0x00,   // rfGroupDelay
  // freqPeriod = 0x007A1200 = 8000000 us
  (uint8_t) GPS_PULSE_NOFIX_PERIOD,  // low byte
    (uint8_t) (GPS_PULSE_NOFIX_PERIOD >> 8),
      (uint8_t) (GPS_PULSE_NOFIX_PERIOD >> 16),
        (uint8_t) (GPS_PULSE_NOFIX_PERIOD >> 24),
  // freqPeriodLock = 0x004C4B40 = 5000000 us
  (uint8_t) GPS_PULSE_FIX_PERIOD_NORMAL,  // low byte
    (uint8_t) (GPS_PULSE_FIX_PERIOD_NORMAL >> 8),
      (uint8_t) (GPS_PULSE_FIX_PERIOD_NORMAL >> 16),
        (uint8_t) (GPS_PULSE_FIX_PERIOD_NORMAL >> 24),
  // pulseLenRatio = 0x000186A0 = 100000 us
  (uint8_t) GPS_PULSE_NOFIX_LEN,  // low byte
    (uint8_t) (GPS_PULSE_NOFIX_LEN >> 8),
      (uint8_t) (GPS_PULSE_NOFIX_LEN >> 16),
        (uint8_t) (GPS_PULSE_NOFIX_LEN >> 24),
  // pulseLenRatioLock = 0x03E8 = 1000 us
  (uint8_t) GPS_PULSE_FIX_LEN,  // low byte
    (uint8_t) (GPS_PULSE_FIX_LEN >> 8),
      (uint8_t) (GPS_PULSE_FIX_LEN >> 16),
        (uint8_t) (GPS_PULSE_FIX_LEN >> 24),
  0x00, 0x00, 0x00, 0x00, // userConfigDelay
  0x37, 0x00, 0x00, 0x00  // flags = 0x37 = 0011 0111
};

const unsigned char ubx_cfg_tp5_power_max[] PROGMEM = {
  0x06, 0x31,   // ID CFG-TP5
  0x20, 0x00,   // len = 32b
  0x00,         // tpIdx 0 = TIMEPULSE
  0x01,         // r1
  0x00, 0x00,   // r2
  0x32, 0x00,   // antCableDelay 0x0032 = 50ns
  0x00, 0x00,   // rfGroupDelay
  // freqPeriod = 0x007A1200 = 8000000 us
  (uint8_t) GPS_PULSE_NOFIX_PERIOD,  // low byte
    (uint8_t) (GPS_PULSE_NOFIX_PERIOD >> 8),
      (uint8_t) (GPS_PULSE_NOFIX_PERIOD >> 16),
        (uint8_t) (GPS_PULSE_NOFIX_PERIOD >> 24),
  // freqPeriodLock = 0x004C4B40 = 5000000 us
  (uint8_t) GPS_PULSE_FIX_PERIOD_FAST,  // low byte
    (uint8_t) (GPS_PULSE_FIX_PERIOD_FAST >> 8),
      (uint8_t) (GPS_PULSE_FIX_PERIOD_FAST >> 16),
        (uint8_t) (GPS_PULSE_FIX_PERIOD_FAST >> 24),
  // pulseLenRatio = 0x000186A0 = 100000 us
  (uint8_t) GPS_PULSE_NOFIX_LEN,  // low byte
    (uint8_t) (GPS_PULSE_NOFIX_LEN >> 8),
      (uint8_t) (GPS_PULSE_NOFIX_LEN >> 16),
        (uint8_t) (GPS_PULSE_NOFIX_LEN >> 24),
  // pulseLenRatioLock = 0x03E8 = 1000 us
  (uint8_t) GPS_PULSE_FIX_LEN,  // low byte
    (uint8_t) (GPS_PULSE_FIX_LEN >> 8),
      (uint8_t) (GPS_PULSE_FIX_LEN >> 16),
        (uint8_t) (GPS_PULSE_FIX_LEN >> 24),
  0x00, 0x00, 0x00, 0x00, // userConfigDelay
  0x37, 0x00, 0x00, 0x00  // flags = 0x37 = 0011 0111
};
// END OF: UBLOX device constants section

//--------------------------
GPS::GPS(Stream* device) : ubloxGPS(device) {
  state = GETTING_SIGNAL;
}

//--------------------------
void GPS::set_fast_baudrate() {
  write_P_simple(ubx_cfg_prt_fast, sizeof(ubx_cfg_prt_fast));
}

//--------------------------
bool GPS::set_rate(uint16_t rate) {
  return send(ublox::cfg_rate_t(rate, 1, ublox::UBX_TIME_REF_UTC));
}

//--------------------------
void GPS::go_power_save() {
  write_P_simple(ubx_cfg_tp5_power_save, sizeof(ubx_cfg_tp5_power_save));
  #if defined(UBLOX_POWER_SAVE_MODE)
    write_P_simple(ubx_cfg_pm2_cyclic, sizeof(ubx_cfg_pm2_cyclic));
    write_P_simple(ubx_cfg_rxm_power_save, sizeof(ubx_cfg_rxm_power_save));
  #endif
}

//--------------------------
void GPS::go_power_max() {
  write_P_simple(ubx_cfg_rxm_power_max, sizeof(ubx_cfg_rxm_power_max));
  write_P_simple(ubx_cfg_tp5_power_max, sizeof(ubx_cfg_tp5_power_max));
}

//--------------------------
void GPS::start_running() {
  go_power_max();

  // set 1Hz rate (1000ms)
  if (!set_rate(1000)) {
    // D(DEBUG_PORT.println(F("set rate FAILED!"));)
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
    // D(DEBUG_PORT.println(F("enable UBX_NAV_STATUS failed!"));)
  }

  if (!enable_msg( ublox::UBX_NAV, ublox::UBX_NAV_TIMEGPS )) {
    // D(DEBUG_PORT.println(F("enable UBX_NAV_TIMEGPS failed!"));)
  }

  if (!enable_msg(ublox::UBX_NAV, ublox::UBX_NAV_TIMEUTC)) {
    // D(DEBUG_PORT.println(F("enable UBX_NAV_TIMEUTC failed!"));)
  }

  if (!enable_msg(ublox::UBX_NAV, ublox::UBX_NAV_POSLLH)) {
    // D(DEBUG_PORT.println(F("enable UBX_NAV_POSLLH failed!"));)
  }

  #if (defined(GPS_FIX_SPEED) | defined(GPS_FIX_HEADING)) & defined(UBLOX_PARSE_VELNED)
    if (!enable_msg(ublox::UBX_NAV, ublox::UBX_NAV_VELNED)) {
      // D(DEBUG_PORT.println(F("enable UBX_NAV_VELNED failed!"));)
    }
  #endif

  #if defined(UBLOX_PARSE_DOP)
    if (!enable_msg(ublox::UBX_NAV, ublox::UBX_NAV_DOP)) {
      // D(DEBUG_PORT.println(F("enable UBX_NAV_DOP failed!"));)
    }
  #endif

  #if (defined(GPS_FIX_SATELLITES) | defined(NMEAGPS_PARSE_SATELLITES)) & defined(UBLOX_PARSE_SVINFO)
    if (!enable_msg(ublox::UBX_NAV, ublox::UBX_NAV_SVINFO)) {
      // D(DEBUG_PORT.println(F("enable UBX_NAV_SVINFO failed!"));)
    }
  #endif

  state = GETTING_SIGNAL;
} // start_running

//--------------------------
void GPS::get_signal() {
  lock();
  bool              safe    = is_safe();
  NeoGPS::clock_t   sow     = GPSTime::start_of_week();
  gps_fix::status_t status  = fix().status;
  unlock();

  if (safe && (sow != 0) && (status >= gps_fix::STATUS_STD)) {
    // go to running state
    state = RUNNING;
  }
} // get_signal


//--------------------------
bool GPS::running() {
  switch (state) {
    case GETTING_SIGNAL : get_signal(); break;
  }
  return (state == RUNNING);
} // running


//--------------------------
void GPS::write_P_simple(const unsigned char* progmem_msg, size_t len){
  m_device->print( (char) SYNC_1 );
  m_device->print( (char) SYNC_2 );
  uint8_t  crc_a = 0;
  uint8_t  crc_b = 0;
  while (len-- > 0) {
    uint8_t c = pgm_read_byte( progmem_msg++ );
    write(c, crc_a, crc_b);
  }
  m_device->print( (char) crc_a );
  m_device->print( (char) crc_b );
  m_device->flush();
};
