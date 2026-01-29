# BQ25890 Arduino Driver

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/welcometotheroot/library/BQ25890.svg)](https://registry.platformio.org/libraries/welcometotheroot/BQ25890)

Arduino-compatible I2C driver library for the [Texas Instruments BQ25890](https://www.ti.com/product/BQ25890) battery charger IC.

> **Note:** This library has only been tested with the **stm32duino** (STM32 Arduino) framework. It should work with other Arduino-compatible platforms that provide the standard `Wire` library, but this has not been verified.

## Features

- Full register coverage (REG00-REG14)
- Human-readable units (mV, mA) with automatic register conversion
- Type-safe enums for multi-value settings
- Blocking and non-blocking (async) ADC conversion
- Continuous ADC mode support
- Fault detection and reporting
- OTG (boost) mode control
- Ship mode (BATFET disconnect)
- HVDCP current pulse control
- Input Current Optimizer (ICO) support
- Wire instance injection for multi-bus configurations

## Hardware

- **Device:** BQ25890
- **Interface:** I2C (address 0x6A, fixed)
- **Max I2C clock:** 400kHz (Fast mode)

## Quick Start

```cpp
#include <Wire.h>
#include <BQ25890.h>

BQ25890 charger;

void setup() {
    Wire.begin();

    if (!charger.begin(Wire)) {
        // Device not found
        while (1);
    }

    charger.setChargeVoltage(4200);       // 4.2V for standard Li-Ion
    charger.setFastChargeCurrent(2000);   // 2A charge current
    charger.setInputCurrentLimit(2000);   // 2A input limit
    charger.setWatchdogTimer(WatchdogTimer::DISABLED);
    charger.setChargeEnabled(true);
}

void loop() {
    charger.startADCConversion();

    Serial.print("Battery: ");
    Serial.print(charger.getBatteryVoltage());
    Serial.print("mV  Charge: ");
    Serial.print(charger.getChargeCurrent());
    Serial.print("mA  Status: ");
    Serial.println(charger.getChargeStatusString());

    delay(1000);
}
```

## File Structure

```
lib/bq25890/
├── library.json        PlatformIO library manifest
├── README.md           This file
├── src/
│   ├── BQ25890.h       Class declaration and enums (documented)
│   ├── BQ25890.cpp     Implementation
│   └── BQ25890_Regs.h  Register addresses, masks, and constants
└── examples/
    └── basic_usage/
        └── basic_usage.ino
```

## API Overview

| Category | Functions |
|---|---|
| Initialization | `begin()`, `isConnected()`, `reset()` |
| Input Control | `setInputCurrentLimit()`, `setHIZMode()` |
| Charging | `setChargeEnabled()`, `setChargeVoltage()`, `setFastChargeCurrent()` |
| ADC | `startADCConversion()`, `startADCConversionAsync()`, `setADCContinuousMode()` |
| Readings | `getBatteryVoltage()`, `getVBUSVoltage()`, `getChargeCurrent()`, `getSystemVoltage()` |
| Status | `getChargeStatus()`, `getVBUSStatus()`, `isPowerGood()` |
| Faults | `readFaults()`, `getChargeFault()`, `getNTCFault()` |
| OTG Boost | `setOTGEnabled()`, `setBoostVoltage()`, `setBoostCurrentLimit()` |
| Ship Mode | `setBATFETDisabled()`, `setBATFETDelay()`, `setBATFETResetEnabled()` |
| Timers | `setWatchdogTimer()`, `resetWatchdog()`, `setSafetyTimerEnabled()` |
| ICO | `setICOEnabled()`, `forceICO()`, `isICOOptimized()`, `getIDPMLimit()` |

See [BQ25890.h](src/BQ25890.h) for full API documentation.

## License

MIT
