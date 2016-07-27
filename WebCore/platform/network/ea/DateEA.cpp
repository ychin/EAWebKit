/*
Copyright (C) 2009, 2010, 2011 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/////////////////////////////////////////////////////////////////////////////
// DateEA.cpp 
// Written by Paul Pedriana 
//
// Implements IETF RFC 2616 date conversions. Based on Curl date conversions.
// Curl Copyright (C) 1998 - 2008, Daniel Stenberg, <daniel@haxx.se>, et al.
/////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "DateEA.h"
#include <internal/include/EAWebKitString.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>


namespace EA
{
namespace WebKit
{

#ifdef _MSC_VER
    #define snprintf _snprintf
#endif


enum Field
{
    kFieldWeekday,
    kFieldMonth,
    kFieldMonthDay,
    kFieldYear,
    kFieldTime
};


// To consider: Get rid of kLongWeekday and just use the first three letters from kShortWeekday.
const char* kShortWeekday[7] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
const char* kLongWeekday[7]  = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
const char* kShortMonth[12]  = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };


struct TimeZoneInfo
{
    const char* mpName;     // e.g. "GMT"
    int         mnOffset;   // Minutes
};


static const struct TimeZoneInfo gTimeZoneInfo[] = 
{
  { "GMT",     0        },  // Greenwich Mean
  { "UTC",     0        },  // Universal (Coordinated)
  { "WET",     0        },  // Western European
  { "BST",     0 - 60   },  // British Summer
  { "WAT",    60        },  // West Africa
  { "AST",   240        },  // Atlantic Standard
  { "ADT",   240 - 60   },  // Atlantic Daylight
  { "EST",   300        },  // Eastern Standard
  { "EDT",   300 - 60   },  // Eastern Daylight
  { "CST",   360        },  // Central Standard
  { "CDT",   360 - 60   },  // Central Daylight
  { "MST",   420        },  // Mountain Standard
  { "MDT",   420 - 60   },  // Mountain Daylight
  { "PST",   480        },  // Pacific Standard
  { "PDT",   480 - 60   },  // Pacific Daylight
  { "YST",   540        },  // Yukon Standard
  { "YDT",   540 - 60   },  // Yukon Daylight
  { "HST",   600        },  // Hawaii Standard
  { "HDT",   600 - 60   },  // Hawaii Daylight
  { "CAT",   600        },  // Central Alaska
  { "AHST",  600        },  // Alaska-Hawaii Standard
  { "NT",    660        },  // Nome
  { "IDLW",  720        },  // International Date Line West
  { "CET",   -60        },  // Central European
  { "MET",   -60        },  // Middle European
  { "MEWT",  -60        },  // Middle European Winter
  { "MEST",  -60 - 60   },  // Middle European Summer
  { "CEST",  -60 - 60   },  // Central European Summer
  { "MESZ",  -60 - 60   },  // Middle European Summer
  { "FWT",   -60        },  // French Winter
  { "FST",   -60 - 60   },  // French Summer
  { "EET",  -120        },  // Eastern Europe, USSR Zone 1
  { "WAST", -420        },  // West Australian Standard
  { "WADT", -420 - 60   },  // West Australian Daylight
  { "CCT",  -480        },  // China Coast, USSR Zone 7
  { "JST",  -540        },  // Japan Standard, USSR Zone 8
  { "EAST", -600        },  // Eastern Australian Standard
  { "EADT", -600 - 60   },  // Eastern Australian Daylight
  { "GST",  -600        },  // Guam Standard, USSR Zone 9
  { "NZT",  -720        },  // New Zealand
  { "NZST", -720        },  // New Zealand Standard
  { "NZDT", -720 - 60   },  // New Zealand Daylight
  { "IDLE", -720        },  // International Date Line East
};


// Monday => 0, Sunday => 6, (error) => -1
static int GetWeekday(const char* pWeekdayString, size_t len)
{
    const char** pWeekday = (len <= 3) ? kShortWeekday : kLongWeekday;

    for(int i = 0; i < 7; ++i, ++pWeekday)
    {
        if(EA::Internal::Stricmp(pWeekdayString, *pWeekday) == 0)
            return i;
    }

    return -1;
}


// January => 0, December => 11, (error) => -1
static int GetMonth(const char* pMonthString)
{
    const char** pMonth = kShortMonth;

    for(int i = 0; i < 12; ++i, ++pMonth)
    {
        if(EA::Internal::Stricmp(pMonthString, *pMonth) == 0)
            return i;
    }

    return -1;
}


// Return the time zone offset in seconds between GMT and pTimeZoneString.
// Returns -1 if pTimeZoneString is not bFound.
static int GetTimeZoneOffsetSeconds(const char* pTimeZoneString)
{
    const TimeZoneInfo* pTimeZoneInfo = gTimeZoneInfo;

    for(size_t i = 0; i < sizeof(gTimeZoneInfo)/sizeof(gTimeZoneInfo[0]); ++i, ++pTimeZoneInfo)
    {
        if(EA::Internal::Stricmp(pTimeZoneString, pTimeZoneInfo->mpName) == 0)
            return (pTimeZoneInfo->mnOffset * 60);
    }

    return -1;
}


static void SkipSeparatorText(const char*& pString)
{
    while(*pString && !isalnum(*pString))
        ++pString;
}


time_t ReadDateString(const char* pDateString)
{
    time_t      t         = 0;
    int         nWeekDay  = -1;    // Day of the week number, 0-6 (mon-sun)
    int         nMonth    = -1;    // Month of the year number, 0-11
    int         nMonthDay = -1;    // Day of month, 1 - 31
    int         nHour     = -1;
    int         nMinute   = -1;
    int         nSecond   = -1;
    int         nYear     = -1;
    int         tzOffset  = -1;
    Field       nextField = kFieldMonthDay;
    int         nPart     = 0;
    const char* pDateStringSaved = pDateString;
    tm          tmValue;

    while(*pDateString && (nPart < 6))
    {
        bool bFound = false;

        SkipSeparatorText(pDateString);

        if(isalpha(*pDateString))
        {
            char buffer[32] = { 0 };

            sscanf(pDateString, "%31[A-Za-z]", buffer);

            size_t len = strlen(buffer);

            if(nWeekDay == -1)
            {
                nWeekDay = GetWeekday(buffer, len);

                if(nWeekDay != -1)
                    bFound = true;
            }

            if(!bFound && (nMonth == -1))
            {
                nMonth = GetMonth(buffer);

                if(nMonth != -1)
                    bFound = true;
            }

            if(!bFound && (tzOffset == -1))
            {
                // this just must be a time zone string
                tzOffset = GetTimeZoneOffsetSeconds(buffer);
                if(tzOffset != -1)
                    bFound = true;
            }

            if(!bFound)
                return -1;

            pDateString += len;
        }
        else if(isdigit(*pDateString))
        {
            int   val;
            char* end;

            if((nSecond == -1) && (sscanf(pDateString, "%02d:%02d:%02d", &nHour, &nMinute, &nSecond) == 3))
            {
                pDateString += 8;
                bFound = true;
            }
            else
            {
                val = (int)strtol(pDateString, &end, 10);

                if((tzOffset == -1) && ((end - pDateString) == 4) && (val <= 1400) && 
                    (pDateStringSaved < pDateString) && ((pDateString[-1] == '+' || pDateString[-1] == '-')))
                {
                    // Four digits and a value less than or equal to 1400 (to take into
                    // account all sorts of funny time zone diffs) and it is preceeded
                    // with a plus or minus. This is a time zone indication. 1400 is
                    // picked since +1300 is frequently used and +1400 is mentioned as
                    // an edge number in the document "ISO C 200X Proposal: Timezone
                    // Functions" at http://david.tribble.com/text/c0xtimezone.html.
                    bFound = true;
                    tzOffset = (val / 100 * 60 + val % 100) * 60;

                    // The + and - prefix indicates the local time compared to GMT,
                    // this we need ther reversed math to get what we want.
                    tzOffset = pDateString[-1]=='+'?-tzOffset:tzOffset;
                }

                if(((end - pDateString) == 8) && (nYear == -1) && (nMonth == -1) && (nMonthDay == -1))
                {
                    // 8 digits, no year, month or day yet. This is YYYYMMDD
                    bFound  = true;
                    nYear = val/10000;
                    nMonth  = (val%10000)/100-1; /* month is 0 - 11 */
                    nMonthDay = val%100;
                }

                if(!bFound && (nextField == kFieldMonthDay) && (nMonthDay == -1))
                {
                    if((val > 0) && (val < 32))
                    {
                        nMonthDay = val;
                        bFound = true;
                    }

                    nextField = kFieldYear;
                }

                if(!bFound && (nextField == kFieldYear) && (nYear == -1))
                {
                    nYear = val;
                    bFound = true;

                    if(nYear < 1900)
                    {
                        if(nYear > 70)
                            nYear += 1900;
                        else
                            nYear += 2000;
                    }

                    if(nMonthDay == -1)
                        nextField = kFieldMonthDay;
                }

                if(!bFound)
                    return -1;

                pDateString = end;
            }
        }

        nPart++;
    }

    if(-1 == nSecond)
        nSecond = nMinute = nHour = 0; /* no time, make it zero */

    if((-1 == nMonthDay) || (-1 == nMonth) ||(-1 == nYear))
        return -1;

    if((sizeof(time_t) < 5) && (nYear > 2037))
    {
        // 32 bit time_t can only hold dates to the beginning of 2038.
        return 0x7fffffff;
    }

    tmValue.tm_sec   = nSecond;
    tmValue.tm_min   = nMinute;
    tmValue.tm_hour  = nHour;
    tmValue.tm_mday  = nMonthDay;
    tmValue.tm_mon   = nMonth;
    tmValue.tm_year  = nYear - 1900;
    tmValue.tm_wday  = 0;
    tmValue.tm_yday  = 0;
    tmValue.tm_isdst = 0;

    // mktime() returns a time_t. time_t is often 32 bits, even on many
    // architectures that feature 64 bit 'long'.
    //
    // Some systems have 64 bit time_t and deal with years beyond 2038. However,
    // even some of the systems with 64 bit time_t returns -1 for dates beyond
    // 03:14:07 UTC, January 19, 2038. (Such as AIX 5100-06)

    t = mktime(&tmValue);

    // time zone adjust (cast t to int to compare to negative one)
    if(-1 != (int)t)
    {
        long   delta;
        time_t t2;

        #ifdef HAVE_GMTIME_R
            tm  result;
            tm* gmt = gmtime_r(&t, &result);

            if(!gmt)
                return -1;

            t2 = mktime(gmt);
        #else
            tm  gmt2;
            tm* gmt = gmtime(&t);

            if(!gmt)
                return -1;

            gmt2 = *gmt;
            t2   = mktime(&gmt2);
        #endif

        // Add the time zone diff (between the given timezone and GMT) and 
        // the diff between the local time zone and GMT. */
        delta = (long)((tzOffset != -1 ? tzOffset : 0) + (t - t2));

        if((delta > 0) && ((t + delta) < t))
            return -1; // time_t overflow

        t += delta;
    }

    return t;
}


int WriteDateString(time_t time, char* pDateString, size_t dateStringCapacity)
{
    #ifdef HAVE_GMTIME_R  // Unix variants want to use gmtime_r
        tm  result;
        tm* pTime = gmtime_r(&time, &result);
    #else
        tm* pTime = gmtime(&time);
    #endif

    int result = snprintf(pDateString, dateStringCapacity - 1, "%s, %02d %s %4d %02d:%02d:%02d GMT",
                             kShortWeekday[pTime->tm_wday ? pTime->tm_wday - 1 : 6],
                             pTime->tm_mday,
                             kShortMonth[pTime->tm_mon],
                             pTime->tm_year + 1900,
                             pTime->tm_hour,
                             pTime->tm_min,
                             pTime->tm_sec);

    if(result >= (int)dateStringCapacity)
        return -1;
    return result;
}




} // namespace WebKit
} // namespace EA


