//
// Created by HZQ on 2026/9/13.
//

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <process.h>

#include "core_ui.h"


#define INITIAL_CAPACITY                16

#define MAX_INPUT_STRING_LENGTH         256

static int vector_size = 0;
static int vector_capacity = 0;
static const Component **components;
static Component *focused_component;

static char ansi_color_string[32];

static char time_string[32];
static time_t current_time;

static HANDLE handle_0;
static HANDLE handle_1;
static HANDLE current_handle;
static COORD origin_coord = {0, 0};
static COORD cursor_coord = {0, 0};

void init_console() {
    // UTF_8输出，用来避免乱码
    SetConsoleOutputCP(CP_UTF8);

    // 获取句柄
    handle_0 = GetStdHandle(STD_OUTPUT_HANDLE);

    // 开启虚拟终端处理，用来实现清屏、文字颜色等效果
    DWORD handel_mode = 0;
    if (GetConsoleMode(handle_0, &handel_mode)) {
        SetConsoleMode(handle_0, handel_mode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING & ~ENABLE_WRAP_AT_EOL_OUTPUT & ~ENABLE_LINE_INPUT);
    }

    // 设置双缓冲防止闪烁
    handle_1 = CreateConsoleScreenBuffer(
        GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );

    // 另一个缓冲也要设置虚拟终端处理
    if (GetConsoleMode(handle_1, &handel_mode)) {
        SetConsoleMode(handle_1, handel_mode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING & ~ENABLE_WRAP_AT_EOL_OUTPUT & ~ENABLE_LINE_INPUT);
    }
    SetConsoleActiveScreenBuffer(handle_1);

    current_handle = handle_1;
}

// 此函数不稳定，不建议使用
void set_console_size(const short width, const short height) {
    const COORD size = {width, height};
    SetConsoleScreenBufferSize(handle_0, size);
    SetConsoleScreenBufferSize(handle_1, size);
    const SMALL_RECT window_bounds = {0, 0, (short) (width - 1), (short) (height - 1)};
    SetConsoleWindowInfo(handle_0, TRUE, &window_bounds);
    SetConsoleWindowInfo(handle_1, TRUE, &window_bounds);
}

void set_cursor_coord(const short x, const short y) {
    const COORD coord = {x, y};
    cursor_coord = coord;
    SetConsoleCursorPosition(handle_0, coord);
    SetConsoleCursorPosition(handle_1, coord);
}

void add_component(const Component *c) {
    if (vector_capacity == 0) {
        vector_capacity = INITIAL_CAPACITY;
        components = malloc(sizeof(Component *) * vector_capacity);
    }

    if (c != NULL && get_component_index(c) == -1) {
        if (vector_size == vector_capacity) {
            const Component **new_components = malloc(sizeof(Component *) * (vector_capacity << 1));
            for (int i = 0; i < vector_capacity; i++) {
                new_components[i] = components[i];
            }
            free(components);
            components = new_components;
            vector_capacity = vector_capacity << 1;
        }
        components[vector_size ++] = c;
    }
}

void set_focused_component(Component *c) {
    if (c != NULL) {
        focused_component = c;
    }
}

int get_component_index(const Component *c) {
    if (c != NULL) {
        for (int i = 0; i < vector_size; i++) {
            // 编译器说这里可能有空指针问题，但是我没查出来，暂且先注释掉警告
            // ReSharper disable once CppDFANullDereference
            if (components[i] == c) {
                return i;
            }
        }
    }
    return -1;
}

void remove_component(const Component *c) {
    remove_component_with_index(get_component_index(c));
}

void remove_component_with_index(const int index) {
    if (index >= 0 && index < vector_size) {
        for (int i = index; i < vector_size - 1; i++) {
            components[i] = components[i + 1];
        }
        vector_size --;
    }
}

void remove_all_components() {
    vector_size = 0;
}

void set_location(Component *c, const int x, const int y) {
    const COORD new_coord = {x, y};
    c -> coord = new_coord;
}

void set_size(Component *c, const int width, const int height) {
    c -> width = width < CORE_UI_MINIMAL_TEXT_WIDTH ? CORE_UI_MINIMAL_TEXT_WIDTH : width;
    c -> height = height;
}

void set_box_choose(Component *c, const int index) {
    if (c -> type == CORE_UI_CHOOSE_BOX) {
        c -> parameters[5] = index;
    }
}

// 创建标签
Component create_label(MultilanguageText *text, const short x, const short y, const short width, const short height, const int color) {
    short w;
    if (width < CORE_UI_MINIMAL_TEXT_WIDTH) {
        w = CORE_UI_MINIMAL_TEXT_WIDTH;
    } else {
        w = width;
    }
    const Component c = {CORE_UI_LABEL, 0, x, y, w, height, color, {0, 0, 0, 0, 0, 0, 0, 0}, 1, text};
    return c;
}

//创建多选框
Component create_choose_box(MultilanguageText *text, const int number, const short x, const short y, const short column, const short row, const short grid_width, const short grid_height, const int color) {
    short w;
    if (grid_width < CORE_UI_MINIMAL_TEXT_WIDTH) {
        w = CORE_UI_MINIMAL_TEXT_WIDTH;
    } else {
        w = grid_width;
    }
    const Component c = {CORE_UI_CHOOSE_BOX, CORE_UI_FOCUSABLE, x, y, (short) (w * column), (short) (grid_height * row), color, {column, row, w, grid_height, 0, 0, 0, 0}, number, text};
    return c;
}

Component creat_clock(const short x, const short y, const int color) {
    const Component c = {CORE_UI_CLOCK, 0, x, y, 32, 1, color, {0, 0, 0, 0, 0, 0, 0, 0}, 0};
    return c;
}

void clean_console() {
    WriteConsole(current_handle, CLEAN_UP_CONSOLE, strlen(CLEAN_UP_CONSOLE), NULL, NULL);
}

// 刷新函数，每帧调用一次
void refresh_console() {
    clean_console();
    refresh_time();
    SetConsoleCursorPosition(current_handle, origin_coord);

    for (int i = 0; i < vector_size; i++) {
        draw_component(components[i]);
    }

    if (current_handle == handle_0) {
        SetConsoleActiveScreenBuffer(handle_0);
        current_handle = handle_1;
    } else {
        SetConsoleActiveScreenBuffer(handle_1);
        current_handle = handle_0;
    }

    if (focused_component != NULL) {
        switch (focused_component -> type) {
            case CORE_UI_CHOOSE_BOX: {
                // 多选框
                if (_kbhit()) {
                    const int key = _getch();
                    switch (key) {
                        case UP_ARROW_4711:
                        case LEFT_ARROW_4711:
                            focused_component -> parameters[5] = (focused_component -> parameters[5] + focused_component -> texts_number - 1) % focused_component -> texts_number;
                            break;
                        case DOWN_ARROW_4711:
                        case RIGHT_ARROW_4711:
                            focused_component -> parameters[5] = (focused_component -> parameters[5] + 1) % focused_component -> texts_number;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
}

// 更新时间字符串
void refresh_time() {
    current_time = time(NULL);
    strftime(time_string, sizeof(time_string), "%Y / %m / %d    %H : %M : %S", localtime(&current_time));
}

void draw_component(const Component *c) {
    if (c != NULL) {
        COORD coord = c -> coord;
        short remaining_height = c -> height;
        switch (c -> type) {
            case CORE_UI_LABEL:
                draw_multilanguage_text(c -> texts[0], coord.X, coord.Y, c -> width, c -> height, c -> color);
                break;
            case CORE_UI_CHOOSE_BOX:
                for (int i = 0, column = 0, row = 0; i < c -> texts_number; i++) {
                    if (i == c -> parameters[5]) {
                        draw_multilanguage_text(c -> texts[i], (short) (coord.X + column * c -> parameters[2]), (short) (coord.Y + row * c -> parameters[3]), (short) c -> parameters[2], (short) c -> parameters[3], c -> color);
                    } else {
                        draw_multilanguage_text(c -> texts[i], (short) (coord.X + column * c -> parameters[2]), (short) (coord.Y + row * c -> parameters[3]), (short) c -> parameters[2], (short) c -> parameters[3], FOREGROUND_COLOR);
                    }
                    column ++;
                    if (column == c -> parameters[0]) {
                        column = 0;
                        row ++;
                    }
                }
                break;
            case CORE_UI_CLOCK: {
                draw_text(time_string, coord.X, coord.Y, c -> width, c -> height, c -> color);
            }
            default:
            case CORE_UI_UNKNOWN: {
                for (int i = 0; i < c -> texts_number; i++) {
                    const int h = (short) draw_multilanguage_text(c -> texts[i], coord.X, coord.Y, c -> width, remaining_height, c -> color);
                    coord.Y += h; // NOLINT(*-narrowing-conversions)
                    remaining_height -= h; // NOLINT(*-narrowing-conversions)
                }
                break;
            }
        }
    }
}

int draw_multilanguage_text(const MultilanguageText text, const short x, const short y, const short width, const short height, const int color) {
    int h = 0;
    switch (get_language()) {
        case 1:
            h += draw_text(text.en_US, x, y, width, height, color);
            break;
        case 2:
            h += draw_text(text.zh_CN, x, y, width, height, color);
            break;
        default:
            h += draw_text(text.en_US, x, y, width, height, color);
            h += draw_text(text.zh_CN, x, (short) (y + h), width, (short) (height - h), color);
            break;
    }
    return h;
}

int draw_text(const char *text, const short x, const short y, const short width, const short height, const int color) {
    COORD coord = {x, y};
    const char *s = text;
    int output_height = 0;
    set_color(color);
    for (short i = y; i < y + height; i ++, coord.Y ++, output_height ++) {
        if (!s[0]) {
            break;
        }
        if (
            s[0] == '\n'          ||
            s[0] == '\r'
        ) {
            // 换行
            output_height ++;
            s ++;
        } else if (
            s[0] == '\t'
        ) {
            // 制表，功能暂不实现，仅跳过
            s ++;
            i --, coord.Y --, output_height --;
        } else if (
            s[0] == ' '
        ) {
            // 行头空格，跳过
            s ++;
            i --, coord.Y --, output_height --;
        } else {
           const int line_length = get_line_length(s, width);
           SetConsoleCursorPosition(current_handle, coord);
           WriteConsole(current_handle, s, line_length, NULL, NULL);
           s += line_length;
        }
    }
    return output_height;
}

// 计算下一行要输出多少个char，自动处理宽字符和ANSI代码
int get_line_length(const char *text, const short width) {
    const char *s = text;
    int remaining_width = width;
    int length = 0;
    while (s[0]) {
        if (s[0] == '\033') {
            // ANSI序列
            s ++;
            while (s[0]) {
                if (
                    s[0] == 'h'  ||         // 隐藏光标
                    s[0] == 'l'  ||         // 显示光标
                    s[0] == 'm'  ||         // 颜色
                    s[0] == 'A'  ||         // 光标上移
                    s[0] == 'B'  ||         // 光标下移
                    s[0] == 'C'  ||         // 光标右移
                    s[0] == 'D'  ||         // 光标左移
                    s[0] == 'J'  ||         // 清屏
                    s[0] == 'K'             // 清除一行
                ) {
                    break;
                }
                s ++;
                length ++;
            }
        } else {
            const COORD lw = get_next_word_length_and_width(s);
            if (lw.Y <= remaining_width) {
                length += lw.X;
                remaining_width -= lw.Y;
                s += lw.X;
            } else {
                if (lw.Y > width) {
                    while (s[0]) {
                        const UnicodeCharacter uc = get_next_utf_8(s);
                        if (uc.is_full_width ? 2 : 1 <= remaining_width) {
                            length += uc.length;
                            remaining_width -= uc.is_full_width ? 2 : 1;
                            s += uc.length;
                        } else {
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
    return length;
}

// 计算下一个词块有多少个char和其宽度
COORD get_next_word_length_and_width(const char *text) {
    const char *s = text;
    UnicodeCharacter uc = get_next_utf_8(s);
    const short area = uc.area;
    if (
        area == UNKNOWN_CHAR_4711       ||
        area == CJK_CHAR_4711           ||
        area == PUNCTUATION_CHAR_4711   ||
        area == EMOJI_CHAR_4711
    ) {
        const COORD lw = {uc.length, uc.is_full_width ? 2 : 1};
        return lw;
    }
    int length = 0;
    int width = 0;
    while (s[0]) {
        if (
            uc.unicode == '\0'     ||
            uc.area != area
        ) {
            break;
        }
        length += uc.length;
        width += uc.is_full_width ? 2 : 1;
        s += uc.length;
        uc = get_next_utf_8(s);
    }
    const COORD lw = {(short) length, (short) width};
    return lw;
}

// 计算下一个utf_8字符的Unicode码
UnicodeCharacter get_next_utf_8(const char *text) {
    const unsigned char *s = (unsigned char *) text;
    unsigned char c = s[0];
    unsigned int unicode = 0;
    char length;
    if ((c & 0xC0) == 0xC0) {
        if ((c & 0xE0) == 0xE0) {
            if ((c & 0xF0) == 0xF0) {
                if ((c & 0xF8) == 0xF8) {
                    if ((c & 0xFC) == 0xFC) {
                        // 6字节
                        length = 6;
                        unicode = c & 0x1;
                        for (int i = 1; i < 6; i++) {
                            unicode = unicode << 6;
                            c = s[i];
                            unicode |= c & 0x3F;
                        }
                    } else {
                        // 5字节
                        length = 5;
                        unicode = c & 0x3;
                        for (int i = 1; i < 5; i++) {
                            unicode = unicode << 6;
                            c = s[i];
                            unicode |= c & 0x3F;
                        }
                    }
                } else {
                    // 4字节
                    length = 4;
                    unicode = c & 0x7;
                    for (int i = 1; i < 4; i++) {
                        unicode = unicode << 6;
                        c = s[i];
                        unicode |= c & 0x3F;
                    }
                }
            } else {
                // 3字节
                length = 3;
                unicode = c & 0xF;
                for (int i = 1; i < 3; i++) {
                    unicode = unicode << 6;
                    c = s[i];
                    unicode |= c & 0x3F;
                }
            }
        } else {
            // 2字节
            length = 2;
            unicode = c & 0x1F;
            unicode = unicode << 6;
            c = s[1];
            unicode |= c & 0x3F;
        }
    } else {
        // 1字节
        length = 1;
        unicode = c;
    }

    const UnicodeCharacter uc = {unicode, get_area(unicode), is_full_width(unicode), length};
    return uc;
}

// 判断一个Unicode码的全半角
char is_full_width(const unsigned int unicode) {
    return (char) (
        (unicode >= 0x1100 && unicode <= 0x115F)  ||                    // 韩文字母
        (unicode >= 0x2E80 && unicode <= 0x303E)  ||                    // 中日韩部首
        (unicode >= 0x3041 && unicode <= 0x33FF)  ||                    // 平假名、片假名、注音
        (unicode >= 0x3400 && unicode <= 0x4DBF)  ||                    // CJK扩展A
        (unicode >= 0x4E00 && unicode <= 0x9FFF)  ||                    // CJK统一表意
        (unicode >= 0xA000 && unicode <= 0xA4CF)  ||                    // 彝文
        (unicode >= 0xAC00 && unicode <= 0xD7A3)  ||                    // 韩文音节
        (unicode >= 0xF900 && unicode <= 0xFAFF)  ||                    // CJK兼容
        (unicode >= 0xFE30 && unicode <= 0xFE6F)  ||                    // CJK兼容形式
        (unicode >= 0xFF00 && unicode <= 0xFF60)  ||                    // 全角ASCII
        (unicode >= 0xFFE0 && unicode <= 0xFFE6)  ||                    // 全角符号
        (unicode >= 0x1F300 && unicode <= 0x1F64F) ||                   // emoji
        (unicode >= 0x1F900 && unicode <= 0x1F9FF) ||                   // 补充emoji
        (unicode >= 0x20000 && unicode <= 0x3FFFD)                      // CJK扩展B+
    );
}

// 判断一个Unicode码所属的区域
short get_area(const unsigned int unicode) {
    if (
        (unicode >= 0x0041 && unicode <= 0x005A)  ||                    // 大写字母
        (unicode >= 0x0061 && unicode <= 0x007A)  ||                    // 小写字母
        (unicode >= 0x0080 && unicode <= 0x024F)                        // 拉丁字母补充/扩展
    ) {
        return LATIN_CHAR_4711;
    }
    if (
        unicode >= 0x0370 && unicode <= 0x03FF                          // 希腊和科普特字母
    ) {
        return GREEK_COPTIC_CHAR_4711;
    }
    if (
        unicode >= 0x0400 && unicode <= 0x052F                          // 西里尔字母及补充
    ) {
        return CYRIL_CHAR_4711;
    }
    if (
        unicode >= 0x0530 && unicode <= 0x058F                          // 亚美尼亚字母
    ) {
        return ARMENIAN_CHAR_4711;
    }
    if (
        unicode >= 0x0590 && unicode <= 0x05FF                          // 希伯来字母
    ) {
        return HEBREW_CHAR_4711;
    }
    if (
        unicode >= 0x0600 && unicode <= 0x06FF                          // 阿拉伯字母
    ) {
        return ARABIC_CHAR_4711;
    }
    if (
        (unicode >= 0x1100 && unicode <= 0x115F)  ||                    // 韩文字母
        (unicode >= 0x2E80 && unicode <= 0x303E)  ||                    // 中日韩部首
        (unicode >= 0x3041 && unicode <= 0x33FF)  ||                    // 平假名、片假名、注音
        (unicode >= 0x3400 && unicode <= 0x4DBF)  ||                    // CJK扩展A
        (unicode >= 0x4E00 && unicode <= 0x9FFF)  ||                    // CJK统一表意
        (unicode >= 0xF900 && unicode <= 0xFAFF)  ||                    // CJK兼容
        (unicode >= 0xFE30 && unicode <= 0xFE6F)  ||                    // CJK兼容形式
        (unicode >= 0x20000 && unicode <= 0x3FFFD)                      // CJK扩展B+
    ) {
        return CJK_CHAR_4711;
    }
    if (
        (unicode >= 0x0020 && unicode <= 0x002F)  ||                    // 一些符号
        (unicode >= 0x003A && unicode <= 0x0040)  ||                    // 一些符号
        (unicode >= 0x005B && unicode <= 0x0060)  ||                    // 一些符号
        (unicode >= 0x007B && unicode <= 0x007E)  ||                    // 一些符号
        (unicode >= 0x2000 && unicode <= 0x206F)  ||                    // 常用符号
        (unicode >= 0x3000 && unicode <= 0x303F)                        // 中日韩标点符号
    ) {
        return PUNCTUATION_CHAR_4711;
    }
    if (
        unicode >= 0x0030 && unicode <= 0x0039                          // 数字
    ) {
        return NUMBER_CHAR_4711;
    }
    if (
        (unicode >= 0x1F300 && unicode <= 0x1F64F) ||                   // emoji
        (unicode >= 0x1F900 && unicode <= 0x1F9FF)                      // 补充emoji
    ) {
        return EMOJI_CHAR_4711;
    }
    return UNKNOWN_CHAR_4711;
}

// 设置输出颜色
void set_color(const unsigned int color) {
    // 这边后来发现可以用stdio.h里的方法简化，不过性能上可能这个还要好一点
    const unsigned int r = color >> 16 & 0xFF;
    const unsigned int g = color >> 8 & 0xFF;
    const unsigned int b = color & 0xFF;
    int rl;
    if (r >= 100) {
        rl = 3;
    } else if (r >= 10) {
        rl = 2;
    } else {
        rl = 1;
    }
    int gl;
    if (g >= 100) {
        gl = 3;
    } else if (g >= 10) {
        gl = 2;
    } else {
        gl = 1;
    }
    int bl;
    if (b >= 100) {
        bl = 3;
    } else if (b >= 10) {
        bl = 2;
    } else {
        bl = 1;
    }
    const int length = rl + gl + bl + 11;
    char *s = ansi_color_string;
    int i = 0;
    s[i ++] = '\033';
    s[i ++] = '[';
    s[i ++] = '3';
    s[i ++] = '8';
    s[i ++] = ';';
    s[i ++] = '2';
    s[i ++] = ';';
    if (rl == 3) {
        s[i ++] = (char) ('0' + r / 100 % 10);
        s[i ++] = (char) ('0' + r / 10 % 10);
        s[i ++] = (char) ('0' + r % 10);
    } else if (rl == 2) {
        s[i ++] = (char) ('0' + r / 10 % 10);
        s[i ++] = (char) ('0' + r % 10);
    } else {
        s[i ++] = (char) ('0' + r % 10);
    }
    s[i ++] = ';';
    if (gl == 3) {
        s[i ++] = (char) ('0' + g / 100 % 10);
        s[i ++] = (char) ('0' + g / 10 % 10);
        s[i ++] = (char) ('0' + g % 10);
    } else if (gl == 2) {
        s[i ++] = (char) ('0' + g / 10 % 10);
        s[i ++] = (char) ('0' + g % 10);
    } else {
        s[i ++] = (char) ('0' + g % 10);
    }
    s[i ++] = ';';
    if (bl == 3) {
        s[i ++] = (char) ('0' + b / 100 % 10);
        s[i ++] = (char) ('0' + b / 10 % 10);
        s[i ++] = (char) ('0' + b % 10);
    } else if (bl == 2) {
        s[i ++] = (char) ('0' + b / 10 % 10);
        s[i ++] = (char) ('0' + b % 10);
    } else {
        s[i ++] = (char) ('0' + b % 10);
    }
    s[i ++] = 'm';
    s[i] = '\0';
    WriteConsole(current_handle, s, length, NULL, NULL);
}

void set_to_default_color() {
    WriteConsole(current_handle, DEFAULT_COLOR_ANSI, strlen(DEFAULT_COLOR_ANSI), NULL, NULL);
}












