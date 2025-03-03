#include <Arduino.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <LoRa.h>
#include "definitions.h"
#include "smartdelay.h"

float lat, lon = 0;
float prev_lat, prev_lon = 0;

double alt, speed = 0;

uint16_t sat = 0;
uint16_t counter = 1;

SoftwareSerial GPS_SERIAL(17, 18);

TinyGPSPlus gps;

extern void get_gps();
extern void combine_packet(int x, float y, float z, double a);
extern void send_packet(String payload);

String packet;

Millis timer(1000);
Millis timer_1(1000);
Millis timer_2(2000);
Millis timer_5(5000);

void setup() {
    Serial.begin(115200);
    GPS_SERIAL.begin(GPS_BAUDRATE);

    if (!LoRa.begin(433E6)) {
        Serial.println("Starting LoRa failed! Restarting...");
        delay(5000);
        ESP.restart();
    }
     LoRa.setSpreadingFactor(12);
     LoRa.setCodingRate4(8);
     LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);
     LoRa.setSignalBandwidth(125E3);
     LoRa.enableCrc();

    Serial.println("Starting Lora");
}

void loop() {
    get_gps();

    // Test
   if(timer) {
       combine_packet(counter, lat, lon, speed);
       send_packet(packet);
       Serial.println(packet);
       counter++;
   }

    //  if(lat != 0 && lon != 0) {
    //      static bool calibratedPrinted = false;

    //      if (!calibratedPrinted) {
    //          Serial.println("Calibrated!");
    //          calibratedPrinted = true;
    //      }
    //      if(timer_2 && lat != prev_lat && lon != prev_lon) {
    //          combine_packet(id, lat, lon, speed);
    //          send_packet(packet);
    //          Serial.println("Packet send! :" + packet);
    //          prev_lat = lat;
    //          prev_lon = lon;
    //      }
    //  }
    //  else if(timer_5) {
    //      Serial.println("Calibrating...");
    //  }   
}

void get_gps() {
    while(GPS_SERIAL.available() > 0) {
        gps.encode(GPS_SERIAL.read());
        Serial.write(GPS_SERIAL.read());
        if(gps.location.isUpdated()){
//            Serial.write((GPS_SERIAL.read()));
            Serial.println(("GPS UPDATED"));
            Serial.println(gps.location.lat(),gps.location.lng());
            lat = gps.location.lat();
            lon = gps.location.lng();
            alt = gps.altitude.meters();
            sat = gps.satellites.value();
            speed = gps.speed.kmph();
        }
    }
}

void combine_packet(int x, float y, float z, double a) {
    packet = "";
    packet += String((int)x) + ","
           += String((float)y, 6) + ","
           += String((float)z, 6) + ","
           += String((double)a);
}

void send_packet(String payload){
    LoRa.beginPacket();
    LoRa.println(payload);
    Serial.println("Send !!!");
    LoRa.endPacket();
}
