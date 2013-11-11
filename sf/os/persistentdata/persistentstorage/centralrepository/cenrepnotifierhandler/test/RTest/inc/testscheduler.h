// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef TESTSCHEDULER_H
#define TESTSCHEDULER_H

// INCLUDES
#include <e32base.h>
#include <e32std.h>

// CLASS DECLARATION

/**
*  ActiveObject that controls the thread
*
*/
NONSHARABLE_CLASS(CControlObject) : public CActive
{
	enum TControlState
		{
		EStateFinished = 0,
		EStateStarting,
		EStateStarted,
		EStateTimed,
		};

public:	
	// returns true if timeout has expired
	TBool Timed() const
	{
		return iState == EStateTimed;
	}
    
    CControlObject();

	~CControlObject();

	void Start(TInt aTimeoutInMilliSecs); 
		
	virtual void DoCancel();

	virtual void RunL();
 
private:

	TInt			iTimeout;	// Timeout in milliseconds
	RTimer			iTimer;		// Timer for timeout
	TControlState	iState;		// Current state
};

/**
*  Controls the handling of asynchronous requests as represented by active objects.
*
*/
NONSHARABLE_CLASS(CTestScheduler) : public CActiveScheduler
{	

public:	
    
    CTestScheduler();

	virtual ~CTestScheduler();

	virtual void WaitForAnyRequest(); // jlof 13-12-2001
	
	virtual void Error(TInt anError) const;

	static TBool StartAndWaitL( TInt aMillisecs );
	
	static CTestScheduler* NewL();

        
private:

    // By default Symbian 2nd phase constructor is private.
    void ConstructL();

	
private:    // Data

	CControlObject *iControl;
};


#endif // TESTSCHEDULER_H
