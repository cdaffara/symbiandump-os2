/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#ifndef __TXTRTPFL_H__
#define __TXTRTPFL_H__

#include <e32std.h>
#include <e32base.h>


//#define __ETEXT_PROFILE

//
// Macros for enabling profiling.

#ifdef __ETEXT_PROFILE
	#pragma message ("------------ N.B. profiling is enabled")
	#include <e32svr.h>

/**
@internalComponent
*/
	#define __ETEXT_WATCH(X) RDebug::ProfileStart(PROFILE_##X##);
	#define __ETEXT_WATCH_END(X) RDebug::ProfileEnd(PROFILE_##X##);

	#define FIRST_PROFILE_INDEX				10
	#define PROFILE_APPLY_CHAR_FORMAT		10
	#define PROFILE_REMOVE_PARA_FORMAT		11
	#define PROFILE_APPEND_RICH_TEXT		12
	#define PROFILE_GET_PARAGRAPH_FORMAT	13
	#define PROFILE_INSERT_DESC				14
	#define PROFILE_APPEND_PARAGRAPH		15
	#define PROFILE_GET_SPECIFIC_CHARS		16
	#define PROFILE_GET_CHARS				17
	#define PROFILE_PICTURE_HANDLE			18
	#define PROFILE_DETACH					19
	#define PROFILE_DETACH_POS_LENGTH		20
	#define PROFILE_RESTORE					21
	#define PROFILE_COUNT					12
#else
	#define __ETEXT_WATCH(X)
	#define __ETEXT_WATCH_END(X)
#endif

#endif
