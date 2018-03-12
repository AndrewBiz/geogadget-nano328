#include "gg_error.hpp"
#include "gg_display.hpp"


/*****************************************************************************
   Error handler Class
*****************************************************************************/

// public:
//-----------------------------------------------------------------------------
GG_Error::GG_Error(GG_Display* display):
  _display(display)
{};

//-----------------------------------------------------------------------------
void GG_Error::fatal(const GG_Error::Type error_type) {
  _display->init();
  _display->show_error_screen(
      (uint_fast8_t)error_type,
      (PGM_P)pgm_read_word(&(str_errors[(uint8_t)error_type])) //pointer to str in PROGMEM
  );
  while (1) {
    ::yield();
  }
};


// private:
