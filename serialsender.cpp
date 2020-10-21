#include <Arduino.h>

void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
}


void loop() {
    char sendme[] = {'h','e','l','l','o','\n'};
    Serial1.write(sendme, 6);
    Serial.println("Printed something!");
    delay(100);
}


