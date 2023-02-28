/*H*************************************************************************************/
/*!
    \File t2balinrender.h

    \Description
        Basic rendering for sample, lagely copied from sony sample

    \Copyright
        Copyright (c) Electronic Arts 2019.  ALL RIGHTS RESERVED.

    \Version    1.0 10/22/2019 (tcho) First Version
*/
/*************************************************************************************H*/

/*** Functions ********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

    void T2RenderInit(int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData);
    void T2RenderUpdate();
    void T2RenderTerm();

#ifdef __cplusplus
};
#endif