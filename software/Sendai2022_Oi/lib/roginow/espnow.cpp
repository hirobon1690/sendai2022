#include "espnow.h"

Espnow espnow;

void callback(const uint8_t* addr, const uint8_t* data, int size){
    memcpy(espnow.received.addr, addr, 6);
    memcpy(&espnow.received.data, data, 1);
    espnow.received.flag = true;
    // Serial.printf("%d\n",espnow.received.data);
}

Espnow::Espnow() {
}

void Espnow::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    if (esp_now_init() == ESP_OK) {
        Serial.println("ESP-Now Init");
    }
    esp_now_register_recv_cb(callback);
}

void Espnow::addPeer(const uint8_t* ADDR) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, ADDR, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
}

void Espnow::send(uint8_t msg) {
    esp_err_t result = esp_now_send(ADDR, &msg, 1);
    switch (result) {
        // case ESP_OK:
        //     Serial.println("Success");
        //     break;
        // case ESP_ERR_ESPNOW_NOT_INIT:
        //     Serial.println("ESPNOW not Init.");
        //     break;
        // case ESP_ERR_ESPNOW_ARG:
        //     Serial.println("Invalid Argument");
        //     break;
        // case ESP_ERR_ESPNOW_INTERNAL:
        //     Serial.println("Internal Error");
        //     break;
        // case ESP_ERR_ESPNOW_NO_MEM:
        //     Serial.println("ESP_ERR_ESPNOW_NO_MEM");
        //     break;
        // case ESP_ERR_ESPNOW_NOT_FOUND:
        //     Serial.println("Peer not found.");
        //     break;
    }
}

bool Espnow::receive(uint8_t* buf,int timeout_max){
    int timeout = 0;
    while(!espnow.received.flag){
        delay(1);
        timeout++;
        if(espnow.received.flag){
            break;
        }
        if(timeout>timeout_max){
            // Serial.println("Time out");
            return false;
        }
        // wait until data received
    }
    memcpy(buf, &espnow.received.data, 1);
    espnow.received.flag = false;
    return true;
}

void Espnow::clearFlg(){
    espnow.received.flag = false;
}