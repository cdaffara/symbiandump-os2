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
// The implementation of the accessor class upon the CDefaultLogOutput
// 
//

// ______________________________________________________________________________
//
_LIT(KTDefaultLogOutput_DumpName,"C:\\System\\Data\\Logs\\TDefaultLogOutput_StateDump.bin");

inline TInt TDefaultLogOutput_StateAccessor::InvariantTest(TAny* aTestObject)
	{
	if(aTestObject)
		{
		// Nothing to do
		}
	return KErrNone;
	}

inline TInt TDefaultLogOutput_StateAccessor::Internalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CDefaultLogOutput* theDefaultLogOutput = REINTERPRET_CAST(CDefaultLogOutput*, aTestObject);
		TRAP(error,InternalizeL(theDefaultLogOutput));
		}
	return error;
	}

inline TInt TDefaultLogOutput_StateAccessor::Externalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CDefaultLogOutput* theDefaultLogOutput = REINTERPRET_CAST(CDefaultLogOutput*, aTestObject);
		TRAP(error,ExternalizeL(theDefaultLogOutput));
		}
	return error;
	}

inline void TDefaultLogOutput_StateAccessor::InternalizeL(RFileReadStream& /* aStream */, CDefaultLogOutput* /* aLoadManager */)
	{
	}

inline void TDefaultLogOutput_StateAccessor::ExternalizeL(RFileWriteStream& /* aStream */, CDefaultLogOutput* /* aLoadManager */)
	{
	}

// internal helpers
inline void TDefaultLogOutput_StateAccessor::InternalizeL(CDefaultLogOutput* aDefaultLogOutput)
	{
	// Read in the dumped data
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Read the object dump
	TParse filename;
	filename.Set(KTDefaultLogOutput_DumpName(),NULL,NULL);
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs,filename.FullName(), EFileRead));
	CleanupClosePushL(stream);
	InternalizeL(stream, aDefaultLogOutput);
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

inline void TDefaultLogOutput_StateAccessor::ExternalizeL(CDefaultLogOutput* aDefaultLogOutput)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Write out the object dump
	TParse filename;
	filename.Set(KTDefaultLogOutput_DumpName(),NULL,NULL);
	RFileWriteStream stream;
	User::LeaveIfError(stream.Replace(fs,filename.FullName(), EFileWrite));
	CleanupClosePushL(stream);
	ExternalizeL(stream, aDefaultLogOutput);
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

