/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* T_NearestFontCapability.cpp
*
*/


/**
 @file
 @test
 @internalComponent Internal Symbian test code
*/

#include "T_GetNearestFontCapability.h"
#include <hal.h>

_LIT( KFontAvailable, "DejaVu Sans Condensed" );

/**
Class test cases for Font Capability testing and checkig panic codes
*/
class CTGetNearestFontCapability : public CTGraphicsBase
	{
public:
	CTGetNearestFontCapability(CTestStep* aStep);	
	virtual ~CTGetNearestFontCapability();

protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL();

private:
	void TestSystemDefaultFont();

private:
	CFbsTypefaceStore* iTfs;	
	};

//
// CTGetNearestFontCapability
//
CTGetNearestFontCapability::CTGetNearestFontCapability(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	
	}

void CTGetNearestFontCapability::ConstructL()
	{
	iTfs = CFbsTypefaceStore::NewL(NULL);	
	}	
	
 CTGetNearestFontCapability::~CTGetNearestFontCapability()
 	{
 	delete iTfs; 	
 	}
	
void CTGetNearestFontCapability::RunTestCaseL( TInt aCurTestCase )
	{	
	((CTGetNearestFontCapabilityStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch ( aCurTestCase )
		{		
	case 1:
		((CTGetNearestFontCapabilityStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0007"));
		INFO_PRINTF1(_L("Test System default font (capability check) Started "));
		TRAPD(err,TestSystemDefaultFont());
		if(err != KErrNone)
			TEST(err == KErrPermissionDenied);
		INFO_PRINTF1(_L("Test System default font (capability check) Finished"));
	break;
	
	case 2:
        	((CTGetNearestFontCapabilityStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTGetNearestFontCapabilityStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();		
		break;
		}
	((CTGetNearestFontCapabilityStep*)iStep)->RecordTestResultL();
	}

/**
@SYMTestCaseID GRAPHICS-FNTSTORE-0007
@SYMTestPriority Critical
@SYMREQ REQ6158

@SYMTestCaseDesc 
Set the default system font with different capabilities

@SYMTestActions 
1 - Set the system default typeface with normal capability
2 - Set the system default typeface with WriteDeviceData capability
3 - Set the system default typeface with no capabilities

@SYMTestExpectedResults 
Leaves with error code KErrPermissionDenied when a client does not have the WriteDeviceData capability.
*/
void CTGetNearestFontCapability::TestSystemDefaultFont()
	{			
	// Now try to set the system default font
	iTfs->SetSystemDefaultTypefaceNameL(KFontAvailable);
	
	// Set back to empty descriptor
	iTfs->SetSystemDefaultTypefaceNameL(KNullDesC);
	}

//--------------
__CONSTRUCT_STEP__(GetNearestFontCapability)
