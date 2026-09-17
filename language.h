//
// Created by HZQ on 2026/9/12.
//

#ifndef SYSTEM_4711_LANGUAGE_H
#define SYSTEM_4711_LANGUAGE_H

#define NO_LANGUAGE_CHOSEN              0
#define en_US_4711                      1
#define zh_CN_4711                      2

#define LANGUAGE_PROFILE_PATH           "language.prof"

typedef struct MultilanguageText {
    char *en_US;
    char *zh_CN;
} MultilanguageText;

int load_language_profile();

void save_language_profile();

int get_language();

void set_language(int language);

char *get_multilanguage_text(MultilanguageText text);

void print_multilanguage_text(MultilanguageText text);

#endif //SYSTEM_4711_LANGUAGE_H
