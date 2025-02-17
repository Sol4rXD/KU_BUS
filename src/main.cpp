#include <Arduino.h>
#include <TinyGPS++.h>
#include <LoRa.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <LoRa.h>
#include "definitions.h"
#include "smartdelay.h"

float lat, lon = 0;
double alt = 0;

uint16_t counter = 0;
uint16_t sat = 0;

SoftwareSerial GPS_SERIAL(18,17);

TinyGPSPlus gps;

extern void get_gps();
extern void combine_packet(int id, float lat, float lon, float alt);
extern void send_packet(String packet);

String packet;

Millis timer(1000);

void setup() {
    Serial.begin(115200);
    GPS_SERIAL.begin(GPS_SERIAL);

    if(!LoRa.begin(433E6)) {
        while(1);
    }
    Serial.println("Starting Lora");
}

void loop() {
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
        }
    }
    combine_packet(id, lat, lon, alt);
}

void combine_packet(int id, float lat, float lon, float alt) {
    packet += String((int)id) + ","
           += String((float)lat) + ","
           += String((float)lon) + ","
           += String((float)alt) + ",";
}

void send_packet(String packet){
    LoRa.beginPacket();
    LoRa.println(packet);
    LoRa.endPacket();
}