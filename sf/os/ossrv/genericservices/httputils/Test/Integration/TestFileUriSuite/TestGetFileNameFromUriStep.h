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

#ifndef __TEST_GET_FILENAME_FROM_URI_STEP_H__
#define __TEST_GET_FILENAME_FROM_URI_STEP_H__

// User Include
#include "TestFileUriServer.h"

/**
The test step that tests GetFileNameL method, that obtains the 
file-path and name, for a URI specified in the ini file. 
@internalTechnology
*/
class CTestGetFileNameFromUriStep : public CTestStep
	{	
	public:
	CTestGetFileNameFromUriStep();
	
	// TEF pure virtual
	virtual enum 	TVerdict doTestStepL();
	
	private:
	void 			DoTestL(const TPtrC& aFileUri, const TPtrC& aExpectedFileName, const TPtrC& aFileType, TPtrC& aDrive, const TInt& aCharacterSet);
	
	template 		<class TUriParserType, class HBufCType>
	void 			ExtractFileNameAndCompareL(TUriParserType& uriParser8Or16, HBufCType*& fileUri8Or16, const TPtrC& aFileUri, const TFileName& aFullyQualifiedName, const TPtrC& aFileType, const TBool& aFileNotFound);
	
	TInt 			CreateFullyQualifiedName(const TPtrC& aRelativeName, const TPtrC& aDrive, TFileName& aFullyQualifiedName);
	
	};

/*@{*/
// Literal constant defined for identifying step name
_LIT(KTestGetFileNameFromUriStep, "TestGetFileNameFromUriStep");	
/*@}*/

#endif		// __TEST_GET_FILENAME_FROM_URI_STEP_H__
