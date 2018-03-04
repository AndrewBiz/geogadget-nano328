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

## 06 3B UBX-CFG-PM2 (len 44b)

Update period 1000ms, cyclic tracking, update RTC, update EPH:
B5 62         header
06 3B         ID
2C 00         len (44)
01            version
06            reserved1
00            reserved2
00            reserved3
00 98 02 00   flags (1st = low):  0000 0000 0000 0010:1001 1000 0000 0000
E8 03 00 00   updatePeriod ms: 03E8 = 1000ms
10 27 00 00   searchPeriod ms: 2710 = 10000ms
00 00 00 00   gridOffset ms
00 00         onTime s
00 00         minAcqTime s
2C 01         r4
00 00         r5
4F C1 03 00   r6
86 02 00 00   r7
FE            r8
00            r9
00 00         r10
64 40 01 00   r11
9B CD         checkSum

Update period = 5000ms:
B5 62
06 3B
2C 00
01
06
00
00
00 98 02 00
88 13 00 00   !!! 1388 = 5000
10 27 00 00
00 00 00 00
00 00
00 00
2C 01
00 00
4F C1 03 00
86 02 00 00
FE
00
00 00
64 40 01 00
4B 7D

06 11 UBX-CFG-RXM (len 2b)
Turn-on 1-Power Save Mode:
B5 62
06 11
02 00   len = 2b   
08      r = 08
01      lpMode (01 = PowerSave)
22 92   checksum

Turn-on 0-Continious mode (Max Power):
B5 62
06 11
02 00
08
00      00 = Continious Mode (Max Power)
21 91


## 06 31 UBX-CFG-TP5 (len 32b)

B5 62
06 31       UBX-CFG-TP5
20 00       32b
00          tpIdx 0 = TIMEPULSE
01          r1
00 00       r2
32 00       antCableDelay 0x0032 = 50ns
00 00       rfGroupDelay
00 12 7A 00 freqPeriod = 0x007A1200 = 8000000 us
40 4B 4C 00 freqPeriodLock = 0x004C4B40 = 5000000 us
A0 86 01 00 pulseLenRatio = 0x000186A0 = 100000 us
E8 03 00 00 pulseLenRatioLock = 0x03E8 = 1000 us
00 00 00 00 userConfigDelay
37 00 00 00 flags = 0x37 = 0011 0111
36 2A       checksum

## 06 00 CFG-PRT (len 20b)
B5 62
06 00
14 00       20b
01          portID
00          r0
00 00       txReady
D0 08 00 00 mode => 0000 1000 (no parity) 1101(8bit reserved)0000
80 25 00 00 baudrate = 0x25 0x80 = 9600
                       0x01 0xC2 0x00 = 115200
01 00       inProtoMask   0x01 = UBX only
01 00       outProtoMask  0x01 = UBX only
00 00       flags
00 00       r5
9A 79       checksum


B5 62
06 00
14 00
01
00
00 00
C0 08 00 00 ! => 0000 1000 1100 0000
80 25 00 00
01 00
01 00
00 00
00 00
8A 79
