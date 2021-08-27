#ifndef DIRECTWRITE_H
#define DIRECTWRITE_H

#include "win32.h"

#define DWRITE_FACTORY_TYPE_SHARED 0

#define DWRITE_TEXT_ALIGNMENT_LEADING 0
#define DWRITE_TEXT_ALIGNMENT_TRAILING 1
#define DWRITE_TEXT_ALIGNMENT_CENTER 2

#define DWRITE_PARAGRAPH_ALIGNMENT_NEAR 0
#define DWRITE_PARAGRAPH_ALIGNMENT_FAR 1
#define DWRITE_PARAGRAPH_ALIGNMENT_CENTER 2

#define DWRITE_FONT_WEIGHT_NORMAL 400
#define DWRITE_FONT_WEIGHT_BOLD 700

#define DWRITE_FONT_STYLE_NORMAL 0
#define DWRITE_FONT_STYLE_ITALIC 2

#define DWRITE_FONT_STRETCH_NORMAL 5

#define DWRITE_MEASURING_MODE_NATURAL 0

typedef struct DWRITE_TEXT_METRICS {
    float left;
    float top;
    float width;
    float widthIncludingTrailingWhitespace;
    float height;
    float layoutWidth;
    float layoutHeight;
    uint32_t maxBidiReorderingDepth;
    uint32_t lineCount;
} DWRITE_TEXT_METRICS;

typedef struct DWRITE_TEXT_RANGE {
    uint32_t startPosition;
    uint32_t length;
} DWRITE_TEXT_RANGE;

// IDWriteTextFormat
typedef struct IDWriteTextFormat IDWriteTextFormat;

typedef struct IDWriteTextFormatVtbl {
    IUnknownVtbl base;

    int32_t __stdcall (*SetTextAlignment)(IDWriteTextFormat *this, uint32_t textAlignment);
    int32_t __stdcall (*SetParagraphAlignment)(IDWriteTextFormat *this, uint32_t paragraphAlignment);
    uint8_t padding1[5 * sizeof(void *)];

    int32_t __stdcall (*SetLineSpacing)(IDWriteTextFormat *this, uint32_t lineSpacingMethod, float lineSpacing, float baseline);
    uint8_t padding2[17 * sizeof(void *)];
} IDWriteTextFormatVtbl;

#define IDWriteTextFormat_SetTextAlignment(this, textAlignment) ((IDWriteTextFormat *)this)->lpVtbl->SetTextAlignment((IDWriteTextFormat *)this, textAlignment)
#define IDWriteTextFormat_SetParagraphAlignment(this, paragraphAlignment) ((IDWriteTextFormat *)this)->lpVtbl->SetParagraphAlignment((IDWriteTextFormat *)this, paragraphAlignment)
#define IDWriteTextFormat_SetLineSpacing(this, lineSpacingMethod, lineSpacing, baseline) ((IDWriteTextFormat *)this)->lpVtbl->SetLineSpacing((IDWriteTextFormat *)this, lineSpacingMethod, lineSpacing, baseline)

struct IDWriteTextFormat {
    const IDWriteTextFormatVtbl *lpVtbl;
};

// IDWriteTextLayout
typedef struct IDWriteTextLayout IDWriteTextLayout;

typedef struct IDWriteTextLayoutVtbl {
    IDWriteTextFormatVtbl base;
    uint8_t padding1[8 * sizeof(void *)];

    int32_t __stdcall (*SetUnderline)(IDWriteTextLayout *this, bool underline, DWRITE_TEXT_RANGE range);
    int32_t __stdcall (*SetStrikethrough)(IDWriteTextLayout *this, bool strikethrough, DWRITE_TEXT_RANGE range);
    uint8_t padding2[22 * sizeof(void *)];

    int32_t __stdcall (*GetMetrics)(IDWriteTextLayout *this, DWRITE_TEXT_METRICS *metrics);
    uint8_t padding3[6 * sizeof(void *)];
} IDWriteTextLayoutVtbl;

#define IDWriteTextLayout_SetUnderline(this, underline, range) ((IDWriteTextLayout *)this)->lpVtbl->SetUnderline((IDWriteTextLayout *)this, underline, range)
#define IDWriteTextLayout_SetStrikethrough(this, strikethrough, range) ((IDWriteTextLayout *)this)->lpVtbl->SetStrikethrough((IDWriteTextLayout *)this, strikethrough, range)
#define IDWriteTextLayout_GetMetrics(this, metrics) ((IDWriteTextLayout *)this)->lpVtbl->GetMetrics((IDWriteTextLayout *)this, metrics)

struct IDWriteTextLayout {
    const IDWriteTextLayoutVtbl *lpVtbl;
};

// IDWriteFactory
typedef struct IDWriteFactory IDWriteFactory;

typedef struct IDWriteFactoryVtbl {
    IUnknownVtbl base;
    uint8_t padding1[12 * sizeof(void *)];

    int32_t __stdcall (*CreateTextFormat)(IDWriteFactory *this, const wchar_t *fontFamilyName, void *fontCollection, uint32_t fontWeight, uint32_t fontStyle,
        uint32_t fontStretch, float fontSize, const wchar_t *localeName, IDWriteTextFormat **textFormat);
    uint8_t padding2[2 * sizeof(void *)];

    int32_t __stdcall (*CreateTextLayout)(IDWriteFactory *this, const wchar_t *string, uint32_t stringLength, IDWriteTextFormat *textFormat,
        float maxWidth, float maxHeight, IDWriteTextLayout **textLayout);
    uint8_t padding3[5 * sizeof(void *)];
} IDWriteFactoryVtbl;

#define IDWriteFactory_CreateTextFormat(this, fontFamilyName, fontCollection, fontWeight, fontStyle, fontStretch, fontSize, localeName, textFormat) \
    ((IDWriteFactory *)this)->lpVtbl->CreateTextFormat((IDWriteFactory *)this, fontFamilyName, fontCollection, fontWeight, fontStyle, fontStretch, fontSize, localeName, textFormat)
#define IDWriteFactory_CreateTextLayout(this, string, stringLength, textFormat, maxWidth, maxHeight, textLayout) \
    ((IDWriteFactory *)this)->lpVtbl->CreateTextLayout((IDWriteFactory *)this, string, stringLength, textFormat, maxWidth, maxHeight, textLayout)

struct IDWriteFactory {
    const IDWriteFactoryVtbl *lpVtbl;
};

// DWriteCreateFactory
extern int32_t __stdcall DWriteCreateFactory(uint32_t factoryType, const IID *riid, IDWriteFactory **factory);

#endif
