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

#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int gpioEnable(int gpio, const char *direction) {
    int fd = open("/sys/class/gpio/export", O_WRONLY, 0);
    if (fd > -1) {
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d\n", gpio);
        write(fd, buffer, strlen(buffer));
        close(fd);
    }

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", gpio);

    int i = 20;
    while (1) {
        fd = open(path, O_WRONLY, 0);
        if (fd < 0) {
            usleep(20000);
            if (i-- < 0) {
                close(fd);
                return -1;
            }
        } else {
            write(fd, direction, strlen(direction));
            write(fd, "\n", 1);
            close(fd);
            break;
        }
    }

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", gpio);
    fd = -1;
    if (strncmp("out", direction, sizeof("out")) == 0) {
        fd = open(path, O_WRONLY, 0);
    } 
    if (strncmp("in", direction, sizeof("in")) == 0) {
        fd = open(path, O_RDONLY, O_NONBLOCK);
    } 
    if (fd < 0) {
        return -1;
    }
    return fd;
}

void gpioSetEdge(int gpio, const char *edge) {
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/edge", gpio);
    int fd = open(path, O_WRONLY, 0);
    if (fd < 0) {
        return;
    }
    write(fd, edge, strlen(edge));
    write(fd, "\n", 1);
    close(fd);
}

int gpioReadValue(int gpio_fd, char* value, int value_size) {
	if(lseek(gpio_fd, 0, SEEK_SET) < 0) {
        return 0;
    }
    int r = read(gpio_fd, value, value_size);
    if (r > 1) {
        if (value[r - 1] == '\n') value[r - 1] = '\0';
        else value[r] = '\0';
    }
    return r;
}

void gpioDisable(int gpio) {
    int fd = open("/sys/class/gpio/unexport", O_WRONLY, 0);
    if (fd > -1) {
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d\n", gpio);
        write(fd, buffer, strlen(buffer));
        close(fd);
    }
}

