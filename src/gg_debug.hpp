#ifndef GG_DEBUG_H
#define GG_DEBUG_H

#include <Arduino.h>
#include <GPSport.h>
// #include <MemoryFree.h>
// DEBUG_PORT.print(F("FM=")); DEBUG_PORT.println(freeMemory());

#ifdef DEBUG
#  include <Streamers.h>
#  define D(x) x
#else
#  define D(x)
#endif

void _dumpPort(Stream& port_in, Stream& port_out, unsigned int cycles);

#endif
