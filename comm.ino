char input = 0;

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    Serial.print("Enter a character: ");
    if (Serial.available()) {
        input = Serial.read();
        switch (input) {
            case '0':
                digitalWrite(LED_BUILTIN, LOW);
                Serial.print("builtin set low");
                break;
            case '1':
                digitalWrite(LED_BUILTIN, HIGH);
                Serial.print("builtin set high");
                break;
        }
        Serial.println(input);
    }
    delay(3);
}