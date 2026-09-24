#include <stdio.h>
#include <windows.h>
#include <conio.h>

#include "language.h"
#include "core_ui.h"
#include "assets.h"
#include "pages.h"


int main(void) {
    init_console();

    init_components();

    const int language_profile = load_language_profile();
    enter_welcome_page(0);

    // set_language(en_US_4711);
    set_input_mode(LINE_INPUT);
    set_cursor_coord(20, 20);
    while (1) {
        refresh_console();
    }


    return 0;
}


