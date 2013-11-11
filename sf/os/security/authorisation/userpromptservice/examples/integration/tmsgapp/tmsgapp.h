/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


/**
 @file
 @internalComponent
 @test
*/

#ifndef MSGAPP_H_
#define MSGAPP_H_

#include <e32base.h>
#include <e32cmn.h>
#include "tmsgclient.h"

// Utility interface for classes that require a callback after a given timeout
class MAlarmObserver 
{
public:
	virtual void WakeupL() = 0;
};

// Utility class that invokes a call back after a given timeout
class CAlarm : public CTimer
	{
public:
	inline CAlarm();
	inline void ConstructL();
	void After(MAlarmObserver* aObserver, TTimeIntervalMicroSeconds32 anInterval);
private:		
	void RunL();
	MAlarmObserver* iObserver;
	};

/**
 * Sends a series of test messages to trigger user prompting.
 */
class CSendMessages : public CActive, public MAlarmObserver
	{
public:
	static CSendMessages* NewLC();
	void WakeupL();
	~CSendMessages();
	
private:		
	CSendMessages();
	void ConstructL();
	
	// From CActive
	void DoCancel();
	TInt RunError(TInt aError);
	void RunL();

private:	
	static const TInt KDefaultTimeout = 10 * 1000 * 1000;	
	TInt iTestNum;
	RMsgSession iMsgCon;
	CAlarm iAlarm;
	};


#endif /*MSGAPP_H_*/
