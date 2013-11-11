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
//

/**
 @file
 @internalComponent
*/

#include "FLOGMAN.H"
#include "FLOGSTD.H"

const TInt KLoggerGranularity=5;

/**
Literal const that hold back slash.
*/
const TText KBackSlash='\\';

/**
full path for log folder
*/
_LIT(KLogFolder,"C:\\LOGS\\");

/**
CFileLoggerManager definitions
*/

CFileLoggerManager* CFileLoggerManager::NewL()
/**
Creates a new CFileLoggerManager Object 
*/
	{

	CFileLoggerManager* r=new(ELeave) CFileLoggerManager();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CFileLoggerManager::~CFileLoggerManager()
/**
Destructor
*/
	{

	TInt i;
	TInt count=iLogger.Count();
	for (i=0; i<count; i++)
		{	
		delete iLogger[i];
		iLogger[i]=NULL;
		}

	iLogger.Delete(0,count);
	iFs.Close();
	}

CFileLoggerManager::CFileLoggerManager()
	: iLogger(KLoggerGranularity)
/**
Default Constructor 
*/
	{}

void CFileLoggerManager::ConstructL()
	{

	User::LeaveIfError(iFs.Connect());
	}

void CFileLoggerManager::FindOrCreateLogL(TLogFile& aLogFile)
/**
Searches the log file. If not found create it otherwise just
increment access count.

@param aLogFile Name of log file
*/
	{

	TInt indexFound=FindLogger(aLogFile);	
	if (indexFound!=KErrNotFound)
		{
		iLogger[indexFound]->CancelDeletionTimer();
		iLogger[indexFound]->IncrementAccessCount();
		aLogFile.SetValid(iLogger[indexFound]->LogFile().Valid());
		return;
		}

	CFileLogger* log=CFileLogger::NewL(this,aLogFile,iFs);
	CleanupStack::PushL(log);
	iLogger.AppendL(log);
	CleanupStack::Pop();
	}

void CFileLoggerManager::CloseLog(const TLogFile& aLogFile)
/**
Searches the log file, if not found return. Decrement the access
count and check for the client usage. If no client is using it 
any more, delete the log file.

@param aLogFile Name of log file
*/
	{

	TInt indexFound=FindLogger(aLogFile);
	if (indexFound==KErrNotFound)
		return;

	if (!iLogger[indexFound]->DeletionTimerActive())
		{
		iLogger[indexFound]->DecrementAccessCount();

		if (iLogger[indexFound]->AccessCount()==0)	// if no client is using it any more
			{
			TBool started=iLogger[indexFound]->StartDeletionTimer();
			if (!started)
				{
				delete iLogger[indexFound];
				iLogger.Delete(indexFound,1);
				}
			}
		}
	}

void CFileLoggerManager::WriteToLogL(const TLogFile& aLogFile, const TDesC8& aBuf)
/**
Searches and Writes to log file.

@param aLogFile Name of log file
@param aBuf Text to write
*/
	{

	TInt indexFound=FindLogger(aLogFile);
	if (indexFound==KErrNotFound)
		{
		User::Leave(KErrNotFound);
		return;
		}

	iLogger[indexFound]->WriteLogL(aBuf);
	}

void CFileLoggerManager::DeleteLogger(CFileLogger* aLogger)
	{

	TInt index=FindLogger(aLogger->LogFile());
	__ASSERT_DEBUG(index!=KErrNotFound, User::Invariant());
	if (index!=KErrNotFound)
		{
		delete iLogger[index];
		iLogger.Delete(index,1);
		}
	}

TInt CFileLoggerManager::FindLogger(const TLogFile& aLogFile) const
/**
Searches log file.
@param aLogFile Name of the log file
*/
	{
	
	TInt i;
	TInt indexFound=KErrNotFound;
	for (i=0; i<iLogger.Count(); i++)
		{
		if (iLogger[i]->LogFile()==aLogFile)
			{
			indexFound=i;
			break;
			}
		}
	return indexFound;
	}

/**
CFileLogger defintions
*/

CFileLogger* CFileLogger::NewL(CFileLoggerManager* aLoggerManager,TLogFile& aLogFile, RFs& aFs)
/**
Creates CFileLogger

*/
	{

	CFileLogger* r=new(ELeave) CFileLogger(aLoggerManager,aLogFile,aFs);
	CleanupStack::PushL(r);
	r->ConstructL(aLogFile);
	CleanupStack::Pop();
	return r;
	}

CFileLogger::~CFileLogger()
/**
Destructor
*/
	{

	__ASSERT_DEBUG(iAccessCount==0, User::Invariant());
	iFile.Close();
	delete iTimer;
	}	

CFileLogger::CFileLogger(CFileLoggerManager* aLoggerManager,TLogFile& aLogFile, RFs& aFs)
	: iLoggerManager(aLoggerManager), iFs(aFs), iLogFile(aLogFile), iAccessCount(0)
/**
Access count will be incremented when construcion is complete

*/
	{}

void CFileLogger::ConstructL(TLogFile& aLogFile)
/**
Decide whether to create a log (only create a log if the folder exists) and if 
so delete any existing one, if the mode is overwirte, append to any existing one 
if the mode is append. Open or create and the file and position to the end of it.
If an error occurs, the iValid flag will just be left as FALSE, as initialised.

@param aLogFile Name of the log file
*/
	{

	iTimer=CLoggerDeletionTimer::NewL(this);

	iLogFile.SetValid(EFalse);
	aLogFile.SetValid(EFalse);

	TFileName logFolder,logFilename;
	GetFolderAndFileNameL(logFolder,logFilename);
	
	TUint n;
	if (iFs.Att(logFolder,n)==KErrNone)		// the folder exists => do logging
		{
		TInt ret=KErrNone;
		TBool fileExists=EFalse;
		if (iLogFile.Mode()==EFileLoggingModeOverwrite)
			{
			ret=iFs.Delete(logFilename);
			if (ret!=KErrNotFound && ret!=KErrNone)
				User::Leave(ret);
			}
		else		// append
			{
			if (iFs.Att(logFilename,n)==KErrNone)
				fileExists=ETrue;
			}

		if (!fileExists)
			ret=iFile.Create(iFs,logFilename,EFileShareAny|EFileWrite);
		else
			ret=iFile.Open(iFs,logFilename,EFileShareAny|EFileWrite);
		if (ret==KErrNone)	// if we managed to open/create it
			{
			TInt pos=0;
			ret=iFile.Seek(ESeekEnd,pos);
			if (ret==KErrNone)
				{
				iLogFile.SetValid(ETrue);
				aLogFile.SetValid(ETrue);
				IncrementAccessCount();
				return;
				}
			}
		iFile.Close();
		User::Leave(ret);
		}
	IncrementAccessCount();
	}

void CFileLogger::WriteLogL(const TDesC8& aBuf)
/**
Checks for log file and then Writes the text

@param aBuf Text to write
*/
	{

	if (iLogFile.Valid())
		{
		User::LeaveIfError(iFile.Write(aBuf));
		User::LeaveIfError(iFile.Flush());
		}
	}	

TBool CFileLogger::StartDeletionTimer()
/**
Starts timer if this was a valid log file (i.e. it was open)
*/
	{

	if (!iLogFile.Valid())
		return EFalse;
	iTimer->After(KShutdownPause);
	return ETrue;
	}

void CFileLogger::CancelDeletionTimer()
/**
*/
	{

	iTimer->Cancel();
	}

void CFileLogger::DeletionTimerExpired()
/**
*/
	{

	iLoggerManager->DeleteLogger(this);
	}

void CFileLogger::GetFolderAndFileNameL(TFileName& aFolder,TFileName& aFilename) const
/**
Work out the full path of the folder and file name of the log

@param aFolder Full path of log file
@param aFilename Name of log file
*/
	{

	aFolder.Append(KLogFolder);
	if ((aFolder.MaxLength()-aFolder.Length()-iLogFile.Directory().Length()-1)>=0)
		{
		aFolder.Append(iLogFile.Directory());
		aFolder.Append(KBackSlash);
		}
	else
		User::Leave(KErrOverflow);

	if ((aFilename.MaxLength()-aFolder.Length()-iLogFile.Name().Length())>=0)
		{
		aFilename.Append(aFolder);
		aFilename.Append(iLogFile.Name());
		}
	else
		User::Leave(KErrOverflow);
	}

/**
CLoggerDeletionTimer class definitions
*/

CLoggerDeletionTimer* CLoggerDeletionTimer::NewL(CFileLogger* aLogger)
/**
Creates CLoggerDeletionTimer
*/
	{

	CLoggerDeletionTimer* r=new(ELeave) CLoggerDeletionTimer(aLogger);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CLoggerDeletionTimer::~CLoggerDeletionTimer()
/**
Destructor
*/
	{}

CLoggerDeletionTimer::CLoggerDeletionTimer(CFileLogger* aLogger)
	: CTimer(EPriorityIdle), iLogger(aLogger)
	{
	CActiveScheduler::Add(this);
	}

void CLoggerDeletionTimer::RunL()
	{
	
	iLogger->DeletionTimerExpired();
	}
