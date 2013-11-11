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
// The implementation of the accessor class upon the CComponentInfo
// 
//

// ______________________________________________________________________________
//
_LIT(KTComponentInfo_DumpName,"C:\\System\\Data\\Logs\\TComponentInfo_StateDump.bin");

inline TInt TComponentInfo_StateAccessor::InvariantTest(TAny* aTestObject)
	{
	if(aTestObject)
		{
		// Nothing to do
		}
	return KErrNone;
	}

inline TInt TComponentInfo_StateAccessor::Internalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CComponentInfo* ComponentInfo = REINTERPRET_CAST(CComponentInfo*, aTestObject);
		TRAP(error,InternalizeL(ComponentInfo));
		}
	return error;
	}

inline TInt TComponentInfo_StateAccessor::Externalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CComponentInfo* ComponentInfo = REINTERPRET_CAST(CComponentInfo*, aTestObject);
		TRAP(error,ExternalizeL(ComponentInfo));
		}
	return error;
	}

inline void TComponentInfo_StateAccessor::InternalizeL(RFileReadStream& /* aStream */, CComponentInfo* /* aLoadManager */)
	{
	// Nothing to do
	}

inline void TComponentInfo_StateAccessor::ExternalizeL(RFileWriteStream& /* aStream */, CComponentInfo* /* aLoadManager */)
	{
	// Nothing to do
	}

// internal helpers
inline void TComponentInfo_StateAccessor::InternalizeL(CComponentInfo* aComponentInfo)
	{
	// Read in the dumped data
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Read the object dump
	TParse filename;
	filename.Set(KTComponentInfo_DumpName(),NULL,NULL);
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs,filename.FullName(), EFileRead));
	CleanupClosePushL(stream);
	InternalizeL(stream, aComponentInfo);
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

inline void TComponentInfo_StateAccessor::ExternalizeL(CComponentInfo* aComponentInfo)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Write out the object dump
	TParse filename;
	filename.Set(KTComponentInfo_DumpName(),NULL,NULL);
	RFileWriteStream stream;
	User::LeaveIfError(stream.Replace(fs,filename.FullName(), EFileWrite));
	CleanupClosePushL(stream);
	ExternalizeL(stream, aComponentInfo);
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

