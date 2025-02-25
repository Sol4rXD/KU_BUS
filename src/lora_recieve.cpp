#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

void setup() {
    Serial.begin(115200);
    Serial.println("LoRa Receiver");

    if (!LoRa.begin(433E6)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    Serial.println("Set up already");
}

void loop() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        digitalWrite(LED_BUILTIN, HIGH);   
        delay(300);                       
        digitalWrite(LED_BUILTIN, LOW);
        delay(300);
                               
        while (LoRa.available()) {
        Serial.println((char)LoRa.read());
        }
  }
}
