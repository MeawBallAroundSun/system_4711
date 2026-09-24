//
// Created by HZQ on 2026/9/24.
//

#ifndef SYSTEM_4711_PAGES_H
#define SYSTEM_4711_PAGES_H

#include "core_ui.h"

extern Component debug_panel;
extern Component clock;
extern Component fps_panel;

extern Component welcome_label;
extern Component choose_language_label;
extern Component languages_box;

void init_components();

void enter_welcome_page(int state);
void enter_create_administrator_account_page(int state);
void enter_create_crasher_account_page(int state);

#endif //SYSTEM_4711_PAGES_H
