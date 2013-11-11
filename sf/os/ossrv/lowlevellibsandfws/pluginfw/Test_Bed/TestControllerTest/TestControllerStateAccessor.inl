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
// The implementation of the accessor class upon the CTestController
// 
//

// ______________________________________________________________________________
//
_LIT(KTTestController_DumpName,"C:\\System\\Data\\Logs\\TTestController_StateDump.bin");

inline TInt TTestController_StateAccessor::InvariantTest(TAny* aTestObject)
	{
	if(aTestObject)
		{
		CTestController* TestController = REINTERPRET_CAST(CTestController*, aTestObject);
		if (TestController->iDataLogger == NULL ||
			TestController->iScheduler == NULL ||
			TestController->iTestManager == NULL)
			return KTestBedFailedInvariant;
		}
	return KErrNone;
	}

inline TInt TTestController_StateAccessor::Internalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CTestController* TestController = REINTERPRET_CAST(CTestController*, aTestObject);
		TRAP(error,InternalizeL(TestController));
		}
	return error;
	}

inline TInt TTestController_StateAccessor::Externalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CTestController* TestController = REINTERPRET_CAST(CTestController*, aTestObject);
		TRAP(error,ExternalizeL(TestController));
		}
	return error;
	}

inline void TTestController_StateAccessor::InternalizeL(RFileReadStream& /* aStream */, CTestController* /* aLoadManager */)
	{
	// Not required
	}

inline void TTestController_StateAccessor::ExternalizeL(RFileWriteStream& /* aStream */, CTestController* /* aLoadManager */)
	{
	// Not required
	}

// internal helpers
inline void TTestController_StateAccessor::InternalizeL(CTestController* aTestController)
	{
	// Read in the dumped data
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Read the object dump
	TParse filename;
	filename.Set(KTTestController_DumpName(),NULL,NULL);
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs,filename.FullName(), EFileRead));
	CleanupClosePushL(stream);
	InternalizeL(stream, aTestController);
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

inline void TTestController_StateAccessor::ExternalizeL(CTestController* aTestController)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Write out the object dump
	TParse filename;
	filename.Set(KTTestController_DumpName(),NULL,NULL);
	RFileWriteStream stream;
	User::LeaveIfError(stream.Replace(fs,filename.FullName(), EFileWrite));
	CleanupClosePushL(stream);
	ExternalizeL(stream, aTestController);
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

