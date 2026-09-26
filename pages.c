//
// Created by HZQ on 2026/9/24.
//

#include "pages.h"
#include "assets.h"
#include "database_4711.h"

char is_system_closed = 0;


Component debug_panel;
Component clock_panel;
Component fps_panel;
Component notice_panel;
Component checkout_panel;
Component stock_panel;
Component price_panel;
Component view_panel;
Component name_panel;


Component welcome_label;
Component nu_cl_label;
Component ou_cl_label;
Component languages_box;

Component nu_ca_label;
Component input_name_label;
Component input_name_box;
Component input_password_label;
Component input_password_box;
Component input_item_label;
Component input_item_box;
Component input_number_label;
Component input_number_box;
Component input_price_label;
Component input_price_box;
Component input_command_label;
Component input_command_box;

Component create_administrator_account_box;

Component login_box;

Component ad_home_box;
Component ca_home_box;

Component cashier_box;

Component stock_box;

Component command_box;

Component view_box;


// 创建全部组件
void init_components() {
    debug_panel = create_debug_panel(0, 0, SELECTED_COLOR);
    clock_panel = create_clock(0, 0, FOREGROUND_COLOR);
    fps_panel = create_fps_panel(0, 0, FOREGROUND_COLOR);
    notice_panel = create_notice_panel(0, 0, SELECTED_COLOR);
    checkout_panel = create_db_checkout_panel(0, 0, SELECTED_COLOR);
    stock_panel = create_db_stock_panel(0, 0, FOREGROUND_COLOR);
    price_panel = create_db_price_panel(0, 0, FOREGROUND_COLOR);
    view_panel = create_db_view_panel(0, 0, FOREGROUND_COLOR);
    name_panel = create_db_name_panel(0, 0, FOREGROUND_COLOR);

    welcome_label = create_label(&WELCOME, 0, 0, CORE_UI_CONSOLE_WIDTH, LANGUAGE_NUMBER * 3, FOREGROUND_COLOR);
    nu_cl_label = create_label(&NU_CL, 0, 0, CORE_UI_CONSOLE_WIDTH, LANGUAGE_NUMBER, FOREGROUND_COLOR);
    ou_cl_label = create_label(&OU_CL, 0, 0, CORE_UI_CONSOLE_WIDTH, LANGUAGE_NUMBER, FOREGROUND_COLOR);
    languages_box = create_choose_box(LANGUAGES, LANGUAGE_NUMBER, 0, 0, LANGUAGE_NUMBER / 2, 2, 128, 1, SELECTED_COLOR);

    nu_ca_label = create_label(&CREATE_ADMINISTRATOR_ACCOUNT, 0, 0, CORE_UI_CONSOLE_WIDTH, 5, FOREGROUND_COLOR);
    input_name_label = create_label(&INPUT_NAME, 0, 0, 16, 1, FOREGROUND_COLOR);
    input_name_box = create_input_box(0, 0, MAX_NAME_LENGTH, 2, MAX_NAME_LENGTH, FOREGROUND_COLOR);
    input_password_label = create_label(&INPUT_PASSWORD, 0, 0, 16, 1, FOREGROUND_COLOR);
    input_password_box = create_input_box(0, 0, MAX_PASSWORD_LENGTH, 2, MAX_PASSWORD_LENGTH, FOREGROUND_COLOR);
    input_item_label = create_label(&INPUT_ITEM, 0, 0, 20, 1, FOREGROUND_COLOR);
    input_item_box = create_input_box(0, 0, MAX_ITEM_LENGTH / 4, 4, MAX_ITEM_LENGTH, FOREGROUND_COLOR);
    input_number_label = create_label(&INPUT_NUMBER, 0, 0, 20, 1, FOREGROUND_COLOR);
    input_number_box = create_input_box(0, 0, MAX_NUMBER_LENGTH, 2, MAX_NUMBER_LENGTH, FOREGROUND_COLOR);
    input_price_label = create_label(&INPUT_PRICE, 0, 0, 20, 1, FOREGROUND_COLOR);
    input_price_box = create_input_box(0, 0, MAX_PRICE_LENGTH, 2, MAX_PRICE_LENGTH, FOREGROUND_COLOR);
    input_command_label = create_label(&INPUT_COMMAND, 0, 0, 40, 1, FOREGROUND_COLOR);
    input_command_box = create_input_box(0, 0, 128, 32, MAX_COMMAND_LENGTH, FOREGROUND_COLOR);

    create_administrator_account_box = create_choose_box(CREATE_ADMINISTRATOR_ACCOUNT_BOX, 1, 0, 0, 1, 1, 32, 1, SELECTED_COLOR);

    login_box = create_choose_box(LOGIN_BOX, 1, 0, 0, 1, 1, 64, 1, SELECTED_COLOR);

    ad_home_box = create_choose_box(AD_HOME_BOX, AD_HOME_OPTION_NUM, 0, 0, 5, 4, 24, 3, SELECTED_COLOR);
    ca_home_box = create_choose_box(CA_HOME_BOX, CA_HOME_OPTION_NUM, 0, 0, 5, 4, 24, 3, SELECTED_COLOR);

    cashier_box = create_choose_box(CASHIER_BOX, 4, 0, 0, 2, 2, 16, 1, SELECTED_COLOR);

    stock_box = create_choose_box(STOCK_BOX, 3, 0, 0, 2, 2, 16, 1, SELECTED_COLOR);

    command_box = create_choose_box(COMMAND_BOX, 4, 0, 0, 4, 1, 16, 1, SELECTED_COLOR);

    view_box = create_choose_box(VIEW_BOX, 1, 0, 0, 1, 1, 16, 1, SELECTED_COLOR);
}

void debug_output(const char *text) {
    print_debug(text, strlen(text), 1);
}


// 顶部栏
static void add_title_bar() {
    set_location(&clock_panel, 0, 0);
    add_component(&clock_panel);

    set_location(&fps_panel, 40, 0);
    add_component(&fps_panel);

    set_location(&name_panel, 60, 0);
    add_component(&name_panel);
}

// 程序入口
void enter_4711() {
    // 初始化控制台
    init_console();

    // 初始化所有要用到的组件
    init_components();

    // 初始化数据库并设置debug输出
    init_database();
    set_command_output(&debug_output);
    set_command_time(&get_time);

    // 获取语言文件
    const int language_profile = load_language_profile();

    if (get_account_number() == 0) {
        if (language_profile == 0) {
            enter_nu_cl_wel();
        } else {
            set_language(language_profile);
            enter_nu_ca_wel();
        }
    } else {
        if (language_profile == 0) {
            enter_ou_cl_wel();
        } else {
            set_language(language_profile);
            enter_ou_wel();
        }
    }
}




// 进入新用户选择语言欢迎界面
void enter_nu_cl_wel() {
    remove_all_components();

    add_title_bar();

    set_location(&welcome_label, 0, 4);
    add_component(&welcome_label);

    set_location(&nu_cl_label, 0, 13);
    add_component(&nu_cl_label);

    set_location(&languages_box, 0, 17);
    set_box_choose(&languages_box, 0);
    set_call_back(&languages_box, leave_nu_cl_wel);
    add_component(&languages_box);
    set_focused_component(&languages_box);
}
// 离开新用户选择语言欢迎界面
void leave_nu_cl_wel(const int state) {
    set_language(state + 1);
    enter_nu_ca_wel();
}




// 进入新用户创建管理员欢迎界面
void enter_nu_ca_wel() {
    remove_all_components();

    add_title_bar();

    set_location(&nu_ca_label, 0, 4);
    add_component(&nu_ca_label);

    set_location(&input_name_label, 0, 12);
    add_component(&input_name_label);

    set_location(&input_name_box, 16, 12);
    set_box_input(&input_name_box, "");
    add_component(&input_name_box);

    set_location(&input_password_label, 0, 16);
    add_component(&input_password_label);

    set_location(&input_password_box, 16, 16);
    set_box_input(&input_password_box, "");
    add_component(&input_password_box);

    set_location(&create_administrator_account_box, 0, 19);
    set_box_choose(&create_administrator_account_box, 0);
    set_call_back(&create_administrator_account_box, leave_nu_ca_wel);
    add_component(&create_administrator_account_box);

    set_location(&notice_panel, 0, 24);
    clear_notice();
    add_component(&notice_panel);

    set_focused_component(&input_name_box);
}
// 离开新用户创建管理员欢迎界面
void leave_nu_ca_wel(int state) {
    if (command_create_account(input_name_box.input, input_password_box.input, "admin") == DB_ERROR) {
        clear_notice();
        print_notice(CREATE_ACCOUNT_ERROR, 1);
    } else {
        command_login(input_name_box.input, input_password_box.input);
        enter_home();
    }
}



// 进入老用户选择语言欢迎界面
void enter_ou_cl_wel(void) {
    remove_all_components();

    add_title_bar();

    set_location(&welcome_label, 0, 4);
    add_component(&welcome_label);

    set_location(&ou_cl_label, 0, 13);
    add_component(&ou_cl_label);

    set_location(&languages_box, 0, 17);
    set_box_choose(&languages_box, 0);
    set_call_back(&languages_box, leave_ou_cl_wel);
    add_component(&languages_box);
    set_focused_component(&languages_box);
}
// 离开老用户选择语言欢迎界面
void leave_ou_cl_wel(const int state) {
    set_language(state + 1);
    enter_ou_wel();
}



// 进入老用户欢迎（登录）界面
void enter_ou_wel(void) {
    remove_all_components();

    add_title_bar();

    set_location(&welcome_label, 0, 4);
    add_component(&welcome_label);

    set_location(&input_name_label, 0, 10);
    add_component(&input_name_label);

    set_location(&input_name_box, 16, 10);
    set_box_input(&input_name_box, "");
    add_component(&input_name_box);

    set_location(&input_password_label, 0, 14);
    add_component(&input_password_label);

    set_location(&input_password_box, 16, 14);
    set_box_input(&input_password_box, "");
    add_component(&input_password_box);

    set_location(&login_box, 0, 17);
    set_box_choose(&login_box, 0);
    set_call_back(&login_box, leave_ou_wel);
    add_component(&login_box);

    set_location(&notice_panel, 0, 22);
    clear_notice();
    add_component(&notice_panel);

    set_focused_component(&input_name_box);
}
// 离开老用户欢迎（登录）界面
void leave_ou_wel(int state) {
    if (command_login(input_name_box.input, input_password_box.input) == DB_ERROR) {
        clear_notice();
        print_notice(LOGIN_ERROR, 1);
    } else {
        enter_home();
    }
}





// 进入主界面
void enter_home() {
    const Account *account = get_current_account();
    if (account -> is_administrator) {
        enter_ad_home();
    } else {
        enter_ca_home();
    }
}



// 进入管理员主界面
void enter_ad_home() {
    remove_all_components();

    add_title_bar();

    set_location(&ad_home_box, 0, 4);
    set_box_choose(&ad_home_box, 0);
    set_call_back(&ad_home_box, leave_ad_home);
    add_component(&ad_home_box);

    set_focused_component(&ad_home_box);
}
// 退出管理员主界面
void leave_ad_home(const int state) {
    switch (state) {
        case 0: {
            enter_cashier();
            break;
        }
        case 1: {
            enter_view();
            break;
        }
        case 2: {
            enter_cl();
            break;
        }
        case 3: {
            enter_stock();
            break;
        }
        case 4: {
            enter_price();
            break;
        }
        case 5: {
            enter_command();
            break;
        }
        case 6: {
            command_logout();
            enter_ou_wel();
            break;
        }
        case 7: {
            is_system_closed = 1;
            break;
        }
    }
}








// 进入收银员界面
void enter_ca_home() {
    remove_all_components();

    add_title_bar();

    set_location(&ca_home_box, 0, 4);
    set_box_choose(&ca_home_box, 0);
    set_call_back(&ca_home_box, leave_ca_home);
    add_component(&ca_home_box);

    set_focused_component(&ca_home_box);
}
// 离开收银员界面
void leave_ca_home(const int state) {
    switch (state) {
        case 0: {
            enter_cashier();
            break;
        }
        case 1: {
            enter_view();
            break;
        }
        case 2: {
            enter_cl();
            break;
        }
        case 3: {
            command_logout();
            enter_ou_wel();
            break;
        }
        case 4: {
            is_system_closed = 1;
            break;
        }
    }
}


// 进入收银界面
void enter_cashier() {
    remove_all_components();

    add_title_bar();

    set_location(&input_item_label, 0, 4);
    add_component(&input_item_label);

    set_location(&input_item_box, 0, 5);
    set_box_input(&input_item_box, "");
    add_component(&input_item_box);

    set_location(&input_number_label, 0, 10);
    add_component(&input_number_label);

    set_location(&input_number_box, 0, 11);
    set_box_input(&input_number_box, "");
    add_component(&input_number_box);

    set_location(&cashier_box, 0, 16);
    set_box_choose(&cashier_box, 0);
    set_call_back(&cashier_box, leave_cashier);
    add_component(&cashier_box);

    set_location(&notice_panel, 0, 22);
    clear_notice();
    add_component(&notice_panel);

    set_location(&checkout_panel, 48, 4);
    add_component(&checkout_panel);

    set_focused_component(&input_item_box);
}
// 退出收银界面
void leave_cashier(const int state) {
    switch (state) {
        case 0: {
            clear_notice();
            const char *number = NULL;
            if (strlen(input_number_box.input) > 0) {
                number = input_number_box.input;
            }
            if (command_pick(input_item_box.input, number) == DB_ERROR) {
                print_notice(CASHIER_ERROR, 1);
            }
            set_box_input(&input_item_box, "");
            set_box_input(&input_number_box, "");
            break;
        }
        case 1: {
            clear_notice();
            if (command_checkout() == DB_ERROR) {
                print_notice(CHECKOUT_ERROR, 1);
            }
            break;
        }
        case 2: {
            clear_notice();
            command_clear();
            break;
        }
        case 3:
        default: {
            enter_home();
            break;
        }
    }
}





// 进入语言选择界面
void enter_cl() {
    remove_all_components();

    add_title_bar();

    set_location(&languages_box, 0, 4);
    set_box_choose(&languages_box, get_language() - 1);
    set_call_back(&languages_box, leave_cl);
    add_component(&languages_box);

    set_focused_component(&languages_box);
}
// 退出语言选择界面
void leave_cl(const int state) {
    set_language(state + 1);
    enter_home();
}







// 进入库存设置界面
void enter_stock() {
    remove_all_components();

    add_title_bar();

    set_location(&input_item_label, 0, 4);
    add_component(&input_item_label);

    set_location(&input_item_box, 0, 5);
    set_box_input(&input_item_box, "");
    add_component(&input_item_box);

    set_location(&input_number_label, 0, 10);
    add_component(&input_number_label);

    set_location(&input_number_box, 0, 11);
    set_box_input(&input_number_box, "");
    add_component(&input_number_box);

    set_location(&stock_box, 0, 16);
    set_box_choose(&stock_box, 0);
    set_call_back(&stock_box, leave_stock);
    add_component(&stock_box);

    set_location(&notice_panel, 0, 22);
    clear_notice();
    add_component(&notice_panel);

    set_location(&stock_panel, 48, 4);\
    stock_panel.parameters[0] = -1;
    add_component(&stock_panel);

    set_focused_component(&input_item_box);
}
// 退出库存设置界面
void leave_stock(const int state) {
    switch (state) {
        case 0: {
            clear_notice();
            const int index = command_search(input_item_box.input);
            if (index == -1) {
                print_notice(STOCK_ERROR[0], 1);
                set_box_input(&input_item_box, "");
            }
            stock_panel.parameters[0] = index;
            break;
        }
        case 1: {
            clear_notice();
            const int index = command_search(input_item_box.input);
            if (index == -1) {
                print_notice(STOCK_ERROR[0], 1);
                set_box_input(&input_item_box, "");
            }
            stock_panel.parameters[0] = index;
            if (command_set_stock(input_item_box.input, input_number_box.input) == DB_ERROR) {
                print_notice(STOCK_ERROR[1], 1);
            }
            set_box_input(&input_number_box, "");
            break;
        }
        case 2:
        default: {
            enter_home();
            break;
        }
    }
}









// 进入价格设置界面
void enter_price() {
    remove_all_components();

    add_title_bar();

    set_location(&input_item_label, 0, 4);
    add_component(&input_item_label);

    set_location(&input_item_box, 0, 5);
    set_box_input(&input_item_box, "");
    add_component(&input_item_box);

    set_location(&input_price_label, 0, 10);
    add_component(&input_price_label);

    set_location(&input_price_box, 0, 11);
    set_box_input(&input_price_box, "");
    add_component(&input_price_box);

    set_location(&stock_box, 0, 16);
    set_box_choose(&stock_box, 0);
    set_call_back(&stock_box, leave_price);
    add_component(&stock_box);

    set_location(&notice_panel, 0, 22);
    clear_notice();
    add_component(&notice_panel);

    set_location(&price_panel, 48, 4);\
    price_panel.parameters[0] = -1;
    add_component(&price_panel);

    set_focused_component(&input_item_box);
}
// 退出价格设置界面
void leave_price(const int state) {
    switch (state) {
        case 0: {
            clear_notice();
            const int index = command_search(input_item_box.input);
            if (index == -1) {
                print_notice(STOCK_ERROR[0], 1);
                set_box_input(&input_item_box, "");
            }
            price_panel.parameters[0] = index;
            break;
        }
        case 1: {
            clear_notice();
            const int index = command_search(input_item_box.input);
            if (index == -1) {
                print_notice(STOCK_ERROR[0], 1);
                set_box_input(&input_item_box, "");
            }
            price_panel.parameters[0] = index;
            if (command_set_price(input_item_box.input, input_price_box.input) == DB_ERROR) {
                print_notice(STOCK_ERROR[1], 1);
            }
            set_box_input(&input_price_box, "");
            break;
        }
        case 2:
        default: {
            enter_home();
            break;
        }
    }
}










// 进入指令页面
void enter_command() {
    remove_all_components();

    add_title_bar();

    set_location(&debug_panel, 0, 3);
    debug_panel.parameters[0] = debug_panel.parameters[1];
    add_component(&debug_panel);

    set_location(&command_box, 0, 20);
    set_box_choose(&command_box, 0);
    command_box.parameters[7] = 1;
    set_call_back(&command_box, leave_command);
    add_component(&command_box);

    set_location(&input_command_label, 0, 24);
    add_component(&input_command_label);

    set_location(&input_command_box, 0, 25);
    set_box_input(&input_command_box, "");
    input_command_box.parameters[2] = 0;
    set_call_back(&input_command_box, leave_command);
    add_component(&input_command_box);

    set_focused_component(&input_command_box);
}
// 退出指令页面
void leave_command(const int state) {
    switch (state) {
        case 0: {
            print_debug(input_command_box.input, strlen(input_command_box.input), 1);
            input_command(input_command_box.input);
            set_box_input(&input_command_box, "");
            debug_panel.parameters[0] = debug_panel.parameters[1];
            break;
        }
        case 1: {
            debug_panel.parameters[0] --;
            break;
        }
        case 2: {
            debug_panel.parameters[0] ++;
            break;
        }
        default:
        case 3: {
            enter_home();
            break;
        }
    }
}

// 进入查看页面
void enter_view() {
    remove_all_components();

    add_title_bar();

    set_location(&view_box, 0, 3);
    set_box_choose(&view_box, 0);
    set_call_back(&view_box, leave_view);
    add_component(&view_box);

    set_location(&view_panel, 0, 6);
    add_component(&view_panel);

    set_focused_component(&view_box);
}
// 退出查看页面
void leave_view(const int state) {
    enter_home();
}



