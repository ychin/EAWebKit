%{

/*
 *  Copyright (C) 2002-2003 Lars Knoll (knoll@kde.org)
 *  Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 *  Copyright (C) 2006 Alexey Proskuryakov (ap@nypop.com)
 *  Copyright (C) 2008 Eric Seidel <eric@webkit.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "config.h"

#include "CSSParser.h"
#include "CSSParserMode.h"
#include "CSSPrimitiveValue.h"
#include "CSSPropertyNames.h"
#include "CSSSelector.h"
#include "CSSSelectorList.h"
#include "Document.h"
#include "HTMLNames.h"
#include "MediaList.h"
#include "MediaQueryExp.h"
#include "StyleRule.h"
#include "StyleSheetContents.h"
#include "WebKitCSSKeyframeRule.h"
#include "WebKitCSSKeyframesRule.h"
#include <wtf/FastMalloc.h>
#include <stdlib.h>
#include <string.h>

using namespace WebCore;
using namespace HTMLNames;

#define YYMALLOC fastMalloc
#define YYFREE fastFree

#define YYENABLE_NLS 0
#define YYLTYPE_IS_TRIVIAL 1
#define YYMAXDEPTH 10000
#define YYDEBUG 0

#if YYDEBUG > 0
#include <wtf/text/CString.h>
#define YYPRINT(File,Type,Value) if (isCSSTokenAString(Type)) YYFPRINTF(File, "%s", String((Value).string).utf8().data())
#endif

%}
%pure_parser
%parse-param { CSSParser* parser }
%lex-param { CSSParser* parser }
%union {
    double number;
    CSSParserString string;
    CSSSelector::MarginBoxType marginBox;
    CSSParserValue value;
}
%{
static inline int cssyyerror(void*, const char*)
{
    return 1;
}
%}
%expect 30
%nonassoc LOWEST_PREC
%left UNIMPORTANT_TOK
%token WHITESPACE SGML_CD
%token TOKEN_EOF 0
%token INCLUDES
%token DASHMATCH
%token BEGINSWITH
%token ENDSWITH
%token CONTAINS
%token <string> STRING
%right <string> IDENT
%token <string> NTH
%nonassoc <string> HEX
%nonassoc <string> IDSEL
%nonassoc ':'
%nonassoc '.'
%nonassoc '['
%nonassoc <string> '*'
%nonassoc error
%left '|'
%token IMPORT_SYM
%token PAGE_SYM
%token MEDIA_SYM
%token FONT_FACE_SYM
%token CHARSET_SYM
%token NAMESPACE_SYM
%token VARFUNCTION
%token WEBKIT_RULE_SYM
%token WEBKIT_DECLS_SYM
%token WEBKIT_KEYFRAME_RULE_SYM
%token WEBKIT_KEYFRAMES_SYM
%token WEBKIT_VALUE_SYM
%token WEBKIT_MEDIAQUERY_SYM
%token WEBKIT_SELECTOR_SYM
%token WEBKIT_REGION_RULE_SYM
%token WEBKIT_VIEWPORT_RULE_SYM
%token <marginBox> TOPLEFTCORNER_SYM
%token <marginBox> TOPLEFT_SYM
%token <marginBox> TOPCENTER_SYM
%token <marginBox> TOPRIGHT_SYM
%token <marginBox> TOPRIGHTCORNER_SYM
%token <marginBox> BOTTOMLEFTCORNER_SYM
%token <marginBox> BOTTOMLEFT_SYM
%token <marginBox> BOTTOMCENTER_SYM
%token <marginBox> BOTTOMRIGHT_SYM
%token <marginBox> BOTTOMRIGHTCORNER_SYM
%token <marginBox> LEFTTOP_SYM
%token <marginBox> LEFTMIDDLE_SYM
%token <marginBox> LEFTBOTTOM_SYM
%token <marginBox> RIGHTTOP_SYM
%token <marginBox> RIGHTMIDDLE_SYM
%token <marginBox> RIGHTBOTTOM_SYM
%token ATKEYWORD
%token IMPORTANT_SYM
%token MEDIA_ONLY
%token MEDIA_NOT
%token MEDIA_AND
%token <number> REMS
%token <number> CHS
%token <number> QEMS
%token <number> EMS
%token <number> EXS
%token <number> PXS
%token <number> CMS
%token <number> MMS
%token <number> INS
%token <number> PTS
%token <number> PCS
%token <number> DEGS
%token <number> RADS
%token <number> GRADS
%token <number> TURNS
%token <number> MSECS
%token <number> SECS
%token <number> HERTZ
%token <number> KHERTZ
%token <string> DIMEN
%token <string> INVALIDDIMEN
%token <number> PERCENTAGE
%token <number> FLOATTOKEN
%token <number> INTEGER
%token <number> VW
%token <number> VH
%token <number> VMIN
%token <number> VMAX
%token <number> DPPX
%token <number> DPI
%token <number> DPCM
%token <number> FR
%token <string> URI
%token <string> FUNCTION
%token <string> ANYFUNCTION
%token <string> NOTFUNCTION
%token <string> CALCFUNCTION
%token <string> MINFUNCTION
%token <string> MAXFUNCTION
%token <string> VAR_DEFINITION
%token <string> UNICODERANGE
%union { CSSSelector::Relation relation; }
%type <relation> combinator
%union { StyleRuleBase* rule; }
%type <rule> block_rule block_valid_rule font_face import keyframes media page region rule ruleset valid_rule
%destructor { if ($$) $$->deref(); } block_rule block_valid_rule font_face import keyframes media page region rule ruleset valid_rule
%union { Vector<RefPtr<StyleRuleBase>>* ruleList; }
%type <ruleList> block_rule_list block_valid_rule_list
%destructor { delete $$; } block_rule_list block_valid_rule_list
%type <string> ident_or_string maybe_ns_prefix namespace_selector string_or_uri
%type <marginBox> margin_sym
%union { MediaQuerySet* mediaList; }
%type <mediaList> media_list maybe_media_list
%destructor { if ($$) $$->deref(); } media_list maybe_media_list
%union { MediaQuery* mediaQuery; }
%type <mediaQuery> media_query
%destructor { delete $$; } media_query
%union { MediaQuery::Restrictor mediaQueryRestrictor; }
%type <mediaQueryRestrictor> maybe_media_restrictor
%union { MediaQueryExp* mediaQueryExp; }
%type <mediaQueryExp> media_query_exp
%destructor { delete $$; } media_query_exp
%union { Vector<OwnPtr<MediaQueryExp>>* mediaQueryExpList; }
%type <mediaQueryExpList> media_query_exp_list maybe_and_media_query_exp_list
%destructor { delete $$; } media_query_exp_list maybe_and_media_query_exp_list
%type <string> keyframe_name
%union { StyleKeyframe* keyframe; }
%type <keyframe> keyframe_rule
%destructor { if ($$) $$->deref(); } keyframe_rule
%union { Vector<RefPtr<StyleKeyframe>>* keyframeRuleList; }
%type <keyframeRuleList> keyframes_rule
%destructor { delete $$; } keyframes_rule
%type <value> key unary_term
%type <value> calc_func_term calc_function function min_or_max_function term
%destructor { destroy($$); } calc_func_term calc_function function min_or_max_function term
%union { CSSPropertyID id; }
%type <id> property
%union { CSSParserSelector* selector; }
%type <selector> attrib class page_selector pseudo pseudo_page selector simple_selector specifier specifier_list
%destructor { delete $$; } attrib class page_selector pseudo pseudo_page selector simple_selector specifier specifier_list
%union { Vector<OwnPtr<CSSParserSelector>>* selectorList; }
%type <selectorList> selector_list simple_selector_list
%destructor { delete $$; } selector_list simple_selector_list
%union { bool boolean; }
%type <boolean> declaration declaration_list decl_list priority
%union { CSSSelector::Match match; }
%type <match> match
%union { int integer; }
%type <integer> unary_operator maybe_unary_operator
%union { char character; }
%type <character> operator calc_func_operator
%union { CSSParserValueList* valueList; }
%type <valueList> calc_func_expr calc_func_expr_list calc_func_paren_expr expr key_list maybe_media_value valid_calc_func_expr valid_expr
%destructor { delete $$; } calc_func_expr calc_func_expr_list calc_func_paren_expr expr key_list maybe_media_value valid_calc_func_expr valid_expr
%type <string> min_or_max
%type <string> element_name
%union { CSSParser::Location location; }
%type <location> error_location
%%
stylesheet:
    maybe_space maybe_charset maybe_sgml rule_list
  | webkit_rule maybe_space
  | webkit_decls maybe_space
  | webkit_value maybe_space
  | webkit_mediaquery maybe_space
  | webkit_selector maybe_space
  | webkit_keyframe_rule maybe_space
  ;
webkit_rule: WEBKIT_RULE_SYM '{' maybe_space valid_rule maybe_space '}' { parser->m_rule = adoptRef($4); } ;
webkit_keyframe_rule: WEBKIT_KEYFRAME_RULE_SYM '{' maybe_space keyframe_rule maybe_space '}' { parser->m_keyframe = adoptRef($4); } ;
webkit_decls: WEBKIT_DECLS_SYM '{' maybe_space_before_declaration declaration_list '}' ;
webkit_value:
    WEBKIT_VALUE_SYM '{' maybe_space expr '}' {
        if ($4) {
            parser->m_valueList = adoptPtr($4);
            int oldParsedProperties = parser->m_parsedProperties.size();
            if (!parser->parseValue(parser->m_id, parser->m_important))
                parser->rollbackLastProperties(parser->m_parsedProperties.size() - oldParsedProperties);
            parser->m_valueList = nullptr;
        }
    }
;
webkit_mediaquery: WEBKIT_MEDIAQUERY_SYM WHITESPACE maybe_space media_query '}' { parser->m_mediaQuery = adoptPtr($4); } ;
webkit_selector:
    WEBKIT_SELECTOR_SYM '{' maybe_space selector_list '}' {
        if ($4) {
            if (parser->m_selectorListForParseSelector)
                parser->m_selectorListForParseSelector->adoptSelectorVector(*$4);
            parser->recycleSelectorVector(adoptPtr($4));
        }
    }
;
maybe_space: %prec UNIMPORTANT_TOK | maybe_space WHITESPACE ;
maybe_sgml: | maybe_sgml SGML_CD | maybe_sgml WHITESPACE ;
maybe_charset: | charset ;
closing_brace: '}' | %prec LOWEST_PREC TOKEN_EOF ;
closing_parenthesis: ')' | %prec LOWEST_PREC TOKEN_EOF ;
charset:
  CHARSET_SYM maybe_space STRING maybe_space ';' {
     if (parser->m_styleSheet)
         parser->m_styleSheet->parserSetEncodingFromCharsetRule($3);
     if (parser->isExtractingSourceData() && parser->m_currentRuleDataStack->isEmpty() && parser->m_ruleSourceDataResult)
         parser->addNewRuleToSourceTree(CSSRuleSourceData::createUnknown());
  }
  | CHARSET_SYM error invalid_block
  | CHARSET_SYM error ';'
;
ignored_charset: CHARSET_SYM maybe_space STRING maybe_space ';' | CHARSET_SYM maybe_space ';' ;
rule_list:
    | rule_list rule maybe_sgml {
        if (RefPtr<StyleRuleBase> rule = adoptRef($2)) {
            if (parser->m_styleSheet)
                parser->m_styleSheet->parserAppendRule(rule.release());
        }
    }
    ;
valid_rule:
    ruleset
  | media
  | page
  | font_face
  | keyframes
  | namespace { $$ = nullptr; }
  | import
  | region
  ;
rule:
    valid_rule {
        $$ = $1;
        parser->m_hadSyntacticallyValidCSSRule = true;
    }
    | ignored_charset { $$ = nullptr; }
    | invalid_rule { $$ = nullptr; }
    | invalid_at { $$ = nullptr; }
    ;
block_rule_list:
                { $$ = nullptr; }
  | block_rule_list block_rule maybe_sgml {
      $$ = $1;
      if (RefPtr<StyleRuleBase> rule = adoptRef($2)) {
          if (!$$)
              $$ = new Vector<RefPtr<StyleRuleBase>>;
          $$->append(rule.release());
      }
  }
  ;
block_valid_rule_list:
                { $$ = nullptr; }
  | block_valid_rule_list block_valid_rule maybe_sgml {
      $$ = $1;
      if (RefPtr<StyleRuleBase> rule = adoptRef($2)) {
          if (!$$)
              $$ = new Vector<RefPtr<StyleRuleBase>>;
          $$->append(rule.release());
      }
  }
  ;
block_valid_rule:
    ruleset
  | page
  | font_face
  | media
  | keyframes
  ;
block_rule: block_valid_rule | invalid_rule { $$ = nullptr; } | invalid_at { $$ = nullptr; } | namespace { $$ = nullptr; } | import | region ;
at_import_header_end_maybe_space:
    maybe_space {
        parser->markRuleHeaderEnd();
        parser->markRuleBodyStart();
    }
    ;
before_import_rule:
                {
        parser->markRuleHeaderStart(CSSRuleSourceData::IMPORT_RULE);
    }
    ;
import:
    before_import_rule IMPORT_SYM at_import_header_end_maybe_space string_or_uri maybe_space maybe_media_list ';' {
        $$ = parser->createImportRule($4, adoptRef($6)).leakRef();
    }
  | before_import_rule IMPORT_SYM at_import_header_end_maybe_space string_or_uri maybe_space maybe_media_list TOKEN_EOF {
        $$ = parser->createImportRule($4, adoptRef($6)).leakRef();
    }
  | before_import_rule IMPORT_SYM at_import_header_end_maybe_space string_or_uri maybe_space maybe_media_list invalid_block {
        $$ = nullptr;
        parser->popRuleData();
        if ($6)
            $6->deref();
    }
  | before_import_rule IMPORT_SYM error ';' {
        $$ = nullptr;
        parser->popRuleData();
    }
  | before_import_rule IMPORT_SYM error invalid_block {
        $$ = nullptr;
        parser->popRuleData();
    }
  ;
namespace:
    NAMESPACE_SYM maybe_space maybe_ns_prefix string_or_uri maybe_space ';' { parser->addNamespace($3, $4); }
    | NAMESPACE_SYM maybe_space maybe_ns_prefix string_or_uri maybe_space invalid_block
    | NAMESPACE_SYM error invalid_block
    | NAMESPACE_SYM error ';'
    ;
maybe_ns_prefix: { $$.clear(); } | IDENT maybe_space;
string_or_uri: STRING | URI ;
maybe_media_value: { $$ = nullptr; } | ':' maybe_space expr maybe_space { $$ = $3; } ;
media_query_exp:
    maybe_media_restrictor maybe_space '(' maybe_space IDENT maybe_space maybe_media_value ')' maybe_space {
        OwnPtr<CSSParserValueList> mediaValue = adoptPtr($7);
        if ($1 != MediaQuery::None)
            $$ = MediaQueryExp::create(emptyString(), nullptr).leakPtr();
        else {
            $5.lower();
            $$ = MediaQueryExp::create($5, mediaValue.get()).leakPtr();
        }
    }
    ;
media_query_exp_list:
    media_query_exp {
        $$ = new Vector<OwnPtr<MediaQueryExp>>;
        $$->append(adoptPtr($1));
    }
    | media_query_exp_list maybe_space MEDIA_AND maybe_space media_query_exp {
        $$ = $1;
        $$->append(adoptPtr($5));
    }
    ;
maybe_and_media_query_exp_list:
              {
        $$ = new Vector<OwnPtr<MediaQueryExp>>;
    }
    | MEDIA_AND maybe_space media_query_exp_list {
        $$ = $3;
    }
    ;
maybe_media_restrictor:
              {
        $$ = MediaQuery::None;
    }
    | MEDIA_ONLY {
        $$ = MediaQuery::Only;
    }
    | MEDIA_NOT {
        $$ = MediaQuery::Not;
    }
    ;
media_query:
    media_query_exp_list {
        $$ = new MediaQuery(MediaQuery::None, "all", adoptPtr($1));
    }
    |
    maybe_media_restrictor maybe_space IDENT maybe_space maybe_and_media_query_exp_list {
        $3.lower();
        $$ = new MediaQuery($1, $3, adoptPtr($5));
    }
    ;
maybe_media_list: { $$ = MediaQuerySet::create().leakRef(); } | media_list ;
media_list:
    media_query {
        $$ = MediaQuerySet::create().leakRef();
        $$->addMediaQuery(adoptPtr($1));
        parser->updateLastMediaLine($$);
    }
    | media_list ',' maybe_space media_query {
        $$ = $1;
        OwnPtr<MediaQuery> mediaQuery = adoptPtr($4);
        if ($$) {
            $$->addMediaQuery(mediaQuery.release());
            parser->updateLastMediaLine($$);
        }
    }
    | media_list error {
        $$ = nullptr;
        if ($1)
            $1->deref();
    }
    ;
at_rule_body_start:
                {
        parser->markRuleBodyStart();
    }
    ;
before_media_rule:
                {
        parser->markRuleHeaderStart(CSSRuleSourceData::MEDIA_RULE);
    }
    ;
at_rule_header_end_maybe_space:
    maybe_space {
        parser->markRuleHeaderEnd();
    }
    ;
media:
    before_media_rule MEDIA_SYM maybe_space media_list at_rule_header_end '{' at_rule_body_start maybe_space block_rule_list save_block {
        $$ = parser->createMediaRule(adoptRef($4), adoptPtr($9).get()).leakRef();
    }
    | before_media_rule MEDIA_SYM at_rule_header_end_maybe_space '{' at_rule_body_start maybe_space block_rule_list save_block {
        $$ = parser->createEmptyMediaRule(adoptPtr($7).get()).leakRef();
    }
    | before_media_rule MEDIA_SYM at_rule_header_end_maybe_space ';' {
        $$ = nullptr;
        parser->popRuleData();
    }
    ;
before_keyframes_rule:
                {
        parser->markRuleHeaderStart(CSSRuleSourceData::KEYFRAMES_RULE);
    }
    ;
keyframes:
    before_keyframes_rule WEBKIT_KEYFRAMES_SYM maybe_space keyframe_name at_rule_header_end_maybe_space '{' at_rule_body_start maybe_space keyframes_rule closing_brace {
        $$ = parser->createKeyframesRule($4, adoptPtr($9)).leakRef();
    }
    ;
keyframe_name: IDENT | STRING ;
keyframes_rule:
                { $$ = new Vector<RefPtr<StyleKeyframe>>; }
    | keyframes_rule keyframe_rule maybe_space {
        $$ = $1;
        if (RefPtr<StyleKeyframe> keyframe = adoptRef($2))
            $$->append(keyframe.release());
    }
    ;
keyframe_rule: key_list maybe_space '{' maybe_space declaration_list closing_brace { $$ = parser->createKeyframe(*adoptPtr($1)).leakRef(); } ;
key_list:
    key {
        $$ = new CSSParserValueList;
        $$->addValue($1);
    }
    | key_list maybe_space ',' maybe_space key {
        $$ = $1;
        ASSERT($5.unit != CSSParserValue::Function);
        if ($$)
            $$->addValue($5);
    }
    ;
key:
    maybe_unary_operator PERCENTAGE { $$.id = CSSValueInvalid; $$.isInt = false; $$.fValue = $1 * $2; $$.unit = CSSPrimitiveValue::CSS_NUMBER; }
    | IDENT {
        $$.id = CSSValueInvalid; $$.isInt = false; $$.unit = CSSPrimitiveValue::CSS_NUMBER;
        CSSParserString& str = $1;
        if (str.equalIgnoringCase("from"))
            $$.fValue = 0;
        else if (str.equalIgnoringCase("to"))
            $$.fValue = 100;
        else {
            $$.unit = 0;
            YYERROR;
        }
    }
    | error {
        $$.unit = 0;
    }
    ;
before_page_rule:
                {
        parser->markRuleHeaderStart(CSSRuleSourceData::PAGE_RULE);
    }
    ;
page:
    before_page_rule PAGE_SYM maybe_space page_selector at_rule_header_end_maybe_space
    '{' at_rule_body_start maybe_space_before_declaration declarations_and_margins closing_brace {
        if ($4)
            $$ = parser->createPageRule(adoptPtr($4)).leakRef();
        else {
            parser->clearProperties();
            $$ = nullptr;
            parser->popRuleData();
        }
    }
    | before_page_rule PAGE_SYM error invalid_block {
        parser->popRuleData();
        $$ = nullptr;
    }
    | before_page_rule PAGE_SYM error ';' {
        parser->popRuleData();
        $$ = nullptr;
    }
    ;
page_selector:
    IDENT {
        $$ = new CSSParserSelector(QualifiedName(nullAtom, $1, parser->m_defaultNamespace));
        $$->setForPage();
    }
    | IDENT pseudo_page {
        $$ = $2;
        if ($$) {
            $$->prependTagSelector(QualifiedName(nullAtom, $1, parser->m_defaultNamespace));
            $$->setForPage();
        }
    }
    | pseudo_page {
        $$ = $1;
        if ($$)
            $$->setForPage();
    }
    | {
        $$ = new CSSParserSelector;
        $$->setForPage();
    }
    ;
declarations_and_margins: declaration_list | declarations_and_margins margin_box maybe_space declaration_list ;
margin_box:
    margin_sym {
        parser->startDeclarationsForMarginBox();
    } maybe_space '{' maybe_space declaration_list closing_brace {
        parser->createMarginAtRule($1);
    }
    ;
margin_sym:
    TOPLEFTCORNER_SYM {
        $$ = CSSSelector::TopLeftCornerMarginBox;
    }
    | TOPLEFT_SYM {
        $$ = CSSSelector::TopLeftMarginBox;
    }
    | TOPCENTER_SYM {
        $$ = CSSSelector::TopCenterMarginBox;
    }
    | TOPRIGHT_SYM {
        $$ = CSSSelector::TopRightMarginBox;
    }
    | TOPRIGHTCORNER_SYM {
        $$ = CSSSelector::TopRightCornerMarginBox;
    }
    | BOTTOMLEFTCORNER_SYM {
        $$ = CSSSelector::BottomLeftCornerMarginBox;
    }
    | BOTTOMLEFT_SYM {
        $$ = CSSSelector::BottomLeftMarginBox;
    }
    | BOTTOMCENTER_SYM {
        $$ = CSSSelector::BottomCenterMarginBox;
    }
    | BOTTOMRIGHT_SYM {
        $$ = CSSSelector::BottomRightMarginBox;
    }
    | BOTTOMRIGHTCORNER_SYM {
        $$ = CSSSelector::BottomRightCornerMarginBox;
    }
    | LEFTTOP_SYM {
        $$ = CSSSelector::LeftTopMarginBox;
    }
    | LEFTMIDDLE_SYM {
        $$ = CSSSelector::LeftMiddleMarginBox;
    }
    | LEFTBOTTOM_SYM {
        $$ = CSSSelector::LeftBottomMarginBox;
    }
    | RIGHTTOP_SYM {
        $$ = CSSSelector::RightTopMarginBox;
    }
    | RIGHTMIDDLE_SYM {
        $$ = CSSSelector::RightMiddleMarginBox;
    }
    | RIGHTBOTTOM_SYM {
        $$ = CSSSelector::RightBottomMarginBox;
    }
    ;
before_font_face_rule:
                {
        parser->markRuleHeaderStart(CSSRuleSourceData::FONT_FACE_RULE);
    }
    ;
font_face:
    before_font_face_rule FONT_FACE_SYM at_rule_header_end_maybe_space '{' at_rule_body_start maybe_space_before_declaration declaration_list closing_brace {
        $$ = parser->createFontFaceRule().leakRef();
    }
    | before_font_face_rule FONT_FACE_SYM error invalid_block {
        $$ = nullptr;
        parser->popRuleData();
    }
    | before_font_face_rule FONT_FACE_SYM error ';' {
        $$ = nullptr;
        parser->popRuleData();
    }
;
before_region_rule:
                {
        parser->markRuleHeaderStart(CSSRuleSourceData::REGION_RULE);
    }
    ;
region:
    before_region_rule WEBKIT_REGION_RULE_SYM WHITESPACE selector_list at_rule_header_end '{' at_rule_body_start maybe_space block_valid_rule_list save_block {
        OwnPtr<Vector<RefPtr<StyleRuleBase>>> ruleList = adoptPtr($9);
        if ($4)
            $$ = parser->createRegionRule(adoptPtr($4).get(), ruleList.get()).leakRef();
        else {
            $$ = nullptr;
            parser->popRuleData();
        }
    }
;
combinator:
    '+' maybe_space { $$ = CSSSelector::DirectAdjacent; }
  | '~' maybe_space { $$ = CSSSelector::IndirectAdjacent; }
  | '>' maybe_space { $$ = CSSSelector::Child; }
  ;
maybe_unary_operator: unary_operator | { $$ = 1; } ;
unary_operator: '-' { $$ = -1; } | '+' { $$ = 1; } ;
maybe_space_before_declaration: maybe_space { parser->markPropertyStart(); } ;
before_selector_list:
    {
        parser->markRuleHeaderStart(CSSRuleSourceData::STYLE_RULE);
        parser->markSelectorStart();
    }
  ;
at_rule_header_end: { parser->markRuleHeaderEnd(); } ;
at_selector_end: { parser->markSelectorEnd(); } ;
ruleset:
    before_selector_list selector_list at_selector_end at_rule_header_end '{' at_rule_body_start maybe_space_before_declaration declaration_list closing_brace {
        $$ = parser->createStyleRule($2).leakRef();
        parser->recycleSelectorVector(adoptPtr($2));
    }
  ;
before_selector_group_item: { parser->markSelectorStart(); } ;
selector_list:
    selector %prec UNIMPORTANT_TOK {
        $$ = nullptr;
        if ($1) {
            $$ = parser->createSelectorVector().leakPtr();
            $$->append(adoptPtr($1));
            parser->updateLastSelectorLineAndPosition();
        }
    }
    | selector_list at_selector_end ',' maybe_space before_selector_group_item selector %prec UNIMPORTANT_TOK {
        OwnPtr<Vector<OwnPtr<CSSParserSelector>>> selectorList = adoptPtr($1);
        OwnPtr<CSSParserSelector> selector = adoptPtr($6);
        $$ = nullptr;
        if (selectorList && selector) {
            $$ = selectorList.leakPtr();
            $$->append(selector.release());
            parser->updateLastSelectorLineAndPosition();
        }
    }
    | selector_list error {
        $$ = nullptr;
        delete $1;
    }
   ;
selector:
    simple_selector
    | selector WHITESPACE
    | selector WHITESPACE simple_selector {
        OwnPtr<CSSParserSelector> left = adoptPtr($1);
        OwnPtr<CSSParserSelector> right = adoptPtr($3);
        $$ = nullptr;
        if (left && right) {
            right->appendTagHistory(CSSSelector::Descendant, left.release());
            $$ = right.leakPtr();
        }
    }
    | selector combinator simple_selector {
        OwnPtr<CSSParserSelector> left = adoptPtr($1);
        OwnPtr<CSSParserSelector> right = adoptPtr($3);
        $$ = nullptr;
        if (left && right) {
            right->appendTagHistory($2, left.release());
            $$ = right.leakPtr();
        }
    }
    | selector error {
        $$ = nullptr;
        delete $1;
    }
    ;
namespace_selector:
    '|' { $$.clear(); }
    | '*' '|' { static LChar star = '*'; $$.init(&star, 1); }
    | IDENT '|'
;
simple_selector:
    element_name {
        $$ = new CSSParserSelector(QualifiedName(nullAtom, $1, parser->m_defaultNamespace));
    }
    | element_name specifier_list {
        $$ = $2;
        if ($$)
            parser->rewriteSpecifiersWithElementName(nullAtom, $1, *$$);
    }
    | specifier_list {
        $$ = $1;
        if ($$)
            parser->rewriteSpecifiersWithNamespaceIfNeeded(*$$);
    }
    | namespace_selector element_name {
        $$ = new CSSParserSelector(parser->determineNameInNamespace($1, $2));
    }
    | namespace_selector element_name specifier_list {
        $$ = $3;
        if ($$)
            parser->rewriteSpecifiersWithElementName($1, $2, *$$);
    }
    | namespace_selector specifier_list {
        $$ = $2;
        if ($$)
            parser->rewriteSpecifiersWithElementName($1, starAtom, *$$);
    }
  ;
simple_selector_list:
    simple_selector %prec UNIMPORTANT_TOK {
        $$ = nullptr;
        if ($1) {
            $$ = parser->createSelectorVector().leakPtr();
            $$->append(adoptPtr($1));
        }
    }
    | simple_selector_list maybe_space ',' maybe_space simple_selector %prec UNIMPORTANT_TOK {
        OwnPtr<Vector<OwnPtr<CSSParserSelector>>> list = adoptPtr($1);
        OwnPtr<CSSParserSelector> selector = adoptPtr($5);
        $$ = nullptr;
        if (list && selector) {
            $$ = list.leakPtr();
            $$->append(selector.release());
        }
    }
    | simple_selector_list error {
        $$ = nullptr;
        delete $1;
    }
    ;
element_name:
    IDENT {
        if (parser->m_context.isHTMLDocument)
            $1.lower();
        $$ = $1;
    }
    | '*' {
        static LChar star = '*';
        $$.init(&star, 1);
    }
    ;
specifier_list:
    specifier
    | specifier_list specifier {
        OwnPtr<CSSParserSelector> list = adoptPtr($1);
        OwnPtr<CSSParserSelector> specifier = adoptPtr($2);
        $$ = nullptr;
        if (list && specifier)
            $$ = parser->rewriteSpecifiers(list.release(), specifier.release()).leakPtr();
    }
    | specifier_list error {
        $$ = nullptr;
        delete $1;
    }
;
specifier:
    IDSEL {
        $$ = new CSSParserSelector;
        $$->setMatch(CSSSelector::Id);
        if (parser->m_context.mode == CSSQuirksMode)
            $1.lower();
        $$->setValue($1);
    }
  | HEX {
        if ($1[0] >= '0' && $1[0] <= '9')
            $$ = nullptr;
        else {
            $$ = new CSSParserSelector;
            $$->setMatch(CSSSelector::Id);
            if (parser->m_context.mode == CSSQuirksMode)
                $1.lower();
            $$->setValue($1);
        }
    }
  | class
  | attrib
  | pseudo
    ;
class:
    '.' IDENT {
        $$ = new CSSParserSelector;
        $$->setMatch(CSSSelector::Class);
        if (parser->m_context.mode == CSSQuirksMode)
            $2.lower();
        $$->setValue($2);
    }
  ;
attrib:
    '[' maybe_space IDENT maybe_space ']' {
        $$ = new CSSParserSelector;
        $$->setAttribute(QualifiedName(nullAtom, $3, nullAtom), parser->m_context.isHTMLDocument);
        $$->setMatch(CSSSelector::Set);
    }
    | '[' maybe_space IDENT maybe_space match maybe_space ident_or_string maybe_space ']' {
        $$ = new CSSParserSelector;
        $$->setAttribute(QualifiedName(nullAtom, $3, nullAtom), parser->m_context.isHTMLDocument);
        $$->setMatch($5);
        $$->setValue($7);
    }
    | '[' maybe_space namespace_selector IDENT maybe_space ']' {
        $$ = new CSSParserSelector;
        $$->setAttribute(parser->determineNameInNamespace($3, $4), parser->m_context.isHTMLDocument);
        $$->setMatch(CSSSelector::Set);
    }
    | '[' maybe_space namespace_selector IDENT maybe_space match maybe_space ident_or_string maybe_space ']' {
        $$ = new CSSParserSelector;
        $$->setAttribute(parser->determineNameInNamespace($3, $4), parser->m_context.isHTMLDocument);
        $$->setMatch($6);
        $$->setValue($8);
    }
  ;
match:
    '=' {
        $$ = CSSSelector::Exact;
    }
    | INCLUDES {
        $$ = CSSSelector::List;
    }
    | DASHMATCH {
        $$ = CSSSelector::Hyphen;
    }
    | BEGINSWITH {
        $$ = CSSSelector::Begin;
    }
    | ENDSWITH {
        $$ = CSSSelector::End;
    }
    | CONTAINS {
        $$ = CSSSelector::Contain;
    }
    ;
ident_or_string: IDENT | STRING ;
pseudo_page:
    ':' IDENT {
        $$ = nullptr;
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PagePseudoClass);
        $2.lower();
        selector->setValue($2);
        if (selector->pseudoType() != CSSSelector::PseudoUnknown)
            $$ = selector.release();
    }
pseudo:
    ':' IDENT {
        $$ = nullptr;
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PseudoClass);
        $2.lower();
        selector->setValue($2);
        if (selector->pseudoType() != CSSSelector::PseudoUnknown)
            $$ = selector.release();
    }
    | ':' ':' IDENT {
        $$ = nullptr;
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PseudoElement);
        $3.lower();
        selector->setValue($3);
        if (selector->pseudoType() != CSSSelector::PseudoUnknown)
            $$ = selector.release();
    }
    | ':' ANYFUNCTION maybe_space simple_selector_list maybe_space ')' {
        $$ = nullptr;
        if ($4) {
            auto selector = std::make_unique<CSSParserSelector>();
            selector->setMatch(CSSSelector::PseudoClass);
            selector->adoptSelectorVector(*adoptPtr($4));
            $2.lower();
            selector->setValue($2);
            if (selector->pseudoType() == CSSSelector::PseudoAny)
                $$ = selector.release();
        }
    }
    | ':' FUNCTION maybe_space NTH maybe_space ')' {
        $$ = nullptr;
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PseudoClass);
        selector->setArgument($4);
        selector->setValue($2);
        if (selector->pseudoType() != CSSSelector::PseudoUnknown)
            $$ = selector.release();
    }
    | ':' FUNCTION maybe_space maybe_unary_operator INTEGER maybe_space ')' {
        $$ = nullptr;
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PseudoClass);
        selector->setArgument(AtomicString::number($4 * $5));
        selector->setValue($2);
        if (selector->pseudoType() != CSSSelector::PseudoUnknown)
            $$ = selector.release();
    }
    | ':' FUNCTION maybe_space IDENT maybe_space ')' {
        auto selector = std::make_unique<CSSParserSelector>();
        selector->setMatch(CSSSelector::PseudoClass);
        selector->setArgument($4);
        $2.lower();
        selector->setValue($2);
        CSSSelector::PseudoType type = selector->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            selector = nullptr;
        else if (type == CSSSelector::PseudoNthChild ||
                 type == CSSSelector::PseudoNthOfType ||
                 type == CSSSelector::PseudoNthLastChild ||
                 type == CSSSelector::PseudoNthLastOfType) {
            if (!isValidNthToken($4))
                selector = nullptr;
        }
        $$ = selector.release();
    }
    | ':' NOTFUNCTION maybe_space simple_selector maybe_space ')' {
        OwnPtr<CSSParserSelector> selector = adoptPtr($4);
        $$ = nullptr;
        if (selector && selector->isSimple()) {
            $$ = new CSSParserSelector;
            $$->setMatch(CSSSelector::PseudoClass);
            Vector<OwnPtr<CSSParserSelector>> selectorVector;
            selectorVector.append(selector.release());
            $$->adoptSelectorVector(selectorVector);
            $2.lower();
            $$->setValue($2);
        }
    }
  ;
declaration_list:
                { $$ = false; }
    | declaration
    | decl_list declaration { $$ = $1 || $2; }
    | decl_list
    | decl_list_recovery { $$ = false; }
    | decl_list decl_list_recovery
    ;
decl_list:
    declaration ';' maybe_space {
        parser->markPropertyStart();
        $$ = $1;
    }
    | decl_list_recovery ';' maybe_space {
        parser->markPropertyStart();
        $$ = false;
    }
    | decl_list declaration ';' maybe_space {
        parser->markPropertyStart();
        $$ = $1;
        if ($2)
            $$ = $2;
    }
    | decl_list decl_list_recovery ';' maybe_space {
        parser->markPropertyStart();
        $$ = $1;
    }
    ;
decl_list_recovery:
    error error_location error_recovery {
        parser->syntaxError($2, CSSParser::PropertyDeclarationError);
    }
    ;
declaration:
    VAR_DEFINITION maybe_space ':' maybe_space expr priority {
        delete $5;
        $$ = false;
    }
    | property ':' maybe_space expr priority {
        $$ = false;
        bool isPropertyParsed = false;
        OwnPtr<CSSParserValueList> propertyValue = adoptPtr($4);
        if ($1 && propertyValue) {
            parser->m_valueList = propertyValue.release();
            int oldParsedProperties = parser->m_parsedProperties.size();
            $$ = parser->parseValue($1, $5);
            if (!$$)
                parser->rollbackLastProperties(parser->m_parsedProperties.size() - oldParsedProperties);
            else
                isPropertyParsed = true;
            parser->m_valueList = nullptr;
        }
        parser->markPropertyEnd($5, isPropertyParsed);
    }
    | property declaration_recovery { $$ = false; }
    | property ':' maybe_space expr priority declaration_recovery {
        parser->markPropertyEnd(false, false);
        delete $4;
        $$ = false;
    }
    | IMPORTANT_SYM maybe_space declaration_recovery {
        $$ = false;
    }
    | property ':' maybe_space declaration_recovery {
        parser->markPropertyEnd(false, false);
        $$ = false;
    }
  ;
declaration_recovery: error error_location error_recovery { parser->syntaxError($2); } ;
property: IDENT maybe_space { $$ = cssPropertyID($1); } ;
priority: IMPORTANT_SYM maybe_space { $$ = true; } | { $$ = false; } ;
expr: valid_expr | valid_expr expr_recovery { $$ = nullptr; delete $1; } ;
valid_expr:
    term {
        $$ = new CSSParserValueList;
        $$->addValue($1);
    }
    | valid_expr operator term {
        $$ = $1;
        if (!$$)
            destroy($3);
        else {
            if ($2) {
                CSSParserValue v;
                v.id = CSSValueInvalid;
                v.unit = CSSParserValue::Operator;
                v.iValue = $2;
                $$->addValue(v);
            }
            $$->addValue($3);
        }
    }
  ;
expr_recovery: error error_location error_recovery ;
operator: '/' maybe_space { $$ = '/'; } | ',' maybe_space { $$ = ','; } | { $$ = 0; } ;
term:
  unary_term maybe_space
  | unary_operator unary_term maybe_space { $$ = $2; $$.fValue *= $1; }
  | STRING maybe_space { $$.id = CSSValueInvalid; $$.string = $1; $$.unit = CSSPrimitiveValue::CSS_STRING; }
  | IDENT maybe_space {
      $$.id = cssValueKeywordID($1);
      $$.unit = CSSPrimitiveValue::CSS_IDENT;
      $$.string = $1;
  }
  | DIMEN maybe_space { $$.id = CSSValueInvalid; $$.string = $1; $$.unit = CSSPrimitiveValue::CSS_DIMENSION; }
  | unary_operator DIMEN maybe_space { $$.id = CSSValueInvalid; $$.string = $2; $$.unit = CSSPrimitiveValue::CSS_DIMENSION; }
  | URI maybe_space { $$.id = CSSValueInvalid; $$.string = $1; $$.unit = CSSPrimitiveValue::CSS_URI; }
  | UNICODERANGE maybe_space { $$.id = CSSValueInvalid; $$.string = $1; $$.unit = CSSPrimitiveValue::CSS_UNICODE_RANGE; }
  | HEX maybe_space { $$.id = CSSValueInvalid; $$.string = $1; $$.unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; }
  | '#' maybe_space { $$.id = CSSValueInvalid; $$.string = CSSParserString(); $$.unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; }
  | VARFUNCTION maybe_space IDENT closing_parenthesis maybe_space {
      $$.id = CSSValueInvalid;
      $$.unit = 0;
  }
  | function maybe_space
  | calc_function maybe_space
  | min_or_max_function maybe_space
  | '%' maybe_space {
      $$.id = CSSValueInvalid; $$.unit = 0;
  }
  ;
unary_term:
  INTEGER { $$.id = CSSValueInvalid; $$.isInt = true; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_NUMBER; }
  | FLOATTOKEN { $$.id = CSSValueInvalid; $$.isInt = false; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_NUMBER; }
  | PERCENTAGE { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_PERCENTAGE; }
  | PXS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_PX; }
  | CMS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_CM; }
  | MMS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_MM; }
  | INS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_IN; }
  | PTS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_PT; }
  | PCS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_PC; }
  | DEGS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_DEG; }
  | RADS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_RAD; }
  | GRADS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_GRAD; }
  | TURNS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_TURN; }
  | MSECS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_MS; }
  | SECS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_S; }
  | HERTZ { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_HZ; }
  | KHERTZ { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_KHZ; }
  | EMS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_EMS; }
  | QEMS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSParserValue::Q_EMS; }
  | EXS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_EXS; }
  | REMS {
      $$.id = CSSValueInvalid;
      $$.fValue = $1;
      $$.unit = CSSPrimitiveValue::CSS_REMS;
      if (parser->m_styleSheet)
          parser->m_styleSheet->parserSetUsesRemUnits(true);
  }
  | CHS { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_CHS; }
  | VW { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_VW; }
  | VH { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_VH; }
  | VMIN { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_VMIN; }
  | VMAX { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_VMAX; }
  | DPPX { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_DPPX; }
  | DPI { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_DPI; }
  | DPCM { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_DPCM; }
  | FR { $$.id = CSSValueInvalid; $$.fValue = $1; $$.unit = CSSPrimitiveValue::CSS_FR; }
  ;
function:
    FUNCTION maybe_space expr closing_parenthesis {
        CSSParserFunction* f = new CSSParserFunction;
        f->name = $1;
        f->args = adoptPtr($3);
        $$.id = CSSValueInvalid;
        $$.unit = CSSParserValue::Function;
        $$.function = f;
    } |
    FUNCTION maybe_space closing_parenthesis {
        CSSParserFunction* f = new CSSParserFunction;
        f->name = $1;
        f->args = adoptPtr(new CSSParserValueList);
        $$.id = CSSValueInvalid;
        $$.unit = CSSParserValue::Function;
        $$.function = f;
    } |
    FUNCTION maybe_space expr_recovery closing_parenthesis {
        CSSParserFunction* f = new CSSParserFunction;
        f->name = $1;
        f->args = nullptr;
        $$.id = CSSValueInvalid;
        $$.unit = CSSParserValue::Function;
        $$.function = f;
  }
  ;
calc_func_term:
  unary_term
  | VARFUNCTION maybe_space IDENT closing_parenthesis {
      $$.id = CSSValueInvalid;
      $$.unit = 0;
  }
  | unary_operator unary_term { $$ = $2; $$.fValue *= $1; }
  ;
calc_func_operator:
    WHITESPACE '+' WHITESPACE {
        $$ = '+';
    }
    | WHITESPACE '-' WHITESPACE {
        $$ = '-';
    }
    | calc_maybe_space '*' maybe_space {
        $$ = '*';
    }
    | calc_maybe_space '/' maybe_space {
        $$ = '/';
    }
  ;
calc_maybe_space: | WHITESPACE ;
calc_func_paren_expr:
    '(' maybe_space calc_func_expr calc_maybe_space closing_parenthesis {
        $$ = nullptr;
        if ($3) {
            $$ = $3;
            CSSParserValue v;
            v.id = CSSValueInvalid;
            v.unit = CSSParserValue::Operator;
            v.iValue = '(';
            $$->insertValueAt(0, v);
            v.iValue = ')';
            $$->addValue(v);
        }
    }
  ;
calc_func_expr: valid_calc_func_expr | valid_calc_func_expr expr_recovery { $$ = nullptr; delete $1; } ;
valid_calc_func_expr:
    calc_func_term {
        $$ = new CSSParserValueList;
        $$->addValue($1);
    }
    | calc_func_expr calc_func_operator calc_func_term {
        OwnPtr<CSSParserValueList> expression = adoptPtr($1);
        $$ = nullptr;
        if (expression && $2) {
            $$ = expression.leakPtr();
            CSSParserValue v;
            v.id = CSSValueInvalid;
            v.unit = CSSParserValue::Operator;
            v.iValue = $2;
            $$->addValue(v);
            $$->addValue($3);
        } else {
            destroy($3);
        }
    }
    | calc_func_expr calc_func_operator calc_func_paren_expr {
        OwnPtr<CSSParserValueList> left = adoptPtr($1);
        OwnPtr<CSSParserValueList> right = adoptPtr($3);
        $$ = nullptr;
        if (left && $2 && right) {
            CSSParserValue v;
            v.id = CSSValueInvalid;
            v.unit = CSSParserValue::Operator;
            v.iValue = $2;
            left->addValue(v);
            left->extend(*right);
            $$ = left.leakPtr();
        }
    }
    | calc_func_paren_expr
  ;
calc_func_expr_list:
    calc_func_expr calc_maybe_space
    | calc_func_expr_list ',' maybe_space calc_func_expr calc_maybe_space {
        OwnPtr<CSSParserValueList> list = adoptPtr($1);
        OwnPtr<CSSParserValueList> expression = adoptPtr($4);
        $$ = nullptr;
        if (list && expression) {
            $$ = list.leakPtr();
            CSSParserValue v;
            v.id = CSSValueInvalid;
            v.unit = CSSParserValue::Operator;
            v.iValue = ',';
            $$->addValue(v);
            $$->extend(*expression);
        }
    }
  ;
calc_function:
    CALCFUNCTION maybe_space calc_func_expr calc_maybe_space closing_parenthesis {
        CSSParserFunction* f = new CSSParserFunction;
        f->name = $1;
        f->args = adoptPtr($3);
        $$.id = CSSValueInvalid;
        $$.unit = CSSParserValue::Function;
        $$.function = f;
    }
    | CALCFUNCTION maybe_space expr_recovery closing_parenthesis {
        $$.id = CSSValueInvalid;
        $$.unit = 0;
        YYERROR;
    }
    ;
min_or_max: MINFUNCTION | MAXFUNCTION ;
min_or_max_function:
    min_or_max maybe_space calc_func_expr_list closing_parenthesis {
        CSSParserFunction* f = new CSSParserFunction;
        f->name = $1;
        f->args = adoptPtr($3);
        $$.id = CSSValueInvalid;
        $$.unit = CSSParserValue::Function;
        $$.function = f;
    }
    | min_or_max maybe_space expr_recovery closing_parenthesis {
        $$.id = CSSValueInvalid;
        $$.unit = 0;
        YYERROR;
    }
    ;
save_block: closing_brace | error closing_brace ;
invalid_at: ATKEYWORD error invalid_block | ATKEYWORD error ';' ;
invalid_rule: error invalid_block ;
invalid_block: '{' error_recovery closing_brace { parser->invalidBlockHit(); } ;
invalid_square_brackets_block: '[' error_recovery ']' | '[' error_recovery TOKEN_EOF ;
invalid_parentheses_block: opening_parenthesis error_recovery closing_parenthesis;
opening_parenthesis:
    '(' | FUNCTION | CALCFUNCTION | VARFUNCTION | MINFUNCTION | MAXFUNCTION | ANYFUNCTION | NOTFUNCTION
    ;
error_location: { $$ = parser->currentLocation(); } ;
error_recovery:
  | error_recovery error
  | error_recovery invalid_block
  | error_recovery invalid_square_brackets_block
  | error_recovery invalid_parentheses_block
    ;
%%
