#include "gg_debug.hpp"

DebugTools::DebugTools(uint8_t test_pin, uint8_t test_pin_state) :
  _test_pin(test_pin),
  _test_pin_state(test_pin_state)
{
  init();
};


void DebugTools::init(uint8_t test_pin, uint8_t test_pin_state) {
  _test_pin = test_pin;
  _test_pin_state = test_pin_state;
  init();
}


// private zone implementation
// ----------------------------------------------------------------------------
void DebugTools::init() {
  pinMode(_test_pin, OUTPUT);
  digitalWrite(_test_pin, _test_pin_state);
}

// -----------------------------------------------------------------------------
// void _dumpPort(Stream& port_in, Stream& port_out, unsigned int cycles) {
//   unsigned int bytes;
//   port_out.println(F("=PORT dump START="));
//   for (size_t i = 0; i < cycles; i++) {
//     delay(1);
//     bytes = port_in.available();
//     if (!bytes) {
//       port_out.print(F("*"));
//     }
//     for (size_t j = 0; j < bytes; j++) {
//       port_out.write(port_in.read());
//     }
//   }
//   port_out.println(F("-PORT dump STOP-"));
//   port_out.flush();
// }

// -----------------------------------------------------------------------------
// const uint16_t TICK_INTERVAL = 500; // ms
//
// void _tick(Stream& port_out) {
//   static uint32_t ts = millis();
//   uint32_t now = millis();
//   if ((now - ts) < TICK_INTERVAL) {
//     return;
//   }
//   port_out.print(now);
//   port_out.println(F(" tick"));
//   ts = now;
// };
