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
#define CORE_UI_DEBUG_PANEL         4
#define CORE_UI_FPS_PANEL           5
#define CORE_UI_INPUT_BOX           6
#define CORE_UI_NOTICE_PANEL        7

#define CORE_UI_DB_CHECKOUT_PANEL   100
#define CORE_UI_DB_STOCK_PANEL      101
#define CORE_UI_DB_PRICE_PANEL      102

#define CORE_UI_CONSOLE_WIDTH       1024
#define CORE_UI_CONSOLE_HEIGHT      576
#define CORE_UI_MINIMAL_TEXT_WIDTH  8

#define CONTROL_INPUT               0               // 英文输入转按键
#define LINE_INPUT                  1               // 回车转按键
#define STRING_INPUT                2               // 除控制符外均使用文本，由于鼠标输入较为复杂暂未引入，此状态又无法通过回车退出，故几乎不用

#define CORE_UI_EAST                0
#define CORE_UI_NORTH               1
#define CORE_UI_WEST                2
#define CORE_UI_SOUTH               3


#define CORE_UI_FOCUSABLE           0x00000001
#define CORE_UI_BORD                0x00000006

#define CLEAN_UP_CONSOLE            "\033[2J"
#define DEFAULT_COLOR_ANSI          "\033[0m"
#define SELECTED_COLOR_ANSI         "\033[38;2;176;64;160m"
#define FOREGROUND_COLOR            0xFFFFFF
#define SELECTED_COLOR              0xB040A0
#define DISABLED_COLOR              0x808080

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
    char *input;
    char *image;
    void (*call_back) (int state);
} Component;

void init_console();

void exit_console();

void clear_debug();

void print_debug(const char *text, int length, char ln);

void clear_notice();

void print_notice(MultilanguageText text, char ln);

void set_input_mode(char mode);

char is_key_pressed(int key);

void set_console_size(short width, short height);

void set_cursor_coord(short x, short y);

void fix_cursor_coord();

void hide_cursor();

void show_cursor();

void add_component(Component *c);

void set_focused_component(Component *c);

void throw_focus(const Component *c);

void move_focus(const Component *c, int direction);

int get_component_index(const Component *c);

void remove_component(const Component *c);

void remove_component_with_index(int index);

void remove_all_components();

void set_location(Component *c, int x, int y);

void set_size(Component *c, int width, int height);

void set_box_choose(Component *c, int index);

void set_box_input(Component *c, const char *text);

void set_call_back(Component *c, void (*callback) (int state));

Component create_label(MultilanguageText *text, short x, short y, short width, short height, int color);

Component create_choose_box(MultilanguageText *text, int number, short x, short y, short column, short row, short grid_width, short grid_height, int color);

Component create_clock(short x, short y, int color);

Component create_debug_panel(short x, short y, int color);

Component create_fps_panel(short x, short y, int color);

Component create_input_box(short x, short y, short width, short height, int length, int color);

Component create_notice_panel(short x, short y, int color);

Component create_db_checkout_panel(short x, short y, int color);

Component create_db_stock_panel(short x, short y, int color);

Component create_db_price_panel(short x, short y, int color);

void clear_console();

void refresh_console();

void refresh_time();

void refresh_fps();

void refresh_window_info();

void refresh_input();

void draw_component(Component *c);

int draw_multilanguage_text(MultilanguageText text, short x, short y, short width, short height, int color, int *skip);

int draw_text(const char *text, short x, short y, short width, short height, int color, int *skip);

int get_line_length(const char *text, short width);

COORD get_next_word_length_and_width(const char *text);

void set_color(unsigned int color);

void set_to_default_color();



#endif //SYSTEM_4711_CORE_UI_H
