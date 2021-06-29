    %include "layout.inc"
    %include "redsquare.inc"

    ; #####################################################
    ; ###################### FONTS ########################
    ; #####################################################

    font FONT_TITLE_ID, FONT_STRING_ID, 400, 48 vx
    font FONT_TEXT_ID, FONT_STRING_ID, 400, 24 vx
    font FONT_LIST_ID, FONT_STRING_ID, 400, 16 vx

    ; #####################################################
    ; ##################### MENU PAGE #####################
    ; #####################################################

    label PAGE_MENU, 0, MENU_VERSION_STRING_ID, FONT_TEXT_ID, ES_RIGHT, 0 px, 16 vh, 624 vw, 24 vx

    %assign y (480 - (48 + 16 + (52 + 8) * 5 + 16)) / 2
    label PAGE_MENU, 0, MENU_TITLE_STRING_ID, FONT_TITLE_ID, ES_CENTER, 0 px, y vh, 640 vw, 48 vh
    %assign y y + 48 + 16

    button PAGE_MENU, MENU_PLAY_BUTTON_ID, MENU_PLAY_STRING_ID, FONT_TEXT_ID, 0, 160 vw, y vh, 320 vw, 52 vh
    %assign y y + 52 + 8
    button PAGE_MENU, MENU_HIGHSCORES_BUTTON_ID, MENU_HIGHSCORES_STRING_ID, FONT_TEXT_ID, 0, 160 vw, y vh, 320 vw, 52 vh
    %assign y y + 52 + 8
    button PAGE_MENU, MENU_HELP_BUTTON_ID, MENU_HELP_STRING_ID, FONT_TEXT_ID, 0, 160 vw, y vh, 320 vw, 52 vh
    %assign y y + 52 + 8
    button PAGE_MENU, MENU_SETTINGS_BUTTON_ID, MENU_SETTINGS_STRING_ID, FONT_TEXT_ID, 0, 160 vw, y vh, 320 vw, 52 vh
    %assign y y + 52 + 8
    button PAGE_MENU, MENU_EXIT_BUTTON_ID, MENU_EXIT_STRING_ID, FONT_TEXT_ID, 0, 160 vw, y vh, 320 vw, 52 vh

    label PAGE_MENU, 0, MENU_FOOTER_STRING_ID, FONT_TEXT_ID, ES_CENTER, 0 px, 440 vh, 640 vw, 56 vh

    ; #####################################################
    ; ################### GAMEOVER PAGE ###################
    ; #####################################################

    %assign y (480 - (48 + 16 + (24 + 16) * 3 + 52)) / 2
    label PAGE_GAMEOVER, 0, GAMEOVER_TITLE_STRING_ID, FONT_TITLE_ID, ES_CENTER, 0 px, y vh, 640 vw, 48 vh
    %assign y y + 48 + 16 + (24 + 16) * 3

    button PAGE_GAMEOVER, GAMEOVER_BACK_BUTTON_ID, BACK_STRING_ID, FONT_TEXT_ID, 0, 160 vw, y vh, 320 vw, 52 vh

    ; #####################################################
    ; ################# HIGH SCORES PAGE ##################
    ; #####################################################

    %assign y (480 - (48 + 16 + 256 + 16 + 52)) / 2
    label PAGE_HIGHSCORES, 0, MENU_HIGHSCORES_STRING_ID, FONT_TITLE_ID, ES_CENTER, 0 px, y vh, 640 vw, 48 vh
    %assign y y + 48 + 16

    list PAGE_HIGHSCORES, HIGHSCORES_LIST_ID, 0, FONT_LIST_ID, LVS_LIST, 160 vw, y vh, 320 vw, 256 vh
    %assign y y + 256 + 16

    button PAGE_HIGHSCORES, HIGHSCORES_BACK_BUTTON_ID, BACK_STRING_ID, FONT_TEXT_ID, 0, 160 vw, y vh, 320 vw, 52 vh

    ; #####################################################
    ; ###################### HELP PAGE ####################
    ; #####################################################

    %assign height 24 * HELP_TEXT_LINES
    %assign y (480 - (48 + 16 + height + 16 + 52)) / 2
    label PAGE_HELP, 0, MENU_HELP_STRING_ID, FONT_TITLE_ID, ES_CENTER, 0 px, y vh, 640 vw, 48 vh
    %assign y y + 48 + 16

    label PAGE_HELP, 0, HELP_TEXT_STRING_ID, FONT_TEXT_ID, ES_CENTER | ES_MULTILINE, 0 px, y vh, 640 vw, height vx
    %assign y y + height + 16

    button PAGE_HELP, HELP_BACK_BUTTON_ID, BACK_STRING_ID, FONT_TEXT_ID, 0, 160 vw, y vh, 320 vw, 52 vh

    ; #####################################################
    ; ################### SETTINGS PAGE ###################
    ; #####################################################

    %assign y (480 - (48 + 16 + 24 + 8 + 24 + 16 + (24 + 8 + 32 + 16) * 2 + 52)) / 2
    label PAGE_SETTINGS, 0, MENU_SETTINGS_STRING_ID, FONT_TITLE_ID, ES_CENTER, 0 px, y vh, 640 vw, 48 vh
    %assign y y + 48 + 16

    label PAGE_SETTINGS, 0, SETTINGS_NAME_STRING_ID, FONT_TEXT_ID, ES_CENTER, 0 px, y vh, 640 vw, 24 vh
    %assign y y + 24 + 8
    edit PAGE_SETTINGS, SETTINGS_NAME_EDIT_ID, 0, FONT_TEXT_ID, ES_AUTOHSCROLL | ES_CENTER, 160 vw, y vh, 320 vw, 24 vh
    %assign y y + 24 + 16

    label PAGE_SETTINGS, 0, SETTINGS_LANGUAGE_STRING_ID, FONT_TEXT_ID, ES_CENTER, 0 px, y vh, 640 vw, 24 vh
    %assign y y + 24 + 8
    combobox PAGE_SETTINGS, SETTINGS_LANGUAGE_SELECT_ID, 0, FONT_TEXT_ID, CBS_DROPDOWNLIST | CBS_HASSTRINGS, 160 vw, y vh, 320 vw, 32 vh
    %assign y y + 32 + 16

    label PAGE_SETTINGS, 0, SETTINGS_THEME_STRING_ID, FONT_TEXT_ID, ES_CENTER, 0 px, y vh, 640 vw, 24 vh
    %assign y y + 24 + 8
    combobox PAGE_SETTINGS, SETTINGS_THEME_SELECT_ID, 0, FONT_TEXT_ID, CBS_DROPDOWNLIST | CBS_HASSTRINGS, 160 vw, y vh, 320 vw, 32 vh
    %assign y y + 32 + 16

    button PAGE_SETTINGS, SETTINGS_BACK_BUTTON_ID, BACK_STRING_ID, FONT_TEXT_ID, 0, 160 vw, y vh, 320 vw, 52 vh
