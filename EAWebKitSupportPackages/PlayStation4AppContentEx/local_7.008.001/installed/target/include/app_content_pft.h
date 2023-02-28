/* SIE CONFIDENTIAL
 PlayStation(R)4 Programmer Tool Runtime Library Release 07.008.001
 *
 *      Copyright (C) 2019 Sony Interactive Entertainment Inc.
 *
 */
#ifndef _SCE_APP_CONTENT_PFT_H
#define _SCE_APP_CONTENT_PFT_H

#include <np/np_common.h>
#include <sdk_version.h>
#include <string.h> 

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif	/* defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus) */

/*
 * Data Types
 */

/*J Play First Trial flag型 */
/*E type of Play First Trial flag */
typedef uint32_t SceAppContentPftFlag;

/*
 * constants
 */

 /*J Play First Trial flag値 */
/*E value for Play First Trial flag */
#define SCE_APP_CONTENT_PFT_FLAG_OFF	(0)	/*J Play First Trialアプリケーションではない */
											/*E No Play First Trial application */
#define SCE_APP_CONTENT_PFT_FLAG_ON		(1)	/*J Play First Trialアプリケーション */
											/*E Play First Trial application */
/*
 * Functions
 */

/*J Play First Trial flagの取得 */
/*E get Play First Trial flag */
int32_t sceAppContentGetPftFlag(

	SceAppContentPftFlag *pftFlag	/*J Play First Trial flag */
									/*E Play First Trial flag */
);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif	/* defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus) */

#endif /* _SCE_APP_CONTENT_PFT_H */
