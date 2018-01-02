# Cold start (long power off)
## Rate = 1000ms (1Hz)
## Default NMEA messages flow
 - GPRMC
 - GPVTG
 - GPGGA
 - GPGSA
 - GPGLL

## No UBX messages flow

# Cold start (short power off after fix status)
## Rate = 1000ms (1Hz)
## Default NMEA messages flow
 - GPRMC
 - GPVTG
 - GPGGA
 - GPGSA
 - GSV GSV GSV
 - GPGLL

## No UBX messages flow

# Geogadget RUNNING
## UBX messages flow (version 1)
 - 01 03 UBX_NAV_STATUS
 - 01 02 UBX_NAV_POSLLH
 - 01 04 UBX_NAV_DOP
 - 01 12 UBX_NAV_VELNED

## UBX messages flow (version 2)
 - 01 03 UBX_NAV_STATUS
 - 01 02 UBX_NAV_POSLLH
 - 01 04 UBX_NAV_DOP
 - 01 12 UBX_NAV_VELNED
 - 01 20 UBX_NAV_TIMEGPS
 - 01 21 UBX_NAV_TIMEUTC

# FYI UBX messages
