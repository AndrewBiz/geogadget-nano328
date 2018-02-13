#include "gg_debug.hpp"
#include "gg_sd.hpp"
#include "gg_gps.hpp"
#include "gg_format.hpp"

#include <SdFat.h>
// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))

// Seek to fileSize + this position before writing track points.
#define GPX_ENDING_OFFSET -24
#define GPX_ENDING "\t</trkseg></trk>\n</gpx>\n"

const uint8_t chipSelect = SS;

SdFat sd;
SdFile gg_file;
char gg_file_name[13] = FILE_BASE_NAME "00.GPX";
char gg_dir_name[9] = "";

//--------------------------
void create_file(uint16_t year, uint8_t month, uint8_t date, uint8_t hours, uint8_t minutes, uint8_t seconds) {
  if (!sd.chdir(true)) {
    error("chdir 1");
  }
  // create dir for today
  format_date(gg_dir_name, ' ', year, month, date);
  if (!sd.exists(gg_dir_name)) {
    if (!sd.mkdir(gg_dir_name)) {
      error("Create dir");
    }
  }
  if (!sd.chdir(gg_dir_name, true)) {
    error("chdir 2");
  }
  // TODO: Alternative way of file name generation
  // Pick a numbered filename, 00 to 99.
  // char filename[15] = "data_##.txt";
  //
  // for (uint8_t i=0; i<100; i++) {
  //   filename[5] = '0' + i/10;
  //   filename[6] = '0' + i%10;
  //   if (!SD.exists(filename)) {
  //     // Use this one!
  //     break;
  //   }
  // }

  while (sd.exists(gg_file_name)) {
    if (gg_file_name[BASE_NAME_SIZE + 1] != '9') {
      gg_file_name[BASE_NAME_SIZE + 1]++;
    } else if (gg_file_name[BASE_NAME_SIZE] != '9') {
      gg_file_name[BASE_NAME_SIZE + 1] = '0';
      gg_file_name[BASE_NAME_SIZE]++;
    } else {
      error("create fname");
    }
  }
  if (!gg_file.open(gg_file_name, O_CREAT | O_WRITE | O_EXCL)) {
    error("file open");
  }
  // set file datetimes
  if (!gg_file.timestamp(T_CREATE, year, month, date, hours, minutes, seconds)) {
    error("set CREATE time");
  }
  if (!gg_file.timestamp(T_WRITE, year, month, date, hours, minutes, seconds)) {
    error("set WRITE time");
  }
  if (!gg_file.timestamp(T_ACCESS, year, month, date, hours, minutes, seconds)) {
    error("set ACCESS time");
  }
  gg_file.print(F(
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<gpx version=\"1.0\">\n"
    "\t<trk><trkseg>\n"));
  gg_file.print(F(GPX_ENDING));
}

//--------------------------
// Log a data record to SD card file
void log_fix(const NMEAGPS& gps, const gps_fix& fix) {
  char _buf[15];
  //TODO replace with seekEND
  gg_file.seekSet(gg_file.fileSize() + GPX_ENDING_OFFSET);
  gg_file.print(F("\t\t<trkpt "));

  gg_file.print(F("lat=\""));
  gg_file.print(format_location(_buf, fix.latitudeL()));
  gg_file.print(F("\" lon=\""));
  gg_file.print(format_location(_buf, fix.longitudeL()));
  gg_file.print(F("\">"));

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

  // TODO: avoid printing bad elevation data
  gg_file.print(F("<ele>")); // meters
  gg_file.print((int) fix.altitude());
  gg_file.print(F("</ele>"));
  gg_file.print(F("</trkpt>\n"));
  gg_file.print(F(GPX_ENDING));

  // Force data to SD and update the directory entry to avoid data loss.
  if (!gg_file.sync() || gg_file.getWriteError()) {
    error("write file");
  }
}

//--------------------------
// SD card initializing
void setup_sd(const NMEAGPS &gps, const gps_fix &fix) {
  // Initialize at the highest speed supported by the board that is
  // not over 50 MHz. Try a lower speed if SPI errors occur.
  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    sd.initErrorHalt();
  }
  create_file(fix.dateTime.full_year(fix.dateTime.year), fix.dateTime.month, fix.dateTime.date, fix.dateTime.hours, fix.dateTime.minutes, fix.dateTime.seconds);

  D(DEBUG_PORT.print(F("Logging to: "));)
  D(DEBUG_PORT.print(gg_dir_name);)
  D(DEBUG_PORT.print(F("/"));)
  D(DEBUG_PORT.println(gg_file_name);)
}
