//
// Created by HZQ on 2026/9/24.
//

#ifndef SYSTEM_4711_PAGES_H
#define SYSTEM_4711_PAGES_H

#include "core_ui.h"

#define MAX_NAME_LENGTH                         32
#define MAX_PASSWORD_LENGTH                     32
#define MAX_ITEM_LENGTH                         128
#define MAX_NUMBER_LENGTH                       32

extern char is_system_closed;

extern Component debug_panel;
extern Component clock_panel;
extern Component fps_panel;
extern Component notice_panel;
extern Component checkout_panel;


extern Component welcome_label;
extern Component nu_cl_label;
extern Component ou_cl_label;
extern Component languages_box;

extern Component nu_ca_label;
extern Component input_name_label;
extern Component input_name_box;
extern Component input_password_label;
extern Component input_password_box;
extern Component input_item_label;
extern Component input_item_box;
extern Component input_number_label;
extern Component input_number_box;

extern Component create_administrator_account_box;

extern Component login_box;

extern Component ad_home_box;

extern Component stock_box;


void init_components();

void enter_4711();

void enter_nu_cl_wel();
void leave_nu_cl_wel(int state);

void enter_nu_ca_wel();
void leave_nu_ca_wel(int state);

void enter_ou_cl_wel();
void leave_ou_cl_wel(int state);

void enter_ou_wel();
void leave_ou_wel(int state);

void enter_home();

void enter_ad_home();
void leave_ad_home(int state);

void enter_cashier();
void leave_cashier(int state);

void enter_cl();
void leave_cl(int state);

void enter_stock();
void leave_stock(int state);

#endif //SYSTEM_4711_PAGES_H
