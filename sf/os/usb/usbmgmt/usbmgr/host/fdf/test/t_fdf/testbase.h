/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TESTBASE_H__
#define __TESTBASE_H__

#include <e32base.h>
#include <e32keys.h>

class MTestManager;

#define LeaveIfErrorL(aError) \
	{ \
	if ( aError ) \
		{ \
		iManager.Write(_L8("LEAVE: line %d, code %d"), __LINE__, aError); \
		User::Leave(aError); \
		} \
	} 

class CTestBase : public CBase
/**
 * Abstract base class for tests.
 */
	{
public:
	CTestBase(MTestManager& aManager);
	~CTestBase();

public:
	// Tests may implement either of these to pick up user selections. 
	// Single-key entries require you to implement the TKeyCode overload; 
	// multi-key selections require you to implement the descriptor overload.
	virtual void ProcessKeyL(TKeyCode aKeyCode);
	virtual void ProcessKeyL(const TDesC8& aString);

	virtual void DisplayTestSpecificMenu() = 0;

protected: // unowned
	MTestManager& iManager;
	};

#endif // __TESTBASE_H__
