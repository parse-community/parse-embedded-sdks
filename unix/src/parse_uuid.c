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

#if (defined (__APPLE__) && defined (__MACH__))
#define OSX
#endif


#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include<uuid/uuid.h>

void parseGetUUID(char *id, size_t id_size) {
    uuid_t uuid;
#ifdef OSX
    uuid_string_t uuid_string;
#else 
    char uuid_string[37];
#endif
    uuid_generate(uuid);
    uuid_unparse(uuid, uuid_string);
    strncpy(id, uuid_string, id_size);
    int i = 0;
    for ( ; i < strlen(id) ; i++) {
        id[i] = tolower(id[i]);
    }
}
