#include "gg_cfg.hpp"
#include "gg_version.hpp"
#include "gg_debug.hpp"
#include "gg_sd.hpp"
#include "gg_gps.hpp"
#include "gg_format.hpp"
#include "gg_error.hpp"

#include <SdFat.h>
// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))

const uint8_t FILE_NAME_DIGIT_H = sizeof(FILE_BASE_NAME) - 1;
const uint8_t FILE_NAME_DIGIT_L = FILE_NAME_DIGIT_H + 1;

// Seek to fileSize + this position before writing track points.
#define GPX_ENDING_OFFSET -24
#define GPX_ENDING "\t</trkseg></trk>\n</gpx>\n"

const uint8_t chipSelect = SS;

SdFat sd;
SdFile gg_file;
char gg_file_name[] = FILE_BASE_NAME "00.GPX";
char gg_dir_name[9] = "";

//-----------------------------------------------------------------------------
// Log a data record to SD card file
void log_fix(const NMEAGPS& gps, const gps_fix& fix, GG_Error& error) {
  char _buf[15];
  static NeoGPS::clock_t ts_nofix_detected = 0;

  NeoGPS::clock_t ts_current = fix.dateTime;
  bool nofix = (!fix.valid.location) || (fix.status == gps_fix::STATUS_NONE);
  bool noalt = (!fix.valid.altitude);

  // checks if fix is ok to be recorded
  if (nofix) {
    if (ts_nofix_detected > 0) {
      // checking if it is high time to record to the log
      if ((ts_current - ts_nofix_detected) < NOFIX_LOGGING_INTERVAL) {
        return;   // exit with no logging to SD card
      } else {
        ts_nofix_detected = ts_current;
        // go log it!
      }
    } else {
      ts_nofix_detected = ts_current;
      return;  // exit with no logging to SD card
    }
  } else {  // location is valid
    ts_nofix_detected = 0;
  }

  gg_file.seekEnd(GPX_ENDING_OFFSET);
  gg_file.print(F("\t\t<trkpt "));

  gg_file.print(F("lat=\""));
  gg_file.print(format_location(_buf, fix.latitudeL()));
  gg_file.print(F("\" lon=\""));
  gg_file.print(format_location(_buf, fix.longitudeL()));
  gg_file.print(F("\">"));

  gg_file.print(F("<fix>"));
  if (nofix) {
    gg_file.print(F("none"));
  } else {
    if (noalt) {
      gg_file.print(F("2d"));
    } else {
      gg_file.print(F("3d"));
    }
  }
  gg_file.print(F("</fix>"));

  gg_file.print(F("<time>"));
  gg_file.print(
    format_date( _buf, '-',
      fix.dateTime.full_year(fix.dateTime.year),
      fix.dateTime.month,
      fix.dateTime.date
    )
  );
  gg_file.print(F("T"));
  gg_file.print(
    format_time( _buf, ':',
      fix.dateTime.hours,
      fix.dateTime.minutes,
      fix.dateTime.seconds
    )
  );
  gg_file.print(F("Z"));
  gg_file.print(F("</time>"));

  if (not noalt) {
    gg_file.print(F("<ele>")); // meters
    gg_file.print((int) fix.altitude());
    gg_file.print(F("</ele>"));
  }

  gg_file.print(F("</trkpt>\n"));
  gg_file.print(F(GPX_ENDING));

  // Force data to SD and update the directory entry to avoid data loss.
  if (!gg_file.sync() || gg_file.getWriteError()) {
    error.fatal(GG_Error::Type::SD_FILE_WRITE);
  }
}

//-----------------------------------------------------------------------------
void create_file(
                  uint16_t year,
                  uint8_t month,
                  uint8_t date,
                  uint8_t hours,
                  uint8_t minutes,
                  uint8_t seconds,
                  GG_Error& error) {

  if (!sd.chdir(true)) {
    error.fatal(GG_Error::Type::SD_CHDIR_ROOT);
  }
  // create dir for today
  format_date(gg_dir_name, ' ', year, month, date);
  if (!sd.exists(gg_dir_name)) {
    if (!sd.mkdir(gg_dir_name)) {
      error.fatal(GG_Error::Type::SD_MKDIR_WORKDIR);
    }
  }
  if (!sd.chdir(gg_dir_name, true)) {
    error.fatal(GG_Error::Type::SD_CHDIR_WORKDIR);
  }

  for (uint8_t i = 0; i < 100; i++) {
    gg_file_name[FILE_NAME_DIGIT_H] = '0' + i/10;
    gg_file_name[FILE_NAME_DIGIT_L] = '0' + i%10;
    if (!sd.exists(gg_file_name)) {
      break;
    }
  }
  if (!gg_file.open(gg_file_name, O_CREAT | O_WRITE | O_EXCL)) {
    error.fatal(GG_Error::Type::SD_OPEN_FILE);
  }
  // set file datetimes
  if (!gg_file.timestamp((T_CREATE | T_WRITE | T_ACCESS), year, month, date, hours, minutes, seconds)) {
    error.fatal(GG_Error::Type::SD_SET_TIMESTAMP);
  }
  gg_file.print(F(
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<gpx version=\"1.1\" creator=\"Geo-Gadget v" GG_VERSION " (" GG_ORIGIN ")\">\n"
    "\t<trk><trkseg>\n"));
  gg_file.print(F(GPX_ENDING));
}

//--------------------------
// SD card initializing
void setup_sd(const NMEAGPS& gps, const gps_fix& fix, GG_Error& error) {
  // Initialize at the highest speed supported by the board that is
  // not over 50 MHz. Try a lower speed if SPI errors occur.
  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    error.fatal(GG_Error::Type::SD_INIT);
  }
  create_file(fix.dateTime.full_year(fix.dateTime.year),
              fix.dateTime.month,
              fix.dateTime.date,
              fix.dateTime.hours,
              fix.dateTime.minutes,
              fix.dateTime.seconds,
              error);
}
