// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// UnitTestSteps.cpp
// Feature Discovery unit test steps
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include "efm_unit_teststeps.h"
#include "efm_teststepbase.h"
#include "featmgrfeatureregistry.h"
#include "efm_test_consts.h"
#include <apgcli.h>
#include <apacmdln.h>

CServerDataTestStep::CServerDataTestStep()
	{
	SetTestStepName(KServerDataTest);
	}

TVerdict CServerDataTestStep::doTestStepL()
	{
	TFeatureServerEntry defaultEntry;
	TESTDIAGNOSTICERROR(defaultEntry.FeatureFlags().Value()==0,		
		_L("TFeatureServerEntry has wrong feature flags: value = %d"),defaultEntry.FeatureFlags().Value());	
	TESTDIAGNOSTICERROR(defaultEntry.FeatureData()==0,
		_L("TFeatureServerEntry has wrong feature data: value = %d"),defaultEntry.FeatureData());
	TESTDIAGNOSTICERROR(defaultEntry.FeatureUid().iUid==0,
		_L("TFeatureServerEntry has wrong feature flags: value = %d"),defaultEntry.FeatureUid());	

	TFeatureServerEntry uidEntry(KDefaultSupportedUid);
	TESTDIAGNOSTICERROR(uidEntry.FeatureUid()==KDefaultSupportedUid,
			_L("TFeatureServerEntry has wrong feature flags: value = %d"),defaultEntry.FeatureUid());
	TESTDIAGNOSTICERROR(uidEntry.FeatureFlags().Value()==0,		
		_L("TFeatureServerEntry has wrong feature flags: value = %d"),defaultEntry.FeatureFlags().Value());	
	TESTDIAGNOSTICERROR(uidEntry.FeatureData()==0,
		_L("TFeatureServerEntry has wrong feature data: value = %d"),defaultEntry.FeatureData());
	
	TFeatureServerEntry argumentListEntry(KDefaultSupportedUid, TBitFlags(1), 1);
	TESTDIAGNOSTICERROR(argumentListEntry.FeatureUid()==KDefaultSupportedUid,
			_L("TFeatureServerEntry has wrong feature flags: value = %d"),defaultEntry.FeatureUid());
	TESTDIAGNOSTICERROR(argumentListEntry.FeatureFlags().Value()==1,		
		_L("TFeatureServerEntry has wrong feature flags: value = %d"),defaultEntry.FeatureFlags().Value());	
	TESTDIAGNOSTICERROR(argumentListEntry.FeatureData()==1,
		_L("TFeatureServerEntry has wrong feature data: value = %d"),defaultEntry.FeatureData());

	return TestStepResult();
	}

// ---------------------------------------------------------------------------

// Create some resources necessary to the tests. Do this in the preamble
// so that it isn't affected if we're running with the TEF '!OOM' option.
// That only operates on the 'doTestStepL' method.
TVerdict CServerBaseTestStep::doTestStepPreambleL()
	{
	CEFMConfigurableTestStepBase::doTestStepPreambleL();
	TInt err = ifs.Connect();
    TESTDIAGNOSTICERROR(err==KErrNone,
		_L("CServerBaseTestStep::doTestStepPreambleL RFS::Connect expected to return KErrNone: returned value is %d"),err);	
	if(err) return TestStepResult();
	CleanupClosePushL(ifs);

	ifeatureSet = CFeatMgrFeatureRegistry::NewL(ifs, *this);

    CleanupStack::PushL(ifeatureSet);
	return TestStepResult();
	}

// Clean up the resources previously allocated.
TVerdict CServerBaseTestStep::doTestStepPostambleL()
	{
	CEFMConfigurableTestStepBase::doTestStepPostambleL();
	CleanupStack::PopAndDestroy(ifeatureSet);
	CleanupStack::PopAndDestroy(&ifs);
	return TestStepResult();
	}

// ---------------------------------------------------------------------------

CServerFeatureLookupTestStep::CServerFeatureLookupTestStep()
	{
	SetTestStepName(KServerFeatureLookupTest);
	}

TVerdict CServerFeatureLookupTestStep::doTestStepL()
	{
    //check that an emty registry returns expected values
	TFeatureServerEntry feaureEntry1(KDefaultSupportedUid,TBitFlags32(1),0);
	TInt supported = ifeatureSet->IsFeatureSupported( feaureEntry1 );
	TESTDIAGNOSTICERROR(supported==KErrNotFound,
		_L("Feature is not expected to be supported in the empty feature registry: value = %d"),supported);
		
	//add one feature and check that it is reported as found	
	TInt error = ifeatureSet->AddFeature( feaureEntry1 );	
	CheckCondition(error==KErrNone, error==KErrNoMemory,
		_L("CFeatMgrFeatureRegistry::AddFeature failed: error = %d"),error);
	if (error==KErrNoMemory)
		{
		return TestStepResult();
		}
	supported = ifeatureSet->IsFeatureSupported( feaureEntry1 );
	TESTDIAGNOSTICERROR(supported==KFeatureSupported,
		_L("Feature is expected to be supported after being added: value = %d"),supported);	
		
	//check that non-existing features are reported as not found	
	TUid supportedUid(KDefaultSupportedUid);
	TFeatureServerEntry feaureEntry2(TUid::Uid(supportedUid.iUid-1));
	supported = ifeatureSet->IsFeatureSupported( feaureEntry2 );
	TESTDIAGNOSTICERROR(supported==KErrNotFound,
		_L("Feature is not expected to be supported: value = %d"),supported);		
	TFeatureServerEntry feaureEntry3(TUid::Uid(supportedUid.iUid+1));
	supported = ifeatureSet->IsFeatureSupported( feaureEntry3 );
	TESTDIAGNOSTICERROR(supported==KErrNotFound,
		_L("Feature is not expected to be supported: value = %d"),supported);			
	
	//add one more feature	
	TFeatureServerEntry feaureEntry4(TUid::Uid(supportedUid.iUid+2),TBitFlags32(1),0);
	error = ifeatureSet->AddFeature( feaureEntry4 );	
	CheckCondition(error==KErrNone, error==KErrNoMemory,
		_L("CFeatMgrFeatureRegistry::AddFeature failed: error = %d"),error);
	if (error==KErrNoMemory)
		{
		return TestStepResult();
		}
	//check that the original feature is still supported
	supported = ifeatureSet->IsFeatureSupported( feaureEntry1 );
	TESTDIAGNOSTICERROR(supported==KFeatureSupported,
		_L("Feature is expected to be supported after being added: value = %d"),supported);	
	
	//check that the newly added feature is supported
	supported = ifeatureSet->IsFeatureSupported( feaureEntry4 );
	TESTDIAGNOSTICERROR(supported==KFeatureSupported,
		_L("Feature is expected to be supported after being added: value = %d"),supported);			
	
	//check that non-existing features are still reported as not found	
	supported = ifeatureSet->IsFeatureSupported( feaureEntry2 );
	TESTDIAGNOSTICERROR(supported==KErrNotFound,
		_L("Feature is expected to be not found: value = %d"),supported);		
	TFeatureServerEntry feaureEntry5(TUid::Uid(supportedUid.iUid+3));
	supported = ifeatureSet->IsFeatureSupported( feaureEntry5 );
	TESTDIAGNOSTICERROR(supported==KErrNotFound,
		_L("Feature is expected to be not found: value = %d"),supported);				
	
	return TestStepResult();
	}

CServerUpdateCodeTestStep::CServerUpdateCodeTestStep()
	{
	SetTestStepName(KServerUpdateCodeTest);
	}

// Run the test..
TVerdict CServerUpdateCodeTestStep::doTestStepL()
	{
    //read the feature data files
    TRAPD(err, ifeatureSet->ReadFeatureFilesL());
    CheckCondition(err==KErrNone,err==KErrNoMemory,
		_L("CFeatMgrFeatureRegistry::ReadFeatureFilesL failed: error = %d"), err);
    if (err==KErrNoMemory)
    	{
    	return TestStepResult();
    	}
    
	TInt nfeat = ifeatureSet->NumberOfSupportedFeatures();
	TESTDIAGNOSTICERROR(nfeat==1,
		_L("CFeatMgrFeatureRegistry::NumberOfSupportedFeatures expected to return 1: returned value is %d"), nfeat);	

    TUint32 featureData = KChangeData;
    err = ifeatureSet->SetFeature( KDefaultSupportedUid, 0, &featureData);
    CheckCondition(err==KErrNone,err==KErrNoMemory,
		_L("CFeatMgrFeatureRegistry::SetFeature failed: error = %d"),err);
    if (err==KErrNoMemory)
        {
        return TestStepResult();
        }
    
	err = ifeatureSet->SetFeature(KUnknownUid, 0, &featureData);	
	TESTDIAGNOSTICERROR(err==KErrNotFound, 
		_L("CFeatMgrFeatureRegistry::SetFeature for an undefined feature failed: returned value is %d"),err);	
	if (err==KErrNoMemory)
	    {
	    return TestStepResult();
	    }
	
	err = ifeatureSet->SetFeature(KDSR1Uid, 0, &featureData); 
	TESTDIAGNOSTICERROR(err==KErrNotFound, 
		_L("CFeatMgrFeatureRegistry::SetFeature for a feature from DSR failed: returned value is %d"),err);
			
	return TestStepResult();
	}

CServerReadRuntimeFeaturesTestStep::CServerReadRuntimeFeaturesTestStep()
	{
	SetTestStepName(KServerReadRuntimeFeaturesTest);
	}

TVerdict CServerReadRuntimeFeaturesTestStep::doTestStepL()
	{
	TBool mybool;
    TRAPD(err, ifeatureSet->ReadRuntimeFeaturesL(mybool));
    TESTDIAGNOSTICERROR(err==KErrNone,
		_L("CFeatMgrFeatureRegistry::ReadRuntimeFeaturesL is expected to return KErrNone: returned value is %d"), err);	
    TESTDIAGNOSTICERROR(mybool,
		_L("CFeatMgrFeatureRegistry::ReadRuntimeFeaturesL, boolean should be true. Returned value is %d"), mybool);	
	
	return TestStepResult();
	}

CServerMergePluginFeaturesTestStep::CServerMergePluginFeaturesTestStep()
	{
	SetTestStepName(KServerMergePluginFeaturesTest);
	}

TVerdict CServerMergePluginFeaturesTestStep::doTestStepPreambleL()
	{
	// Get the required RFs handle and CFeatMgrFeatureRegistry object set up.
	CServerBaseTestStep::doTestStepPreambleL();

	// Set up an RFeatureArray with 5 features. These uids are arbitrary.
	// We do this in the preamble because this test is expected to operate
	// under low memory conditions with the TEF '!OOM' option. We don't
	// want that interfering with the creation of the RFeatureArray object
	// necessary to the test.
	TBitFlags32 flags1(0);
	flags1.Set( EFeatureSupported );
	flags1.Set( EFeatureModifiable );
	TFeatureEntry newentry1(KMergeUid1, flags1, KDefaultData1 );

	TBitFlags32 flags2(0);
	flags2.Set( EFeatureSupported );
	flags2.Set( EFeatureBlackListed );
	TFeatureEntry newentry2(KMergeUid2, flags2, KDefaultData1 );

	TBitFlags32 flags3(0);
	flags3.Set( EFeatureSupported );
	flags3.Set( EFeatureUpgradeable );
	flags3.Set( EFeatureModifiable );
	TFeatureEntry newentry3(KMergeUid3, flags3, KDefaultData1 );

	TBitFlags32 flags4(0);
	flags4.Set( EFeatureSupported );
	flags4.Set( EFeatureUpgradeable );
	TFeatureEntry newentry4(KMergeUid4, flags4, KDefaultData1 );

	TBitFlags32 flags5(0);
	flags5.Set( EFeatureSupported );
	flags5.Set( EFeatureModifiable );
	flags5.Set( EFeaturePersisted );
	TFeatureEntry newentry5(KMergeUid5, flags5, KDefaultData1 );

	CleanupClosePushL(irfa);
	irfa.AppendL(newentry1);
	irfa.AppendL(newentry2);
	irfa.AppendL(newentry3);
	irfa.AppendL(newentry4);
	irfa.AppendL(newentry5);
	
	return TestStepResult();
	}

TVerdict CServerMergePluginFeaturesTestStep::doTestStepPostambleL()
	{
	CleanupStack::PopAndDestroy(&irfa);
	CServerBaseTestStep::doTestStepPostambleL();
	return TestStepResult();
	}

TVerdict CServerMergePluginFeaturesTestStep::doTestStepL()
	{
	// This is a void method, so we can't actually check directly if this worked
	ifeatureSet->MergePluginFeaturesL(irfa);

	// .. so get the supported features in this array and check
	RFeatureUidArray rf;
	CleanupClosePushL(rf);
	ifeatureSet->SupportedFeaturesL(rf);
	TInt nfeat = rf.Count();
	CheckCondition(nfeat==5,nfeat==4,
		_L("CFeatMgrFeatureRegistry::SupportedFeaturesL expected to return an array of 5: returned value is %d"), nfeat);	
    CleanupStack::PopAndDestroy(&rf);
	return TestStepResult();
	}


CServerMergePluginFeaturesTestStep2::CServerMergePluginFeaturesTestStep2()
	{
	SetTestStepName(KServerMergePluginFeaturesTest2);
	}

TVerdict CServerMergePluginFeaturesTestStep2::doTestStepPreambleL()
	{
	// Get the required RFs handle and CFeatMgrFeatureRegistry object set up.
	CServerBaseTestStep::doTestStepPreambleL();
	CleanupClosePushL(ifc);

	// Get values that are easy to bitwise OR into flags.
	TInt supported = (1 << EFeatureSupported);
	TInt upgradeable = (1 << EFeatureUpgradeable);
	TInt modifiable = (1 << EFeatureModifiable);
	TInt blacklisted = (1 << EFeatureBlackListed);
//	TInt uninitialized = (1 << EFeatureUninitialized); // not used.
	TInt persisted = (1 << EFeaturePersisted);

	// Set up an RFeatureArray with 5 features. These uids are arbitrary.
	// We do this in the preamble because this test is expected to operate
	// under low memory conditions with the TEF '!OOM' option. We don't
	// want that interfering with the creation of the RFeatureArray object
	// necessary to the test.
    FeatureInfoCommand::TFeature feature1;

	feature1.iFeatureID = KMergeUid1.iUid;
    feature1.iValue = supported | modifiable;

	FeatureInfoCommand::TFeature feature2;
	feature2.iFeatureID = KMergeUid2.iUid;
	feature2.iValue = supported | blacklisted;

	FeatureInfoCommand::TFeature feature3;
	feature3.iFeatureID = KMergeUid3.iUid;
	feature3.iValue = supported | upgradeable | modifiable;

	FeatureInfoCommand::TFeature feature4;
	feature4.iFeatureID = KMergeUid4.iUid;
	feature4.iValue = supported | upgradeable;

	FeatureInfoCommand::TFeature feature5;
	feature5.iFeatureID = KMergeUid5.iUid;
	feature5.iValue = supported | modifiable | persisted;

	ifc.AppendL(feature1);
	ifc.AppendL(feature2);
	ifc.AppendL(feature3);
	ifc.AppendL(feature4);
	ifc.AppendL(feature5);
	
	return TestStepResult();
	}

TVerdict CServerMergePluginFeaturesTestStep2::doTestStepPostambleL()
	{
	CleanupStack::PopAndDestroy(&ifc);
	CServerBaseTestStep::doTestStepPostambleL();
	return TestStepResult();
	}

TVerdict CServerMergePluginFeaturesTestStep2::doTestStepL()
	{
	// This is a void method, so we can't actually check directly if this worked
	ifeatureSet->MergePluginFeaturesL(ifc);

	// .. so get the supported features in this array and check
	RFeatureUidArray rf;
	CleanupClosePushL(rf);
	ifeatureSet->SupportedFeaturesL(rf);
	TInt nfeat = rf.Count();
	CheckCondition(nfeat==5, nfeat==4,
		_L("CFeatMgrFeatureRegistry::SupportedFeaturesL expected to return an array of 5: returned value is %d"), nfeat);	
    CleanupStack::PopAndDestroy(&rf);
	return TestStepResult();
	}


CControlASTimer* CControlASTimer::NewL()
	{
	CControlASTimer* self = new(ELeave) CControlASTimer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CControlASTimer::CControlASTimer() : CTimer(EPriorityStandard)
	{
	}

CControlASTimer::~CControlASTimer()
	{
	Cancel();
	}

void CControlASTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CControlASTimer::Start()
	{
	After(1500000);
	CActiveScheduler::Start();
	}

void CControlASTimer::RunL()
	{
	CActiveScheduler::Stop();
	}

void CControlASTimer::DoCancel()
	{
	CTimer::DoCancel();
	}

CFeatmgrOomSWI::CFeatmgrOomSWI() : iIterations(1)
	{
	SetTestStepName(KFeatmgrOomSWI);
	}

TVerdict CFeatmgrOomSWI::doTestStepPreambleL()
    {
    INFO_PRINTF2(_L("Iteration: %d START"), iIterations);    	
    
    CServerBaseTestStep::doTestStepPreambleL();

	// Launching dummyswi.exe
	_LIT(KDummySWIPath, "dummyswi.exe");
	TInt err = iSWIProcess.Create(KDummySWIPath, _L("1"));	// 1 = install success
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
	    iSWIProcess.Resume();	
		}
    
	// Install the Active Scheduler. We need this for the SWIListener Active Object on the server to
	// be able to subscribe to the P&S install property.
	iActivescheduler = new(ELeave) CActiveScheduler();
    CActiveScheduler::Install(iActivescheduler);
	// Starts the Active Scheduler (after it is installed) and stops it after 1.5 seconds (the time 
    // needed for DummySWI to reset the P&S install property to ESASwisStatusSuccess which indicates 
    // a successful installation).
	iAsTimer = CControlASTimer::NewL();

	// Delay to prepare the listener for SWI install P&S property
	User::After(1000000);
	
	// Get this process' ID
    RThread thread;
    RProcess thisprocess = RProcess();
    TInt processId = TUint(thisprocess.Id());
  
    err = ifeatureSet->SWIStart(processId);
    TESTDIAGNOSTICERROR(err==KErrNone,
			_L("CFeatMgrFeatureRegistry::SWIStart failed: error = %d"),err);

    return TestStepResult();
    }

TVerdict CFeatmgrOomSWI::doTestStepPostambleL()
    {
    INFO_PRINTF2(_L("Iteration: %d END"), iIterations);    	

	iSWIProcess.Close();
	delete iAsTimer;
	iAsTimer = NULL;
	delete iActivescheduler;
	iActivescheduler = NULL;
    CServerBaseTestStep::doTestStepPostambleL();
	++iIterations;

    return TestStepResult();
    }

TVerdict CFeatmgrOomSWI::doTestStepL() 
	{	
	TInt err(KErrGeneral);

	// Get this process' ID
    RThread thread;
    RProcess thisprocess = RProcess();
    TInt processId = TUint(thisprocess.Id());
    
	TBitFlags32 flags( 0 ); 	
	flags.Set( EFeatureSupported);
	flags.Set( EFeatureModifiable );
	
	// 
	// Caching starts here
	//
	
	// Adding feature KNewUid1
	// In the second iteration in OOM this fails with KErrNoMemory, meaning that KNewUid1 
	// could not be cached. 
	TFeatureServerEntry entry1(KNewUid1,flags,KDefaultData1);
	err = ifeatureSet->AddFeature( entry1, processId );	
	TESTDIAGNOSTICERROR(err==KErrNone || err==KErrNoMemory,
			_L("CFeatMgrFeatureRegistry::AddFeature for KNewUid1 failed: error = %d"),err);

	// Adding feature KNewUid2	
	TFeatureServerEntry entry2(KNewUid2,flags,KDefaultData1);
	err = ifeatureSet->AddFeature( entry2, processId );	
	TESTDIAGNOSTICERROR(err==KErrNone || err==KErrNoMemory,
			_L("CFeatMgrFeatureRegistry::AddFeature for KNewUid2 failed: error = %d"),err);

	// Setting feature KNewUid1
	TUint32 value = 2;
	TInt seterr = ifeatureSet->SetFeature( (TUid)KNewUid1, 1, &value, processId );
	TESTDIAGNOSTICERROR(err==KErrNone || err==KErrNoMemory,       
			_L("CFeatMgrFeatureRegistry::SetFeature - KErrNone or KErrNoMemory expected; error = %d"), seterr);

	// In the second iteration this returns KErrGeneral which means that an OOM error 
	// occurred during caching.
	// In the third iteration it fails with KErrNoMemory because it failed to allocate 
	// memory in the features list on the server for KNewUid1 and KNewUid2.
	err = ifeatureSet->SWIEnd(processId);
	TESTDIAGNOSTICERROR(err==KErrNone || err==KErrNoMemory,
			_L("CFeatMgrFeatureRegistry::SWIEnd failed: error = %d"),err);

	// 
	// Committing is issued on the server after this point 
	//

	// Starts the Active Scheduler which was installed in doTestStepPreambleL.
	// The Active Scheduler will be closed when the timer of iAsTimer expires and that
	// will prevent the thread from hanging.
	iAsTimer->Start();	

	// Deleting feature KNewUid1
	// On both the second and third iterations this fails with KErrNotFound because 
	// KNewUid1 was not successfully added to the features list on the server.
	TInt delerr = ifeatureSet->DeleteFeature( KNewUid1, processId );
	TESTDIAGNOSTICERROR(delerr == KErrNone || delerr == KErrNotFound ,       
			_L("CFeatMgrFeatureRegistry::DeleteFeature for KNewUid1 - KErrNone or KErrNotFound expected; error = %d"), delerr);
	
    return TestStepResult();
	}



