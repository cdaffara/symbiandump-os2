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

#ifndef __TEST_FOR_ALL_FILES_STEP_H__
#define __TEST_FOR_ALL_FILES_STEP_H__

// User Include
#include "TestFileUriServer.h"

/**
The test step that tests the creation of URI and converting it back to 
a filename for all the files in the system.
@internalTechnology
*/
class CTestForAllFilesStep : public CTestStep
	{	
	public:
	CTestForAllFilesStep();
	
	// TEF pure virtual
	virtual enum 	TVerdict doTestStepL();
	
	private:
	void 			DoTestL();
	void 			TestCreateFileUriL(const TPtrC& aFilename, RArray<TPtrC>& aFileUris, RArray<TPtrC>& aFileNames);
	
	template <class CUriType, class TUriParserType>
	void 			TestBothWaysL(CUriType*& cUri8Or16, TUriParserType& tUriParser8Or16, TFileName& aFileName);
	};

/*@{*/
// Literal constant defined for identifying step name
_LIT(KTestForAllFilesStep, "TestForAllFilesStep");	
/*@}*/

#endif		// __TEST_FOR_ALL_FILES_STEP_H__
