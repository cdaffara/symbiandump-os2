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

#include <babackup.h>
#include <bafl/backup_std.h>
#include <e32std.h>
#include <e32base.h>
#include <baksrv.h>
#include "Baksrvs.h"
#include <e32math.h>
#include <e32svr.h>
#include <baflpan.h>

#define UNUSED_VAR(a) a = a

const TUid KServerUid3={0x10004900};
const TInt KBADefaultPriority = CActive::EPriorityUserInput;
_LIT(KBackupSrvName,"baksrvs");

//
// class RBaBackupSession
//

const TInt KNumConnectRetries	=10;


class RBaBackupSession : public RSessionBase
	{
public:
	TInt Connect();
	void RegisterForNotifications(TRequestStatus& aStatus) const;
	void DeregisterForNotifications() const;
	void GetEvent(TDes& aFileName,MBackupObserver::TFileLockFlags& aFileFlag) const;
	void CloseAllFiles(MBackupObserver::TFileLockFlags aFlags,TRequestStatus& aStatus) const;
	void RestartApps() const;
	TInt CloseFile(const TDesC& aFileName,MBackupObserver::TFileLockFlags aFlags) const;
	void RestartFile(const TDesC& aFileName) const;
	TInt NotifyChangeFileLock(const TDesC& aFileName) const;
	void NotifyChangeFileLockCancel(const TDesC& aFileName) const;
	void CloseServer() const;
	void NotifyBackupOperation(const TBackupOperationAttributes& aBackupOperationAttributes);
	TBool IsBackupOperationRunning() const;
	void BackupOperationEventReady(TRequestStatus& aStatus, TPckgBuf<TBackupOperationAttributes>& aBackupOperationAttributes) const;
	void CancelOutstandingEventForBackupOperation() const;
	void GetBackupOperationEvent(TBackupOperationAttributes& aBackupOperationAttributes) const;
	void SetBackupOperationObserverIsPresent(TBool aObserverIsPresent) const;
private:
	TInt StartServer();
	};

TInt RBaBackupSession::Connect()
	{
	TInt err=KErrNone;
	TInt retry=KNumConnectRetries;
	FOREVER
		{
		err=CreateSession(__BACKUP_SERVER_NAME_V2,TVersion(KBakServMajorVN,KBakServMinorVN,KBakServBuildVN),KBakServMessageSlots);
		if ((--retry>0) && ((err==KErrNotFound) || (err==KErrServerTerminated)))
			{
			err = StartServer();
			if ((err!=KErrNone) && (err!=KErrAlreadyExists))
				{
				break;
				}
			}
		else
			{
			break;
			}
		}
	return err;
	}

TInt RBaBackupSession::StartServer()
	{
	const TUidType serverUid(KNullUid,KNullUid,KServerUid3);
	TInt error=KErrNone;
	RProcess server;
	error = server.Create(KBackupSrvName,KNullDesC,serverUid);
	if(error!=KErrNone)
		return error;
 	TRequestStatus stat;
	server.Rendezvous(stat);
 	if (stat!=KRequestPending)
 		server.Kill(0);		// abort startup
 	else
 		server.Resume();	// logon OK - start the server
 	User::WaitForRequest(stat);		// wait for start or death
 	// we can't use the 'exit reason' if the server panicked as this
 	// is the panic 'reason' and may be '0' which cannot be distinguished
 	// from KErrNone
 	error=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return error;
	}


void RBaBackupSession::RegisterForNotifications(TRequestStatus& aStatus) const
	{
	SendReceive(EBakOpCodeEventReady,aStatus);
	}

void RBaBackupSession::DeregisterForNotifications() const
	{
	SendReceive(EBakOpCodeStopNotifications);
	}

void RBaBackupSession::GetEvent(TDes& aFileName,MBackupObserver::TFileLockFlags& aFileFlag) const
	{
	TBuf<KMaxFileName+1> buf;
	if (SendReceive(EBakOpCodeGetEvent,TIpcArgs(&buf))!=KErrServerTerminated)
		{
		TBuf<1> num=buf.Left(1);
		buf.Delete(0,1);
		aFileName=buf;
		aFileFlag=(MBackupObserver::TFileLockFlags)(num[0]-'0');
		}
	}

void RBaBackupSession::CloseAllFiles(MBackupObserver::TFileLockFlags aFlags,TRequestStatus& aStatus) const
	{
	SendReceive(EBakOpCodeCloseAllFiles,TIpcArgs(aFlags),aStatus);
	}

void RBaBackupSession::RestartApps() const
	{
	SendReceive(EBakOpCodeRestartAll);
	}

TInt RBaBackupSession::CloseFile(const TDesC& aFileName,MBackupObserver::TFileLockFlags aFlags) const
	{
	const TInt err=SendReceive(EBakOpCodeCloseFile,TIpcArgs(aFileName.Length(),&aFileName,aFlags));
	return err;
	}

void RBaBackupSession::RestartFile(const TDesC& aFileName) const
	{
	SendReceive(EBakOpCodeRestartFile,TIpcArgs(aFileName.Length(),&aFileName));
	}

TInt RBaBackupSession::NotifyChangeFileLock(const TDesC& aFileName) const
	{
	return SendReceive(EBakOpCodeNotifyLockChange,TIpcArgs(aFileName.Length(),&aFileName));
	}

void RBaBackupSession::NotifyChangeFileLockCancel(const TDesC& aFileName) const
	{
	SendReceive(EBakOpCodeNotifyLockChangeCancel,TIpcArgs(aFileName.Length(),&aFileName));
	}

void RBaBackupSession::CloseServer() const
	{
	Send(EBakOpCodeCloseServer);
	}

void RBaBackupSession::NotifyBackupOperation(const TBackupOperationAttributes& aBackupOperationAttributes)
	{
	TPckgC<TBackupOperationAttributes> backupOpAttPkg(aBackupOperationAttributes);
	SendReceive(EBakOpCodeNotifyBackupOperation, TIpcArgs(&backupOpAttPkg));
	}

void RBaBackupSession::CancelOutstandingEventForBackupOperation() const 
	{
	SendReceive(EBakOpCodeCancelOutstandingBackupOperationEvent);
	}

TBool RBaBackupSession::IsBackupOperationRunning() const
	{
	TBool isRunning=EFalse;
	TPckg<TBool> pkg(isRunning);
	SendReceive(EBakOpCodeGetBackupOperationState, TIpcArgs(&pkg));
	return pkg();
	}

void RBaBackupSession::BackupOperationEventReady(TRequestStatus& aStatus, TPckgBuf<TBackupOperationAttributes>& aBackupOperationAttributes) const
	{
	SendReceive(EBakOpCodeBackupOperationEventReady,TIpcArgs(&aBackupOperationAttributes),aStatus);
	}

void RBaBackupSession::GetBackupOperationEvent(TBackupOperationAttributes& aBackupOperationAttributes) const
	{
	TPckg<TBackupOperationAttributes> backupOpAttPkg(aBackupOperationAttributes);
	SendReceive(EBakOpCodeGetBackupOperationEvent, TIpcArgs(&backupOpAttPkg));
	}

void RBaBackupSession::SetBackupOperationObserverIsPresent(TBool aObserverIsPresent) const
	{
	Send(EBakOpCodeSetBackupOperationObserverIsPresent, TIpcArgs(aObserverIsPresent));
	}

//
// class CBaLockChangeNotifier
//

NONSHARABLE_CLASS(CBaLockChangeNotifier) : public CActive
	{
public:
	static CBaLockChangeNotifier* NewL(RBaBackupSession& aBackupSession);
	~CBaLockChangeNotifier();
	void AddL(const TDesC& aFileName, MBackupObserver& aObserver);
	void Remove(const TDesC& aFileName);
protected:
	void StartNotifications();
	void StopNotifications();
private: // from CActive
	void DoCancel();
	void RunL();
private:
	CBaLockChangeNotifier(RBaBackupSession& aBackupSession);
	void DoRunL();
	TInt Find(const TDesC& aFileName) const;
private:
	class TFileItem
		{
	public:
		TFileItem(HBufC* aFile,MBackupObserver& aObserver);
	public:
		HBufC* iFile;
		MBackupObserver& iObserver;
		};
private:
	RBaBackupSession& iBackupSession;
	RArray<TFileItem> iFileItems;
	};

CBaLockChangeNotifier::TFileItem::TFileItem(HBufC* aFile,MBackupObserver& aObserver)
	:	iFile(aFile), 
		iObserver(aObserver)
	{}

CBaLockChangeNotifier* CBaLockChangeNotifier::NewL(RBaBackupSession& aBackupSession)
	{ // static
	CBaLockChangeNotifier* self=new(ELeave) CBaLockChangeNotifier(aBackupSession);
	CActiveScheduler::Add(self);
	return self;
	}

CBaLockChangeNotifier::~CBaLockChangeNotifier()
	{
	Cancel();

	const TInt count=iFileItems.Count();
	for (TInt ii=0;ii<count;ii++)
		{
		delete iFileItems[ii].iFile;
		}
	iFileItems.Close();
	}

void CBaLockChangeNotifier::StartNotifications()
	{
	if(!IsActive())
		{	
		iBackupSession.RegisterForNotifications(iStatus);
		SetActive();
		}
	}

void CBaLockChangeNotifier:: StopNotifications()
	{
	const TInt count=iFileItems.Count();
	
	if(count==0)
		{
		Cancel();
		}
	}

void CBaLockChangeNotifier::AddL(const TDesC& aFileName, MBackupObserver& aObserver)
	{
	StartNotifications();

	HBufC* file=aFileName.AllocLC();
	TFileItem fileItem(file,aObserver);
	User::LeaveIfError(iFileItems.Append(fileItem));
	CleanupStack::Pop(); // file
	const TInt err=iBackupSession.NotifyChangeFileLock(aFileName);
	if (err!=KErrNone)
		{
		delete file;
		iFileItems.Remove(iFileItems.Count()-1);
		iFileItems.Compress();
		User::Leave(err);
		}
	}

void CBaLockChangeNotifier::Remove(const TDesC& aFileName)
	{
	const TInt index=Find(aFileName);
	if (index!=KErrNotFound)
		{
		const TFileItem& fileItem=iFileItems[index];
		iBackupSession.NotifyChangeFileLockCancel(*fileItem.iFile);
		delete fileItem.iFile;
		iFileItems.Remove(index);
		iFileItems.Compress();
		}

	StopNotifications();
	}


void CBaLockChangeNotifier::DoCancel()
	{
	const TInt count=iFileItems.Count();	

	// release the locks on all outstandng files in this session
  	for (TInt ii=0;ii<count;ii++)		
  		{
  		const TFileItem& fileItem=iFileItems[ii];
  		iBackupSession.NotifyChangeFileLockCancel(*fileItem.iFile);
  		}								
	iBackupSession.DeregisterForNotifications();
	}

void CBaLockChangeNotifier::RunL()
	{
	TRAPD(err,DoRunL());
	if (err!=KErrServerTerminated)
		{
		StartNotifications();
		}
	User::LeaveIfError(err);
	}

CBaLockChangeNotifier::CBaLockChangeNotifier(RBaBackupSession& aBackupSession)
	: CActive(KBADefaultPriority), iBackupSession(aBackupSession)
	{}


void CBaLockChangeNotifier::DoRunL()
	{
	const TInt status=iStatus.Int();

	if (status<0)
		{
		User::Leave(status);
		}
	TFileName fileName;
	MBackupObserver::TFileLockFlags fileFlag;
	iBackupSession.GetEvent(fileName,fileFlag);
	TInt err=KErrNone;
	const TInt count=iFileItems.Count();
	for (TInt ii=0;ii<count;ii++)
		{
		const TFileItem& fileItem=iFileItems[ii];
		if (fileItem.iFile->MatchF(fileName)==0)
			{
			TRAPD(r,fileItem.iObserver.ChangeFileLockL(*fileItem.iFile,fileFlag));
			if (r!=KErrNone && err==KErrNone)
				{
				err=r;
				}
			}
		}
	User::LeaveIfError(err);
	}

TInt CBaLockChangeNotifier::Find(const TDesC& aFileName) const
	{
	TInt index=KErrNotFound;
	const TInt count=iFileItems.Count();
	for (TInt ii=0;ii<count;ii++)
		{
		const TFileItem& fileItem=iFileItems[ii];
		if (*fileItem.iFile==aFileName)
			{
			index=ii;
			break;
			}
		}
	return index;
	}

//
// class CBaBackupOperationNotifier
//

NONSHARABLE_CLASS(CBaBackupOperationNotifier) : public CActive
	{
public:
	static CBaBackupOperationNotifier* NewL(RBaBackupSession& aBackupSession);
	~CBaBackupOperationNotifier();
	void AddBackupOperationObserverL(MBackupOperationObserver& aBackupSession);
	void RemoveBackupOperationObserver(MBackupOperationObserver& aBackupSession);
private: // from CActive
	void DoCancel();
	void RunL();
private:
	CBaBackupOperationNotifier(RBaBackupSession& aBackupSession);
	void Queue();
	void DoRunL();
private:
	RPointerArray<MBackupOperationObserver> iObservers;
	RBaBackupSession& iBackupSession;
	TPckgBuf<TBackupOperationAttributes> iBackupOperationAttributes;
	};


CBaBackupOperationNotifier* CBaBackupOperationNotifier::NewL(RBaBackupSession& aBackupSession)
	{ // static
	CBaBackupOperationNotifier* self=new(ELeave) CBaBackupOperationNotifier(aBackupSession);
	CActiveScheduler::Add(self);
	return self;
	}

CBaBackupOperationNotifier::~CBaBackupOperationNotifier()
	{
	Cancel();
	iObservers.Reset();
	iObservers.Close();
	}

void CBaBackupOperationNotifier::AddBackupOperationObserverL(MBackupOperationObserver& aBackupOperationObserver)
	{
	const TInt index = iObservers.Find(&aBackupOperationObserver);
	if (index == KErrNotFound)
		{
		User::LeaveIfError(iObservers.Append(&aBackupOperationObserver));
		if (iBackupSession.IsBackupOperationRunning())
			{
			TBackupOperationAttributes backupOperationAttributes;
			iBackupSession.GetBackupOperationEvent(backupOperationAttributes);
			aBackupOperationObserver.HandleBackupOperationEventL(backupOperationAttributes);
			}
		}
	Queue();
	}

void CBaBackupOperationNotifier::RemoveBackupOperationObserver(MBackupOperationObserver& aBackupOperationObserver)
	{
	const TInt index = iObservers.Find(&aBackupOperationObserver);
	if (index != KErrNotFound)
		{
		iObservers.Remove(index);
		if (iObservers.Count() == 0)
			{
			iBackupSession.CancelOutstandingEventForBackupOperation();
			iBackupSession.SetBackupOperationObserverIsPresent(EFalse);
			}
		}
	}

void CBaBackupOperationNotifier::DoCancel()
	{
	TBackupOperationAttributes backupOperationAttributes;
	iBackupSession.GetBackupOperationEvent(backupOperationAttributes);
	const TInt count = iObservers.Count();
	for (TInt index=0; index<count; ++index)
		{
		// TRAP and ignore the errCode
		TRAPD(errCode, iObservers[index]->HandleBackupOperationEventL(backupOperationAttributes));
        UNUSED_VAR(errCode);
		}
	iBackupSession.CancelOutstandingEventForBackupOperation();
	}

void CBaBackupOperationNotifier::RunL()
	{
	const TInt status=iStatus.Int();
	if (status!=KErrCancel && iObservers.Count()>0)
		{
		TRAPD(err,DoRunL());
		if (err!=KErrServerTerminated)
			{
			Queue();
			}
		User::LeaveIfError(err);
		}
	}

CBaBackupOperationNotifier::CBaBackupOperationNotifier(RBaBackupSession& aBackupSession)
	: CActive(EPriorityStandard), iBackupSession(aBackupSession)
	{}

void CBaBackupOperationNotifier::Queue()
	{
	if (!IsActive())
		{
		iStatus=KRequestPending;
		SetActive();
		iBackupSession.SetBackupOperationObserverIsPresent(ETrue);
		iBackupSession.BackupOperationEventReady(iStatus, iBackupOperationAttributes);
		}
	}

void CBaBackupOperationNotifier::DoRunL()
	{
	const TInt status=iStatus.Int();
	if (status<0)
		{
		User::Leave(status);
		}
	const TInt count = iObservers.Count();
	for (TInt index=0; index<count; ++index)
		{
		iObservers[index]->HandleBackupOperationEventL(iBackupOperationAttributes());
		}
	}

//
// class CBaBackupSessionWrapper
//

/**
 * Returns a newly created CBaBackupSessionWrapper, passing ownership immediately
 */
EXPORT_C CBaBackupSessionWrapper* CBaBackupSessionWrapper::NewL()
	{ // static
	CBaBackupSessionWrapper* self=new(ELeave) CBaBackupSessionWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

/**
 * D'tor.  Any files or apps that have been closed will be restarted
 */
EXPORT_C CBaBackupSessionWrapper::~CBaBackupSessionWrapper()
	{
	delete iLockChangeNotifier;
	delete iBackupOperationNotifier;
	if (iBackupSession)
		{
		iBackupSession->Close();
		delete iBackupSession;
		}
	}

/**
Register the specified file to the server. The given observer will be called back when 
the lock state of the file should be modified

@param aFileName the name of the file to be observed.
@param aObserver the observer which will be called back when the lock state of the file should be modified.

@leave KErrServerBusy if the server is busy with the other client or under CloseAll operation. KErrNoMemory 
if not enough memory to register this file.
*/
EXPORT_C void CBaBackupSessionWrapper::RegisterFileL(const TDesC& aFileName,MBackupObserver& aObserver)
	{
	if (!iLockChangeNotifier)
 		{
 		iLockChangeNotifier=CBaLockChangeNotifier::NewL(*iBackupSession);
 		}

	__ASSERT_ALWAYS(iLockChangeNotifier, Panic(EBafPanicNullPointer));
	iLockChangeNotifier->AddL(aFileName,aObserver);
	}

/**
 * Stop sending this client requests to alter the lock state of aFileName
 */
EXPORT_C void CBaBackupSessionWrapper::DeregisterFile(const TDesC& aFileName)
	{
	if(iLockChangeNotifier)
		iLockChangeNotifier->Remove(aFileName);
	}

/**
 Closes all non-system apps and signal all registered files to have their locks altered according to aFlags.
 Returns immediately before having finished all processing.  aStatus will be completed when everything is
 closed or closing has completed.  Possible error codes are
 		KErrNoMemory		- Not enough memory to signal all apps/files to close
 		KErrServerBusy	- Another client has some files closed - No-one else should attempt any backup operation
 		KErrLocked			- Not all apps were successfully closed
 In all cases, an undefined number of apps/files may have been closed and backup may still be possible although
 install/restore operations should not be attempted.
  
 @param aFlags the file lock state to request the other clients with.
 @param aStatus the request status to be completed when all files have been closed.
 @publishedPartner
 @released
 @capability WriteDeviceData
 */
EXPORT_C void CBaBackupSessionWrapper::CloseAll(MBackupObserver::TFileLockFlags aFlags,TRequestStatus& aStatus)
	{
	iBackupSession->CloseAllFiles(aFlags,aStatus);
	}

/**
 * Opposite of CloseAll.  Can safely be called nothing has been closed by this client
 * 
 * @publishedPartner
 * @released
 * @capability WriteDeviceData
 */
EXPORT_C void CBaBackupSessionWrapper::RestartAll()
	{
	iBackupSession->RestartApps();
	}

/**
 Close or reduce use of aFileName depending on the state of aFlags.  
  
 @param aFileName the name of the file to be closed / changed the file lock state.
 @param aFlag the file lock state to change to.
 @leave KErrNoMemory if not enough memory to signal the files to close. KErrServerIsBusy if another client 
 has some files being closed.
 @publishedPartner
 @released
 @capability WriteDeviceData
 */
EXPORT_C void CBaBackupSessionWrapper::CloseFileL(const TDesC& aFileName,MBackupObserver::TFileLockFlags aFlags)
	{
	User::LeaveIfError(iBackupSession->CloseFile(aFileName,aFlags));
	}

/**
 * Opposite of CloseFileL.  Can be safely called if CloseFileL hasn't been called before
 * 
 * @publishedPartner
 * @released
 * @capability WriteDeviceData
 */
EXPORT_C void CBaBackupSessionWrapper::RestartFile(const TDesC& aFileName)
	{
	iBackupSession->RestartFile(aFileName);
	}

CBaBackupSessionWrapper::CBaBackupSessionWrapper()
	{}

void CBaBackupSessionWrapper::ConstructL()
	{
	iBackupSession=new(ELeave) RBaBackupSession();
	User::LeaveIfError(iBackupSession->Connect());
	}


/**
 * Registers the observer aBackupOperationObserver for getting notifications whether a backup or
 * restore operation starts or ends.
 * 
 * @since App-Framework_6.2
 */
EXPORT_C void CBaBackupSessionWrapper::RegisterBackupOperationObserverL(MBackupOperationObserver& aBackupOperationObserver)
	{
	if (!iBackupOperationNotifier)
		{
		iBackupOperationNotifier=CBaBackupOperationNotifier::NewL(*iBackupSession);
		}
	iBackupOperationNotifier->AddBackupOperationObserverL(aBackupOperationObserver);
	}

/**
 * De-registers the observer aBackupOperationObserver for getting notifications whether a backup or
 * restore operation starts or ends.
 *
 * @since App-Framework_6.2
 */
EXPORT_C void CBaBackupSessionWrapper::DeRegisterBackupOperationObserver(MBackupOperationObserver& aBackupOperationObserver)
	{
	if (iBackupOperationNotifier)
		{
		iBackupOperationNotifier->RemoveBackupOperationObserver(aBackupOperationObserver);
		}
	}

/**
 * Returns ETrue when either a backup or restore operation is running, otherwise it retunrs EFalse.
 *
 * @since App-Framework_6.2
 */
EXPORT_C TBool CBaBackupSessionWrapper::IsBackupOperationRunning() const
	{
	return iBackupSession->IsBackupOperationRunning();
	}

/**
 * Notifies the server that a backup operation is going to happen.
 *
 * @since App-Framework_6.2
 * @publishedPartner
 * @released
 * @capability WriteDeviceData
 */
EXPORT_C void CBaBackupSessionWrapper::NotifyBackupOperationL(const TBackupOperationAttributes& aBackupOperationAttributes)
	{
	iBackupSession->NotifyBackupOperation(aBackupOperationAttributes);
	}


EXPORT_C void MBackupOperationObserver::Reserved1()
	{
	}
