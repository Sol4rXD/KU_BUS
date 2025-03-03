#include <Arduino.h>

// DECLARE ID
uint16_t id = 1;
uint16_t GPS_BAUDRATE = 9600;

// MQTT
#define WIFI_SSID    "KUWIN-IOT"
#define WIFI_PASS    ""
// #define WIFI_SSID    "Tanny"
// #define WIFI_PASS    "123456789"
//#define WIFI_SSID    "Feennnnnn"
//#define WIFI_PASS    "........"
#define MQTT_BROKER  "iot.cpe.ku.ac.th"
#define MQTT_USER    "b6710504051"  
#define MQTT_PASS    "pratchayanon.t@ku.th"     
#define TOPIC_PREFIX "b6710504051" 

#define TOPIC_KU_BUS TOPIC_PREFIX "/bus"