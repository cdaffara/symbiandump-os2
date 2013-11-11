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
// The implementation of the accessor class upon the CComponentTester
// 
//

// ______________________________________________________________________________
//
_LIT(KTComponentTester_DumpName,"C:\\System\\Data\\Logs\\TComponentTester_StateDump.bin");

inline TInt TComponentTester_StateAccessor::InvariantTest(TAny* aTestObject)
	{
	if(aTestObject)
		{
		CComponentTester* theComponentTester = REINTERPRET_CAST(CComponentTester*, aTestObject);
		if(theComponentTester->iUnitTests == NULL)
			return KTestBedFailedInvariant;
		}
	return KErrNone;
	}

inline TInt TComponentTester_StateAccessor::Internalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CComponentTester* ComponentTester = REINTERPRET_CAST(CComponentTester*, aTestObject);
		TRAP(error,InternalizeL(ComponentTester));
		}
	return error;
	}

inline TInt TComponentTester_StateAccessor::Externalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CComponentTester* ComponentTester = REINTERPRET_CAST(CComponentTester*, aTestObject);
		TRAP(error,ExternalizeL(ComponentTester));
		}
	return error;
	}

inline void TComponentTester_StateAccessor::InternalizeL(RFileReadStream& /* aStream */, CComponentTester* /* aLoadManager */)
	{
	// Not required
	}

inline void TComponentTester_StateAccessor::ExternalizeL(RFileWriteStream& /* aStream */, CComponentTester* /* aLoadManager */)
	{
	// Not required
	}


inline CComponentTesterTestDerivation::CComponentTesterTestDerivation(CDataLogger& aDataLogger, MComponentTestObserver& aObserver)
	: CComponentTester(aDataLogger, aObserver)
	{
	}

inline void CComponentTesterTestDerivation::ConstructL()
	{
	ComponentTesterConstructL();
	}

inline CComponentTesterTestDerivation* TComponentTester_StateAccessor::CreateComponentTester(CComponentTester& /*aCompTest*/, 
																							 CDataLogger& aDataLogger, 
																							 MComponentTestObserver& aObserver)
	{
	return new(ELeave)CComponentTesterTestDerivation(aDataLogger, aObserver);
	}

inline void TComponentTester_StateAccessor::AddUnitTestL(CComponentTester& aCompTest, CUnitTest* aUnitTest)
	{
	aCompTest.AddUnitTestL(aUnitTest);
	}

inline void TComponentTester_StateAccessor::AddParamUnitTestL(CComponentTester& aCompTest, CUnitTest* aUnitTest)
	{
	aCompTest.AddParamUnitTestL(aUnitTest);
	}

inline void TComponentTester_StateAccessor::RunL(CComponentTester& aCompTest)
	{
	aCompTest.RunL();
	}

// internal helpers
inline void TComponentTester_StateAccessor::InternalizeL(CComponentTester* aComponentTester)
	{
	// Read in the dumped data
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Read the object dump
	TParse filename;
	filename.Set(KTComponentTester_DumpName(),NULL,NULL);
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs,filename.FullName(), EFileRead));
	CleanupClosePushL(stream);
	InternalizeL(stream, aComponentTester);
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

inline void TComponentTester_StateAccessor::ExternalizeL(CComponentTester* aComponentTester)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Write out the object dump
	TParse filename;
	filename.Set(KTComponentTester_DumpName(),NULL,NULL);
	RFileWriteStream stream;
	User::LeaveIfError(stream.Replace(fs,filename.FullName(), EFileWrite));
	CleanupClosePushL(stream);
	ExternalizeL(stream, aComponentTester);
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

