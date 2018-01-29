#include <Arduino.h>
#include <SoftwareSerial.h>

#include "ArduinoJson.h"

#define PIN_LORA_RX 10
#define PIN_LORA_TX 11
#define PIN_BATT_GAUGE A0

const long BAUD_SERIAL = 9600;
const long BAUD_LORA = 9600;

const int DATA_INTERVAL = 5000;
const int SAMPLE_INTERVAL = 250;
SoftwareSerial lora_serial(PIN_LORA_RX, PIN_LORA_TX);

unsigned long CurrTime = 0;
unsigned long PrevTime_data = 0;

int val_volts = 0;
int val_amps = 0;

int val_p_volts = 0;
int val_p_amps = 0;

/* Json Format
 Json: {
     "Sensor Name" : "sensor_name",
     "Data" : {
         "Vols" : "val_volts",
         "Amps" : "val_amps"
     }
 }
*/

void send_data(SoftwareSerial, JsonObject);

void setup() {
    Serial.begin(BAUD_SERIAL);

    pinMode(PIN_LORA_RX, INPUT);
    pinMode(PIN_LORA_TX, OUTPUT);
    pinMode(PIN_BATT_GAUGE, INPUT);

    lora_serial.begin(BAUD_LORA);
    
    Serial.println("LoRa volts meter rdy!");
}

void loop() {
    CurrTime = millis();

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& send_pkt = jsonBuffer.createObject();

    send_pkt["Sensor Name"] = "Volts meter";
    JsonObject& data = send_pkt.createNestedObject("data");

    // Read sensor data
    if (CurrTime - PrevTime_data > SAMPLE_INTERVAL) {
        val_volts = random(100);
    }

    // Write data to LoRa
    if (CurrTime - PrevTime_data > DATA_INTERVAL) {
        PrevTime_data = CurrTime;

        data["Volts"] = analogRead(PIN_BATT_GAUGE);
        data["TimeStamp"] = millis();

        String json_str;
        send_pkt.printTo(json_str);

        lora_serial.println("$Trup=" + json_str);
    }

    if (lora_serial.available()) {
        Serial.print(lora_serial.readString());
    }
}

void serialEvent() {
    while (Serial.available()) {
        String str = Serial.readString();

        Serial.println(str);
        lora_serial.print(str);
    }
}