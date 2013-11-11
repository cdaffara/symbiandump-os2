// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test the CDiscoverer class using the test bed.
// 
//

#include "DiscovererUnitTest.h"
#include "ComponentTester.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Test the CDiscoverer class.
 */
class CDiscoverer_ComponentTest : public CComponentTester
	{
public:
	/**
		@fn				NewLC(CDataLogger& aDataLogger,
							 MComponentTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave			KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this component test.
		@return			CDiscoverer_CreateAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CDiscoverer_ComponentTest is fully constructed.
	*/
	static CDiscoverer_ComponentTest* NewLC(CDataLogger& aDataLogger, 
							MComponentTestObserver& aObserver);

private:
	/**
		@fn				CDiscoverer_ComponentTest(CDataLogger& aDataLogger,
											MComponentTestObserver& aObserver)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aDataLogger The logging object.
		@param			aObserver The observer of this component test.
		@pre 			None.
		@post			CDiscoverer_ComponentTest is fully constructed.
	*/
	inline CDiscoverer_ComponentTest(CDataLogger& aDataLogger,
												MComponentTestObserver& aObserver);
	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
						to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory.
		@since			7.0
		@return			None 
		@pre 			CDiscoverer_ComponentTest is fully constructed.
		@post			CDiscoverer_ComponentTest is fully initialised.
	*/
	inline void ConstructL();

	};	// CDiscoverer_ComponentTest

// ______________________________________________________________________________
//
inline CDiscoverer_ComponentTest* CDiscoverer_ComponentTest::NewLC(CDataLogger& aDataLogger, MComponentTestObserver& aObserver)
	{
	CDiscoverer_ComponentTest* self = new (ELeave) CDiscoverer_ComponentTest(aDataLogger, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

inline CDiscoverer_ComponentTest::CDiscoverer_ComponentTest(CDataLogger& aDataLogger,
									MComponentTestObserver& aObserver)
: CComponentTester(aDataLogger, aObserver)
	{
	// Do nothing here.
	}

inline void CDiscoverer_ComponentTest::ConstructL()
	{
	ComponentTesterConstructL();
	AddUnitTestL(CDiscoverer_CreateAndDestroy_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CDiscoverer_UnmountDrive_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CDiscoverer_MountDrive_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CDiscoverer_IsValidEntryL_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CDiscoverer_ProcessEntryL_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CDiscoverer_ScanDirectory_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CDiscoverer_ScanDirectoryCancel_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CDiscoverer_ScanDirectoryIncrement_UnitTest::NewL(iDataLogger, *this));

	AddUnitTestL(CDiscoverer_CompleteNotificationProcessing_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CDiscoverer_Notification_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CDiscoverer_SuspendResume_UnitTest::NewL(iDataLogger, *this));
	}

// ______________________________________________________________________________
//
EXPORT_C CComponentTester* NewComponentTestLC(CDataLogger& aDataLogger,
											  MComponentTestObserver&	aComponentTestObserver)
	{
	return CDiscoverer_ComponentTest::NewLC(aDataLogger, aComponentTestObserver);
	}

// ___________________________________________________________________________
//
// This section of the module simply includes the exported test harness template which 
// makes this a "whole" CPP file with a E32Main entry point below. The test MMP 
// project file can then produce a EXE for the test project instead of a DLL.

#include <ecom/test_bed/testharnesstemplate.h>

GLDEF_C TInt E32Main()
	{
	return E32Main_TestHarness(NewComponentTestLC);
	}

