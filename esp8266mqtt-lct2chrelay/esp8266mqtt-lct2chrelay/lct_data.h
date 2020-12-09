#ifndef LCT2CH_DATA_h
#define LCT2CH_DATA_h
enum Relay {ONE = 1, TWO = 2};
enum Status {OFF = 0, ON = 1, NOTKNOWN = -1};
//Hex command to send to serial for close relay
static constexpr byte relON[4]  = {0xA0, 0x01, 0x01, 0xA2};
//Hex command to send to serial for open relay
static constexpr byte relOFF[4] = {0xA0, 0x01, 0x00, 0xA1};
//Hex command to send to serial for close relay
static constexpr byte rel2ON[4]  = {0xA0, 0x02, 0x01, 0xA3};
//Hex command to send to serial for open relay
static constexpr byte rel2OFF[4] = {0xA0, 0x02, 0x00, 0xA2};
#endif
