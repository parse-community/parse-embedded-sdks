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

#include <simplelink.h>
#include <hw_types.h>
#include <hw_memmap.h>
#include <uart.h>
#include <device.h>

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "parse_impl.h"

#ifdef CONSOLE_DEBUG
void debugPrint(const char *buffer) {
    if (buffer != NULL) {
        while(*buffer != '\0') {
            UARTCharPut(UARTA0_BASE,*buffer++);
        }
    }
}

int debugPrintf(const char *format, ...) {
    int status = 0;

    va_list list;
    int size = 256;
    char *buffer2 = NULL;
    char *buffer = (char*)malloc(size);

    if (buffer == NULL) {
        return -1;
    }

    while (1) {
        va_start(list, format);
        status = vsnprintf(buffer, size, format, list);
        va_end(list);

        if ((status > -1) && (status < size)) {
            break;
        } else {
            size *= 2;
            if ((buffer2=realloc(buffer, size)) == NULL) {
                status = -1;
                break;
            } else {
                buffer = buffer2;
                buffer2 = NULL;
            }
        }
    }

    debugPrint(buffer);

    free(buffer);

    return status;
}
#endif /* CONSOLE_DEBUG */
