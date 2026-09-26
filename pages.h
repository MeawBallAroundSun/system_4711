//
// Created by HZQ on 2026/9/24.
//

#ifndef SYSTEM_4711_PAGES_H
#define SYSTEM_4711_PAGES_H

#include "core_ui.h"

#define MAX_NAME_LENGTH                         64
#define MAX_PASSWORD_LENGTH                     64

extern Component debug_panel;
extern Component clock_panel;
extern Component fps_panel;

extern Component welcome_label;
extern Component choose_language_label_0;
extern Component languages_box;

extern Component create_administrator_account_label;
extern Component input_name_label;
extern Component input_name_box;
extern Component input_password_label;
extern Component input_password_box;
extern Component create_administrator_account_box;

void init_components();

void enter_4711();

void enter_nu_cl_wel(void);
void leave_nu_cl_wel(int state);

void enter_nu_ca_wel(void);
void leave_nu_ca_wel(int state);

void enter_ou_cl_wel(void);
void leave_ou_cl_wel(int state);

void enter_ad_home(void);

#endif //SYSTEM_4711_PAGES_H
