#include "buttons.h"
#include "hardware/gpio.h"

// Return if the button up is pressed
bool buttonUpPressed() {
    return !gpio_get(GPIO_UP);
}

// Return if the button down is pressed
bool buttonDownPressed() {
    return !gpio_get(GPIO_DOWN);
}

// Return if the button left is pressed
bool buttonLeftPressed() {
    return !gpio_get(GPIO_LEFT);
}

// Return if the button right is pressed
bool buttonRightPressed() {
    return !gpio_get(GPIO_RIGHT);
}
