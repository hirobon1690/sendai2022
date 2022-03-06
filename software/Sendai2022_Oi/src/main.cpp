#include <Wire.h>
#include <arduino.h>

//アドレス指定
#define GP2Y0E03_ADDR 0x40
#define DISTANCE_ADDR 0x5E

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

  delay(500);  // 500msec待機(0.5秒待機)
}

void loop() {
  //変数宣言
  int t = 0;
  t = readDistance();
  Serial.printf("%d\n", t);

  delay(1000);  // 1000msec待機(1秒待機)
}