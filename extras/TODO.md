## TO-DO for GeoGadget
- [x] change the way of DateTime fix (try to avoid TOW parsing to get date and time, disable parsing GPSTime)
- [x] make dir YYYYMMDD to separate GPX files
- [x] refactor to use FormatDate and FormatTime functions
- [ ] init gps device: 1fix per 10 sec, power save mode
- [ ] implement sleep_mode (like in OpenLog project)
- [ ] do sd.sync every 10 secs (its too expensive operation)
- [ ] upgrade to GPX 1.1
- [ ] add header info to GPX (Geo-Gadged, version, url)
- [ ] add SATs info (starting screen, operation screen)
- [ ] do  SD card init message (like in NMEASDLOG.ino)
- [ ] try implicit merging in NMEAGPS_cfg
- [ ] to understand: ```
ublox::nav_timeutc_t::valid_t &v = *((ublox::nav_timeutc_t::valid_t *) &chr);```
