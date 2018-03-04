#include <Arduino.h>

// -----------------------------------------------------------------------------
void _dumpPort(Stream& port_in, Stream& port_out, unsigned int cycles) {
  unsigned int bytes;
  port_out.println(F("=PORT dump START="));
  for (size_t i = 0; i < cycles; i++) {
    delay(1);
    bytes = port_in.available();
    if (!bytes) {
      port_out.print(F("*"));
    }
    for (size_t j = 0; j < bytes; j++) {
      port_out.write(port_in.read());
    }
  }
  port_out.println(F("-PORT dump STOP-"));
  port_out.flush();
}

// -----------------------------------------------------------------------------
const uint8_t TICK_INTERVAL = 100; // ms

void _tick(Stream& port_out) {
  static uint32_t ts = millis();
  uint32_t now = millis();
  if ((now - ts) < TICK_INTERVAL) {
    return;
  }
  port_out.print(now);
  port_out.println(F(" tick"));
  ts = now;
};
