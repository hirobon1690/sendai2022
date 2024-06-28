# roginow - KRA wireless communication library for ESP32
## 概要
roginowは，[ESP-NOW](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html)プロトコルのラッパーで，ESP-NOWに基本的な再送機能・誤り訂正を提供します．
以下，2台のESP32をA, Bとする．

### 送信フロー
![image](https://github.com/KeioRoboticsAssociation/roginow/assets/58695125/3152dd1b-e077-4c3c-aada-5e6cd5dde7b9)

```cpp
bool Roginow::send(uint8_t msg);
```
内でこれらの処理を行っている．成功/失敗を`bool`で返す．pong待ちのタイムアウトは100msにしてある．

### 受信フロー
![image](https://github.com/KeioRoboticsAssociation/roginow/assets/58695125/11af8492-6ef7-475c-97cd-36a66646288d)


```cpp
bool Roginow::receive(uint8_t* buf);
```
内でこれらの処理を行っている．成功/失敗を`bool`で返す．なお，受信待ちのタイムアウトをms単位で設定できる．
