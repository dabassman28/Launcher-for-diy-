#include "hal/device.h"
#include "hal/inputs/buttons.h"
#include "idf/launcher_platform.h"
#include "powerSave.h"

#define SEL_BTN 0
#define UP_BTN 41
#define DW_BTN 40
#define L_BTN 38
#define R_BTN 39
#define BACK_BTN 4          // not wired in yet, see earlier note
#define BTN_ACT LOW

#define CC1101_CS_PIN 46
#define NRF24_CS_PIN 14
#define NRF24_CE_PIN 21

#define BAT_ADC_PIN 2
#define BAT_DIV_RATIO 2.0f  // Vbat = Vpin * ratio; set this to your divider

static DeviceButtons buttonsCfg() { return DeviceButtons{L_BTN, R_BTN, UP_BTN, DW_BTN, SEL_BTN}; }

void _setup_gpio() {
    hal_buttons_init(buttonsCfg(), 5);

    launcherGpioOutput(CC1101_CS_PIN);
    launcherGpioOutput(NRF24_CS_PIN);
    launcherGpioOutput(NRF24_CE_PIN);
    launcherGpioWrite(CC1101_CS_PIN, HIGH);
    launcherGpioWrite(NRF24_CS_PIN, HIGH);
    launcherGpioWrite(NRF24_CE_PIN, LOW);   // NRF24 standby
}

int getBattery() {
    uint32_t sum = 0;
    for (int i = 0; i < 8; i++) sum += analogReadMilliVolts(BAT_ADC_PIN);
    int mv = (sum / 8) * BAT_DIV_RATIO;
    int percent = (mv - 3300) * 100 / (4150 - 3300);
    return percent < 0 ? 0 : percent > 100 ? 100 : percent;
}

void InputHandler(void) { hal_buttons_poll_5(buttonsCfg()); }
