/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TESTMANAGER_H__
#define __TESTMANAGER_H__

#include <e32std.h>
#include <e32keys.h>

class MTestManager
	{
public:
	/**
	Called when a test finishes.
	*/
	virtual void TestFinished() = 0;

	/**
	Called to display some text.
	*/
	virtual void Write(TRefByValue<const TDesC8> aFmt, ...) = 0;
	virtual void WriteNoReturn(TRefByValue<const TDesC8> aFmt, ...) = 0;

	/**
	Read a user-inputted number.
	aNumber should be initialised before calling.
	*/
	virtual void GetNumberL(TUint& aNumber) = 0;
	};

#endif // __TESTMANAGER_H__
