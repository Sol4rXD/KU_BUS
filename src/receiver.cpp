#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "definitions.h"
#include "smartdelay.h"

//DEBUG
#define USE_DEBUG

#ifdef USE_DEBUG
#define DEBUG(str) Serial.println(str)
#else
#define DEBUG(str)
#endif

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

uint32_t last_publish;

extern void mqtt_callback(char* topic, byte* payload, unsigned int length);
extern void connect_wifi();
extern void connect_mqtt();

Millis timer(2000);

void setup() {
    Serial.begin(115200);

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

    Serial.println("starting Lora");

    connect_wifi();
    mqtt.setServer(MQTT_BROKER, 1883);
    connect_mqtt();
    last_publish = millis();

    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    if (!mqtt.connected()) {
        DEBUG("MQTT is not connected. Reconnecting...");
        connect_mqtt();
    }
    mqtt.loop();  

    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        String loraData = ""; 
        while (LoRa.available()) {
            digitalWrite(LED_BUILTIN, HIGH);
            digitalWrite(LED_BUILTIN, LOW);

            loraData += (char)LoRa.read(); 
        }
        DEBUG("Received LoRa data: " + loraData + " Rssi: " + LoRa.packetRssi());

        if (mqtt.connected()) {
            mqtt.publish(TOPIC_KU_BUS, loraData.c_str()); 
            DEBUG("Published to MQTT: " + loraData);
        }
    }
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
