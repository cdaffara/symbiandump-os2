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
// Name        : tliblogger.cpp
// 
//


#include <stdio.h>
#include "tliblogger.h"
#include "liblogger.h"





extern "C"
void ExampleLC(void);

CTestLibLogger::~CTestLibLogger() 
	{ 
	}  

CTestLibLogger::CTestLibLogger(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestLibLogger::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestLibLogger::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestLibLogger::doTestStepL()
	{
	int err;
	if(TestStepName() == KLibloggerTest)
   			{
   	   		INFO_PRINTF1(_L("LibloggerTest():"));
   	   		err = LibloggerTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	return TestStepResult(); 

	}
	
	
	
 // Panic the server
GLDEF_C void PanicServer(TInt aPanic)
{
	_LIT(KTxtServerPanic,"App1");
	User::Panic(KTxtServerPanic,aPanic);
}

TInt counter = 0;
TInt TheadEntryL(TAny *args)
    {
    (void)args;
    CTrapCleanup *cleanup = CTrapCleanup::New();
    LIB_TRACE(ELibTraceTypeInfo, "Testing macro. %s, %d, %s", "Nokia", 01, "xsdfs");
    LIB_TRACE_DUMP_LEN(ELibTraceTypeMajor, "dumping test", " aslfk aslfasfl asflksdfjaslkdfj aslfjasdf", 20);
	delete cleanup;
	counter = 1;
    return 0;
    }

void CreateThreadL()
    {
    RThread thread;
    _LIT(KName, "LoggerTestThrad");
    thread.Create(KName, TheadEntryL, 2000, NULL, NULL);
    thread.Resume();
    }

void ExampleLC(void)
    {
    LIB_TRACE(ELibTraceTypeInfo, "VARIABLE - %s, %d, %s, %u, %s, %.2x, %s, %s, %d, %d", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven", "Eight", 9, 10);
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE");
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_1 - %d", 1);
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_2 - %s, %d", "Nokia", 01);
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_3 - %s, %d, %s", "Nokia", 01, "Third macro");
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_4 - %s, %d, %s, %u", "Nokia", 01, "Third macro", 4);
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_5 - %s, %d, %s, %u, %s", "Nokia", 01, "Third macro", 4, "Five");
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_6 - %s, %d, %s, %u, %s, %.2x", "Nokia", 01, "Third macro", 4, "Five", 12);
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_7 - %s, %d, %s, %u, %s, %.2x, %s", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven");
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_8 - %s, %d, %s, %u, %s, %.2x, %s, %s", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven", "Eight");
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_9 - %s, %d, %s, %u, %s, %.2x, %s, %s, %d", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven", "Eight", 9);
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_10 - %s, %d, %s, %u, %s, %.2x, %s, %s, %d, %d", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven", "Eight", 9, 10);

    LIB_TRACE_DUMP_LEN(ELibTraceTypeInfo, "Testing LIB_TRACE_HEX_DUMP", "LIB_TRACE_PRINTF", 5);


    LIB_TRACE_DUMP(ELibTraceTypeCritical, "Testing LIB_TRACE_DUMP", "LIB_TRACE_DUMP");
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_1", "LIB_TRACE_DUMP_1 - %d", 1);
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_2", "LIB_TRACE_DUMP_2 - %s, %d", "Nokia", 01);
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_3", "LIB_TRACE_DUMP_3 - %s, %d, %s", "Nokia", 01, "Third macro");
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_4", "LIB_TRACE_DUMP_4 - %s, %d, %s, %u", "Nokia", 01, "Third macro", 4);
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_5", "LIB_TRACE_DUMP_5 - %s, %d, %s, %u, %s", "Nokia", 01, "Third macro", 4, "Five");
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_6", "LIB_TRACE_DUMP_6 - %s, %d, %s, %u, %s, %.2x", "Nokia", 01, "Third macro", 4, "Five", 12);
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_7", "LIB_TRACE_DUMP_7 - %s, %d, %s, %u, %s, %.2x, %s", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven");
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_8", "LIB_TRACE_DUMP_8 - %s, %d, %s, %u, %s, %.2x, %s, %s", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven", "Eight");
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_9", "LIB_TRACE_DUMP_9 - %s, %d, %s, %u, %s, %.2x, %s, %s, %d", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven", "Eight", 9);
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_10", "LIB_TRACE_DUMP_10 - %s, %d, %s, %u, %s, %.2x, %s, %s, %d, %d", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven", "Eight", 9, 10);
    }
    
    
    
void doExampleL()
    {
    ExampleLC();
    LIB_TRACE(ELibTraceTypeInfo, "VARIABLE - %s, %d, %s, %u, %s, %.2x, %s, %s, %d, %d", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven", "Eight", 9, 10);
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE");
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_1 - %d", 1);
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_2 - %s, %d", "Nokia", 01);
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_3 - %s, %d, %s", "Nokia", 01, "Third macro");
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_4 - %s, %d, %s, %u", "Nokia", 01, "Third macro", 4);
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_5 - %s, %d, %s, %u, %s", "Nokia", 01, "Third macro", 4, "Five");
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_6 - %s, %d, %s, %u, %s, %.2x", "Nokia", 01, "Third macro", 4, "Five", 12);
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_7 - %s, %d, %s, %u, %s, %.2x, %s", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven");
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_8 - %s, %d, %s, %u, %s, %.2x, %s, %s", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven", "Eight");
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_9 - %s, %d, %s, %u, %s, %.2x, %s, %s, %d", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven", "Eight", 9);
    LIB_TRACE(ELibTraceTypeInfo, "LIB_TRACE_10 - %s, %d, %s, %u, %s, %.2x, %s, %s, %d, %d", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven", "Eight", 9, 10);

    LIB_TRACE_DUMP_LEN(ELibTraceTypeInfo, "Testing LIB_TRACE_HEX_DUMP", "LIB_TRACE_PRINTF", 5);


    LIB_TRACE_DUMP(ELibTraceTypeCritical, "Testing LIB_TRACE_DUMP", "LIB_TRACE_DUMP");
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_1", "LIB_TRACE_DUMP_1 - %d", 1);
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_2", "LIB_TRACE_DUMP_2 - %s, %d", "Nokia", 01);
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_3", "LIB_TRACE_DUMP_3 - %s, %d, %s", "Nokia", 01, "Third macro");
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_4", "LIB_TRACE_DUMP_4 - %s, %d, %s, %u", "Nokia", 01, "Third macro", 4);
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_5", "LIB_TRACE_DUMP_5 - %s, %d, %s, %u, %s", "Nokia", 01, "Third macro", 4, "Five");
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_6", "LIB_TRACE_DUMP_6 - %s, %d, %s, %u, %s, %.2x", "Nokia", 01, "Third macro", 4, "Five", 12);
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_7", "LIB_TRACE_DUMP_7 - %s, %d, %s, %u, %s, %.2x, %s", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven");
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_8", "LIB_TRACE_DUMP_8 - %s, %d, %s, %u, %s, %.2x, %s, %s", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven", "Eight");
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_9", "LIB_TRACE_DUMP_9 - %s, %d, %s, %u, %s, %.2x, %s, %s, %d", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven", "Eight", 9);
    LIB_TRACE_DUMP(ELibTraceTypeInfo, "Testing LIB_TRACE_DUMP_10", "LIB_TRACE_DUMP_10 - %s, %d, %s, %u, %s, %.2x, %s, %s, %d, %d", "Nokia", 01, "Third macro", 4, "Five", 12, "Seven", "Eight", 9, 10);

	CreateThreadL();
	while(!counter)
	    User::After(50000);
    }

TInt CTestLibLogger::LibloggerTest()
	{
	INFO_PRINTF2(_L("Calling Function - %d\n"), 2);
	TRAPD(error,doExampleL()); // perform example function
	if (error)
		return KErrGeneral;
	else
		return KErrNone;
	}





