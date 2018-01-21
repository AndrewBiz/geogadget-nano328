#include <Arduino.h>
#include <ublox/ubxGPS.h>
#include <GPSport.h>
#include <U8x8lib.h>
#include <NeoICSerial.h>
#define GPS_PORT_NAME "NeoICSerial(RX pin 8, TX pin 9)"
#include <PinButton.h>

#include "gg_version.hpp"
#include "gg_int.hpp"
#include "gg_debug.hpp"
#include "gg_gps.hpp"
#include "gg_sd.hpp"
#include "gg_format.hpp"

// AltSoftSerial gpsPort;
NeoICSerial gpsPort; // 8 & 9 for an UNO

const uint8_t modeButtonPin = 2;
PinButton modeButton(modeButtonPin);

enum gg_mode_t {LOGGING_DISPLAY, TO_LOGGING_NORMAL, LOGGING_NORMAL, TO_LOGGING_DISPLAY};
enum gg_mode_t gg_mode = TO_LOGGING_DISPLAY; //The current mode

// unsigned long timeNow = 0;
// unsigned long timePrevious = 0;
// const unsigned int interval = 200;

const uint16_t  NORMAL_RATE = 5000; //ms = 1 tick per 5 sec
const uint16_t  FAST_RATE = 1000;   //ms = 1 tick per 1 sec = 1Hz

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
const char format_sta_sat[] PROGMEM = "ST %1d SAT: %02d(%02d)";
const uint8_t SAT_GOOD_SNR = 20;
char _buf[17];

void display_sta_sat(const NMEAGPS & gps, const gps_fix & fix) {
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

  if (not fix.valid.date) u8x8.setInverseFont(1);
  u8x8.print(F("YMD: "));
  u8x8.print(
    format_date( _buf, '-',
      fix.dateTime.full_year(fix.dateTime.year),
      fix.dateTime.month,
      fix.dateTime.date
    )
  );
  u8x8.setInverseFont(0);
}

void display_hms(const NMEAGPS & gps, const gps_fix & fix) {

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

void displaydata_init(const NMEAGPS & gps, const gps_fix & fix) {
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

  u8x8.setCursor(0, 5);
  display_ymd(gps, fix);

  u8x8.setCursor(0, 6);
  display_hms(gps, fix);
}

void displaydata(const NMEAGPS & gps, const gps_fix & fix) {

  u8x8.setFont(u8x8_font_artossans8_r);

  u8x8.setCursor(0, 0);
  display_sta_sat(gps, fix);

  u8x8.setCursor(0, 1);
  display_ymd(gps, fix);

  u8x8.setCursor(0, 2);
  display_hms(gps, fix);

  u8x8.setCursor(0, 3);
  if (not fix.valid.location || fix.status == gps_fix::STATUS_NONE)
    u8x8.setInverseFont(1);
  u8x8.print(F("LAT: "));
  u8x8.print(format_location(_buf, fix.latitudeL()));

  u8x8.setCursor(0, 4);
  u8x8.print(F("LON: "));
  u8x8.print(format_location(_buf, fix.longitudeL()));
  u8x8.setInverseFont(0);

  u8x8.setCursor(0, 5);
  if (not fix.valid.altitude) u8x8.setInverseFont(1);
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

//--------------------------
void setup() {
  DEBUG_PORT.begin(9600);
  while (!DEBUG_PORT)
    ;

  u8x8.begin();

  D(DEBUG_PORT.println(F("Geo-Gadget v" GG_VERSION));)
  D(DEBUG_PORT << F("fix object size = ") << sizeof(gps.fix()) << '\n';)
  D(DEBUG_PORT << F("GPS object size = ") << sizeof(gps) << '\n';)
  D(DEBUG_PORT.flush();)

  // GPS device initializing
  gpsPort.begin(9600);
  D(DEBUG_PORT.println(F("Looking for GPS device on " GPS_PORT_NAME));)

  gps.start_running();
  bool running = false;
  do {
    if (gps.available(gpsPort)) {
      fix = gps.read();
      running = gps.running();
    }
    displaydata_init(gps, fix);
    // _dumpPort(gpsPort, DEBUG_PORT, 1200);
  } while (not running);

  // SD card initializing
  setup_sd(gps, fix);

  D(trace_header(DEBUG_PORT);)
  gg_mode = TO_LOGGING_DISPLAY;
}

//--------------------------
void loop() {
  // timeNow = millis();
  modeButton.update();

//   switch (gg_mode) {
//
//     case LOGGING_DISPLAY:
// DEBUG_PORT.println(F("Enter LOGGING_DISPLAY"));
//       if (gps.available(gpsPort)) {
//         fix = gps.read();
//         displaydata(gps, fix);
//         log_fix(gps, fix);
//       }
//       if (modeButton.isSingleClick()) {
//         gg_mode = TO_LOGGING_NORMAL;
//       }
//       break;
//
//     case TO_LOGGING_NORMAL:
// DEBUG_PORT.println(F("Enter TO_LOGGING_NORMAL"));
//       gps.set_rate(NORMAL_RATE);
//       u8x8.setPowerSave(1); //activates power save on display
//       gg_mode = LOGGING_NORMAL;
//       break;
//
//     case LOGGING_NORMAL:
// DEBUG_PORT.println(F("Enter LOGGING_NORMAL"));
//       if (gps.available(gpsPort)) {
//         fix = gps.read();
//         log_fix(gps, fix);
//       }
//       if (modeButton.isSingleClick()) {
//         gg_mode = TO_LOGGING_DISPLAY;
//       }
//       break;
//
//     case TO_LOGGING_DISPLAY:
// DEBUG_PORT.println(F("Enter TO_LOGGING DISPLAY"));
//       gps.set_rate(FAST_RATE);  // 1Hz normally
//       u8x8.setPowerSave(0); //deactivates power save on display
//       clear_display();
//       gg_mode = LOGGING_DISPLAY;
//       break;
//
//     default:
//       break;
//   }

  // if (modeButton.isSingleClick()) {
  //   display_is_sleeping = !display_is_sleeping;
  //   if (display_is_sleeping) {
  //     u8x8.setPowerSave(1); //activates power save
  //   } else {
  //     u8x8.setPowerSave(0); //deactivates power save
  //   }
  // }
  if (gps.available(gpsPort)) {
    fix = gps.read();
    // D(trace_all(DEBUG_PORT, gps, fix);)
    displaydata(gps, fix);
    log_fix(gps, fix);
  }
}
