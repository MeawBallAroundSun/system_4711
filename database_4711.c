//
// Created by HZQ on 2026/9/12.
//


#include "database_4711.h"


// 账户Vector
static int av_size = 0;
static int av_capacity = 0;
static Account **av;

// 物品Vector
static int iv_size = 0;
static int iv_capacity = 0;
static Item **iv;

// 记录Vector
// Record会进行malloc操作分配内存，注意free
static int rv_size = 0;
static int rv_capacity = 0;
static Record **rv;

// 账单记录Vector
static int ruv_size = 0;
static int ruv_capacity = 0;
static RecordUnit **ruv;

static volatile int sales_sum = 0;
static int current_account_index = -1;


int input_command(const char *command) {
    char buffer_0[1024];
    char buffer_1[1024];
    char buffer_2[1024];
    char buffer_3[1024];

    int result = DB_ERROR;
    switch (sscanf(command, "%s%s%s%s", buffer_0, buffer_1, buffer_2, buffer_3)) {
        case 1: {
            if (strcmp(buffer_0, "checkout") == 0 || strcmp(buffer_0, "结账") == 0) {
                result = command_checkout();
            } else if (strcmp(buffer_0, "clear") == 0 || strcmp(buffer_0, "清空") == 0) {
                result = command_clear();
            } else if (strcmp(buffer_0, "sales") == 0 || strcmp(buffer_0, "销售额") == 0) {
                result = command_sales(NULL, NULL, NULL);
            } else if (strcmp(buffer_0, "logout") == 0 || strcmp(buffer_0, "登出") == 0) {
                result = command_logout();
            }
            break;
        }
        case 2: {
            if (strcmp(buffer_0, "search") == 0 || strcmp(buffer_0, "查找") == 0) {
                result = command_search(buffer_1) >= 0 ? DB_FINE : DB_ERROR;
            } else if (strcmp(buffer_0, "pick") == 0 || strcmp(buffer_0, "下单") == 0) {
                result = command_pick(buffer_1, "1");
            } else if (strcmp(buffer_0, "sales") == 0 || strcmp(buffer_0, "销售额") == 0) {
                result = command_sales(NULL, NULL, buffer_1);
            } else if (strcmp(buffer_0, "delete_item") == 0 || strcmp(buffer_0, "删除商品") == 0) {
                result = command_delete_item(buffer_1);
            }
            break;
        }
        case 3: {
            if (strcmp(buffer_0, "pick") == 0 || strcmp(buffer_0, "下单") == 0) {
                result = command_pick(buffer_1, buffer_2);
            } else if (strcmp(buffer_0, "sales") == 0 || strcmp(buffer_0, "销售额") == 0) {
                result = command_sales(NULL, buffer_1, buffer_2);
            } else if (strcmp(buffer_0, "set_price") == 0 || strcmp(buffer_0, "设置售价") == 0) {
                result = command_set_price(buffer_1, buffer_2);
            } else if (strcmp(buffer_0, "set_stock") == 0 || strcmp(buffer_0, "设置库存") == 0) {
                result = command_set_stock(buffer_1, buffer_2);
            } else if (strcmp(buffer_0, "login") == 0 || strcmp(buffer_0, "登录") == 0) {
                result = command_login(buffer_1, buffer_2);
            } else if (strcmp(buffer_0, "create_account") == 0 || strcmp(buffer_0, "创建账户") == 0) {
                result = command_create_account(buffer_1, buffer_2, NULL);
            }
            break;
        }
        case 4: {
            if (strcmp(buffer_0, "sales") == 0 || strcmp(buffer_0, "销售额") == 0) {
                result = command_sales(buffer_1, buffer_2, buffer_3);
            } else if (strcmp(buffer_0, "add_item") == 0 || strcmp(buffer_0, "添加商品") == 0) {
                result = command_add_item(buffer_1, buffer_2, buffer_3);
            } else if (strcmp(buffer_0, "create_account") == 0 || strcmp(buffer_0, "创建账户") == 0) {
                result = command_create_account(buffer_1, buffer_2, buffer_3);
            }
            break;
        }
        default: {
            break;
        }
    }


    return result;
}

// 选中物品到结账区
int command_pick(const char *key_word, const char *number) {
    const int index = command_search(key_word);
    if (index == -1) {
        return DB_ERROR;
    }

    const Item *item = iv[index];

    int mode = 1;
    if (number[0] == '+') {
        mode = 1;
        number ++;
    } else if (number[0] == '-') {
        mode = -1;
        number ++;
    } else if (number[0] == 's') {
        mode = 0;
        number ++;
    }

    int n;
    if (sscanf(number, "%d", &n) != 1) { // NOLINT(*-err34-c)
        return DB_ERROR;
    }

    if (n < 0) {
        return DB_ERROR;
    }

    if (item -> stock <= 0 && mode >= 0) {
        return DB_ERROR;
    }

    add_ru(item, n, mode);
    return DB_FINE;
}

// 结账
int command_checkout() {
    if (ruv_size > 0) {
        const time_t t = time(NULL);
        const struct tm *lt = localtime(&t);
        init_records(lt);

        Record *record = malloc(sizeof(Record));
        record -> year = lt -> tm_year + 1900;
        record -> month = lt -> tm_mon + 1;
        record -> day = lt -> tm_mday;
        record -> hour = lt -> tm_hour;
        record -> minute = lt -> tm_min;
        record -> second = lt -> tm_sec;
        record -> length = ruv_size;
        record -> units = malloc(sizeof(RecordUnit) * ruv_size);
        // 深拷贝、库存对应减少
        for (int i = 0; i < ruv_size; i++) {
            record -> units[i].id = ruv[i] -> id;
            record -> units[i].name = malloc(sizeof(char) * (strlen(ruv[i] -> name) + 1));
            strcpy(record -> units[i].name, ruv[i] -> name);
            record -> units[i].price = ruv[i] -> price;
            record -> units[i].number = ruv[i] -> number;

            for (int j = 0; j < iv_size; j++) {
                if (record -> units[i].id == iv[j] -> id) {
                    iv[j] -> stock -= record -> units[i].number;
                    break;
                }
            }
        }
        add_record(record);
        delete_all_ru();
        save_record(lt);
        save_item();
        return DB_FINE;
    }
    return DB_ERROR;
}

// 查找物品，返回索引，错误则返回-1
int command_search(const char *key_word) {
    int key_id;
    if (sscanf(key_word, "%d", &key_id) == 1) { // NOLINT(*-err34-c)
        for (int i = 0; i < iv_size; i++) {
            if (key_id != -1 && key_id == iv[i] -> id) {
                return i;
            }
        }
    }

    for (int i = 0; i < iv_size; i++) {
        if (strcmp(key_word, iv[i] -> name) == 0) {
            return i;
        }
    }
    return -1;
}

// 清空当前选中商品，重新结账
int command_clear() {
    delete_all_ru();
    return DB_FINE;
}

// 修改价格
int command_set_price(const char *key_word, const char *price) {
    const int index = command_search(key_word);
    if (index == -1) {
        return DB_ERROR;
    }
    double price_double;
    if (sscanf(price, "%lf", &price_double) != 1) { // NOLINT(*-err34-c)
        return DB_ERROR;
    }
    const int price_int = (int) (price_double * 100.0 + 0.5);
    if (price_int <= 0) {
        return DB_ERROR;
    }
    iv[index] -> price = price_int;
    save_item();
    return DB_FINE;
}

// 统计某一天的销售额
int command_sales(const char *year, const char *month, const char *day) {
    sales_sum = 0;

    const time_t t = time(NULL);
    const struct tm *lt = localtime(&t);

    int y;
    if (!(year != NULL && sscanf(year, "%d", &y) == 1)) { // NOLINT(*-err34-c)
        y = lt->tm_year + 1900;
    }
    const int feb = (y % 4 == 0 && y % 100 != 0) || y % 400 == 0 ? 29 : 28;     // 二月天数

    int m;
    if (!(month != NULL && sscanf(month, "%d", &m) == 1)) { // NOLINT(*-err34-c)
        m = lt -> tm_mon + 1;
        if (year != NULL) {
            m = 1;
        }
    }
    if (m < 1 || m > 12) {
        return DB_ERROR;
    }

    int d;
    if (!(day != NULL && sscanf(day, "%d", &d) == 1)) { // NOLINT(*-err34-c)
        d = lt -> tm_mday;
        if (month != NULL) {
            d = 1;
        }
    }
    if (d < 1 || d > 31) {
        return DB_ERROR;
    }
    if ((m == 4 || m == 6 || m == 9 || m == 11) && d > 30) {
        return DB_ERROR;
    }
    if (m == 2 && d > feb) {
        return DB_ERROR;
    }

    struct tm mt = *lt;
    mt.tm_year = y - 1900;
    mt.tm_mon = m - 1;
    mt.tm_mday = d;
    mktime(&mt);

    init_records(&mt);
    int sum = 0;
    for (int i = 0; i < rv_size; i++) {
        for (int j = 0; j < rv[i] -> length; j++) {
            sum += rv[i] -> units[j].number * rv[i] -> units[j].price;
        }
    }
    sales_sum = sum;
    init_records(lt);

    return DB_FINE;
}

// 设置物品库存
int command_set_stock(const char *key_word, const char *number) {
    const int index = command_search(key_word);
    if (index == -1) {
        return DB_ERROR;
    }

    Item *item = iv[index];

    int mode = 0;
    if (number[0] == '+') {
        mode = 1;
        number ++;
    } else if (number[0] == '-') {
        mode = -1;
        number ++;
    }

    int n;
    if (sscanf(number, "%d", &n) != 1) { // NOLINT(*-err34-c)
        return DB_ERROR;
    }

    if (n < 0) {
        return DB_ERROR;
    }

    if (mode == 0) {
        item -> stock = n;
    } else if (mode > 0) {
        item -> stock += n;
    } else {
        item -> stock -= n;
        if (item -> stock < 0) {
            item -> stock = 0;
        }
    }
    save_item();
    return DB_FINE;
}

// 添加新物品
int command_add_item(const char *id, const char *name, const char *price) {
    int id_int;
    if (sscanf(id, "%d", &id_int) != 1) { // NOLINT(*-err34-c)
        return DB_ERROR;
    }
    if (id_int <= 0) {
        return DB_ERROR;
    }

    double price_float;
    if (sscanf(price, "%lf", &price_float) != 1) { // NOLINT(*-err34-c)
        return DB_ERROR;
    }
    const int price_int = (int) (price_float * 100.0 + 0.5);
    if (price_int <= 0) {
        return DB_ERROR;
    }

    // 检测没有重复Item
    for (int i = 0; i < iv_size; i++) {
        if (iv[i] -> id == id_int || strcmp(name, iv[i] -> name) == 0) {
            return DB_ERROR;
        }
    }

    Item *item = malloc(sizeof(Item));
    item -> id = id_int;
    item -> price = price_int;
    item -> stock = 0;
    item -> name = malloc(strlen(name) + 1);
    strcpy(item -> name, name);
    add_item(item);
    save_item();
    return DB_FINE;
}

// 删除物品
int command_delete_item(const char *key_word) {
    const int index = command_search(key_word);
    if (index == -1) {
        return DB_ERROR;
    }
    const Item *item = iv[index];
    delete_item(item);
    save_item();
    return DB_FINE;
}

// 登入
int command_login(const char *name, const char *password) {
    for (int i = 0; i < av_size; i++) {
        if (strcmp(av[i] -> name, name) == 0 && strcmp(av[i] -> password, password) == 0) {
            current_account_index = i;
            return DB_FINE;
        }
    }
    return DB_ERROR;
}

// 登出
int command_logout() {
    if (current_account_index == -1) {
        return DB_ERROR;
    }
    current_account_index = -1;
    return DB_FINE;
}

// 创建账户
int command_create_account(const char *name, const char *password, const char *administrator) {
    if (name == NULL || password == NULL) {
        return DB_ERROR;
    }
    if (strlen(name) < 4 || strlen(password) < 4) {
        return DB_ERROR;
    }
    for (int i = 0; i < av_size; i++) {
        if (strcmp(av[i] -> name, name) == 0) {
            return DB_ERROR;
        }
    }
    Account *account = malloc(sizeof(Account));
    account -> name = malloc(strlen(name) + 1);
    strcpy(account -> name, name);
    account -> password = malloc(strlen(password) + 1);
    strcpy(account -> password, password);
    account -> is_administrator = 0;
    if (administrator != NULL && (strcmp(administrator, "ad") == 0 || strcmp(administrator, "admin") == 0 || strcmp(administrator, "administrator") == 0 || strcmp(administrator, "管理员") == 0)) {
        account -> is_administrator = 1;
    }
    add_account(account);
    save_account();
    return DB_FINE;
}

// 获取统计后的销售额
int get_sales() {
    return sales_sum;
}

// 获取当前账户
Account *get_current_account() {
    if (current_account_index == -1) {
        return NULL;
    }
    return av[current_account_index];
}

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
    for (int i = 0; i < av_size; i ++) {
        free(av[i] -> name);
        free(av[i] -> password);
        free(av[i]);
    }
    av_size = 0;

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

    if (file == NULL) {
        return;
    }

    for (int i = 0; i < av_size; i++) {
        fprintf(file, "%s\t", av[i] -> name);
        fprintf(file, "%s\t", av[i] -> password);
        if (av[i] -> is_administrator) {
            fprintf(file, "ad\n");
        } else {
            fprintf(file, "ca\n");
        }
    }
    fclose(file);
}

int get_account_number() {
    return av_size;
}

Account *get_account(const int index) {
    if (index >= 0 && index < av_size) {
        return av[index];
    }
    return NULL;
}





// 导入物品
int init_items() {
    for (int i = 0; i < iv_size; i ++) {
        free(iv[i] -> name);
        free(iv[i]);
    }
    iv_size = 0;

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
    for (int i = 0; i < iv_size; i++) {
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

    if (file == NULL) {
        return;
    }

    for (int i = 0; i < iv_size; i++) {
        fprintf(file, "%d\t", iv[i] -> id);
        fprintf(file, "%s\t", iv[i] -> name);
        fprintf(file, "%d\t", iv[i] -> stock);
        fprintf(file, "%d\n", iv[i] -> price);
    }
    fclose(file);
}

int get_item_number() {
    return iv_size;
}

Item *get_item(const int index) {
    if (index >= 0 && index < iv_size) {
        return iv[index];
    }
    return NULL;
}





// 导入某天的记录
int init_records(const struct tm *time) {
    // 释放内存
    for (int i = 0; i < rv_size; i++) {
        for (int j = 0; j < rv[i] -> length; j++) {
            free(rv[i] -> units[j].name);
        }
        free(rv[i] -> units);
        free(rv[i]);
    }
    rv_size = 0;


    char path[256];
    sprintf(path, "%s%d_%d_%d.txt", DB_RECORD_PATH, time -> tm_year + 1900, time -> tm_mon + 1, time -> tm_mday);
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        return DB_ERROR;
    }


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
}

void save_record(const struct tm *time) {
    char path[256];
    sprintf(path, "%s%d_%d_%d.txt", DB_RECORD_PATH, time->tm_year + 1900, time->tm_mon + 1, time->tm_mday);
    FILE *file = fopen(path, "w");

    if (file == NULL) {
        return;
    }

    for (int i = 0; i < rv_size; i++) {
        fprintf(file, "%d\t", rv[i] -> year);
        fprintf(file, "%d\t", rv[i] -> month);
        fprintf(file, "%d\t", rv[i] -> day);
        fprintf(file, "%d\t", rv[i] -> hour);
        fprintf(file, "%d\t", rv[i] -> minute);
        fprintf(file, "%d\t", rv[i] -> second);
        fprintf(file, "%d\n", rv[i] -> length);
        for (int j = 0; j < rv[i] -> length; j++) {
            fprintf(file, "\t%d\t", rv[i] -> units[j].id);
            fprintf(file, "%s\t", rv[i] -> units[j].name);
            fprintf(file, "%d\t", rv[i] -> units[j].price);
            fprintf(file, "%d\n", rv[i] -> units[j].number);
        }
    }
    fclose(file);
}

int get_record_number() {
    return rv_size;
}

Record *get_record(const int index) {
    if (index >= 0 && index < rv_size) {
        return rv[index];
    }
    return NULL;
}




// 根据物品增加记录，模式为1（增加），0（设置）和-1（减少）
void add_ru(const Item *item, const int number, const int mode) {
    if (item == NULL) {
        return;
    }

    const int id = item -> id;
    const char *name = item -> name;
    int ruv_index = -1;

    // 检索是否已有此RecordUnit
    for (int i = 0; i < ruv_size; i++) {
        if (ruv[i] -> id == id && strcmp(name, ruv[i] -> name) == 0) {
            ruv_index = i;
            break;
        }
    }

    if (ruv_index == -1) {
        if (mode >= 0) {
            const int n = number <= item -> stock ? number : item -> stock;
            if (n <= 0) {
                return;
            }
            RecordUnit *unit = malloc(sizeof(RecordUnit));
            unit -> id = id;
            unit -> name = malloc(strlen(name) + 1);
            strcpy(unit -> name, name);
            unit -> price = item -> price;
            unit -> number = n;

            if (ruv_capacity == 0) {
                ruv_capacity = 16;
                ruv = malloc(sizeof(RecordUnit *) * ruv_capacity);
            }

            if (ruv_size >= ruv_capacity) {
                ruv_capacity *= 2;
                RecordUnit **new_ruv = malloc(sizeof(RecordUnit *) * ruv_capacity);
                for (int i = 0; i < ruv_size; i++) {
                    new_ruv[i] = ruv[i];
                }
                free(ruv);
                ruv = new_ruv;
            }
            ruv[ruv_size ++] = unit;
        }
    } else {
        RecordUnit *unit = ruv[ruv_index];
        if (mode == 0) {
            unit -> number = number;
        } else if (mode > 0) {
            unit -> number += number;
        } else {
            unit -> number -= number;
        }

        if (unit -> number > item -> stock) {
            unit -> number = item -> stock;
        }
        if (unit -> number <= 0) {
            delete_ru(ruv_index);
        }

    }
}

void delete_ru(const int index) {
    if (index >= 0 && index < ruv_size) {
        free(ruv[index] -> name);
        free(ruv[index]);
        for (int i = index; i < ruv_size - 1; i++) {
            ruv[i] = ruv[i + 1];
        }
        ruv_size --;
    }
}

void delete_all_ru() {
    for (int i = 0; i < ruv_size; i++) {
        free(ruv[i] -> name);
        free(ruv[i]);
    }
    ruv_size = 0;
}

int get_ru_number() {
    return ruv_size;
}

RecordUnit *get_ru(const int index) {
    if (index >= 0 && index < ruv_size) {
        return ruv[index];
    }
    return NULL;
}



