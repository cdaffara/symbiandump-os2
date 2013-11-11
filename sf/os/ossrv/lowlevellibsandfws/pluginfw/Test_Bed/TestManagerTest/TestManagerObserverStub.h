// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the stub class for the Test Manager tests
// 
//


#ifndef __TESTMANAGEROBSERVERSTUB_H__
#define __TESTMANAGEROBSERVERSTUB_H__

#include <e32base.h>

#include "ManagerObserver.h"


class CTestManagerObserverStub : public CBase, public MManagerObserver
{
public:

	static CTestManagerObserverStub* NewL();

	static CTestManagerObserverStub* NewLC();

	virtual ~CTestManagerObserverStub();

	void TestsComplete();

	void SetSchedulerStarted(TBool aSchedulerStarted);

private:
	
	CTestManagerObserverStub();
	void ConstructL();


// Attributes / Properties
private:
	TBool iSchedulerStarted;


};  // End of CTestManagerObserverStub definition


#endif	// __TESTMANAGEROBSERVERSTUB_H__
