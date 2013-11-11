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
*
*/


#include "t_testactionspec.h"
#include "t_testsetup.h"
#include "tScriptSetup.h"
#include "t_input.h"
#include "tScriptTests.h"
#include "t_tefinput.h"

_LIT8(KTrue, "true");
_LIT8(KFalse, "false");

_LIT8(KExOOMStart, "<exoom>");
_LIT8(KExOOMEnd, "</exoom>");
_LIT8(KInOOMStart, "<inoom>");
_LIT8(KInOOMEnd, "</inoom>");
_LIT8(KExCancelStart, "<excancel>");
_LIT8(KExCancelEnd, "</excancel>");
_LIT8(KInCancelStart, "<incancel>");
_LIT8(KInCancelEnd, "</incancel>");
_LIT8(KSkippedStart, "<skipped>");
_LIT8(KSkippedEnd, "</skipped>");
_LIT8(KSmokeStart, "<smoke>");
_LIT8(KSmokeEnd, "</smoke>");
_LIT8(KInteractiveStart, "<interactive>");
_LIT8(KInteractiveEnd, "</interactive>");


TTestActionSpec::TTestActionSpec()
	{
	}

TTestActionSpec::~TTestActionSpec()
	{
	iniSectionResultBody.Close();
	}

TInt TTestActionSpec::Init(const TDesC8& aInput, 
						   TInt& aPos,
						   CConsoleBase& /*aConsole*/,
						   Output& aOut,
						   TInt& aBitFlag)
	{
	TInt err = KErrNone;
	iActionName.Set(Input::ParseElement(aInput, KActionNameStart, KActionNameEnd, aPos, err));
	if (err != KErrNone)
		{
		aOut.writeString(_L("Error couldn't find actionname in test case spec"));
		aOut.writeNewLine();
		return err;
		}
	
	iActionType.Set(Input::ParseElement(aInput, KActionTypeStart, KActionTypeEnd, aPos, err));
	if (err != KErrNone)
		{
		aOut.writeString(_L("Error couldn't find actiontype in test case spec"));
		aOut.writeNewLine();
		return err;
		}
	
	iActionGroup.Set(Input::ParseElement(aInput, KActionGroupingStart, KActionGroupingEnd, aPos, err));
	if (err == KErrNone)
		{
		//Do parsing of the returned groupings string
		TInt relativePos=0;
		TPtrC8 excludeOOM = Input::ParseElement(iActionGroup, KExOOMStart, KExOOMEnd, relativePos, err);
		relativePos=0;
		TPtrC8 includeOOM = Input::ParseElement(iActionGroup, KInOOMStart, KInOOMEnd, relativePos, err);
		relativePos=0;
		TPtrC8 excludeCancel = Input::ParseElement(iActionGroup, KExCancelStart, KExCancelEnd, relativePos, err);
		relativePos=0;
		TPtrC8 includeCancel = Input::ParseElement(iActionGroup, KInCancelStart, KInCancelEnd, relativePos, err);
		relativePos=0;
		TPtrC8 skipped = Input::ParseElement(iActionGroup, KSkippedStart, KSkippedEnd, relativePos, err);
		relativePos=0;
		TPtrC8 smoketest = Input::ParseElement(iActionGroup, KSmokeStart, KSmokeEnd, relativePos, err);
		relativePos=0;
		TPtrC8 interactive = Input::ParseElement(iActionGroup, KInteractiveStart, KInteractiveEnd, relativePos, err);

		if (excludeOOM==KTrue)
			{
			aBitFlag|=EXOOM;
			}
		else if (excludeOOM==KFalse)
			{
			aBitFlag&= (~EXOOM);
			}
		
		if (includeOOM==KTrue)
			{
			aBitFlag|=INOOM;
			}
		else if (includeOOM==KFalse)
			{
			aBitFlag&= (~INOOM);
			}

		if (excludeCancel==KTrue)
			{
			aBitFlag|=EXCANCEL;
			}
		else if (excludeCancel==KFalse)
			{
			aBitFlag&= (~EXCANCEL);
			}

		if (includeCancel==KTrue)
			{
			aBitFlag|=INCANCEL;
			}
		else if (includeCancel==KFalse)
			{
			aBitFlag&= (~INCANCEL);
			}

		if (skipped==KTrue)
			{
			aBitFlag|=SKIP;
			}
		else if (skipped==KFalse)
			{
			aBitFlag&= (~SKIP);
			}
	
		if (smoketest==KTrue)
			{
			aBitFlag|=SMOKE;
			}
		else if (smoketest==KFalse)
			{
			aBitFlag&= (~SMOKE);
			}

		if (interactive==KTrue)
			{
			aBitFlag|=INTER;
			}
		else if (interactive==KFalse)
			{
			aBitFlag&= (~INTER);
			}
		}
	
	iActionBody.Set(Input::ParseElement(aInput, KActionBodyStart, KActionBodyEnd, aPos, err));
	iActionResult.Set(Input::ParseElement(aInput, KActionResultStart, KActionResultEnd, aPos, err));
	iTefScript = EFalse;
	
	return KErrNone; 
	}


// Extracts information from *.script and *.ini files to the appropriate member variables.

TInt TTestActionSpec::TEFInit(RFs& aFs, 
						   const TDesC8& aInput,
						   const TDesC8& aTestCaseID,
						   TDesC8& aPrevTestCaseID,
						   TBool& tefFile,
						   TBool& runtest,
						   TBool& inifile,
						   TDes8& aScriptResult,
						   TDes8& aActionType,
						   const TDesC& aScriptPath,
						   CConsoleBase& /*aConsole*/,
						   Output& aOut)
	{
	
	TInt 		err = KErrNone;
	TInt 		actionTypePos=3;
	TBool 		scriptactionResultSet = EFalse;
//	TBool 		iniactionResultSet = EFalse;
	TBuf8<512> 	prevTestCaseID;
	TBuf8<512> 	prevTestDescription;


	
// Checks if the first word from the script file is an error code
// if(error code)
//		sets 'actionResult' to the relevent error code with the <return></return> tags

	TPtrC8 firstWord = Tefinput::ParseNthElement(aInput,1, err);
	if(err == KErrNone)
		{
		TInt tmpno;
		TLex8 lex8(firstWord); 
		err = lex8.Val(tmpno);			
		if(err != KErrNone)
			{
			actionTypePos = 4;
			err = Tefinput::ParseActionResult(firstWord, aScriptResult);
			scriptactionResultSet = ETrue;
			}
		}
	
//1. Extracts the .ini file name from the .script file
//2. Reads in the [section] name from the .script file
//3. Generates  .ini path from .script file path and .ini file name
//4. Sets iActionBody with the info from .ini file
	
	TBuf<KMaxPath + KMaxFileName> 	iniFilePath;
	TBuf<512> 						iniFileData;
	TPtrC8 							iniSectionData;
	
	
	TPtrC8 iniFileName = Tefinput::ParseNthElement(aInput,actionTypePos+1,err);
	if(err == KErrNone)
		{
		TPtrC8 iniSectionName = Tefinput::ParseNthElement(aInput,actionTypePos+2,err);
		if(err == KErrNone)
			{
			err = Tefinput::ParseiniPath(iniFileName, aScriptPath, iniFilePath);
			if(err == KErrNone)
				{
				aTestIniFilePtr.Assign(Tefinput::GetiniFile(aFs, iniFilePath, err)); // Load up our local RBuf ptr to takeover the management of the inifile data in Heap
				
				if(err == KErrNone)
					{
					TPtrC8 iniFile = aTestIniFilePtr.Ptr();
					inifile = ETrue;
					err = Tefinput::ParseActionbody(iniFile, iniSectionName, iniSectionData);
					if(err == KErrNone)
						{
						TInt pos = 0;
						iActionBody.Set(Input::ParseElement(iniSectionData, KActionBodyStart, KActionBodyEnd, pos, err));
						if (err == KErrNotFound)
							{
							aOut.writeString(_L("Error couldn't find actionbody in test case spec"));
							aOut.writeNewLine();
							return err;
							}
						}
					}
				else
					{
					inifile = EFalse;
					}
				}
			}
		}
	else
		{
		inifile = EFalse;
		}
		
	TInt pos = 0;

// Extracts info b/w <actionresult> </actionresult> tags
// 	Sets iActionResult with info both from .script and .ini file
	TPtrC8 tempResult;
	TPtrC8 scriptResult;
	TInt resultlen ;
	
	if(scriptactionResultSet)
		{
		scriptResult.Set(aScriptResult);
		tempResult.Set(Input::ParseElement(iniSectionData, KActionResultStart, KActionResultEnd, pos, err));
		if (err == KErrNone)
			{
			resultlen = scriptResult.Length() + tempResult.Length();
			iniSectionResultBody.Create(tempResult,resultlen);
			iniSectionResultBody.Insert(0,scriptResult);			
			iActionResult.Set(iniSectionResultBody);
//			iniactionResultSet = ETrue;
			}
		else
			{
			iniSectionResultBody.Create(scriptResult);
			iActionResult.Set(iniSectionResultBody);
			}
		}
	else
		{
		tempResult.Set(Input::ParseElement(iniSectionData, KActionResultStart, KActionResultEnd, pos, err));
		err = Tefinput::GetActionResult(KErrNone, aScriptResult);
		if(err == KErrNone)
			{
			scriptResult.Set(aScriptResult);
			resultlen = tempResult.Length() + scriptResult.Length();
			iniSectionResultBody.Create(tempResult, resultlen);
			iniSectionResultBody.Insert(0,aScriptResult);
			iActionResult.Set(iniSectionResultBody);
			}
		else
			{
			iniSectionResultBody.Create(tempResult);
			iActionResult.Set(iniSectionResultBody);
			}
		}
	
		
//	aPrevTestCaseID = aTestCaseID;
//Sets iActionName with the @SYMTestCaseID
	if(!runtest)
		{
		iActionName.Set(aPrevTestCaseID);
		}
	else
		{
		iActionName.Set(aTestCaseID);
		}

	

//Sets iActionType from .script file
	
	TBuf8<512> modifiedType;
	TBuf8<512> modifiedTypeTemp;
	TPtrC8 actionTypeTemp;
	TPtrC8 actionType;
	TPtrC8 TempType;
	
	
	iActionType.Set(Tefinput::ParseNthElement(aInput,actionTypePos,err));
	if (err == KErrNone)
		{
		modifiedType.Copy(iActionType);
		modifiedType.Copy(Tefinput::TrimActionType(modifiedType, aActionType));

		iActionType.Set(aActionType);
		}
	else
		{
		aOut.writeString(_L("Error couldn't find actiontype in test case spec"));
		aOut.writeNewLine();
		return err;
		}

	

	iTefScript = tefFile;


	
	
	return KErrNone; 
	}


void TTestActionSpec::HardcodedInit(const TDesC8& aInput)
	{
	iActionName.Set(aInput);
	iActionType.Set(KNullDesC8);
	iActionGroup.Set(KNullDesC8);
	iActionBody.Set(KNullDesC8);
	iActionResult.Set(KNullDesC8);
	}
