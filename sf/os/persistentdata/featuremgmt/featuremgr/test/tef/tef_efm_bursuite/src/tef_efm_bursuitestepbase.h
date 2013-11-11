// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent
*/

#if (!defined __TEF_EFM_BURSUITESTEPBASE__)
#define __TEF_EFM_BURSUITESTEPBASE__

#include "tef_efm_bursuitedefs.h"
#include <test/testexecutestepbase.h>
#include <e32property.h>
#include <bautils.h>


/**
 * This class forms the basic functionality required of a test step.
 * Including the usual preamble and postamble, it handles the file
 * specific operations.
 */ 
class CTefEfmBURSuiteStepBase : public CTestStep
	{
public:
    virtual ~CTefEfmBURSuiteStepBase();
	CTefEfmBURSuiteStepBase();

	virtual TVerdict doTestStepPreambleL(); 	
	virtual TVerdict doTestStepPostambleL(); 	
	
public:
	TVerdict DeleteDefaultBURFeatureFile( const TDesC & );
	
protected:
	TVerdict CopyFeatureFileBackupL();
	TVerdict CopyFeatureFileRestoreL();
	TVerdict CreateDirStructureL();
	TVerdict CleanUpTestFilesL();

	};

#endif
