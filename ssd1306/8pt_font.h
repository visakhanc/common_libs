

/*
 *
 * 8pt_font
 *
 * created with FontCreator
 * written by F. Maximilian Thiele
 *
 * http://www.apetech.de/fontCreator
 * me@apetech.de
 *
 * File Name           : 8pt_font.h
 * Date                : 11.11.2017
 * Font size in bytes  : 2806
 * Font width          : 10
 * Font height         : 8
 * Font first char     : 32
 * Font last char      : 128
 * Font used chars     : 96
 *
 * The font data are defined as
 *
 * struct _FONT_ {
 *     uint16_t   font_Size_in_Bytes_over_all_included_Size_it_self;
 *     uint8_t    font_Width_in_Pixel_for_fixed_drawing;
 *     uint8_t    font_Height_in_Pixel_for_all_characters;
 *     unit8_t    font_First_Char;
 *     uint8_t    font_Char_Count;
 *
 *     uint8_t    font_Char_Widths[font_Last_Char - font_First_Char +1];
 *                  // for each character the separate width in pixels,
 *                  // characters < 128 have an implicit virtual right empty row
 *
 *     uint8_t    font_data[];
 *                  // bit field of all characters
 */

#include <inttypes.h>
#include <avr/pgmspace.h>

#ifndef FONT_8PT_H
#define FONT_8PT_H


static uint8_t font_8pt[] PROGMEM = {
    0x08, // height
    0x20, // first char
    0x60, // char count
    
    // char widths
    0x03, 0x01, 0x01, 0x05, 0x05, 0x06, 0x05, 0x01, 0x02, 0x02, 
    0x02, 0x05, 0x01, 0x02, 0x01, 0x03, 0x04, 0x02, 0x04, 0x04, 
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x01, 0x01, 0x04, 0x03, 
    0x04, 0x04, 0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 
    0x04, 0x01, 0x04, 0x04, 0x04, 0x05, 0x05, 0x06, 0x04, 0x06, 
    0x04, 0x05, 0x05, 0x04, 0x05, 0x07, 0x05, 0x05, 0x05, 0x02, 
    0x03, 0x02, 0x03, 0x04, 0x01, 0x04, 0x04, 0x04, 0x04, 0x04, 
    0x02, 0x04, 0x04, 0x01, 0x03, 0x03, 0x01, 0x05, 0x04, 0x04, 
    0x04, 0x04, 0x03, 0x04, 0x02, 0x03, 0x03, 0x05, 0x05, 0x03, 
    0x04, 0x02, 0x01, 0x02, 0x04, 0x04,
    
    // font data
	0x00, 0x00, 0x00, // 32
    0xBF, // 33
    0x03, // 34
    0x24, 0x7E, 0x24, 0x7E, 0x24, // 35
    0x2E, 0x2A, 0x7F, 0x2A, 0x3A, // 36
    0x43, 0x23, 0x18, 0x04, 0x62, 0x61, // 37
    0x10, 0x2A, 0x2D, 0x7A, 0x20, // 38
    0x03, // 39
    0x3E, 0x41, // 40
    0x41, 0x3E, // 41
    0x18, 0x18, // 42
    0x08, 0x08, 0x7F, 0x08, 0x08, // 43
    0x60, // 44
    0x08, 0x08, // 45
    0x40, // 46
    0x60, 0x1C, 0x03, // 47
    0x3E, 0x41, 0x41, 0x3E, // 48
    0x02, 0x7F, // 49
    0x62, 0x51, 0x49, 0x46, // 50
    0x22, 0x49, 0x49, 0x36, // 51
    0x1E, 0x10, 0x7F, 0x10, // 52
    0x27, 0x45, 0x45, 0x39, // 53
    0x3E, 0x49, 0x49, 0x32, // 54
    0x01, 0x71, 0x0D, 0x03, // 55
    0x36, 0x49, 0x49, 0x36, // 56
    0x26, 0x49, 0x49, 0x3E, // 57
    0x24, // 58
    0x74, // 59
    0x08, 0x14, 0x22, 0x41, // 60
    0x14, 0x14, 0x14, // 61
    0x41, 0x22, 0x14, 0x08, // 62
    0x02, 0x59, 0x09, 0x06, // 63
    0x04, 0x32, 0x49, 0x51, 0x7E, // 64
    0x7F, 0x11, 0x11, 0x7F, // 65
    0x7F, 0x49, 0x49, 0x36, // 66
    0x3E, 0x41, 0x41, 0x22, // 67
    0x7F, 0x41, 0x41, 0x3E, // 68
    0x7F, 0x49, 0x49, 0x41, // 69
    0x7F, 0x09, 0x09, 0x01, // 70
    0x3C, 0x62, 0x41, 0x49, 0x3A, // 71
    0x7F, 0x08, 0x08, 0x7F, // 72
    0x7F, // 73
    0x20, 0x40, 0x40, 0x3F, // 74
    0x7F, 0x08, 0x14, 0x63, // 75
    0x7F, 0x40, 0x40, 0x40, // 76
    0x7F, 0x02, 0x0C, 0x02, 0x7F, // 77
    0x7F, 0x02, 0x0C, 0x10, 0x7F, // 78
    0x1C, 0x22, 0x41, 0x41, 0x22, 0x1C, // 79
    0x7F, 0x09, 0x09, 0x0F, // 80
    0x1C, 0x22, 0x21, 0x71, 0x22, 0x1C, // 81
    0x7F, 0x09, 0x09, 0x76, // 82
    0x26, 0x49, 0x49, 0x49, 0x32, // 83
    0x01, 0x01, 0x7F, 0x01, 0x01, // 84
    0x7F, 0x40, 0x40, 0x7F, // 85
    0x03, 0x1C, 0x60, 0x1C, 0x03, // 86
    0x07, 0x18, 0x60, 0x18, 0x60, 0x18, 0x07, // 87
    0x63, 0x14, 0x08, 0x14, 0x63, // 88
    0x07, 0x08, 0x70, 0x08, 0x07, // 89
    0x61, 0x51, 0x49, 0x45, 0x43, // 90
    0x7F, 0x41, // 91
    0x03, 0x1C, 0x60, // 92
    0x41, 0x7F, // 93
    0x06, 0x03, 0x06, // 94
    0x40, 0x40, 0x40, 0x40, // 95
    0x03, // 96
    0x32, 0x4A, 0x4A, 0x7C, // 97
    0x7F, 0x48, 0x48, 0x30, // 98
    0x38, 0x44, 0x44, 0x44, // 99
    0x30, 0x48, 0x48, 0x7F, // 100
    0x7C, 0x54, 0x54, 0x5C, // 101
    0x7E, 0x09, // 102
    0x4C, 0x52, 0x4A, 0x3E, // 103
    0x7F, 0x08, 0x08, 0x70, // 104
    0x7A, // 105
    0x20, 0x40, 0x3A, // 106
    0x7F, 0x10, 0x68, // 107
    0x7F, // 108
    0x78, 0x04, 0x78, 0x04, 0x78, // 109
    0x7C, 0x04, 0x04, 0x78, // 110
    0x7C, 0x44, 0x44, 0x7C, // 111
    0x7E, 0x12, 0x12, 0x0C, // 112
    0x0C, 0x12, 0x12, 0x7E, // 113
    0x78, 0x04, 0x04, // 114
    0x5C, 0x54, 0x54, 0x74, // 115
    0x7E, 0x48, // 116
    0x7C, 0x40, 0x7C, // 117
    0x3C, 0x40, 0x3C, // 118
    0x3C, 0x40, 0x3C, 0x40, 0x3C, // 119
    0x44, 0x28, 0x10, 0x28, 0x44, // 120
    0x5C, 0x50, 0x7C, // 121
    0x44, 0x64, 0x54, 0x4C, // 122
    0x08, 0x77, // 123
    0x7F, // 124
    0x77, 0x08, // 125
    0x04, 0x02, 0x04, 0x02, // 126
    0x3E, 0x22, 0x22, 0x3E // 127
    
};

#endif