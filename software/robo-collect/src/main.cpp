#include <Arduino.h>
#include <Wire.h>

#include <VL53L0X.h>
int array[11];
int diff=0;
int avrg=0;
int prevavrg=0;

VL53L0X sensor;

void setup() {
    Serial.begin(9600);
    Wire.begin();

    sensor.setTimeout(500);
    if (!sensor.init()) {
        Serial.println("Failed to detect and initialize sensor!");
        while (1) {
        }
    }

    // Start continuous back-to-back mode (take readings as
    // fast as possible).  To use continuous timed mode
    // instead, provide a desired inter-measurement period in
    // ms (e.g. sensor.startContinuous(100)).
    sensor.startContinuous();
}

void loop() {
    prevavrg=avrg;
    array[10]=sensor.readRangeContinuousMillimeters();
    for(int i=0;i<10;i++){
        array[i]=array[i+1];
    }

    for(int i=0;i<10;i++){
        avrg+=array[i];
    }

    avrg/=10;
    diff=(avrg-prevavrg);
    

    Serial.printf("%d,%d",avrg,diff);
    Serial.println();

    // Serial.print(sensor.readRangeContinuousMillimeters());
    // if (sensor.timeoutOccurred()) {
    //     Serial.print(" TIMEOUT");
    // }

    // Serial.println();
    delay(10);
}

// /* This example shows how to use continuous mode to take
// range measurements with the VL53L0X. It is based on
// vl53l0x_ContinuousRanging_Example.c from the VL53L0X API.

// The range readings are in units of mm. */
// #include <Arduino.h>
// #include <VL53L0X.h>
// #include <Wire.h>

// VL53L0X sensor;

// void setup() {
//     Serial.begin(9600);
//     Wire.begin();

//     sensor.setTimeout(500);
//     if (!sensor.init()) {
//         Serial.println("Failed to detect and initialize sensor!");
//         while (1) {
//         }
//     }

//     // Start continuous back-to-back mode (take readings as
//     // fast as possible).  To use continuous timed mode
//     // instead, provide a desired inter-measurement period in
//     // ms (e.g. sensor.startContinuous(100)).
//     sensor.startContinuous();
// }

// void loop() {
//     Serial.print(sensor.readRangeContinuousMillimeters());
//     if (sensor.timeoutOccurred()) {
//         Serial.print(" TIMEOUT");
//     }

//     Serial.println();
//     delay(1000);
// }

// // #include <Arduino.h>
// // #include "config.h"

// // // Courtesy http://www.instructables.com/id/How-to-Use-an-RGB-LED/?ALLSTEPS
// // // function to convert a color to its Red, Green, and Blue components.
// // uint8_t color = 0;         // a value from 0 to 255 representing the hue
// // uint32_t R, G, B;          // the Red Green and Blue color components
// // uint8_t brightness = 100;  // 255 is maximum brightness, but can be changed.
// //                            // Might need 256 for common anode to fully turn
// //                            off.

// // void hueToRGB(uint8_t hue, uint8_t brightness) {
// //     uint16_t scaledHue = (hue * 6);
// //     uint8_t segment = scaledHue / 256;  // segment 0 to 5 around the
// //                                         // color wheel
// //     uint16_t segmentOffset =
// //         scaledHue - (segment * 256);  // position within the segment

// //     uint8_t complement = 0;
// //     uint16_t prev = (brightness * (255 - segmentOffset)) / 256;
// //     uint16_t next = (brightness * segmentOffset) / 256;

// //     switch (segment) {
// //         case 0:  // red
// //             R = brightness;
// //             G = next;
// //             B = complement;
// //             break;
// //         case 1:  // yellow
// //             R = prev;
// //             G = brightness;
// //             B = complement;
// //             break;
// //         case 2:  // green
// //             R = complement;
// //             G = brightness;
// //             B = next;
// //             break;
// //         case 3:  // cyan
// //             R = complement;
// //             G = prev;
// //             B = brightness;
// //             break;
// //         case 4:  // blue
// //             R = next;
// //             G = complement;
// //             B = brightness;
// //             break;
// //         case 5:  // magenta
// //         default:
// //             R = brightness;
// //             G = complement;
// //             B = prev;
// //             break;
// //     }
// // }

// // void setup() {
// //     pinMode(15, OUTPUT);
// //     pinMode(25, OUTPUT);
// //     pinMode(17, OUTPUT);
// //     pinMode(18, INPUT_PULLUP);
// //     pinMode(LEDR, OUTPUT);
// //     pinMode(LEDG, OUTPUT);
// //     pinMode(LEDB, OUTPUT);
// //     pinMode(SERVO0,OUTPUT);
// //     ledcAttachPin(LEDR, 1);
// //     ledcAttachPin(LEDG, 2);
// //     ledcAttachPin(LEDB, 3);
// //     ledcAttachPin(BUZZER,5);
// //     ledcSetup(1, 12000, 8);
// //     ledcSetup(2, 12000, 8);
// //     ledcSetup(3, 12000, 8);
// //     ledcAttachPin(SERVO0,4);
// //     ledcSetup(4,50,16);
// //     ledcSetup(5,261,8);
// //     pinMode(5, OUTPUT);
// //     pinMode(19, OUTPUT);
// //     digitalWrite(5, 0);
// //     digitalWrite(19, 0);
// //     Serial.begin(9600);
// //         ledcWrite(1, 100);
// //     ledcWrite(2, 100);
// //     ledcWrite(3, 100);
// // }

// // void loop() {
// //     ledcWriteNote(5,NOTE_C,4);
// //     delay(1000);
// //     ledcWriteNote(5,NOTE_D,4);
// //     delay(1000);
// //     ledcWriteNote(5,NOTE_E,4);
// //     delay(1000);
// //     ledcWriteNote(5,NOTE_F,4);
// //     delay(1000);
// //     ledcWriteNote(5,NOTE_G,4);
// //     delay(1000);
// //     ledcWriteNote(5,NOTE_A,4);
// //     delay(1000);
// //     ledcWriteNote(5,NOTE_B,4);
// //     delay(1000);
// //     ledcWriteNote(5,NOTE_C,5);
// //     delay(1000);

// //     // ledcWrite(4,7);
// //     // delay(1000);
// //     // ledcWrite(4,30);
// //     // delay(1000);

// //     // for (color = 0; color < 255; color++) {  // Slew through the color
// //     spectrum

// //     //     hueToRGB(color, brightness);  // call function to convert hue to
// //     RGB

// //     //     // write the RGB values to the pins
// //     //     ledcWrite(1, R);  // write red component to channel 1, etc.
// //     //     ledcWrite(2, G);
// //     //     ledcWrite(3, B);

// //     //     delay(20);  // full cycle of rgb over 256 colors takes 26 seconds
// //     // }

// //     // digitalWrite(17,0);
// //     // while(1){
// //     //     if(digitalRead(18)==0){
// //     //         break;
// //     //     }
// //     // }
// //     // while(1){
// //     //     digitalWrite(15,1);
// //     //     digitalWrite(25,1);
// //     //     delay(2);
// //     //     digitalWrite(15,0);
// //     //     digitalWrite(25,0);
// //     //     delay(2);
// //     //     // Serial.printf("hi\n");
// //     // }
// // }