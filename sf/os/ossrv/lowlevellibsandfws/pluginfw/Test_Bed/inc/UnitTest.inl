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
//


inline CUnitTest::CUnitTest(const TDesC&		 aName, 
							CDataLogger&		 aDataLogger, 
							MUnitTestObserver&	 aUnitTestObserver) :
CTimer(CActive::EPriorityStandard),
iUnitTestName(aName),
iDataLogger(aDataLogger),
iUnitTestObserver(aUnitTestObserver)
	{
	CActiveScheduler::Add(this);
	}


inline const TDesC& CUnitTest::UnitTestName() const
	{
	return iUnitTestName;
	}

inline void CUnitTest::PrepareUnitTestL()
	{
	// Do nothing in the default implementation
	}

inline void CUnitTest::SetRTest(RTest* aRTest)
	{
	// Record a handle on the RTest object to use in decoding the result 
	// of a unit test run in RunL().
	iRTest = aRTest;
	}



