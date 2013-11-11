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
// The state accessor class implementations for the CLoadManager class
// 
//

// ______________________________________________________________________________
//
_LIT(KTLoadManager_DumpName,"C:\\System\\Data\\Logs\\TLoadManager_StateDump.bin");

inline TInt TLoadManager_StateAccessor::InvariantTest(TAny* aTestObject)
	{
	if(aTestObject)
		{
		// Nothing to do
		// CLoadManager* LoadManager = REINTERPRET_CAST(CLoadManager*, aTestObject);
		}
	return KErrNone;
	}

inline TInt TLoadManager_StateAccessor::Internalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject)
		{
		CLoadManager* LoadManager = REINTERPRET_CAST(CLoadManager*, aTestObject);
		TRAP(error,InternalizeL(LoadManager));
		}
	return error;
	}

inline TInt TLoadManager_StateAccessor::Externalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject)
		{
		CLoadManager* LoadManager = REINTERPRET_CAST(CLoadManager*, aTestObject);
		TRAP(error,ExternalizeL(LoadManager));
		}
	return error;
	}

inline void TLoadManager_StateAccessor::InternalizeL(RFileReadStream& /* aStream */, CLoadManager* /* aLoadManager */)
	{
	// Nothing to do
	}

inline void TLoadManager_StateAccessor::ExternalizeL(RFileWriteStream& /* aStream */, CLoadManager* /* aLoadManager */)
	{
	// Nothing to do
	}

// internal helpers
inline void TLoadManager_StateAccessor::InternalizeL(CLoadManager* aLoadManager)
	{
	// Read in the dumped data
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Read the object dump
	TParse filename;
	filename.Set(KTLoadManager_DumpName(),NULL,NULL);
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs,filename.FullName(), EFileRead));
	CleanupClosePushL(stream);
	InternalizeL(stream, aLoadManager);
	CleanupStack::PopAndDestroy();	// stream
	CleanupStack::PopAndDestroy();	// fs
	}

inline void TLoadManager_StateAccessor::ExternalizeL(CLoadManager* aLoadManager)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Write out the object dump
	TParse filename;
	filename.Set(KTLoadManager_DumpName(),NULL,NULL);
	RFileWriteStream stream;
	User::LeaveIfError(stream.Replace(fs,filename.FullName(), EFileWrite));
	CleanupClosePushL(stream);
	ExternalizeL(stream, aLoadManager);
	CleanupStack::PopAndDestroy();	// stream
	CleanupStack::PopAndDestroy();	// fs
	}
