#include <Arduino.h>
#include <LoRa.h>

// #define ss 15
// #define rst 16
// #define dio0 2

int counter = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("1");

    Serial.println("LoRa Sender");

    if (!LoRa.begin(433E6)) {
        Serial.println("Starting Lora failed");
        while(1);
    }

    Serial.println("2");
}

void loop() {
    Serial.println(counter);

    LoRa.beginPacket();
    LoRa.print(counter);
    LoRa.endPacket();

    Serial.println("Send!");

    counter++;

    delay(1000);
}