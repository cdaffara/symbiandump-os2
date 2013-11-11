/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#if (!defined __CLEANUP_STEP_H__)
#define __CLEANUP_STEP_H__
#include <test/testexecutestepbase.h>

#include "cafstep.h"
#include <caf/dirstreamable.h>
#include "contentIterator.h"

// Constants used to name test cases 
_LIT(KCAFTestCleanupStep,"CAFCleanupStep");

class CCAFServer;

/* 
* @internalComponent Exposure internally
*/
class CCAFTestCleanupStep : public CCAFStep
	{
public:
	CCAFTestCleanupStep(CCAFServer& aParent);
	~CCAFTestCleanupStep();
	virtual TVerdict doTestStepL();
private:
	void MakeFilesWritable(CFileMan& aFileMan, const TDesC& aDir);
	CCAFServer& iParent;
	};

#endif

