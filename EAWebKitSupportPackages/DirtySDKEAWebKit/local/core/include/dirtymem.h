/*
Copyright (C) 2005,2009, 2012 Electronic Arts, Inc.  All rights reserved.

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
    \File dirtymem.h

*/
/********************************************************************************H*/

#ifndef _dirtymem_h
#define _dirtymem_h

/*** Include files ****************************************************************/

#include "dirtyplatform.h"

/*** Defines **********************************************************************/

#define BUDDYAPI_MEMID          ('budd')
#define CLUBAPI_MEMID           ('club')
#define HLBUDAPI_MEMID          ('hbud')
#define XBFRIENDS_MEMID         ('xbfr')
#define XNFRIENDS_MEMID         ('xnfr')

#define PCISP_MEMID             ('pcip')

#define COMMSER_MEMID           ('cser')
#define COMMSRP_MEMID           ('csrp')
#define COMMTCP_MEMID           ('ctcp')
#define COMMUDP_MEMID           ('cudp')

#define DIRTYLSP_MEMID          ('dlsp')
#define DIRTYSESSMGR_MEMID      ('dsmg')
#define SOCKET_MEMID            ('dsoc')
#define RPC_MEMID               ('drpc')
#define NETCONN_MEMID           ('ncon')

#define CONNAPI_MEMID           ('conn')
#define NETGAMEDIST_MEMID       ('ngdt')
#define NETGAMEDISTSERV_MEMID   ('ngds')
#define NETGAMELINK_MEMID       ('nglk')
#define NETGAMEUTIL_MEMID       ('ngut')

#define DIRTYGRAPH_MEMID        ('dgph')
#define DIRTYJPG_MEMID          ('djpg')
#define DIRTYPNG_MEMID          ('dpng')

#define FAVORITES_MEMID         ('favr')
#define GAMECONFIGAPI_MEMID     ('gcfg')
#define GAMEMANAGER_MEMID       ('gmgr')
#define USERSETMANAGER_MEMID    ('umgr')
#define LEAGAPI_MEMID           ('leag')
#define NEW_TOURNEY_MEMID       ('ntou')
#define ONLINE_LEAGAPI_MEMID    ('lolg')
#define LOBBYAPI_MEMID          ('lapi')
#define LOBBYAPIASYNC_MEMID     ('lasc')
#define LOBBYAPILIST_MEMID      ('llst')
#define LOBBYCHAL_MEMID         ('lchl')
#define LOBBYFINDUSER_MEMID     ('lfus')
#define LOBBYGLUE_MEMID         ('lglu')
#define BUDDYTICKERGLUE_MEMID   ('btgl')
#define LOBBYLOCALIZE_MEMID     ('lloc')
#define LOBBYLOGGER_MEMID       ('llgr')
#define LOBBYLOGIN_MEMID        ('llgn')
#define MSGLOG_MEMID            ('lmlg')
#define LOBBYRANK_MEMID         ('lrnk')
#define LOBBYREG_MEMID          ('lreg')
#define LOBBYSETTING_MEMID      ('lset')
#define LOBBYSTATBOOK_MEMID     ('lsta')
#define BLAZESTATS_MEMID        ('bzst')
#define LOBBYROOMLIST_MEMID     ('lrml')
#define TOURNEYAPI_MEMID        ('tour')
#define LOBBYAPIPLAT_MEMID      ('lapp')

#define LOBBYLAN_MEMID          ('llan')
#define PORTTESTER_MEMID        ('ptst')
#define TELEMETRYAPI_MEMID      ('telm')
#define TICKERAPI_MEMID         ('tick')

#define PROTOADVT_MEMID         ('padv')
#define PROTOARIES_MEMID        ('pari')
#define PROTOHTTP_MEMID         ('phtp')
#define HTTPMGR_MEMID           ('hmgr')
#define PROTOMANGLE_MEMID       ('pmgl')
#define PROTONAME_MEMID         ('pnam')
#define PROTOPING_MEMID         ('ppng')
#define PINGMGR_MEMID           ('lpmg')
#define PROTOSSL_MEMID          ('pssl')
#define PROTOSTREAM_MEMID       ('pstr')
#define PROTOTUNNEL_MEMID       ('ptun')
#define PROTOUDP_MEMID          ('pudp')
#define PROTOUPNP_MEMID         ('pupp')
#define PROTOFILTER_MEMID       ('pflt')

#define DISPLIST_MEMID          ('ldsp')
#define HASHER_MEMID            ('lhsh')
#define LOBBYSORT_MEMID         ('lsor')

#define QOSAPI_MEMID            ('dqos')

#define VOIP_MEMID              ('voip')

#define VOIPTUNNEL_MEMID        ('vtun')

#define DIGOBJAPI_MEMID         ('dobj')
#define LOCKERAPI_MEMID         ('lckr')
#define NETRSRC_MEMID           ('nrsc')
#define WEBLOG_MEMID            ('wlog')
#define WEBOFFER_MEMID          ('webo')

#define XMLLIST_MEMID           ('xmll')

#define NINSOC_MEMID            ('nsoc')
#define NINDWC_MEMID            ('ndwc')


/*** Macros ***********************************************************************/

#if !DIRTYCODE_DEBUG
 #define DirtyMemDebugAlloc(_pMem, _iSize, _iMemModule, _iMemGroup, _pMemGroupUserData) {;}
 #define DirtyMemDebugFree(_pMem, _iSize, _iMemModule, _iMemGroup, _pMemGroupUserData) {;}
#endif

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

/*** Functions ********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void DirtyMemGroupEnter(int32_t iGroup, void *pMemGroupUserData);

void DirtyMemGroupLeave(void);

void DirtyMemGroupQuery(int32_t *pMemGroup, void **ppMemGroupUserData);

#if DIRTYCODE_DEBUG
void DirtyMemDebugAlloc(void *pMem, int32_t iSize, int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData);

void DirtyMemDebugFree(void *pMem, int32_t iSize, int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData);
#endif

void *DirtyMemAlloc(int32_t iSize, int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData);

void DirtyMemFree(void *pMem, int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData);

#ifdef __cplusplus
}
#endif

#endif // _dirtymem_h

