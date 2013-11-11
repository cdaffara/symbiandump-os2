/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file TEFScriptUtils.h
*/

#if !(defined __TEF_SCRIPT_UTILS_H__)
#define __TEF_SCRIPT_UTILS_H__

#include <e32base.h>
#include <f32file.h>
#include <test/tefexportconst.h>

struct TCommandForCounting
/**
@internalComponent
@test
*/
	{
	TBuf<KMaxTestExecuteNameLength> iCommandName;
	TInt iCommandCount;
	};

class CScriptPreProcess : public CBase
/**
@internalComponent
@test
*/
	{
	public:
	CScriptPreProcess(const TDesC& aScriptFilePath, TPtrC aCommandList[KTEFCommandCountsStructSize]);
	~CScriptPreProcess();
	TInt CountNoOfOccurences();
	inline TInt GetStructSize(){return iStructSize;};
	void RetrieveValues(TInt& aIndex, TDes& aCommandName, TInt& aCommandCount);	
	
	//private:
	void CreateScriptDataFromScriptFileL();
	TBool GetNextScriptLine();
	void UpdateCommandCounts();
	
	private:
	TBuf<KMaxTestExecuteCommandLength> iScriptFilePath;
	TInt iStructSize;
	// Update the array size from TEFExportConst.h whenever the number of commands to count varies
	TCommandForCounting iCommandsAndCounts[KTEFCommandCountsStructSize];
	HBufC16* iScriptData;
	TLex iScriptLex;
	TPtrC iCurrentScriptLine;
	};

#endif
