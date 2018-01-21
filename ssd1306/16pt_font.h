

/*
 *
 * new Font
 *
 * created with FontCreator
 * written by F. Maximilian Thiele
 *
 * http://www.apetech.de/fontCreator
 * me@apetech.de
 *
 * File Name           : 16pt_font.h
 * Date                : 11.11.2017
 * Font size in bytes  : 8950
 * Font width          : 10
 * Font height         : 16
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

#ifndef FONT_16PT_H
#define FONT_16PT_H

static uint8_t font_16pt[] PROGMEM = {
    0x10, // height
    0x20, // first char
    0x60, // char count
    
    // char widths
    0x04, 0x02, 0x06, 0x07, 0x07, 0x07, 0x07, 0x02, 0x04, 0x04, 
    0x07, 0x07, 0x02, 0x05, 0x02, 0x07, 0x06, 0x03, 0x06, 0x06, 
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x02, 0x02, 0x06, 0x05, 
    0x06, 0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 
    0x07, 0x03, 0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x05, 
    0x07, 0x05, 0x05, 0x08, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 
    0x05, 0x07, 0x07, 0x01, 0x03, 0x07, 0x01, 0x07, 0x07, 0x07, 
    0x07, 0x07, 0x05, 0x07, 0x05, 0x07, 0x07, 0x07, 0x07, 0x07, 
    0x07, 0x04, 0x01, 0x04, 0x06, 0x01, 
    
    // font data
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 32
    0xFE, 0x00, 0x33, 0x30, // 33
    0x04, 0x03, 0x01, 0x04, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 34
    0x20, 0xE0, 0x3E, 0x20, 0xE0, 0x3E, 0x20, 0x04, 0x7F, 0x04, 0x04, 0x7F, 0x04, 0x00, // 35
    0x18, 0x24, 0x42, 0xFF, 0x82, 0x04, 0x18, 0x0C, 0x10, 0x20, 0x7F, 0x20, 0x11, 0x0E, // 36
    0x1C, 0x22, 0x22, 0x9C, 0x60, 0x18, 0x06, 0x30, 0x0C, 0x03, 0x1C, 0x22, 0x22, 0x1C, // 37
    0x00, 0x3C, 0xC2, 0x42, 0x3C, 0x00, 0x00, 0x1E, 0x21, 0x20, 0x13, 0x0C, 0x13, 0x20, // 38
    0x0B, 0x07, 0x00, 0x00, // 39
    0xF0, 0x0C, 0x02, 0x01, 0x07, 0x18, 0x20, 0x40, // 40
    0x01, 0x02, 0x0C, 0xF0, 0x40, 0x20, 0x18, 0x07, // 41
    0x10, 0x20, 0x40, 0xF8, 0x40, 0x20, 0x10, 0x04, 0x02, 0x01, 0x0F, 0x01, 0x02, 0x04, // 42
    0x80, 0x80, 0x80, 0xF0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, // 43
    0x00, 0x00, 0x58, 0x38, // 44
    0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, // 45
    0x00, 0x00, 0x30, 0x30, // 46
    0x00, 0x00, 0x00, 0x80, 0x60, 0x18, 0x06, 0x30, 0x0C, 0x03, 0x00, 0x00, 0x00, 0x00, // 47
    0xF8, 0x04, 0x02, 0x02, 0x04, 0xF8, 0x0F, 0x10, 0x20, 0x20, 0x10, 0x0F, // 48
    0x04, 0x04, 0xFE, 0x00, 0x00, 0x3F, // 49
    0x18, 0x04, 0x02, 0x02, 0xC4, 0x38, 0x30, 0x28, 0x26, 0x21, 0x20, 0x20, // 50
    0x18, 0x04, 0x82, 0x82, 0x44, 0x38, 0x0C, 0x10, 0x20, 0x20, 0x11, 0x0E, // 51
    0x00, 0x80, 0x60, 0x18, 0xFE, 0x00, 0x06, 0x05, 0x04, 0x04, 0x3F, 0x04, // 52
    0xFE, 0x42, 0x22, 0x22, 0x42, 0x82, 0x08, 0x10, 0x20, 0x20, 0x10, 0x0F, // 53
    0xF8, 0x84, 0x42, 0x42, 0x84, 0x18, 0x0F, 0x10, 0x20, 0x20, 0x10, 0x0F, // 54
    0x02, 0x02, 0x02, 0x82, 0x72, 0x0E, 0x00, 0x00, 0x3C, 0x03, 0x00, 0x00, // 55
    0x38, 0x44, 0x82, 0x82, 0x44, 0x38, 0x0E, 0x11, 0x20, 0x20, 0x11, 0x0E, // 56
    0x78, 0x84, 0x02, 0x02, 0x84, 0xF8, 0x0C, 0x10, 0x21, 0x21, 0x10, 0x0F, // 57
    0x60, 0x60, 0x18, 0x18, // 58
    0x60, 0x60, 0x58, 0x38, // 59
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, // 60
    0x20, 0x20, 0x20, 0x20, 0x20, 0x02, 0x02, 0x02, 0x02, 0x02, // 61
    0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, // 62
    0x18, 0x04, 0x02, 0x82, 0x44, 0x38, 0x00, 0x00, 0x33, 0x30, 0x00, 0x00, // 63
    0xF8, 0x04, 0xE2, 0x12, 0xF2, 0x04, 0xF8, 0x0F, 0x10, 0x23, 0x24, 0x27, 0x24, 0x13, // 64
    0x00, 0x80, 0x78, 0x06, 0x78, 0x80, 0x00, 0x38, 0x07, 0x04, 0x04, 0x04, 0x07, 0x38, // 65
    0xFE, 0x82, 0x82, 0x82, 0x82, 0x44, 0x38, 0x3F, 0x20, 0x20, 0x20, 0x20, 0x11, 0x0E, // 66
    0xF8, 0x04, 0x02, 0x02, 0x02, 0x04, 0x18, 0x0F, 0x10, 0x20, 0x20, 0x20, 0x10, 0x0C, // 67
    0xFE, 0x02, 0x02, 0x02, 0x02, 0x04, 0xF8, 0x3F, 0x20, 0x20, 0x20, 0x20, 0x10, 0x0F, // 68
    0xFE, 0x82, 0x82, 0x82, 0x82, 0x82, 0x02, 0x3F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // 69
    0xFE, 0x82, 0x82, 0x82, 0x82, 0x82, 0x02, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 70
    0xF8, 0x04, 0x02, 0x02, 0x02, 0x04, 0x18, 0x0F, 0x10, 0x20, 0x20, 0x21, 0x11, 0x3F, // 71
    0xFE, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFE, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, // 72
    0x02, 0xFE, 0x02, 0x20, 0x3F, 0x20, // 73
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x0C, 0x10, 0x20, 0x20, 0x10, 0x0F, // 74
    0xFE, 0x00, 0x80, 0x60, 0x10, 0x0C, 0x02, 0x3F, 0x01, 0x00, 0x01, 0x06, 0x08, 0x30, // 75
    0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // 76
    0xFE, 0x78, 0x80, 0x00, 0x80, 0x78, 0xFE, 0x3F, 0x00, 0x07, 0x38, 0x07, 0x00, 0x3F, // 77
    0xFE, 0x18, 0x60, 0x80, 0x00, 0x00, 0xFE, 0x3F, 0x00, 0x00, 0x01, 0x06, 0x18, 0x3F, // 78
    0xF8, 0x04, 0x02, 0x02, 0x02, 0x04, 0xF8, 0x0F, 0x10, 0x20, 0x20, 0x20, 0x10, 0x0F, // 79
    0xFE, 0x82, 0x82, 0x82, 0x82, 0x44, 0x38, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 80
    0xF8, 0x04, 0x02, 0x02, 0x02, 0x04, 0xF8, 0x0F, 0x10, 0x20, 0x24, 0x28, 0x10, 0x2F, // 81
    0xFE, 0x82, 0x82, 0x82, 0x82, 0x44, 0x38, 0x3F, 0x00, 0x00, 0x00, 0x03, 0x0C, 0x30, // 82
    0x38, 0x44, 0x42, 0x82, 0x82, 0x04, 0x18, 0x0C, 0x10, 0x20, 0x20, 0x20, 0x11, 0x0E, // 83
    0x02, 0x02, 0x02, 0xFE, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, // 84
    0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x0F, 0x10, 0x20, 0x20, 0x20, 0x10, 0x0F, // 85
    0x0E, 0xF0, 0x00, 0x00, 0x00, 0xF0, 0x0E, 0x00, 0x00, 0x07, 0x38, 0x07, 0x00, 0x00, // 86
    0xFE, 0x00, 0xE0, 0x1E, 0xE0, 0x00, 0xFE, 0x01, 0x3E, 0x01, 0x00, 0x01, 0x3E, 0x01, // 87
    0x06, 0x18, 0x60, 0x80, 0x60, 0x18, 0x06, 0x30, 0x0C, 0x03, 0x00, 0x03, 0x0C, 0x30, // 88
    0x06, 0x18, 0x60, 0x80, 0x60, 0x18, 0x06, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, // 89
    0x02, 0x02, 0x02, 0x82, 0x62, 0x1A, 0x06, 0x30, 0x2C, 0x23, 0x20, 0x20, 0x20, 0x20, // 90
    0xFF, 0x01, 0x01, 0x01, 0x01, 0x7F, 0x40, 0x40, 0x40, 0x40, // 91
    0x02, 0x0C, 0x30, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0C, 0x30, // 92
    0x01, 0x01, 0x01, 0x01, 0xFF, 0x40, 0x40, 0x40, 0x40, 0x7F, // 93
    0x04, 0x02, 0x01, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, // 94
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 95
    0x01, 0x02, 0x04, 0x00, 0x00, 0x00, // 96
    0x80, 0x40, 0x40, 0x40, 0x40, 0x80, 0x00, 0x18, 0x24, 0x22, 0x22, 0x12, 0x3F, 0x20, // 97
    0xFE, 0x80, 0x40, 0x40, 0x40, 0x80, 0x00, 0x3F, 0x10, 0x20, 0x20, 0x20, 0x10, 0x0F, // 98
    0x00, 0x80, 0x40, 0x40, 0x40, 0x40, 0x80, 0x0F, 0x10, 0x20, 0x20, 0x20, 0x20, 0x10, // 99
    0x00, 0x80, 0x40, 0x40, 0x40, 0x80, 0xFE, 0x0F, 0x10, 0x20, 0x20, 0x20, 0x10, 0x3F, // 100
    0x00, 0x80, 0x40, 0x40, 0x40, 0x80, 0x00, 0x0F, 0x12, 0x22, 0x22, 0x22, 0x22, 0x13, // 101
    0x40, 0x40, 0xFC, 0x42, 0x42, 0x00, 0x00, 0x3F, 0x00, 0x00, // 102
    0x00, 0x80, 0x40, 0x40, 0x40, 0x80, 0x40, 0x30, 0x4D, 0x4A, 0x4A, 0x4A, 0x49, 0x30, // 103
    0xFE, 0x80, 0x40, 0x40, 0x40, 0x80, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, // 104
    0xCC, 0x3F, // 105
    0x00, 0x00, 0xCC, 0x40, 0x40, 0x3F, // 106
    0xFE, 0x00, 0x00, 0x00, 0x80, 0x40, 0x00, 0x3F, 0x04, 0x02, 0x05, 0x08, 0x10, 0x20, // 107
    0xFE, 0x3F, // 108
    0xC0, 0x80, 0x40, 0x80, 0x40, 0x40, 0x80, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, // 109
    0xC0, 0x80, 0x40, 0x40, 0x40, 0x80, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, // 110
    0x00, 0x80, 0x40, 0x40, 0x40, 0x80, 0x00, 0x0F, 0x10, 0x20, 0x20, 0x20, 0x10, 0x0F, // 111
    0xC0, 0x80, 0x40, 0x40, 0x40, 0x80, 0x00, 0x7F, 0x08, 0x10, 0x10, 0x10, 0x08, 0x07, // 112
    0x00, 0x80, 0x40, 0x40, 0x40, 0x80, 0xC0, 0x07, 0x08, 0x10, 0x10, 0x10, 0x08, 0x7F, // 113
    0xC0, 0x80, 0x40, 0x40, 0x40, 0x3F, 0x00, 0x00, 0x00, 0x00, // 114
    0x80, 0x40, 0x40, 0x40, 0x40, 0x40, 0x80, 0x11, 0x22, 0x22, 0x24, 0x24, 0x24, 0x18, // 115
    0x40, 0x40, 0xFC, 0x40, 0x40, 0x00, 0x00, 0x1F, 0x20, 0x20, // 116
    0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x0F, 0x10, 0x20, 0x20, 0x20, 0x10, 0x3F, // 117
    0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x03, 0x0C, 0x30, 0x0C, 0x03, 0x00, // 118
    0xC0, 0x00, 0x00, 0xC0, 0x00, 0x00, 0xC0, 0x07, 0x38, 0x06, 0x01, 0x06, 0x38, 0x07, // 119
    0x40, 0x80, 0x00, 0x00, 0x00, 0x80, 0x40, 0x20, 0x10, 0x09, 0x06, 0x09, 0x10, 0x20, // 120
    0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x40, 0x43, 0x2C, 0x10, 0x0C, 0x03, 0x00, // 121
    0x40, 0x40, 0x40, 0x40, 0x40, 0xC0, 0x40, 0x30, 0x28, 0x24, 0x22, 0x21, 0x20, 0x20, // 122
    0x80, 0x7F, 0x01, 0x01, 0x00, 0x7F, 0x40, 0x40, // 123
    0xFF, 0xFF, // 124
    0x01, 0x01, 0x7F, 0x80, 0x40, 0x40, 0x7F, 0x00, // 125
    0x02, 0x01, 0x01, 0x02, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 126
    0xFC, 0x3F // 127
    
};

#endif