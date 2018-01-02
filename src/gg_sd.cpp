#include "gg_debug.hpp"
#include "gg_sd.hpp"
#include "gg_gps.hpp"

#include <SdFat.h>
// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))

const uint8_t chipSelect = SS;

SdFat sd;
SdFile gg_file;
char gg_file_name[13] = FILE_BASE_NAME "00.GPX";

//--------------------------
// Log a data record to SD card file
void log_fix(const NMEAGPS &gps, const gps_fix &fix) {
  gg_file.print(fix.dateTime); gg_file.write(',');
  gg_file.print(fix.latitude(), 6); gg_file.write(',');
  gg_file.print(fix.longitude(), 6); gg_file.write(',');
  gg_file.print(fix.altitude(), 1); gg_file.write(',');

  gg_file.println();
  // Force data to SD and update the directory entry to avoid data loss.
  if (!gg_file.sync() || gg_file.getWriteError()) {
    error("write error");
  }
}

// SD card initializing
void setup_sd(const NMEAGPS &gps, const gps_fix &fix) {
  // Initialize at the highest speed supported by the board that is
  // not over 50 MHz. Try a lower speed if SPI errors occur.
  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    sd.initErrorHalt();
  }
  // Find an unused file name.
  if (BASE_NAME_SIZE > 6) {
    error("file name too long");
  }
  while (sd.exists(gg_file_name)) {
    if (gg_file_name[BASE_NAME_SIZE + 1] != '9') {
      gg_file_name[BASE_NAME_SIZE + 1]++;
    } else if (gg_file_name[BASE_NAME_SIZE] != '9') {
      gg_file_name[BASE_NAME_SIZE + 1] = '0';
      gg_file_name[BASE_NAME_SIZE]++;
    } else {
      error("Can't create file name");
    }
  }
  if (!gg_file.open(gg_file_name, O_CREAT | O_WRITE | O_EXCL)) {
    error("gg_file.open");
  }
  // set file datetimes
  if (!gg_file.timestamp(T_CREATE, fix.dateTime.full_year(fix.dateTime.year), fix.dateTime.month, fix.dateTime.date, fix.dateTime.hours, fix.dateTime.minutes, fix.dateTime.seconds)) {
    error("set create time failed");
  }
  if (!gg_file.timestamp(T_WRITE, fix.dateTime.full_year(fix.dateTime.year), fix.dateTime.month, fix.dateTime.date, fix.dateTime.hours, fix.dateTime.minutes, fix.dateTime.seconds)) {
    error("set WRITE time failed");
  }
  if (!gg_file.timestamp(T_ACCESS, fix.dateTime.full_year(fix.dateTime.year), fix.dateTime.month, fix.dateTime.date, fix.dateTime.hours, fix.dateTime.minutes, fix.dateTime.seconds)) {
    error("set ACCESS time failed");
  }
  DEBUG_PORT.print(F("Logging to: "));
  DEBUG_PORT.println(gg_file_name);
}
