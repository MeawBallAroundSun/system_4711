//
// Created by HZQ on 2026/9/21.
//

#include "core_char.h"

// 计算下一个utf_8字符的Unicode码（注：5、6个字节的utf_8已弃用）
UnicodeCharacter get_next_utf_8(const char *text) {
    const unsigned char *s = (unsigned char *) text;
    unsigned char c = s[0];
    unsigned int unicode = 0;
    char length;
    if ((c & 0xC0) == 0xC0) {
        if ((c & 0xE0) == 0xE0) {
            if ((c & 0xF0) == 0xF0) {
                if ((c & 0xF8) == 0xF8) {
                    if ((c & 0xFC) == 0xFC) {
                        // 6 字节
                        length = 6;
                        unicode = c & 0x1;
                        for (int i = 1; i < 6; i++) {
                            unicode = unicode << 6;
                            c = s[i];
                            unicode |= c & 0x3F;
                        }
                    } else {
                        // 5 字节
                        length = 5;
                        unicode = c & 0x3;
                        for (int i = 1; i < 5; i++) {
                            unicode = unicode << 6;
                            c = s[i];
                            unicode |= c & 0x3F;
                        }
                    }
                } else {
                    // 4 字节
                    length = 4;
                    unicode = c & 0x7;
                    for (int i = 1; i < 4; i++) {
                        unicode = unicode << 6;
                        c = s[i];
                        unicode |= c & 0x3F;
                    }
                }
            } else {
                // 3 字节
                length = 3;
                unicode = c & 0xF;
                for (int i = 1; i < 3; i++) {
                    unicode = unicode << 6;
                    c = s[i];
                    unicode |= c & 0x3F;
                }
            }
        } else {
            // 2 字节
            length = 2;
            unicode = c & 0x1F;
            unicode = unicode << 6;
            c = s[1];
            unicode |= c & 0x3F;
        }
    } else {
        // 1 字节
        length = 1;
        unicode = c;
    }

    const UnicodeCharacter uc = {unicode, get_area(unicode), is_full_width(unicode), length};
    return uc;
}

// 计算上一个utf_8字符的Unicode码
UnicodeCharacter get_last_utf_8(const char *text) {
    const char * r = text;
    for (int i = 0; i < 6; i++) {
        if ((*r & 0xC0) != 0x80) {
            break;
        }
        r --;
    }
    return get_next_utf_8(r);
}

// 判断一个Unicode码的全半角
char is_full_width(const unsigned int unicode) {
    return (char) (
        (unicode >= 0x1100 && unicode <= 0x115F)  ||                    // 韩文字母
        (unicode >= 0x2E80 && unicode <= 0x303E)  ||                    // 中日韩部首
        (unicode >= 0x3041 && unicode <= 0x33FF)  ||                    // 平假名、片假名、注音
        (unicode >= 0x3400 && unicode <= 0x4DBF)  ||                    // CJK扩展A
        (unicode >= 0x4E00 && unicode <= 0x9FFF)  ||                    // CJK统一表意
        (unicode >= 0xA000 && unicode <= 0xA4CF)  ||                    // 彝文
        (unicode >= 0xAC00 && unicode <= 0xD7A3)  ||                    // 韩文音节
        (unicode >= 0xF900 && unicode <= 0xFAFF)  ||                    // CJK兼容
        (unicode >= 0xFE30 && unicode <= 0xFE6F)  ||                    // CJK兼容形式
        (unicode >= 0xFF00 && unicode <= 0xFF60)  ||                    // 全角ASCII
        (unicode >= 0xFFE0 && unicode <= 0xFFE6)  ||                    // 全角符号
        (unicode >= 0x1F300 && unicode <= 0x1F64F) ||                   // emoji
        (unicode >= 0x1F900 && unicode <= 0x1F9FF) ||                   // 补充emoji
        (unicode >= 0x20000 && unicode <= 0x3FFFD)                      // CJK扩展B+
    );
}

// 判断一个Unicode码所属的区域
short get_area(const unsigned int unicode) {
    if (
        (unicode >= 0x0041 && unicode <= 0x005A)  ||                    // 大写字母
        (unicode >= 0x0061 && unicode <= 0x007A)  ||                    // 小写字母
        (unicode >= 0x0080 && unicode <= 0x024F)                        // 拉丁字母补充/扩展
    ) {
        return LATIN_CHAR_4711;
    }
    if (
        unicode >= 0x0370 && unicode <= 0x03FF                          // 希腊和科普特字母
    ) {
        return GREEK_COPTIC_CHAR_4711;
    }
    if (
        unicode >= 0x0400 && unicode <= 0x052F                          // 西里尔字母及补充
    ) {
        return CYRIL_CHAR_4711;
    }
    if (
        unicode >= 0x0530 && unicode <= 0x058F                          // 亚美尼亚字母
    ) {
        return ARMENIAN_CHAR_4711;
    }
    if (
        unicode >= 0x0590 && unicode <= 0x05FF                          // 希伯来字母
    ) {
        return HEBREW_CHAR_4711;
    }
    if (
        unicode >= 0x0600 && unicode <= 0x06FF                          // 阿拉伯字母
    ) {
        return ARABIC_CHAR_4711;
    }
    if (
        (unicode >= 0x1100 && unicode <= 0x115F)  ||                    // 韩文字母
        (unicode >= 0x2E80 && unicode <= 0x303E)  ||                    // 中日韩部首
        (unicode >= 0x3041 && unicode <= 0x33FF)  ||                    // 平假名、片假名、注音
        (unicode >= 0x3400 && unicode <= 0x4DBF)  ||                    // CJK扩展A
        (unicode >= 0x4E00 && unicode <= 0x9FFF)  ||                    // CJK统一表意
        (unicode >= 0xF900 && unicode <= 0xFAFF)  ||                    // CJK兼容
        (unicode >= 0xFE30 && unicode <= 0xFE6F)  ||                    // CJK兼容形式
        (unicode >= 0x20000 && unicode <= 0x3FFFD)                      // CJK扩展B+
    ) {
        return CJK_CHAR_4711;
    }
    if (
        (unicode >= 0x0020 && unicode <= 0x002F)  ||                    // 一些符号
        (unicode >= 0x003A && unicode <= 0x0040)  ||                    // 一些符号
        (unicode >= 0x005B && unicode <= 0x0060)  ||                    // 一些符号
        (unicode >= 0x007B && unicode <= 0x007E)  ||                    // 一些符号
        (unicode >= 0x2000 && unicode <= 0x206F)  ||                    // 常用符号
        (unicode >= 0x3000 && unicode <= 0x303F)                        // 中日韩标点符号
    ) {
        return PUNCTUATION_CHAR_4711;
    }
    if (
        unicode >= 0x0030 && unicode <= 0x0039                          // 数字
    ) {
        return NUMBER_CHAR_4711;
    }
    if (
        (unicode >= 0x1F300 && unicode <= 0x1F64F) ||                   // emoji
        (unicode >= 0x1F900 && unicode <= 0x1F9FF)                      // 补充emoji
    ) {
        return EMOJI_CHAR_4711;
    }
    return UNKNOWN_CHAR_4711;
}
