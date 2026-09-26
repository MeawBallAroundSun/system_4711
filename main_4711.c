#include <stdio.h>
#include <windows.h>
#include <conio.h>



#include "language.h"
#include "core_ui.h"
#include "database_4711.h"
#include "pages.h"



int main(void) {
    enter_4711();

    command_add_item("10001", "苹果", "3.00");

     while (!is_system_closed) {
         refresh_console();
     }
}




