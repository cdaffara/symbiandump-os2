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
//

#include "t_cenrep_helper.h"
#include <centralrepository.h>
#include <e32property.h>
#include <e32test.h>
#include <f32file.h>
#include <hal.h>
#include <hal_data.h>
#include "t_cenrep_PatchData.h"

using namespace NCentralRepositoryConstants;

RTest TheTest(_L("Central Repository Defect Tests"));

TUint64 FastCountToMicrosecondsInInt(TInt aFastCount)
	{
	TInt freqInHz;
	HAL::Get(HAL::EFastCounterFrequency, freqInHz);
	return (aFastCount*1000000LL)/freqInHz;
	}

void BulkBackupCreL(RFs& aFileServer,TInt startUid,TInt endUid,TBool aRestore)
	{
	TFileName persist_path(_L("c:\\private\\10202be9\\persists\\"));
	for (TInt uid=startUid;uid<=endUid;uid++)
		{
		TUid keyuid = TUid::Uid(uid);
		TFileName source;
		source.Append(persist_path);
		source.AppendFormat(_L("%x.cre"),uid);
		TFileName target;
		target.Append(persist_path);
		target.AppendFormat(_L("%x.cre_bak"),uid);
		TInt ret=0;
		if (!aRestore)
			ret=aFileServer.Rename(source,target);
		else
			ret=aFileServer.Rename(target,source);
		User::LeaveIfError(ret);
		}
	}

/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4069
@SYMTestCaseDesc Open and close 300 keyspace files.  This will ensure that the keyspace information will be
read for all 300 keyspaces.  300 txt keyspace files, single layer rofs, pre-PREQ2112 code
@SYMTestPriority Medium
@SYMTestActions Open keyspaces 0x10286767 through 0x10286892.  The keyspace file names in the source have been mangled in a similar way to that
which would be expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Time taken to run test logged to file.
@SYMREQ REQ11525
*/
/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4070
@SYMTestCaseDesc Open and close 300 keyspace files.  This will ensure that the keyspace information will be
read for all 300 keyspaces.  300 cre keyspace files, single layer rofs, pre-PREQ2112 code
@SYMTestPriority Medium
@SYMTestActions Open keyspaces 0x10286893 through 0x102869BE.  The keyspace file names in the source have been mangled in a similar way to that
which would be expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Time taken to run test logged to file.
@SYMREQ REQ11525
*/
/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4071
@SYMTestCaseDesc Open and close 300 keyspace files.  This will ensure that the keyspace information will be
read for all 300 keyspaces.  300 txt keyspace files, single layer rofs, post-PREQ2112 code
@SYMTestPriority Medium
@SYMTestActions Open keyspaces 0x10286767 through 0x10286892.  The keyspace file names in the source have been mangled in a similar way to that
which would be expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Time taken to run test logged to file.
@SYMREQ REQ11525
*/
/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4072
@SYMTestCaseDesc Open and close 300 keyspace files.  This will ensure that the keyspace information will be
read for all 300 keyspaces.  300 cre keyspace files, single layer rofs, post-PREQ2112 code
@SYMTestPriority Medium
@SYMTestActions Open keyspaces 0x10286893 through 0x102869BE.  The keyspace file names in the source have been mangled in a similar way to that
which would be expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Time taken to run test logged to file.
@SYMREQ REQ11525
*/
/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4073
@SYMTestCaseDesc Open and close 300 keyspace files.  This will ensure that the keyspace information will be
read for all 300 keyspaces.  300 txt keyspace files, two layer rofs, post-PREQ2112 code
@SYMTestPriority Medium
@SYMTestActions Open keyspaces 0x10286767 through 0x10286892 for txt tests.  The keyspace file names in the source have been mangled in a similar way to that
which would be expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Time taken to run test logged to file.
@SYMREQ REQ11525
*/
/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4074
@SYMTestCaseDesc Open and close 300 keyspace files.  This will ensure that the keyspace information will be
read for all 300 keyspaces.  300 cre keyspace files, two layer rofs, post-PREQ2112 code
@SYMTestPriority Medium
@SYMTestActions Open keyspaces 0x10286893 through 0x102869BE.  The keyspace file names in the source have been mangled in a similar way to that
which would be expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Time taken to run test logged to file.
@SYMREQ REQ11525
*/
/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4075
@SYMTestCaseDesc Open and close 300 keyspace files.  This will ensure that the keyspace information will be
read for all 300 keyspaces.  300 txt keyspace files, four layer rofs, post-PREQ2112 code
@SYMTestPriority Medium
@SYMTestActions Open keyspaces 0x10286767 through 0x10286892.  The keyspace file names in the source have been mangled in a similar way to that
which would be expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Time taken to run test logged to file.
@SYMREQ REQ11525
*/

/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4076
@SYMTestCaseDesc Open and close 300 keyspace files.  This will ensure that the keyspace information will be
read for all 300 keyspaces.  300 cre keyspace files, four layer rofs, post-PREQ2112 code
@SYMTestPriority Medium
@SYMTestActions Open keyspaces 0x10286893 through 0x102869BE.  The keyspace file names in the source have been mangled in a similar way to that
which would be expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Time taken to run test logged to file.
@SYMREQ REQ11525
*/
LOCAL_C void PerformanceTestsL()
	{
	RDebug::Print(_L("This test is running one of the eight test cases listed.  Which one is dependent on the ROM build.  See associated test headers for further info"));
	TheTest.Next(_L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-4069 through 4076" ));

	CRepository* rep;
   	TUint32 startFastCounter;
   	TUint64 fastCounterromOpenMicroseconds;
   	TBuf<100> outputtxt;
	//----------------SETUP BEFORE RUNNING TXT TESTING---------------------------
	//backup files from cre to another name so not get detected
	RFs fileserver;
	User::LeaveIfError(fileserver.Connect());
	CleanupClosePushL(fileserver);

	//-------------------RUN TXT PERFORMANCE FIRST-----------------------------------
if (KCentrepPerfTestRunMode==2)
	{
	//now restore previously backup file for TXT testing
	BulkBackupCreL(fileserver,0x10286767,0x10286892,ETrue);
	}

if (KCentrepPerfTestRunMode==1 || KCentrepPerfTestRunMode==2)
	{
	for(TInt uid = 0x10286767; uid <= 0x10286892; uid++)
		{
		TUid keyuid = TUid::Uid(uid);
	    startFastCounter = User::FastCounter();
		rep = CRepository::NewL(keyuid);
		delete rep;

		fastCounterromOpenMicroseconds = FastCountToMicrosecondsInInt(User::FastCounter() - startFastCounter);
		outputtxt.Format(_L("to open and close the %d txt file took %lu ms"),uid,fastCounterromOpenMicroseconds);
		TheTest.Next(outputtxt);
		}
	}
	//-------------------SETUP BEFORE RUNNING CRE TESTING------------------------------

if (KCentrepPerfTestRunMode==2)
	{
	//Kill the server first before proceeding with the CRE testing
	_LIT( KCentralRepositoryServerName, "Centralrepositorysrv");
	KillProcess(KCentralRepositoryServerName());
	// Delay to allow the server to close down
	User::After(2000000);
	CleanupCDriveL();
	//now backup files that are generated from the txt testing
	BulkBackupCreL(fileserver,0x10286767,0x10286892,EFalse);

	//now restore previously backup file for CRE testing
	BulkBackupCreL(fileserver,0x10286893,0x102869BE,ETrue);
	}
	//-----------------RUN CRE PERFORMANCE FIRST---------------------------------------

if (KCentrepPerfTestRunMode==0 || KCentrepPerfTestRunMode==2)
	{
	for(TInt uid = 0x10286893; uid <= 0x102869BE; uid++)
		{
		TUid keyuid = TUid::Uid(uid);
	    startFastCounter = User::FastCounter();
		rep = CRepository::NewL(keyuid);
		delete rep;
		fastCounterromOpenMicroseconds = FastCountToMicrosecondsInInt(User::FastCounter() - startFastCounter);
		outputtxt.Format(_L("to open and close the %d cre file took %lu ms"),uid,fastCounterromOpenMicroseconds);
		TheTest.Next(outputtxt);
		}
	}

if (KCentrepPerfTestRunMode==2)
	{
	//now backup files that are generated from the cre testing
	BulkBackupCreL(fileserver,0x10286893,0x102869BE,EFalse);
	}

	//cleanup
	CleanupStack::PopAndDestroy();  // fileserver
	}

LOCAL_C void MainL()
	{
	TheTest.Start(_L("PREQ2112 Performance Testing"));

	TheTest.Next(_L("Performance Tests"));
	PerformanceTestsL();

	TheTest.End();
	TheTest.Close();
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err, MainL());
	if (err != KErrNone)
		User::Panic(_L("Testing failed: "), err);

	delete cleanup;
	__UHEAP_MARKEND;

	return 0;
	}
