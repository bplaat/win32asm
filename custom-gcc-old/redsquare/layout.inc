%define ES_LEFT 0x0000
%define ES_CENTER 0x0001
%define ES_RIGHT 0x0002
%define ES_MULTILINE 0x0004
%define ES_AUTOHSCROLL 0x0080

%define CBS_DROPDOWNLIST 0x0003
%define CBS_HASSTRINGS 0x0200

%define LVS_LIST 0x0003

%define px , CONTROL_UNIT_PX
%define vw , CONTROL_UNIT_VW
%define vh , CONTROL_UNIT_VH
%define vx , CONTROL_UNIT_VX

%macro widget 14
    db %1 ; Type
    db %2 ; Page
    dw %3 ; Id
    dw %4 ; String id
    dw %5 ; Font id
    dd %6 ; Style

    dd %7.0 ; x
    dd %9.0 ; y
    dd %11.0 ; width
    dd %13.0 ; height

    db %8 ; x unit
    db %10 ; y unit
    db %12 ; width unit
    db %14 ; height unit
%endmacro

%macro font 5
    widget CONTROL_TYPE_FONT, 0, %1, %2, 0, 0, 0, 0, 0, 0, %3, CONTROL_UNIT_PX, %4, %5
%endmacro

%macro label 13
    widget CONTROL_TYPE_LABEL, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13
%endmacro

%macro button 13
    widget CONTROL_TYPE_BUTTON, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13
%endmacro

%macro edit 13
    widget CONTROL_TYPE_EDIT, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13
%endmacro

%macro combobox 13
    widget CONTROL_TYPE_COMBOBOX, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13
%endmacro

%macro list 13
    widget CONTROL_TYPE_LIST, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13
%endmacro
