//
// Created by HZQ on 2026/9/13.
//

#ifndef SYSTEM_4711_CORE_UI_H
#define SYSTEM_4711_CORE_UI_H

#include <windows.h>

#include "language.h"

#define CORE_UI_UNKNOWN             0
#define CORE_UI_LABEL               1
#define CORE_UI_CHOOSE_BOX          2
#define CORE_UI_CLOCK               3

#define CORE_UI_CONSOLE_WIDTH       256
#define CORE_UI_CONSOLE_HEIGHT      144
#define CORE_UI_MINIMAL_TEXT_WIDTH  8

#define CORE_UI_FOCUSABLE           0x00000001
#define CORE_UI_BORD                0x00000002

#define UP_ARROW_4711               72
#define DOWN_ARROW_4711             80
#define LEFT_ARROW_4711             75
#define RIGHT_ARROW_4711            77
#define ENTER_4711                  13

#define CLEAN_UP_CONSOLE            "\033[2J"
#define DEFAULT_COLOR_ANSI          "\033[0m"
#define FOREGROUND_COLOR            0xFFFFFF
#define SELECTED_COLOR              0xB040A0

#define UNKNOWN_CHAR_4711           0
#define LATIN_CHAR_4711             1
#define GREEK_COPTIC_CHAR_4711      2
#define CYRIL_CHAR_4711             3
#define ARMENIAN_CHAR_4711          4
#define HEBREW_CHAR_4711            5
#define ARABIC_CHAR_4711            6
#define CJK_CHAR_4711               7
#define PUNCTUATION_CHAR_4711       100
#define NUMBER_CHAR_4711            101
#define EMOJI_CHAR_4711             102

typedef struct Component {
    int type;
    int mode;
    COORD coord;
    short width;
    short height;
    int color;
    int parameters[8];
    int texts_number;
    MultilanguageText *texts;
    void *image;
    void (*call_back) (int state);
} Component;

typedef struct UnicodeCharacter {
    unsigned int unicode;
    short area;                     // 指语言种类，比如中文、emoji
    char is_full_width;             // 全半角
    char length;                    // 指utf_8编码下的字节数
} UnicodeCharacter;

void init_console();

void set_console_size(short width, short height);

void set_cursor_coord(short x, short y);

void add_component(const Component *c);

void set_focused_component(Component *c);

int get_component_index(const Component *c);

void remove_component(const Component *c);

void remove_component_with_index(int index);

void remove_all_components();

Component create_label(MultilanguageText *text, short x, short y, short width, short height, int color);

Component create_choose_box(MultilanguageText *text, int number, short x, short y, short column, short row, short grid_width, short grid_height, int color);

Component creat_clock(short x, short y, int color);

void clean_console();

void refresh_console();

void refresh_time();

void draw_component(const Component *c);

int draw_multilanguage_text(MultilanguageText text, short x, short y, short width, short height, int color);

int draw_text(const char *text, short x, short y, short width, short height, int color);

int get_line_length(const char *text, short width);

COORD get_next_word_length_and_width(const char *text);

UnicodeCharacter get_next_utf_8(const char *text);

char is_full_width(unsigned int unicode);

short get_area(unsigned int unicode);

void set_color(unsigned int color);

void set_to_default_color();

#endif //SYSTEM_4711_CORE_UI_H
