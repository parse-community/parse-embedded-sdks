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

/***** A simple demo of Parse for IoT SDK *****/
/*
 *   Press SW3 - the device will cycle the LED throu off | on | blink state and update its state in the Parse cloud application
 *   Press SW2 - the devive will be factory reset and lose all configuration information (if J4 is shorted, SW3 is not active)
 *     Push notification - the device will change LED state appropriately and update the state in the Parse cloud application
 *                         Notification payload is in the following format: {"alert":"blink|on|off"}
 *
 *   http://parse.com
 */

#include "blink.h"

OsiMsgQ_t g_ApplicationMessageQueue;

void applicationMessageLoop() {
    ApplicationMessage msg;
    int status = 0;

    for (;;) {
        status = osi_MsgQRead(&g_ApplicationMessageQueue, &msg, OSI_WAIT_FOREVER);
        if (OSI_OK == status) {
            msg.handler(msg.params);
        }
    }
}

void startApplication(void *pvParameters) {
    // init the network state
    initNetwork();

    // init the Parse app config
    short status = initConfig();

    if (status < 0) {
        // not configured, start configuration mode
        startConfigurationMode();
    } else {
        // configured, start application mode
        startApplicationMode();

        setBoardTimeFromNtp();

        initParseClient();

        // update Led state to parse
        saveLedState();

        // start the Parse push service
        status = startPushServiceTask();
        if (status < 0) {
            UART_PRINT("[Blink] Start push service task error\r\n");
            ERR_PRINT(status);
        }

        // configure and enable the board buttons
        enableButtons();

        // run the message loop
        applicationMessageLoop();
    }
}

int main(void) {
    // init the hardware
    initBoard();

    UART_PRINT("[Blink] Start application\r\n");

    // create the main application message queue
    // this call properly enables the OSI scheduler to function
    short status = osi_MsgQCreate(&g_ApplicationMessageQueue, "ApplicationMessageQueue", sizeof(ApplicationMessage), 1);
    if (status < 0) {
        UART_PRINT("[Blink] Create application message queue error\r\n");
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    // start the main application task
    // this is necessary because SimpleLink host driver is started by sl_Start(),
    // which cannot be called on before the OSI scheduler is started
    status = osi_TaskCreate(startApplication, (signed char *)"Blink", OSI_STACK_SIZE,  NULL, OOB_TASK_PRIORITY, NULL);
    if (status < 0) {
        UART_PRINT("[Blink] Start application error\r\n");
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    // start the OSI scheduler
    osi_start();

    return 0;
}
