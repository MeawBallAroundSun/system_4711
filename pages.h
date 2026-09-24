//
// Created by HZQ on 2026/9/24.
//

#ifndef SYSTEM_4711_PAGES_H
#define SYSTEM_4711_PAGES_H

#include "core_ui.h"

Component debug_panel;
Component clock;
Component fps_panel;

Component welcome_label;
Component choose_language_label;
Component languages_box;

void enter_welcome_page(int state);
void enter_create_administrator_account_page(int state);
void enter_create_crasher_account_page(int state);

#endif //SYSTEM_4711_PAGES_H
