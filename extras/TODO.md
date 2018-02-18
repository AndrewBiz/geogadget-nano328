## TO-DO for GeoGadget
- [x] change the way of DateTime fix (try to avoid TOW parsing to get date and time, disable parsing GPSTime)
- [x] make dir YYYYMMDD to separate GPX files
- [x] refactor to use FormatDate and FormatTime functions
- [x] use alt NeoSerial lib
- [x] add SATs info (starting screen, operation screen)
- [x] green gg: 1fix per xx (5-10 sec) in normal mode
- [ ] green gg: power save mode of GPS device: cyclyc tracking (to wake up send dummy 0xff message to GPS and wait for ACK)
- [ ] green gg: minimal LED flashing via UBX-CFG start_running (TIMEPULSE)
- [ ] green gg: implement sleep_mode (like in OpenLog project), wake up via UART OR Button press
- [ ] do sd.sync every 10 secs (its too expensive operation)
- [ ] upgrade to GPX 1.1
- [ ] add header info to GPX (Geo-Gadged, version, url)
- [ ] do SD card init message (like in NMEASDLOG.ino)
- [ ] make parsing UBX-NAV-PVT instead all other tags
- [ ] to understand how it works: ```
ublox::nav_timeutc_t::valid_t &v = *((ublox::nav_timeutc_t::valid_t *) &chr);```
- [ ] make program reset the device after 25 minutes of work - to avoid hangings
