# Project logbook
## Memory use

|FLASH |RAM  |Comment                      |
|------------------------------------------|
|29380 |1664 | initial
|30328 |1687 | added SATs and minors
|30245 |--   | D() for some log messages
|30186 |--   | refactor init disable messages
|30166 |--   | remove format_date_dot
|30028 |1682 | refactor to get_signal
|28744!|1682 | branch new-location-format - avoid using dtostrf 1300bytes!
|28558 |1682 | replace sdfat.print float with custom location format
|28126!|1670 | remove sdfat.print float with print int 420! bytes
|27702!|--   | replace display print(F) with sprintf_P, remove clearLine 424! bytes
|27798 |--   | added cfg_rate call
|27794 |--   | refactor initializing cfg_rate_t
|27954 |--   | added main state machine
|27864 |1702 | refactored display to sep file
|27976 |1685 | put _buf inside functions
|27948 |1653 | refactored to Class GG_Display
|-     |-    | no effect after put to inline and to hpp!!
|27956 |1689 | branch GPS-power-save started
|28206 |1721 | added go_power_max and go_power_save for GPS device
|28386 |1689 | added logic to not to log the nofix values (one nofix per 5 minutes)
|28442 |1689 | added desc "location NOK" to GPX
|28478 |1689 | fixed errors
|28476 |1687 | gg_file_name [] instead of [13]
|28534 |1687 | print 2d if no valid altitude
|28516 |1687 | replace sd.seekSet with seekEnd
|28330 |1723?| 1 sd_file.timestamp call instead of 3
|28408 |1723 | added creator to GPX header
|28412 |1687?| change version to 00.00.00 format
|28496 |1691 | added UBX-CFG-TP5 parameters
