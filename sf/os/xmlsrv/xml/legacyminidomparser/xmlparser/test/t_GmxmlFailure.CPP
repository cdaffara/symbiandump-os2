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

#include <e32std.h>
#include <e32test.h> 
#include "t_GmxmlFailure.h"
#include "t_XMLParserLOG.H"

LOCAL_D CActiveScheduler* scheduler;
LOCAL_D CImLog* log; 
_LIT(KTestTitle, "T_GmxmlFailure");
_LIT(KExistingFilePath, "c:\\GmxmlTest.txt");
_LIT(KNonExistingFilePath, "c:\\msg\\NotExist.txt");
LOCAL_D CGmxmlFailure* myTester;
// Global variables
RTest gTest(_L("t_GmxmlFailure Test Harness"));

CGmxmlFailure :: CGmxmlFailure() : CActive(EPriorityStandard)
	{
	}

/**
 * Second stage constructor:
 * Creates the XML parser and calls the ParseFile() with 
 * a non-exixtent file
 */

void CGmxmlFailure :: ConstructL()
	{
	iSession.Connect();
	iCMDXMLParser = CMDXMLParser::NewL(this);
	log->AppendComment(_L8("Performing tests on opening non-existing file"));
	LoadXmlFile(KNonExistingFilePath);
	log->AppendComment(_L8("Completed checking of opening non-existing file"));
		
	CActiveScheduler::Add(this);

	iState = ENonExistFileState;
	TRequestStatus *s = &iStatus;
	User::RequestComplete(s, KErrNone);
	SetActive();
	}

void CGmxmlFailure::LoadXmlFile(const TDesC &aFilePath)
	{	
	TInt err=iCMDXMLParser->ParseFile(iSession, aFilePath);	
	if(err!=KErrNone)
		{
		log->AppendError(_L8("Parse File returned an error "),err);
		}
	}
	
void CGmxmlFailure::LockXmlFile()
	{	
	RFile file;
	//Open the file before calling ParseFile function.
	file.Open(iSession, KExistingFilePath,EFileShareExclusive);
	//Call the ParseFile() on the locked file
	TInt err=iCMDXMLParser->ParseFile(iSession, KExistingFilePath);	
	if(err!=KErrNone)
		{
		log->AppendError(_L8("Parse File returned an error "),err);
		}
	file.Close();	
	}
	
CGmxmlFailure :: ~CGmxmlFailure()
/**
 * Destructor
 */
	{	
	iSession.Close();	
	delete iCMDXMLParser;
	}

CGmxmlFailure* CGmxmlFailure :: NewLC()
/** 
* Allocates a new CGmxmlFailure test object and pushes 
* it on clean up stack.
* @return New CGmxmlFailure 
*/
	{
	CGmxmlFailure* self = new (ELeave) CGmxmlFailure();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CGmxmlFailure :: ParseFileCompleteL()
/**
* Function from MMDXMLParserObserver class
*/
	{
	}

	
void CGmxmlFailure :: DoCancel()
/**
* Functions from CActive class
*/
	{
	}

void CGmxmlFailure :: RunL()	
	{
	switch(iState)
		{
	case ENonExistFileState:
		{				
		CActiveScheduler::Stop();
		iState = ELockFileState;
		TRequestStatus *s = &iStatus;
		User::RequestComplete(s, KErrNone);		
		SetActive();		
		}
		break;
	case ELockFileState:
		{
		CActiveScheduler::Stop();
		iState = EEndState;
		TRequestStatus *s = &iStatus;
		User::RequestComplete(s, KErrNone);
		SetActive();				
		}
		break;				
	case EEndState:
		{
		CActiveScheduler::Stop();
		}
		break;
	default:
		break;
		}

	}

LOCAL_C void InitL()
	{
	TBuf<80> logFile(KLogFileName);

#if defined(__WINS__) && defined(__WINSCW__)
	logFile.Append(_L(".WINSCW"));
#elif(__WINS__)
	logFile.Append(_L(".WINS"));
#else
	logFile.Append(_L(".MARM"));
#endif

#if defined(_DEBUG)
	logFile.Append(_L(".DEB"));
#else
	logFile.Append(_L(".REL"));
#endif
	logFile.Append(_L(".LOG"));

	gTest.Printf(_L("\nLog File "));
	gTest.Printf(logFile);
	gTest.Printf(_L("\n"));
	log = CImLog::NewL(logFile, EAppend);
	CleanupStack::PushL(log);
	log->AppendComment(_L8("Test Results"));
	log->AppendComment(_L8("T_GMXMLFAILURE memory leak test in XMLParser"));
	log->AppendComment(_L8("Test 1 Starting"));
	}

/**
 * setup test environment, create CGmxmlFailure object and run test.
 * 
 * This is an memory leak test. 
 * Test case: 
 * Passes the file name and session to CMDXMLParser::ParseFile() 
 * as arguments and run the memory leak test.
 * Running Instructions:
 * 1. Build this test harness:	cd \GMXML\XMLParser\GROUP
 *								bldmake bldfiles
 *								abld test build wins udeb t_GmxmlFailure
 * 2. Run t_GmxmlFailure.exe:	cd \epoc32\release\wins\udeb
 *								t_GmxmlFailure -dtextshell -- 
 *
 *  Output files produced by running test harness:
 *	        <DRIVE>:\msglogs\T_GMXMLFAILURE.<PLATFORM>.<VARIANT>.txt
 */
	
LOCAL_C void doMainL()
	{
	gTest.Start(KTestTitle);
	InitL();
	scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );
	__UHEAP_MARK;	
	myTester = CGmxmlFailure::NewLC();		
	CActiveScheduler::Start();	
	log->AppendComment(_L8("Completed tests. No panic"));
	log->AppendComment(_L8("Test PASSED"));
	log->AppendComment(_L8("Test 1 OK"));
	
	log->AppendComment(_L8("Test 2 Starting"));	
	log->AppendComment(_L8("Performing tests on opening existing file"));
	myTester->LoadXmlFile(KExistingFilePath);
	log->AppendComment(_L8("Completed checking of opening existing file"));	
	CActiveScheduler::Start();		
	log->AppendComment(_L8("Completed tests. No panic"));
	log->AppendComment(_L8("Test PASSED"));	
	log->AppendComment(_L8("Test 2 OK"));
	
	log->AppendComment(_L8("Test 3 Starting"));
	log->AppendComment(_L8("Performing tests on locked file"));
	myTester->LockXmlFile();
	log->AppendComment(_L8("Completed checking of opening a locked file"));		
	CActiveScheduler::Start();
	log->AppendComment(_L8("Completed tests. No panic"));
	log->AppendComment(_L8("Test PASSED"));	
	log->AppendComment(_L8("Test 3 OK"));
	
	CleanupStack::PopAndDestroy(myTester);		// myTester
	__UHEAP_MARKEND;	
	
	log->AppendComment(_L8("No memory leak in XMLParser"));	
	log->AppendComment(_L8("T_GMXMLFAILURE Tests Complete"));
	log->AppendComment(_L8("Tests Completed"));

	CleanupStack::PopAndDestroy(2);		// scheduler, log	
	gTest.End();
	gTest.Close();
	}

/** entry point for test executable */
GLDEF_C TInt E32Main()
	{
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	gTest(ret==KErrNone);
	delete theCleanup;	

	return(KErrNone);
	}
