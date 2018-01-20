#include <ublox/ubxGPS.h>
//-----------------------------------------------------------------
//  Derive a class to add the state machine for starting up:
//    - The status must change to something other than NONE.
//    - The UTC time must be received
//    - All configured messages are "requested"
//         (i.e., "enabled" in the ublox device)
//  Then, all configured messages are parsed and explicitly merged.

class GPS : public ubloxGPS {
public:
  enum {
    GETTING_SIGNAL,
    RUNNING
  } state NEOGPS_BF(8);

  GPS(Stream*);
  void start_running(void);
  bool running(void);

private:
  void get_signal(void);

} NEOGPS_PACKED;
