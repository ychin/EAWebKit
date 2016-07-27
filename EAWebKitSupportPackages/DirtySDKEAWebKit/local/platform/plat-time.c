/*
Copyright (C) 2005,2009-2010, 2012 Electronic Arts, Inc.  All rights reserved.

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

/*H********************************************************************************/
/*!
    \File plat-time.c

*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include <string.h>

#include "dirtyplatform.h"

/*** Defines **********************************************************************/

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

/*** Public functions *************************************************************/
uint32_t ds_timeinsecs(void)
{
    return((uint32_t)time(NULL));
}

struct tm *ds_secstotime(struct tm *tm, uint32_t elap)
{
    int32_t year;
    int32_t leap;
    int32_t next;
    int32_t days;
    int32_t secs;
    const int32_t *mon;

    static const int32_t dayspermonth[24] = {
        31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, // leap
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31  // norm
    };

    days = elap / (24*60*60);
    secs = elap % (24*60*60);

    tm->tm_sec = secs % 60;
    secs /= 60;
    tm->tm_min = secs % 60;
    secs /= 60;
    tm->tm_hour = secs;

    for (year = 1970;; year = next) {
        leap = (((year & 3) == 0) && (((year % 100 != 0) || ((year % 400) == 0))) ? 366 : 365);
        if (days < leap)
            break;

        next = year + (days / 366);

        if (next == year)
            ++next;

        days -= (next - year) * 365;
        days += ((year-1)/4 - (year-1)/100 + (year-1)/400);
        days -= ((next-1)/4 - (next-1)/100 + (next-1)/400);
    }

    tm->tm_year = year - 1900;
    tm->tm_yday = days;
    mon = dayspermonth + 12*(leap&1);
    for (tm->tm_mon = 0; days >= *mon; tm->tm_mon += 1)
        days -= *mon++;
    tm->tm_mday = days + 1;
    tm->tm_isdst = 0;

    return(tm);
}

uint32_t ds_timetosecs(const struct tm *tm)
{
    int32_t res;
    struct tm cmp;
    uint32_t min;
    uint32_t max;
    uint32_t mid;

    min = 0;
    max = -1;

    res = 0;
    mid = 0;

    while (min <= max) {
        mid = (min/2)+(max/2)+(min&max&1);
        ds_secstotime(&cmp, mid);
        if ((res = (cmp.tm_year - tm->tm_year)) == 0) {
            if ((res = (cmp.tm_mon - tm->tm_mon)) == 0) {
                if ((res = (cmp.tm_mday - tm->tm_mday)) == 0) {
                    if ((res = (cmp.tm_hour - tm->tm_hour)) == 0) {
                        if ((res = (cmp.tm_min - tm->tm_min)) == 0) {
                            if ((res = cmp.tm_sec - tm->tm_sec) == 0) {
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (min == max)
            break;

        if (res > 0)
            max = mid-1;
        else
            min = mid+1;
    }

    return(res == 0 ? mid : 0);
}

uint32_t ds_strtotime(const char *pStr)
{
    int32_t i;
    const char *s;
    struct tm tm;
    static const char *wday[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", 0 };
    static const char *month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", 0 };

    memset(&tm, -1, sizeof(tm));

    while ((*pStr != 0) && (*pStr <= ' '))
    {
        pStr++;
    }

    for (i = 0; (s=wday[i]) != 0; ++i)
    {
        if ((pStr[0] == s[0]) && (pStr[1] == s[1]) && (pStr[2] == s[2]))
        {
            tm.tm_wday = i;
            while ((*pStr != ',') && (*pStr != ' ') && (*pStr != 0))
                ++pStr;
            while ((*pStr == ',') || (*pStr == ' '))
                ++pStr;
            break;
        }
    }

    if ((*pStr < '0') || (*pStr > '9'))
    {
        for (i = 0; (s=month[i]) != 0; ++i)
        {
            if ((pStr[0] != s[0]) || (pStr[1] != s[1]) || (pStr[2] != s[2]))
                continue;
            tm.tm_mon = i;
            while ((*pStr != 0) && ((*pStr < '0') || (*pStr > '9')))
                ++pStr;
            for (i = 0; ((*pStr >= '0') && (*pStr <= '9')); ++pStr)
                i = (i * 10) + (*pStr & 15);
            if (i > 0)
                tm.tm_mday = i;
            break;
        }
    }

    if ((tm.tm_mon < 0) && (pStr[0] >= '0') && (pStr[0] <= '9') &&
        ((pStr[1] > '@') || (pStr[2] > '@') || (pStr[3] > '@')))
    {
        for (i = 0; ((*pStr >= '0') && (*pStr <= '9')); ++pStr)
            i = (i * 10) + (*pStr & 15);
        tm.tm_mday = i;
        while (*pStr < '@')
            ++pStr;
        for (i = 0; (s=month[i]) != 0; ++i)
        {
            if ((pStr[0] != s[0]) || (pStr[1] != s[1]) || (pStr[2] != s[2]))
                continue;
            tm.tm_mon = i;
            while ((*pStr != 0) && (*pStr != ' '))
                ++pStr;
            break;
        }
    }

    if ((*pStr >= '0') && (*pStr <= '9') && (tm.tm_mon < 0))
    {
        for (i = 0; ((*pStr >= '0') && (*pStr <= '9')); ++pStr)
            i = (i * 10) + (*pStr & 15);
        tm.tm_mon = i - 1;
        if (*pStr != 0)
            ++pStr;
        for (i = 0; ((*pStr >= '0') && (*pStr <= '9')); ++pStr)
            i = (i * 10) + (*pStr & 15);
        tm.tm_mday = i;
        if (*pStr != 0)
            ++pStr;
    }

    while ((*pStr != 0) && ((*pStr < '0') || (*pStr > '9')))
        ++pStr;
    if ((pStr[0] >= '0') && (pStr[0] <= '9') && (pStr[1] != ':') && (pStr[2] != ':'))
    {
        for (i = 0; ((*pStr >= '0') && (*pStr <= '9')); ++pStr)
            i = (i * 10) + (*pStr & 15);
        if (i > 999)
            tm.tm_year = i;
        else if (i >= 50)
            tm.tm_year = 1900+i;
        else
            tm.tm_year = 2000+i;
        while ((*pStr != 0) && ((*pStr < '0') || (*pStr > '9')))
            ++pStr;
    }

    if ((*pStr >= '0') && (*pStr <= '9'))
    {
        i = (*pStr++ & 15);
        if ((*pStr >= '0') && (*pStr <= '9'))
            i = (i * 10) + (*pStr++ & 15);
        tm.tm_hour = i;
        if (*pStr == ':')
            ++pStr;
    }

    if ((*pStr >= '0') && (*pStr <= '9'))
    {
        i = (*pStr++ & 15);
        if ((*pStr >= '0') && (*pStr <= '9'))
            i = (i * 10) + (*pStr++ & 15);
        tm.tm_min = i;
        if (*pStr == ':')
            ++pStr;
    }

    if ((*pStr >= '0') && (*pStr <= '9'))
    {
        i = (*pStr++ & 15);
        if ((*pStr >= '0') && (*pStr <= '9'))
            i = (i * 10) + (*pStr++ & 15);
        tm.tm_sec = i;
    }

    if (tm.tm_year < 0)
    {
        while ((*pStr != 0) && ((*pStr < '0') || (*pStr > '9')))
            ++pStr;
        for (i = 0; ((*pStr >= '0') && (*pStr <= '9')); ++pStr)
            i = (i * 10) + (*pStr & 15);
        if (i > 999)
            tm.tm_year = i;
    }

    if (tm.tm_year > 1900)
        tm.tm_year -= 1900;

    return(ds_timetosecs(&tm));
}


