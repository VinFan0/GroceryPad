#ifndef __NETWORK_H_
#define __NETWORK_H_
// Network Selection

// #define __HOME_NETWORK_
#define __JOURNAL_NETWORK_
// #define __USU_NETWORK_
// #define __PHONE_HOTSPOT_
#ifdef __HOME_NETWORK_
const char* ssid = "B-Fiber 2.4";
const char* password = "WEST=COOK=MINE";
#endif
#ifdef __JOURNAL_NETWORK_
const char* ssid = "Journal";
const char* password = "plain,stars,neighbor";
#endif
#ifdef __USU_NETWORK_
const char* ssid = "eduroam";
const char* password = "";
#endif
#ifdef __PHONE_HOTSPOT_
const char* ssid = "RTBS24+";
const char* password = "i<3frisbee";
#endif

#endif // __NETWORK_H_