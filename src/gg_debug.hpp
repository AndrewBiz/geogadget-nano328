#ifndef GG_DEBUG_H
#define GG_DEBUG_H

#include <Arduino.h>
#include <GPSport.h>

#ifdef DEBUG
#  include <Streamers.h>
#  define D(x) x
#else
#  define D(x)
#endif

void _dumpPort(Stream& port_in, Stream& port_out, unsigned int cycles);
void _tick(Stream& port_out);

#endif
