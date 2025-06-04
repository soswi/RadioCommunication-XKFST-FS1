#include "DataReceiver.h"

void DataReceiver::begin() {
    buffer.clear();
}

void DataReceiver::update() {
    while (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        processInput(input);
    }
}

void DataReceiver::processInput(const String& input) {
    // Konwertuj string ASCII na bajty (np. jako znaki)
    for (char c : input) {
        buffer.push_back(static_cast<uint8_t>(c));
    }

    // Opcjonalnie: podział co N bajtów i obliczenie CRC
    const size_t segmentSize = 4;  // np. 4 bajty = 32 bity
    for (size_t i = 0; i + segmentSize <= buffer.size(); i += segmentSize) {
        std::vector<uint8_t> segment(buffer.begin() + i, buffer.begin() + i + segmentSize);
        uint16_t crc = calculateCRC(segment);

        Serial.print("Segment: ");
        for (uint8_t b : segment) Serial.print(b, HEX), Serial.print(' ');
        Serial.print(" | CRC: ");
        Serial.println(crc, HEX);
    }
}

uint16_t DataReceiver::calculateCRC(const std::vector<uint8_t>& data) {
    uint16_t crc = 0xFFFF;
    for (uint8_t b : data) {
        crc ^= b;
        for (int i = 0; i < 8; i++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

const std::vector<uint8_t>& DataReceiver::getBuffer() const {
    return buffer;
}
