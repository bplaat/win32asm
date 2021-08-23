#ifndef DIRECTWRITE_H
#define DIRECTWRITE_H

#include "win32.h"

// IDWriteTextFormat
typedef struct IDWriteTextFormat IDWriteTextFormat;

#define DWRITE_TEXT_ALIGNMENT_LEADING 0
#define DWRITE_TEXT_ALIGNMENT_TRAILING 1
#define DWRITE_TEXT_ALIGNMENT_CENTER 2

#define DWRITE_PARAGRAPH_ALIGNMENT_NEAR 0
#define DWRITE_PARAGRAPH_ALIGNMENT_FAR 1
#define DWRITE_PARAGRAPH_ALIGNMENT_CENTER 2

typedef struct IDWriteTextFormatVtbl {
    IUnknownVtbl Base;

    int32_t (__stdcall *SetTextAlignment)(IDWriteTextFormat *This, uint32_t textAlignment);
    int32_t (__stdcall *SetParagraphAlignment)(IDWriteTextFormat *This, uint32_t paragraphAlignment);
    uint8_t padding1[5 * sizeof(void *)];

    int32_t (__stdcall *SetLineSpacing)(IDWriteTextFormat *This, uint32_t spacing, float line_spacing, float baseline);
    uint8_t padding2[17 * sizeof(void *)];
} IDWriteTextFormatVtbl;

#define IDWriteTextFormat_SetTextAlignment(ptr, a) ((IDWriteTextFormat *)ptr)->lpVtbl->SetTextAlignment((IDWriteTextFormat *)ptr, a)
#define IDWriteTextFormat_SetParagraphAlignment(ptr, a) ((IDWriteTextFormat *)ptr)->lpVtbl->SetParagraphAlignment((IDWriteTextFormat *)ptr, a)
#define IDWriteTextFormat_SetLineSpacing(ptr, a, b, c) ((IDWriteTextFormat *)ptr)->lpVtbl->SetLineSpacing((IDWriteTextFormat *)ptr, a, b, c)

struct IDWriteTextFormat {
    const IDWriteTextFormatVtbl *lpVtbl;
};

// IDWriteFactory
typedef struct IDWriteFactory IDWriteFactory;

#define DWRITE_FONT_WEIGHT_NORMAL 4
#define DWRITE_FONT_STYLE_NORMAL 0
#define DWRITE_FONT_STRETCH_NORMAL 5

typedef struct IDWriteFactoryVtbl {
    IUnknownVtbl Base;
    uint8_t padding1[12 * sizeof(void *)];

    int32_t (__stdcall *CreateTextFormat)(IDWriteFactory *This, const wchar_t *family_name, void *collection, uint32_t weight, uint32_t style, uint32_t stretch, float size, const wchar_t *locale, IDWriteTextFormat **format);
    uint8_t padding2[8 * sizeof(void *)];
} IDWriteFactoryVtbl;

#define IDWriteFactory_CreateTextFormat(ptr, a, b, c, d, e, f, g, h) ((IDWriteFactory *)ptr)->lpVtbl->CreateTextFormat((IDWriteFactory *)ptr, a, b, c, d, e, f, g, h)

struct IDWriteFactory {
    const IDWriteFactoryVtbl *lpVtbl;
};

#define DWRITE_FACTORY_TYPE_SHARED 0

extern int32_t __stdcall DWriteCreateFactory(uint32_t factoryType, GUID *riid, IDWriteFactory **factory);

#endif
