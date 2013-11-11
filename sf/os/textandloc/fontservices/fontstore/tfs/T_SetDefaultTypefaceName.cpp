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
*
*/


/**
 @file
 @test
 @internalComponent Internal Symbian test code
*/

#include "T_SetDefaultTypefaceName.h"
#include "fbsmessage.h"
#include <hal.h>

// String entry into ini file for reading panic test sequence.
_LIT(KCaseNumber, "CaseNumber");

/**
Class test cases for Font Capability testing and checkig panic codes
*/
class CTSetDefaultTypefaceName : public CTGraphicsBase
	{
public:
	CTSetDefaultTypefaceName(CTestStep* aStep);	
	virtual ~CTSetDefaultTypefaceName();

protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL();

private:	
	void TestDirectServerAccessLongDescriptor();
	void TestDirectServerAccessBadDescriptor();

private:
	CFbsTypefaceStore* iTfs;	
	};

/**
Defines a FBS client class to talk to the FBS server directly.
*/
class RFbsTestClient : protected RSessionBase
	{
	public:
		TInt Connect();
		void Close();
		TInt SendReceive(TInt aFunction, const TIpcArgs &aArgs) const;
	};
	
//
// RFbsTestClient
//
TInt RFbsTestClient::Connect()
	{
	_LIT(KFBSERVGlobalThreadName,"!Fontbitmapserver");	
	return CreateSession(KFBSERVGlobalThreadName, TVersion(0,0,0), -1);
	}

void RFbsTestClient::Close()
	{
	RSessionBase::Close();
	}

TInt RFbsTestClient::SendReceive(TInt aFunction, const TIpcArgs &aArgs) const
	{
	return RSessionBase::SendReceive(aFunction, aArgs);
	}

//
// CTSetDefaultTypefaceName
//
CTSetDefaultTypefaceName::CTSetDefaultTypefaceName(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	
	}

void CTSetDefaultTypefaceName::ConstructL()
	{
	iTfs = CFbsTypefaceStore::NewL(NULL);	
	}	
	
 CTSetDefaultTypefaceName::~CTSetDefaultTypefaceName()
 	{
 	delete iTfs; 	
 	}
	
void CTSetDefaultTypefaceName::RunTestCaseL( TInt aCurTestCase )
	{
	if (!iStep->GetIntFromConfig(iStep->ConfigSection(), KCaseNumber, aCurTestCase))
		{
		ERR_PRINTF1(_L("Failed to read from config file."));
		return;
		}

	((CTSetDefaultTypefaceNameStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch ( aCurTestCase )
		{				
	case 1:
		((CTSetDefaultTypefaceNameStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0008"));
		INFO_PRINTF1(_L("Test System default font direct server access Started "));
		TestDirectServerAccessLongDescriptor();
		INFO_PRINTF1(_L("Test System default font direct server access Finished"));
		break;
	case 2:
		((CTSetDefaultTypefaceNameStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0052"));
		INFO_PRINTF1(_L("Test System default font direct server access Started "));
		TestDirectServerAccessBadDescriptor();
		INFO_PRINTF1(_L("Test System default font direct server access Finished"));
		break;
	case 3:
		((CTSetDefaultTypefaceNameStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTSetDefaultTypefaceNameStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();		
		break;
		}
	((CTSetDefaultTypefaceNameStep*)iStep)->RecordTestResultL();
	}

/**
@SYMTestCaseID GRAPHICS-FNTSTORE-0008
@SYMTestPriority Critical
@SYMREQ REQ6158

@SYMTestCaseDesc 
Access FBS Server directly and set the default typeface name.

@SYMTestActions 
Set default system typeface name via FBS server to a valid value.
Attempt to set default system typeface name to a larger descriptor than KMaxTypefaceNameLength.

@SYMTestExpectedResults
If default system typeface is larger then KMaxTypefaceNameLength then panic client with FBSSERV -40.
*/
void CTSetDefaultTypefaceName::TestDirectServerAccessLongDescriptor()
	{
	// Create test FBS client to connect to FBS Server
	RFbsTestClient fbsClient;
	TInt ret = fbsClient.Connect();
	TEST(ret == KErrNone);

	// Set typeface name 
	const TInt KMaxTypefaceNameSize = 255;
	TBuf <KMaxTypefaceNameSize> fontTypefaceName;
	fontTypefaceName = _L("DejaVu Sans Condensed");
	TIpcArgs args(&fontTypefaceName);	
	
	ret=fbsClient.SendReceive(EFbsSetSystemDefaultTypefaceName, args);	
	TEST(ret == KErrNone);
	
	// Send a larger descriptor than KMaxTypefaceNameLength. Should panic
	fontTypefaceName.Zero();
	fontTypefaceName.Fill('a', KMaxTypefaceNameLength+1);
	ret=fbsClient.SendReceive(EFbsSetSystemDefaultTypefaceName, args);	
	TEST(ret == KErrTooBig);	
		
	fbsClient.Close();
	}

/**
@SYMTestCaseID GRAPHICS-FNTSTORE-0052
@SYMTestPriority Critical
@SYMREQ REQ6158

@SYMTestCaseDesc 
Access FBS Server directly and set the default typeface name.

@SYMTestActions 
Attempt to set default system typeface name to a 'bad descriptor'.

@SYMTestExpectedResults 
Panic client with FBERV -38.
*/
void CTSetDefaultTypefaceName::TestDirectServerAccessBadDescriptor()
    {
    // Create test FBS client to connect to FBS Server
    RFbsTestClient fbsClient;
    TInt ret = fbsClient.Connect();
    TEST(ret == KErrNone);

    // Send a 'bad descriptor' to the server. Should panic the client.
    ret=fbsClient.SendReceive(EFbsSetSystemDefaultTypefaceName, TIpcArgs(NULL));
    TEST(ret == KErrBadDescriptor);
    fbsClient.Close();
    }

//--------------
__CONSTRUCT_STEP__(SetDefaultTypefaceName)
