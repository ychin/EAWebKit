/* C++ code produced by gperf version 3.0.3 */
/* Command-line: gperf --key-positions='*' -D -s 2 --output-file=./ColorData.cpp /cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"

#include "HashTools.h"
#include <string.h>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wdeprecated-register"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#endif

namespace WebCore {
enum
  {
    TOTAL_KEYWORDS = 151,
    MIN_WORD_LENGTH = 3,
    MAX_WORD_LENGTH = 20,
    MIN_HASH_VALUE = 3,
    MAX_HASH_VALUE = 1055
  };

/* maximum key range = 1053, duplicates = 0 */

class ColorDataHash
{
private:
  static inline unsigned int colordata_hash_function (const char *str, unsigned int len);
public:
  static const struct NamedColor *findColorImpl (const char *str, unsigned int len);
};

inline unsigned int
ColorDataHash::colordata_hash_function (const char *str, unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056,    5,    0,  105,
         0,    0,   30,   40,   40,   10,    0,    0,   15,   60,
         0,    5,  255,   40,    0,   10,   15,  130,  300,  215,
         5,    0,    0, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056
    };
  int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[19]];
      /*FALLTHROUGH*/
      case 19:
        hval += asso_values[(unsigned char)str[18]];
      /*FALLTHROUGH*/
      case 18:
        hval += asso_values[(unsigned char)str[17]];
      /*FALLTHROUGH*/
      case 17:
        hval += asso_values[(unsigned char)str[16]];
      /*FALLTHROUGH*/
      case 16:
        hval += asso_values[(unsigned char)str[15]];
      /*FALLTHROUGH*/
      case 15:
        hval += asso_values[(unsigned char)str[14]];
      /*FALLTHROUGH*/
      case 14:
        hval += asso_values[(unsigned char)str[13]];
      /*FALLTHROUGH*/
      case 13:
        hval += asso_values[(unsigned char)str[12]];
      /*FALLTHROUGH*/
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      /*FALLTHROUGH*/
      case 11:
        hval += asso_values[(unsigned char)str[10]+1];
      /*FALLTHROUGH*/
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      /*FALLTHROUGH*/
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

static const struct NamedColor wordlist[] =
  {
#line 147 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"red", 0xffff0000},
#line 58 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkred", 0xff8b0000},
#line 164 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"tan", 0xffd2b48c},
#line 111 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"linen", 0xfffaf0e6},
#line 155 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"sienna", 0xffa0522d},
#line 85 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"indianred", 0xffcd5c5c},
#line 165 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"teal", 0xff008080},
#line 80 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"grey", 0xff808080},
#line 81 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"green", 0xff008000},
#line 79 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"gray", 0xff808080},
#line 51 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkgrey", 0xffa9a9a9},
#line 52 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkgreen", 0xff006400},
#line 31 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"beige", 0xfff5f5dc},
#line 132 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"orange", 0xffffa500},
#line 50 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkgray", 0xffa9a9a9},
#line 133 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"orangered", 0xffff4500},
#line 88 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"khaki", 0xfff0e68c},
#line 153 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"seagreen", 0xff2e8b57},
#line 77 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"gold", 0xffffd700},
#line 56 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkorange", 0xffff8c00},
#line 53 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkkhaki", 0xffbdb76b},
#line 86 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"indigo", 0xff4b0082},
#line 78 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"goldenrod", 0xffdaa520},
#line 113 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"maroon", 0xff800000},
#line 75 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"gainsboro", 0xffdcdcdc},
#line 109 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lime", 0xff00ff00},
#line 82 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"greenyellow", 0xffadff2f},
#line 49 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkgoldenrod", 0xffb8860b},
#line 160 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"slategrey", 0xff708090},
#line 159 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"slategray", 0xff708090},
#line 151 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"salmon", 0xfffa8072},
#line 60 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkseagreen", 0xff8fbc8f},
#line 154 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"seashell", 0xfffff5ee},
#line 59 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darksalmon", 0xffe9967a},
#line 167 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"tomato", 0xffff6347},
#line 166 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"thistle", 0xffd8bfd8},
#line 63 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkslategrey", 0xff2f4f4f},
#line 46 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"cyan", 0xff00ffff},
#line 73 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"forestgreen", 0xff228b22},
#line 69 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"dimgrey", 0xff696969},
#line 62 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkslategray", 0xff2f4f4f},
#line 125 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"mistyrose", 0xffffe4e1},
#line 68 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"dimgray", 0xff696969},
#line 48 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkcyan", 0xff008b8b},
#line 33 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"black", 0xff000000},
#line 112 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"magenta", 0xffff00ff},
#line 110 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"limegreen", 0xff32cd32},
#line 42 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"coral", 0xffff7f50},
#line 54 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkmagenta", 0xff8b008b},
#line 30 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"azure", 0xfff0ffff},
#line 35 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"blue", 0xff0000ff},
#line 129 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"oldlace", 0xfffdf5e6},
#line 44 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"cornsilk", 0xfffff8dc},
#line 47 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkblue", 0xff00008b},
#line 157 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"skyblue", 0xff87ceeb},
#line 71 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"firebrick", 0xffb22222},
#line 134 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"orchid", 0xffda70d6},
#line 98 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightgrey", 0xffd3d3d3},
#line 99 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightgreen", 0xff90ee90},
#line 108 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightyellow", 0xffffffe0},
#line 97 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightgray", 0xffd3d3d3},
#line 57 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkorchid", 0xff9932cc},
#line 149 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"royalblue", 0xff4169e1},
#line 28 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"aqua", 0xff00ffff},
#line 163 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"steelblue", 0xff4682b4},
#line 32 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"bisque", 0xffffe4c4},
#line 45 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"crimson", 0xffdc143c},
#line 158 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"slateblue", 0xff6a5acd},
#line 70 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"dodgerblue", 0xff1e90ff},
#line 34 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"blanchedalmond", 0xffffebcd},
#line 102 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightseagreen", 0xff20b2aa},
#line 106 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightslategrey", 0xff778899},
#line 105 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightslategray", 0xff778899},
#line 37 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"brown", 0xffa52a2a},
#line 101 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightsalmon", 0xffffa07a},
#line 161 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"snow", 0xfffffafa},
#line 95 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightcyan", 0xffe0ffff},
#line 148 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"rosybrown", 0xffbc8f8f},
#line 152 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"sandybrown", 0xfff4a460},
#line 61 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkslateblue", 0xff483d8b},
#line 175 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"yellow", 0xffffff00},
#line 94 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightcoral", 0xfff08080},
#line 124 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"mintcream", 0xfff5fffa},
#line 29 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"aquamarine", 0xff7fffd4},
#line 150 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"saddlebrown", 0xff8b4513},
#line 83 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"honeydew", 0xfff0fff0},
#line 142 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"pink", 0xffffc0cb},
#line 93 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightblue", 0xffadd8e6},
#line 39 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"cadetblue", 0xff5f9ea0},
#line 172 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"wheat", 0xfff5deb3},
#line 91 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lawngreen", 0xff7cfc00},
#line 173 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"white", 0xffffffff},
#line 26 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"aliceblue", 0xfff0f8ff},
#line 41 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"chocolate", 0xffd2691e},
#line 176 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"yellowgreen", 0xff9acd32},
#line 126 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"moccasin", 0xffffe4b5},
#line 128 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"navy", 0xff000080},
#line 40 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"chartreuse", 0xff7fff00},
#line 87 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"ivory", 0xfffffff0},
#line 136 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"palegreen", 0xff98fb98},
#line 89 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lavender", 0xffe6e6fa},
#line 84 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"hotpink", 0xffff69b4},
#line 130 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"olive", 0xff808000},
#line 74 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"fuchsia", 0xffff00ff},
#line 118 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"mediumseagreen", 0xff3cb371},
#line 156 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"silver", 0xffc0c0c0},
#line 131 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"olivedrab", 0xff6b8e23},
#line 64 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkturquoise", 0xff00ced1},
#line 169 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"turquoise", 0xff40e0d0},
#line 170 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"violet", 0xffee82ee},
#line 171 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"violetred", 0xffd02090},
#line 65 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkviolet", 0xff9400d3},
#line 135 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"palegoldenrod", 0xffeee8aa},
#line 174 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"whitesmoke", 0xfff5f5f5},
#line 162 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"springgreen", 0xff00ff7f},
#line 38 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"burlywood", 0xffdeb887},
#line 141 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"peru", 0xffcd853f},
#line 72 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"floralwhite", 0xfffffaf0},
#line 100 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightpink", 0xffffb6c1},
#line 55 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"darkolivegreen", 0xff556b2f},
#line 76 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"ghostwhite", 0xfff8f8ff},
#line 115 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"mediumblue", 0xff0000cd},
#line 116 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"mediumorchid", 0xffba55d3},
#line 107 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightsteelblue", 0xffb0c4de},
#line 104 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightslateblue", 0xff8470ff},
#line 168 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"transparent", 0x00000000},
#line 67 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"deepskyblue", 0xff00bfff},
#line 103 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightskyblue", 0xff87cefa},
#line 96 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lightgoldenrodyellow", 0xfffafad2},
#line 143 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"plum", 0xffdda0dd},
#line 114 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"mediumaquamarine", 0xff66cdaa},
#line 119 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"mediumslateblue", 0xff7b68ee},
#line 36 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"blueviolet", 0xff8a2be2},
#line 123 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"midnightblue", 0xff191970},
#line 66 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"deeppink", 0xffff1493},
#line 92 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lemonchiffon", 0xfffffacd},
#line 27 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"antiquewhite", 0xfffaebd7},
#line 137 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"paleturquoise", 0xffafeeee},
#line 144 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"powderblue", 0xffb0e0e6},
#line 127 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"navajowhite", 0xffffdead},
#line 120 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"mediumspringgreen", 0xff00fa9a},
#line 43 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"cornflowerblue", 0xff6495ed},
#line 138 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"palevioletred", 0xffdb7093},
#line 122 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"mediumvioletred", 0xffc71585},
#line 145 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"purple", 0xff800080},
#line 146 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"rebeccapurple", 0xff663399},
#line 90 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"lavenderblush", 0xfffff0f5},
#line 121 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"mediumturquoise", 0xff48d1cc},
#line 140 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"peachpuff", 0xffffdab9},
#line 117 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"mediumpurple", 0xff9370db},
#line 139 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"
    {"papayawhip", 0xffffefd5}
  };

static const short lookup[] =
  {
     -1,  -1,  -1,   0,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,   2,  -1,  -1,  -1,  -1,  -1,  -1,
      3,   4,  -1,  -1,   5,  -1,  -1,  -1,  -1,   6,
     -1,  -1,  -1,  -1,   7,   8,  -1,  -1,  -1,   9,
     -1,  -1,  -1,  10,  11,  12,  13,  -1,  14,  15,
     16,  -1,  -1,  17,  18,  19,  -1,  -1,  -1,  20,
     -1,  21,  -1,  -1,  22,  -1,  -1,  -1,  -1,  -1,
     -1,  23,  -1,  -1,  24,  -1,  -1,  -1,  -1,  25,
     -1,  26,  -1,  27,  28,  -1,  -1,  -1,  -1,  29,
     -1,  30,  31,  32,  -1,  -1,  -1,  -1,  -1,  -1,
     33,  34,  35,  36,  37,  -1,  38,  39,  40,  41,
     -1,  -1,  42,  43,  -1,  -1,  -1,  -1,  -1,  -1,
     44,  -1,  45,  -1,  46,  47,  48,  -1,  -1,  -1,
     49,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,
     -1,  -1,  51,  52,  -1,  -1,  -1,  -1,  53,  -1,
     -1,  -1,  54,  -1,  55,  -1,  56,  -1,  -1,  57,
     58,  59,  -1,  -1,  60,  61,  -1,  -1,  -1,  62,
     -1,  -1,  -1,  -1,  63,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  64,  -1,  65,  66,  -1,  67,
     68,  -1,  -1,  -1,  69,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  70,  71,
     -1,  -1,  -1,  -1,  72,  73,  -1,  -1,  -1,  -1,
     -1,  74,  -1,  -1,  75,  -1,  -1,  -1,  -1,  76,
     -1,  -1,  -1,  -1,  77,  78,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  79,  -1,  -1,  80,  -1,  -1,  -1,
     81,  -1,  -1,  -1,  82,  83,  84,  -1,  85,  86,
     -1,  -1,  -1,  -1,  87,  -1,  -1,  -1,  -1,  88,
     89,  -1,  -1,  -1,  90,  91,  -1,  -1,  -1,  92,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  93,  -1,  94,  -1,  95,  96,
     -1,  -1,  -1,  -1,  -1,  97,  -1,  -1,  -1,  -1,
     98,  -1,  -1,  -1,  99,  -1,  -1,  -1, 100,  -1,
     -1,  -1, 101,  -1,  -1, 102,  -1, 103,  -1, 104,
     -1, 105,  -1,  -1, 106,  -1,  -1,  -1, 107, 108,
     -1, 109,  -1,  -1, 110,  -1,  -1,  -1,  -1,  -1,
    111,  -1,  -1, 112,  -1, 113,  -1,  -1,  -1,  -1,
     -1, 114,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 115,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 116,
     -1, 117,  -1,  -1, 118,  -1,  -1,  -1,  -1, 119,
    120,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 121,  -1,  -1,  -1,  -1,
     -1,  -1, 122,  -1, 123,  -1,  -1,  -1,  -1, 124,
     -1, 125,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 126,  -1,  -1,  -1,  -1,  -1, 127,  -1,  -1,
    128,  -1,  -1,  -1, 129,  -1,  -1,  -1,  -1,  -1,
     -1, 130,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 131,  -1,  -1,  -1,  -1,
    132,  -1, 133,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 134,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 135,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 136,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 137,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    138,  -1,  -1,  -1,  -1,  -1, 139, 140,  -1, 141,
     -1,  -1,  -1, 142,  -1,  -1,  -1,  -1,  -1,  -1,
    143,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 144,  -1,  -1,  -1,  -1,  -1,  -1, 145,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 146,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 147,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 148,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 149,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 150
  };

const struct NamedColor *
ColorDataHash::findColorImpl (const char *str, unsigned int len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
       int key = colordata_hash_function (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
           int index = lookup[key];

          if (index >= 0)
            {
               const char *s = wordlist[index].name;

              if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
                return &wordlist[index];
            }
        }
    }
  return 0;
}
#line 177 "/cygdrive/c/eaos/EAOS/EAWebKit/DL/EAWebKit/dev/WebCore/platform/ColorData.gperf"

const struct NamedColor* findColor(const char* str, unsigned int len)
{
    return ColorDataHash::findColorImpl(str, len);
}

} // namespace WebCore

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
