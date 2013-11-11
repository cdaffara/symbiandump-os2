// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Feature Registry test step base class - utilities for derived test steps.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include "te_featreg_stepbase.h"
#include <e32uid.h>
#include "featregcmn.h"

/**
 * Runs the featregsetup.exe as a separate process, waiting for completion.
 * @return KErrNone on success, or negative error code, or positive panic code if setup panicked.
 */
TInt CTe_FeatReg_StepBase::RunFeatRegSetup()
	{
	RProcess setupProc;
	const TUidType setupExeUidType(KExecutableImageUid, TUid::Null(), KFeaturePropCat);
	TInt result = setupProc.Create(KFeatRegSetupExe, KNullDesC, setupExeUidType);
	if (result != KErrNone)
		{
		return result;
		}
	TRequestStatus setupStatus;
	setupProc.Rendezvous(setupStatus);
	setupProc.Resume();
	setupProc.Close();
	User::WaitForRequest(setupStatus);
	return setupStatus.Int();
	}
	
_LIT(KFeatRegMakeTestConfigExe, "z:\\sys\\bin\\featreg_maketestconfig.exe");

/**
 * Runs the featreg_maketestconfig.exe, waiting for completion.
 * @return KErrNone on success, or negative error code, or positive panic code if exe process panicked.
 */
TInt CTe_FeatReg_StepBase::RunMakeTestConfig(const TDesC& aConfigName)
	{
	TRequestStatus runStatus;
	RProcess setupProc;
	TInt result = setupProc.Create(KFeatRegMakeTestConfigExe, aConfigName);
	if (result != KErrNone)
		{
		return result;
		}
	setupProc.Resume();
	setupProc.Rendezvous(runStatus);
	User::WaitForRequest(runStatus);
	setupProc.Close();
	return runStatus.Int();
	}
