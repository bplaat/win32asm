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

    int32_t (__stdcall *SetLineSpacing)(IDWriteTextFormat *This, uint32_t lineSpacingMethod, float lineSpacing, float baseline);
    uint8_t padding2[17 * sizeof(void *)];
} IDWriteTextFormatVtbl;

#define IDWriteTextFormat_SetTextAlignment(This, textAlignment) ((IDWriteTextFormat *)This)->lpVtbl->SetTextAlignment((IDWriteTextFormat *)This, textAlignment)
#define IDWriteTextFormat_SetParagraphAlignment(This, paragraphAlignment) ((IDWriteTextFormat *)This)->lpVtbl->SetParagraphAlignment((IDWriteTextFormat *)This, paragraphAlignment)
#define IDWriteTextFormat_SetLineSpacing(This, lineSpacingMethod, lineSpacing, baseline) ((IDWriteTextFormat *)This)->lpVtbl->SetLineSpacing((IDWriteTextFormat *)This, lineSpacingMethod, lineSpacing, baseline)

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

    int32_t (__stdcall *CreateTextFormat)(IDWriteFactory *This, const wchar_t *fontFamilyName, void *fontCollection, uint32_t fontWeight, uint32_t fontStyle,
        uint32_t fontStretch, float fontSize, const wchar_t *localeName, IDWriteTextFormat **textFormat);

    uint8_t padding2[8 * sizeof(void *)];
} IDWriteFactoryVtbl;

#define IDWriteFactory_CreateTextFormat(This, fontFamilyName, fontCollection, fontWeight, fontStyle, fontStretch, fontSize, localeName, textFormat) \
    ((IDWriteFactory *)This)->lpVtbl->CreateTextFormat((IDWriteFactory *)This, fontFamilyName, fontCollection, fontWeight, fontStyle, fontStretch, fontSize, localeName, textFormat)

struct IDWriteFactory {
    const IDWriteFactoryVtbl *lpVtbl;
};

#define DWRITE_FACTORY_TYPE_SHARED 0

extern int32_t __stdcall DWriteCreateFactory(uint32_t factoryType, GUID *riid, IDWriteFactory **factory);

#endif
