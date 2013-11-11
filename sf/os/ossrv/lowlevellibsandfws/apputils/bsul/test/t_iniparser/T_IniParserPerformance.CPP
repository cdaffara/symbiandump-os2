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
//

#include <e32std.h>
#include <e32base.h>
#include <e32test.h>
#include <f32file.h>
#include <bsul/inifile.h>			// CIniDocument8, CIniDocument16
#include "LegacyParser.h"

RTest test(_L("Ini File Parser Performance Test"));

RFs TheRFs;

using namespace BSUL;

//comparing light weight and heavy weight on a small ini file
static void PerformanceTest1L()
	{
	test.Printf(_L("\nSmall Ini file comparison \n"));
	TTime startTime, stopTime;

	TPtrC8 value;
	startTime.UniversalTime();
for (TInt i=0;i<1000;i++)
	{
	CIniFile8* ini=CIniFile8::NewL(TheRFs,_L("z:\\resource\\testconfig8.ini"));
	test(ini->FindVar(_L8("test_section"),_L8("key1"),value)==KErrNone);
	test(value.Compare(_L8("value1"))==0);
	test(ini->FindVar(_L8("test_section"),_L8("key2"),value)==KErrNone);
	test(value.Compare(_L8("value2"))==0);
	test(ini->FindVar(_L8("test_section"),_L8("key3"),value)==KErrNone);
	test(value.Compare(_L8("value3"))==0);
	test(ini->FindVar(_L8("test_section"),_L8("key4"),value)==KErrNone);
	test(value.Compare(_L8("value4"))==0);
	test(ini->FindVar(_L8("test_section"),_L8("key5"),value)==KErrNone);
	test(value.Compare(_L8("value value value"))==0);
	test(ini->FindVar(_L8("SERVERS"),_L8("SWTRACER"),value)==KErrNone);
	test(value.Compare(_L8(""))==0);
	test(ini->FindVar(_L8("1"),_L8("timestamps"),value)==KErrNone);
	test(value.Compare(_L8("0"))==0);
	test(ini->FindVar(_L8("1"),_L8("setting"),value)==KErrNone);
	test(value.Compare(_L8("value"))==0);
	delete ini;
	}
	stopTime.UniversalTime();
	TTimeIntervalMicroSeconds timeTaken = stopTime.MicroSecondsFrom(startTime);
	test.Printf(_L("Time taken for Light= %d microseconds\n"), timeTaken.Int64() );

	//heavy weight testing
	startTime.UniversalTime();
for (TInt j=0;j<1000;j++)
	{
	CIniDocument8* dom=CIniDocument8::NewL(TheRFs,_L("z:\\resource\\testconfig8.ini"));
	test(dom->GetKeyValue(_L8("test_section"),_L8("key1"),value)==KErrNone);
	test(value.Compare(_L8("value1"))==0);
	test(dom->GetKeyValue(_L8("test_section"),_L8("key2"),value)==KErrNone);
	test(value.Compare(_L8("value2"))==0);
	test(dom->GetKeyValue(_L8("test_section"),_L8("key3"),value)==KErrNone);
	test(value.Compare(_L8("value3"))==0);
	test(dom->GetKeyValue(_L8("test_section"),_L8("key4"),value)==KErrNone);
	test(value.Compare(_L8("value4"))==0);
	test(dom->GetKeyValue(_L8("test_section"),_L8("key5"),value)==KErrNone);
	test(value.Compare(_L8("value value value"))==0);
	test(dom->GetKeyValue(_L8("SERVERS"),_L8("SWTRACER"),value)==KErrNone);
	test(value.Compare(_L8(""))==0);
	test(dom->GetKeyValue(_L8("1"),_L8("timestamps"),value)==KErrNone);
	test(value.Compare(_L8("0"))==0);
	test(dom->GetKeyValue(_L8("1"),_L8("setting"),value)==KErrNone);
	test(value.Compare(_L8("value"))==0);
	delete dom;
	}

	stopTime.UniversalTime();
	timeTaken = stopTime.MicroSecondsFrom(startTime);
	test.Printf(_L("Time taken for Heavy= %d microseconds\n"), timeTaken.Int64() );

	}

static void PerformanceTest2L()
{
	test.Printf(_L("\nHuge Ini file(684Kb) performance test\n"));
	//This test will try finding values within different section in the ini file, beginning
	//middle and end of the file.
	//Light weight testing
	TTime startTime, stopTime;

	TPtrC16 value;
	startTime.UniversalTime();
for (TInt i=0;i<1;i++)
	{
	CIniFile16* ini=CIniFile16::NewL(TheRFs,_L("z:\\resource\\big16.ini"));
	test(ini->FindVar(_L("Secure_DB_Backup"),_L("source"),value)==KErrNone);
	test(value.Compare(_L("C:\\private\\100012a5\\DBS_100065FF_performance.cdb"))==0);
	test(ini->FindVar(_L("20701_AddEntries"),_L("firstname430"),value)==KErrNone);
	test(value.Compare(_L("Husham"))==0);
	test(ini->FindVar(_L("20701_AddEntries"),_L("homemobile514"),value)==KErrNone);
	test(value.Compare(_L("07763 222730"))==0);
	test(ini->FindVar(_L("20701_AddEntries"),_L("dummyI830"),value)==KErrNone);
	test(value.Compare(_L(""))==0);
	test(ini->FindVar(_L("20701_AddEntries"),_L("workemail1066"),value)==KErrNone);
	test(value.Compare(_L("ZoranS@BenQ.com"))==0);
	delete ini;
	}
	stopTime.UniversalTime();
	TTimeIntervalMicroSeconds timeTaken = stopTime.MicroSecondsFrom(startTime);
	test.Printf(_L("Time taken for Light= %d microseconds\n"), timeTaken.Int64() );

	//heavy weight testing
	startTime.UniversalTime();
for (TInt j=0;j<1;j++)
	{
	CIniDocument16* dom=CIniDocument16::NewL(TheRFs,_L("z:\\resource\\big16.ini"));
	test(dom->GetKeyValue(_L("Secure_DB_Backup"),_L("source"),value)==KErrNone);
	test(value.Compare(_L("C:\\private\\100012a5\\DBS_100065FF_performance.cdb"))==0);
	test(dom->GetKeyValue(_L("20701_AddEntries"),_L("firstname430"),value)==KErrNone);
	test(value.Compare(_L("Husham"))==0);
	test(dom->GetKeyValue(_L("20701_AddEntries"),_L("homemobile514"),value)==KErrNone);
	test(value.Compare(_L("07763 222730"))==0);
	test(dom->GetKeyValue(_L("20701_AddEntries"),_L("dummyI830"),value)==KErrNone);
	test(value.Compare(_L(""))==0);
	test(dom->GetKeyValue(_L("20701_AddEntries"),_L("workemail1066"),value)==KErrNone);
	test(value.Compare(_L("ZoranS@BenQ.com"))==0);
	delete dom;
	}
	stopTime.UniversalTime();
	timeTaken = stopTime.MicroSecondsFrom(startTime);
	test.Printf(_L("Time taken for Heavy= %d microseconds\n"), timeTaken.Int64() );

}

static void PerformanceTest3L()
	{
	test.Printf(_L("\n8 byte performance with existing parser comparison \n"));
	TTime startTime, stopTime;
//legacy
	startTime.UniversalTime();
	TPtrC value;
for (TInt i=0;i<1000;i++)
	{
	CIniFile* ini=CIniFile::NewL(_L("z:\\resource\\legacy8.ini"));
	test(ini->FindVar(_L("test_section"),_L("key1"),value));
	test(value.Compare(_L("value1"))==0);
	test(ini->FindVar(_L("test_section"),_L("key2"),value));
	test(value.Compare(_L("value2"))==0);
	test(ini->FindVar(_L("test_section"),_L("key3"),value));
	test(value.Compare(_L("value3"))==0);
	test(ini->FindVar(_L("test_section"),_L("key4"),value));
	test(value.Compare(_L("value4"))==0);
	test(ini->FindVar(_L("test_section"),_L("key5"),value));
	test(value.Compare(_L("value value value"))==0);
	test(ini->FindVar(_L("1"),_L("timestamps"),value));
	test(value.Compare(_L("0"))==0);
	test(ini->FindVar(_L("1"),_L("setting"),value));
	test(value.Compare(_L("value"))==0);
	delete ini;

	}
	stopTime.UniversalTime();
	TTimeIntervalMicroSeconds timeTaken = stopTime.MicroSecondsFrom(startTime);
	test.Printf(_L("Time taken for 8 bit Legacy= %d microseconds\n"), timeTaken.Int64() );

//light weight
startTime.UniversalTime();
for (TInt i=0;i<1000;i++)
	{
	TPtrC8 value;
	//it is not necessarily to reopen a new file server session but just
	//for fair performance comparison
	RFs myFs;
	User::LeaveIfError(myFs.Connect())	;
	CIniFile8* ini=CIniFile8::NewL(myFs,_L("z:\\resource\\legacy8.ini"));

	test(ini->FindVar(_L8("test_section"),_L8("key1"),value)==KErrNone);
	test(value.Compare(_L8("value1"))==0);
	test(ini->FindVar(_L8("test_section"),_L8("key2"),value)==KErrNone);
	test(value.Compare(_L8("value2"))==0);
	test(ini->FindVar(_L8("test_section"),_L8("key3"),value)==KErrNone);
	test(value.Compare(_L8("value3"))==0);
	test(ini->FindVar(_L8("test_section"),_L8("key4"),value)==KErrNone);
	test(value.Compare(_L8("value4"))==0);
	test(ini->FindVar(_L8("test_section"),_L8("key5"),value)==KErrNone);
	test(value.Compare(_L8("value value value"))==0);
	test(ini->FindVar(_L8("1"),_L8("timestamps"),value)==KErrNone);
	test(value.Compare(_L8("0"))==0);
	test(ini->FindVar(_L8("1"),_L8("setting"),value)==KErrNone);
	test(value.Compare(_L8("value"))==0);
	delete ini;
	myFs.Close();
	}
stopTime.UniversalTime();
timeTaken = stopTime.MicroSecondsFrom(startTime);
test.Printf(_L("Time taken for 8 bit Light= %d microseconds\n"), timeTaken.Int64() );

//heavy weight
startTime.UniversalTime();
for (TInt i=0;i<1000;i++)
	{
	TPtrC8 value;
	//it is not necessarily to reopen a new file server session but just
	//for fair performance comparison
	RFs myFs;
	User::LeaveIfError(myFs.Connect())	;
	CIniDocument8* ini=CIniDocument8::NewL(myFs,_L("z:\\resource\\legacy8.ini"));

	test(ini->GetKeyValue(_L8("test_section"),_L8("key1"),value)==KErrNone);
	test(value.Compare(_L8("value1"))==0);

	test(ini->GetKeyValue(_L8("test_section"),_L8("key2"),value)==KErrNone);
	test(value.Compare(_L8("value2"))==0);

	test(ini->GetKeyValue(_L8("test_section"),_L8("key3"),value)==KErrNone);
	test(value.Compare(_L8("value3"))==0);

	test(ini->GetKeyValue(_L8("test_section"),_L8("key4"),value)==KErrNone);
	test(value.Compare(_L8("value4"))==0);

	test(ini->GetKeyValue(_L8("test_section"),_L8("key5"),value)==KErrNone);
	test(value.Compare(_L8("value value value"))==0);

	test(ini->GetKeyValue(_L8("1"),_L8("timestamps"),value)==KErrNone);
	test(value.Compare(_L8("0"))==0);

	test(ini->GetKeyValue(_L8("1"),_L8("setting"),value)==KErrNone);
	test(value.Compare(_L8("value"))==0);
	delete ini;
	myFs.Close();
	}
	stopTime.UniversalTime();
	timeTaken = stopTime.MicroSecondsFrom(startTime);
	test.Printf(_L("Time taken for 8 bit Heavy= %d microseconds\n"), timeTaken.Int64() );

}

static void PerformanceTest4L()
{
	test.Printf(_L("\n16 byte performance with existing parser comparison \n"));
	TTime startTime, stopTime;
	startTime.UniversalTime();
for (TInt i=0;i<1000;i++)
	{
	TPtrC value;
	CIniFile* ini=CIniFile::NewL(_L("z:\\resource\\legacy16.ini"));
	test(ini->FindVar(_L("test_section"),_L("key1"),value));
	test(value.Compare(_L("value1"))==0);
	test(ini->FindVar(_L("test_section"),_L("key2"),value));
	test(value.Compare(_L("value2"))==0);
	test(ini->FindVar(_L("test_section"),_L("key3"),value));
	test(value.Compare(_L("value3"))==0);
	test(ini->FindVar(_L("test_section"),_L("key4"),value));
	test(value.Compare(_L("value4"))==0);
	test(ini->FindVar(_L("test_section"),_L("key5"),value));
	test(value.Compare(_L("value value value"))==0);
	test(ini->FindVar(_L("1"),_L("timestamps"),value));
	test(value.Compare(_L("0"))==0);
	test(ini->FindVar(_L("1"),_L("setting"),value));
	test(value.Compare(_L("value"))==0);
	delete ini;
	}
	stopTime.UniversalTime();
	TTimeIntervalMicroSeconds timeTaken = stopTime.MicroSecondsFrom(startTime);
	test.Printf(_L("Time taken for 16 bit Legacy= %d microseconds\n"), timeTaken.Int64() );

	startTime.UniversalTime();
for (TInt i=0;i<1000;i++)
	{
	TPtrC16 value;
	CIniFile16* ini=CIniFile16::NewL(TheRFs,_L("z:\\resource\\legacy16.ini"));
	test(ini->FindVar(_L("test_section"),_L("key1"),value)==KErrNone);
	test(value.Compare(_L("value1"))==0);
	test(ini->FindVar(_L("test_section"),_L("key2"),value)==KErrNone);
	test(value.Compare(_L("value2"))==0);
	test(ini->FindVar(_L("test_section"),_L("key3"),value)==KErrNone);
	test(value.Compare(_L("value3"))==0);
	test(ini->FindVar(_L("test_section"),_L("key4"),value)==KErrNone);
	test(value.Compare(_L("value4"))==0);
	test(ini->FindVar(_L("test_section"),_L("key5"),value)==KErrNone);
	test(value.Compare(_L("value value value"))==0);
	test(ini->FindVar(_L("1"),_L("timestamps"),value)==KErrNone);
	test(value.Compare(_L("0"))==0);
	test(ini->FindVar(_L("1"),_L("setting"),value)==KErrNone);
	test(value.Compare(_L("value"))==0);
	delete ini;
	}
	stopTime.UniversalTime();
	timeTaken = stopTime.MicroSecondsFrom(startTime);
	test.Printf(_L("Time taken for 16 bit Light= %d microseconds\n"), timeTaken.Int64() );

	startTime.UniversalTime();
for (TInt i=0;i<1000;i++)
	{
	TPtrC16 value;
	CIniDocument16* ini=CIniDocument16::NewL(TheRFs,_L("z:\\resource\\legacy16.ini"));
	test(ini->GetKeyValue(_L("test_section"),_L("key1"),value)==KErrNone);
	test(value.Compare(_L("value1"))==0);
	test(ini->GetKeyValue(_L("test_section"),_L("key2"),value)==KErrNone);
	test(value.Compare(_L("value2"))==0);
	test(ini->GetKeyValue(_L("test_section"),_L("key3"),value)==KErrNone);
	test(value.Compare(_L("value3"))==0);
	test(ini->GetKeyValue(_L("test_section"),_L("key4"),value)==KErrNone);
	test(value.Compare(_L("value4"))==0);
	test(ini->GetKeyValue(_L("test_section"),_L("key5"),value)==KErrNone);
	test(value.Compare(_L("value value value"))==0);
	test(ini->GetKeyValue(_L("1"),_L("timestamps"),value)==KErrNone);
	test(value.Compare(_L("0"))==0);
	test(ini->GetKeyValue(_L("1"),_L("setting"),value)==KErrNone);
	test(value.Compare(_L("value"))==0);
	delete ini;
	}
	stopTime.UniversalTime();
	timeTaken = stopTime.MicroSecondsFrom(startTime);
	test.Printf(_L("Time taken for 16 bit Heavy= %d microseconds\n"), timeTaken.Int64() );
}

/**
@SYMTestCaseID		SYSLIB-BAFL-CT-1557
@SYMTestCaseDesc 	Test Performance
@SYMTestPriority 	High
@SYMTestActions  	-Test Performance by running Small 8 bit comparison against huge 16 bit.
			-Test Performance by comparing with exisiting ini file implementation
			 with this new implementation.
@SYMTestExpectedResults The test must not fail.
@SYMREQ PREQ505
*/
static void DoTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1557 "));
	//Small 8 bit comparision between light and heavy
	PerformanceTest1L();
	//Huge 16 bit comparison between light and heavy
	PerformanceTest2L();

	/**The legacy ini parser implementation is based on
	http://lon-xref.intra/lxr/source//common/generic/ser-comms/usb/usbman/src/inifile.cpp
	http://lon-xref.intra/lxr/source//common/generic/ser-comms/usb/usbman/inc/inifile.h
	*/
	//8 bit comparision with legacy,light and heavy
	PerformanceTest3L();
	//16 bit comparision with legacy,light and heavy
	PerformanceTest4L();
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* trapCleanup=CTrapCleanup::New();
	test(TheRFs.Connect()==KErrNone);
	test.Start(_L("Ini Parser Performance Testing"));

	TRAPD(error, DoTestL());
	test(error == KErrNone);

	TheRFs.Close();
	test.End();
	test.Close();
	delete trapCleanup;
	__UHEAP_MARKEND;
	return error;
	}

