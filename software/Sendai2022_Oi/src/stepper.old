#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "Adafruit_TCS34725.h"

hw_timer_t* timer = NULL;

int cnt[2] = {0, 0};
int stepcycle[2] = {0, 0};
int stp[2] = {15, 25};

void IRAM_ATTR step() {
    for (int i = 0; i < 2; i++) {      //今回はステッパ2つ分
        if (cnt[i] >= stepcycle[i]) {  //閾値を超えると
            cnt[i] = 0;                //カウンタをリセット
        } else {
            cnt[i] = cnt[i] + 1;  //今回は1msごとに呼び出し
        }
        digitalWrite(stp[i], (stepcycle[i] != 0 && cnt[i] <= 2)); //2ms
    }
}

void setup() {
    pinMode(19, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(16, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(15, OUTPUT);
    pinMode(17, OUTPUT);

    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &step, true);
    timerAlarmWrite(timer, 1000, true);
    timerAlarmEnable(timer);
}

void loop() {
stepcycle[0]=1000;

}
