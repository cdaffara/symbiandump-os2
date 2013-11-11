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

#include <e32base.h>
#include <bautils.h>
#include <f32file.h>
#include "../EcomTestUtils/EcomTestCompTestabilityUtils.h"
#include "../EcomTestUtils/EcomTestIniFileUtils.h"
#include "../EcomTestUtils/EcomTestUtils.h"

_LIT(KIniEcomTestBehaviourFileNameInit, "z:\\test\\data\\EComTestBehaviour.ini");
_LIT(KIniEcomTestBehaviourFileNameFinal, "c:\\EComTestBehaviour.ini");

//
//
//Test macroes and functions
//
//

static  void Check(RTest& aTest, TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		aTest(EFalse, aLine);
		}
	}
#define TEST2(aTest, aValue, aExpected) ::Check(aTest, aValue, aExpected, __LINE__)

//
//
//Helper functions
//
//
/**
Enable ECom test behaviour by copying the EComTestBehaviour.ini
@param aTest the RTest that this method is called from
@param	aFs A reference to an connected file server session.
*/
void EnableEcomTestBehaviour(RTest& aTest, RFs& /*aFs*/)
	{
	TInt err = KErrNone;
	TParse initFileName;
	TParse finalFileName;

	initFileName.Set(KIniEcomTestBehaviourFileNameInit, NULL, NULL);
	finalFileName.Set(KIniEcomTestBehaviourFileNameFinal, NULL, NULL);
	
	//copy the file to enable test behaviour
	TRAP(err, EComTestUtils::FileManCopyFileL(
					initFileName.FullName(),
	                finalFileName.FullName()));
	TEST2(aTest, err, KErrNone);
	}

/**
Enable ECom test behaviour by deleting the EComTestBehaviour.ini
@param aTest the RTest that this method is called from
@param	aFs A reference to an connected file server session.
*/
void DisableEcomTestBehaviour(RTest& aTest, RFs& /*aFs*/)
	{
	TInt err = KErrNone;
	TParse finalFileName;

	finalFileName.Set(KIniEcomTestBehaviourFileNameFinal, NULL, NULL);
	
	//delete the file to disable test behaviour
	TRAP(err, EComTestUtils::RfsDeleteFileL(finalFileName.FullName()));
	TEST2(aTest, err, KErrNone);
	}

