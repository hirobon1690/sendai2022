// alt+shift+Fでキレイキレイ
#include <PCA9685.h>  //PCA9685用ヘッダーファイル（秋月電子通商作成）
#include <SPI.h>
#include <Wire.h>
#include <arduino.h>
#include "Adafruit_TCS34725.h"

hw_timer_t* timer = NULL;

PCA9685 pwm =
    PCA9685(0x40);  // PCA9685のアドレス指定（アドレスジャンパ未接続時）

#define SERVOMIN 150  //最小パルス幅 (標準的なサーボパルスに設定)
#define SERVOMAX 600  //最大パルス幅 (標準的なサーボパルスに設定)

#include "Adafruit_TCS34725.h"

#define redpin 3
#define greenpin 4
#define bluepin 5
#define commonAnode true

byte gammatable[256];

Adafruit_TCS34725 tcs =
    Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

//アドレス指定
#define GP2Y0E03_ADDR 0x40
#define DISTANCE_ADDR 0x5E

const int servo0 = 0;  //射出方向変換用サーボ
const int servo1 = 1;  //ストッパー用サーボ
const int servo2 = 2;  //坂展開用サーボ(車体方向)
const int servo3 = 3;  //坂展開用サーボ(横方向)
const int servo4 = 4;  //坂展開用サーボ(横方向)

const int s0 = 4;
const int s1 = 34;
const int s2 = 35;
const int s3 = 14;

void servo_write(int ch, int ang) {  //動かすサーボチャンネルと角度を指定
    ang = map(ang, 0, 180, SERVOMIN,
              SERVOMAX);  //角度（0～180）をPWMのパルス幅（150～600）に変換
    pwm.setPWM(ch, 0, ang);
    // delay(1);
}

int readDistance() {
    unsigned int dac[2];
    Wire.beginTransmission(
        GP2Y0E03_ADDR);         // I2Cスレーブ「Arduino Uno」のデータ送信開始
    Wire.write(DISTANCE_ADDR);  //距離の測定
    Wire.endTransmission();     // I2Cスレーブ「Arduino Uno」のデータ送信終了

    Wire.requestFrom(GP2Y0E03_ADDR,
                     2);  // I2Cデバイス「GP2Y0E03」に2Byteのデータ要求
    for (int i = 0; i < 2; i++) {
        dac[i] = Wire.read();  // dacにI2Cデバイス「GP2Y0E03」のデータ読み込み
    }
    Wire.endTransmission();  // I2Cスレーブ「Arduino Uno」のデータ送信終了

    return ((dac[0] * 16 + dac[1]) / 16) / (2 * 2);  //距離(cm)を計算
}

int cnt[2] = {0, 0};
int stepcycle[2] = {0, 0};
int stp[2] = {15, 25};

void IRAM_ATTR step() {
    for (int i = 0; i < 2; i++) {      //今回はステッパ2つ分
        if (cnt[i] >= stepcycle[i]) {  //閾値を超えると
            cnt[i] = 0;                //カウンタをリセット
        } else {
            cnt[i] = cnt[i] + 1;  //今回は0.1msごとに呼び出し
        }
        digitalWrite(stp[i], (stepcycle[i] != 0 && cnt[i] <= 20));  // 0.1*20=2ms(HIGHの時間)
    }
}

void speed0(int c0) {
    stepcycle[0] = (int)(5000 / c0);  // 1sでspeed0/100回転
}

void speed1(int c1) {
    stepcycle[1] = (int)(5000 / c1);  // 1sでspeed1/100回転
}

void setup() {
    Serial.begin(9600);  //シリアル通信を9600bpsで初期化
    Wire.begin();        // I2Cを初期化

    ledcAttachPin(redpin, 1);
    ledcAttachPin(greenpin, 2);
    ledcAttachPin(bluepin, 3);
    ledcSetup(1, 12000, 8);
    ledcSetup(2, 12000, 8);
    ledcSetup(3, 12000, 8);

    for (int i = 0; i < 256; i++) {
        float x = i;
        x /= 255;
        x = pow(x, 2.5);
        x *= 255;

        if (commonAnode) {
            gammatable[i] = 255 - x;
        } else {
            gammatable[i] = x;
        }
        // Serial.println(gammatable[i]);
    }

    pwm.begin();         //初期設定 (アドレス0x40用)
    pwm.setPWMFreq(60);  // PWM周期を60Hzに設定 (アドレス0x40用)

    pinMode(5, OUTPUT);
    pinMode(15, OUTPUT);
    pinMode(17, OUTPUT);
    pinMode(19, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(16, OUTPUT);

    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &step, true);
    timerAlarmWrite(timer, 100, true);
    timerAlarmEnable(timer);

    delay(500);
}

void loop() {
    int val0;
    int val1;
    int val2;
    int val3;

    delay(500);

    speed0(50);
    speed1(50);
    val0 = analogRead(s0);
    val1 = analogRead(s1);
    val2 = analogRead(s2);
    val3 = analogRead(s3);
    while (val0 > 200 && val3 > 200) {
        val0 = analogRead(s0);
        val1 = analogRead(s1);
        val2 = analogRead(s2);
        val3 = analogRead(s3);
        delay(100);
    }
    if (val0 <= 200) {
        speed0(0);
        speed1(10);
    } else {
        speed0(10);
        speed1(0);
    }
    while (val0 <= 200 || val3 <= 200) {
        val0 = analogRead(s0);
        val1 = analogRead(s1);
        val2 = analogRead(s2);
        val3 = analogRead(s3);
        delay(100);
    }
    speed0(0);
    speed1(0);
    delay(500);
    speed0(50);
    speed1(50);
    delay(1000);
    val0 = analogRead(s0);
    val1 = analogRead(s1);
    val2 = analogRead(s2);
    val3 = analogRead(s3);
    if (val0 <= 200) {
        speed0(10);
        speed1(50);
        delay(1000);
    } else if (val3 <= 200) {
        speed0(50);
        speed1(10);
        delay(1000);
    }
    while (val0 > 200 || val3 > 200) {
        val0 = analogRead(s0);
        val1 = analogRead(s1);
        val2 = analogRead(s2);
        val3 = analogRead(s3);
        speed0(int(val1 / 10)); //p制御
        speed1(int(val2 / 10));
        delay(100);
    }
    speed0(0);
    speed1(0);
    digitalWrite(5,1);
    digitalWrite(19,0);

    
    for (int num0 = 1; num0 <= 18; num0 = num0 + 1) {
        servo_write(servo2, 5 * num0);
        delay(200);
    }
    delay(500);
    for (int num1 = 1; num1 <= 18; num1 = num1 + 1) {
        servo_write(servo3, 5 * num1);
        servo_write(servo4, 5 * num1);
        delay(200);
    }
    delay(500);
    for (int count = 0; count < 15; count = count + 1) {
        int t = 0;
        t = readDistance();
        // Serial.printf("%d\n", t);
        while (t > 10) {
            delay(500);
            t = readDistance();
        }
        delay(1000);
        float red, green, blue;
        tcs.setInterrupt(false);  // turn on LED
        delay(60);                // takes 50ms to read
        tcs.getRGB(red, green, blue);
        tcs.setInterrupt(true);  // turn off LED

        if (red > 100) {
            servo_write(servo0, 60);
            delay(500);
            servo_write(servo1, 60);
            delay(2000);
            servo_write(servo1, 0);
            delay(500);
            servo_write(servo0, 0);
        } else if (green > 100) {
            servo_write(servo0, -60);
            delay(500);
            servo_write(servo1, 60);
            delay(2000);
            servo_write(servo1, 0);
            delay(500);
            servo_write(servo0, 0);
        } else {
            delay(500);
            servo_write(servo1, 60);
            delay(2000);
            servo_write(servo1, 0);
            delay(500);
        }
        delay(100);
    }
    delay(500);
    for (int num2 = 1; num2 <= 18; num2 = num2 + 1) {
        servo_write(servo3, 90 - 5 * num2);
        servo_write(servo4, 90 - 5 * num2);
        delay(200);
    }
    delay(500);
    for (int num3 = 1; num3 <= 18; num3 = num3 + 1) {
        servo_write(servo2, 90 - 5 * num3);
        delay(200);
    }
    delay(500);
    while (1)
        ;
}