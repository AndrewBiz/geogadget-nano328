#ifndef GG_FORMAT_HPP
#define GG_FORMAT_HPP
#include <stdint.h>

char* format_date(char* buf, char ftype, uint16_t year, uint8_t month, uint8_t date);
char* format_time(char* buf, char ftype, uint8_t hours, uint8_t minutes, uint8_t seconds);
char* format_location(char* buf, int32_t val);

#endif
