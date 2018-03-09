## TO-DO for GeoGadget
- [x] change the way of DateTime fix (try to avoid TOW parsing to get date and time, disable parsing GPSTime)
- [x] make dir YYYYMMDD to separate GPX files
- [x] refactor to use FormatDate and FormatTime functions
- [x] use alt NeoSerial lib
- [x] add SATs info (starting screen, operation screen)
- [x] green gg: 1 GPS fix per xx (5-10 sec) in normal mode
- [x] upgrade to GPX 1.1, add header info to GPX (Geo-Gadged, version, url)
- [x] green gg: power save mode of GPS device: cyclyc tracking
- [x] log NoFix records every 5 minutes (to avoid recording 2Hz useless fixes in weak signal (cyclyc mode))
- [x] green gg: minimal LED flashing via UBX-CFG start_running (TIMEPULSE)
- [x] baudrate to increase btw GPS and MCU (115200?)
- [x] completely disable NMEA via CFG-PRT
- [x] green gg: implement sleep_mode (like in OpenLog project), wake up via UART OR Button press
- [x] green gg: go to normal_mode from display_mode after 10-20 sec
- [ ] green gg: make all unused pins as OUTPUT LOW
- [ ] green gg: to wake up GPS send dummy 0xff message to GPS and wait for ACK
- [ ] do SD card init message (like in NMEASDLOG.ino)
- [ ] do SD card error messages to display
- [ ] check ERR is wrong
- [ ] check ALT is wrong
***** version 01.00.00 *****
- [ ] to understand how it works: ```
ublox::nav_timeutc_t::valid_t &v = *((ublox::nav_timeutc_t::valid_t *) &chr);```
- [ ] make program reset the device after 25 minutes of work - to avoid hangings
- [ ] green gg: log 1 fix per xx (5-10 sec) in all modes (in sd_log)
- [ ] long-click - prepare for shutdown (sd.sync, switch all off)
- [ ] make parsing UBX-NAV-PVT instead all other tags
