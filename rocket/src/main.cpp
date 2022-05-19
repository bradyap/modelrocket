#include <Arduino.h>
#include <Servo.h>
#include <SoftwareSerial.h>

//SoftwareSerial Bt(2, 3); // rx, tx
char input = 0;

Servo left_servo;
Servo right_servo;

void setup()
{

    pinMode(LED_BUILTIN, OUTPUT);

    left_servo.attach(5);
    right_servo.attach(6);

    Serial.begin(9600);
}

void loop()
{
    //Serial.println("Serial working at 4800.");
    //Bt.println("Bluetooth working at 9600.");
    
    if (Serial.available())
    {
        input = Serial.read();

        switch (input)
        {
        case 's':
            Serial.println("Connected. Flashing onboard LED.");
            digitalWrite(LED_BUILTIN, HIGH);
            delay(1000);
            digitalWrite(LED_BUILTIN, LOW);
            break;
        case 'o':
            left_servo.write(100);
            right_servo.write(100);
            Serial.println("Parachute door opened.");
            break;
        case 'c':
            left_servo.write(50);
            right_servo.write(50);
            Serial.println("Parachute door closed.");
            break;
        }
    }
}
