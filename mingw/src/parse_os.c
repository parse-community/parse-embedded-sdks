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

#include <config.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <windows.h>
#include <sys/stat.h>
#include <unistd.h>

#include <parse_log.h>

static char parse_local_store[PATH_MAX] = {0};

static int parseCreateDir(const char* dir) {
    if (mkdir(dir,  S_ISUID | S_IRWXU)) {
        if (errno == EEXIST) {
            parseLog(PARSE_LOG_INFO, "Local store already exists.\n");
        } else {
            parseLog(PARSE_LOG_WARN, "Got error creating local store: %s\n", strerror(errno));
	    return 1;
        }
    }
    if (chmod(dir, S_ISUID | S_IRWXU)) {
        parseLog(PARSE_LOG_WARN, "Got error changing permissions of local store: %s\n", strerror(errno));
    }
    return 0;
}

static void parseGetLocalStore(const char* applicationId, char* buffer, size_t size) {
    char* homedir = NULL;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    strncpy(buffer, homedir, size);
#ifdef PACKAGE_NAME
    strncat(buffer, "/.", PATH_MAX - strlen(buffer) - 1);
    strncat(buffer, PACKAGE_NAME, PATH_MAX - strlen(buffer) - 1);
    strncat(buffer, "/", PATH_MAX - strlen(buffer) - 1);
#else
    strncat(buffer, "/.parse-embedded-local-store/", PATH_MAX - strlen(buffer) - 1);
#endif

    if(parseCreateDir(buffer)) {
        parseLog(PARSE_LOG_ERROR, "Could not create local store dir: %s\n", buffer);
    }

    strncat(buffer, applicationId, PATH_MAX - strlen(buffer) - 1);
    strncat(buffer, "/", PATH_MAX - strlen(buffer) - 1);

    if(parseCreateDir(buffer)) {
        parseLog(PARSE_LOG_ERROR, "Could not create local store dir for the app: %s\n", buffer);
    }
}

static int parseOsSetLocalStorageIfNeeded(const char* applicationId) {
    if (parse_local_store[0] == '\0') {
        parseGetLocalStore(applicationId, parse_local_store, sizeof(parse_local_store));
        if (parse_local_store[0] == '\0') {
            return 1;
        }
    }
    return 0;
}

void parseOsGetVersion(char* buffer, size_t size) {
    OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    if (GetVersionEx(&osvi)){
        parseLog(PARSE_LOG_WARN, "Failed to get OS info: %s\n", strerror(errno));
        snprintf(buffer, size, "unknown");
        return;
    }
    snprintf(buffer, size, "%s-%s-%s", "Windows", osvi.dwMajorVersion, osvi.dwMinorVersion);
    parseLog(PARSE_LOG_INFO, "os version:  %s\n", buffer);
}

void parseOsStoreKey(const char* applicationId, const char* key, const char* value) {
    if (parseOsSetLocalStorageIfNeeded(applicationId)) {
        return;
    }
    char key_file[PATH_MAX] = {0};
    strncat(key_file, parse_local_store, PATH_MAX - strlen(key_file) - 1);
    strncat(key_file, key, PATH_MAX - strlen(key_file) - 1);
    int fdkey = open(key_file, O_WRONLY | O_CREAT | O_TRUNC,  S_ISUID | S_IRWXU);
    if (fdkey >= 0) {
        parseLog(PARSE_LOG_INFO, "Writing %s\n", key_file);
        int bytes = write(fdkey, value, strlen(value) + 1);
        close(fdkey);
    } else {
        parseLog(PARSE_LOG_ERROR, "Could not write %s\n", key_file);
        parseLog(PARSE_LOG_ERROR, "Got error writing: %s\n", strerror(errno));
    }
    return;
}

void parseOsLoadKey(const char* applicationId, const char* key, char* value, size_t size) {
    if (size <= 0) return;

    if (parseOsSetLocalStorageIfNeeded(applicationId)) {
        return;
    }

    char key_file[PATH_MAX] = {0};
    strncat(key_file, parse_local_store, PATH_MAX - strlen(key_file) - 1);
    strncat(key_file, key, PATH_MAX - strlen(key_file) - 1);
    int fdkey = open(key_file, O_RDONLY);
    if (fdkey >= 0) {
        parseLog(PARSE_LOG_INFO, "Reading %s\n", key_file);
        int bytes = read(fdkey, value, size);
        close(fdkey);
    } else {
        value[0] = '\0';
        parseLog(PARSE_LOG_INFO, "Could not read %s\n", key_file);
    }
    return;
}

void parseOsClearKey(const char* applicationId, const char* key) {
    if (parseOsSetLocalStorageIfNeeded(applicationId)) {
        return;
    }
    char key_file[PATH_MAX] = {0};
    strncat(key_file, parse_local_store, PATH_MAX - strlen(key_file) - 1);
    strncat(key_file, key, PATH_MAX - strlen(key_file) - 1);

    if (remove(key_file)) {
        parseLog(PARSE_LOG_ERROR, "Cannot delete %s\n", key_file);
        parseLog(PARSE_LOG_ERROR, "Got error unlinking: %s\n", strerror(errno));
    }
}

static int parseOsRemoveDir(const char* dir) {
    DIR* dir_ptr;
    struct dirent* entry_ptr;

    char filename[FILENAME_MAX];

    dir_ptr = opendir(dir);
    if (dir_ptr != NULL) {
        while ((entry_ptr = readdir(dir_ptr))) {
            struct stat entry_stats;
            snprintf(filename, FILENAME_MAX, "%s/%s", dir, entry_ptr->d_name);

            if (lstat(filename, &entry_stats) < 0) {
                parseLog(PARSE_LOG_ERROR, "Could not stat %s\n", filename);
                return 1;
            }
            if(S_ISDIR(entry_stats.st_mode)) {
                if(strcmp(entry_ptr->d_name, ".") && strcmp(entry_ptr->d_name, "..")) {
                    parseOsRemoveDir(filename);
                }
            } else {
                remove(filename);
                parseLog(PARSE_LOG_INFO, "Removing: %s\n", filename);
            }
        }
        closedir(dir_ptr);
    }
    remove(dir);
    parseLog(PARSE_LOG_INFO, "Removing: %s\n", dir);
    return 0;
}

void parseOsClearAll(const char* applicationId) {
    if (parseOsSetLocalStorageIfNeeded(applicationId)) {
        parseLog(PARSE_LOG_ERROR, "Cannot get local storage\n");
        return;
    }
    parseLog(PARSE_LOG_INFO, "Removing: %s\n", parse_local_store);
    if (parseOsRemoveDir(parse_local_store)) {
        parseLog(PARSE_LOG_ERROR, "Cannot delete %s\n", parse_local_store);
        parseLog(PARSE_LOG_ERROR, "Got error unlinking: %s\n", strerror(errno));
    }
}
