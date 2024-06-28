#pragma once
#include "espnow.h"

class Roginow {
   public:
    Roginow(uint8_t* ADDR);
    void begin();
    void ping();
    void pong();
    bool send(uint8_t msg);
    bool receive(uint8_t* buf);
    void clearFlg();
   private:
    uint8_t ADDR[6];
    Espnow now;
};