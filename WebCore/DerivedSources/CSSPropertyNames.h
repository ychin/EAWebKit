/* This file is automatically generated from CSSPropertyNames.in by makeprop, do not edit */

#ifndef CSSPropertyNames_h
#define CSSPropertyNames_h

#include <string.h>
#include <wtf/HashFunctions.h>
#include <wtf/HashTraits.h>

namespace WTF {
class AtomicString;
class String;
}

namespace WebCore {

enum CSSPropertyID {
    CSSPropertyInvalid = 0,
#if ENABLE(CSS_VARIABLES)
    CSSPropertyVariable = 1,
#endif
    CSSPropertyColor = 2,
    CSSPropertyDirection = 3,
    CSSPropertyDisplay = 4,
    CSSPropertyFont = 5,
    CSSPropertyFontFamily = 6,
    CSSPropertyFontSize = 7,
    CSSPropertyFontStyle = 8,
    CSSPropertyFontVariant = 9,
    CSSPropertyFontWeight = 10,
    CSSPropertyTextRendering = 11,
    CSSPropertyWebkitFontFeatureSettings = 12,
    CSSPropertyWebkitFontKerning = 13,
    CSSPropertyWebkitFontSmoothing = 14,
    CSSPropertyWebkitFontVariantLigatures = 15,
    CSSPropertyWebkitLocale = 16,
    CSSPropertyWebkitTextOrientation = 17,
    CSSPropertyWebkitWritingMode = 18,
    CSSPropertyZoom = 19,
    CSSPropertyLineHeight = 20,
    CSSPropertyBackground = 21,
    CSSPropertyBackgroundAttachment = 22,
    CSSPropertyBackgroundClip = 23,
    CSSPropertyBackgroundColor = 24,
    CSSPropertyBackgroundImage = 25,
    CSSPropertyBackgroundOrigin = 26,
    CSSPropertyBackgroundPosition = 27,
    CSSPropertyBackgroundPositionX = 28,
    CSSPropertyBackgroundPositionY = 29,
    CSSPropertyBackgroundRepeat = 30,
    CSSPropertyBackgroundRepeatX = 31,
    CSSPropertyBackgroundRepeatY = 32,
    CSSPropertyBackgroundSize = 33,
    CSSPropertyBorder = 34,
    CSSPropertyBorderBottom = 35,
    CSSPropertyBorderBottomColor = 36,
    CSSPropertyBorderBottomLeftRadius = 37,
    CSSPropertyBorderBottomRightRadius = 38,
    CSSPropertyBorderBottomStyle = 39,
    CSSPropertyBorderBottomWidth = 40,
    CSSPropertyBorderCollapse = 41,
    CSSPropertyBorderColor = 42,
    CSSPropertyBorderImage = 43,
    CSSPropertyBorderImageOutset = 44,
    CSSPropertyBorderImageRepeat = 45,
    CSSPropertyBorderImageSlice = 46,
    CSSPropertyBorderImageSource = 47,
    CSSPropertyBorderImageWidth = 48,
    CSSPropertyBorderLeft = 49,
    CSSPropertyBorderLeftColor = 50,
    CSSPropertyBorderLeftStyle = 51,
    CSSPropertyBorderLeftWidth = 52,
    CSSPropertyBorderRadius = 53,
    CSSPropertyBorderRight = 54,
    CSSPropertyBorderRightColor = 55,
    CSSPropertyBorderRightStyle = 56,
    CSSPropertyBorderRightWidth = 57,
    CSSPropertyBorderSpacing = 58,
    CSSPropertyBorderStyle = 59,
    CSSPropertyBorderTop = 60,
    CSSPropertyBorderTopColor = 61,
    CSSPropertyBorderTopLeftRadius = 62,
    CSSPropertyBorderTopRightRadius = 63,
    CSSPropertyBorderTopStyle = 64,
    CSSPropertyBorderTopWidth = 65,
    CSSPropertyBorderWidth = 66,
    CSSPropertyBottom = 67,
    CSSPropertyBoxShadow = 68,
    CSSPropertyBoxSizing = 69,
    CSSPropertyCaptionSide = 70,
    CSSPropertyClear = 71,
    CSSPropertyClip = 72,
    CSSPropertyWebkitClipPath = 73,
    CSSPropertyContent = 74,
    CSSPropertyCounterIncrement = 75,
    CSSPropertyCounterReset = 76,
    CSSPropertyCursor = 77,
    CSSPropertyEmptyCells = 78,
    CSSPropertyFloat = 79,
    CSSPropertyFontStretch = 80,
    CSSPropertyHeight = 81,
    CSSPropertyImageRendering = 82,
    CSSPropertyLeft = 83,
    CSSPropertyLetterSpacing = 84,
    CSSPropertyListStyle = 85,
    CSSPropertyListStyleImage = 86,
    CSSPropertyListStylePosition = 87,
    CSSPropertyListStyleType = 88,
    CSSPropertyMargin = 89,
    CSSPropertyMarginBottom = 90,
    CSSPropertyMarginLeft = 91,
    CSSPropertyMarginRight = 92,
    CSSPropertyMarginTop = 93,
    CSSPropertyMaxHeight = 94,
    CSSPropertyMaxWidth = 95,
    CSSPropertyMinHeight = 96,
    CSSPropertyMinWidth = 97,
    CSSPropertyObjectFit = 98,
    CSSPropertyOpacity = 99,
    CSSPropertyOrphans = 100,
    CSSPropertyOutline = 101,
    CSSPropertyOutlineColor = 102,
    CSSPropertyOutlineOffset = 103,
    CSSPropertyOutlineStyle = 104,
    CSSPropertyOutlineWidth = 105,
    CSSPropertyOverflow = 106,
    CSSPropertyOverflowWrap = 107,
    CSSPropertyOverflowX = 108,
    CSSPropertyOverflowY = 109,
    CSSPropertyPadding = 110,
    CSSPropertyPaddingBottom = 111,
    CSSPropertyPaddingLeft = 112,
    CSSPropertyPaddingRight = 113,
    CSSPropertyPaddingTop = 114,
    CSSPropertyPage = 115,
    CSSPropertyPageBreakAfter = 116,
    CSSPropertyPageBreakBefore = 117,
    CSSPropertyPageBreakInside = 118,
    CSSPropertyPointerEvents = 119,
    CSSPropertyPosition = 120,
    CSSPropertyQuotes = 121,
    CSSPropertyResize = 122,
    CSSPropertyRight = 123,
    CSSPropertySize = 124,
    CSSPropertySrc = 125,
    CSSPropertySpeak = 126,
    CSSPropertyTableLayout = 127,
    CSSPropertyTabSize = 128,
    CSSPropertyTextAlign = 129,
    CSSPropertyTextDecoration = 130,
    CSSPropertyTextIndent = 131,
    CSSPropertyTextLineThrough = 132,
    CSSPropertyTextLineThroughColor = 133,
    CSSPropertyTextLineThroughMode = 134,
    CSSPropertyTextLineThroughStyle = 135,
    CSSPropertyTextLineThroughWidth = 136,
    CSSPropertyTextOverflow = 137,
    CSSPropertyTextOverline = 138,
    CSSPropertyTextOverlineColor = 139,
    CSSPropertyTextOverlineMode = 140,
    CSSPropertyTextOverlineStyle = 141,
    CSSPropertyTextOverlineWidth = 142,
    CSSPropertyTextShadow = 143,
    CSSPropertyTextTransform = 144,
    CSSPropertyTextUnderline = 145,
    CSSPropertyTextUnderlineColor = 146,
    CSSPropertyTextUnderlineMode = 147,
    CSSPropertyTextUnderlineStyle = 148,
    CSSPropertyTextUnderlineWidth = 149,
    CSSPropertyTop = 150,
    CSSPropertyTransition = 151,
    CSSPropertyTransitionDelay = 152,
    CSSPropertyTransitionDuration = 153,
    CSSPropertyTransitionProperty = 154,
    CSSPropertyTransitionTimingFunction = 155,
    CSSPropertyUnicodeBidi = 156,
    CSSPropertyUnicodeRange = 157,
    CSSPropertyVerticalAlign = 158,
    CSSPropertyVisibility = 159,
    CSSPropertyWhiteSpace = 160,
    CSSPropertyWidows = 161,
    CSSPropertyWidth = 162,
    CSSPropertyWordBreak = 163,
    CSSPropertyWordSpacing = 164,
    CSSPropertyWordWrap = 165,
    CSSPropertyZIndex = 166,
    CSSPropertyWebkitAnimation = 167,
    CSSPropertyWebkitAnimationDelay = 168,
    CSSPropertyWebkitAnimationDirection = 169,
    CSSPropertyWebkitAnimationDuration = 170,
    CSSPropertyWebkitAnimationFillMode = 171,
    CSSPropertyWebkitAnimationIterationCount = 172,
    CSSPropertyWebkitAnimationName = 173,
    CSSPropertyWebkitAnimationPlayState = 174,
    CSSPropertyWebkitAnimationTimingFunction = 175,
    CSSPropertyWebkitAppearance = 176,
    CSSPropertyWebkitAspectRatio = 177,
    CSSPropertyWebkitBackfaceVisibility = 178,
    CSSPropertyWebkitBackgroundBlendMode = 179,
    CSSPropertyWebkitBackgroundClip = 180,
    CSSPropertyWebkitBackgroundComposite = 181,
    CSSPropertyWebkitBackgroundOrigin = 182,
    CSSPropertyWebkitBackgroundSize = 183,
    CSSPropertyWebkitBorderAfter = 184,
    CSSPropertyWebkitBorderAfterColor = 185,
    CSSPropertyWebkitBorderAfterStyle = 186,
    CSSPropertyWebkitBorderAfterWidth = 187,
    CSSPropertyWebkitBorderBefore = 188,
    CSSPropertyWebkitBorderBeforeColor = 189,
    CSSPropertyWebkitBorderBeforeStyle = 190,
    CSSPropertyWebkitBorderBeforeWidth = 191,
    CSSPropertyWebkitBorderEnd = 192,
    CSSPropertyWebkitBorderEndColor = 193,
    CSSPropertyWebkitBorderEndStyle = 194,
    CSSPropertyWebkitBorderEndWidth = 195,
    CSSPropertyWebkitBorderFit = 196,
    CSSPropertyWebkitBorderHorizontalSpacing = 197,
    CSSPropertyWebkitBorderImage = 198,
    CSSPropertyWebkitBorderRadius = 199,
    CSSPropertyWebkitBorderStart = 200,
    CSSPropertyWebkitBorderStartColor = 201,
    CSSPropertyWebkitBorderStartStyle = 202,
    CSSPropertyWebkitBorderStartWidth = 203,
    CSSPropertyWebkitBorderVerticalSpacing = 204,
    CSSPropertyWebkitBoxAlign = 205,
    CSSPropertyWebkitBoxDirection = 206,
    CSSPropertyWebkitBoxFlex = 207,
    CSSPropertyWebkitBoxFlexGroup = 208,
    CSSPropertyWebkitBoxLines = 209,
    CSSPropertyWebkitBoxOrdinalGroup = 210,
    CSSPropertyWebkitBoxOrient = 211,
    CSSPropertyWebkitBoxPack = 212,
    CSSPropertyWebkitBoxReflect = 213,
    CSSPropertyWebkitBoxShadow = 214,
    CSSPropertyWebkitColorCorrection = 215,
    CSSPropertyWebkitColumnAxis = 216,
    CSSPropertyWebkitColumnBreakAfter = 217,
    CSSPropertyWebkitColumnBreakBefore = 218,
    CSSPropertyWebkitColumnBreakInside = 219,
    CSSPropertyWebkitColumnCount = 220,
    CSSPropertyWebkitColumnGap = 221,
    CSSPropertyWebkitColumnProgression = 222,
    CSSPropertyWebkitColumnRule = 223,
    CSSPropertyWebkitColumnRuleColor = 224,
    CSSPropertyWebkitColumnRuleStyle = 225,
    CSSPropertyWebkitColumnRuleWidth = 226,
    CSSPropertyWebkitColumnSpan = 227,
    CSSPropertyWebkitColumnWidth = 228,
    CSSPropertyWebkitColumns = 229,
    CSSPropertyWebkitBoxDecorationBreak = 230,
    CSSPropertyWebkitFilter = 231,
    CSSPropertyWebkitAlignContent = 232,
    CSSPropertyWebkitAlignItems = 233,
    CSSPropertyWebkitAlignSelf = 234,
    CSSPropertyWebkitFlex = 235,
    CSSPropertyWebkitFlexBasis = 236,
    CSSPropertyWebkitFlexDirection = 237,
    CSSPropertyWebkitFlexFlow = 238,
    CSSPropertyWebkitFlexGrow = 239,
    CSSPropertyWebkitFlexShrink = 240,
    CSSPropertyWebkitFlexWrap = 241,
    CSSPropertyWebkitJustifyContent = 242,
    CSSPropertyWebkitFontSizeDelta = 243,
    CSSPropertyWebkitGridArea = 244,
    CSSPropertyWebkitGridAutoColumns = 245,
    CSSPropertyWebkitGridAutoRows = 246,
    CSSPropertyWebkitGridColumnEnd = 247,
    CSSPropertyWebkitGridColumnStart = 248,
    CSSPropertyWebkitGridDefinitionColumns = 249,
    CSSPropertyWebkitGridDefinitionRows = 250,
    CSSPropertyWebkitGridRowEnd = 251,
    CSSPropertyWebkitGridRowStart = 252,
    CSSPropertyWebkitGridColumn = 253,
    CSSPropertyWebkitGridRow = 254,
    CSSPropertyWebkitGridTemplate = 255,
    CSSPropertyWebkitGridAutoFlow = 256,
    CSSPropertyWebkitHighlight = 257,
    CSSPropertyWebkitHyphenateCharacter = 258,
    CSSPropertyWebkitHyphenateLimitAfter = 259,
    CSSPropertyWebkitHyphenateLimitBefore = 260,
    CSSPropertyWebkitHyphenateLimitLines = 261,
    CSSPropertyWebkitHyphens = 262,
    CSSPropertyWebkitLineBoxContain = 263,
    CSSPropertyWebkitLineAlign = 264,
    CSSPropertyWebkitLineBreak = 265,
    CSSPropertyWebkitLineClamp = 266,
    CSSPropertyWebkitLineGrid = 267,
    CSSPropertyWebkitLineSnap = 268,
    CSSPropertyWebkitLogicalWidth = 269,
    CSSPropertyWebkitLogicalHeight = 270,
    CSSPropertyWebkitMarginAfterCollapse = 271,
    CSSPropertyWebkitMarginBeforeCollapse = 272,
    CSSPropertyWebkitMarginBottomCollapse = 273,
    CSSPropertyWebkitMarginTopCollapse = 274,
    CSSPropertyWebkitMarginCollapse = 275,
    CSSPropertyWebkitMarginAfter = 276,
    CSSPropertyWebkitMarginBefore = 277,
    CSSPropertyWebkitMarginEnd = 278,
    CSSPropertyWebkitMarginStart = 279,
    CSSPropertyWebkitMarquee = 280,
    CSSPropertyWebkitMarqueeDirection = 281,
    CSSPropertyWebkitMarqueeIncrement = 282,
    CSSPropertyWebkitMarqueeRepetition = 283,
    CSSPropertyWebkitMarqueeSpeed = 284,
    CSSPropertyWebkitMarqueeStyle = 285,
    CSSPropertyWebkitMask = 286,
    CSSPropertyWebkitMaskBoxImage = 287,
    CSSPropertyWebkitMaskBoxImageOutset = 288,
    CSSPropertyWebkitMaskBoxImageRepeat = 289,
    CSSPropertyWebkitMaskBoxImageSlice = 290,
    CSSPropertyWebkitMaskBoxImageSource = 291,
    CSSPropertyWebkitMaskBoxImageWidth = 292,
    CSSPropertyWebkitMaskClip = 293,
    CSSPropertyWebkitMaskComposite = 294,
    CSSPropertyWebkitMaskImage = 295,
    CSSPropertyWebkitMaskOrigin = 296,
    CSSPropertyWebkitMaskPosition = 297,
    CSSPropertyWebkitMaskPositionX = 298,
    CSSPropertyWebkitMaskPositionY = 299,
    CSSPropertyWebkitMaskRepeat = 300,
    CSSPropertyWebkitMaskRepeatX = 301,
    CSSPropertyWebkitMaskRepeatY = 302,
    CSSPropertyWebkitMaskSize = 303,
    CSSPropertyWebkitMaskSourceType = 304,
    CSSPropertyWebkitMaxLogicalWidth = 305,
    CSSPropertyWebkitMaxLogicalHeight = 306,
    CSSPropertyWebkitMinLogicalWidth = 307,
    CSSPropertyWebkitMinLogicalHeight = 308,
    CSSPropertyWebkitNbspMode = 309,
    CSSPropertyWebkitOrder = 310,
    CSSPropertyWebkitPaddingAfter = 311,
    CSSPropertyWebkitPaddingBefore = 312,
    CSSPropertyWebkitPaddingEnd = 313,
    CSSPropertyWebkitPaddingStart = 314,
    CSSPropertyWebkitPerspective = 315,
    CSSPropertyWebkitPerspectiveOrigin = 316,
    CSSPropertyWebkitPerspectiveOriginX = 317,
    CSSPropertyWebkitPerspectiveOriginY = 318,
    CSSPropertyWebkitPrintColorAdjust = 319,
    CSSPropertyWebkitRtlOrdering = 320,
    CSSPropertyWebkitRubyPosition = 321,
    CSSPropertyWebkitTextCombine = 322,
    CSSPropertyWebkitTextDecorationsInEffect = 323,
    CSSPropertyWebkitTextEmphasis = 324,
    CSSPropertyWebkitTextEmphasisColor = 325,
    CSSPropertyWebkitTextEmphasisPosition = 326,
    CSSPropertyWebkitTextEmphasisStyle = 327,
    CSSPropertyWebkitTextFillColor = 328,
    CSSPropertyWebkitTextSecurity = 329,
    CSSPropertyWebkitTextStroke = 330,
    CSSPropertyWebkitTextStrokeColor = 331,
    CSSPropertyWebkitTextStrokeWidth = 332,
    CSSPropertyWebkitTransform = 333,
    CSSPropertyWebkitTransformOrigin = 334,
    CSSPropertyWebkitTransformOriginX = 335,
    CSSPropertyWebkitTransformOriginY = 336,
    CSSPropertyWebkitTransformOriginZ = 337,
    CSSPropertyWebkitTransformStyle = 338,
    CSSPropertyWebkitTransition = 339,
    CSSPropertyWebkitTransitionDelay = 340,
    CSSPropertyWebkitTransitionDuration = 341,
    CSSPropertyWebkitTransitionProperty = 342,
    CSSPropertyWebkitTransitionTimingFunction = 343,
    CSSPropertyWebkitUserDrag = 344,
    CSSPropertyWebkitUserModify = 345,
    CSSPropertyWebkitUserSelect = 346,
    CSSPropertyWebkitFlowInto = 347,
    CSSPropertyWebkitFlowFrom = 348,
    CSSPropertyWebkitRegionFragment = 349,
    CSSPropertyWebkitRegionBreakAfter = 350,
    CSSPropertyWebkitRegionBreakBefore = 351,
    CSSPropertyWebkitRegionBreakInside = 352,
    CSSPropertyWebkitTapHighlightColor = 353,
    CSSPropertyBufferedRendering = 354,
    CSSPropertyClipPath = 355,
    CSSPropertyClipRule = 356,
    CSSPropertyMask = 357,
    CSSPropertyEnableBackground = 358,
    CSSPropertyFilter = 359,
    CSSPropertyFloodColor = 360,
    CSSPropertyFloodOpacity = 361,
    CSSPropertyLightingColor = 362,
    CSSPropertyStopColor = 363,
    CSSPropertyStopOpacity = 364,
    CSSPropertyColorInterpolation = 365,
    CSSPropertyColorInterpolationFilters = 366,
    CSSPropertyColorProfile = 367,
    CSSPropertyColorRendering = 368,
    CSSPropertyFill = 369,
    CSSPropertyFillOpacity = 370,
    CSSPropertyFillRule = 371,
    CSSPropertyMarker = 372,
    CSSPropertyMarkerEnd = 373,
    CSSPropertyMarkerMid = 374,
    CSSPropertyMarkerStart = 375,
    CSSPropertyMaskType = 376,
    CSSPropertyShapeRendering = 377,
    CSSPropertyStroke = 378,
    CSSPropertyStrokeDasharray = 379,
    CSSPropertyStrokeDashoffset = 380,
    CSSPropertyStrokeLinecap = 381,
    CSSPropertyStrokeLinejoin = 382,
    CSSPropertyStrokeMiterlimit = 383,
    CSSPropertyStrokeOpacity = 384,
    CSSPropertyStrokeWidth = 385,
    CSSPropertyAlignmentBaseline = 386,
    CSSPropertyBaselineShift = 387,
    CSSPropertyDominantBaseline = 388,
    CSSPropertyGlyphOrientationHorizontal = 389,
    CSSPropertyGlyphOrientationVertical = 390,
    CSSPropertyKerning = 391,
    CSSPropertyTextAnchor = 392,
    CSSPropertyVectorEffect = 393,
    CSSPropertyWritingMode = 394,
    CSSPropertyWebkitSvgShadow = 395,
};

const int firstCSSProperty = 2;
const int numCSSProperties = 394;
const int lastCSSProperty = 395;
const size_t maxCSSPropertyNameLength = 34;

const char* getPropertyName(CSSPropertyID);
const WTF::AtomicString& getPropertyNameAtomicString(CSSPropertyID id);
WTF::String getPropertyNameString(CSSPropertyID id);
WTF::String getJSPropertyName(CSSPropertyID);

inline CSSPropertyID convertToCSSPropertyID(int value)
{
    ASSERT((value >= firstCSSProperty && value <= lastCSSProperty) || value == CSSPropertyInvalid);
    return static_cast<CSSPropertyID>(value);
}

} // namespace WebCore

namespace WTF {
template<> struct DefaultHash<WebCore::CSSPropertyID> { typedef IntHash<unsigned> Hash; };
template<> struct HashTraits<WebCore::CSSPropertyID> : GenericHashTraits<WebCore::CSSPropertyID> {
    static const bool emptyValueIsZero = true;
    static const bool needsDestruction = false;
    static void constructDeletedValue(WebCore::CSSPropertyID& slot) { slot = static_cast<WebCore::CSSPropertyID>(WebCore::lastCSSProperty + 1); }
    static bool isDeletedValue(WebCore::CSSPropertyID value) { return value == (WebCore::lastCSSProperty + 1); }
};
}

#endif // CSSPropertyNames_h
