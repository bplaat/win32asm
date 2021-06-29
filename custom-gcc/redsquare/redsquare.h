#ifndef REDSQUARE_H
#define REDSQUARE_H

// Utils
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// Languages
#define LANG_NEUTRAL 0x00
#define SUBLANG_NEUTRAL 0x00
#define LANG_ENGLISH 0x09
#define SUBLANG_ENGLISH_US 0x01
#define LANG_DUTCH 0x13
#define SUBLANG_DUTCH 0x01

// Version
#define APP_VERSION_MAJOR 0
#define APP_VERSION_MINOR 5
#define APP_VERSION_PATCH 1

// Icons
#define APP_ICON_ID 1

// Bitmaps
#define PAPER_BITMAP_ID 1
#define PAPER_DARK_BITMAP_ID 2

// Sounds
#define GAMEOVER_WAVE_ID 1

// Layouts
#define MAIN_LAYOUT_ID 1
#define CONTROL_TYPE_FONT 0
#define CONTROL_TYPE_LABEL 1
#define CONTROL_TYPE_BUTTON 2
#define CONTROL_TYPE_EDIT 3
#define CONTROL_TYPE_COMBOBOX 4
#define CONTROL_TYPE_LIST 5

#define CONTROL_UNIT_PX 0
#define CONTROL_UNIT_VW 1
#define CONTROL_UNIT_VH 2
#define CONTROL_UNIT_VX 3

// Pages
#define PAGE_MENU 0
#define PAGE_GAME 1
#define PAGE_GAMEOVER 2
#define PAGE_HIGHSCORES 3
#define PAGE_HELP 4
#define PAGE_SETTINGS 5

// Controls
#define FONT_TITLE_ID 1
#define FONT_TEXT_ID 2
#define FONT_LIST_ID 3
#define MENU_PLAY_BUTTON_ID 4
#define MENU_HIGHSCORES_BUTTON_ID 5
#define MENU_HELP_BUTTON_ID 6
#define MENU_SETTINGS_BUTTON_ID 7
#define MENU_EXIT_BUTTON_ID 8
#define GAMEOVER_BACK_BUTTON_ID 9
#define HIGHSCORES_LIST_ID 10
#define HIGHSCORES_BACK_BUTTON_ID 11
#define HELP_BACK_BUTTON_ID 12
#define SETTINGS_NAME_EDIT_ID 13
#define SETTINGS_LANGUAGE_SELECT_ID 14
#define SETTINGS_THEME_SELECT_ID 15
#define SETTINGS_BACK_BUTTON_ID 16

// Strings
#define FONT_STRING_ID 1
#define MENU_VERSION_STRING_ID 2
#define MENU_TITLE_STRING_ID 3
#define MENU_PLAY_STRING_ID 16
#define MENU_HIGHSCORES_STRING_ID 17
#define MENU_HELP_STRING_ID 18
#define MENU_SETTINGS_STRING_ID 19
#define MENU_EXIT_STRING_ID 20
#define MENU_FOOTER_STRING_ID 21
#define GAME_SCORE_STRING_ID 22
#define GAME_TIME_STRIND_ID 23
#define GAME_LEVEL_STRING_ID 24
#define GAMEOVER_TITLE_STRING_ID 25
#define BACK_STRING_ID 26
#define HIGHSCORES_EMPTY_STRING_ID 27
#define HELP_TEXT_STRING_ID 28
#define HELP_TEXT_LINES 6
#define SETTINGS_NAME_STRING_ID 29
#define SETTINGS_LANGUAGE_STRING_ID 30
#define SETTINGS_THEME_STRING_ID 31
#define SETTINGS_THEME_LIGHT_STRING_ID 32
#define SETTINGS_THEME_DARK_STRING_ID 33

// Manifest
#define MANIFEST_ID 1

#endif
