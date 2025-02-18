#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <definitions.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define USE_DEBUG

#ifdef USE_DEBUG
#define DEBUG(str) Serial.println(str)
#else
#define DEBUG(str)
#endif

WiFiClient wifiClient;
PubSubClient mqtt(MQTT_BROKER, 1883, wifiClient);
uint32_t last_publish;

String data;

extern void mqtt_callback(char* topic, byte* payload, unsigned int length);
extern void connect_wifi();
extern void connect_mqtt();

void setup() {
    Serial.begin(115200);
    if (!LoRa.begin(433E6)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }

    connect_wifi();
    connect_mqtt();
    last_publish = 0;
}

void loop() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(300);
        digitalWrite(LED_BUILTIN, LOW);
        delay(300);

        String loraData = ""; 
        while (LoRa.available()) {
            loraData += (char)LoRa.read(); 
        }
        DEBUG("Received LoRa data: " + loraData);

        if (mqtt.connected()) {
            mqtt.publish(TOPIC_KU_BUS, loraData.c_str()); 
            DEBUG("Published to MQTT: " + loraData);
        } else {
            DEBUG("MQTT is not connected. Reconnecting...");
            connect_mqtt(); 
        }
    }
    mqtt.loop();
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
    for (;;) {} 
  }
  mqtt.setCallback(mqtt_callback);
  
  mqtt.subscribe(TOPIC_KU_BUS);
  printf("MQTT broker connected.\n");
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, TOPIC_KU_BUS) == 0) {
    char message[length + 1];
    for (int i = 0; i < length; i++) {
      message[i] = (char)payload[i];
    }
    message[length] = '\0';
    String receivedMessage = String(message);

    DEBUG("Received MQTT message on TOPIC_KU_BUS: " + receivedMessage);

    if (receivedMessage == "ON") {
        digitalWrite(LED_BUILTIN, HIGH);
    } else if (receivedMessage == "OFF") {
        digitalWrite(LED_BUILTIN, LOW);
    }
  }
}