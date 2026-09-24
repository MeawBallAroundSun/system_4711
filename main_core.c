#include <stdio.h>
#include <windows.h>
#include <conio.h>

#include "language.h"
#include "core_ui.h"
#include "pages.h"


int main(void) {
    // 初始化控制台
    init_console();

    // 初始化所有要用到的组件
    init_components();


    const int language_profile = load_language_profile();
    enter_welcome_page(language_profile);
    if (language_profile != 0) {
        set_language(language_profile);
    }

    set_input_mode(LINE_INPUT);
    hide_cursor();
    while (1) {
        refresh_console();
    }


    return 0;
}


