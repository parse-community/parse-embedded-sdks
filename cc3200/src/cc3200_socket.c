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
#include <socket.h>

#include "parse_impl.h"

#define CONNECT_TIMEOUT_MICROSECONDS 5 * 1000 * 1000
#define READ_SMALL_TIMEOUT_MICROSECONDS 10 * 1000

// Unfortunately, select supports only waiting on connect
// no waiting on write, or on exception
#define WAIT_READ 0
#define WAIT_CONNECT 1

short socketWait(short socketHandle, unsigned int timeoutMicroSeconds, int waitType) {
    timeval timeout;
    timeout.tv_sec = timeoutMicroSeconds / 1000000;
    timeout.tv_usec = timeoutMicroSeconds % 1000000;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(socketHandle, &fds);

    return select(socketHandle + 1, waitType ? 0 : &fds, waitType ? &fds : 0, 0, &timeout);
}

short socketSslConnect(const char *host, unsigned short port) {
    short socketHandle = -1;
    int enableOption = 1;

    unsigned long destinationIp = 0;
    short status = gethostbyname((signed char *)host, strlen(host), &destinationIp, AF_INET);

    if (status >= 0) {
        // create the secure socket
        status = socket(AF_INET, SOCK_STREAM, SL_SEC_SOCKET);
    }

    if (status >= 0) {
        socketHandle = status;
    }

    if (status >= 0) {
        unsigned char sslMethod = SL_SO_SEC_METHOD_SSLv3_TLSV1_2;
        status = setsockopt(socketHandle, SOL_SOCKET, SL_SO_SECMETHOD, &sslMethod, sizeof(sslMethod));
    }

    if (status >= 0) {
        ensureCertificateFile();
        status = sl_SetSockOpt(socketHandle, SL_SOL_SOCKET, SL_SO_SECURE_FILES_CA_FILE_NAME, SL_SSL_CA_CERT_FILE_NAME, strlen(SL_SSL_CA_CERT_FILE_NAME));
    }

    if (status >= 0) {
        sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        server.sin_addr.s_addr = htonl(destinationIp);

        status = connect(socketHandle, (const sockaddr *)(&server), sizeof(server));
    }

#ifdef SSL_DEBUG
    if (status == SL_ESECSNOVERIFY) {
        DEBUG_PRINT("[Parse] Can't verify server certificate.\r\n");
    } else if (status == SL_ESECDATEERROR) {
        DEBUG_PRINT("[Parse] Server certificate date error.\r\n");
    } else if (status == SL_ESECBADCAFILE) {
        DEBUG_PRINT("[Parse] Server root certificate is missing.\r\n");
    } else if (status < 0) {
        DEBUG_PRINT("[Parse] SSL connect error: %d.\r\n", status);
    }
#endif /* SSL_DEBUG */

    if (status >= 0) {
        status = socketWait(socketHandle, CONNECT_TIMEOUT_MICROSECONDS, WAIT_CONNECT);
    }

    if (status >= 0) {
        status = setsockopt(socketHandle, SOL_SOCKET, SO_NONBLOCKING, &enableOption, sizeof(enableOption));
    }

    if (status >= 0) {
        status = setsockopt(socketHandle, SOL_SOCKET, SO_KEEPALIVE, &enableOption, sizeof(enableOption));
    }

    if (status < 0) {
        close(socketHandle);
    }

    return (status < 0) ? status : socketHandle;
}

short socketRead(short socketHandle, char *receiveBuffer, unsigned int receiveBufferSize, unsigned int timeoutMilliseconds) {
    short status = 0;
    short readBlock = 0;
    short bytesReceived = 0;

    if (socketHandle < 0) {
        return socketHandle;
    }

    if ((receiveBuffer == NULL) || (receiveBufferSize == 0)) {
        return -1;
    }

    receiveBuffer[0] = 0;

    do {
        status = socketWait(socketHandle, readBlock ? READ_SMALL_TIMEOUT_MICROSECONDS : timeoutMilliseconds * 1000, WAIT_READ);
        if (status <= 0) {
            break;
        }

        while ((status = recv(socketHandle, receiveBuffer + bytesReceived, receiveBufferSize - bytesReceived - 1, 0)) == EAGAIN) {
            if (readBlock) {
                break;
            }

#ifndef SL_PLATFORM_MULTI_THREADED
            _SlNonOsMainLoopTask();
#endif
        }

        if (status >= 0) {
            bytesReceived += status;
            receiveBuffer[bytesReceived] = 0;
            readBlock = 1;
        } else {
            break;
        }

    } while (bytesReceived < receiveBufferSize - 1);

    receiveBuffer[bytesReceived] = 0;

    return (status < 0) ? status : bytesReceived;
}

short socketWrite(short socketHandle, const char *sendBuffer, unsigned int sendBufferSize) {
    short status = 0;

    if (socketHandle < 0) {
        return socketHandle;
    }

    if (sendBuffer == NULL) {
        return -1;
    }

    // Assume the buffer to send is a zero-terminated string
    // Safe, as this is internal use only
    if (sendBufferSize == 0) {
        sendBufferSize = strlen(sendBuffer);
    }

    if (sendBufferSize > 0) {
        status = send(socketHandle, sendBuffer, sendBufferSize, 0);
    }

    return status;
}

short socketClose(short socketHandle) {
    return close(socketHandle);
}
