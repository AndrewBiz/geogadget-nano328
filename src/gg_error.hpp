#ifndef GG_ERROR_HPP
#define GG_ERROR_HPP

#include "gg_debug.hpp"


class GG_Display;

// error wordings:

char const str_error_0[] PROGMEM = "  SD card init";
char const str_error_1[] PROGMEM = "SD - CHDIR: root";
char const str_error_2[] PROGMEM = "SD - MKDIR: work";
char const str_error_3[] PROGMEM = "SD - CHDIR: work";
char const str_error_4[] PROGMEM = "SD - CREATE file";
char const str_error_5[] PROGMEM = "SD - timestamp";
char const str_error_6[] PROGMEM = "SD - WRITE file";


PGM_P const str_errors[] PROGMEM = {
  str_error_0,
  str_error_1,
  str_error_2,
  str_error_3,
  str_error_4,
  str_error_5,
  str_error_6
};


/*****************************************************************************
   Error handler Class
*****************************************************************************/
class GG_Error {
public:
  enum class Type : uint8_t {
    SD_INIT,
    SD_CHDIR_ROOT,
    SD_MKDIR_WORKDIR,
    SD_CHDIR_WORKDIR,
    SD_OPEN_FILE,
    SD_SET_TIMESTAMP,
    SD_FILE_WRITE
  };

  GG_Error(GG_Display* display);

  void fatal(const GG_Error::Type error_type);

private:
  GG_Display* _display;
};

#endif
