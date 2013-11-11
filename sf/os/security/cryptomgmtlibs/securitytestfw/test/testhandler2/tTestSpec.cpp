/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* T_TESTSPEC.CPP
*
*/


#include "tTestSpec.h"
#include "t_testaction.h"

EXPORT_C CTestSpec* CTestSpec::NewL()
	{
	CTestSpec* self = new(ELeave) CTestSpec();
	return self;
	}

CTestSpec::CTestSpec()
	{
	}

CTestSpec::~CTestSpec()
	{
	iTestActionArray.ResetAndDestroy();
	iTestActionArray.Close();
	}

EXPORT_C TBool CTestSpec::GetNextTest(CTestAction*& aAction)
	{
	// Delete the previous test to free memory up for use by later tests
	TInt prevTest = iTestNumber - 1;
	if (prevTest >= 0 && prevTest < iTestActionArray.Count())
		{
		delete iTestActionArray[prevTest];
		iTestActionArray[prevTest] = NULL;
		}
	
	if (iTestNumber >= iTestActionArray.Count())
		{
		return EFalse;
		}

	aAction=iTestActionArray[iTestNumber];
	++iTestNumber;
	
	return ETrue;
	}

EXPORT_C TInt CTestSpec::AddNextTest(CTestAction* aAction)
	{
	return iTestActionArray.Append(aAction);
	}

void CTestSpec::FreeAllTests()
	{
	iTestActionArray.ResetAndDestroy();
	}
