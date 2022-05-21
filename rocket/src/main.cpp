#include <Arduino.h>
#include <SFE_BMP180.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <Wire.h>

// Config
const long failsafe_deploy_time = 126 * 1000;
const double launch_threshhold = 3.0;  // roc threshold for launch
const double open_threshold = 0.5;     // roc threshold for parachute door open
const long poll_interval = 100;        // delay between measurements in milliseconds (1000 milliseconds = 1 second)
const long n = 5;                      // number of altitude readings to store
bool logging = false;                  // flag to indicate if data should be sent over bluetooth
const long Serial_interval = 1000;     // interval at which data is sent to bluetooth

// SoftwareSerial Bt(2, 3);  // rx, tx
char input = 0;

Servo right_servo;
Servo left_servo;

unsigned long timer;  // time since arduino boot

SFE_BMP180 bmp;
double ref_pressure;                               // reference pressure
double alt;                                        // current altitude relative to reference
double roc;                                        // rate of change of altitude (average of roc of last n readings) in meters per second
unsigned long timer2;                              // last time altitude was measured
const double multiplier = poll_interval / 1000.0;  // used to convert roc to m/s

double altitudes[n] = {0};  // array of most recent altitudes
int next;                   // index to store next altitude reading

bool launched = false;  // flag to indicate if rocket has been launched
bool open = false;      // flag to indicate if servos are open

unsigned long timer3 = 0;  // last time data was sent over bluetooth

double getTemperature() {
    char status;
    double t;

    status = bmp.startTemperature();
    if (status != 0) {
        delay(status);
        status = bmp.getTemperature(t);
        if (status != 0) {
            return t;
        } else
            Serial.println(
                "[BMP180] Error retrieving temperature measurement.");
    } else
        Serial.println("[BMP180] Error starting temperature measurement.");
}

double getPressure(double t) {
    char status;
    double p;

    status = bmp.startPressure(3);  // oversampling rate, 0-3 lowest to highest res
    if (status != 0) {
        delay(status);
        status = bmp.getPressure(p, t);
        if (status != 0) {
            return (p);
        } else
            Serial.println("[BMP180] Error retrieving pressure measurement.");
    } else
        Serial.println("[BMP180] Error starting pressure measurement.");
}

double getAltitude() {
    double t, a, p;

    t = getTemperature();
    p = getPressure(t);
    a = bmp.altitude(p, ref_pressure);

    return (a);
}

void openDoor() {
    right_servo.write(180);
    left_servo.write(0);

    open = true;
    Serial.println("[Door] Latch opened.");
}

void closeDoor() {
    right_servo.write(90);
    left_servo.write(90);

    open = false;
    Serial.println("[Door] Latch closed.");
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);

    right_servo.attach(4);
    left_servo.attach(5);

    closeDoor();

    if (bmp.begin()) {
        Serial.println("[BMP180] Init success.");
    } else {
        Serial.println("[BMP180] Init failed.");
    }

    ref_pressure = getPressure(getTemperature());
}

void loop() {
    if (millis() - timer > failsafe_deploy_time) {
        Serial.println("[Failsafe] Deploying parachute.");
    }

    if (millis() - timer2 > poll_interval) {
        timer2 = millis();
        alt = getAltitude();
        altitudes[next] = alt;

        if (next == n - 1) {
            next = 0;
        } else {
            next++;
        }

        double sum = 0;
        int a = next;
        int b;
        for (int i = 0; i < n; i++) {
            if (a == n - 1) {
                b = 0;
            } else {
                b = a + 1;
            }
            sum += altitudes[a] - altitudes[b];
        }

        roc = sum / n * multiplier;

        if (roc > launch_threshhold) {
            launched = true;
        } else if (roc < open_threshold && launched && !open) {
            openDoor();
        }
    }

    if (millis() - timer3 >= Serial_interval && logging) {  // send data over bluetooth every interval
        timer3 = millis();
        Serial.println("[BMP180] Altitude: " + String(alt) + "m | ROC: " + String(roc) + "m/s.");
    }

    if (Serial.available())  // handle manual commands issued over bluetooth
    {
        input = Serial.read();

        switch (input) {
            case 'p':
                Serial.println("[Status] Pong.");
                break;
            case 's':
                Serial.println("[Status] Altitude: " + String(alt) + "m | ROC: " + String(roc) + "m/s | Launched: " + String(launched) + " | Open: " + String(open) + " | Logging: " + String(logging) + ".");
                break;
            case 'r':
                openDoor();
                launched = false;
                logging = false;
                Serial.println("[Status] Reset.");
                break;
            case 'l':
                if (logging) {
                    logging = false;
                    Serial.println("[Status] Logging stopped.");
                } else {
                    logging = true;
                    Serial.println("[Status] Logging started.");
                }
                break;
            case 'o':
                openDoor();
                if (logging) {
                    logging = false;
                    Serial.println("[Status] Logging automatically stopped.");
                }
                break;
            case 'c':
                closeDoor();
                if (!logging) {
                    logging = true;
                    Serial.println("[Status] Logging automatically started.");
                }
                break;
        }
    }
}