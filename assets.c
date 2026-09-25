//
// Created by HZQ on 2026/9/13.
//

#include "language.h"
#include "assets.h"

MultilanguageText WELCOME = {
    "Welcome to use this supermarket system!\n"
    "Use [Tab] [↑] [↓] [←] [→] to move.\n"
    "Use [Enter] to select.",
    "欢迎使用此超市系统！\n"
    "使用 [Tab] [↑] [↓] [←] [→] 来移动。\n"
    "使用 [Enter] 来选择。"
};

MultilanguageText CHOOSE_LANGUAGE = {
    "Since this is your first time using the system, please choose the language you want to use.",
    "这是您第一次使用此系统，请选择您想要使用的语言。"
};

MultilanguageText LANGUAGES[LANGUAGE_NUMBER] = {
    {
        "English",
        "English"
    },
    {
        "中文",
        "中文"
    }
};

MultilanguageText CREATE_ADMINISTRATOR_ACCOUNT = {
    "Create an administrator account:",
    "创建管理员账户："
};

MultilanguageText CREATE_CRASHER_ACCOUNT = {
    "Create a crasher account:",
    "创建营业员账户："
};

MultilanguageText CREATE_ADMINISTRATOR_ACCOUNT_BOX[1] = {
    {
    "Create an administrator account",
    "创建管理员账户"
    }
};

MultilanguageText INPUT_NAME = {
    "Name:",
    "名称："
};

MultilanguageText INPUT_PASSWORD = {
    "Password:",
    "密码："
};