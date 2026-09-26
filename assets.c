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

MultilanguageText NU_CL = {
    "Since this is your first time using the system, please choose the language you want to use.",
    "这是您第一次使用此系统，请选择您想要使用的语言。"
};

MultilanguageText OU_CL = {
    "Since you haven't set the language yet, please choose the language you want to use.",
    "您还没有设置语言，请选择您想要使用的语言。"
};

MultilanguageText LANGUAGES[LANGUAGE_NUMBER] = {
    {
        "English", "English"
    },
    {
        "中文", "中文"
    }
};

MultilanguageText CREATE_ADMINISTRATOR_ACCOUNT = {
    "Since this is your first time using the system, please create an administrator account:\n\n\n\n"
    "( Both the name and the password must be at least six characters long )",
    "这是您第一次使用此系统，请创建管理员账户：\n\n\n\n"
    "（名称和密码均需要至少六位）"
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

MultilanguageText INPUT_ITEM = {
    "Item (ID or Name) :",
    "商品（ID或名称）："
};

MultilanguageText INPUT_NUMBER = {
    "Number:",
    "数量："
};

MultilanguageText INPUT_PRICE = {
    "Price:",
    "数量："
};

MultilanguageText CREATE_ACCOUNT_ERROR = {
    "The input format is incorrect, or the username has already been taken.",
    "输入格式有误，或此用户名已存在"
};

MultilanguageText LOGIN_ERROR = {
    "Username or password is incorrect.",
    "用户名或密码错误。"
};

MultilanguageText CASHIER_ERROR = {
    "Can't find the item, or the number is incorrect.",
    "找不到对应商品，或数量有误。"
};

MultilanguageText CHECKOUT_ERROR = {
    "No selected items.",
    "当前无已选商品。"
};

MultilanguageText STOCK_ERROR[2] = {
    {
        "Can't find the item.",
        "找不到对应商品。"
    },
    {
        "The number is incorrect.",
        "数字出错。"
    }
};

MultilanguageText LOGIN_BOX[1] = {
    {
        "Login",
        "登录"
    }
};

MultilanguageText AD_HOME_BOX[AD_HOME_OPTION_NUM] = {
    {
        "Cashier","收银"
    },
    {
        "View Items", "查看商品"
    },
    {
        "Set Language", "设置语言"
    },
    {
        "Set Stock", "设置库存"
    },
    {
        "Set Price", "设置价格"
    },
    {
        "Command", "指令"
    },
    {
        "Log Out", "退出登录"
    },
    {
        "Quit", "关闭程序"
    }
};

MultilanguageText CASHIER_BOX[4] = {
    {
        "Pick", "下单"
    },
    {
        "Checkout", "结账"
    },
    {
        "Clear", "清空"
    },
    {
        "Back", "返回"
    }
};

MultilanguageText CHECKOUT_PANEL_TITLE[4] = {
    {
        "ID", "编号"
    },
    {
        "Name", "名称"
    },
    {
        "Price", "价格"
    },
    {
        "Number", "数量"
    }
};

MultilanguageText STOCK_PANEL_TITLE[4] = {
    {
        "ID", "编号"
    },
    {
        "Name", "名称"
    },
    {
        "Stock", "库存"
    },
    {
        "No selected items.","无选中商品。"
    }
};

MultilanguageText PRICE_PANEL_TITLE[4] = {
    {
        "ID", "编号"
    },
    {
        "Name", "名称"
    },
    {
        "Price", "价格"
    },
    {
        "No selected items.","无选中商品。"
    }
};

MultilanguageText STOCK_BOX[3] = {
    {
        "View", "查看"
    },
    {
        "Set", "设置"
    },
    {
        "Back", "返回"
    }
};