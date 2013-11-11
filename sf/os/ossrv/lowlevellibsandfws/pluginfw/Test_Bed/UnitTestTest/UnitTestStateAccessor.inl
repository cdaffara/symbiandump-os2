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
// The implementation of the accessor class upon the CUnitTest
// 
//

// ______________________________________________________________________________
//
_LIT(KTUnitTest_DumpName,"C:\\System\\Data\\Logs\\TUnitTest_StateDump.bin");

inline TInt TUnitTest_StateAccessor::InvariantTest(TAny* aTestObject)
	{
	if(aTestObject)
		{
		// CUnitTest* UnitTest = REINTERPRET_CAST(CUnitTest*, aTestObject);
		// Implement the correct Invariant test for CUnitTest.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct Invariant test for CUnitTest.")
		}
	return KErrNone;
	}

inline TInt TUnitTest_StateAccessor::Internalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CUnitTest* UnitTest = REINTERPRET_CAST(CUnitTest*, aTestObject);
		TRAP(error,InternalizeL(UnitTest));
		}
	return error;
	}

inline TInt TUnitTest_StateAccessor::Externalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CUnitTest* UnitTest = REINTERPRET_CAST(CUnitTest*, aTestObject);
		TRAP(error,ExternalizeL(UnitTest));
		}
	return error;
	}

inline void TUnitTest_StateAccessor::InternalizeL(RFileReadStream& /* aStream */, CUnitTest* /* aLoadManager */)
	{
	// Not required
	}

inline void TUnitTest_StateAccessor::ExternalizeL(RFileWriteStream& /* aStream */, CUnitTest* /* aLoadManager */)
	{
	// Not required
	}


inline CUnitTestTestDerivation::CUnitTestTestDerivation(const TDesC& aName, CDataLogger& aDataLogger, MUnitTestObserver& aUnitTestObserver)
	: CUnitTest(aName, aDataLogger, aUnitTestObserver)
	{
	}

inline void CUnitTestTestDerivation::ConstructL()
	{
	UnitTestConstructL();
	}

inline CUnitTestTestDerivation* TUnitTest_StateAccessor::CreateUnitTest(CUnitTest&			/*aUnitTest*/, 
																		const TDesC&		aName, 
																		CDataLogger&		aDataLogger, 
																		MUnitTestObserver&	aUnitTestObserver)
	{
	return new(ELeave)CUnitTestTestDerivation(aName, aDataLogger, aUnitTestObserver);
	}


inline void TUnitTest_StateAccessor::RunL(CUnitTest& aUnitTest)
	{
	aUnitTest.RunL();
	}

inline void TUnitTest_StateAccessor::AddTransitionL(CUnitTest& aUnitTest, CTransition* aTransition)
	{
	aUnitTest.AddTransitionL(aTransition);
	}

inline void TUnitTest_StateAccessor::AddBlockingTransitionL(CUnitTest& aUnitTest, CTransition* aTransition)
	{
	aUnitTest.AddBlockingTransitionL(aTransition);
	}

inline void TUnitTest_StateAccessor::AddLeaveErrorCodeL(CUnitTest& aUnitTest, TInt& aLeaveErrorCode)
	{
	aUnitTest.AddLeaveErrorCodeL(aLeaveErrorCode);
	}


// internal helpers
inline void TUnitTest_StateAccessor::InternalizeL(CUnitTest* aUnitTest)
	{
	// Read in the dumped data
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Read the object dump
	TParse filename;
	filename.Set(KTUnitTest_DumpName(),NULL,NULL);
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs,filename.FullName(), EFileRead));
	CleanupClosePushL(stream);
	InternalizeL(stream, aUnitTest);
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

inline void TUnitTest_StateAccessor::ExternalizeL(CUnitTest* aUnitTest)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Write out the object dump
	TParse filename;
	filename.Set(KTUnitTest_DumpName(),NULL,NULL);
	RFileWriteStream stream;
	User::LeaveIfError(stream.Replace(fs,filename.FullName(), EFileWrite));
	CleanupClosePushL(stream);
	ExternalizeL(stream, aUnitTest);
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

