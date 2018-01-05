#include <ublox/ubxGPS.h>

//-----------------------------------------------------------------
//  Derive a class to add the state machine for starting up:
//    1) The status must change to something other than NONE.
//    2) The GPS leap seconds must be received
//    3) The UTC time must be received
//    4) All configured messages are "requested"
//         (i.e., "enabled" in the ublox device)
//  Then, all configured messages are parsed and explicitly merged.

class GPS : public ubloxGPS {
public:
  enum {
    GETTING_STATUS,
    GETTING_UTC,
    RUNNING
  } state NEOGPS_BF(8);

  GPS(Stream*);
  void start_running(void);
  bool running(void);

private:
  void get_status(void);
  void get_utc(void);

} NEOGPS_PACKED;
