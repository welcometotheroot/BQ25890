#include "BQ25890.h"

BQ25890::BQ25890() : _wire(nullptr), _lastFaults(0) {
}

// ============================================================================
// Initialization and Core I2C
// ============================================================================

bool BQ25890::begin(TwoWire &wire) {
    _wire = &wire;

    if (!isConnected()) {
        return false;
    }

    // Verify device ID
    uint8_t partNumber = getPartNumber();
    if (partNumber != BQ25890_PART_NUMBER) {
        return false;
    }

    return true;
}

bool BQ25890::isConnected() {
    if (_wire == nullptr) return false;

    _wire->beginTransmission(BQ25890_I2C_ADDRESS);
    return (_wire->endTransmission() == 0);
}

uint8_t BQ25890::getDeviceID() {
    return getPartNumber();
}

uint8_t BQ25890::getDeviceRevision() {
    return getRevision();
}

void BQ25890::reset() {
    resetRegisters();
}

// ============================================================================
// Input Source Control (REG00)
// ============================================================================

void BQ25890::setHIZMode(bool enable) {
    writeBits(BQ25890_REG00, BQ25890_REG00_EN_HIZ_MASK, BQ25890_REG00_EN_HIZ_SHIFT, enable ? 1 : 0);
}

bool BQ25890::getHIZMode() {
    return readBits(BQ25890_REG00, BQ25890_REG00_EN_HIZ_MASK, BQ25890_REG00_EN_HIZ_SHIFT) != 0;
}

void BQ25890::setILIMPinEnabled(bool enable) {
    writeBits(BQ25890_REG00, BQ25890_REG00_EN_ILIM_MASK, BQ25890_REG00_EN_ILIM_SHIFT, enable ? 1 : 0);
}

bool BQ25890::getILIMPinEnabled() {
    return readBits(BQ25890_REG00, BQ25890_REG00_EN_ILIM_MASK, BQ25890_REG00_EN_ILIM_SHIFT) != 0;
}

void BQ25890::setInputCurrentLimit(uint16_t mA) {
    // Range: 100mA to 3250mA, 50mA steps
    if (mA < 100) mA = 100;
    if (mA > 3250) mA = 3250;
    uint8_t value = (mA - BQ25890_REG00_IINLIM_BASE) / BQ25890_REG00_IINLIM_LSB;
    writeBits(BQ25890_REG00, BQ25890_REG00_IINLIM_MASK, BQ25890_REG00_IINLIM_SHIFT, value);
}

uint16_t BQ25890::getInputCurrentLimit() {
    uint8_t value = readBits(BQ25890_REG00, BQ25890_REG00_IINLIM_MASK, BQ25890_REG00_IINLIM_SHIFT);
    return BQ25890_REG00_IINLIM_BASE + (value * BQ25890_REG00_IINLIM_LSB);
}

// ============================================================================
// Boost Mode Temperature Thresholds (REG01)
// ============================================================================

void BQ25890::setBoostHotThreshold(uint8_t threshold) {
    if (threshold > 3) threshold = 3;
    writeBits(BQ25890_REG01, BQ25890_REG01_BHOT_MASK, BQ25890_REG01_BHOT_SHIFT, threshold);
}

uint8_t BQ25890::getBoostHotThreshold() {
    return readBits(BQ25890_REG01, BQ25890_REG01_BHOT_MASK, BQ25890_REG01_BHOT_SHIFT);
}

void BQ25890::setBoostColdThreshold(bool threshold) {
    writeBits(BQ25890_REG01, BQ25890_REG01_BCOLD_MASK, BQ25890_REG01_BCOLD_SHIFT, threshold ? 1 : 0);
}

bool BQ25890::getBoostColdThreshold() {
    return readBits(BQ25890_REG01, BQ25890_REG01_BCOLD_MASK, BQ25890_REG01_BCOLD_SHIFT) != 0;
}

void BQ25890::setVINDPMOffset(uint16_t mV) {
    // Range: 0-3100mV, 100mV steps
    if (mV > 3100) mV = 3100;
    uint8_t value = mV / BQ25890_REG01_VINDPM_OS_LSB;
    if (value > 0x1F) value = 0x1F;
    writeBits(BQ25890_REG01, BQ25890_REG01_VINDPM_OS_MASK, BQ25890_REG01_VINDPM_OS_SHIFT, value);
}

uint16_t BQ25890::getVINDPMOffset() {
    uint8_t value = readBits(BQ25890_REG01, BQ25890_REG01_VINDPM_OS_MASK, BQ25890_REG01_VINDPM_OS_SHIFT);
    return value * BQ25890_REG01_VINDPM_OS_LSB;
}

// ============================================================================
// ADC Control (REG02)
// ============================================================================

void BQ25890::startADCConversion() {
    // Start one-shot conversion and wait for completion
    startADCConversionAsync();

    // Wait up to 1 second for conversion to complete
    unsigned long startTime = millis();
    while (!isADCConversionComplete()) {
        if (millis() - startTime > 1000) {
            break; // Timeout
        }
        delay(10);
    }
}

bool BQ25890::startADCConversionAsync() {
    writeBits(BQ25890_REG02, BQ25890_REG02_CONV_START_MASK, BQ25890_REG02_CONV_START_SHIFT, 1);
    return true;
}

bool BQ25890::isADCConversionComplete() {
    // Conversion is complete when CONV_START bit returns to 0
    return readBits(BQ25890_REG02, BQ25890_REG02_CONV_START_MASK, BQ25890_REG02_CONV_START_SHIFT) == 0;
}

void BQ25890::setADCContinuousMode(bool enable) {
    writeBits(BQ25890_REG02, BQ25890_REG02_CONV_RATE_MASK, BQ25890_REG02_CONV_RATE_SHIFT, enable ? 1 : 0);
}

bool BQ25890::getADCContinuousMode() {
    return readBits(BQ25890_REG02, BQ25890_REG02_CONV_RATE_MASK, BQ25890_REG02_CONV_RATE_SHIFT) != 0;
}

void BQ25890::setBoostFrequency(bool freq500kHz) {
    writeBits(BQ25890_REG02, BQ25890_REG02_BOOST_FREQ_MASK, BQ25890_REG02_BOOST_FREQ_SHIFT, freq500kHz ? 1 : 0);
}

void BQ25890::setICOEnabled(bool enable) {
    writeBits(BQ25890_REG02, BQ25890_REG02_ICO_EN_MASK, BQ25890_REG02_ICO_EN_SHIFT, enable ? 1 : 0);
}

bool BQ25890::getICOEnabled() {
    return readBits(BQ25890_REG02, BQ25890_REG02_ICO_EN_MASK, BQ25890_REG02_ICO_EN_SHIFT) != 0;
}

void BQ25890::setHVDCPEnabled(bool enable) {
    writeBits(BQ25890_REG02, BQ25890_REG02_HVDCP_EN_MASK, BQ25890_REG02_HVDCP_EN_SHIFT, enable ? 1 : 0);
}

void BQ25890::setMaxChargeEnabled(bool enable) {
    writeBits(BQ25890_REG02, BQ25890_REG02_MAXC_EN_MASK, BQ25890_REG02_MAXC_EN_SHIFT, enable ? 1 : 0);
}

void BQ25890::forceDPDMDetection() {
    writeBits(BQ25890_REG02, BQ25890_REG02_FORCE_DPDM_MASK, BQ25890_REG02_FORCE_DPDM_SHIFT, 1);
}

void BQ25890::setAutoDPDMEnabled(bool enable) {
    writeBits(BQ25890_REG02, BQ25890_REG02_AUTO_DPDM_MASK, BQ25890_REG02_AUTO_DPDM_SHIFT, enable ? 1 : 0);
}

// ============================================================================
// Charge and OTG Configuration (REG03)
// ============================================================================

void BQ25890::setBatteryLoadEnabled(bool enable) {
    writeBits(BQ25890_REG03, BQ25890_REG03_BAT_LOADEN_MASK, BQ25890_REG03_BAT_LOADEN_SHIFT, enable ? 1 : 0);
}

void BQ25890::resetWatchdog() {
    writeBits(BQ25890_REG03, BQ25890_REG03_WD_RST_MASK, BQ25890_REG03_WD_RST_SHIFT, 1);
}

void BQ25890::setOTGEnabled(bool enable) {
    writeBits(BQ25890_REG03, BQ25890_REG03_OTG_CONFIG_MASK, BQ25890_REG03_OTG_CONFIG_SHIFT, enable ? 1 : 0);
}

bool BQ25890::getOTGEnabled() {
    return readBits(BQ25890_REG03, BQ25890_REG03_OTG_CONFIG_MASK, BQ25890_REG03_OTG_CONFIG_SHIFT) != 0;
}

void BQ25890::setChargeEnabled(bool enable) {
    writeBits(BQ25890_REG03, BQ25890_REG03_CHG_CONFIG_MASK, BQ25890_REG03_CHG_CONFIG_SHIFT, enable ? 1 : 0);
}

bool BQ25890::getChargeEnabled() {
    return readBits(BQ25890_REG03, BQ25890_REG03_CHG_CONFIG_MASK, BQ25890_REG03_CHG_CONFIG_SHIFT) != 0;
}

void BQ25890::setMinSystemVoltage(uint16_t mV) {
    // Range: 3000-3700mV, 100mV steps
    if (mV < 3000) mV = 3000;
    if (mV > 3700) mV = 3700;
    uint8_t value = (mV - BQ25890_REG03_SYS_MIN_BASE) / BQ25890_REG03_SYS_MIN_LSB;
    if (value > 7) value = 7;
    writeBits(BQ25890_REG03, BQ25890_REG03_SYS_MIN_MASK, BQ25890_REG03_SYS_MIN_SHIFT, value);
}

uint16_t BQ25890::getMinSystemVoltage() {
    uint8_t value = readBits(BQ25890_REG03, BQ25890_REG03_SYS_MIN_MASK, BQ25890_REG03_SYS_MIN_SHIFT);
    return BQ25890_REG03_SYS_MIN_BASE + (value * BQ25890_REG03_SYS_MIN_LSB);
}

// ============================================================================
// Current Pulse Control and Charge Current (REG04)
// ============================================================================

void BQ25890::setCurrentPulseControlEnabled(bool enable) {
    writeBits(BQ25890_REG04, BQ25890_REG04_EN_PUMPX_MASK, BQ25890_REG04_EN_PUMPX_SHIFT, enable ? 1 : 0);
}

bool BQ25890::getCurrentPulseControlEnabled() {
    return readBits(BQ25890_REG04, BQ25890_REG04_EN_PUMPX_MASK, BQ25890_REG04_EN_PUMPX_SHIFT) != 0;
}

void BQ25890::setFastChargeCurrent(uint16_t mA) {
    // Range: 0-5056mA, 64mA steps
    if (mA > 5056) mA = 5056;
    uint8_t value = mA / BQ25890_REG04_ICHG_LSB;
    if (value > 0x7F) value = 0x7F;
    writeBits(BQ25890_REG04, BQ25890_REG04_ICHG_MASK, BQ25890_REG04_ICHG_SHIFT, value);
}

uint16_t BQ25890::getFastChargeCurrent() {
    uint8_t value = readBits(BQ25890_REG04, BQ25890_REG04_ICHG_MASK, BQ25890_REG04_ICHG_SHIFT);
    return value * BQ25890_REG04_ICHG_LSB;
}

// ============================================================================
// Precharge and Termination Current (REG05)
// ============================================================================

void BQ25890::setPrechargeCurrent(uint16_t mA) {
    // Range: 64-1024mA, 64mA steps
    if (mA < 64) mA = 64;
    if (mA > 1024) mA = 1024;
    uint8_t value = (mA - BQ25890_REG05_IPRECHG_BASE) / BQ25890_REG05_IPRECHG_LSB;
    if (value > 0x0F) value = 0x0F;
    writeBits(BQ25890_REG05, BQ25890_REG05_IPRECHG_MASK, BQ25890_REG05_IPRECHG_SHIFT, value);
}

uint16_t BQ25890::getPrechargeCurrent() {
    uint8_t value = readBits(BQ25890_REG05, BQ25890_REG05_IPRECHG_MASK, BQ25890_REG05_IPRECHG_SHIFT);
    return BQ25890_REG05_IPRECHG_BASE + (value * BQ25890_REG05_IPRECHG_LSB);
}

void BQ25890::setTerminationCurrent(uint16_t mA) {
    // Range: 64-1024mA, 64mA steps
    if (mA < 64) mA = 64;
    if (mA > 1024) mA = 1024;
    uint8_t value = (mA - BQ25890_REG05_ITERM_BASE) / BQ25890_REG05_ITERM_LSB;
    if (value > 0x0F) value = 0x0F;
    writeBits(BQ25890_REG05, BQ25890_REG05_ITERM_MASK, BQ25890_REG05_ITERM_SHIFT, value);
}

uint16_t BQ25890::getTerminationCurrent() {
    uint8_t value = readBits(BQ25890_REG05, BQ25890_REG05_ITERM_MASK, BQ25890_REG05_ITERM_SHIFT);
    return BQ25890_REG05_ITERM_BASE + (value * BQ25890_REG05_ITERM_LSB);
}

// ============================================================================
// Charge Voltage Settings (REG06)
// ============================================================================

void BQ25890::setChargeVoltage(uint16_t mV) {
    // Range: 3840-4608mV, 16mV steps
    if (mV < 3840) mV = 3840;
    if (mV > 4608) mV = 4608;
    uint8_t value = (mV - BQ25890_REG06_VREG_BASE) / BQ25890_REG06_VREG_LSB;
    if (value > 0x3F) value = 0x3F;
    writeBits(BQ25890_REG06, BQ25890_REG06_VREG_MASK, BQ25890_REG06_VREG_SHIFT, value);
}

uint16_t BQ25890::getChargeVoltage() {
    uint8_t value = readBits(BQ25890_REG06, BQ25890_REG06_VREG_MASK, BQ25890_REG06_VREG_SHIFT);
    return BQ25890_REG06_VREG_BASE + (value * BQ25890_REG06_VREG_LSB);
}

void BQ25890::setBatteryLowVThreshold(bool use3V) {
    writeBits(BQ25890_REG06, BQ25890_REG06_BATLOWV_MASK, BQ25890_REG06_BATLOWV_SHIFT, use3V ? 1 : 0);
}

bool BQ25890::getBatteryLowVThreshold() {
    return readBits(BQ25890_REG06, BQ25890_REG06_BATLOWV_MASK, BQ25890_REG06_BATLOWV_SHIFT) != 0;
}

void BQ25890::setRechargeThreshold(bool use200mV) {
    writeBits(BQ25890_REG06, BQ25890_REG06_VRECHG_MASK, BQ25890_REG06_VRECHG_SHIFT, use200mV ? 1 : 0);
}

bool BQ25890::getRechargeThreshold() {
    return readBits(BQ25890_REG06, BQ25890_REG06_VRECHG_MASK, BQ25890_REG06_VRECHG_SHIFT) != 0;
}

// ============================================================================
// Termination and Timer Settings (REG07)
// ============================================================================

void BQ25890::setTerminationEnabled(bool enable) {
    writeBits(BQ25890_REG07, BQ25890_REG07_EN_TERM_MASK, BQ25890_REG07_EN_TERM_SHIFT, enable ? 1 : 0);
}

bool BQ25890::getTerminationEnabled() {
    return readBits(BQ25890_REG07, BQ25890_REG07_EN_TERM_MASK, BQ25890_REG07_EN_TERM_SHIFT) != 0;
}

void BQ25890::setSTATPinDisabled(bool disable) {
    writeBits(BQ25890_REG07, BQ25890_REG07_STAT_DIS_MASK, BQ25890_REG07_STAT_DIS_SHIFT, disable ? 1 : 0);
}

bool BQ25890::getSTATPinDisabled() {
    return readBits(BQ25890_REG07, BQ25890_REG07_STAT_DIS_MASK, BQ25890_REG07_STAT_DIS_SHIFT) != 0;
}

void BQ25890::setWatchdogTimer(WatchdogTimer setting) {
    writeBits(BQ25890_REG07, BQ25890_REG07_WATCHDOG_MASK, BQ25890_REG07_WATCHDOG_SHIFT, static_cast<uint8_t>(setting));
}

WatchdogTimer BQ25890::getWatchdogTimer() {
    return static_cast<WatchdogTimer>(readBits(BQ25890_REG07, BQ25890_REG07_WATCHDOG_MASK, BQ25890_REG07_WATCHDOG_SHIFT));
}

void BQ25890::setSafetyTimerEnabled(bool enable) {
    writeBits(BQ25890_REG07, BQ25890_REG07_EN_TIMER_MASK, BQ25890_REG07_EN_TIMER_SHIFT, enable ? 1 : 0);
}

bool BQ25890::getSafetyTimerEnabled() {
    return readBits(BQ25890_REG07, BQ25890_REG07_EN_TIMER_MASK, BQ25890_REG07_EN_TIMER_SHIFT) != 0;
}

void BQ25890::setFastChargeTimer(FastChargeTimer setting) {
    writeBits(BQ25890_REG07, BQ25890_REG07_CHG_TIMER_MASK, BQ25890_REG07_CHG_TIMER_SHIFT, static_cast<uint8_t>(setting));
}

FastChargeTimer BQ25890::getFastChargeTimer() {
    return static_cast<FastChargeTimer>(readBits(BQ25890_REG07, BQ25890_REG07_CHG_TIMER_MASK, BQ25890_REG07_CHG_TIMER_SHIFT));
}

void BQ25890::setJEITALowTempCurrent(bool use20pct) {
    writeBits(BQ25890_REG07, BQ25890_REG07_JEITA_ISET_MASK, BQ25890_REG07_JEITA_ISET_SHIFT, use20pct ? 1 : 0);
}

// ============================================================================
// IR Compensation and Thermal Regulation (REG08)
// ============================================================================

void BQ25890::setIRCompResistance(uint8_t mohm) {
    // Range: 0-140mOhm, 20mOhm steps
    if (mohm > 140) mohm = 140;
    uint8_t value = mohm / BQ25890_REG08_BAT_COMP_LSB;
    if (value > 7) value = 7;
    writeBits(BQ25890_REG08, BQ25890_REG08_BAT_COMP_MASK, BQ25890_REG08_BAT_COMP_SHIFT, value);
}

uint8_t BQ25890::getIRCompResistance() {
    uint8_t value = readBits(BQ25890_REG08, BQ25890_REG08_BAT_COMP_MASK, BQ25890_REG08_BAT_COMP_SHIFT);
    return value * BQ25890_REG08_BAT_COMP_LSB;
}

void BQ25890::setIRCompVoltageClamp(uint16_t mV) {
    // Range: 0-224mV, 32mV steps
    if (mV > 224) mV = 224;
    uint8_t value = mV / BQ25890_REG08_VCLAMP_LSB;
    if (value > 7) value = 7;
    writeBits(BQ25890_REG08, BQ25890_REG08_VCLAMP_MASK, BQ25890_REG08_VCLAMP_SHIFT, value);
}

uint16_t BQ25890::getIRCompVoltageClamp() {
    uint8_t value = readBits(BQ25890_REG08, BQ25890_REG08_VCLAMP_MASK, BQ25890_REG08_VCLAMP_SHIFT);
    return value * BQ25890_REG08_VCLAMP_LSB;
}

void BQ25890::setThermalRegThreshold(ThermalRegulation setting) {
    writeBits(BQ25890_REG08, BQ25890_REG08_TREG_MASK, BQ25890_REG08_TREG_SHIFT, static_cast<uint8_t>(setting));
}

ThermalRegulation BQ25890::getThermalRegThreshold() {
    return static_cast<ThermalRegulation>(readBits(BQ25890_REG08, BQ25890_REG08_TREG_MASK, BQ25890_REG08_TREG_SHIFT));
}

// ============================================================================
// BATFET and Current Pulse Control (REG09)
// ============================================================================

void BQ25890::forceICO() {
    writeBits(BQ25890_REG09, BQ25890_REG09_FORCE_ICO_MASK, BQ25890_REG09_FORCE_ICO_SHIFT, 1);
}

void BQ25890::setSafetyTimer2xEnabled(bool enable) {
    writeBits(BQ25890_REG09, BQ25890_REG09_TMR2X_EN_MASK, BQ25890_REG09_TMR2X_EN_SHIFT, enable ? 1 : 0);
}

void BQ25890::setBATFETDisabled(bool disable) {
    writeBits(BQ25890_REG09, BQ25890_REG09_BATFET_DIS_MASK, BQ25890_REG09_BATFET_DIS_SHIFT, disable ? 1 : 0);
}

bool BQ25890::getBATFETDisabled() {
    return readBits(BQ25890_REG09, BQ25890_REG09_BATFET_DIS_MASK, BQ25890_REG09_BATFET_DIS_SHIFT) != 0;
}

void BQ25890::setJEITAHighTempVoltage(bool useVREG) {
    writeBits(BQ25890_REG09, BQ25890_REG09_JEITA_VSET_MASK, BQ25890_REG09_JEITA_VSET_SHIFT, useVREG ? 1 : 0);
}

void BQ25890::setBATFETDelay(bool useDelay) {
    writeBits(BQ25890_REG09, BQ25890_REG09_BATFET_DLY_MASK, BQ25890_REG09_BATFET_DLY_SHIFT, useDelay ? 1 : 0);
}

void BQ25890::setBATFETResetEnabled(bool enable) {
    writeBits(BQ25890_REG09, BQ25890_REG09_BATFET_RST_MASK, BQ25890_REG09_BATFET_RST_SHIFT, enable ? 1 : 0);
}

bool BQ25890::pumpVoltageUp() {
    writeBits(BQ25890_REG09, BQ25890_REG09_PUMPX_UP_MASK, BQ25890_REG09_PUMPX_UP_SHIFT, 1);
    // Wait for completion (bit auto-clears)
    unsigned long startTime = millis();
    while (readBits(BQ25890_REG09, BQ25890_REG09_PUMPX_UP_MASK, BQ25890_REG09_PUMPX_UP_SHIFT) != 0) {
        if (millis() - startTime > 1000) {
            return false; // Timeout
        }
        delay(10);
    }
    return true;
}

bool BQ25890::pumpVoltageDown() {
    writeBits(BQ25890_REG09, BQ25890_REG09_PUMPX_DN_MASK, BQ25890_REG09_PUMPX_DN_SHIFT, 1);
    // Wait for completion (bit auto-clears)
    unsigned long startTime = millis();
    while (readBits(BQ25890_REG09, BQ25890_REG09_PUMPX_DN_MASK, BQ25890_REG09_PUMPX_DN_SHIFT) != 0) {
        if (millis() - startTime > 1000) {
            return false; // Timeout
        }
        delay(10);
    }
    return true;
}

// ============================================================================
// Boost Mode Settings (REG0A)
// ============================================================================

void BQ25890::setBoostVoltage(uint16_t mV) {
    // Range: 4550-5510mV, 64mV steps
    if (mV < 4550) mV = 4550;
    if (mV > 5510) mV = 5510;
    uint8_t value = (mV - BQ25890_REG0A_BOOSTV_BASE) / BQ25890_REG0A_BOOSTV_LSB;
    if (value > 0x0F) value = 0x0F;
    writeBits(BQ25890_REG0A, BQ25890_REG0A_BOOSTV_MASK, BQ25890_REG0A_BOOSTV_SHIFT, value);
}

uint16_t BQ25890::getBoostVoltage() {
    uint8_t value = readBits(BQ25890_REG0A, BQ25890_REG0A_BOOSTV_MASK, BQ25890_REG0A_BOOSTV_SHIFT);
    return BQ25890_REG0A_BOOSTV_BASE + (value * BQ25890_REG0A_BOOSTV_LSB);
}

void BQ25890::setBoostCurrentLimit(BoostCurrentLimit setting) {
    writeBits(BQ25890_REG0A, BQ25890_REG0A_BOOST_LIM_MASK, BQ25890_REG0A_BOOST_LIM_SHIFT, static_cast<uint8_t>(setting));
}

BoostCurrentLimit BQ25890::getBoostCurrentLimit() {
    return static_cast<BoostCurrentLimit>(readBits(BQ25890_REG0A, BQ25890_REG0A_BOOST_LIM_MASK, BQ25890_REG0A_BOOST_LIM_SHIFT));
}

// ============================================================================
// Status Reading (REG0B) - Interrupt Support
// ============================================================================

VBUSStatus BQ25890::getVBUSStatus() {
    return static_cast<VBUSStatus>(readBits(BQ25890_REG0B, BQ25890_REG0B_VBUS_STAT_MASK, BQ25890_REG0B_VBUS_STAT_SHIFT));
}

ChargeStatus BQ25890::getChargeStatus() {
    return static_cast<ChargeStatus>(readBits(BQ25890_REG0B, BQ25890_REG0B_CHRG_STAT_MASK, BQ25890_REG0B_CHRG_STAT_SHIFT));
}

bool BQ25890::isPowerGood() {
    return readBits(BQ25890_REG0B, BQ25890_REG0B_PG_STAT_MASK, BQ25890_REG0B_PG_STAT_SHIFT) != 0;
}

bool BQ25890::isVSYSRegulation() {
    return readBits(BQ25890_REG0B, BQ25890_REG0B_VSYS_STAT_MASK, BQ25890_REG0B_VSYS_STAT_SHIFT) != 0;
}

const char* BQ25890::getVBUSStatusString() {
    switch (getVBUSStatus()) {
        case VBUSStatus::NO_INPUT:     return "No Input";
        case VBUSStatus::USB_SDP:      return "USB SDP (500mA)";
        case VBUSStatus::USB_CDP:      return "USB CDP (1.5A)";
        case VBUSStatus::USB_DCP:      return "USB DCP (3.25A)";
        case VBUSStatus::HVDCP:        return "HVDCP";
        case VBUSStatus::UNKNOWN:      return "Unknown Adapter";
        case VBUSStatus::NON_STANDARD: return "Non-Standard Adapter";
        case VBUSStatus::OTG:          return "OTG Mode";
        default:                       return "Unknown";
    }
}

const char* BQ25890::getChargeStatusString() {
    switch (getChargeStatus()) {
        case ChargeStatus::NOT_CHARGING: return "Not Charging";
        case ChargeStatus::PRE_CHARGE:   return "Pre-Charge";
        case ChargeStatus::FAST_CHARGE:  return "Fast Charging";
        case ChargeStatus::CHARGE_DONE:  return "Charge Done";
        default:                         return "Unknown";
    }
}

// ============================================================================
// Fault Reading (REG0C) - Interrupt Support
// ============================================================================

uint8_t BQ25890::readFaults() {
    _lastFaults = readRegister(BQ25890_REG0C);
    return _lastFaults;
}

bool BQ25890::isWatchdogFault() {
    return (_lastFaults & BQ25890_REG0C_WATCHDOG_FAULT_MASK) != 0;
}

bool BQ25890::isBoostFault() {
    return (_lastFaults & BQ25890_REG0C_BOOST_FAULT_MASK) != 0;
}

ChargeFault BQ25890::getChargeFault() {
    return static_cast<ChargeFault>((_lastFaults & BQ25890_REG0C_CHRG_FAULT_MASK) >> BQ25890_REG0C_CHRG_FAULT_SHIFT);
}

bool BQ25890::isBatteryFault() {
    return (_lastFaults & BQ25890_REG0C_BAT_FAULT_MASK) != 0;
}

NTCFault BQ25890::getNTCFault() {
    return static_cast<NTCFault>((_lastFaults & BQ25890_REG0C_NTC_FAULT_MASK) >> BQ25890_REG0C_NTC_FAULT_SHIFT);
}

const char* BQ25890::getChargeFaultString() {
    switch (getChargeFault()) {
        case ChargeFault::NORMAL:           return "Normal";
        case ChargeFault::INPUT_FAULT:      return "Input Fault (VBUS OVP)";
        case ChargeFault::THERMAL_SHUTDOWN: return "Thermal Shutdown";
        case ChargeFault::TIMER_EXPIRED:    return "Charge Timer Expired";
        default:                            return "Unknown";
    }
}

const char* BQ25890::getNTCFaultString() {
    switch (getNTCFault()) {
        case NTCFault::NORMAL: return "Normal";
        case NTCFault::WARM:   return "Warm";
        case NTCFault::COOL:   return "Cool";
        case NTCFault::COLD:   return "Cold";
        case NTCFault::HOT:    return "Hot";
        default:               return "Unknown";
    }
}

// ============================================================================
// VINDPM Settings (REG0D)
// ============================================================================

void BQ25890::setForceVINDPM(bool force) {
    writeBits(BQ25890_REG0D, BQ25890_REG0D_FORCE_VINDPM_MASK, BQ25890_REG0D_FORCE_VINDPM_SHIFT, force ? 1 : 0);
}

bool BQ25890::getForceVINDPM() {
    return readBits(BQ25890_REG0D, BQ25890_REG0D_FORCE_VINDPM_MASK, BQ25890_REG0D_FORCE_VINDPM_SHIFT) != 0;
}

void BQ25890::setVINDPMThreshold(uint16_t mV) {
    // Range: 3900-15300mV (with 2600mV base + 100mV per bit)
    if (mV < 3900) mV = 3900;
    if (mV > 15300) mV = 15300;
    uint8_t value = (mV - BQ25890_REG0D_VINDPM_BASE) / BQ25890_REG0D_VINDPM_LSB;
    if (value > 0x7F) value = 0x7F;
    writeBits(BQ25890_REG0D, BQ25890_REG0D_VINDPM_MASK, BQ25890_REG0D_VINDPM_SHIFT, value);
}

uint16_t BQ25890::getVINDPMThreshold() {
    uint8_t value = readBits(BQ25890_REG0D, BQ25890_REG0D_VINDPM_MASK, BQ25890_REG0D_VINDPM_SHIFT);
    return BQ25890_REG0D_VINDPM_BASE + (value * BQ25890_REG0D_VINDPM_LSB);
}

// ============================================================================
// ADC Readings (REG0E-REG12)
// ============================================================================

bool BQ25890::isThermalRegulation() {
    return readBits(BQ25890_REG0E, BQ25890_REG0E_THERM_STAT_MASK, BQ25890_REG0E_THERM_STAT_SHIFT) != 0;
}

uint16_t BQ25890::getBatteryVoltage() {
    uint8_t value = readBits(BQ25890_REG0E, BQ25890_REG0E_BATV_MASK, BQ25890_REG0E_BATV_SHIFT);
    return BQ25890_REG0E_BATV_BASE + (value * BQ25890_REG0E_BATV_LSB);
}

uint16_t BQ25890::getSystemVoltage() {
    uint8_t value = readBits(BQ25890_REG0F, BQ25890_REG0F_SYSV_MASK, BQ25890_REG0F_SYSV_SHIFT);
    return BQ25890_REG0F_SYSV_BASE + (value * BQ25890_REG0F_SYSV_LSB);
}

float BQ25890::getTSPercent() {
    uint8_t value = readBits(BQ25890_REG10, BQ25890_REG10_TSPCT_MASK, BQ25890_REG10_TSPCT_SHIFT);
    return BQ25890_REG10_TSPCT_BASE + (value * BQ25890_REG10_TSPCT_LSB);
}

bool BQ25890::isVBUSGood() {
    return readBits(BQ25890_REG11, BQ25890_REG11_VBUS_GD_MASK, BQ25890_REG11_VBUS_GD_SHIFT) != 0;
}

uint16_t BQ25890::getVBUSVoltage() {
    uint8_t value = readBits(BQ25890_REG11, BQ25890_REG11_VBUSV_MASK, BQ25890_REG11_VBUSV_SHIFT);
    return BQ25890_REG11_VBUSV_BASE + (value * BQ25890_REG11_VBUSV_LSB);
}

uint16_t BQ25890::getChargeCurrent() {
    uint8_t value = readBits(BQ25890_REG12, BQ25890_REG12_ICHGR_MASK, BQ25890_REG12_ICHGR_SHIFT);
    return BQ25890_REG12_ICHGR_BASE + (value * BQ25890_REG12_ICHGR_LSB);
}

// ============================================================================
// DPM Status and Limit (REG13)
// ============================================================================

bool BQ25890::isVINDPMActive() {
    return readBits(BQ25890_REG13, BQ25890_REG13_VDPM_STAT_MASK, BQ25890_REG13_VDPM_STAT_SHIFT) != 0;
}

bool BQ25890::isIINDPMActive() {
    return readBits(BQ25890_REG13, BQ25890_REG13_IDPM_STAT_MASK, BQ25890_REG13_IDPM_STAT_SHIFT) != 0;
}

uint16_t BQ25890::getIDPMLimit() {
    uint8_t value = readBits(BQ25890_REG13, BQ25890_REG13_IDPM_LIM_MASK, BQ25890_REG13_IDPM_LIM_SHIFT);
    return BQ25890_REG13_IDPM_LIM_BASE + (value * BQ25890_REG13_IDPM_LIM_LSB);
}

// ============================================================================
// Device Information (REG14)
// ============================================================================

void BQ25890::resetRegisters() {
    writeBits(BQ25890_REG14, BQ25890_REG14_REG_RST_MASK, BQ25890_REG14_REG_RST_SHIFT, 1);
    delay(10); // Wait for reset to complete
}

bool BQ25890::isICOOptimized() {
    return readBits(BQ25890_REG14, BQ25890_REG14_ICO_OPTIMIZED_MASK, BQ25890_REG14_ICO_OPTIMIZED_SHIFT) != 0;
}

uint8_t BQ25890::getPartNumber() {
    return readBits(BQ25890_REG14, BQ25890_REG14_PN_MASK, BQ25890_REG14_PN_SHIFT);
}

bool BQ25890::getTSProfile() {
    return readBits(BQ25890_REG14, BQ25890_REG14_TS_PROFILE_MASK, BQ25890_REG14_TS_PROFILE_SHIFT) != 0;
}

uint8_t BQ25890::getRevision() {
    return readBits(BQ25890_REG14, BQ25890_REG14_DEV_REV_MASK, BQ25890_REG14_DEV_REV_SHIFT);
}

// ============================================================================
// Private I2C Helper Functions
// ============================================================================

uint8_t BQ25890::readRegister(uint8_t reg) {
    if (_wire == nullptr) return 0;

    _wire->beginTransmission(BQ25890_I2C_ADDRESS);
    _wire->write(reg);
    if (_wire->endTransmission(false) != 0) {
        return 0;
    }

    _wire->requestFrom((uint8_t)BQ25890_I2C_ADDRESS, (uint8_t)1);
    if (_wire->available()) {
        return _wire->read();
    }
    return 0;
}

bool BQ25890::writeRegister(uint8_t reg, uint8_t value) {
    if (_wire == nullptr) return false;

    _wire->beginTransmission(BQ25890_I2C_ADDRESS);
    _wire->write(reg);
    _wire->write(value);
    return (_wire->endTransmission() == 0);
}

uint8_t BQ25890::readBits(uint8_t reg, uint8_t mask, uint8_t shift) {
    uint8_t value = readRegister(reg);
    return (value & mask) >> shift;
}

bool BQ25890::writeBits(uint8_t reg, uint8_t mask, uint8_t shift, uint8_t value) {
    uint8_t regValue = readRegister(reg);
    regValue = (regValue & ~mask) | ((value << shift) & mask);
    return writeRegister(reg, regValue);
}
