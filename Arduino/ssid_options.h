#ifndef __NETWORK_H_
#define __NETWORK_H_
// Network Selection

// Select which WiFi to use by commenting in the respective line below
// #define __LOCAL_NETWORK_

#ifdef __LOCAL_NETWORK_
const char* ssid = "<Local SSID Name>";
const char* password = "<Password>";
#endif

#endif // __NETWORK_H_
