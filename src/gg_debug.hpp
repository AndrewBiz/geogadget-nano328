#ifndef GG_DEBUG_H
#define GG_DEBUG_H

#include <Arduino.h>
#include <GPSport.h>

#ifdef DEBUG
#  define D(x) x
#else
#  define D(x)
#endif

void _dumpPort(Stream& port_in, Stream& port_out, unsigned int cycles);

#endif
