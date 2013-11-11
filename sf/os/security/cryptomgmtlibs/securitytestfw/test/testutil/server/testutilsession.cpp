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
* TestUtil - server implementation
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#include "testutilserver.h"
#include "testutilsession.h"
#include "testutilclientserver.h"

// Timer implementation
CGenericTimer* CGenericTimer::NewL(MTimeoutClient& aClient)
	{
	CGenericTimer* self = new(ELeave) CGenericTimer(aClient);
	CleanupStack::PushL(self);
	self->ConstructL(); // calls CTimer::Construct
	CleanupStack::Pop(self);
	return self;
	}

CGenericTimer::CGenericTimer(MTimeoutClient& aClient)
	: CTimer(-1), iClient(aClient)
	{
	CActiveScheduler::Add(this);
	}

void CGenericTimer::RunL()
	{
	// When the timeout expires, then call the client's handler
	iClient.HandleTimeout();
	};

// file detector implementation
CTestFileDetector* CTestFileDetector::NewL(const RMessage2& aMessage, RFs& aFs)
	{
	CTestFileDetector* self = new (ELeave) CTestFileDetector(aMessage, aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTestFileDetector::CTestFileDetector(const RMessage2& aMessage, RFs& aFs)
	:CActive(EPriorityNormal), iFs(aFs), iMessage(aMessage)
	{
	iTimeInterval = iMessage.Int1();
	CActiveScheduler::Add(this);
	}

CTestFileDetector::~CTestFileDetector()
	{
	Cancel();
	delete iTimer;
	delete iFileName;
	}

void CTestFileDetector::ConstructL()
	{
	if (iTimeInterval!=0)
		{
		iTimer=CGenericTimer::NewL(*this);
		}
	iFileName = CTestUtilSessionCommon::AllocateInputBufferLC(iMessage, 0);
	CleanupStack::Pop();
	}

void CTestFileDetector::DetectFile()
	{
	if (!iTimer)
		{
		CheckAndComplete();
		}
	else
		{
		TEntry entry;
		TInt err=iFs.Entry(iFileName->Des(), entry);
		if (err == KErrNone)
			{
			TPckgC<TBool> exists(ETrue);
			iMessage.WriteL(2, exists);
			iMessage.Complete(KErrNone);
			}
		else
			{
			iTimer->After(iTimeInterval*1000);
			iFs.NotifyChange(ENotifyFile, 
							iStatus, 
							iFileName->Des());
			SetActive();			
			}
		}
	}

void CTestFileDetector::RunL()
	{
	if (iTimer)
		{
		iTimer->Cancel();
		}
	CheckAndComplete();
	}

void CTestFileDetector::DoCancel()
	{
	iFs.NotifyChangeCancel(iStatus);
	}

void CTestFileDetector::HandleTimeout()
	{
	Cancel();
	CheckAndComplete();
	}

void CTestFileDetector::CheckAndComplete()
	{
	TEntry entry;
	TInt err=iFs.Entry(iFileName->Des(), entry);
	if (err == KErrNone)
		{
		TPckgC<TBool> exists(ETrue);
		iMessage.WriteL(2, exists);
		iMessage.Complete(KErrNone);
		}
	else if (err == KErrNotFound 
			|| err == KErrPathNotFound
			|| err == KErrNotReady 
			|| err == KErrCorrupt)
		{
		TPckgC<TBool> exists(EFalse);
		iMessage.WriteL(2, exists);
		iMessage.Complete(KErrNone);
		}
	else
		{
		iMessage.Complete(err);
		}
	}

// CTestUtilSession Implementation
CTestUtilSession::CTestUtilSession()
	{
	}

CTestUtilSession::~CTestUtilSession()
	{
	Server().DropSession();
	for (TInt i = 0;i < iLockedFileHandles.Count(); i++)
		{
		iLockedFileHandles[i].Close();
		}
	iLockedFileHandles.Close();
	
	delete iFileWatcher;
	delete iDetector;
	}
	
void CTestUtilSession::CreateL()
 	{
	Server().AddSession();
  	}

_LIT(KBP, ":\\");
_LIT(KFAT,"Fat");
	
void CTestUtilSession::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
		case ECopy:
			{
			HBufC* source = CTestUtilSessionCommon::AllocateInputBufferLC(aMessage,0);
			HBufC* destination = CTestUtilSessionCommon::AllocateInputBufferLC(aMessage,1);

			TInt err = Server().FileMan().Copy(*source, *destination, CFileMan::ERecurse | CFileMan::EOverWrite);
			if (err == KErrNone)
				{
				// Turn off the read only attributes
				TTime time(0); // must specify 0, or a valid time, otherwise sets time to a random value and causes -6/-21 errors
				err = Server().FileMan().Attribs(*destination, 0, KEntryAttReadOnly, time, CFileMan::ERecurse);
				}
			
			CleanupStack::PopAndDestroy(destination);
			CleanupStack::PopAndDestroy(source);

			aMessage.Complete(err);
			break;
			}
		case EMove:
			{
			HBufC* source = CTestUtilSessionCommon::AllocateInputBufferLC(aMessage,0);
			HBufC* destination = CTestUtilSessionCommon::AllocateInputBufferLC(aMessage,1);

			TInt err = Server().FS().Rename(*source,*destination);
			if (err == KErrNone)
				{
				// Turn off the read only attributes
				TTime time(0); // must specify 0, or a valid time, otherwise sets time to a random value and causes -6/-21 errors
				err = Server().FileMan().Attribs(*destination, 0, KEntryAttReadOnly, time, CFileMan::ERecurse);
				}
			
			CleanupStack::PopAndDestroy(destination);
			CleanupStack::PopAndDestroy(source);
			
			aMessage.Complete(err);
			break;
			}
		case EDelete:
			{
			HBufC* fileName = CTestUtilSessionCommon::AllocateInputBufferLC(aMessage,0);
			TEntry entry;
			TInt err = Server().FS().Entry(*fileName, entry);
			if (err == KErrNone)
				{
				if (entry.IsDir())
					{
					TPath pathName(*fileName);
					if (pathName[pathName.Length() - 1] != KPathDelimiter)
						{
						pathName.Append(KPathDelimiter);
						}
					err = Server().FileMan().RmDir(pathName);
					}
				else
					{
					err = Server().FS().Delete(*fileName);
					}
				}
			CleanupStack::PopAndDestroy(fileName);
			
			aMessage.Complete(err);
			break;
			}
		case ERmDir:
			{
			HBufC* fileName = CTestUtilSessionCommon::AllocateInputBufferLC(aMessage,0);
			TParsePtrC parsePtr(*fileName);
			if(parsePtr.IsRoot())
				{
				User::Leave(KErrAccessDenied);
				}
			TInt err = Server().FileMan().RmDir(*fileName);
			CleanupStack::PopAndDestroy(fileName);
			
			aMessage.Complete(err);
			break;
			}
		case EMkDirAll:
			{
			HBufC* fileName = CTestUtilSessionCommon::AllocateInputBufferLC(aMessage,0);
			TInt err = Server().FS().MkDirAll(*fileName);
			CleanupStack::PopAndDestroy(fileName);
			
			aMessage.Complete(err);
			break;
			}
		case EFileExists:
			{			
			delete iDetector;
			iDetector=CTestFileDetector::NewL(aMessage,
												Server().FS());			
			iDetector->DetectFile();
			break;
			}
		case ELock:
			{
			HBufC* fileName = CTestUtilSessionCommon::AllocateInputBufferLC(aMessage,0);
			RFile lockFile;
			TInt err = lockFile.Open(Server().FS(), *fileName, EFileWrite);
			if (err == KErrNone)
				iLockedFileHandles.Append(lockFile);

			CleanupStack::PopAndDestroy(fileName);			
			aMessage.Complete(err);
			break;
			}			
		case EUnlock:
			{
			HBufC* fileName = CTestUtilSessionCommon::AllocateInputBufferLC(aMessage,0);
			TInt err = KErrNotFound;
			TFileName lockedFileName;
			for (TInt i = 0; i < iLockedFileHandles.Count() && err;i++)
				{
				TInt err2 = iLockedFileHandles[i].FullName(lockedFileName);
				User::LeaveIfError(err2);
				if (lockedFileName.MatchF(*fileName) != KErrNotFound)
					{
					iLockedFileHandles[i].Close();
					iLockedFileHandles.Remove(i);
					err = KErrNone;
					}
				}
			CleanupStack::PopAndDestroy(fileName);			
			aMessage.Complete(err);
			break;
			}
			case EFormat:
			{
			TInt drive = aMessage.Int0();
			TBool formatFatOnly = aMessage.Int1();
			TChar aDriveChar;
			User::LeaveIfError(Server().FS().DriveToChar(drive, aDriveChar));
			TBuf<3> bfDrv;
			bfDrv.Append(aDriveChar);
			bfDrv.Append(KBP);
	
			RFormat format;
			TInt count;
			User::LeaveIfError(format.Open(Server().FS(), bfDrv, EHighDensity, count));
			CleanupClosePushL(format);
			
			if (formatFatOnly)
   				{
   				User::LeaveIfError(format.Next(count));
   				}
			else
				{
				while (count > 0)
					{
					User::LeaveIfError(format.Next(count));
					}
				}
				
			CleanupStack::PopAndDestroy(&format);
			aMessage.Complete(KErrNone);
			break;
			}
		case EMount:
			{
			TInt drive = aMessage.Int0();
			User::LeaveIfError(Server().FS().Connect());
			//Mount the drive synchronizely to make sure the drive is ready for the next operation
			User::LeaveIfError(Server().FS().MountFileSystem(KFAT, drive, ETrue));
			aMessage.Complete(KErrNone);
			break;
			}
		case EUnMount:
			{
			TInt drive = aMessage.Int0();
			TFileName fsName;
			User::LeaveIfError(Server().FS().FileSystemName(fsName, drive));
			User::LeaveIfError(Server().FS().DismountFileSystem(fsName, drive));
			aMessage.Complete(KErrNone);
			break;
			}
		case ESetReadOnly:
			{
			HBufC* fileName = CTestUtilSessionCommon::AllocateInputBufferLC(aMessage,0);
			TInt setReadOnly = aMessage.Int1();
			TUint setmask;
			TUint clearmask;
			if (setReadOnly)
				{
				// Setting read only attribute
				setmask = KEntryAttReadOnly;
				clearmask = 0;
				}
			else
				{
				// Clearing read only attribute
				setmask = 0;
				clearmask = KEntryAttReadOnly;				
				}
			
			// Turn off the read only attributes
			TTime time(0);
			TInt err = Server().FileMan().Attribs(*fileName, setmask, clearmask, time);
			CleanupStack::PopAndDestroy(fileName);			
			aMessage.Complete(err);
			break;
			}
		case EGetFileHandle:
			{
			HBufC* fileName = CTestUtilSessionCommon::AllocateInputBufferLC(aMessage,0); 
			RFile file;
			CleanupClosePushL(file);
			User::LeaveIfError(file.Open(Server().FS(), *fileName, EFileRead | EFileShareReadersOnly));
			User::LeaveIfError(file.TransferToClient(aMessage, 1));
			CleanupStack::PopAndDestroy(2, fileName); // file
			break;
			}
		case EWatchFile:
			{
			if (iFileWatcher)
				{
				if (iFileWatcher->IsActive())
					{
					aMessage.Complete(KErrServerBusy);
					break;
					}
				else
					{
					delete iFileWatcher;
					iFileWatcher = NULL;
					}
				}
			// Create a new file watcher for this session
			iFileWatcher = CFileWatcher::NewL(Server().FS(), aMessage);
			break;
			}
		case EWatchFileCancel:
			{
			if (iFileWatcher)
				{
				iFileWatcher->Cancel();
				aMessage.Complete(KErrNone);
				}
			else
				{
				// No file watch request to cancel!
				aMessage.Complete(KErrNotReady);
				}
			break;
			}
		case EGetNumFiles:
			{
			HBufC* dirPath = CTestUtilSessionCommon::AllocateInputBufferLC(aMessage,0);
			CDir* dirContents = NULL;
			
			User::LeaveIfError(Server().FS().GetDir(*dirPath, KEntryAttNormal, ESortNone, dirContents));
			TPckg<TInt> numFiles(dirContents->Count());
			
			delete dirContents;
			aMessage.WriteL(1, numFiles);
			aMessage.Complete(KErrNone);
			CleanupStack::PopAndDestroy(dirPath);
			break;
			}
		case ESetSecureClock:
			{
			TTime currentTime(0);
			currentTime.UniversalTime();
			TTimeIntervalSeconds increment(aMessage.Int0());
			currentTime += increment;
			User::SetUTCTimeSecure(currentTime);
			aMessage.Complete(KErrNone);

			}
			break;
		default:
			{
			PanicClient(aMessage,EPanicIllegalFunction);
			break;
			}
		}
	}

void CTestUtilSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	if (aError==KErrBadDescriptor)
		PanicClient(aMessage,EPanicBadDescriptor);
	CSession2::ServiceError(aMessage,aError);
	}
// End of file
