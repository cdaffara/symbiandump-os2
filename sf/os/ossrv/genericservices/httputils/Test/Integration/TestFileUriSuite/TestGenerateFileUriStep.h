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

#ifndef __TEST_GENERATE_FILE_URI_STEP_H__
#define __TEST_GENERATE_FILE_URI_STEP_H__

// User Include
#include "TestFileUriServer.h"

/**
The test step that tests CreatePrivateFileUriL and CreateFileUriL methods, 
that generate a file uri for a file-path and name specified in the ini file. 
@internalTechnology
*/
class CTestGenerateFileUriStep : public CTestStep
	{	
	public:
	CTestGenerateFileUriStep();
	
	// TEF pure virtual
	virtual enum TVerdict 	doTestStepL();
	
	private:
	void 					DoTestL(const TPtrC& aFileName, const TPtrC& aExpectedFileUri, const TPtrC& aFileType, const TPtrC& aDrive, const TInt& aCharacterSet);
	
	template <class CUriType>
	void	 				GenerateFileUriAndCompareL(CUriType*& cUri8Or16, const TPtrC& aFileName, HBufC16*& aExpectedFileUri8Or16, const TUint& aFlags, const TInt& aDriveNum, const TPtrC& aFileType);
	};

/*@{*/
// Literal constant defined for identifying step name
_LIT(KTestGenerateFileUriStep, "TestGenerateFileUriStep");	
/*@}*/

#endif		// __TEST_GENERATE_FILE_URI_STEP_H__
