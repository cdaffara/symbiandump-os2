/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* TestUtil - client testutils interface implementation
*
*/


/**
 @file 
 @test
 @internalComponent
*/
 
#include "testutilclient.h"
#include "testutilclientserver.h"

static TInt StartTestUtilServer()
	{
	const TUidType serverUid(KNullUid, KNullUid, KServerUid3);
	RProcess server;
	TInt err = server.Create(KTestUtilServerImg, KNullDesC, serverUid);
	if (err != KErrNone)
		{
		return err;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat != KRequestPending)
		{
		server.Kill(0);		// abort startup
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	err = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return err;
	}

EXPORT_C TInt RTestUtilSession::Connect()
//
// Connect to the server, attempting to start it if necessary
//
	{
	TInt retry=2;
	for (;;)
		{
		TInt err = CreateSession(KTestUtilServerName, TVersion(0, 0, 0), 2);
		if (err != KErrNotFound && err != KErrServerTerminated)
			{
			return err;
			}
		if (--retry==0)
			{
			return err;
			}
		err = StartTestUtilServer();
		if (err != KErrNone && err != KErrAlreadyExists)
			{
			return err;
			}
		}
	}

EXPORT_C TInt RTestUtilSession::Copy(const TDesC& aSourceFile, const TDesC& aDestinationFile)
	{
	return SendReceive(ECopy,TIpcArgs(&aSourceFile, &aDestinationFile));
	}

EXPORT_C TInt RTestUtilSession::Move(const TDesC& aSourceFile, const TDesC& aDestinationFile)
	{
	return SendReceive(EMove,TIpcArgs(&aSourceFile, &aDestinationFile));
	}

EXPORT_C TInt RTestUtilSession::Delete(const TDesC& aFileName)
	{
	return SendReceive(EDelete,TIpcArgs(&aFileName));
	}

EXPORT_C TInt RTestUtilSession::MkDirAll(const TDesC& aFileName)
	{
	return SendReceive(EMkDirAll,TIpcArgs(&aFileName));
	}

EXPORT_C TInt RTestUtilSession::RmDir(const TDesC& aFileName)
	{
	return SendReceive(ERmDir,TIpcArgs(&aFileName));
	}

EXPORT_C TBool RTestUtilSession::FileExistsL(const TDesC& aFileName)
	{
	return FileExistsL(aFileName, 0);
	}

EXPORT_C TBool RTestUtilSession::FileExistsL(const TDesC& aFileName, TInt aMsecTimeout)
	{
	TBool fileExists;
	TPckg<TBool> exists(fileExists);
	User::LeaveIfError(SendReceive(EFileExists,TIpcArgs(&aFileName, aMsecTimeout, &exists)));
	return fileExists;
	}
EXPORT_C TInt RTestUtilSession::FormatDrive(TInt aDrive, TBool aFormatFatTableOnly)
	{
	return SendReceive(EFormat,TIpcArgs(aDrive, aFormatFatTableOnly));
	}
EXPORT_C TInt RTestUtilSession::MountDrive(TInt aDrive)
	{
	return SendReceive(EMount,TIpcArgs(aDrive));
	}
EXPORT_C TInt RTestUtilSession::UnMountDrive(TInt aDrive)
	{
	return SendReceive(EUnMount,TIpcArgs(aDrive));
	}
	
EXPORT_C TInt RTestUtilSession::Lock(const TDesC& aFileName)
	{
	return SendReceive(ELock,TIpcArgs(&aFileName));
	}
	
EXPORT_C TInt RTestUtilSession::Unlock(const TDesC& aFileName)
	{
	return SendReceive(EUnlock,TIpcArgs(&aFileName));
	}

EXPORT_C TInt RTestUtilSession::SetReadOnly(const TDesC& aFileName, TInt aSetReadOnly)
	{
	return SendReceive(ESetReadOnly,TIpcArgs(&aFileName, aSetReadOnly));
	}
EXPORT_C TInt RTestUtilSession::GetFileHandle(const TDesC& aFileName, RFile &aRFile)
	{
	TPckgBuf<TInt> fh;
	TInt fsh = SendReceive(EGetFileHandle, TIpcArgs(&aFileName, &fh));
	if(fsh < 0)
		{
		return fsh;
		}
	return aRFile.AdoptFromServer(fsh, fh());
	}
EXPORT_C void RTestUtilSession::WatchFile(const TDesC& aFileName, TRequestStatus& aStatus)
	{
	aStatus=KRequestPending;
	SendReceive(EWatchFile, TIpcArgs(&aFileName), aStatus);
	}
EXPORT_C void RTestUtilSession::WatchFileCancelL()
	{
	User::LeaveIfError(SendReceive(EWatchFileCancel));
	}
EXPORT_C TInt RTestUtilSession::GetNumFilesL(const TDesC& aDirName)
	{
	TInt numFiles;
	TPckg<TInt> getNum(numFiles);
	
	User::LeaveIfError(SendReceive(EGetNumFiles, TIpcArgs(&aDirName, &getNum)));
	
	return numFiles;
	}

EXPORT_C TInt RTestUtilSession::SetSecureClock(TInt aTimeOffset)
	{
	return SendReceive(ESetSecureClock,TIpcArgs(aTimeOffset));
	}

// End of file
