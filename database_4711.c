//
// Created by HZQ on 2026/9/12.
//


#include <windows.h>


#include "database_4711.h"



int init_folders() {
    int result = DB_FINE;
    result |= ensure_folder("C:\\ProgramData\\system_4711");
    result |= ensure_folder("C:\\ProgramData\\system_4711\\data") << 1;
    result |= ensure_folder("C:\\ProgramData\\system_4711\\logs") << 2;
    return result;
}

int ensure_folder(const char *path) {
    const DWORD attributes = GetFileAttributesA(path);

    // 若已有文件
    if (attributes != INVALID_FILE_ATTRIBUTES) {
        if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
            return DB_FINE;
        }
        // 不是目录
        return DB_ERROR;
    }

    // 创建文件
    if (CreateDirectoryA(path, NULL)) {
        return DB_FINE;
    }

    return DB_ERROR;

}
