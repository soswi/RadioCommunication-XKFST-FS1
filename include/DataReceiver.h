#ifndef DATA_RECEIVER_H
#define DATA_RECEIVER_H

#include <Arduino.h>
#include <vector>

class DataReceiver {
public:
    void begin();
    void update();

    const std::vector<uint8_t>& getBuffer() const;

private:
    std::vector<uint8_t> buffer;

    void processInput(const String& input);
    uint16_t calculateCRC(const std::vector<uint8_t>& data);
};

#endif
