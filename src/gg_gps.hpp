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

  void set_fast_baudrate(void);
  bool set_rate(uint16_t rate);
  void go_power_save(void);
  void go_power_max(void);

  void start_running(void);
  bool running(void);


private:
  void get_signal(void);
  void write_P_simple(const unsigned char* progmem_msg, size_t len);

} NEOGPS_PACKED;
