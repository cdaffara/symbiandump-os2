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
// The implementation of the accessor class upon the CTestManager
// 
//

// ______________________________________________________________________________
//
_LIT(KTTestManager_DumpName,"C:\\System\\Data\\Logs\\TTestManager_StateDump.bin");

inline TInt TTestManager_StateAccessor::InvariantTest(TAny* aTestObject)
	{
	if(aTestObject)
		{
		CTestManager* theTestManager = REINTERPRET_CAST(CTestManager*, aTestObject);
		if(theTestManager->iTestList == NULL)
			return KErrArgument;
		}
	return KErrNone;
	}

inline TInt TTestManager_StateAccessor::Internalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CTestManager* theTestManager = REINTERPRET_CAST(CTestManager*, aTestObject);
		TRAP(error,InternalizeL(theTestManager));
		}
	return error;
	}

inline TInt TTestManager_StateAccessor::Externalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CTestManager* theTestManager = REINTERPRET_CAST(CTestManager*, aTestObject);
		TRAP(error,ExternalizeL(theTestManager));
		}
	return error;
	}

inline void TTestManager_StateAccessor::InternalizeL(RFileReadStream& /* aStream */, CTestManager* /* aLoadManager */)
	{
	// Not required
	}

inline void TTestManager_StateAccessor::ExternalizeL(RFileWriteStream& /* aStream */, CTestManager* /* aLoadManager */)
	{
	// Not required
	}

inline void TTestManager_StateAccessor::RunL(CTestManager& aTestMan)
	{
	aTestMan.RunL();
	}

inline void TTestManager_StateAccessor::TestComponentL(CTestManager& aTestMan, TInt& aComponentIndex)
	{
	aTestMan.TestComponentL(aComponentIndex);
	}

inline void TTestManager_StateAccessor::ActivateManager(CTestManager& aTestMan)
	{
	aTestMan.SetActive();
	aTestMan.iStatus = KRequestPending;
	}

// internal helpers
inline void TTestManager_StateAccessor::InternalizeL(CTestManager* aTestManager)
	{
	// Read in the dumped data
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Read the object dump
	TParse filename;
	filename.Set(KTTestManager_DumpName(),NULL,NULL);
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs,filename.FullName(), EFileRead));
	CleanupClosePushL(stream);
	InternalizeL(stream, aTestManager);
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

inline void TTestManager_StateAccessor::ExternalizeL(CTestManager* aTestManager)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Write out the object dump
	TParse filename;
	filename.Set(KTTestManager_DumpName(),NULL,NULL);
	RFileWriteStream stream;
	User::LeaveIfError(stream.Replace(fs,filename.FullName(), EFileWrite));
	CleanupClosePushL(stream);
	ExternalizeL(stream, aTestManager);
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

