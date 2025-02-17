#include <Arduino.h>

#define GPS_BAUDRATE 9600;

// DECLARE ID
uint16_t id = 1;

// MQTT
#define WIFI_SSID    "KUWIN-IOT"
#define WIFI_PASS    ""
#define MQTT_BROKER  "iot.cpe.ku.ac.th"
#define MQTT_USER    "b6710504344"  
#define MQTT_PASS    "watcharawish.o@ku.th"     
#define TOPIC_PREFIX "b6710504344" 

#define TOPIC_KU_BUS TOPIC_PREFIX "/led/switch"