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



#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <_ansi.h>
/**
@publishedAll
@released
*/
struct timeval {
  long tv_sec;
  long tv_usec;
};
/**
@publishedAll
@released
*/
struct timezone {
  int tz_minuteswest;
  int tz_dsttime;
};

IMPORT_C int gettimeofday (struct timeval *tp, struct timezone *tzp);

#ifdef __cplusplus
}
#endif
#endif
