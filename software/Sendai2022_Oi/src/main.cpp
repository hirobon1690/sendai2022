// alt+shift+Fでキレイキレイ
#include <PCA9685.h>  //PCA9685用ヘッダーファイル（秋月電子通商作成）
#include <SPI.h>
#include <Wire.h>
#include <arduino.h>

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

void servo_write(int ch, int ang) {  //動かすサーボチャンネルと角度を指定
  ang = map(ang, 0, 180, SERVOMIN,
            SERVOMAX);  //角度（0～180）をPWMのパルス幅（150～600）に変換
  pwm.setPWM(ch, 0, ang);
  // delay(1);
}

int readDistance() {
  unsigned int dac[2];
  Wire.beginTransmission(
      GP2Y0E03_ADDR);  // I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(DISTANCE_ADDR);  //距離の測定
  Wire.endTransmission();  // I2Cスレーブ「Arduino Uno」のデータ送信終了

  Wire.requestFrom(GP2Y0E03_ADDR,
                   2);  // I2Cデバイス「GP2Y0E03」に2Byteのデータ要求
  for (int i = 0; i < 2; i++) {
    dac[i] = Wire.read();  // dacにI2Cデバイス「GP2Y0E03」のデータ読み込み
  }
  Wire.endTransmission();  // I2Cスレーブ「Arduino Uno」のデータ送信終了

  return ((dac[0] * 16 + dac[1]) / 16) / (2 * 2);  //距離(cm)を計算
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

  delay(500);  // 500msec待機(0.5秒待機)
}

void loop() {
  //変数宣言
  int t = 0;
  int count;
  for (count = 0; count < 15; count = count + 1) {
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
    } else if (green > 100) {
    } else {
    }
    delay(3000);
  }
}