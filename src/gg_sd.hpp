#include <stdint.h>
#include "gg_cfg.hpp"

class NMEAGPS;
class gps_fix;

extern char gg_file_name[];

//--------------------------
// Log a data record to SD card file
void log_fix(const NMEAGPS& gps, const gps_fix& fix);
// SD card initializing
void setup_sd(const NMEAGPS& gps, const gps_fix& fix);
