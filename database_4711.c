//
// Created by HZQ on 2026/9/12.
//


#include <windows.h>
#include <stdio.h>
#include <time.h>

#include "database_4711.h"




// 账户Vector
static int av_size = 0;
static int av_capacity = 0;
static Account **av;

// 物品Vector
static int iv_size = 0;
static int iv_capacity = 0;
static Item **iv;

// 消息Vector
// Record会进行malloc操作分配内存，注意free
static int rv_size = 0;
static int rv_capacity = 0;
static Record **rv;




// 初始化数据库
int init_database() {
    int result = DB_FINE;
    result |= init_folders();
    result |= init_accounts() << 3;
    result |= init_items() << 4;
    return result;
}

// 确保文件夹存在
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




// 导入账户
int init_accounts() {
    FILE *file = fopen(DB_ACCOUNT_PATH, "r");
    if (file == NULL) {
        return DB_ERROR;
    }
    char buffer_0[1024];
    char buffer_1[1024];
    char buffer_2[4];
    while (fscanf(file, "%s%s%s", buffer_0, buffer_1, buffer_2) == 3) {
        Account *account = malloc(sizeof(Account));
        account -> name = malloc(strlen(buffer_0) + 1);
        strcpy(account -> name, buffer_0);
        account -> password = malloc(strlen(buffer_1) + 1);
        strcpy(account -> password, buffer_1);
        if (strcmp(buffer_2, "ad") == 0) {
            account -> is_administrator = 1;
        } else {
            account -> is_administrator = 0;
        }
        add_account(account);
    }
    fclose(file);
    return DB_FINE;
}

void add_account(Account *account) {
    if (av_capacity == 0) {
        av_capacity = 16;
        av = malloc(sizeof(Account *) * av_capacity);
    }

    if (account == NULL) {
        return;
    }

    if (av_size >= av_capacity) {
        av_capacity *= 2;
        Account **new_av = malloc(sizeof(Account *) * av_capacity);
        for (int i = 0; i < av_size; i++) {
            new_av[i] = av[i];
        }
        free(av);
        av = new_av;
    }
    av[av_size ++] = account;
}

void save_account() {
    FILE *file = fopen(DB_ACCOUNT_PATH, "w");
    for (int i = 0; i < av_size; i++) {
        fprintf(file, "%s\t", av[i] -> name);
        fprintf(file, "%s\t", av[i] -> password);
        if (av[i] -> is_administrator) {
            fprintf(file, "ad\n");
        } else {
            fprintf(file, "cr\n");
        }
    }
    fclose(file);
}




// 导入物品
int init_items() {
    FILE *file = fopen(DB_ITEM_PATH, "r");
    if (file == NULL) {
        return DB_ERROR;
    }
    char buffer[1024];
    int id, stock, price;
    while (fscanf(file, "%d%s%d%d", &id, buffer, &stock, &price) == 4) { // NOLINT(*-err34-c)
        Item *item = malloc(sizeof(Item));
        item -> id = id;
        item -> stock = stock;
        item -> price = price;
        item -> name = malloc(strlen(buffer) + 1);
        strcpy(item -> name, buffer);
        add_item(item);
    }
    fclose(file);
    return DB_FINE;
}

void add_item(Item *item) {
    if (iv_capacity == 0) {
        iv_capacity = 16;
        iv = malloc(sizeof(Item *) * iv_capacity);
    }

    if (item == NULL) {
        return;
    }

    if (iv_size >= iv_capacity) {
        iv_capacity *= 2;
        Item **new_iv = malloc(sizeof(Item *) * iv_capacity);
        for (int i = 0; i < iv_size; i++) {
            new_iv[i] = iv[i];
        }
        free(iv);
        iv = new_iv;
    }
    iv[iv_size ++] = item;
}

void delete_item(const Item *item) {
    int i;
    for (i = 0; i < iv_size; i++) {
        if (iv[i] -> id == item -> id) {
            free(iv[i] -> name);
            free(iv[i]);
            for (int j = i; j < iv_size - 1; j++) {
                iv[j] = iv[j + 1];
            }
            iv_size --;
            break;
        }
    }
}

void save_item() {
    FILE *file = fopen(DB_ITEM_PATH, "w");
    for (int i = 0; i < iv_size; i++) {
        fprintf(file, "%d\t", iv[i] -> id);
        fprintf(file, "%s\t", iv[i] -> name);
        fprintf(file, "%d\t", iv[i] -> stock);
        fprintf(file, "%d\n", iv[i] -> price);
    }
    fclose(file);
}





// 导入某天的记录
int init_records(const struct tm time) {
    char path[256];
    sprintf(path, "%s%d_%d_%d.txt", DB_RECORD_PATH, time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return DB_ERROR;
    }
    // 释放内存
    for (int i = 0; i < rv_size; i++) {
        for (int j = 0; j < rv[i] -> length; j++) {
            free(rv[i] -> units[j].name);
        }
        free(rv[i] -> units);
        free(rv[i]);
    }
    rv_size = 0;

    int year, month, day, hour, minute, second, length;
    int id, price, number;
    while (fscanf(file, "%d%d%d%d%d%d%d", &year, &month, &day, &hour, &minute, &second, &length) == 7) { // NOLINT(*-err34-c)
        Record *record = malloc(sizeof(Record));
        record -> year = year;
        record -> month = month;
        record -> day = day;
        record -> hour = hour;
        record -> minute = minute;
        record -> second = second;
        record -> length = length;
        record -> units = malloc(sizeof(RecordUnit) * length);

        char buffer[1024];

        for (int i = 0; i < length; i++) {
            if (fscanf(file, "%d%s%d%d", &id, buffer, &price, &number) != 4) { // NOLINT(*-err34-c)
                record->length = i;
                break;
            }
            RecordUnit *unit = record-> units + i;
            unit -> id = id;
            unit -> price = price;
            unit -> number = number;
            unit -> name = malloc(strlen(buffer) + 1);
            strcpy(unit -> name, buffer);
        }

        add_record(record);
    }
    fclose(file);
    return DB_FINE;
};

void add_record(Record *record) {
    if (rv_capacity == 0) {
        rv_capacity = 16;
        rv = malloc(sizeof(Record *) * rv_capacity);
    }

    if (record == NULL) {
        return;
    }

    if (rv_size >= rv_capacity) {
        rv_capacity *= 2;
        Record **new_rv = malloc(sizeof(Record *) * rv_capacity);
        for (int i = 0; i < rv_size; i++) {
            new_rv[i] = rv[i];
        }
        free(rv);
        rv = new_rv;
    }
    rv[rv_size ++] = record;
};

void save_record(const struct tm time) {
    char path[256];
    sprintf(path, "%s%d_%d_%d.txt", DB_RECORD_PATH, time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);
    FILE *file = fopen(path, "w");
    for (int i = 0; i < rv_size; i++) {
        fprintf(file, "%d\t", rv[i] -> year);
        fprintf(file, "%d\t", rv[i] -> month);
        fprintf(file, "%d\t", rv[i] -> day);
        fprintf(file, "%d\t", rv[i] -> hour);
        fprintf(file, "%d\t", rv[i] -> minute);
        fprintf(file, "%d\t", rv[i] -> second);
        fprintf(file, "%d\n", rv[i] -> length);
        for (int j = 0; j < rv[i] -> length; j++) {
            fprintf(file, "%d\t", rv[i] -> units[j].id);
            fprintf(file, "%s\t", rv[i] -> units[j].name);
            fprintf(file, "%d\t", rv[i] -> units[j].price);
            fprintf(file, "%d\n", rv[i] -> units[j].number);
        }
    }
    fclose(file);
}



