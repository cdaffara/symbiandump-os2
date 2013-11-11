// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// UTrace Unit Tests.
//



/**
 @file te_timer.h
 @internalTechnology
 @prototype
*/


#ifndef TE_TIMER_H
#define TE_TIMER_H

#include <e32base.h>
#include <f32file.h>

/**
This is a user-side utility class to start the timer, get the elapsed time as a 64bit storage element 
and get the timer resolution 

@internalTechnology
@prototype
 */
class CUtfTimer: public CBase
	{
public:
	CUtfTimer();
	~CUtfTimer();

	TInt StartBackgroundTimer(TRequestStatus& aStatus);
	static TUint32 CalculateTime(const TUint32 aCount);
private:	
	RTimer iTimer; //use to store timer session
	};
	
#endif //TE_TIMER_H


