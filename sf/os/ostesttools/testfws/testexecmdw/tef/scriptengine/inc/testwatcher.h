/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file TestWatcher.h
*/

#if !(defined __TEST_WATCHER_H__)
#define __TEST_WATCHER_H__

#include <e32base.h>
#include <e32std.h>

/////////
// Remote Panic Detection
////////
class TThreadPanicDetails
	{
public:
	TThreadPanicDetails(TName aThreadName, TInt aReason, TExitCategoryName aCategory, TTime aTime);
	TName iThreadName;
	TInt iReason;
	TExitCategoryName iCategory;
	TTime iTime;
	};

/** Provides access to shared data between the threads. Because of the nature of construction	
 *	it is too complicated to generate mutexes, and as such you MUST NOT
 *  use this class inside TestWatcher whilst the UndertakerWatcher thread is running.
 *  Kill it before you access this data, and construct the object before that thread
 *  is running. YHBW.
 */	
class CSharedData : public CBase
	{
public:
	RPointerArray<TThreadPanicDetails> iPanicDetails;
	};

class CTestWatcher : public CBase
	{
public:
	CTestWatcher();
	~CTestWatcher();
	void ConstructL();
	void StartL();
	void Stop();
	static CTestWatcher* NewL();
	CSharedData* iSharedData;
private:
	RThread iWorker;
	};

#endif
