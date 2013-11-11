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


#ifndef __T_TESTACTIONSPEC_H__
#define __T_TESTACTIONSPEC_H__

#include <e32base.h>
#include <f32file.h> 


class CConsoleBase;
class Output;

/**
 * TODO
 */
class TTestActionSpec
	{ 
public:
	TTestActionSpec();
	~TTestActionSpec();
	TInt Init(const TDesC8& aInput, TInt& aPos, CConsoleBase& aConsole, Output& aOut, TInt& aBitFlag);
//	TInt TEFInit(RFs& aFs, const TDesC8& aInput, HBufC8*& aSectionData, HBufC8*& aIniSectionResultBody, const TDesC8& aTestCaseID, const TDesC8& aPrevTestCaseID, const TDesC8& aTestDescription, TBool testSet, const TDesC& aScriptPath, CConsoleBase& aConsole, Output& aOut, TInt& aBitFlag);
	TInt TEFInit(RFs& aFs, const TDesC8& aInput, const TDesC8& aTestCaseID, TDesC8& aPrevTestCaseID, TBool& tefFile, TBool& runtest, TBool& iniFile, TDes8& aScriptResult , TDes8& aActionType, const TDesC& aScriptPath, CConsoleBase& /*aConsole*/, Output& aOut);
	void HardcodedInit(const TDesC8& aInput);

public:
	TPtrC8 iActionName;
	TPtrC8 iActionType;
	TPtrC8 iActionGroup;
	TPtrC8 iActionBody;
	TPtrC8 iActionResult;
	

	
	TBool	iTefScript;
	
	TPtrC8 iActionDescription;

	RBuf8  iniSectionResultBody;
	RBuf8 aTestIniFilePtr;		// This will hold the location in Heap where the ini file has been loaded for general use

	/**
	 * This member indicates the expected result of the action.
	 */
	
	};

#endif

