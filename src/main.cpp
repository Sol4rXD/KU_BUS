#include <Arduino.h>
#include <TinyGPS++.h>
#include <LoRa.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <LoRa.h>
#include "definitions.h"
#include "smartdelay.h"
#include <WiFi.h>
#include <PubSubClient.h>

float lat, lon = 0;
double alt, speed = 0;

uint16_t counter = 0;
uint16_t sat = 0;

uint32_t last_publish;

SoftwareSerial GPS_SERIAL(18,17);

TinyGPSPlus gps;

extern void get_gps();
extern void combine_packet(int id, float lat, float lon, double speed);
extern void send_packet(String packet);
extern void mqtt_callback(char* topic, byte* payload, unsigned int length);
extern void connect_wifi();
extern void connect_mqtt();

String packet;

Millis timer(1000);

WiFiClient wifiClient;
PubSubClient mqtt(MQTT_BROKER, 1883, wifiClient);

void setup() {
    Serial.begin(115200);
    GPS_SERIAL.begin(GPS_SERIAL);

    if(!LoRa.begin(433E6)) {
        while(1);
    }
    Serial.println("Starting Lora");

    connect_wifi();
    connect_mqtt();
    last_publish = 0;
}

void loop() {
    // mqtt.loop();
    get_gps();
    if(timer) {
        combine_packet(id, lat, lon, alt);
        send_packet(packet);
    }
}

void get_gps() {
    while (GPS_SERIAL.available() > 0) {
        gps.encode(GPS_SERIAL.read());
        if (gps.location.isUpdated()) {
            lat = gps.location.lat();
            lon = gps.location.lng();
            alt = gps.altitude.meters();
            sat = gps.satellites.value();
            speed = gps.speed.kmph();
        }
    }
    combine_packet(id, lat, lon, alt);
}

void combine_packet(int id, float lat, float lon, double speed) {
    packet += String((int)id) + ","
           += String((float)lat) + ","
           += String((float)lon) + ","
           += String((double)speed) + ",";
}

void send_packet(String packet){
    LoRa.beginPacket();
    LoRa.println(packet);
    LoRa.endPacket();
}

void connect_wifi() {
    printf("WiFi MAC address is %s\n", WiFi.macAddress().c_str());
    printf("Connecting to WiFi %s.\n", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        printf(".");
        fflush(stdout);
        delay(500);
    }
    printf("\nWiFi connected.\n");
}

void connect_mqtt() {
    printf("Connecting to MQTT broker at %s.\n", MQTT_BROKER);
    if (!mqtt.connect("", MQTT_USER, MQTT_PASS)) {
        printf("Failed to connect to MQTT broker.\n");
        for (;;) {} // wait here forever
    }
    mqtt.setCallback(mqtt_callback);

    mqtt.subscribe(TOPIC_KU_BUS);

    printf("MQTT broker connected.\n");
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {

}