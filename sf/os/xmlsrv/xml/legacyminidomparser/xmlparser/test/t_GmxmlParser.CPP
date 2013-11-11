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

#include <e32test.h>
#include <e32std.h>
#include <f32file.h>
#include <gmxmlparser.h>

#include "GmxmlTestsetup.h"


_LIT(KGMXMLParserTest,"T_GMXMLParser");
class CTestGMXMLParser;

RTest test(KGMXMLParserTest);
LOCAL_D CTestGMXMLParser* testParser;
LOCAL_D CActiveScheduler* scheduler;

//------------------------------------------------------------------------------

class CStreamErrorDataSupplier : public CBase, public MMDXMLParserDataProvider
	{
public:
	static CStreamErrorDataSupplier * NewL();
	~CStreamErrorDataSupplier () {}

	// From MMDXMLParserDataProvided
	void GetData(TPtrC8& aPtr, TRequestStatus &aStatus);
	void Disconnect() {}
	};

//------------------------------------------------------------------------------

CStreamErrorDataSupplier* CStreamErrorDataSupplier::NewL()
	{
	CStreamErrorDataSupplier* self = new (ELeave) CStreamErrorDataSupplier();
	return self;
	}

//------------------------------------------------------------------------------
// From MMDXMLParserDataProvided
void CStreamErrorDataSupplier::GetData(TPtrC8& /*aPtr*/, TRequestStatus &aStatus)
	{
	TRequestStatus *s = &aStatus;
	User::RequestComplete(s, KDataStreamError);
	return;
	}

//------------------------------------------------------------------------------

class CTestGMXMLParser : public CActive, public MMDXMLParserObserver
	{
public:
	static CTestGMXMLParser* NewLC();
	~CTestGMXMLParser();
	void ConstructL();
	void RunTestL();

public: // from CActive
	void DoCancel();
	void RunL();

public: // from MMDXMLParserObserver
	void ParseFileCompleteL();

private:
	CTestGMXMLParser();

public:
	TBool iParseFileCompleted; // Whether the ParseFileCompleted was called.
	TBool iParserCalled;       // Whether the GMXMLParser::ParseSource was called

private:
	CMDXMLParser* iParser;
	CStreamErrorDataSupplier* iDataSupplier;
	CTestTimer* iTimer;

	static const TTimeIntervalMicroSeconds32 TestParserTimeout;
	};

//------------------------------------------------------------------------------

const TTimeIntervalMicroSeconds32 CTestGMXMLParser::TestParserTimeout = 10000000; // 10s

//------------------------------------------------------------------------------

CTestGMXMLParser* CTestGMXMLParser::NewLC()
	{
	CTestGMXMLParser* self = new (ELeave) CTestGMXMLParser();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

//------------------------------------------------------------------------------

CTestGMXMLParser::~CTestGMXMLParser()
	{
	delete iParser;
	delete iDataSupplier;
	delete iTimer;
	}

//------------------------------------------------------------------------------

CTestGMXMLParser::CTestGMXMLParser() : CActive(EPriorityStandard), iParseFileCompleted(EFalse), iParserCalled(EFalse)
	{
	}

//------------------------------------------------------------------------------

void CTestGMXMLParser::ConstructL()
	{
	iParser = CMDXMLParser::NewL(this);
	iTimer = CTestTimer::NewL();

	CActiveScheduler::Add(this);

	TRequestStatus *s = &iStatus;
	User::RequestComplete(s, KErrNone);
	SetActive();
	}

//------------------------------------------------------------------------------

void CTestGMXMLParser::RunL()
	{
	RunTestL();
	}

//------------------------------------------------------------------------------

void CTestGMXMLParser::DoCancel()
	{
	}

//------------------------------------------------------------------------------

void CTestGMXMLParser::RunTestL()
	{
	if (!iParserCalled)
		{
		delete iDataSupplier;
		iDataSupplier = NULL;
		iDataSupplier = CStreamErrorDataSupplier::NewL();
		iParser->ParseSource(iDataSupplier);

		iStatus = KRequestPending;
		SetActive();
		iParserCalled = ETrue;
		// Create a timer to stop the active scheduler if ParseFileComplete not called.
		iTimer->AfterReq(CTestGMXMLParser::TestParserTimeout, iStatus);
		}
	else
		CActiveScheduler::Stop();
	}

//------------------------------------------------------------------------------

void CTestGMXMLParser::ParseFileCompleteL()
	{
	// Success.
	iParseFileCompleted = ETrue;

	TRequestStatus *s = &iStatus;
	User::RequestComplete(s, KErrNone);
	}

//------------------------------------------------------------------------------

LOCAL_C void doTestsL()
	{
	scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );
	testParser = CTestGMXMLParser::NewLC();

	theUtils->WriteComment(_L("\nPerforming Tests\n"));
	theUtils->Start(_L("Testing GMXMLParser receiving KDataStreamError from MMDXMLParserDataProvider::GetData()"));
	CActiveScheduler::Start();
	theUtils->Complete();

	// Succeed only if the GMXMLParser::ParserSource was called and it resulted
	// in ParseFileCompleted being called.
	if (!testParser->iParserCalled || !testParser->iParseFileCompleted)
		{
		theUtils->WriteComment(_L("\nParseFileCompleted was not called within the timeout (10s)!"));
		User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(2);  //testParser, ischeduler
	}

//------------------------------------------------------------------------------

