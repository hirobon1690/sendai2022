#include "roginow.h"

Roginow::Roginow(uint8_t* ADDR)
    : now(espnow) {
    memcpy(this->ADDR, ADDR, 6);
}

void Roginow::begin() {
    now.begin();
    now.addPeer(ADDR);
    now.send(0xFF);
}

bool Roginow::send(uint8_t msg) {
    now.send(msg);
    uint8_t buf;
    bool status = 0;
    int t = 0;
    while (t < 10) {
        now.receive(&buf, 100);
        if (buf == msg) {
            Serial.println("[Roginow] Sent success");
            break;
        } else {
            // Serial.printf("[Roginow] Sent times %d\n", t);
            now.send(msg);
        }
        t++;
    }
    if (t < 10) {
        // Serial.println("[Roginow] Sent success");
        return true;
    } else {
        Serial.println("[Roginow] Failed to send");
        return false;
    }
}

bool Roginow::receive(uint8_t* buf) {
    bool status = 0;
    status = now.receive(buf, 65535);
    if (!status) {
        Serial.printf("[Roginow] Time out\n");
        return false;
    }
    Serial.printf("[Roginow] Received %d\n", *buf);
    now.send(*buf);
    // Serial.printf("[Roginow] Sent pong\n");
    return true;
}

void Roginow::clearFlg() {
    now.clearFlg();
}