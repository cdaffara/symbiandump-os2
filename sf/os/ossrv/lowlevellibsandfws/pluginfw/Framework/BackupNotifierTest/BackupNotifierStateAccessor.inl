// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The implementation of the accessor class upon the CBackupNotifier
// 
//

// ______________________________________________________________________________
//
_LIT(KTBackupNotifier_DumpName,"C:\\System\\Data\\Logs\\TBackupNotifier_StateDump.bin");

inline TInt TBackupNotifier_StateAccessor::InvariantTest(TAny* aTestObject)
	{
	if(aTestObject)
		{
		CBackupNotifier* theBackupNotifier = REINTERPRET_CAST(CBackupNotifier*, aTestObject);
		if(theBackupNotifier->iBackupSession == NULL)
			{
			return KTestBedFailedInvariant;
			}
		}
	return KErrNone;
	}

inline TInt TBackupNotifier_StateAccessor::Internalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CBackupNotifier* theBackupNotifier = REINTERPRET_CAST(CBackupNotifier*, aTestObject);
		TRAP(error,InternalizeL(theBackupNotifier));
		}
	return error;
	}

inline TInt TBackupNotifier_StateAccessor::Externalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CBackupNotifier* theBackupNotifier = REINTERPRET_CAST(CBackupNotifier*, aTestObject);
		TRAP(error,ExternalizeL(theBackupNotifier));
		}
	return error;
	}

inline void TBackupNotifier_StateAccessor::InternalizeL(RFileReadStream& /* aStream */, CBackupNotifier* /* aBackupNotifier */)
	{
	// Has no state to restore
	}

inline void TBackupNotifier_StateAccessor::ExternalizeL(RFileWriteStream& /* aStream */, CBackupNotifier* /* aBackupNotifier */)
	{
	// Has no state to persist
	}

// internal helpers
inline void TBackupNotifier_StateAccessor::InternalizeL(CBackupNotifier* aBackupNotifier)
	{
	// Read in the dumped data
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Read the object dump
	TParse filename;
	filename.Set(KTBackupNotifier_DumpName(),NULL,NULL);
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs,filename.FullName(), EFileRead));
	CleanupClosePushL(stream);
	InternalizeL(stream, aBackupNotifier);
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

inline void TBackupNotifier_StateAccessor::ExternalizeL(CBackupNotifier* aBackupNotifier)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Write out the object dump
	TParse filename;
	filename.Set(KTBackupNotifier_DumpName(),NULL,NULL);
	RFileWriteStream stream;
	User::LeaveIfError(stream.Replace(fs,filename.FullName(), EFileWrite));
	CleanupClosePushL(stream);
	ExternalizeL(stream, aBackupNotifier);
	stream.CommitL();
	CleanupStack::PopAndDestroy(2);	// stream & fs
	}

