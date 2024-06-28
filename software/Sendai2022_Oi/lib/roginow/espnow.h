#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <functional>

class Espnow {
   public:
    Espnow();
    void begin();
    void addPeer(const uint8_t* ADDR);
    void receiveCb(const uint8_t* addr, const uint8_t* data, int size);
    void send(uint8_t msg);
    bool receive(uint8_t* buf, int timeout_max = 65535);
    void clearFlg();
    uint8_t* ADDR;
    struct Frame {
        uint8_t addr[6];
        uint8_t data;
        size_t size;
        bool flag;
    } received;
};

extern Espnow espnow;