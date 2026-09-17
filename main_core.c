#include <stdio.h>
#include <windows.h>
#include <conio.h>

#include "language.h"
#include "core_ui.h"
#include "assets.h"

int main(void) {
    init_console();

    Component welcome_label = create_label(&WELCOME, 0, 0, CORE_UI_CONSOLE_WIDTH, 2, FOREGROUND_COLOR);
    add_component(&welcome_label);

    Component choose_language_label = create_label(&CHOOSE_LANGUAGE, 0, 4, CORE_UI_CONSOLE_WIDTH, 2, FOREGROUND_COLOR);

    const int language_profile = load_language_profile();
    if (language_profile == 0) {
        add_component(&choose_language_label);
    }

    while (1) {
        refresh_console();
    }


    return 0;
}


