#include "hal/device.h"
#include "hal/inputs/buttons.h"
#include "hal/power/pmic.h"
#include "idf/launcher_platform.h"
#include "powerSave.h"
#include <Wire.h>

#define SEL_BTN 0
#define UP_BTN 41
#define DW_BTN 40
#define L_BTN 38
#define R_BTN 39
#define BACK_BTN 4          // not wired in yet, see note below
#define BTN_ACT LOW

#define CC1101_CS_PIN 46
#define NRF24_CS_PIN 14
#define NRF24_CE_PIN 21

#define I2C_SDA 47
#define I2C_SCL 42
#define BQ25896_ADDRESS 0x6B

static DeviceButtons buttonsCfg() { return DeviceButtons{L_BTN, R_BTN, UP_BTN, DW_BTN, SEL_BTN}; }

void _setup_gpio() {
    hal_buttons_init(buttonsCfg(), 5);

    // Radios are on their own SPI bus; just keep them idle
    launcherGpioOutput(CC1101_CS_PIN);
    launcherGpioOutput(NRF24_CS_PIN);
    launcherGpioOutput(NRF24_CE_PIN);
    launcherGpioWrite(CC1101_CS_PIN, HIGH);
    launcherGpioWrite(NRF24_CS_PIN, HIGH);
    launcherGpioWrite(NRF24_CE_PIN, LOW);   // NRF24 standby

    Wire.begin(I2C_SDA, I2C_SCL);
    DevicePmic pmicCfg{I2C_SDA, I2C_SCL, BQ25896_ADDRESS};
    if (!hal_pmic_init(pmicCfg)) { launcherConsolePrintln("PMIC: Failed starting BQ25896"); }
}

int getBattery() {
    int mv = hal_pmic_get_system_voltage_mv();
    int percent = (mv - 3300) * 100 / (4150 - 3300);
    return percent < 0 ? 0 : percent > 100 ? 100 : percent;
}

void InputHandler(void) { hal_buttons_poll_5(buttonsCfg()); }
