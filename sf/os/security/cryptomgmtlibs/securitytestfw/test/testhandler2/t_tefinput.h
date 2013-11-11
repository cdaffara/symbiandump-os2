//
// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
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


#ifndef T_TEFINPUT_H_
#define T_TEFINPUT_H_

#include <e32cons.h>
#include <f32file.h>
#include <e32std.h>
#include "t_input.h"



class Tefinput
	{
public: // static functions no object required
	
	// Extracts RUN_TESTSTEP from the whole of script file passed in
	IMPORT_C static TPtrC8 ParseRunTestStep(const TDesC8& aBuf, const TDesC8& aTag, TInt& aPos, TInt& aError);
	
	// Extracts the nth word from the given descritor
	IMPORT_C static TPtrC8 ParseNthElement(const TDesC8& aBuf, TInt aWordPos, TInt& aError);
	
	// Extracts testCaseID from the whole script file
	IMPORT_C static TPtrC8 ParseTestCaseID(const TDesC8& aBuf, const TDesC8& aTag, TDes8& aPrevTestID, TInt& aPos, TInt& aError, TBool& testSet, TBool& startTest);
			
	// Generates the path for .ini file from the .script file path
	IMPORT_C static TInt ParseiniPath(const TDesC8& aIniFileName, const TDesC& aScriptPath, TDes& aIniFilePath);
	
	// Reads the whole of .ini file contents, calls ParseActionbody()to extract info between <actionbody></actionbody>tags
	IMPORT_C static HBufC8* GetiniFile(RFs& aFs, const TDesC& aIniFilePath, TInt& err);
	
	// Extracts info between <actionbody></actionbody>tags from the inifile contents
	IMPORT_C static TInt ParseActionbody(TPtrC8& aBuf, const TDesC8& aIniTag, TPtrC8& aSectionBody);
	
	// Takes in an integer and returns the appropriate error code with <return></return> tags
	IMPORT_C static TInt GetActionResult(TInt aErrCode, TDes8& aReturnTag);
	
	// Checks if the first word in RUN_TEST_STEP is an error code. If, then calls GetActionResult().
	IMPORT_C static TInt ParseActionResult(const TDesC8& aBuf, TDes8& aActionResult);
	
	
	// Removes any /t, /n and empty spaces from any extracted data.
	IMPORT_C static TPtrC8 Trim(const TDesC8& aBuf);
	
	//Removes '_' that is joining 2 words
	IMPORT_C static TDes8 TrimActionType(const TDesC8& aBuf, TDes8& aType);
	
	//Returns ETRUE if reached end of a testcase
	IMPORT_C static TBool EndTestCase(const TDesC8& aBuf, const TDesC8& aRunStep, const TDesC8& aTag, TInt& aPos);

	
};

#endif 
