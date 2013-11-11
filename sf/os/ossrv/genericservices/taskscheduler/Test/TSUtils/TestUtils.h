// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TESTUTILS_H__
#define __TESTUTILS_H__

#include <e32base.h>
#include <e32test.h>

extern RTest TheTest;
#define TEST(arg) TheTest(arg, __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)


//Does "aValue == aExpected" and if it is not true - prints out an error message
//with source line number and cancels the test
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#endif//__TESTUTILS_H__
