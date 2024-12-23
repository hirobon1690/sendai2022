// alt+shift+Fでキレイキレイ
#include <PCA9685.h>  //PCA9685用ヘッダーファイル（秋月電子通商作成）
#include <SPI.h>
#include <Wire.h>
#include <arduino.h>
#include "Adafruit_TCS34725.h"
#include "info.h"
#include "roginow.h"

hw_timer_t* timer = NULL;
uint8_t ADDR[6] = {0xA0, 0x76, 0x4E, 0x41, 0xED, 0x94};
Roginow roginow(ADDR);

PCA9685 pwm(0x41);  // PCA9685のアドレス指定（アドレスジャンパ未接続時）

const int SERVOMIN = 150;  // 最小パルス幅 (標準的なサーボパルスに設定)
const int SERVOMAX = 600;  // 最大パルス幅 (標準的なサーボパルスに設定)

#include "Adafruit_TCS34725.h"

const int LEDpin = 32;

byte gammatable[256];

Adafruit_TCS34725 tcs(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// アドレス指定
const int GP2Y0E03_ADDR = 0x40;
const int DISTANCE_ADDR = 0x5E;

const int servo0 = 0;  // 射出方向変換用サーボ
const int servo1 = 1;  // ストッパー用サーボ
const int servo2 = 7;  // 坂展開用サーボ(車体方向)
const int servo3 = 2;  // 坂展開用サーボ(横方向)
const int servo4 = 3;  // 坂展開用サーボ(横方向)

// const int s0 = 4;
const int s0 = 34;
const int s1 = 34;
const int s2 = 35;
const int s3 = 35;
// const int s3 = 14;

void servo_write(int ch, int ang) {              // 動かすサーボチャンネルと角度を指定
    ang = map(ang, 0, 180, SERVOMIN, SERVOMAX);  // 角度（0～180）をPWMのパルス幅（150～600）に変換
    pwm.setPWM(ch, 0, ang);
    // delay(1);
}

int readDistance() {
    unsigned int dac[2];
    Wire.beginTransmission(GP2Y0E03_ADDR);  // I2Cスレーブ「Arduino Uno」のデータ送信開始
    Wire.write(DISTANCE_ADDR);              // 距離の測定
    Wire.endTransmission();                 // I2Cスレーブ「Arduino Uno」のデータ送信終了

    Wire.requestFrom(GP2Y0E03_ADDR, 2);  // I2Cデバイス「GP2Y0E03」に2Byteのデータ要求
    for (int i = 0; i < 2; i++) {
        dac[i] = Wire.read();  // dacにI2Cデバイス「GP2Y0E03」のデータ読み込み
    }
    Wire.endTransmission();  // I2Cスレーブ「Arduino Uno」のデータ送信終了

    return ((dac[0] * 16 + dac[1]) / 16) / (2 * 2);  // 距離(cm)を計算
    delay(50);
}

int cnt[2] = {0, 0};
int stepcycle[2] = {0, 0};
int stp[2] = {15, 25};

void IRAM_ATTR step() {
    for (int i = 0; i < 2; i++) {      // 今回はステッパ2つ分
        if (cnt[i] >= stepcycle[i]) {  // 閾値を超えると
            cnt[i] = 0;                // カウンタをリセット
        } else {
            cnt[i] = cnt[i] + 1;  // 今回は0.1msごとに呼び出し
        }
        digitalWrite(stp[i], (stepcycle[i] != 0 && cnt[i] <= 20));  // 0.1*20=2ms(HIGHの時間)
    }
}

void speed0(int c0) {
    if (c0 < 0) {
        digitalWrite(5, 1);
        stepcycle[0] = int(5000 / -c0);  // 1sでspeed0/100回転
        return;
    } else if (c0 == 0) {
        stepcycle[0] = 0;
        return;
    } else if (c0 > 0) {
        digitalWrite(5, 0);
        stepcycle[0] = int(5000 / c0);  // 1sでspeed0/100回転
        return;
    }
}

void speed1(int c1) {
    if (c1 < 0) {
        digitalWrite(19, 0);
        stepcycle[1] = int(5000 / -c1);  // 1sでspeed1/100回転
        return;
        if (c1 == 0) {
            stepcycle[1] = 0;
            return;
        }
    } else if (c1 > 0) {
        digitalWrite(19, 1);
        stepcycle[1] = int(5000 / c1);  // 1sでspeed1/100回転
    }
}

void setup() {
    Serial.begin(115200);  // シリアル通信を9600bpsで初期化
    Serial.println("HELLO");
    Wire.begin();  // I2Cを初期化
    showInfo();    // 情報表示
    TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
    TaskHandle_t idle_1 = xTaskGetIdleTaskHandleForCPU(1);
    // roginow.begin();

    pwm.begin();         // 初期設定 (アドレス0x40用)
    pwm.setPWMFreq(60);  // PWM周期を60Hzに設定 (アドレス0x40用)

    pinMode(5, OUTPUT);
    pinMode(15, OUTPUT);
    pinMode(17, OUTPUT);
    pinMode(19, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(16, OUTPUT);
    pinMode(LEDpin, OUTPUT);

    digitalWrite(LEDpin, LOW);

    pinMode(26, INPUT_PULLUP);
    pinMode(27, INPUT_PULLUP);

    pinMode(2, OUTPUT);

    digitalWrite(19, 1);
    digitalWrite(5, 0);

    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &step, true);
    timerAlarmWrite(timer, 100, true);
    timerAlarmEnable(timer);

    servo_write(servo1, 10);
    servo_write(servo0, 60);

    delay(500);
}

void loop() {
    int val0;
    int val1;
    int val2;
    int val3;

    servo_write(servo2, 10);

    delay(500);
    Serial.println("hello");
    speed0(50);
    speed1(50);
    delay(10000);
    speed0(0);
    speed1(0);
    delay(1000);
    speed0(20);
    speed1(-20);
    delay(3000);
    speed0(-20);
    speed1(-20);
    // val0 = analogRead(s0);
    // val1 = analogRead(s1);
    // val2 = analogRead(s2);
    // val3 = analogRead(s3);
    // Serial.println("1");
    // //Serial.println(val0);
    // while (val0 > 200 && val3 > 200) {
    //     val0 = analogRead(s0);
    //     val1 = analogRead(s1);
    //     val2 = analogRead(s2);
    //     val3 = analogRead(s3);
    //     delay(100);
    // }
    // Serial.println("2");
    // if (val0 <= 200) {
    //     speed0(0);
    //     speed1(10);
    // } else {
    //     speed0(10);
    //     speed1(0);
    // }
    // Serial.println("3");
    // while (val0 <= 200 || val3 <= 200) {
    //     val0 = analogRead(s0);
    //     val1 = analogRead(s1);
    //     val2 = analogRead(s2);
    //     val3 = analogRead(s3);
    //     delay(100);
    // }
    // Serial.println("4");
    // speed0(0);
    // speed1(0);
    // delay(500);
    // speed0(50);
    // speed1(50);
    // delay(1000);
    // Serial.println("5");
    // val0 = analogRead(s0);
    // val1 = analogRead(s1);
    // val2 = analogRead(s2);
    // val3 = analogRead(s3);
    // Serial.println("6");
    // if (val0 <= 200) {
    //     speed0(10);
    //     speed1(50);
    //     delay(1000);
    // } else if (val3 <= 200) {
    //     speed0(50);
    //     speed1(10);
    //     delay(1000);
    // }
    // Serial.println("7");
    // while (val0 > 200 || val3 > 200) {
    //     val0 = analogRead(s0);
    //     val1 = analogRead(s1);
    //     val2 = analogRead(s2);
    //     val3 = analogRead(s3);
    //     speed0(int(val1 / 10));  // p制御
    //     speed1(int(val2 / 10));
    //     delay(100);
    // }
    // Serial.println("8");
    // speed0(0);
    // speed1(0);
    // digitalWrite(5, 1);
    // digitalWrite(19, 0);

    // digitalWrite(LEDpin, HIGH);

    // for (int num0 = 1; num0 <= 18; num0 = num0 + 1) {
    //     servo_write(servo2, 5 * num0);
    //     delay(200);
    // }
    // delay(500);
    // for (int num1 = 1; num1 <= 18; num1 = num1 + 1) {
    //     servo_write(servo3, 5 * num1);
    //     servo_write(servo4, 5 * num1);
    //     delay(200);
    // }
    // delay(500);

    while (digitalRead(26) == 1 || digitalRead(27) == 1) {
        delay(100);
    }

    speed0(0);
    speed1(0);

    digitalWrite(LEDpin, HIGH);
    delay(300);
    digitalWrite(LEDpin, LOW);
    Serial.printf("Start\n");
    for (int count = 0; count < 100; count = count + 1) {

        servo_write(servo2, 10);

        Serial.printf("Ready\n");
        int t = 0;
        t = readDistance();
        // Serial.printf("%d\n", t);

        uint8_t buf[5];
        roginow.clearFlg();
        while(1){
        roginow.receive(buf);
            if(buf[0] == 'G'){
                buf[0] = 0x00;
            break;
            }
        }

        //delay(1000);

        for (int rot = 0; rot < 100; rot = rot + 1) {
            servo_write(servo2, 10+30*rot/100);
            delay(30);
        }

        while (readDistance() > 10) {
            delay(500);
            // Serial.printf("%d\n", t);
        }
        delay(1000);
        float red, green, blue;
        // tcs.setInterrupt(false);  // turn on LED
        digitalWrite(LEDpin, HIGH);
        delay(100);
        tcs.getRGB(red, green, blue);
        Serial.printf("R: %f, G: %f, B: %f\n", red, green, blue);
        delay(60);
        digitalWrite(LEDpin, LOW);  // takes 50ms to read

        // tcs.setInterrupt(true);  // turn off LED

        if (blue > 100) {
            Serial.println("blue");
            servo_write(servo0, 60);
            delay(500);
            servo_write(servo1, 90);
            delay(2000);
            servo_write(servo1, 10);
            delay(500);
        } else if (red > 100 && green > 80) {
            Serial.println("yellow");
            servo_write(servo0, 120);
            delay(500);
            servo_write(servo1, 90);
            delay(2000);
            servo_write(servo1, 10);
            delay(500);
            servo_write(servo0, 60);
        } else if (red > 100 && green <= 80) {
            Serial.println("red");
            servo_write(servo0, 0);
            delay(500);
            servo_write(servo1, 90);
            delay(2000);
            servo_write(servo1, 10);
            delay(500);
            servo_write(servo0, 60);
        }else {
            Serial.printf("???\n");
        }
        delay(100);

        for (int rot = 0; rot < 30; rot = rot + 1) {
            servo_write(servo2, 10+30*(1-rot/30.0));
            delay(30);
        }
    }

    digitalWrite(2, 1);
    delay(200);
    digitalWrite(2, 0);
    // delay(500);
    // for (int num2 = 1; num2 <= 18; num2 = num2 + 1) {
    //     servo_write(servo3, 90 - 5 * num2);
    //     servo_write(servo4, 90 - 5 * num2);
    //     delay(200);
    // }
    // delay(500);
    // for (int num3 = 1; num3 <= 18; num3 = num3 + 1) {
    //     servo_write(servo2, 90 - 5 * num3);
    //     delay(200);
    // }
    // delay(500);
    while (1)
        ;
}