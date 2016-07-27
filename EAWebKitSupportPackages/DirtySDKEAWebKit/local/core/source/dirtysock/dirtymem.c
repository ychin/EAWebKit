/*
Copyright (C) 2005,2009-2010 Electronic Arts, Inc.  All rights reserved.

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
    \File dirtymem.c

*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include "dirtysock.h"
#include "dirtymem.h"

/*** Defines **********************************************************************/

#define DIRTYMEM_MAXGROUPS  (16)

/*** Type Definitions *************************************************************/

typedef struct DirtyMemInfoT
{
    int32_t iMemGroup;
    void *pMemGroupUserData;
} DirtyMemInfoT;


/*** Variables ********************************************************************/

static int32_t _DirtyMem_iGroup = 0;
static DirtyMemInfoT _DirtyMem_iGroupStack[DIRTYMEM_MAXGROUPS] = { {'dflt', NULL} };

/*** Private Functions ************************************************************/

/*** Public functions *************************************************************/


void DirtyMemGroupEnter(int32_t iMemGroup, void *pMemGroupUserData)
{
    if (_DirtyMem_iGroup >= DIRTYMEM_MAXGROUPS)
    {
        NetPrintf(("dirtymem: group stack overflow\n"));
        return;
    }
    _DirtyMem_iGroup += 1;   
    _DirtyMem_iGroupStack[_DirtyMem_iGroup].iMemGroup = iMemGroup;
    _DirtyMem_iGroupStack[_DirtyMem_iGroup].pMemGroupUserData = pMemGroupUserData;
}

void DirtyMemGroupLeave(void)
{
    if (_DirtyMem_iGroup <= 0)
    {
        NetPrintf(("dirtymem: group stack underflow\n"));
        return;
    }
    _DirtyMem_iGroup -= 1;
}

void DirtyMemGroupQuery(int32_t *pMemGroup, void **ppMemGroupUserData)
{
    if (pMemGroup != NULL)
    {
        *pMemGroup = _DirtyMem_iGroupStack[_DirtyMem_iGroup].iMemGroup;
    }
    if (ppMemGroupUserData != NULL)
    {
        *ppMemGroupUserData = _DirtyMem_iGroupStack[_DirtyMem_iGroup].pMemGroupUserData;
    }
}

#if DIRTYCODE_DEBUG
void DirtyMemDebugAlloc(void *pMem, int32_t iSize, int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData)
{
    NetPrintf(("dirtymem: [a] 0x%08x mod=%c%c%c%c grp=%c%c%c%c udataptr=0x%08x size=%d\n", (uintptr_t)pMem,
        (iMemModule>>24)&0xff, (iMemModule>>16)&0xff, (iMemModule>>8)&0xff, iMemModule&0xff,
        (iMemGroup>>24)&0xff, (iMemGroup>>16)&0xff, (iMemGroup>>8)&0xff, iMemGroup&0xff,
        pMemGroupUserData, iSize));
}
#endif

#if DIRTYCODE_DEBUG
void DirtyMemDebugFree(void *pMem, int32_t iSize, int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData)
{
    NetPrintf(("dirtymem: [f] 0x%08x mod=%c%c%c%c grp=%c%c%c%c udataptr=0x%08x", (uintptr_t)pMem,
        (iMemModule>>24)&0xff, (iMemModule>>16)&0xff, (iMemModule>>8)&0xff, iMemModule&0xff,
        (iMemGroup>>24)&0xff, (iMemGroup>>16)&0xff, (iMemGroup>>8)&0xff, iMemGroup&0xff,
        pMemGroupUserData));
    if (iSize > 0)
    {
        NetPrintf((" size=%d\n", iSize));
    }
    else
    {
        NetPrintf((" size=0x%08x\n", iSize));
    }
}
#endif
