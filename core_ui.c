//
// Created by HZQ on 2026/9/13.
//

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <process.h>

#include "database_4711.h"
#include "core_ui.h"

#include "assets.h"
#include "core_char.h"
#include "core_buffer.h"


#define INITIAL_CAPACITY                    16


#define MAX_DEBUG_TEXT_LENGTH               4096
#define MAX_INPUT_TEXT_LENGTH               4096
#define MAX_NOTICE_TEXT_LENGTH              4096

// 组件的vector变长数组
static int vector_size = 0;
static int vector_capacity = 0;
static Component **components;
static Component *focused_component;

// ANSI颜色代码
static char ansi_color_string[32];

// 时间相关
static char time_string[32];
static time_t current_time;

// 帧率相关
static char fps_string[16];
static int fps;
static int frame_count;
static int last_second;

// 调试文本缓冲区
static Buffer debug_buffer;
static char debug_text[MAX_DEBUG_TEXT_LENGTH];

// 输入缓冲区
static Buffer input_buffer;
static char input_text[MAX_INPUT_TEXT_LENGTH];
static char input_mode;
static char input_box_text_buffer[MAX_INPUT_TEXT_LENGTH];

// 通知文本缓冲区
static Buffer notice_buffer;
static char notice_text[MAX_NOTICE_TEXT_LENGTH];

// 输入线程相关
static HANDLE input_handle;
static HANDLE thread_handle;
static volatile char thread_should_exit;

// 输出窗口句柄
static HANDLE handle_0;
static HANDLE handle_1;
static HANDLE current_handle;
static CONSOLE_CURSOR_INFO cursor_info;
static CONSOLE_SCREEN_BUFFER_INFO screen_info;

// 坐标
static COORD origin_coord = {0, 0};
static COORD cursor_coord = {0, 0};

// 窗口尺寸
static volatile int window_width;
static volatile int window_height;

// 键盘输入
static volatile char key_info_0[256];
static char key_info_1[256];

// 输入线程函数
// ReSharper disable once CppParameterMayBeConstPtrOrRef
static unsigned __stdcall input_thread_function(void *args) {
    (void) args;

    DWORD read_count = 0;

    while (!thread_should_exit) {
        // 这里似乎有极其微小的概率输入的utf_8会被意外截断，但实测对程序稳定性无影响
        ReadConsoleA(input_handle, input_text, MAX_INPUT_TEXT_LENGTH, &read_count, NULL);
        // 原地处理转义序列 （此段代码纯粹是枚举转义序列的常见情况，可读性差，总之能跑；另外没处理ctrl、shift、alt键的部分功能）
        char *w = input_text;
        for (int i = 0; i < read_count;) {
            char c;
            switch (c = input_text[i ++]) {
                case '\033':
                    // 退出键或转义序列
                    switch (c = input_text[i ++]) {
                        case 'O':
                            switch (input_text[i ++]) {
                                case 'P':   key_info_0[VK_F1]          = 1;        break;
                                case 'Q':   key_info_0[VK_F2]          = 1;        break;
                                case 'R':   key_info_0[VK_F3]          = 1;        break;
                                case 'S':   key_info_0[VK_F4]          = 1;        break;
                                default:    break;
                            }
                            break;

                        case '[':
                            switch (c = input_text[i ++]) {
                                case 'A':   key_info_0[VK_UP]          = 1;        break;
                                case 'B':   key_info_0[VK_DOWN]        = 1;        break;
                                case 'C':   key_info_0[VK_RIGHT]       = 1;        break;
                                case 'D':   key_info_0[VK_LEFT]        = 1;        break;
                                case 'H':   key_info_0[VK_HOME]        = 1;        break;
                                case 'F':   key_info_0[VK_END]         = 1;        break;

                                case '1':
                                    switch (input_text[i ++]) {
                                        case '5':   key_info_0[VK_F5]  = 1;        break;
                                        case '7':   key_info_0[VK_F6]  = 1;        break;
                                        case '8':   key_info_0[VK_F7]  = 1;        break;
                                        case '9':   key_info_0[VK_F8]  = 1;        break;
                                        case ';':
                                            // 跳过5，之后不跳过~符号
                                            switch (input_text[++ i]) {
                                                case 'A':   key_info_0[VK_UP]      = 1;        break;
                                                case 'B':   key_info_0[VK_DOWN]    = 1;        break;
                                                case 'C':   key_info_0[VK_RIGHT]   = 1;        break;
                                                case 'D':   key_info_0[VK_LEFT]    = 1;        break;
                                                default:    break;
                                            }
                                            key_info_0[VK_CONTROL]     = 1;
                                            break;
                                        default:    break;
                                    }
                                    i ++;   // 跳过~符号
                                    break;

                                case '2':
                                    switch (input_text[i ++]) {
                                        case '0':   key_info_0[VK_F9]  = 1;        break;
                                        case '1':   key_info_0[VK_F10] = 1;        break;
                                        case '3':   key_info_0[VK_F11] = 1;        break;
                                        case '4':   key_info_0[VK_F12] = 1;        break;
                                        default:    break;
                                    }
                                    i ++;   // 跳过~符号
                                    break;

                                default:
                                    key_info_0[VK_ESCAPE] = 1;
                                    *w = '[';
                                    w ++;
                                    *w = c;
                                    w ++;
                                    break;
                            }
                            break;

                        default:
                            key_info_0[VK_ESCAPE] = 1;
                            *w = c;
                            w ++;
                            break;
                    }
                    break;

                case '\r':
                case '\n':
                    // 回车
                    if (input_mode == STRING_INPUT) {
                        *w = '\n';
                        w ++;
                    } else {
                        key_info_0[VK_RETURN] = 1;
                    }
                    break;

                case '\t':
                    // 制表
                    if (input_mode == STRING_INPUT) {
                        *w = '\t';
                        w ++;
                    } else {
                        key_info_0[VK_TAB] = 1;
                    }
                    break;

                case 0x7F:      key_info_0[VK_DELETE]      = 1;        break;      // 退格
                case 0x1A:      key_info_0[VK_PAUSE]       = 1;        break;      // 暂停

                default:
                    if (input_mode == CONTROL_INPUT) {
                        const short k = VkKeyScanA(c);
                        const char vk = LOBYTE(k);
                        const char state = HIBYTE(k);
                        if (vk != -1 && state != -1) {
                            key_info_0[vk] = 1;
                            switch (state) {
                                case 1:     key_info_0[VK_SHIFT]   = 1;    break;
                                case 2:     key_info_0[VK_CONTROL] = 1;    break;
                                case 3:     key_info_0[VK_MENU]    = 1;    break;
                                default:    break;
                            }
                        }
                    } else {
                        *w = c;
                        w ++;
                    }
                    break;
            }

        }
        *w = '\0';
        // 将处理后的纯文本流输入存入环形缓冲区
        write_buffer(&input_buffer, input_text, (int) read_count);
        print_debug(input_text, (int) read_count, 0);
    }
    return 0;
}

// 初始化控制台并启动输入线程
// 其实应该加返回值来检测初始化是否有问题的，但是暂时还没改
void init_console() {
    // UTF_8输入输出，用来避免乱码
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    // 获取句柄
    handle_0 = GetStdHandle(STD_OUTPUT_HANDLE);

    // 开启虚拟终端处理，用来实现清屏、文字颜色等效果
    DWORD handel_mode = 0;
    if (GetConsoleMode(handle_0, &handel_mode)) {
        SetConsoleMode(handle_0, handel_mode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
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
        SetConsoleMode(handle_1, handel_mode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    SetConsoleActiveScreenBuffer(handle_1);

    current_handle = handle_1;

    // 初始化缓冲区
    create_buffer(&debug_buffer, CORE_BUFFER_RING, MAX_DEBUG_TEXT_LENGTH);
    create_buffer(&input_buffer, CORE_BUFFER_RING, MAX_INPUT_TEXT_LENGTH);
    create_buffer(&notice_buffer, CORE_BUFFER_RING, MAX_NOTICE_TEXT_LENGTH);

    // 创建新线程处理输入
    input_handle = GetStdHandle(STD_INPUT_HANDLE);
    if (GetConsoleMode(input_handle, &handel_mode)) {
        SetConsoleMode(input_handle, (handel_mode | ENABLE_PROCESSED_INPUT | ENABLE_VIRTUAL_TERMINAL_INPUT) & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
    }

    thread_should_exit = 0;
    thread_handle = (HANDLE) _beginthreadex(NULL, 0, input_thread_function, NULL, 0, NULL);
    CloseHandle(thread_handle);

    input_mode = CONTROL_INPUT;
}

// 退出控制台
void exit_console() {
    // 关闭输入线程
    thread_should_exit = 1;

    // 清理缓冲区
    release_buffer(&debug_buffer);
    release_buffer(&input_buffer);
    release_buffer(&notice_buffer);

    // 其他的交给C运行时自动释放
}

// 清空调试区
void clear_debug() {
    clear_buffer(&debug_buffer);
}

// 打印调试字符，所有的debug_panel都会显示其中的内容
// 当输入过长时，环形缓冲区会自动覆盖旧内容
// 另外，覆盖之后不保证utf_8字符串不完整部分能正常显示，可能在开头部分出现乱码（比较简陋凑合用）（此问题后来在core_buffer中修复）
void print_debug(const char *text, const int length, const char ln) {
    write_buffer(&debug_buffer, text, length);

    if (ln) {
        const char c = '\n';
        write_buffer(&debug_buffer, &c, 1);
    }
}

// 清空消息区
void clear_notice() {
    clear_buffer(&notice_buffer);
}

// 打印消息
void print_notice(const MultilanguageText text, const char ln) {
    switch (get_language()) {
        case en_US_4711:
            write_buffer(&notice_buffer, text.en_US, MAX_NOTICE_TEXT_LENGTH);
            if (ln) {
                const char c = '\n';
                write_buffer(&notice_buffer, &c, 1);
            }
            break;
        case zh_CN_4711:
            write_buffer(&notice_buffer, text.zh_CN, MAX_NOTICE_TEXT_LENGTH);
            if (ln) {
                const char c = '\n';
                write_buffer(&notice_buffer, &c, 1);
            }
            break;
        default:
            write_buffer(&notice_buffer, text.en_US, MAX_NOTICE_TEXT_LENGTH);
            if (ln) {
                const char c = '\n';
                write_buffer(&notice_buffer, &c, 1);
            }
            write_buffer(&notice_buffer, text.zh_CN, MAX_NOTICE_TEXT_LENGTH);
            if (ln) {
                const char c = '\n';
                write_buffer(&notice_buffer, &c, 1);
            }
            break;
    }
}


// 修改输入模式
void set_input_mode(const char mode) {
    if (mode >= CONTROL_INPUT && mode <= STRING_INPUT) {
        input_mode = mode;
        if (mode == CONTROL_INPUT) {
            clear_buffer(&input_buffer);
        }
    } else {
        input_mode = CONTROL_INPUT;
    }
}

// 输入虚拟键码，查询是否按下(读取后会将按键状态设为未按下，防止单次输入多次读取)
char is_key_pressed(const int key) {
    if (key >= 0 && key < 256) {
        return key_info_1[key];
    }
    return 0;
}

// 此函数在当前Windows版本下不稳定，不建议使用
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

void fix_cursor_coord() {
    GetConsoleScreenBufferInfo(current_handle, &screen_info);
    cursor_coord = screen_info.dwCursorPosition;
    SetConsoleCursorPosition(handle_0, screen_info.dwCursorPosition);
    SetConsoleCursorPosition(handle_1, screen_info.dwCursorPosition);
}

void hide_cursor() {
    GetConsoleCursorInfo(handle_0, &cursor_info);
    cursor_info.bVisible = FALSE;
    SetConsoleCursorInfo(handle_0, &cursor_info);

    GetConsoleCursorInfo(handle_1, &cursor_info);
    cursor_info.bVisible = FALSE;
    SetConsoleCursorInfo(handle_1, &cursor_info);
}

void show_cursor() {
    GetConsoleCursorInfo(handle_0, &cursor_info);
    cursor_info.bVisible = TRUE;
    SetConsoleCursorInfo(handle_0, &cursor_info);

    GetConsoleCursorInfo(handle_1, &cursor_info);
    cursor_info.bVisible = TRUE;
    SetConsoleCursorInfo(handle_1, &cursor_info);
}

void add_component(Component *c) {
    if (vector_capacity == 0) {
        vector_capacity = INITIAL_CAPACITY;
        components = malloc(sizeof(Component *) * vector_capacity);
    }

    if (c != NULL && get_component_index(c) == -1) {
        if (vector_size == vector_capacity) {
            Component **new_components = malloc(sizeof(Component *) * (vector_capacity << 1));
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
    if (c == NULL) {
        focused_component = NULL;
        return;
    }
    if (c -> mode & CORE_UI_FOCUSABLE) {
        focused_component = c;
    }
}

// 若可能，传出焦点
void throw_focus(const Component *c) {
    if (c != NULL && focused_component == c) {
        for (int i = 0, j = (get_component_index(c) + 1) % vector_size; i < vector_size - 1; i ++, j = (j + 1) % vector_size) {
            if (components[j] -> mode & CORE_UI_FOCUSABLE) {
                set_focused_component((Component *) components[j]);
                break;
            }
        }
    }
}

// 获取某点处的组件
static Component *get_component_form_point(const short x, const short y, const short half_width, const short half_height, const Component *exclude) {
    for (int i = 0; i < vector_size; i ++) {
        if (components[i] == exclude) {
            continue;
        }
        if (!(components[i] -> mode & CORE_UI_FOCUSABLE)) {
            continue;
        }
        const COORD coord = components[i] -> coord;
        const short w = components[i] -> width;
        const short h = components[i] -> height;
        if (x + half_width >= coord.X && x - half_width <= coord.X + w && y + half_height >= coord.Y && y - half_height <= coord.Y + h) {
            return components[i];
        }
    }
    return NULL;
}

// 从某点向某个方向尝试移出焦点
static void move_focus_from_point(const short x, const short y, const int direction, const Component *exclude) {
    switch (direction) {
        case CORE_UI_EAST: {
            for (int i = x + 1, j = 0; i < window_width; i ++, j = j > 128 ? 128 : j + 1) {
                Component *c = get_component_form_point((short) i, y, 0, (short) j, exclude);
                if (c != NULL) {
                    set_focused_component(c);
                    return;
                }
            }
            break;
        }
        case CORE_UI_NORTH: {
            for (int i = y - 1, j = 0; i >= 0; i --, j = j > 128 ? 128 : j + 1) {
                const Component *c = get_component_form_point(x, (short) i, (short) j, 0, exclude);
                if (c != NULL) {
                    set_focused_component((Component *) c);
                    return;
                }
            }
            break;
        }
        case CORE_UI_WEST: {
            for (int i = x - 1, j = 0; i >= 0; i --, j = j > 128 ? 128 : j + 1) {
                const Component *c = get_component_form_point((short) i, y, 0, (short) j, exclude);
                if (c != NULL) {
                    set_focused_component((Component *) c);
                    return;
                }
            }
            break;
        }
        case CORE_UI_SOUTH: {
            for (int i = y + 1, j = 0; i < window_height; i ++, j = j > 128 ? 128 : j + 1) {
                const Component *c = get_component_form_point(x, (short) i, (short) j, 0, exclude);
                if (c != NULL) {
                    set_focused_component((Component *) c);
                    return;
                }
            }
            break;
        }
        default: break;
    }
}

// 向某个方向尝试移出焦点
void move_focus(const Component *c, const int direction) {
    switch (c -> type) {
        case CORE_UI_CHOOSE_BOX: {
            int x = c -> coord.X;
            int y = c -> coord.Y;
            const int cn = c -> parameters[0];
            const int rn = c -> parameters[1];
            const int tn = cn * rn;
            const int cw = c -> parameters[2];
            const int rh = c -> parameters[3];
            const int index = c -> parameters[4] % tn;
            const int ci = index % cn;
            const int ri = index / cn;
            x += ci * (cw + 1) + (cw + 2) / 2;
            y += ri * (rh + 1) + (rh + 2) / 2 + 1;

            move_focus_from_point((short) x, (short) y, direction, c);
            break;
        }
        case CORE_UI_INPUT_BOX: {
            move_focus_from_point((short) (c -> coord.X + c -> width / 2), (short) (c -> coord.Y + c -> height / 2), direction, c);
            break;
        }
        default: break;
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
    set_focused_component(NULL);
}

void set_location(Component *c, const int x, const int y) {
    const COORD new_coord = {(short) x, (short) y};
    c -> coord = new_coord;
}

void set_size(Component *c, const int width, const int height) {
    c -> width = width < CORE_UI_MINIMAL_TEXT_WIDTH ? CORE_UI_MINIMAL_TEXT_WIDTH : (short) width;
    c -> height = (short) height;
}

void set_box_choose(Component *c, const int index) {
    if (c -> type == CORE_UI_CHOOSE_BOX) {
        c -> parameters[4] = index;
    }
}

void set_box_input(Component *c, const char *text) {
    if (c ->type == CORE_UI_INPUT_BOX) {
        int i;
        for (i = 0; i < c -> parameters[1]; i++) {
            if (text[i] == '\0') {
                break;
            }
            c -> input[i] = text[i];
        }
        c -> parameters[0] = i;
        c -> input[c -> parameters[0]] = '\0';
    }
}

void set_call_back(Component *c, void (*callback) (int state)) {
    c -> call_back = callback;
}

// 创建标签
Component create_label(MultilanguageText *text, const short x, const short y, const short width, const short height, const int color) {
    short w;
    if (width < CORE_UI_MINIMAL_TEXT_WIDTH) {
        w = CORE_UI_MINIMAL_TEXT_WIDTH;
    } else {
        w = width;
    }
    const Component c = {CORE_UI_LABEL, 0, x, y, w, height, color, {0, 0, 0, 0, 0, 0, 0, 0}, 1, text, NULL, NULL, NULL};
    return c;
}

// 创建多选框
// 参数列表：列数，行数，列宽，行高，当前选中的索引数，当前页数，总页数
Component create_choose_box(MultilanguageText *text, const int number, const short x, const short y, const short column, const short row, const short grid_width, const short grid_height, const int color) {
    short w;
    if (grid_width < CORE_UI_MINIMAL_TEXT_WIDTH) {
        w = CORE_UI_MINIMAL_TEXT_WIDTH;
    } else {
        w = grid_width;
    }
    const Component c = {CORE_UI_CHOOSE_BOX, CORE_UI_FOCUSABLE, x, y, (short) ((w + 1) * column + 1), (short) ((grid_height + 1) * row + 1), color, {column, row, w, grid_height, 0, 0, 0, 0}, number, text, NULL, NULL, NULL};
    return c;
}

// 创建时钟
Component create_clock(const short x, const short y, const int color) {
    const Component c = {CORE_UI_CLOCK, 0, x, y, 32, 1, color, {0, 0, 0, 0, 0, 0, 0, 0}, 0, NULL, NULL, NULL, NULL};
    return c;
}

// 创建调试面板
// 参数列表：当前行数，总行数
Component create_debug_panel(const short x, const short y, const int color) {
    const Component c = {CORE_UI_DEBUG_PANEL, 0, x, y, 128, 16, color, {0, 0, 0, 0, 0, 0, 0, 0}, 0, NULL, NULL, NULL, NULL};
    return c;
}

// 创建FPS面板
Component create_fps_panel(const short x, const short y, const int color) {
    const Component c = {CORE_UI_FPS_PANEL, 0, x, y, 16, 1, color, {0, 0, 0, 0, 0, 0, 0, 0}, 0, NULL, NULL, NULL, NULL};
    return c;
}

// 创建输入框
// 参数列表：当前字符数，总字符数
Component create_input_box(const short x, const short y, const short width, const short height, const int length, const int color) {
    const Component c = {CORE_UI_INPUT_BOX, CORE_UI_FOCUSABLE, x, y, width, height, color, {0, length, 0, 0, 0, 0, 0, 0}, 0, NULL, malloc(sizeof(char) * (length + 1)), NULL, NULL};
    return c;
}

// 创建消息面板
// 参数列表：当前行数，总行数
Component create_notice_panel(const short x, const short y, const int color) {
    const Component c = {CORE_UI_NOTICE_PANEL, 0, x, y, 128, 2, color, {0, 0, 0, 0, 0, 0, 0, 0}, 0, NULL, NULL, NULL, NULL};
    return c;
}

// 创建结账面板
// 参数列表：当前行数，总行数
Component create_db_checkout_panel(const short x, const short y, const int color) {
    const Component c = {CORE_UI_DB_CHECKOUT_PANEL, 0, x, y, 72, 128, color, {0, 0, 0, 0, 0, 0, 0, 0}, 0, NULL, NULL, NULL, NULL};
    return c;
}

// 创建库存面板
// 参数列表：当前商品索引
Component create_db_stock_panel(const short x, const short y, const int color) {
    const Component c = {CORE_UI_DB_STOCK_PANEL, 0, x, y, 72, 128, color, {0, 0, 0, 0, 0, 0, 0, 0}, 0, NULL, NULL, NULL, NULL};
    return c;
}

// 创建价格面板
// 参数列表：当前商品索引
Component create_db_price_panel(const short x, const short y, const int color) {
    const Component c = {CORE_UI_DB_PRICE_PANEL, 0, x, y, 72, 128, color, {0, 0, 0, 0, 0, 0, 0, 0}, 0, NULL, NULL, NULL, NULL};
    return c;
}

void clear_console() {
    WriteConsole(current_handle, CLEAN_UP_CONSOLE, strlen(CLEAN_UP_CONSOLE), NULL, NULL);
    SetConsoleCursorPosition(current_handle, origin_coord);
}

// 刷新函数，每帧调用一次
void refresh_console() {
    // 清屏
    clear_console();

    // 更新时间
    refresh_time();

    // 更新fps
    refresh_fps();

    // 获取窗口大小
    refresh_window_info();

    // 刷新按键输入
    refresh_input();

    // 逐个绘制
    for (int i = 0; i < vector_size; i++) {
        draw_component(components[i]);
    }
    set_to_default_color();

    // 挪回光标（下面可能再挪走光标）
    SetConsoleCursorPosition(current_handle, cursor_coord);

    // 处理特殊组件的逻辑
    if (focused_component != NULL) {
        switch (focused_component -> type) {
            case CORE_UI_CHOOSE_BOX: {
                // 多选框
                hide_cursor();
                set_input_mode(CONTROL_INPUT);

                const int cn = focused_component -> parameters[0];
                const int rn = focused_component -> parameters[1];
                const int tn = cn * rn;
                const int index = focused_component -> parameters[4] % tn;
                const int ci = index % cn;
                const int ri = index / cn;



                if (is_key_pressed(VK_DOWN)) {
                    if (ri < rn - 1) {
                        focused_component -> parameters[4] += cn;
                        if (focused_component -> parameters[4] > focused_component -> texts_number - 1) {
                            focused_component -> parameters[4] = focused_component -> texts_number - 1;
                        }
                    } else {
                        move_focus(focused_component, CORE_UI_SOUTH);
                    }
                } else if (is_key_pressed(VK_UP)) {
                    if (ri > 0) {
                        focused_component -> parameters[4] -= cn;
                    } else {
                        move_focus(focused_component, CORE_UI_NORTH);
                    }
                } else if (is_key_pressed(VK_RIGHT)) {
                    if (ci < cn - 1) {
                        focused_component -> parameters[4] += 1;
                        if (focused_component -> parameters[4] > focused_component -> texts_number - 1) {
                            focused_component -> parameters[4] = focused_component -> texts_number - 1;
                        }
                    } else {
                        move_focus(focused_component, CORE_UI_EAST);
                    }
                } else if (is_key_pressed(VK_LEFT)) {
                    if (ci > 0) {
                        focused_component -> parameters[4] -= 1;
                    } else {
                        move_focus(focused_component, CORE_UI_WEST);
                    }
                } else if (is_key_pressed(VK_RETURN)) {
                    if (focused_component -> call_back != NULL) {
                        focused_component -> call_back(focused_component -> parameters[4]);
                    }
                } else if (is_key_pressed(VK_TAB)) {
                    throw_focus(focused_component);
                }
                break;
            }

            case CORE_UI_INPUT_BOX: {
                // 输入框
                show_cursor();
                set_input_mode(LINE_INPUT);

                read_buffer(&input_buffer, input_box_text_buffer, MAX_INPUT_TEXT_LENGTH, 1);
                for (int i = 0; i < MAX_INPUT_TEXT_LENGTH;) {
                    if (input_box_text_buffer[i] == '\0') {
                        break;
                    }
                    const UnicodeCharacter u = get_next_utf_8(input_box_text_buffer + i);
                    if (focused_component -> parameters[1] - focused_component -> parameters[0] >= u.length) {
                        for (int j = 0; j < u.length;j ++) {
                            focused_component -> input[focused_component -> parameters[0] ++] = input_box_text_buffer[i ++];
                        }
                    } else {
                        break;
                    }
                }
                focused_component -> input[focused_component -> parameters[0]] = '\0';

                if (is_key_pressed(VK_DOWN)) {
                    move_focus(focused_component, CORE_UI_SOUTH);
                } else if (is_key_pressed(VK_UP)) {
                    move_focus(focused_component, CORE_UI_NORTH);
                } else if (is_key_pressed(VK_RIGHT)) {
                    move_focus(focused_component, CORE_UI_EAST);
                } else if (is_key_pressed(VK_LEFT)) {
                    move_focus(focused_component, CORE_UI_WEST);
                } else if (is_key_pressed(VK_DELETE)) {
                    if (focused_component -> parameters[0] > 0) {
                        const UnicodeCharacter u = get_last_utf_8(focused_component -> input + focused_component -> parameters[0] - 1);
                        focused_component -> parameters[0] -= u.length;
                        focused_component -> input[focused_component -> parameters[0]] = '\0';
                    }
                } else if (is_key_pressed(VK_RETURN)) {
                    if (focused_component -> call_back == NULL) {
                        throw_focus(focused_component);
                    } else {
                        focused_component -> call_back(focused_component -> parameters[0]);
                    }
                } else if (is_key_pressed(VK_TAB)) {
                    throw_focus(focused_component);
                }
                break;
            }

            default:
                hide_cursor();
                set_input_mode(CONTROL_INPUT);
                break;
        }
    } else {
        hide_cursor();
        set_input_mode(CONTROL_INPUT);
    }

    SetConsoleCursorPosition(handle_0, cursor_coord);
    SetConsoleCursorPosition(handle_1, cursor_coord);

    // 交换缓冲区
    if (current_handle == handle_0) {
        SetConsoleActiveScreenBuffer(handle_0);
        current_handle = handle_1;
    } else {
        SetConsoleActiveScreenBuffer(handle_1);
        current_handle = handle_0;
    }
}

// 更新时间字符串
void refresh_time() {
    current_time = time(NULL);
    strftime(time_string, sizeof(time_string), "%Y / %m / %d    %H : %M : %S", localtime(&current_time));
}

// 更新fps字符串
void refresh_fps() {
    if (last_second != localtime(&current_time)->tm_sec) {
        last_second = localtime(&current_time)->tm_sec;
        fps = frame_count;
        frame_count = 0;
    } else {
        frame_count ++;
    }
    if (get_language() == zh_CN_4711) {
        sprintf(fps_string, "帧率 : %d", fps);
    } else {
        sprintf(fps_string, "FPS : %d", fps);
    }
}

void refresh_window_info() {
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(current_handle, &info);
    window_width = info.srWindow.Right - info.srWindow.Left + 1;
    window_height = info.srWindow.Bottom - info.srWindow.Top + 1;
}

void refresh_input() {
    for (int i = 0; i < 256; i++) {
        if (key_info_0[i]) {
            key_info_0[i] = 0;
            key_info_1[i] = 1;
        } else {
            key_info_1[i] = 0;
        }
    }
}

void draw_component(Component *c) {
    if (c != NULL) {
        const COORD coord = c -> coord;
        const short x = coord.X;
        short y = coord.Y;
        const short w = c -> width;
        const short h = c -> height;

        if (x > window_width) {
            return;
        }

        short remaining_height = h;

        switch (c -> type) {
            case CORE_UI_LABEL: {
                draw_multilanguage_text(c -> texts[0], x, y, w, h, c -> color, NULL);
                break;
            }
            case CORE_UI_CHOOSE_BOX: {
                const int tn = c -> texts_number;
                const int cn = c -> parameters[0];
                const int rn = c -> parameters[1];
                const int pn = cn * rn;                                                 // 一页上的总选项数
                const int cw = c -> parameters[2];
                const int rh = c -> parameters[3];

                // 更新当前索引、当前页数、总页数
                c -> parameters[6] = (tn + pn - 1) / pn;
                if (c -> parameters[5] < 0) {
                    c -> parameters[5] = 0;
                } if (c -> parameters[5] > c -> parameters[6] - 1) {
                    c -> parameters[5] = c -> parameters[6] - 1;
                }
                const int p = (c -> parameters[4] - c -> parameters[4] % pn) / pn;
                const int index_begin = c -> parameters[5] * pn;                        // 左上角的索引
                if (p != c -> parameters[5]) {
                    c -> parameters[4] = index_begin;
                }
                int index_end = index_begin + pn;
                if (index_end > tn) {
                    index_end = tn;
                }

                for (int i = index_begin, column = 0, row = 0; i < index_end; i++) {
                    if (i == c -> parameters[4]) {
                        if (c == focused_component) {
                            draw_text("•", (short) (x + column * (cw + 1)), (short) (y + row * (rh + 1) + 1), (short) cw, (short) rh, c -> color, NULL);
                            draw_multilanguage_text(c -> texts[i], (short) (x + column * (cw + 1) + 1), (short) (y + row * (rh + 1) + 1), (short) cw, (short) rh, c -> color, NULL);
                        } else {
                            draw_text("•", (short) (x + column * (cw + 1)), (short) (y + row * (rh + 1) + 1), (short) cw, (short) rh, FOREGROUND_COLOR, NULL);
                            draw_multilanguage_text(c -> texts[i], (short) (x + column * (cw + 1) + 1), (short) (y + row * (rh + 1) + 1), (short) cw, (short) rh, FOREGROUND_COLOR, NULL);
                        }
                    } else {
                        draw_multilanguage_text(c -> texts[i], (short) (x + column * (cw + 1) + 1), (short) (y + row * (rh + 1) + 1), (short) cw, (short) rh, DISABLED_COLOR, NULL);
                    }
                    column ++;
                    if (column == c -> parameters[0]) {
                        column = 0;
                        row ++;
                    }
                }
                break;
            }
            case CORE_UI_CLOCK: {
                draw_text(time_string, x, y, w, h, c -> color, NULL);
                break;
            }
            case CORE_UI_DEBUG_PANEL: {
                read_buffer(&debug_buffer, debug_text, MAX_DEBUG_TEXT_LENGTH, 0);
                c -> parameters[1] = count_total_lines(debug_text, w);
                if (c -> parameters[0] > c -> parameters[1] - h) {
                    c -> parameters[0] = c -> parameters[1] - h;
                }
                if (c -> parameters[0] < 0) {
                    c -> parameters[0] = 0;
                }
                int skip = c -> parameters[0];
                draw_text(debug_text, x, y, w, h, c -> color, &skip);
                break;
            }
            case CORE_UI_FPS_PANEL: {
                draw_text(fps_string, x, y, w, h, c -> color, NULL);
                break;
            }
            case CORE_UI_INPUT_BOX: {
                if (focused_component == c) {
                    draw_text("█", x, y, w, h, c -> color, NULL);
                    draw_text(c -> input, x, y, w, h, c -> color, NULL);
                    fix_cursor_coord();
                } else {
                    draw_text("_", x, y, w, h, DISABLED_COLOR, NULL);
                    draw_text(c -> input, x, y, w, h, DISABLED_COLOR, NULL);
                }
                break;
            }
            case CORE_UI_NOTICE_PANEL: {
                read_buffer(&notice_buffer, notice_text, MAX_NOTICE_TEXT_LENGTH, 0);
                int skip = c -> parameters[0];
                draw_text(notice_text, x, y, w, h, c -> color, &skip);
                break;
            }
            case CORE_UI_DB_CHECKOUT_PANEL: {
                const int cw = w / 4;
                draw_multilanguage_text(CHECKOUT_PANEL_TITLE[0], x, y, cw, 1, c -> color, NULL);
                draw_multilanguage_text(CHECKOUT_PANEL_TITLE[1], x + cw, y, cw, 1, c -> color, NULL);
                draw_multilanguage_text(CHECKOUT_PANEL_TITLE[2], x + cw * 2, y, cw, 1, c -> color, NULL);
                draw_multilanguage_text(CHECKOUT_PANEL_TITLE[3], x + cw * 3, y, cw, 1, c -> color, NULL);
                for (int i = 0;i < get_ru_number(); i ++) {
                    const RecordUnit *ru = get_ru(i);
                    char buffer[16];
                    sprintf(buffer, "%d", ru -> id);
                    draw_text(buffer, x, y + i * 2 + 2, cw, 2, c -> color, NULL);
                    draw_text(ru -> name, x + cw, y + i * 2 + 2, cw, 2, c -> color, NULL);
                    sprintf(buffer, "%.2f", ru -> price / 100.0);
                    draw_text(buffer, x + cw * 2, y + i * 2 + 2, cw, 2, c -> color, NULL);
                    sprintf(buffer, "%d", ru -> number);
                    draw_text(buffer, x + cw * 3, y + i * 2 + 2, cw, 2, c -> color, NULL);
                }
                break;
            }
            case CORE_UI_DB_STOCK_PANEL: {
                const int cw = w / 3;
                draw_multilanguage_text(STOCK_PANEL_TITLE[0], x, y, cw, 1, c -> color, NULL);
                draw_multilanguage_text(STOCK_PANEL_TITLE[1], x + cw, y, cw, 1, c -> color, NULL);
                draw_multilanguage_text(STOCK_PANEL_TITLE[2], x + cw * 2, y, cw, 1, c -> color, NULL);
                const int index = c -> parameters[0];
                if (index == -1) {
                    draw_multilanguage_text(STOCK_PANEL_TITLE[3], x, y + 2, w, 1, c -> color, NULL);
                } else {
                    const Item *item = get_item(index);
                    char buffer[16];
                    sprintf(buffer, "%d", item -> id);
                    draw_text(buffer, x, y + 2, cw, 2, c -> color, NULL);
                    draw_text(item -> name, x + cw, y + 2, cw, 2, c -> color, NULL);
                    sprintf(buffer, "%d", item -> stock);
                    draw_text(buffer, x + cw * 2, y + 2, cw, 2, c -> color, NULL);
                }
            }
            case CORE_UI_DB_PRICE_PANEL: {
                const int cw = w / 3;
                draw_multilanguage_text(PRICE_PANEL_TITLE[0], x, y, cw, 1, c -> color, NULL);
                draw_multilanguage_text(PRICE_PANEL_TITLE[1], x + cw, y, cw, 1, c -> color, NULL);
                draw_multilanguage_text(PRICE_PANEL_TITLE[2], x + cw * 2, y, cw, 1, c -> color, NULL);
                const int index = c -> parameters[0];
                if (index == -1) {
                    draw_multilanguage_text(PRICE_PANEL_TITLE[3], x, y + 2, w, 1, c -> color, NULL);
                } else {
                    const Item *item = get_item(index);
                    char buffer[16];
                    sprintf(buffer, "%d", item -> id);
                    draw_text(buffer, x, y + 2, cw, 2, c -> color, NULL);
                    draw_text(item -> name, x + cw, y + 2, cw, 2, c -> color, NULL);
                    sprintf(buffer, "%.2f", item -> price / 100.0);
                    draw_text(buffer, x + cw * 2, y + 2, cw, 2, c -> color, NULL);
                }
            }
            default:
            case CORE_UI_UNKNOWN: {
                for (int i = 0; i < c -> texts_number; i++) {
                    const int th = (short) draw_multilanguage_text(c -> texts[i], x, y, w, remaining_height, c -> color, NULL);
                    y = (short) (y + th);
                    remaining_height = (short) (remaining_height - th);
                }
                break;
            }
        }
    }
}

int draw_multilanguage_text(const MultilanguageText text, const short x, const short y, const short width, const short height, const int color, int *skip) {
    int h = 0;
    short w2 = (short) (window_width - x);
    if (w2 > width) {
        w2 = width;
    }
    switch (get_language()) {
        case 1:
            h += draw_text(text.en_US, x, y, w2, height, color, skip);
            break;
        case 2:
            h += draw_text(text.zh_CN, x, y, w2, height, color, skip);
            break;
        default:
            h += draw_text(text.en_US, x, y, w2, height, color, skip);
            h += draw_text(text.zh_CN, x, (short) (y + h), w2, (short) (height - h), color, skip);
            break;
    }
    return h;
}

int draw_text(const char *text, const short x, const short y, const short width, const short height, const int color, int *skip) {
    COORD coord = {x, y};
    const char *s = text;
    int output_height = 0;
    set_color(color);

    // 跳过skip行
    if (skip != NULL) {
        int skip_count = 0;
        for (short i = 0; i < *skip;) {
            if (!s[0]) {
                break;
            }
            if (s[0] == '\n' || s[0] == '\r') {
                // 换行
                s ++;
                i ++, skip_count ++;
            } else if (s[0] == '\t' || s[0] == ' ') {
                // 制表，功能暂不实现，仅跳过
                // 或者空格，跳过
                s++;
            } else {
                const int line_length = get_line_length(s, width);
                s += line_length;
                if (s[0] != '\n' && s[0] != '\r') {
                    i ++, skip_count ++;
                }
            }
        }
        *skip -= skip_count;
    }


    for (short i = y; i < y + height;) {
        if (!s[0]) {
            break;
        }
        if (s[0] == '\n' || s[0] == '\r') {
            // 换行
            s ++;
            coord.X = x;
            i ++, coord.Y ++, output_height ++;
        } else if (s[0] == '\t' || s[0] == ' ') {
            // 制表，功能暂不实现，仅跳过
            // 或者空格，跳过
            s++;
        } else {
            const int line_length = get_line_length(s, width);
            SetConsoleCursorPosition(current_handle, coord);
            WriteConsole(current_handle, s, line_length, NULL, NULL);
            s += line_length;
            if (s[0] != '\n' && s[0] != '\r') {
                i ++, coord.Y ++, output_height ++;
            }
        }
    }
    return output_height;
}




// 计算文本行数
int count_total_lines(const char *text, const short width) {
    const char *s = text;
    int lines = 0;
    while (s[0]) {
        if (s[0] == '\n' || s[0] == '\r') {
            s++;
            lines++;
        } else if (s[0] == '\t' || s[0] == ' ') {
            s++;
        } else {
            s += get_line_length(s, width);
            if (s[0] != '\n' && s[0] != '\r') lines++;
        }
    }
    return lines;
}

// 计算下一行要输出多少个char，自动处理宽字符和ANSI代码
int get_line_length(const char *text, const short width) {
    const char *s = text;
    int remaining_width = width;
    int length = 0;
    while (s[0]) {
        if (s[0] == '\n' || s[0] == '\r') {
            break;
        }

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












