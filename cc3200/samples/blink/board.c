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
#include "pinmux.h"

void initBoard() {
#ifndef USE_TIRTOS
#if defined(ccs) || defined(gcc)
    MAP_IntVTableBaseSet((unsigned long) &g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif

    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();

    PinMuxConfig();
    GPIO_IF_LedConfigure(LED1);
    GPIO_IF_LedOff(MCU_RED_LED_GPIO);

    InitTerm();
    ClearTerm();

    UART_PRINT("Blink - Parse for IoT sample application\r\n");
    UART_PRINT("----------------------------------------\r\n");
    UART_PRINT("\r\n");
    UART_PRINT("[Blink] Board init\r\n");

    // start the spawn task
    short status = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
    if (status < 0) {
        UART_PRINT("[Blink] Spawn task failed\r\n");
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    // initialize the I2C bus
    status = I2C_IF_Open(I2C_MASTER_MODE_FST);
    if (status < 0) {
        UART_PRINT("[Blink] I2C opening error\r\n");
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    UART_PRINT("[Blink] Device                    : TI SimpleLink CC3200\r\n");
#ifdef USE_TIRTOS
    UART_PRINT("[Blink] Operating system          : TI-RTOS\r\n");
#endif
#ifdef USE_FREERTOS
    UART_PRINT("[Blink] Operating system          : FreeRTOS\r\n");
#endif
#ifndef SL_PLATFORM_MULTI_THREADED
    UART_PRINT("[Blink] Operating system          : None\r\n");
#endif
}

void resetBoard() {
    UART_PRINT("[Blink] Board shutdown\r\n");

    sl_Stop(SL_STOP_TIMEOUT);
    PRCMMCUReset(TRUE);
}

void factoryReset() {
    UART_PRINT("[Blink] Factory reset\r\n");

    resetParseClient();
    resetConfig();
    resetNetwork();
    resetBoard();
}

void getBoardTimeDate(SlDateTime_t *dateTime) {
    unsigned char configOpt = SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME;
    unsigned char configLen = sizeof(SlDateTime_t);
    memset(dateTime, 0, configLen);
    sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOpt, &configLen, (unsigned char *)dateTime);
}

void setBoardTimeDate(SlDateTime_t *dateTime) {
    sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION, SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME, sizeof(SlDateTime_t), (unsigned char *)dateTime);
}

#ifdef USE_FREERTOS
void vAssertCalled(const char *file, unsigned long line) {
    UART_PRINT("[FreeRTOS] Assert at %s (line %ul)\r\n", file, line);
    LOOP_FOREVER();
}

void vApplicationIdleHook(void) {
}

void vApplicationMallocFailedHook() {
    UART_PRINT("[FreeRTOS] Malloc fail\r\n");
    LOOP_FOREVER();
}

void vApplicationStackOverflowHook(OsiTaskHandle *task, signed char *taskName) {
    UART_PRINT("[FreeRTOS] Stack Overflow in task %s\r\n", taskName);
    LOOP_FOREVER();
}
#endif //USE_FREERTOS
