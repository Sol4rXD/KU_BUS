#include <Arduino.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include "definitions.h"
#include "smartdelay.h"
#include <WiFi.h>
#include <PubSubClient.h>

#define USE_DEBUG

#ifdef USE_DEBUG
#define DEBUG(str) Serial.println(str)
#else
#define DEBUG(str)
#endif

float lat, lon = 0;
double alt, speed = 0;

// uint16_t counter = 0;
uint16_t sat = 0;

SoftwareSerial GPS_SERIAL(17, 18);

TinyGPSPlus gps;

extern void get_gps();
extern void combine_packet(int id, float lat, float lon, double speed);
extern void send_packet(String packet);
extern void connect_wifi();
extern void connect_mqtt();
extern void mqtt_callback(char* topic, byte* payload, unsigned int length);

String packet;

Millis timer(2000);

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

uint32_t last_publish;

extern void mqtt_callback(char* topic, byte* payload, unsigned int length);
extern void connect_wifi();
extern void connect_mqtt();

void setup() {
    Serial.begin(115200);
    GPS_SERIAL.begin(9600);
    Serial.println("Start");
    connect_wifi();
    mqtt.setServer(MQTT_BROKER, 1883);
    connect_mqtt();
    last_publish = millis();
}

void loop() {
    if (!mqtt.connected()) {
        DEBUG("MQTT is not connected. Reconnecting...");
        connect_mqtt();
    }
    mqtt.loop();  
    get_gps();
    combine_packet(id, lat, lon, speed);
    if(timer) {
        Serial.print("GPS : ");
        Serial.println(" " + packet + ", " + sat);
        if (mqtt.connected() && gps.satellites.isValid() && lat != 0 && lon != 0) {
            mqtt.publish(TOPIC_KU_BUS, packet.c_str()); 
            DEBUG("Published to MQTT: " + packet);
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
           += String((double)speed);
}

void connect_wifi() {
    Serial.printf("WiFi MAC address: %s\n", WiFi.macAddress().c_str());
    Serial.printf("Connecting to WiFi: %s\n", WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
        attempt++;
        if (attempt > 20) {  
            Serial.println("\nWiFi connection failed! Restarting...");
            ESP.restart();
        }
    }
    Serial.println("\nWiFi connected.");
}

void connect_mqtt() {
    Serial.printf("Connecting to MQTT broker at %s...\n", MQTT_BROKER);
    
    while (!mqtt.connected()) {
        if (mqtt.connect("ESP32_Client", MQTT_USER, MQTT_PASS)) {
            Serial.println("MQTT broker connected.");
            mqtt.setCallback(mqtt_callback);
        } else {
            Serial.println("Failed to connect to MQTT broker. Retrying in 5s...");
            delay(5000);
        }
    }
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';

    String receivedMessage = String(message);
    DEBUG("Received MQTT message on TOPIC_KU_BUS: " + receivedMessage);

    if (receivedMessage == "ON") {
        digitalWrite(LED_BUILTIN, HIGH);
    } else if (receivedMessage == "OFF") {
        digitalWrite(LED_BUILTIN, LOW);
    }
}

