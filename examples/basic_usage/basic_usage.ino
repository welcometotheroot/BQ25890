/**
 * BQ25890 Basic Usage Example
 *
 * This example demonstrates basic usage of the BQ25890 battery charger library.
 * It shows how to initialize the device, configure charging parameters,
 * and read status/ADC values.
 *
 * Connections:
 *   BQ25890 SDA -> Arduino SDA
 *   BQ25890 SCL -> Arduino SCL
 *   BQ25890 VCC -> 3.3V
 *   BQ25890 GND -> GND
 */

#include <Wire.h>
#include <BQ25890.h>

BQ25890 charger;

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10); // Wait for serial port (USB boards)
    }

    Serial.println("BQ25890 Battery Charger Example");
    Serial.println("================================");

    // Initialize I2C
    Wire.begin();

    // Initialize the charger
    if (!charger.begin(Wire)) {
        Serial.println("ERROR: BQ25890 not found!");
        Serial.println("Check wiring and I2C address.");
        while (1) {
            delay(1000);
        }
    }

    Serial.print("Device ID: 0x");
    Serial.println(charger.getDeviceID(), HEX);
    Serial.print("Revision: ");
    Serial.println(charger.getDeviceRevision());
    Serial.println();

    // Configure charging parameters for a typical Li-Ion battery
    configureCharger();

    Serial.println("Charger configured. Starting monitoring...");
    Serial.println();
}

void configureCharger() {
    // Set charge voltage limit (4.2V for standard Li-Ion)
    charger.setChargeVoltage(4200);
    Serial.print("Charge Voltage: ");
    Serial.print(charger.getChargeVoltage());
    Serial.println(" mV");

    // Set fast charge current (2A)
    charger.setFastChargeCurrent(2000);
    Serial.print("Fast Charge Current: ");
    Serial.print(charger.getFastChargeCurrent());
    Serial.println(" mA");

    // Set input current limit (2A)
    charger.setInputCurrentLimit(2000);
    Serial.print("Input Current Limit: ");
    Serial.print(charger.getInputCurrentLimit());
    Serial.println(" mA");

    // Set minimum system voltage (3.5V)
    charger.setMinSystemVoltage(3500);
    Serial.print("Min System Voltage: ");
    Serial.print(charger.getMinSystemVoltage());
    Serial.println(" mV");

    // Set precharge current (256mA)
    charger.setPrechargeCurrent(256);
    Serial.print("Precharge Current: ");
    Serial.print(charger.getPrechargeCurrent());
    Serial.println(" mA");

    // Set termination current (128mA)
    charger.setTerminationCurrent(128);
    Serial.print("Termination Current: ");
    Serial.print(charger.getTerminationCurrent());
    Serial.println(" mA");

    // Disable watchdog for autonomous operation
    charger.setWatchdogTimer(WatchdogTimer::DISABLED);
    Serial.println("Watchdog: Disabled");

    // Enable charging
    charger.setChargeEnabled(true);
    Serial.print("Charging: ");
    Serial.println(charger.getChargeEnabled() ? "Enabled" : "Disabled");

    Serial.println();
}

void loop() {
    // Start ADC conversion (blocking)
    charger.startADCConversion();

    // Print status
    Serial.println("--- Status ---");
    Serial.print("VBUS: ");
    Serial.println(charger.getVBUSStatusString());

    Serial.print("Charge Status: ");
    Serial.println(charger.getChargeStatusString());

    Serial.print("Power Good: ");
    Serial.println(charger.isPowerGood() ? "Yes" : "No");

    // Print ADC readings
    Serial.println();
    Serial.println("--- ADC Readings ---");

    Serial.print("Battery Voltage: ");
    Serial.print(charger.getBatteryVoltage());
    Serial.println(" mV");

    Serial.print("System Voltage: ");
    Serial.print(charger.getSystemVoltage());
    Serial.println(" mV");

    Serial.print("VBUS Voltage: ");
    Serial.print(charger.getVBUSVoltage());
    Serial.println(" mV");

    Serial.print("Charge Current: ");
    Serial.print(charger.getChargeCurrent());
    Serial.println(" mA");

    Serial.print("TS Percent: ");
    Serial.print(charger.getTSPercent(), 1);
    Serial.println(" %");

    // Check for faults
    uint8_t faults = charger.readFaults();
    if (faults != 0) {
        Serial.println();
        Serial.println("--- FAULTS DETECTED ---");

        if (charger.isWatchdogFault()) {
            Serial.println("  Watchdog Fault!");
        }
        if (charger.isBoostFault()) {
            Serial.println("  Boost Fault!");
        }
        if (charger.isBatteryFault()) {
            Serial.println("  Battery OVP Fault!");
        }

        ChargeFault chgFault = charger.getChargeFault();
        if (chgFault != ChargeFault::NORMAL) {
            Serial.print("  Charge Fault: ");
            Serial.println(charger.getChargeFaultString());
        }

        NTCFault ntcFault = charger.getNTCFault();
        if (ntcFault != NTCFault::NORMAL) {
            Serial.print("  NTC Fault: ");
            Serial.println(charger.getNTCFaultString());
        }
    }

    // Check DPM status
    if (charger.isVINDPMActive()) {
        Serial.println();
        Serial.println("VINDPM Active (Input voltage limited)");
    }
    if (charger.isIINDPMActive()) {
        Serial.println();
        Serial.print("IINDPM Active, Limit: ");
        Serial.print(charger.getIDPMLimit());
        Serial.println(" mA");
    }

    // Check thermal status
    if (charger.isThermalRegulation()) {
        Serial.println();
        Serial.println("Thermal Regulation Active!");
    }

    Serial.println();
    Serial.println("================================");
    Serial.println();

    delay(2000); // Update every 2 seconds
}
