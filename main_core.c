#include <stdio.h>
#include <windows.h>
#include <conio.h>

#include "language.h"
#include "core_ui.h"
#include "assets.h"

Component debug_panel;

Component clock;
Component fps_panel;
Component welcome_label;
Component choose_language_label;
Component languages_box;





// 创建全部组件
void init_components() {
    debug_panel = create_debug_panel(0, 0, SELECTED_COLOR);

    clock = create_clock(0, 0, FOREGROUND_COLOR);
    fps_panel = create_fps_panel(0, 0, FOREGROUND_COLOR);
    welcome_label = create_label(&WELCOME, 0, 0, CORE_UI_CONSOLE_WIDTH, LANGUAGE_NUMBER, FOREGROUND_COLOR);
    choose_language_label = create_label(&CHOOSE_LANGUAGE, 0, 0, CORE_UI_CONSOLE_WIDTH, LANGUAGE_NUMBER, FOREGROUND_COLOR);
    languages_box = create_choose_box(LANGUAGES, LANGUAGE_NUMBER, 0, 0, LANGUAGE_NUMBER / 2, 2, 16, 1, SELECTED_COLOR);
}

// 进入欢迎界面
void enter_welcome_page(const int state) {
    remove_all_components();
    set_focused_component(NULL);
    switch (state) {
        case 0:
            // 新用户
            set_location(&clock, 0, 0);
            add_component(&clock);

            set_location(&fps_panel, 32, 0);
            add_component(&fps_panel);

            set_location(&welcome_label, 0, 2);
            add_component(&welcome_label);

            set_location(&choose_language_label, 0, 5);
            add_component(&choose_language_label);

            set_location(&languages_box, 0, 8);
            set_box_choose(&languages_box, 0);
            add_component(&languages_box);
            set_focused_component(&languages_box);

            set_location(&debug_panel, 0, 12);
            set_size(&debug_panel, 64, 16);
            add_component(&debug_panel);
            break;
        case 1:
        case 2:
            // 老用户
            break;
        default: break;
    }
}

int main(void) {
    init_console();

    init_components();

    const int language_profile = load_language_profile();
    enter_welcome_page(0);
    // set_language(en_US_4711);
    while (1) {
        refresh_console();
    }


    return 0;
}


