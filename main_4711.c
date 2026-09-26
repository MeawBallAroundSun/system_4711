#include <stdio.h>
#include <windows.h>
#include <conio.h>



#include "language.h"
#include "core_ui.h"
#include "database_4711.h"
#include "pages.h"



int main(void) {
    enter_4711();


     // 初始化控制台
     init_console();

     // 初始化所有要用到的组件
     init_components();

     // 初始化文件
     init_folders();


     const int language_profile = load_language_profile();
     enter_nu_cl_wel();
     if (language_profile != 0) {
         set_language(language_profile);
     }


     while (1) {
         refresh_console();
     }


     return 0;

}




