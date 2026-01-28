#ifndef BQ25890_REGS_H
#define BQ25890_REGS_H

#include <stdint.h>

// BQ25890 I2C Address
#define BQ25890_I2C_ADDRESS 0x6A

// Register Addresses
#define BQ25890_REG00 0x00  // Input Source Control
#define BQ25890_REG01 0x01  // Boost Temp Thresholds, VINDPM Offset
#define BQ25890_REG02 0x02  // ADC Control, ICO, D+/D- Detection
#define BQ25890_REG03 0x03  // Charge/OTG Config, Min System Voltage
#define BQ25890_REG04 0x04  // Current Pulse Control, Fast Charge Current
#define BQ25890_REG05 0x05  // Precharge/Termination Current
#define BQ25890_REG06 0x06  // Charge Voltage Limit
#define BQ25890_REG07 0x07  // Termination, Watchdog, Safety Timer
#define BQ25890_REG08 0x08  // IR Compensation, Thermal Regulation
#define BQ25890_REG09 0x09  // BATFET Control, JEITA, Current Pulse
#define BQ25890_REG0A 0x0A  // Boost Voltage/Current Settings
#define BQ25890_REG0B 0x0B  // Status Register (Read Only)
#define BQ25890_REG0C 0x0C  // Fault Register (Read Only)
#define BQ25890_REG0D 0x0D  // VINDPM Threshold
#define BQ25890_REG0E 0x0E  // ADC: Battery Voltage (Read Only)
#define BQ25890_REG0F 0x0F  // ADC: System Voltage (Read Only)
#define BQ25890_REG10 0x10  // ADC: TS Voltage (Read Only)
#define BQ25890_REG11 0x11  // ADC: VBUS Voltage (Read Only)
#define BQ25890_REG12 0x12  // ADC: Charge Current (Read Only)
#define BQ25890_REG13 0x13  // IDPM Status and Limit (Read Only)
#define BQ25890_REG14 0x14  // Device Info, Register Reset

// REG00 - Input Source Control
#define BQ25890_REG00_EN_HIZ_MASK       0x80  // Enable HIZ Mode
#define BQ25890_REG00_EN_HIZ_SHIFT      7
#define BQ25890_REG00_EN_ILIM_MASK      0x40  // Enable ILIM Pin
#define BQ25890_REG00_EN_ILIM_SHIFT     6
#define BQ25890_REG00_IINLIM_MASK       0x3F  // Input Current Limit (6 bits)
#define BQ25890_REG00_IINLIM_SHIFT      0
#define BQ25890_REG00_IINLIM_BASE       100   // 100mA base
#define BQ25890_REG00_IINLIM_LSB        50    // 50mA per LSB

// REG01 - Boost Temp Thresholds, VINDPM Offset
#define BQ25890_REG01_BHOT_MASK         0xC0  // Boost Hot Threshold (2 bits)
#define BQ25890_REG01_BHOT_SHIFT        6
#define BQ25890_REG01_BCOLD_MASK        0x20  // Boost Cold Threshold
#define BQ25890_REG01_BCOLD_SHIFT       5
#define BQ25890_REG01_VINDPM_OS_MASK    0x1F  // VINDPM Offset (5 bits)
#define BQ25890_REG01_VINDPM_OS_SHIFT   0
#define BQ25890_REG01_VINDPM_OS_BASE    0     // 0mV base
#define BQ25890_REG01_VINDPM_OS_LSB     100   // 100mV per LSB

// REG02 - ADC Control, ICO, D+/D- Detection
#define BQ25890_REG02_CONV_START_MASK   0x80  // Start ADC Conversion
#define BQ25890_REG02_CONV_START_SHIFT  7
#define BQ25890_REG02_CONV_RATE_MASK    0x40  // ADC Conversion Rate (0=one-shot, 1=continuous)
#define BQ25890_REG02_CONV_RATE_SHIFT   6
#define BQ25890_REG02_BOOST_FREQ_MASK   0x20  // Boost Frequency (0=1.5MHz, 1=500kHz)
#define BQ25890_REG02_BOOST_FREQ_SHIFT  5
#define BQ25890_REG02_ICO_EN_MASK       0x10  // Input Current Optimizer Enable
#define BQ25890_REG02_ICO_EN_SHIFT      4
#define BQ25890_REG02_HVDCP_EN_MASK     0x08  // HVDCP Handshake Enable
#define BQ25890_REG02_HVDCP_EN_SHIFT    3
#define BQ25890_REG02_MAXC_EN_MASK      0x04  // MaxCharge Handshake Enable
#define BQ25890_REG02_MAXC_EN_SHIFT     2
#define BQ25890_REG02_FORCE_DPDM_MASK   0x02  // Force D+/D- Detection
#define BQ25890_REG02_FORCE_DPDM_SHIFT  1
#define BQ25890_REG02_AUTO_DPDM_MASK    0x01  // Auto D+/D- Detection Enable
#define BQ25890_REG02_AUTO_DPDM_SHIFT   0

// REG03 - Charge/OTG Config, Min System Voltage
#define BQ25890_REG03_BAT_LOADEN_MASK   0x80  // Battery Load Enable
#define BQ25890_REG03_BAT_LOADEN_SHIFT  7
#define BQ25890_REG03_WD_RST_MASK       0x40  // Watchdog Reset
#define BQ25890_REG03_WD_RST_SHIFT      6
#define BQ25890_REG03_OTG_CONFIG_MASK   0x20  // OTG Enable
#define BQ25890_REG03_OTG_CONFIG_SHIFT  5
#define BQ25890_REG03_CHG_CONFIG_MASK   0x10  // Charge Enable
#define BQ25890_REG03_CHG_CONFIG_SHIFT  4
#define BQ25890_REG03_SYS_MIN_MASK      0x0E  // Min System Voltage (3 bits)
#define BQ25890_REG03_SYS_MIN_SHIFT     1
#define BQ25890_REG03_SYS_MIN_BASE      3000  // 3000mV base
#define BQ25890_REG03_SYS_MIN_LSB       100   // 100mV per LSB

// REG04 - Current Pulse Control, Fast Charge Current
#define BQ25890_REG04_EN_PUMPX_MASK     0x80  // Current Pulse Control Enable
#define BQ25890_REG04_EN_PUMPX_SHIFT    7
#define BQ25890_REG04_ICHG_MASK         0x7F  // Fast Charge Current Limit (7 bits)
#define BQ25890_REG04_ICHG_SHIFT        0
#define BQ25890_REG04_ICHG_BASE         0     // 0mA base
#define BQ25890_REG04_ICHG_LSB          64    // 64mA per LSB

// REG05 - Precharge/Termination Current
#define BQ25890_REG05_IPRECHG_MASK      0xF0  // Precharge Current Limit (4 bits)
#define BQ25890_REG05_IPRECHG_SHIFT     4
#define BQ25890_REG05_IPRECHG_BASE      64    // 64mA base
#define BQ25890_REG05_IPRECHG_LSB       64    // 64mA per LSB
#define BQ25890_REG05_ITERM_MASK        0x0F  // Termination Current Limit (4 bits)
#define BQ25890_REG05_ITERM_SHIFT       0
#define BQ25890_REG05_ITERM_BASE        64    // 64mA base
#define BQ25890_REG05_ITERM_LSB         64    // 64mA per LSB

// REG06 - Charge Voltage Limit
#define BQ25890_REG06_VREG_MASK         0xFC  // Charge Voltage Limit (6 bits)
#define BQ25890_REG06_VREG_SHIFT        2
#define BQ25890_REG06_VREG_BASE         3840  // 3840mV base
#define BQ25890_REG06_VREG_LSB          16    // 16mV per LSB
#define BQ25890_REG06_BATLOWV_MASK      0x02  // Battery Low Voltage Threshold
#define BQ25890_REG06_BATLOWV_SHIFT     1
#define BQ25890_REG06_VRECHG_MASK       0x01  // Recharge Threshold Offset
#define BQ25890_REG06_VRECHG_SHIFT      0

// REG07 - Termination, Watchdog, Safety Timer
#define BQ25890_REG07_EN_TERM_MASK      0x80  // Termination Enable
#define BQ25890_REG07_EN_TERM_SHIFT     7
#define BQ25890_REG07_STAT_DIS_MASK     0x40  // STAT Pin Disable
#define BQ25890_REG07_STAT_DIS_SHIFT    6
#define BQ25890_REG07_WATCHDOG_MASK     0x30  // Watchdog Timer Setting (2 bits)
#define BQ25890_REG07_WATCHDOG_SHIFT    4
#define BQ25890_REG07_EN_TIMER_MASK     0x08  // Charging Safety Timer Enable
#define BQ25890_REG07_EN_TIMER_SHIFT    3
#define BQ25890_REG07_CHG_TIMER_MASK    0x06  // Fast Charge Timer Setting (2 bits)
#define BQ25890_REG07_CHG_TIMER_SHIFT   1
#define BQ25890_REG07_JEITA_ISET_MASK   0x01  // JEITA Low Temp Current Setting
#define BQ25890_REG07_JEITA_ISET_SHIFT  0

// REG08 - IR Compensation, Thermal Regulation
#define BQ25890_REG08_BAT_COMP_MASK     0xE0  // IR Compensation Resistance (3 bits)
#define BQ25890_REG08_BAT_COMP_SHIFT    5
#define BQ25890_REG08_BAT_COMP_BASE     0     // 0mOhm base
#define BQ25890_REG08_BAT_COMP_LSB      20    // 20mOhm per LSB
#define BQ25890_REG08_VCLAMP_MASK       0x1C  // IR Comp Voltage Clamp (3 bits)
#define BQ25890_REG08_VCLAMP_SHIFT      2
#define BQ25890_REG08_VCLAMP_BASE       0     // 0mV base
#define BQ25890_REG08_VCLAMP_LSB        32    // 32mV per LSB
#define BQ25890_REG08_TREG_MASK         0x03  // Thermal Regulation Threshold (2 bits)
#define BQ25890_REG08_TREG_SHIFT        0

// REG09 - BATFET Control, JEITA, Current Pulse
#define BQ25890_REG09_FORCE_ICO_MASK    0x80  // Force Start ICO
#define BQ25890_REG09_FORCE_ICO_SHIFT   7
#define BQ25890_REG09_TMR2X_EN_MASK     0x40  // Safety Timer 2x Enable
#define BQ25890_REG09_TMR2X_EN_SHIFT    6
#define BQ25890_REG09_BATFET_DIS_MASK   0x20  // BATFET Disable
#define BQ25890_REG09_BATFET_DIS_SHIFT  5
#define BQ25890_REG09_JEITA_VSET_MASK   0x10  // JEITA High Temp Voltage Setting
#define BQ25890_REG09_JEITA_VSET_SHIFT  4
#define BQ25890_REG09_BATFET_DLY_MASK   0x08  // BATFET Turn-Off Delay
#define BQ25890_REG09_BATFET_DLY_SHIFT  3
#define BQ25890_REG09_BATFET_RST_MASK   0x04  // BATFET Full System Reset Enable
#define BQ25890_REG09_BATFET_RST_SHIFT  2
#define BQ25890_REG09_PUMPX_UP_MASK     0x02  // Current Pulse Voltage Up
#define BQ25890_REG09_PUMPX_UP_SHIFT    1
#define BQ25890_REG09_PUMPX_DN_MASK     0x01  // Current Pulse Voltage Down
#define BQ25890_REG09_PUMPX_DN_SHIFT    0

// REG0A - Boost Voltage/Current Settings
#define BQ25890_REG0A_BOOSTV_MASK       0xF0  // Boost Voltage (4 bits)
#define BQ25890_REG0A_BOOSTV_SHIFT      4
#define BQ25890_REG0A_BOOSTV_BASE       4550  // 4550mV base
#define BQ25890_REG0A_BOOSTV_LSB        64    // 64mV per LSB
#define BQ25890_REG0A_BOOST_LIM_MASK    0x07  // Boost Current Limit (3 bits)
#define BQ25890_REG0A_BOOST_LIM_SHIFT   0

// REG0B - Status Register (Read Only)
#define BQ25890_REG0B_VBUS_STAT_MASK    0xE0  // VBUS Status (3 bits)
#define BQ25890_REG0B_VBUS_STAT_SHIFT   5
#define BQ25890_REG0B_CHRG_STAT_MASK    0x18  // Charging Status (2 bits)
#define BQ25890_REG0B_CHRG_STAT_SHIFT   3
#define BQ25890_REG0B_PG_STAT_MASK      0x04  // Power Good Status
#define BQ25890_REG0B_PG_STAT_SHIFT     2
#define BQ25890_REG0B_SDP_STAT_MASK     0x02  // SDP Status (Reserved on BQ25890)
#define BQ25890_REG0B_SDP_STAT_SHIFT    1
#define BQ25890_REG0B_VSYS_STAT_MASK    0x01  // VSYSMIN Regulation Status
#define BQ25890_REG0B_VSYS_STAT_SHIFT   0

// REG0C - Fault Register (Read Only)
#define BQ25890_REG0C_WATCHDOG_FAULT_MASK   0x80  // Watchdog Fault
#define BQ25890_REG0C_WATCHDOG_FAULT_SHIFT  7
#define BQ25890_REG0C_BOOST_FAULT_MASK      0x40  // Boost Fault
#define BQ25890_REG0C_BOOST_FAULT_SHIFT     6
#define BQ25890_REG0C_CHRG_FAULT_MASK       0x30  // Charge Fault (2 bits)
#define BQ25890_REG0C_CHRG_FAULT_SHIFT      4
#define BQ25890_REG0C_BAT_FAULT_MASK        0x08  // Battery Fault
#define BQ25890_REG0C_BAT_FAULT_SHIFT       3
#define BQ25890_REG0C_NTC_FAULT_MASK        0x07  // NTC Fault (3 bits)
#define BQ25890_REG0C_NTC_FAULT_SHIFT       0

// REG0D - VINDPM Threshold
#define BQ25890_REG0D_FORCE_VINDPM_MASK     0x80  // Force VINDPM Mode
#define BQ25890_REG0D_FORCE_VINDPM_SHIFT    7
#define BQ25890_REG0D_VINDPM_MASK           0x7F  // VINDPM Threshold (7 bits)
#define BQ25890_REG0D_VINDPM_SHIFT          0
#define BQ25890_REG0D_VINDPM_BASE           2600  // 2600mV base
#define BQ25890_REG0D_VINDPM_LSB            100   // 100mV per LSB

// REG0E - ADC Battery Voltage (Read Only)
#define BQ25890_REG0E_THERM_STAT_MASK   0x80  // Thermal Regulation Status
#define BQ25890_REG0E_THERM_STAT_SHIFT  7
#define BQ25890_REG0E_BATV_MASK         0x7F  // Battery Voltage (7 bits)
#define BQ25890_REG0E_BATV_SHIFT        0
#define BQ25890_REG0E_BATV_BASE         2304  // 2304mV base
#define BQ25890_REG0E_BATV_LSB          20    // 20mV per LSB

// REG0F - ADC System Voltage (Read Only)
#define BQ25890_REG0F_SYSV_MASK         0x7F  // System Voltage (7 bits)
#define BQ25890_REG0F_SYSV_SHIFT        0
#define BQ25890_REG0F_SYSV_BASE         2304  // 2304mV base
#define BQ25890_REG0F_SYSV_LSB          20    // 20mV per LSB

// REG10 - ADC TS Voltage (Read Only)
#define BQ25890_REG10_TSPCT_MASK        0x7F  // TS Voltage (7 bits)
#define BQ25890_REG10_TSPCT_SHIFT       0
#define BQ25890_REG10_TSPCT_BASE        21    // 21% base (of REGN)
#define BQ25890_REG10_TSPCT_LSB         0.465f // 0.465% per LSB

// REG11 - ADC VBUS Voltage (Read Only)
#define BQ25890_REG11_VBUS_GD_MASK      0x80  // VBUS Good Status
#define BQ25890_REG11_VBUS_GD_SHIFT     7
#define BQ25890_REG11_VBUSV_MASK        0x7F  // VBUS Voltage (7 bits)
#define BQ25890_REG11_VBUSV_SHIFT       0
#define BQ25890_REG11_VBUSV_BASE        2600  // 2600mV base
#define BQ25890_REG11_VBUSV_LSB         100   // 100mV per LSB

// REG12 - ADC Charge Current (Read Only)
#define BQ25890_REG12_ICHGR_MASK        0x7F  // Charge Current (7 bits)
#define BQ25890_REG12_ICHGR_SHIFT       0
#define BQ25890_REG12_ICHGR_BASE        0     // 0mA base
#define BQ25890_REG12_ICHGR_LSB         50    // 50mA per LSB

// REG13 - IDPM Status and Limit (Read Only)
#define BQ25890_REG13_VDPM_STAT_MASK    0x80  // VINDPM Status
#define BQ25890_REG13_VDPM_STAT_SHIFT   7
#define BQ25890_REG13_IDPM_STAT_MASK    0x40  // IINDPM Status
#define BQ25890_REG13_IDPM_STAT_SHIFT   6
#define BQ25890_REG13_IDPM_LIM_MASK     0x3F  // Input Current Limit in Effect (6 bits)
#define BQ25890_REG13_IDPM_LIM_SHIFT    0
#define BQ25890_REG13_IDPM_LIM_BASE     100   // 100mA base
#define BQ25890_REG13_IDPM_LIM_LSB      50    // 50mA per LSB

// REG14 - Device Info, Register Reset
#define BQ25890_REG14_REG_RST_MASK      0x80  // Register Reset
#define BQ25890_REG14_REG_RST_SHIFT     7
#define BQ25890_REG14_ICO_OPTIMIZED_MASK    0x40  // ICO Optimized Status
#define BQ25890_REG14_ICO_OPTIMIZED_SHIFT   6
#define BQ25890_REG14_PN_MASK           0x38  // Part Number (3 bits)
#define BQ25890_REG14_PN_SHIFT          3
#define BQ25890_REG14_TS_PROFILE_MASK   0x04  // Temperature Profile
#define BQ25890_REG14_TS_PROFILE_SHIFT  2
#define BQ25890_REG14_DEV_REV_MASK      0x03  // Device Revision (2 bits)
#define BQ25890_REG14_DEV_REV_SHIFT     0

// Part Number Values
#define BQ25890_PART_NUMBER             0x03  // BQ25890 Part Number

// VBUS Status Values
#define BQ25890_VBUS_NO_INPUT           0x00
#define BQ25890_VBUS_USB_SDP            0x01
#define BQ25890_VBUS_USB_CDP            0x02
#define BQ25890_VBUS_USB_DCP            0x03
#define BQ25890_VBUS_HVDCP              0x04
#define BQ25890_VBUS_UNKNOWN            0x05
#define BQ25890_VBUS_NON_STANDARD       0x06
#define BQ25890_VBUS_OTG                0x07

// Charge Status Values
#define BQ25890_CHG_STAT_NOT_CHARGING   0x00
#define BQ25890_CHG_STAT_PRE_CHARGE     0x01
#define BQ25890_CHG_STAT_FAST_CHARGE    0x02
#define BQ25890_CHG_STAT_DONE           0x03

// Charge Fault Values
#define BQ25890_CHG_FAULT_NORMAL        0x00
#define BQ25890_CHG_FAULT_INPUT         0x01
#define BQ25890_CHG_FAULT_THERMAL       0x02
#define BQ25890_CHG_FAULT_TIMER         0x03

// NTC Fault Values
#define BQ25890_NTC_FAULT_NORMAL        0x00
#define BQ25890_NTC_FAULT_WARM          0x02
#define BQ25890_NTC_FAULT_COOL          0x03
#define BQ25890_NTC_FAULT_COLD          0x05
#define BQ25890_NTC_FAULT_HOT           0x06

// Watchdog Timer Values
#define BQ25890_WATCHDOG_DISABLED       0x00
#define BQ25890_WATCHDOG_40S            0x01
#define BQ25890_WATCHDOG_80S            0x02
#define BQ25890_WATCHDOG_160S           0x03

// Fast Charge Timer Values
#define BQ25890_CHG_TIMER_5H            0x00
#define BQ25890_CHG_TIMER_8H            0x01
#define BQ25890_CHG_TIMER_12H           0x02
#define BQ25890_CHG_TIMER_20H           0x03

// Thermal Regulation Threshold Values
#define BQ25890_TREG_60C                0x00
#define BQ25890_TREG_80C                0x01
#define BQ25890_TREG_100C               0x02
#define BQ25890_TREG_120C               0x03

// Boost Current Limit Values (mA)
#define BQ25890_BOOST_LIM_500MA         0x00
#define BQ25890_BOOST_LIM_750MA         0x01
#define BQ25890_BOOST_LIM_1200MA        0x02
#define BQ25890_BOOST_LIM_1400MA        0x03
#define BQ25890_BOOST_LIM_1650MA        0x04
#define BQ25890_BOOST_LIM_1875MA        0x05
#define BQ25890_BOOST_LIM_2150MA        0x06
#define BQ25890_BOOST_LIM_2450MA        0x07

#endif // BQ25890_REGS_H
