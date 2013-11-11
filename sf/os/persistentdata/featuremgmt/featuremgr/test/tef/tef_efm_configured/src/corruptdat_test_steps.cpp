// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EFM test steps for checking behaviour with invalid feature set data files,
// also with valid data files with overlapping DSR ranges.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <featmgr/featurecontrol.h>
#include "corruptdat_test_steps.h"
#include "efm_teststepbase.h"

CCorruptFeatDatFileStep::CCorruptFeatDatFileStep()
	{
	SetTestStepName(KCorruptFeatDatTest);
	}

TVerdict CCorruptFeatDatFileStep::doTestStepL()
	{
	// We can't use CFeatureDiscovery, that's not in the configured server.
	RFeatureControl control;
	TInt err = control.Open();
	// The server should panic with a bad feature data file, so we should
	// expect KErrGeneral returned from the client/server framework.
    TESTDIAGNOSTICERROR(err==KErrGeneral, _L("RFeatureControl::Open, expected KErrCorrupt, got error = %d"), err);
	control.Close();
	return TestStepResult();
	}

CMissingDSRDatFileStep::CMissingDSRDatFileStep()
	{
	SetTestStepName(KMissingDSRDatFileStep);
	}

TVerdict CMissingDSRDatFileStep::doTestStepL()
	{
	// We can't use CFeatureDiscovery, that's not in the configured server.
	RFeatureControl control;
	TInt err = control.Open();
	// This test uses a feature data file whose header says there are three
	// DSRs, where there are in fact only two.
	// The server should leave with a bad feature data file having less DSRs than it's specified in the file header, 
	//so we should expect KErrEof returned from the server startup function.
    TESTDIAGNOSTICERROR(err==KErrEof, _L("RFeatureControl::Open, expected KErrEof, got error = %d"), err);
	control.Close();
	return TestStepResult();
	}

CMissingDatFileStep::CMissingDatFileStep()
	{
	SetTestStepName(KMissingDatFileStep);
	}

TVerdict CMissingDatFileStep::doTestStepL()
	{
	// We can't use CFeatureDiscovery, that's not in the configured server.
	RFeatureControl control;
	TInt err = control.Open();
	// No ROM feature data file is present, therefore the server will panic.
	// The client-server framework will give us KErrGeneral/-2.
    TESTDIAGNOSTICERROR(err==KErrGeneral, _L("RFeatureControl::Open, expected KErrGeneral, got error = %d"), err);
	control.Close();
	return TestStepResult();
	}

COverlappingDSRFeatDatFileStep::COverlappingDSRFeatDatFileStep()
	{
	SetTestStepName(KOverlappingDSRFeatDatTest);
	}

TVerdict COverlappingDSRFeatDatFileStep::doTestStepL()
	{
	// We can't use feature discovery here because the configured server does not
	// have feature discovery client, so use RFeatureControl.
	RFeatureControl control;
	TInt err = control.Open();
    TESTDIAGNOSTICERROR(err==KErrNone,
	    _L("RFeatureControl::Open failed: error = %d"), err);
	if(err) return TestStepResult();
	CleanupClosePushL(control);

	const TUid uid1 = { 268435462 };  //  0x10000006
    TInt supported = control.FeatureSupported(uid1);
    TESTDIAGNOSTICERROR(supported == KErrNotFound, _L("RFeatureControl::FeatureSupported wrong:, expected KErrNotFound, got %d"), supported);

	const TUid uid2 = { 268435463 };  // 0x1000007
    supported = control.FeatureSupported(uid2);
    TESTDIAGNOSTICERROR(supported == KFeatureSupported, _L("RFeatureControl::FeatureSupported wrong:, expected KFeatureSupported, got %d"), supported);

	const TUid uid3 = { 268435472 };  // 0x1000010
    supported = control.FeatureSupported(uid3);
    TESTDIAGNOSTICERROR(supported == KFeatureSupported, _L("RFeatureControl::FeatureSupported wrong:, expected KFeatureSupported, got %d"), supported);

	const TUid uid4 = { 268435473 };  // 0x1000011
    supported = control.FeatureSupported(uid4);
    TESTDIAGNOSTICERROR(supported == KErrNotFound, _L("RFeatureControl::FeatureSupported wrong:, expected KErrNotFound, got %d"), supported);

	const TUid uid5 = { 268435464 };  // 0x1000008
    supported = control.FeatureSupported(uid5);
    TESTDIAGNOSTICERROR(supported == KFeatureSupported, _L("RFeatureControl::FeatureSupported wrong:, expected KFeatureSupported, got %d"), supported);

	CleanupStack::PopAndDestroy(&control);
	return TestStepResult();
	}

CCorruptRuntimeFeaturesStep::~CCorruptRuntimeFeaturesStep()
	{
	}

CCorruptRuntimeFeaturesStep::CCorruptRuntimeFeaturesStep()
	{
	SetTestStepName( KCorruptRuntimeFeaturesStep );
	}

/**
 * Tests if a corupt features.dat file on the ffs is deleted and is then created when a new peristed feature is
 * added.
 */
TVerdict CCorruptRuntimeFeaturesStep::doTestStepL()
	{
	RFeatureControl control;
	TInt err = control.Open();
	if (err == KErrNone)
		{

		RFs fs;
		fs.Connect();
		CleanupClosePushL(fs);

		//Block the test until featuremanager has finished opening. 
		control.FeatureSupported(KNewUid3);
		
		TEntry fileEntry;
		_LIT( KRuntimePath, "?:\\Private\\102836E5\\runtime\\features.dat" );
	    TFileName file( KRuntimePath );
	    file[0] = fs.GetSystemDriveChar();
	    err = fs.Entry(file, fileEntry);

		if (err != KErrNotFound) // file should not exist
			{
			ERR_PRINTF2( _L("KErrNotFound expected when checking if the file exists got  %d"), err);
			SetTestStepResult(EFail);
			}
		else
			{
			TBitFlags32 testFlag;
			testFlag.Set( EFeaturePersisted);
			TFeatureEntry testEntry( KNewUid3, testFlag, KDefaultData1 );
			
			err = control.AddFeature( testEntry );
			if (err == KErrNone)
				{
	
				err = fs.Entry(file, fileEntry);
				if (err != KErrNone) //file should now exist
					{
					ERR_PRINTF2( _L("KErrNone expected when checking if the file exists got  %d"), err);
					SetTestStepResult(EFail);
					}
				}
			else // feature not added
				{
				ERR_PRINTF2( _L("Unable to add the feature error = %d"), err);
				SetTestStepResult(EFail);
				}
			}
		CleanupStack::PopAndDestroy(&fs);
		control.Close();
		}
	else //open failed 
		{
		ERR_PRINTF2( _L("Unable to connect to featmgr error = %d"), err);
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}

