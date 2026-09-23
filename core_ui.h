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

#define CORE_UI_CONSOLE_WIDTH       256
#define CORE_UI_CONSOLE_HEIGHT      144
#define CORE_UI_MINIMAL_TEXT_WIDTH  8

#define CONTROL_INPUT               0               // 英文输入转按键
#define LINE_INPUT                  1               // 回车转按键
#define STRING_INPUT                2               // 除控制符外均使用文本，由于鼠标输入较为复杂暂未引入，此状态又无法通过回车退出，故几乎不用

#define CORE_UI_FOCUSABLE           0x00000001
#define CORE_UI_BORD                0x00000006

#define CLEAN_UP_CONSOLE            "\033[2J"
#define DEFAULT_COLOR_ANSI          "\033[0m"
#define FOREGROUND_COLOR            0xFFFFFF
#define SELECTED_COLOR              0xB040A0

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
    char *image;
    void (*call_back) (int state);
} Component;

void init_console();

void exit_console();

void clear_debug();

void print_debug(const char *text, int length, char ln);

void set_input_mode(char mode);

char is_key_pressed(int key);

void set_console_size(short width, short height);

void set_cursor_coord(short x, short y);

void add_component(const Component *c);

void set_focused_component(Component *c);

int get_component_index(const Component *c);

void remove_component(const Component *c);

void remove_component_with_index(int index);

void remove_all_components();

void set_location(Component *c, int x, int y);

void set_size(Component *c, int width, int height);

void set_box_choose(Component *c, int index);

Component create_label(MultilanguageText *text, short x, short y, short width, short height, int color);

// ChooseBox组件参数：列数，行数，列宽，行高，当前选中的索引数，当前页数，总页数
Component create_choose_box(MultilanguageText *text, int number, short x, short y, short column, short row, short grid_width, short grid_height, int color);

Component create_clock(short x, short y, int color);

// DebugPanel组件参数：当前行数，总行数
Component create_debug_panel(short x, short y, int color);

Component create_fps_panel(short x, short y, int color);

void clear_console();

void refresh_console();

void refresh_time();

void refresh_fps();

void draw_component(const Component *c);

int draw_multilanguage_text(MultilanguageText text, short x, short y, short width, short height, int color);

int draw_text(const char *text, short x, short y, short width, short height, int color);

int get_line_length(const char *text, short width);

COORD get_next_word_length_and_width(const char *text);

void set_color(unsigned int color);

void set_to_default_color();



#endif //SYSTEM_4711_CORE_UI_H
