#ifndef __NETWORK_H_
#define __NETWORK_H_
// Network Selection

struct WifiCred {
  const char* ssid;
  const char* password;
};

const WifiCred knownNetworks[] PROGMEM = {
  {"<SSID1>", "<Pass1>"},
  {"<SSID2>", "<Pass2>"}
};

const int knownNetworksCount = sizeof(knownNetworks)/sizeof(knownNetworks[0]);

#endif // __NETWORK_H_
