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
