/*
 *  Copyright (c) 2015, Parse, LLC. All rights reserved.
 *
 *  You are hereby granted a non-exclusive, worldwide, royalty-free license to use,
 *  copy, modify, and distribute this software in source code or binary form for use
 *  in connection with the web services and APIs provided by Parse.
 *
 *  As with any software that integrates with the Parse platform, your use of
 *  this software is subject to the Parse Terms of Service
 *  [https://www.parse.com/about/terms]. This copyright notice shall be
 *  included in all copies or substantial portions of the software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "blink.h"

OsiTaskHandle ledBlinkTask = 0;
int ledStateIndex = 0;
const char* ledStates[] = {"off", "on", "blink"};

void ledBlinkHandler(void *pvParameters) {
    GPIO_IF_LedOff(MCU_RED_LED_GPIO);

    while(1) {
        GPIO_IF_LedToggle(MCU_RED_LED_GPIO);
        UtilsDelay(10000000 / 3); // 250 ms
        GPIO_IF_LedToggle(MCU_RED_LED_GPIO);
        UtilsDelay(10000000 / 3); // 250 ms
    }
}

int setLedState(const char* state) {
    if (strcmp("on", state) == 0 || strcmp("off", state) == 0 ) {
        if (ledBlinkTask) {
            osi_TaskDelete(&ledBlinkTask);
            ledBlinkTask = 0;
        }
        if (strcmp("on", state) == 0) {
            UART_PRINT("[Blink] LED on\r\n");
            GPIO_IF_LedOn(MCU_RED_LED_GPIO);
            ledStateIndex = 1;
        } else {
            UART_PRINT("[Blink] LED off\r\n");
            GPIO_IF_LedOff(MCU_RED_LED_GPIO);
            ledStateIndex = 0;
        }
    } else if (strcmp("blink", state) == 0) {
        UART_PRINT("[Blink] LED blinking\r\n");
        if (!ledBlinkTask) {
            osi_TaskCreate(ledBlinkHandler, (signed char*) "LEDBlinkHandler",
                    OSI_STACK_SIZE,  NULL, OOB_TASK_PRIORITY, &ledBlinkTask);
        }
        ledStateIndex = 2;
    } else {
        // Unknown message, stay in the same state
        return 0;
    }

    return 1;
}

const char *getLedState() {
    return ledStates[ledStateIndex];
}

void cycleLedState() {
    // cycle to next LED states
    // off -> on -> blink -> off
    ledStateIndex = (ledStateIndex + 1) % 3;
    setLedState(ledStates[ledStateIndex]);
}
