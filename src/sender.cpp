#include <Arduino.h>
#include <TinyGPS++.h>
#include <LoRa.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <LoRa.h>
#include "definitions.h"
#include "smartdelay.h"

float lat, lon = 0;
float prev_lat, prev_lon = 0;
double alt, speed = 0;

uint16_t sat = 0;

SoftwareSerial GPS_SERIAL(18,17);

TinyGPSPlus gps;

extern void get_gps();
extern void combine_packet(int id, float lat, float lon, double speed);
extern void send_packet(String packet);

String packet;

Millis timer(1000);

void setup() {
    Serial.begin(115200);
    GPS_SERIAL.begin(GPS_BAUDRATE);

    if(!LoRa.begin(433E6)) {
        while(1);
    }
    Serial.println("Starting Lora");
}

void loop() {
    get_gps();
    if(lat != 0 && lon != 0) {
        static bool calibratedPrinted = false; 

        if (!calibratedPrinted) {
            Serial.println("Calibrated!");
            calibratedPrinted = true; 
        }
        if(timer && lat != prev_lat && lon != prev_lon) {
            combine_packet(id, lat, lon, speed);
            send_packet(packet);
            Serial.println("Packet send! :" + packet);
            prev_lat = lat;
            prev_lon = lon;
        }
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
}

void combine_packet(int id, float lat, float lon, double speed) {
    packet = "";
    packet += String((int)id) + ","
           += String((float)lat, 6) + ","
           += String((float)lon, 6) + ","
           += String((double)speed) + ",";
}

void send_packet(String packet){
    LoRa.beginPacket();
    LoRa.println(packet);
    LoRa.endPacket();
}

