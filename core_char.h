//
// Created by HZQ on 2026/9/21.
//

#ifndef SYSTEM_4711_CORE_CHAR_H
#define SYSTEM_4711_CORE_CHAR_H

#define UNKNOWN_CHAR_4711           0
#define LATIN_CHAR_4711             1
#define GREEK_COPTIC_CHAR_4711      2
#define CYRIL_CHAR_4711             3
#define ARMENIAN_CHAR_4711          4
#define HEBREW_CHAR_4711            5
#define ARABIC_CHAR_4711            6
#define CJK_CHAR_4711               7
#define PUNCTUATION_CHAR_4711       100
#define NUMBER_CHAR_4711            101
#define EMOJI_CHAR_4711             102

typedef struct UnicodeCharacter {
    unsigned int unicode;
    short area;                     // 指语言种类，比如中文、emoji
    char is_full_width;             // 全半角
    char length;                    // 指utf_8编码下的字节数
} UnicodeCharacter;

UnicodeCharacter get_next_utf_8(const char *text);

char is_full_width(unsigned int unicode);

short get_area(unsigned int unicode);


#endif //SYSTEM_4711_CORE_CHAR_H
