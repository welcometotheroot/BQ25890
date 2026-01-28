#ifndef BQ25890_H
#define BQ25890_H

#include <Arduino.h>
#include <Wire.h>
#include "BQ25890_Regs.h"

/// VBUS input source type detected by D+/D- or HVDCP handshake.
enum class VBUSStatus : uint8_t {
    NO_INPUT = 0b000,
    USB_SDP = 0b001,       ///< USB Host SDP (500mA)
    USB_CDP = 0b010,       ///< USB CDP (1.5A)
    USB_DCP = 0b011,       ///< USB DCP (3.25A)
    HVDCP = 0b100,         ///< Adjustable High Voltage DCP (1.5A)
    UNKNOWN = 0b101,       ///< Unknown Adapter (500mA)
    NON_STANDARD = 0b110,  ///< Non-Standard Adapter
    OTG = 0b111            ///< OTG (boost) Mode
};

/// Battery charging state machine status.
enum class ChargeStatus : uint8_t {
    NOT_CHARGING = 0b00,
    PRE_CHARGE = 0b01,     ///< Battery below BATLOWV threshold
    FAST_CHARGE = 0b10,    ///< Constant current or constant voltage phase
    CHARGE_DONE = 0b11     ///< Termination current reached
};

/// Charge fault type from the fault register (REG0C).
enum class ChargeFault : uint8_t {
    NORMAL = 0b00,
    INPUT_FAULT = 0b01,    ///< VBUS OVP or bad adapter
    THERMAL_SHUTDOWN = 0b10,
    TIMER_EXPIRED = 0b11
};

/// NTC thermistor fault status from the fault register (REG0C).
enum class NTCFault : uint8_t {
    NORMAL = 0b000,
    WARM = 0b010,
    COOL = 0b011,
    COLD = 0b101,
    HOT = 0b110
};

/// I2C watchdog timer period. Expiry resets charger registers to defaults.
enum class WatchdogTimer : uint8_t {
    DISABLED = 0b00,
    TIMER_40S = 0b01,
    TIMER_80S = 0b10,
    TIMER_160S = 0b11
};

/// Maximum duration for the fast-charge phase before a safety timeout fault.
enum class FastChargeTimer : uint8_t {
    TIMER_5H = 0b00,
    TIMER_8H = 0b01,
    TIMER_12H = 0b10,
    TIMER_20H = 0b11
};

/// IC junction temperature at which charge current is reduced.
enum class ThermalRegulation : uint8_t {
    TREG_60C = 0b00,
    TREG_80C = 0b01,
    TREG_100C = 0b10,
    TREG_120C = 0b11
};

/// Maximum current the boost (OTG) converter will deliver.
enum class BoostCurrentLimit : uint8_t {
    LIMIT_500MA = 0b000,
    LIMIT_750MA = 0b001,
    LIMIT_1200MA = 0b010,
    LIMIT_1400MA = 0b011,
    LIMIT_1650MA = 0b100,
    LIMIT_1875MA = 0b101,
    LIMIT_2150MA = 0b110,
    LIMIT_2450MA = 0b111
};

/// Arduino driver for the Texas Instruments BQ25890 I2C battery charger IC.
///
/// Provides register-level access through human-readable functions using
/// millivolt and milliamp units. Supports charging configuration, ADC
/// readings, fault monitoring, OTG boost mode, and ship mode.
///
/// I2C address: 0x6A (fixed).
class BQ25890 {
public:
    BQ25890();

    // =========================================================================
    // Initialization and Core I2C
    // =========================================================================

    /// Initialize the driver and verify the device is present.
    /// @param wire  TwoWire instance to use (default: Wire)
    /// @return true if device is found and part number matches BQ25890
    bool begin(TwoWire &wire = Wire);

    /// Check if the device responds on the I2C bus.
    /// @return true if ACK received at address 0x6A
    bool isConnected();

    /// Read the part number from REG14. Alias for getPartNumber().
    /// @return Part number field (0x03 for BQ25890)
    uint8_t getDeviceID();

    /// Read the device silicon revision from REG14. Alias for getRevision().
    /// @return Revision number
    uint8_t getDeviceRevision();

    /// Perform a software reset of all registers to power-on defaults.
    void reset();

    // =========================================================================
    // Input Source Control (REG00)
    // =========================================================================

    /// Enable or disable High-Impedance (HIZ) mode.
    /// When enabled, the device disconnects from VBUS and draws no input
    /// current. The system runs entirely from the battery.
    /// @param enable  true to enter HIZ mode
    void setHIZMode(bool enable);

    /// @return true if HIZ mode is active
    bool getHIZMode();

    /// Enable or disable the external ILIM pin current limit function.
    /// When enabled, the input current limit is the lower of the ILIM pin
    /// setting and the I2C register setting.
    /// @param enable  true to enable ILIM pin
    void setILIMPinEnabled(bool enable);

    /// @return true if the ILIM pin function is enabled
    bool getILIMPinEnabled();

    /// Set the input current limit.
    /// @param mA  Current limit in milliamps (100-3250, 50mA steps).
    ///            Values are clamped to the valid range.
    void setInputCurrentLimit(uint16_t mA);

    /// @return Current input current limit setting in milliamps
    uint16_t getInputCurrentLimit();

    // =========================================================================
    // Boost Mode Temperature Thresholds (REG01)
    // =========================================================================

    /// Set the boost mode hot temperature threshold.
    /// @param threshold  0=VBHOT1 (34.75%), 1=VBHOT0 (37.75%),
    ///                   2=VBHOT2 (31.25%), 3=Disable
    void setBoostHotThreshold(uint8_t threshold);

    /// @return Current boost hot threshold setting (0-3)
    uint8_t getBoostHotThreshold();

    /// Set the boost mode cold temperature threshold.
    /// @param threshold  false=77% of REGN, true=80% of REGN
    void setBoostColdThreshold(bool threshold);

    /// @return Current boost cold threshold (false=77%, true=80%)
    bool getBoostColdThreshold();

    /// Set the VINDPM offset used in relative VINDPM mode.
    /// The actual VINDPM threshold = VBUS at power-on + offset.
    /// @param mV  Offset in millivolts (0-3100, 100mV steps)
    void setVINDPMOffset(uint16_t mV);

    /// @return Current VINDPM offset setting in millivolts
    uint16_t getVINDPMOffset();

    // =========================================================================
    // ADC Control (REG02)
    // =========================================================================

    /// Start a one-shot ADC conversion and block until complete (up to 1s).
    /// After this call returns, ADC readings (battery voltage, VBUS, etc.)
    /// are available via the getter functions.
    void startADCConversion();

    /// Start a one-shot ADC conversion without blocking.
    /// Poll isADCConversionComplete() to know when results are ready.
    /// @return true (always succeeds)
    bool startADCConversionAsync();

    /// Check if the ADC conversion started by startADCConversionAsync() has
    /// finished. The CONV_START bit self-clears when conversion is done.
    /// @return true if conversion is complete (or was never started)
    bool isADCConversionComplete();

    /// Enable or disable continuous ADC conversion mode.
    /// In continuous mode the ADC re-samples automatically, keeping readings
    /// always fresh without needing to trigger conversions.
    /// @param enable  true for continuous, false for one-shot
    void setADCContinuousMode(bool enable);

    /// @return true if continuous ADC mode is enabled
    bool getADCContinuousMode();

    /// Set the boost converter switching frequency.
    /// @param freq500kHz  false=1.5MHz (default), true=500kHz
    void setBoostFrequency(bool freq500kHz);

    /// Enable or disable the Input Current Optimizer (ICO).
    /// ICO automatically finds the maximum input current the source can
    /// deliver without collapsing its voltage.
    /// @param enable  true to enable ICO
    void setICOEnabled(bool enable);

    /// @return true if ICO is enabled
    bool getICOEnabled();

    /// Enable or disable High Voltage DCP (HVDCP) handshake on D+/D-.
    /// @param enable  true to enable HVDCP detection
    void setHVDCPEnabled(bool enable);

    /// Enable or disable MaxCharge handshake on D+/D-.
    /// @param enable  true to enable MaxCharge detection
    void setMaxChargeEnabled(bool enable);

    /// Force a D+/D- detection cycle to re-identify the input source type.
    /// The FORCE_DPDM bit self-clears when detection completes.
    void forceDPDMDetection();

    /// Enable or disable automatic D+/D- detection on VBUS insertion.
    /// @param enable  true to enable auto detection
    void setAutoDPDMEnabled(bool enable);

    // =========================================================================
    // Charge and OTG Configuration (REG03)
    // =========================================================================

    /// Enable or disable the battery load (IBATLOAD) for battery presence
    /// detection.
    /// @param enable  true to enable battery load
    void setBatteryLoadEnabled(bool enable);

    /// Reset the I2C watchdog timer to prevent a watchdog fault.
    /// Must be called periodically if the watchdog is enabled.
    void resetWatchdog();

    /// Enable or disable OTG (boost) mode. When enabled, the device outputs
    /// a regulated voltage on VBUS from the battery (reverse power path).
    /// Charging must be disabled before enabling OTG.
    /// @param enable  true to enable OTG boost mode
    void setOTGEnabled(bool enable);

    /// @return true if OTG boost mode is enabled
    bool getOTGEnabled();

    /// Enable or disable battery charging.
    /// @param enable  true to enable charging
    void setChargeEnabled(bool enable);

    /// @return true if charging is enabled
    bool getChargeEnabled();

    /// Set the minimum system voltage. If the battery voltage is below this
    /// threshold, the charger reduces charge current to prioritise the system
    /// load.
    /// @param mV  Minimum voltage in millivolts (3000-3700, 100mV steps)
    void setMinSystemVoltage(uint16_t mV);

    /// @return Current minimum system voltage setting in millivolts
    uint16_t getMinSystemVoltage();

    // =========================================================================
    // Current Pulse Control and Charge Current (REG04)
    // =========================================================================

    /// Enable or disable the current pulse control (PUMPX) protocol for
    /// voltage negotiation with HVDCP adapters.
    /// @param enable  true to enable current pulse control
    void setCurrentPulseControlEnabled(bool enable);

    /// @return true if current pulse control is enabled
    bool getCurrentPulseControlEnabled();

    /// Set the fast-charge constant-current limit.
    /// @param mA  Charge current in milliamps (0-5056, 64mA steps)
    void setFastChargeCurrent(uint16_t mA);

    /// @return Current fast-charge current setting in milliamps
    uint16_t getFastChargeCurrent();

    // =========================================================================
    // Precharge and Termination Current (REG05)
    // =========================================================================

    /// Set the precharge current limit. Precharge is used when the battery
    /// voltage is below the BATLOWV threshold.
    /// @param mA  Current in milliamps (64-1024, 64mA steps)
    void setPrechargeCurrent(uint16_t mA);

    /// @return Current precharge current setting in milliamps
    uint16_t getPrechargeCurrent();

    /// Set the charge termination current. Charging ends when the charge
    /// current drops below this threshold during the CV phase.
    /// @param mA  Current in milliamps (64-1024, 64mA steps)
    void setTerminationCurrent(uint16_t mA);

    /// @return Current termination current setting in milliamps
    uint16_t getTerminationCurrent();

    // =========================================================================
    // Charge Voltage Settings (REG06)
    // =========================================================================

    /// Set the charge voltage regulation limit (VREG). This is the target
    /// battery voltage during the constant-voltage phase.
    /// @param mV  Voltage in millivolts (3840-4608, 16mV steps)
    void setChargeVoltage(uint16_t mV);

    /// @return Current charge voltage setting in millivolts
    uint16_t getChargeVoltage();

    /// Set the battery low-voltage (precharge-to-fast-charge) threshold.
    /// @param use3V  false=2.8V, true=3.0V
    void setBatteryLowVThreshold(bool use3V);

    /// @return false=2.8V threshold, true=3.0V threshold
    bool getBatteryLowVThreshold();

    /// Set the recharge threshold below VREG. A new charge cycle begins
    /// when the battery drops below VREG minus this offset.
    /// @param use200mV  false=100mV below VREG, true=200mV below VREG
    void setRechargeThreshold(bool use200mV);

    /// @return false=100mV offset, true=200mV offset
    bool getRechargeThreshold();

    // =========================================================================
    // Termination and Timer Settings (REG07)
    // =========================================================================

    /// Enable or disable charge termination. When disabled, charging
    /// continues indefinitely in CV mode without terminating.
    /// @param enable  true to enable termination detection
    void setTerminationEnabled(bool enable);

    /// @return true if charge termination is enabled
    bool getTerminationEnabled();

    /// Disable the STAT pin output. When disabled, the STAT pin is
    /// high-impedance regardless of charger status.
    /// @param disable  true to disable STAT pin, false to enable
    void setSTATPinDisabled(bool disable);

    /// @return true if the STAT pin is disabled
    bool getSTATPinDisabled();

    /// Set the I2C watchdog timer period. If the watchdog expires without
    /// being reset, all registers revert to defaults.
    /// @param setting  Watchdog period (DISABLED, 40s, 80s, or 160s)
    void setWatchdogTimer(WatchdogTimer setting);

    /// @return Current watchdog timer setting
    WatchdogTimer getWatchdogTimer();

    /// Enable or disable the charging safety timer. A fault is raised if
    /// fast-charge is not completed within the timer period.
    /// @param enable  true to enable the safety timer
    void setSafetyTimerEnabled(bool enable);

    /// @return true if the safety timer is enabled
    bool getSafetyTimerEnabled();

    /// Set the fast-charge safety timer duration.
    /// @param setting  Timer duration (5h, 8h, 12h, or 20h)
    void setFastChargeTimer(FastChargeTimer setting);

    /// @return Current fast-charge timer setting
    FastChargeTimer getFastChargeTimer();

    /// Set the JEITA low-temperature charging current as a fraction of
    /// the fast-charge current (ICHG).
    /// @param use20pct  false=50% of ICHG, true=20% of ICHG
    void setJEITALowTempCurrent(bool use20pct);

    // =========================================================================
    // IR Compensation and Thermal Regulation (REG08)
    // =========================================================================

    /// Set the IR compensation resistance. The charger adds this voltage
    /// (ICHG * R) to the charge voltage to compensate for PCB trace and
    /// connector resistance between the IC and battery.
    /// @param mohm  Resistance in milliohms (0-140, 20mOhm steps)
    void setIRCompResistance(uint8_t mohm);

    /// @return Current IR compensation resistance in milliohms
    uint8_t getIRCompResistance();

    /// Set the IR compensation voltage clamp. Limits the maximum voltage
    /// added by IR compensation to prevent overcharging.
    /// @param mV  Clamp voltage in millivolts (0-224, 32mV steps)
    void setIRCompVoltageClamp(uint16_t mV);

    /// @return Current IR compensation voltage clamp in millivolts
    uint16_t getIRCompVoltageClamp();

    /// Set the thermal regulation threshold. The charger reduces charge
    /// current when the IC junction temperature exceeds this value.
    /// @param setting  Temperature threshold (60, 80, 100, or 120 deg C)
    void setThermalRegThreshold(ThermalRegulation setting);

    /// @return Current thermal regulation threshold setting
    ThermalRegulation getThermalRegThreshold();

    // =========================================================================
    // BATFET and Current Pulse Control (REG09)
    // =========================================================================

    /// Force-start the Input Current Optimizer (ICO) algorithm.
    /// The FORCE_ICO bit self-clears when optimisation completes.
    void forceICO();

    /// Enable or disable 2x safety timer slowdown during input DPM
    /// (dynamic power management) or thermal regulation.
    /// @param enable  true to double the safety timer during DPM
    void setSafetyTimer2xEnabled(bool enable);

    /// Disconnect the battery by disabling the internal BATFET (ship mode).
    /// The system will lose power unless VBUS is present.
    /// @param disable  true to turn off BATFET (enter ship mode)
    void setBATFETDisabled(bool disable);

    /// @return true if BATFET is disabled (ship mode active)
    bool getBATFETDisabled();

    /// Set the JEITA high-temperature charge voltage.
    /// @param useVREG  false=VREG-200mV, true=VREG (full voltage)
    void setJEITAHighTempVoltage(bool useVREG);

    /// Enable a 10-second delay before BATFET turn-off. Gives the system
    /// time to save state before power is removed.
    /// @param useDelay  true to enable the 10s turn-off delay
    void setBATFETDelay(bool useDelay);

    /// Enable or disable BATFET full system reset. When enabled, a BATFET
    /// turn-off/turn-on cycle performs a full system reset.
    /// @param enable  true to enable reset on BATFET re-enable
    void setBATFETResetEnabled(bool enable);

    /// Send a current pulse to request a HVDCP voltage increment.
    /// Blocks until the pulse completes (up to 1s timeout).
    /// @return true if the pulse completed, false on timeout
    bool pumpVoltageUp();

    /// Send a current pulse to request a HVDCP voltage decrement.
    /// Blocks until the pulse completes (up to 1s timeout).
    /// @return true if the pulse completed, false on timeout
    bool pumpVoltageDown();

    // =========================================================================
    // Boost Mode Settings (REG0A)
    // =========================================================================

    /// Set the boost (OTG) output voltage.
    /// @param mV  Output voltage in millivolts (4550-5510, 64mV steps)
    void setBoostVoltage(uint16_t mV);

    /// @return Current boost output voltage setting in millivolts
    uint16_t getBoostVoltage();

    /// Set the boost mode output current limit.
    /// @param setting  Current limit (500mA to 2450mA)
    void setBoostCurrentLimit(BoostCurrentLimit setting);

    /// @return Current boost current limit setting
    BoostCurrentLimit getBoostCurrentLimit();

    // =========================================================================
    // Status Reading (REG0B)
    // =========================================================================

    /// Get the detected VBUS input source type.
    /// @return VBUSStatus enum indicating the source type
    VBUSStatus getVBUSStatus();

    /// Get the current charging state.
    /// @return ChargeStatus enum (not charging, pre-charge, fast, or done)
    ChargeStatus getChargeStatus();

    /// Check if the input power source is good (VBUS above VGOOD threshold).
    /// @return true if power is good
    bool isPowerGood();

    /// Check if the charger is in VSYSMIN regulation (battery too low to
    /// sustain the system load at the requested charge current).
    /// @return true if in VSYS regulation
    bool isVSYSRegulation();

    /// @return Human-readable string describing the current VBUS status
    const char* getVBUSStatusString();

    /// @return Human-readable string describing the current charge status
    const char* getChargeStatusString();

    // =========================================================================
    // Fault Reading (REG0C)
    // =========================================================================

    /// Read and latch the fault register. The first read returns latched
    /// faults; a second read returns the real-time state. Subsequent calls
    /// to isWatchdogFault(), getChargeFault(), etc. operate on this
    /// latched value.
    /// @return Raw fault register byte
    uint8_t readFaults();

    /// @return true if a watchdog timer fault is latched
    bool isWatchdogFault();

    /// @return true if a boost mode fault is latched
    bool isBoostFault();

    /// @return Charge fault type from the latched fault register
    ChargeFault getChargeFault();

    /// @return true if a battery OVP fault is latched
    bool isBatteryFault();

    /// @return NTC thermistor fault type from the latched fault register
    NTCFault getNTCFault();

    /// @return Human-readable string for the latched charge fault
    const char* getChargeFaultString();

    /// @return Human-readable string for the latched NTC fault
    const char* getNTCFaultString();

    // =========================================================================
    // VINDPM Settings (REG0D)
    // =========================================================================

    /// Select absolute or relative VINDPM mode.
    /// @param force  true=absolute threshold (from register), false=relative
    ///               (VBUS at power-on + offset from REG01)
    void setForceVINDPM(bool force);

    /// @return true if absolute VINDPM mode is selected
    bool getForceVINDPM();

    /// Set the absolute VINDPM threshold. The charger reduces input current
    /// when VBUS drops below this voltage.
    /// @param mV  Threshold in millivolts (3900-15300, 100mV steps)
    void setVINDPMThreshold(uint16_t mV);

    /// @return Current absolute VINDPM threshold in millivolts
    uint16_t getVINDPMThreshold();

    // =========================================================================
    // ADC Readings (REG0E-REG12)
    // =========================================================================

    /// Check if the IC is currently in thermal regulation (charge current
    /// reduced due to junction temperature).
    /// @return true if thermal regulation is active
    bool isThermalRegulation();

    /// @return Measured battery voltage in millivolts (2304mV base, 20mV LSB)
    uint16_t getBatteryVoltage();

    /// @return Measured system output voltage in millivolts
    uint16_t getSystemVoltage();

    /// @return TS pin voltage as a percentage of REGN (21% base, 0.465%/LSB)
    float getTSPercent();

    /// Check if VBUS is above the valid threshold.
    /// @return true if VBUS is good
    bool isVBUSGood();

    /// @return Measured VBUS voltage in millivolts (2600mV base, 100mV LSB)
    uint16_t getVBUSVoltage();

    /// @return Measured charge current in milliamps (0mA base, 50mA LSB)
    uint16_t getChargeCurrent();

    // =========================================================================
    // DPM Status and Limit (REG13)
    // =========================================================================

    /// Check if the charger is in VINDPM (input voltage-based dynamic power
    /// management), reducing charge current to maintain input voltage.
    /// @return true if VINDPM regulation is active
    bool isVINDPMActive();

    /// Check if the charger is in IINDPM (input current-based dynamic power
    /// management), limiting charge current to stay within the input current
    /// limit.
    /// @return true if IINDPM regulation is active
    bool isIINDPMActive();

    /// Read the actual input current limit currently in effect. This may
    /// differ from the configured limit if ICO has optimised it.
    /// @return Input current limit in milliamps
    uint16_t getIDPMLimit();

    // =========================================================================
    // Device Information (REG14)
    // =========================================================================

    /// Reset all registers to their power-on default values.
    /// The REG_RST bit self-clears after reset completes.
    void resetRegisters();

    /// Check if the Input Current Optimizer (ICO) has completed and found
    /// the maximum input current.
    /// @return true if ICO optimisation is complete
    bool isICOOptimized();

    /// Read the device part number from REG14.
    /// @return Part number (0x03 for BQ25890)
    uint8_t getPartNumber();

    /// Read the temperature sensing profile setting.
    /// @return true if JEITA profile is selected
    bool getTSProfile();

    /// Read the device silicon revision.
    /// @return Revision number
    uint8_t getRevision();

private:
    TwoWire* _wire;
    uint8_t _lastFaults;

    uint8_t readRegister(uint8_t reg);
    bool writeRegister(uint8_t reg, uint8_t value);
    uint8_t readBits(uint8_t reg, uint8_t mask, uint8_t shift);
    bool writeBits(uint8_t reg, uint8_t mask, uint8_t shift, uint8_t value);
};

#endif // BQ25890_H
