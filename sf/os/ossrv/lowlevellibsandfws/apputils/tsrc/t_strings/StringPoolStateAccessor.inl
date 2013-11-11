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
// The implementation of the accessor class upon the RStringPool
// 
//

// ______________________________________________________________________________
//
_LIT(KTStringPool_DumpName,"C:\\System\\Data\\Logs\\TStringPool_StateDump.bin");

inline TInt TStringPool_StateAccessor::InvariantTest(TAny* aTestObject)
	{
	if(aTestObject)
		{
		}
	return KErrNone;
	}

inline TInt TStringPool_StateAccessor::Internalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		RStringPool* StringPool = REINTERPRET_CAST(RStringPool*, aTestObject);
		TRAP(error,InternalizeL(StringPool));
		}
	return error;
	}

inline TInt TStringPool_StateAccessor::Externalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		RStringPool* StringPool = REINTERPRET_CAST(RStringPool*, aTestObject);
		TRAP(error,ExternalizeL(StringPool));
		}
	return error;
	}

inline void TStringPool_StateAccessor::InternalizeL(RFileReadStream& /* aStream */, RStringPool* /* aLoadManager */)
	{
	}

inline void TStringPool_StateAccessor::ExternalizeL(RFileWriteStream& /* aStream */, RStringPool* /* aLoadManager */)
	{
	}

// internal helpers
inline void TStringPool_StateAccessor::InternalizeL(RStringPool* aStringPool)
	{
	// Read in the dumped data
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Read the object dump
	TParse filename;
	filename.Set(KTStringPool_DumpName(),NULL,NULL);
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs,filename.FullName(), EFileRead));
	CleanupClosePushL(stream);
	InternalizeL(stream, aStringPool);
	CleanupStack::PopAndDestroy();	// stream
	CleanupStack::PopAndDestroy();	// fs
	}

inline void TStringPool_StateAccessor::ExternalizeL(RStringPool* aStringPool)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Write out the object dump
	TParse filename;
	filename.Set(KTStringPool_DumpName(),NULL,NULL);
	RFileWriteStream stream;
	User::LeaveIfError(stream.Replace(fs,filename.FullName(), EFileWrite));
	CleanupClosePushL(stream);
	ExternalizeL(stream, aStringPool);
	CleanupStack::PopAndDestroy();	// stream
	CleanupStack::PopAndDestroy();	// fs
	}

