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

/**
 @file
 @internalTechnology 
*/

// System Include
#include <escapeutils.h>

// User Includes
#include "TestFileUriServer.h"
#include "TestCreateFileStep.h"
#include "TestGetFileNameFromUriStep.h"
#include "TestGenerateFileUriStep.h"
#include "TestDeleteFileStep.h"
#include "TestForAllFilesStep.h"

// The system-wide unique name for the test-server
_LIT(KServerName, "TestFileUriServer");

TBuf<KMaxDrives> CTestFileUriServer::iRemovableDrives(KNullDesC);

/**
Static factory constructor. Creates and returns instance of the test server
@return		A pointer to the newly created CTestFileUriServer object
*/
CTestFileUriServer*  CTestFileUriServer::NewL()
	{
	// Construct the server
	CTestFileUriServer* server = new(ELeave) CTestFileUriServer();
	CleanupStack::PushL(server);

	// CServer base class call
	// Name the server using the system-wide unique string
	// Clients use this to create server sessions.
	server->StartL(KServerName);
	
	CleanupStack::Pop(server);
	return server;
	}
	
	
#if (!defined EKA2)

/**
Creates the Active Scheduler, then creates the test-server, synchronises the 
thread with the client and then enters the active scheduler.

This is EKA1 version of MainL(). Uses sempahore to sync with client
as Rendezvous calls are not available
*/
LOCAL_C void MainL()
	{
	// Create and install the active scheduler.
	CActiveScheduler* sched = NULL;
	sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	
	// Create the server inside trap harness
	CTestFileUriServer *server = NULL;
	TRAPD(err, server = CTestFileUriServer::NewL());
	if (!err)
		{
		CleanupStack::PushL(server);
		RSemaphore sem;
		
		// The client API of TestExecute will already have created the 
		// semaphore and will be waiting on it.
		User::LeaveIfError(sem.OpenGlobal(KServerName));
		
		CleanupStack::Pop(server);
		
		// Signal the client
		sem.Signal();
		sem.Close();
		
		// Enter the active scheduler
		sched->Start();
		}
	CleanupStack::Pop(sched);
	delete server;
	delete sched;
	}
#else
/**
EKA2 version of MainL()
Uses the new Rendezvous call isntead of the older semaphore.
*/
LOCAL_C void MainL()
	{
	// For platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);	
	RProcess().SecureApi(RProcess::ESecureApiOn);	
#endif
	CActiveScheduler* sched = NULL;
	sched = new(ELeave) CActiveScheduler; 
	CActiveScheduler::Install(sched);
	CTestFileUriServer* server = NULL;
	
	// Create the test-server
	TRAPD(err, server = CTestFileUriServer::NewL());
	
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;	
	}
#endif		// #if (!defined EKA2)
	

#if (defined __WINS__ && !defined EKA2)
/**
DLL entry-point for EKA1 emulator builds.
*/
GLDEF_C TInt E32Dll(enum TDllReason /*aDllReason*/)
	{
	return KErrNone;
	}
	
#else
/**
Exe entry point code, for EKA1 hardware and EKA2 builds.
*/
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup *cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TInt err = KErrNone;
	TRAP(err, MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}
#endif		// #if (defined __WINS__ && !defined EKA2)

#if (defined __WINS__ && !defined EKA2)
/**
For EKA1 emulator builds. This function is called when the thread is first 
resumed. Has the standard thread entry siganture. 
*/
TInt ThreadFunc (TAny* /*aParam*/)
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TInt err = KErrNone;
	TRAP(err, MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
For EKA1 emulator builds. Creates and starts a thread for the server to run.
*/
EXPORT_C TInt NewServer()
	{
	_LIT(KThread, "Thread");
	RThread thread;
	
	// Name the thread as "<Server-Name>Thread" making it hopefully unique
	TBuf<KMaxTestExecuteNameLength> threadName(KServerName);
	threadName.Append(KThread);
	
	const TInt KMaxHeapSize = 0x1000000;
	
	// Create the thread
	TInt err = thread.Create(threadName, ThreadFunc, KDefaultStackSize, 
							 KMinHeapSize, KMaxHeapSize, NULL, EOwnerProcess);
	if (err != KErrNone)
		{
		return err;
		}
	
	// Start the thread -> effectively calls ThreadFunc
	thread.Resume();
	
	thread.Close();
	return KErrNone;
	}
	
#endif 		// #if (defined __WINS__ && !defined EKA2)


/**
Base class pure virtual
@return - Instance of the test step
*/
CTestStep* CTestFileUriServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep *testStep = NULL;
	
	if (aStepName == KTestCreateFileStep)
		{
		testStep = new (ELeave) CTestCreateFileStep;
		}
	else if (aStepName == KTestGetFileNameFromUriStep)
		{
		testStep = new (ELeave) CTestGetFileNameFromUriStep;
		}	
	else if (aStepName == KTestGenerateFileUriStep)
		{
		testStep = new (ELeave) CTestGenerateFileUriStep;
		}
	else if (aStepName == KTestDeleteFileStep)
		{
		testStep = new (ELeave) CTestDeleteFileStep;
		}
	else if (aStepName == KTestForAllFilesStep)
		{
		testStep = new (ELeave) CTestForAllFilesStep();
		}
		return testStep;
	}

/**
Returns the equivalent drive number of a drive
*/
void CTestFileUriServer::GetDriveNumber(const TDesC& aDrive, TDriveNumber& aDriveNum)
	{
	TBuf<1> driveLetter(aDrive.Left(1));
	driveLetter.LowerCase();
	aDriveNum = static_cast <TDriveNumber> (driveLetter[0] - KLetterA);
	}

/**
Checks whether a specific drive is a removable drive
*/
TInt CTestFileUriServer::IsRemovableDrive(const TDriveNumber& aDriveNum, TBool& aResult)
	{
	TInt err = KErrNone;
	TDriveInfo driveInfo;	
	RFs fs;
	aResult = EFalse;
	err = fs.Connect();
	if(err == KErrNone)
		{
		err = fs.Drive(driveInfo, aDriveNum);
		if (err == KErrNone && driveInfo.iDriveAtt & KDriveAttRemovable)
			{
			aResult = ETrue;
			}
		fs.Close();	
		}
	return err;	
	}
	
/**
Replaces the drive placeholder <drive> with the actual drive
*/
HBufC16* CTestFileUriServer::CheckAndFillDriveNameL(const TPtrC& aFileUri, const TPtrC& aDrive)
	{
	HBufC16* expectedUriWithDrive = aFileUri.AllocL();
	TInt placeHolderPos = aFileUri.Find(KDrivePlaceHolder);	
	if(placeHolderPos >= KErrNone)
		{
		if(aDrive == KExtMedia)
			{// Create a descriptor that is big enough
			// Just in case ReAllocL leaves
			CleanupStack::PushL(expectedUriWithDrive);
			expectedUriWithDrive = expectedUriWithDrive->ReAllocL(aFileUri.Length() + (KExtMedia().Length() - KDrivePlaceHolder().Length()));
			CleanupStack::Pop(); // expectedUriWithDrive
			}
		expectedUriWithDrive->Des().Replace(placeHolderPos, KDrivePlaceHolder().Length(), aDrive);
		}
	return expectedUriWithDrive;
	}

/**
Private function used to find the remobale drives and populate iRemovableDrives
*/
TInt CTestFileUriServer::PopulateRemovableDrivesBuf(const RFs& aFs)
	{
	TInt err = KErrNone;
	TDriveInfo driveInfo;
	TInt driveNum;
	for (driveNum = EDriveA; driveNum <= EDriveZ; driveNum++)   
		{
		// Populate iRemovableDrives with all removable drives in alphabetical order
		err = aFs.Drive(driveInfo, driveNum);
		if (err == KErrNone && (driveInfo.iDriveAtt & KDriveAttRemovable))       
			{
			iRemovableDrives.Append(TInt16('a' + driveNum));
			}
		}
	return err;
	}

/**
Searches whether a file with same name and path exists in any other removable drive
*/
TInt CTestFileUriServer::FirstRemovableDriveWithSameFileName(const TDesC& aFileName, TBuf<1>& aCorrectDrive)
	{
	aCorrectDrive = aFileName.Left(1);
	TInt err = KErrNone;
	RFs fs;
	if((err = fs.Connect()) != KErrNone)
		{
		return err;
		}

	if(iRemovableDrives == KNullDesC)
		{
		if((err = PopulateRemovableDrivesBuf(fs)) != KErrNone)
			{
			return err;
			}
		}
	TInt index;
	HBufC* tempFileName	= NULL;
	if((tempFileName = aFileName.Alloc()) == NULL)
		{
		return KErrGeneral;
		}
	for(index = 0; index < iRemovableDrives.Length(); ++index)
		{
		TUint attValue;
		// change the drive in the filename and check whether such a file exists
		tempFileName->Des()[0] = iRemovableDrives[index];
		err = fs.Att(tempFileName->Des(), attValue);
		if(err == KErrNone)
			{
			aCorrectDrive[0] = iRemovableDrives[index];
			break;
			}
		}
	if(index >= iRemovableDrives.Length())
		{// File not found on any removable drive
		aCorrectDrive = KNullDesC;
		}
	delete tempFileName;
	fs.Close();	
	return KErrNone;
	}
	
/**
Obtains the private directory of the application and appends it along with the
relative filename and drive to create the fully qualified filename
*/
TInt CTestFileUriServer::CreateFullyQualifiedName(const TPtrC& aRelativeName, const TPtrC& aDrive, TFileName& aFullyQualifiedName)
	{
	RFs fs;
	TInt err = fs.Connect();
	if(err != KErrNone)
		{
		return err;
		}
	// Get private dir name
	err = fs.PrivatePath(aFullyQualifiedName);
	fs.Close();
	if(err != KErrNone)
		{
		return err;
		}
	// Construct fully-qualified filename
	aFullyQualifiedName.Insert(0, KDriveSeparator);
	aFullyQualifiedName.Insert(0, aDrive);
	TInt position = 0;
	// If backslash already exists dont include again
	if(aRelativeName.Left(1) == KBackSlash)
		{
		++position;
		}
	aFullyQualifiedName.Append(aRelativeName.Right(aRelativeName.Length() - position));
	return KErrNone;
	}
