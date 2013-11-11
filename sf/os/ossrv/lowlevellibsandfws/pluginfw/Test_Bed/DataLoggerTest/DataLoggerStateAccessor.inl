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
// The implementation of the accessor class upon the CDataLogger
// 
//

// ______________________________________________________________________________
//
_LIT(KTDataLogger_DumpName,"C:\\System\\Data\\Logs\\TDataLogger_StateDump.bin");

inline TInt TDataLogger_StateAccessor::InvariantTest(TAny* aTestObject)
	{
	if(aTestObject)
		{
		CDataLogger* theDataLogger = REINTERPRET_CAST(CDataLogger*, aTestObject);
		if(	theDataLogger->iLogOutput == NULL || 
			theDataLogger->iReportOutput == NULL)
			return KTestBedFailedInvariant;
		}
	return KErrNone;
	}

inline TInt TDataLogger_StateAccessor::Internalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CDataLogger* theDataLogger = REINTERPRET_CAST(CDataLogger*, aTestObject);
		TRAP(error,InternalizeL(theDataLogger));
		}
	return error;
	}

inline TInt TDataLogger_StateAccessor::Externalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CDataLogger* theDataLogger = REINTERPRET_CAST(CDataLogger*, aTestObject);
		TRAP(error,ExternalizeL(theDataLogger));
		}
	return error;
	}

inline void TDataLogger_StateAccessor::InternalizeL(RFileReadStream& /* aStream */, CDataLogger* /* aLoadManager */)
	{
	// There is nothing to restore
	}

inline void TDataLogger_StateAccessor::ExternalizeL(RFileWriteStream& /* aStream */, CDataLogger* /* aLoadManager */)
	{
	// There is nothing to persist
	}

// internal helpers
inline void TDataLogger_StateAccessor::InternalizeL(CDataLogger* aDataLogger)
	{
	// Read in the dumped data
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Read the object dump
	TParse filename;
	filename.Set(KTDataLogger_DumpName(),NULL,NULL);
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs,filename.FullName(), EFileRead));
	CleanupClosePushL(stream);
	InternalizeL(stream, aDataLogger);
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

inline void TDataLogger_StateAccessor::ExternalizeL(CDataLogger* aDataLogger)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Write out the object dump
	TParse filename;
	filename.Set(KTDataLogger_DumpName(),NULL,NULL);
	RFileWriteStream stream;
	User::LeaveIfError(stream.Replace(fs,filename.FullName(), EFileWrite));
	CleanupClosePushL(stream);
	ExternalizeL(stream, aDataLogger);
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

