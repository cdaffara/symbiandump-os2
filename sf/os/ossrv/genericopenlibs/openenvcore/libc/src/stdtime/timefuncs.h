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

 
#ifndef _TIMEFUNCS_H_
#define	_TIMEFUNCS_H_

#include <time.h>
#include <e32def.h>
#include <sys/cdefs.h>
#include <sys/errno.h>


typedef enum
{
	EUtcToLocal,
	ELocalToUtc,
}TimeConversionType;

#define MAX_TIME_ZONE_LEN 256 

__BEGIN_DECLS

TInt ConvertTime(const TInt atimeConverFlg, time_t* aUTCTime, struct tm* atmStruct);

__END_DECLS
#endif /* !_TIMEFUNCS_H_ */
