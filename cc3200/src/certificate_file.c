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

#include <stdio.h>

#include <parse_impl.h>

void ensureCertificateFile(const unsigned char *certificate, unsigned int certificateLen) {
    long certificateFile = -1;

    short status = sl_FsOpen((unsigned char *)SSL_CA_CERT_FILE_NAME, FS_MODE_OPEN_READ, NULL, &certificateFile);
    if (status < 0) {
#ifdef SSL_DEBUG
        DEBUG_PRINT("[Parse] Creating certificate file\r\n");
#endif /* SSL_DEBUG */

        sl_FsClose(certificateFile, 0, 0, 0);
        certificateFile = -1;

        status = sl_FsOpen((unsigned char *)SSL_CA_CERT_FILE_NAME,
                FS_MODE_OPEN_CREATE(65536, _FS_FILE_OPEN_FLAG_COMMIT | _FS_FILE_PUBLIC_WRITE),
                NULL, &certificateFile);
        if (status < 0) {
#ifdef SSL_DEBUG
            DEBUG_PRINT("[Parse] Error creating certificate file: %d\r\n", status);
            if (status == SL_FS_ERR_NO_AVAILABLE_BLOCKS) {
                 DEBUG_PRINT("[Parse] No available blocks. Need to flash your device file system...\r\n");
            }
#endif /* SSL_DEBUG */
            return;
        }

        sl_FsClose(certificateFile, 0, 0, 0);
        certificateFile = -1;

        status = sl_FsOpen((unsigned char *)SSL_CA_CERT_FILE_NAME, FS_MODE_OPEN_WRITE, NULL, &certificateFile);
        if (status < 0) {
#ifdef SSL_DEBUG
            DEBUG_PRINT("[Parse] Error creating certificate file: %d\r\n", status);
            if (status == SL_FS_ERR_NO_AVAILABLE_BLOCKS) {
                 DEBUG_PRINT("[Parse] No available blocks. Need to flash your device file system...\r\n");
            }
#endif /* SSL_DEBUG */
            sl_FsClose(certificateFile, 0, 0, 0);
            return;
        }

        status = sl_FsWrite(certificateFile, 0, (unsigned char*)certificate, certificateLen);
        if (status < 0) {
#ifdef SSL_DEBUG
            DEBUG_PRINT("[Parse] Error creating certificate file: %d\r\n", status);
            if (status == SL_FS_ERR_NO_AVAILABLE_BLOCKS) {
                 DEBUG_PRINT("[Parse] No available blocks. Need to flash your device file system...\r\n");
            }
#endif /* SSL_DEBUG */
        }
    }

    sl_FsClose(certificateFile, 0, 0, 0);
}
