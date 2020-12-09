#ifndef LCT2CH_h
#define LCT2CH_h
#include "lct_data.h"

class LCT2CH
{
  public:
    LCT2CH();
    void connectDevice(void);
    Status setRelayStatus(Relay relay, Status status);
    Status getRelayStatus(Relay relay);
  private:
    Status statusRelayOne;
    Status statusRelayTwo;
    bool connect2Relay();
};

#endif
