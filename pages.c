//
// Created by HZQ on 2026/9/24.
//

#include "pages.h"
#include "assets.h"

Component debug_panel;
Component clock;
Component fps_panel;


Component welcome_label;
Component choose_language_label_0;
Component languages_box;

Component create_administrator_account_label;
Component input_name_label;
Component input_name_box;
Component input_password_label;
Component input_password_box;
Component create_administrator_account_box;



// 创建全部组件
void init_components() {
    debug_panel = create_debug_panel(0, 0, SELECTED_COLOR);
    clock = create_clock(0, 0, FOREGROUND_COLOR);
    fps_panel = create_fps_panel(0, 0, FOREGROUND_COLOR);

    welcome_label = create_label(&WELCOME, 0, 0, CORE_UI_CONSOLE_WIDTH, LANGUAGE_NUMBER, FOREGROUND_COLOR);
    choose_language_label_0 = create_label(&CHOOSE_LANGUAGE, 0, 0, CORE_UI_CONSOLE_WIDTH, LANGUAGE_NUMBER, FOREGROUND_COLOR);
    languages_box = create_choose_box(LANGUAGES, LANGUAGE_NUMBER, 0, 0, LANGUAGE_NUMBER / 2, 2, 16, 1, SELECTED_COLOR);

    create_administrator_account_label = create_label(&CREATE_ADMINISTRATOR_ACCOUNT, 0, 0, CORE_UI_CONSOLE_WIDTH, 2, FOREGROUND_COLOR);
    input_name_label = create_label(&INPUT_NAME, 0, 0, 16, 1, FOREGROUND_COLOR);
    input_name_box = create_input_box(0, 0, MAX_NAME_LENGTH, 2, MAX_NAME_LENGTH, FOREGROUND_COLOR);
    input_password_label = create_label(&INPUT_PASSWORD, 0, 0, 16, 1, FOREGROUND_COLOR);
    input_password_box = create_input_box(0, 0, MAX_PASSWORD_LENGTH, 2, MAX_PASSWORD_LENGTH, FOREGROUND_COLOR);
    create_administrator_account_box = create_choose_box(CREATE_ADMINISTRATOR_ACCOUNT_BOX, 1, 0, 0, 1, 1, 32, 1, SELECTED_COLOR);
}

// 顶部栏
static void add_title_bar() {
    set_location(&clock, 0, 0);
    add_component(&clock);

    set_location(&fps_panel, 40, 0);
    add_component(&fps_panel);
}

// 进入欢迎界面
void enter_welcome_page(const int state) {
    remove_all_components();
    set_focused_component(NULL);

    switch (state) {
        case 0:
            // 新用户
            add_title_bar();

            set_location(&welcome_label, 0, 4);
            add_component(&welcome_label);

            set_location(&choose_language_label_0, 0, 9);
            add_component(&choose_language_label_0);

            set_location(&languages_box, 0, 13);
            set_box_choose(&languages_box, 0);
            set_call_back(&languages_box, leave_welcome_page_0);
            add_component(&languages_box);

            set_focused_component(&languages_box);

            break;
        case 1:
        case 2:
            // 老用户
            add_title_bar();


            break;
        default: break;
    }
}

// 离开欢迎界面（新用户）
void leave_welcome_page_0(const int state) {
    set_language(state + 1);
    enter_create_administrator_account_page(state);
}


// 进入创建管理员界面
void enter_create_administrator_account_page(const int state) {
    remove_all_components();
    set_focused_component(NULL);

    add_title_bar();

    set_location(&create_administrator_account_label, 0, 4);
    add_component(&create_administrator_account_label);

    set_location(&input_name_label, 0, 8);
    add_component(&input_name_label);

    set_location(&input_name_box, 16, 8);
    set_box_input(&input_name_box, "");
    add_component(&input_name_box);

    set_location(&input_password_label, 0, 12);
    add_component(&input_password_label);

    set_location(&input_password_box, 16, 12);
    set_box_input(&input_password_box, "");
    add_component(&input_password_box);

    set_location(&create_administrator_account_box, 0, 15);
    set_box_choose(&create_administrator_account_box, 0);
    add_component(&create_administrator_account_box);

    set_focused_component(&input_name_box);

}
