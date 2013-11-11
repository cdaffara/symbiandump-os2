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

#ifndef __TEST_DELETE_FILE_STEP_H__
#define __TEST_DELETE_FILE_STEP_H__

// User Include
#include "TestFileUriServer.h"

/**
The test step that Deletes a file based on the path and name specified 
in the ini file. 
@internalTechnology
*/
class CTestDeleteFileStep : public CTestStep
	{	
	public:
	CTestDeleteFileStep();
	
	// TEF pure virtual
	virtual enum TVerdict doTestStepL();
	};

/*@{*/
// Literal constant defined for identifying step name
_LIT(KTestDeleteFileStep, "TestDeleteFileStep");	
/*@}*/

#endif		// __TEST_DELETE_FILE_STEP_H__
