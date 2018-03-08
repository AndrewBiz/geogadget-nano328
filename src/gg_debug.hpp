#ifndef GG_DEBUG_H
#define GG_DEBUG_H

#include <Arduino.h>
#include <GPSport.h>

#ifdef DEBUG
#  define D(x) x
#else
#  define D(x)
#endif

// ----------------------------------------------------------------------------
class DebugTools {

public:
  DebugTools(uint8_t test_pin, uint8_t test_pin_state = LOW );
  void init(uint8_t test_pin, uint8_t test_pin_state = LOW);
  void inline toggle_test_pin(void);
  // void _dumpPort(Stream& port_in, Stream& port_out, unsigned int cycles);
  // void serial_tick(void);

private:
  uint8_t _test_pin;
  uint8_t _test_pin_state;
  void init(void);
};

// implementation zone
void inline DebugTools::toggle_test_pin() {
  _test_pin_state = !_test_pin_state;
  digitalWrite(_test_pin, _test_pin_state);
}

#endif
