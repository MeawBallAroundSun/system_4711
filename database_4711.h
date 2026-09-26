//
// Created by HZQ on 2026/9/12.
//

#ifndef SYSTEM_4711_DATABASE_4711_H
#define SYSTEM_4711_DATABASE_4711_H

#define DB_FINE                 0
#define DB_ERROR                1

#define DB_ERR_FOLDER_ROOT      0x01
#define DB_ERR_FOLDER_DATA      0x02
#define DB_ERR_FOLDER_LOGS      0x04
#define DB_ERR_ACCOUNT          0x08
#define DB_ERR_ITEM             0x10

#define DB_ACCOUNT_PATH         "C:\\ProgramData\\system_4711\\data\\accounts.txt"
#define DB_ITEM_PATH            "C:\\ProgramData\\system_4711\\data\\items.txt"
#define DB_RECORD_PATH          "C:\\ProgramData\\system_4711\\logs\\"

#include <time.h>
#include <windows.h>
#include <stdio.h>



typedef struct Account {
    char *name;
    char *password;
    char is_administrator;
} Account;

typedef struct Item {
    int id;
    char *name;
    int stock;
    int price;                      // 价格的计量单位均使用分，即1元的price会记作100。
} Item;

typedef struct RecordUnit {
    int id;
    char *name;
    int price;                      // 记录时时单价
    int number;
} RecordUnit;

typedef struct Record {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int length;
    RecordUnit *units;
} Record;



int input_command(const char *command);

int command_pick(const char *key_word, const char *number);
int command_checkout();
int command_search(const char *key_word);
int command_clear();
int command_set_price(const char *key_word, const char *price);
int command_sales(const char *year, const char *month, const char *day);
int command_set_stock(const char *key_word, const char *number);
int command_add_item(const char *id, const char *name, const char *price);
int command_delete_item(const char *key_word);
int command_login(const char *name, const char *password);
int command_logout();
int command_create_account(const char *name, const char *password, const char *administrator);

void set_command_output(void (* output) (const char *));
void set_command_time(const char *(* time_f) (void));

int get_sales();
Account *get_current_account();

int init_database();

int init_folders();

int ensure_folder(const char *path);


int init_accounts();
void add_account(Account *account);
void save_account();
int get_account_number();
Account *get_account(int index);


int init_items();
void add_item(Item *item);
void delete_item(const Item *item);
void save_item();
int get_item_number();
Item *get_item(int index);


int init_records(const struct tm *time);
void add_record(Record *record);
void save_record(const struct tm *time);
int get_record_number();
Record *get_record(int index);


void add_ru(const Item *item, int number, int mode);
void delete_ru(int index);
void delete_all_ru();
int get_ru_number();
RecordUnit *get_ru(int index);


#endif //SYSTEM_4711_DATABASE_4711_H
