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


#include "util.h"

using namespace UserPromptService;

const TDesC& OptionToString(CPolicy::TOptions aOption)
	{
	switch (aOption)
		{
		case CPolicy::EYes:
			_LIT(KYes, "Yes");
			return KYes;
		case CPolicy::ENo:
			_LIT(KNo, "No");
			return KNo;
		case CPolicy::ESessionYes:
			_LIT(KSessionYes, "SessionYes");
			return KSessionYes;
		case CPolicy::ESessionNo:
			_LIT(KSessionNo, "SessionNo");
			return KSessionNo;
		case CPolicy::EAlways:
			_LIT(KAlways, "Always");
			return KAlways;
		case CPolicy::ENever:
			_LIT(KNever, "Never");
			return KNever;
		default:		
			_LIT(KUnknown, "Unknown");
			return KUnknown;
		}	
	}

TInt StringToOptionL(const TDesC& aString)
	{
	if (aString.MatchF(_L("yes")))
		return CPolicy::EYes;
	else if (aString.MatchF(_L("no")))
		return CPolicy::ENo;
	else if (aString.MatchF(_L("session")))
		return CPolicy::ESessionYes;
	else if (aString.MatchF(_L("sessionyes")))
		return CPolicy::ESessionYes;
	else if (aString.MatchF(_L("sessionno")))
		return CPolicy::ESessionNo;
	else if (aString.MatchF(_L("always")))
		return CPolicy::EAlways;
	else if (aString.MatchF(_L("never")))
		return CPolicy::ENever;	
	
	User::Leave(KErrGeneral);
	/*lint -unreachable*/
	return 0;
	}
	
TBool CheckExpectedError(CTestExecuteLogger& aLogger, TInt aExpected, TInt aActual)
/**
Compares the expected and the actual error and logs the result.
@param aExpected 	The expected error code.
@param aActual 		The actual error.
@return ETrue, if the expected and actual errors matched; otherwise, EFalse is returned.
*/
	{
	if (aActual == KErrNone && aExpected == KErrNone)
		{
		aLogger.LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, 
			_L("Passed - no error"));
		return ETrue;
		}
	else if (aActual == aExpected)
		{
		aLogger.LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, 
			_L("Encountered expected error %d"), aActual);
		return ETrue;
		}
	else if (aActual == KErrNone)
		{
		aLogger.LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, 
			_L("Did not encounter expected error %d"), aExpected);
		return EFalse;
		}
	else 
		{
		aLogger.LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, 
			_L("Unexpected error %d"), aActual);
		return EFalse;
		}	
	}
