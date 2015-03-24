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

void onButton2Pressed(void *p) {
    UART_PRINT("[Blink] Cycle LED state\r\n");

    Button_IF_EnableInterrupt(SW2);
    cycleLedState();
    saveLedState();
}

void onButton3Pressed(void *p) {
    Button_IF_EnableInterrupt(SW3);
    factoryReset();
}

void postButton2Msg(void) {
    ApplicationMessage msg;
    msg.handler = &onButton2Pressed;
    msg.params = NULL;
    osi_MsgQWrite(&g_ApplicationMessageQueue, &msg, OSI_NO_WAIT);
}

void postButton3Msg(void) {
    ApplicationMessage msg;
    msg.handler = &onButton3Pressed;
    msg.params = NULL;
    osi_MsgQWrite(&g_ApplicationMessageQueue, &msg, OSI_NO_WAIT);
}

void enableButtons() {
    Button_IF_Init(postButton2Msg, postButton3Msg);
    Button_IF_EnableInterrupt(SW2 | SW3);
}
