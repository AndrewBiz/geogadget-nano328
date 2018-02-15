#include <Arduino.h>
#include <ublox/ubxGPS.h>
#include <GPSport.h>
#include <NeoICSerial.h>
#define GPS_PORT_NAME "NeoICSerial(RX pin 8, TX pin 9)"
#include <PinButton.h>

#include "gg_cfg.hpp"
#include "gg_version.hpp"
#include "gg_int.hpp"
#include "gg_debug.hpp"
#include "gg_gps.hpp"
#include "gg_sd.hpp"
#include "gg_format.hpp"
#include "gg_display.hpp"

NeoICSerial gpsPort; // 8 & 9 for an UNO

GG_Display display;

const uint8_t modeButtonPin = 2;
PinButton modeButton(modeButtonPin);

enum class Mode : uint8_t {LOGGING_DISPLAY, TO_LOGGING_NORMAL, LOGGING_NORMAL, TO_LOGGING_DISPLAY} mode;

static GPS gps(&gpsPort);
static gps_fix fix;

//--------------------------
void setup() {
  D(DEBUG_PORT.begin(9600);)

  display.init();

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
    display.show_init_screen(gps, fix);
    // _dumpPort(gpsPort, DEBUG_PORT, 1200);
  } while (not running);

  // SD card initializing
  setup_sd(gps, fix);

  D(trace_header(DEBUG_PORT);)
  mode = Mode::TO_LOGGING_DISPLAY;
}

//--------------------------
void loop() {
  modeButton.update();

  switch (mode) {
    case Mode::LOGGING_DISPLAY:
      if (gps.available(gpsPort)) {
        fix = gps.read();
        display.show_main_screen(gps, fix, gg_file_name);
        log_fix(gps, fix);
      }
      if (modeButton.isSingleClick()) {
        mode = Mode::TO_LOGGING_NORMAL;
      }
      break;

    case Mode::TO_LOGGING_NORMAL:
      gps.set_rate(NORMAL_RATE);
      display.sleep();
      mode = Mode::LOGGING_NORMAL;
      break;

    case Mode::LOGGING_NORMAL:
      if (gps.available(gpsPort)) {
        fix = gps.read();
        log_fix(gps, fix);
      }
      if (modeButton.isSingleClick()) {
        mode = Mode::TO_LOGGING_DISPLAY;
      }
      break;

    case Mode::TO_LOGGING_DISPLAY:
      gps.set_rate(FAST_RATE);  // 1Hz normally
      display.wakeup();
      display.clear();
      mode = Mode::LOGGING_DISPLAY;
      break;

    default:
      ;
      break;
  }
}
