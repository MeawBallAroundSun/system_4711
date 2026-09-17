//
// Created by HZQ on 2026/9/12.
//

#include <stdio.h>

#include "language.h"

static int USER_LANGUAGE = 0;

int load_language_profile() {
    FILE *language_profile = fopen(LANGUAGE_PROFILE_PATH, "rb");

    if (!language_profile) {
        fclose(language_profile);
        return NO_LANGUAGE_CHOSEN;
    }

    int language;
    const size_t read_num = fread(&language, sizeof(language), 1, language_profile);
    fclose(language_profile);
    if (read_num != 1) {
        return NO_LANGUAGE_CHOSEN;
    }
    if (language < 1 || language > 2) {
        return NO_LANGUAGE_CHOSEN;
    }
    USER_LANGUAGE = language;
    return language;
}

void save_language_profile() {
    FILE *language_profile = fopen(LANGUAGE_PROFILE_PATH, "wb");
    fwrite(&USER_LANGUAGE, sizeof(int), 1, language_profile);
    fclose(language_profile);
}

int get_language() {
    return USER_LANGUAGE;
}

void set_language(const int language) {
    if (language < 1 || language > 2) {
        USER_LANGUAGE = 1;
    } else {
        USER_LANGUAGE = language;
    }
    save_language_profile();
}

char *get_multilanguage_text(const MultilanguageText text) {
    switch (USER_LANGUAGE) {
        case 1:
            return text.en_US;
        case 2:
            return text.zh_CN;
        default:
            return text.en_US;
    }
}

void print_multilanguage_text(const MultilanguageText text) {
    switch (USER_LANGUAGE) {
        case 1:
            puts(text.en_US);
            break;
        case 2:
            puts(text.zh_CN);
            break;
        default:
            puts(text.en_US);
            puts(text.zh_CN);
            break;
    }
}