#include <Arduino.h>

// Log file base name.  Must be six characters or less.
#define FILE_BASE_NAME "GGTRK"

class NMEAGPS;
class gps_fix;

const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
extern char gg_file_name[];

//--------------------------
// Log a data record to SD card file
void log_fix(const NMEAGPS& gps, const gps_fix& fix);
// SD card initializing
void setup_sd(const NMEAGPS& gps, const gps_fix& fix);
