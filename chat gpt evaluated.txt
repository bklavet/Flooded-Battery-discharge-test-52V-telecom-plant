const int NUM_SAMPLES = 10;
const float VOLTAGE_DIVIDER = 11.0;
const float CALIBRATION_DELAY = 10000.0;
const float REFERENCE_VOLTAGE = 5.11;

float analogReferenceVoltage = REFERENCE_VOLTAGE;
float voltageDividerFactor = 0.0;

void setup()
{
    Serial.begin(9600);
    Serial.println("Battery discharge routine. For Flooded Batteries Only!");
    Serial.println("V = Hyperterm");

    Serial.println("Enter float voltage:");
    float floatV = waitForFloat();
    analogReferenceVoltage = floatV / VOLTAGE_DIVIDER;

    Serial.println("Calibrating...");
    int sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sum += analogRead(A2);
        delay(20);
    }
    float voltage = (sum / (float)NUM_SAMPLES) * analogReferenceVoltage / 1024.0;
    voltageDividerFactor = floatV / voltage;

    Serial.println("Calibration complete!");
    Serial.println("Turn off rectifiers!");
    delay(CALIBRATION_DELAY);
}

void loop()
{
    int sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sum += analogRead(A2);
        delay(20);
    }
    float voltage = (sum / (float)NUM_SAMPLES) * analogReferenceVoltage / 1024.0;
    float floatVoltage = voltage * voltageDividerFactor;

    Serial.print("Float Voltage = ");
    Serial.print(floatVoltage);
    Serial.println(" V");
    delay(500);
}

float waitForFloat() {
    String inputString = "";
    while (Serial.available() == 0) {
        delay(50);
    }
    while (Serial.available() > 0) {
        char inChar = Serial.read();
        if (isDigit(inChar) || inChar == '.') {
            inputString += inChar;
        }
        delay(2);
    }
    return inputString.toFloat();
}
