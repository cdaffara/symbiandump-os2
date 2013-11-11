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

#include <baksrv.h>
#include "Baksrvs.h"
#include <babackup.h>
#include <bafl/backup_std.h>
#include <basched.h>
#include <bsul/clientmessage.h>
#include "patchdata.h"

extern const BSUL::TClientMessageServerData KServerData;

const TInt KBakServMaxOperationTimerLoops = 3; // Number of iterations for base operation timer

_LIT(KPanic,"BackupServer");
//
// RMessage::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
//
void PanicClient(const RMessagePtr2& aMessage, TInt aPanic)
	{
	aMessage.Panic(KPanic,aPanic);
	}


//
// class CShutdownServer
//

NONSHARABLE_CLASS(CShutdownServer) : public CTimer
	{
	enum {KMyShutdownDelay=0x2000000};	// approx 2s
public:
	inline CShutdownServer();
	inline void ConstructL();
	inline void Start();
private:
	void RunL();
	};

inline CShutdownServer::CShutdownServer()
	:CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}

inline void CShutdownServer::ConstructL()
	{
	CTimer::ConstructL();
	}
inline void CShutdownServer::Start()
	{
	After(KMyShutdownDelay);
	}

//
// Initiate server exit when the timer expires
//
void CShutdownServer::RunL()
	{
	CActiveScheduler::Stop();
	}


//
// class CBaServBackupScheduler
//

EXPORT_C CBaServBackupScheduler::CBaServBackupScheduler()
{
}

EXPORT_C CBaServBackupScheduler::~CBaServBackupScheduler()
{
}

/**
 *
 * Set the error handler to aErrorHandler to the current scheduler.
 *
 * @param     "CBaServBackupSession* aErrorHandler"
 *            The handler session.
 */
EXPORT_C void CBaServBackupScheduler::SetErrorHandler(CBaServBackupSession* aErrorHandler)
	{
	iErrorHandler=aErrorHandler;
	}

/**
 *
 * Handles the error aError.
 *
 * @param     "TInt aError"
 *            The error.
 */
EXPORT_C void CBaServBackupScheduler::Error(TInt aError) const
	{
	if (iErrorHandler)
		{
		iErrorHandler->HandleError(aError);
		}
	else if (aError!=KLeaveWithoutAlert)
		{
		// Handling the in this way implies that ServiceL should not leave.
		CActiveScheduler::Error(aError);
		}
	}

/**
Class CBaServBackupSession::CReRegistrationTimer
*/
NONSHARABLE_CLASS(CBaServBackupSession::CReRegistrationTimer) : public CPeriodic
{
	public:
		static CReRegistrationTimer* NewL(TInt aPriority);
		static TInt ReRegistrationTimerCallBack(TAny* aPtr);
		
	protected:
		TInt RunError(TInt aError);
		
	private:
		CReRegistrationTimer(TInt aPriority);
		void HandleReRegistrationTimerCallBack();

    public:
		CBaBackupServer* iBackupServer;
};

/**
Class CBaBackupServer::CBaServCloseAllOperationTimer
*/

NONSHARABLE_CLASS(CBaBackupServer::CBaServCloseAllOperationTimer): public CPeriodic
{
	public:
	    static CBaServCloseAllOperationTimer* NewL(CBaBackupServer* aBackupServer);
	   void ConstructL(CBaBackupServer* aBackupServer);
	   ~CBaServCloseAllOperationTimer();
	   void SetMessage(const RMessagePtr2& aPtr);
	   RMessagePtr2 Message();
	   TInt GetOperationCount();
	   void SetOperationCount(TInt aCount);
	   static TInt OperationTimerCallBack(TAny* aPtr);
	protected:
	   TInt RunError(TInt aError);
	private:
	   void HandleOperationTimerCallBack();
	   CBaServCloseAllOperationTimer();
	private:
	   RMessagePtr2 iCloseAllFilesMessage;
	   TInt16 iOperationCount;
	   CBaBackupServer* iBackupServer;
};

//
// class CBaBackupServer
//

/**
 *
 * Returns a pointer to <code>CBaBackupServer</code> object.
 *
 * @return   "CBaBackupServer"
 *            A newly-constructed backup server object.
 */
EXPORT_C CBaBackupServer* CBaBackupServer::NewL()
	{ // static
	CBaBackupServer* self=new(ELeave) CBaBackupServer(CActive::EPriorityStandard);
	return self;
	}

/**
 *
 * Constructor.
 *
 * @param     "TInt aPriority"
 *            The active object priority.
 */
EXPORT_C CBaBackupServer::CBaBackupServer(TInt aPriority)
	: CServer2(aPriority)
	{}
/**
 *
 * Destructor.
 *
 */
EXPORT_C CBaBackupServer::~CBaBackupServer()
	{
	TDblQueIter<CSession2> iter(iSessionIter);
	iter.SetToFirst();

	for (CSession2* session=iter++; session!=NULL; session=iter++)
		{
		delete session;
		}
	delete iShutdown;
	delete iExtension;
	delete iCloseAllFilesOperationTimer;
	}

/**
 *
 * Completes the server construction by adding the server to the active scheduler and creating
 * a <code>CShutdownServer</code> object.
 *
 */
EXPORT_C void CBaBackupServer::ConstructL()
	{
	StartL(__BACKUP_SERVER_NAME_V2);
	iExtension = new(ELeave) CBaBackupServerExt();
	iShutdown = new (ELeave) CShutdownServer;
	iShutdown->ConstructL();
	// ensure that the server still exits even if the 1st client fails to connect
	iShutdown->Start();
	iRegisteredFilesCount = 0;
	iCloseAllFilesOperationTimer = CBaBackupServer::CBaServCloseAllOperationTimer::NewL(this);
	iCloseAllOperationRunning = EFalse;
	
	//initialise the client message framework
	BSUL::CClientMessage::InitialiseFrameworkL(KServerData);
	}

/**
 *
 * Sets the server to be busy with the aUniqueClientId client.
 *
 * @param     "TUint32 aUniqueClientId"
 *            A unique client identifier.
 */
EXPORT_C void CBaBackupServer::SetBusy(TUint32 aUniqueClientId)
	{
	iExtension->iUniqueBusyClientId=aUniqueClientId;
	}

/**
 *
 * Returns <code>ETrue</code> if the client using the server is not the one identified by aUniqueClientId
 * oterwise <code>EFalse</code>
 *
 * @param     "TUint32 aUniqueClientId"
 *            A unique client identifier.
 */
EXPORT_C TBool CBaBackupServer::IsOtherClientBusy(TUint32 aUniqueClientId) const
	{
	return (iExtension->iUniqueBusyClientId!=0 && iExtension->iUniqueBusyClientId!=aUniqueClientId);
	}

/**
 *
 * Returns <code>ETrue</code> if the client using the server corresponds has aUniqueClientId as unique id
 * oterwise <code>EFalse</code>
 *
 * @param     "TUint32 aUniqueClientId"
 *            A unique client identifier id.
 */
EXPORT_C TBool CBaBackupServer::IsClientBusy(TUint32 aUniqueClientId) const
	{
	return ((iExtension->iUniqueBusyClientId==aUniqueClientId) || 
			((iExtension->iCachedBusyClientId != 0) && (iExtension->iCachedBusyClientId == aUniqueClientId))); 
	}

/**
 *
 * Handles the closing of all files by signaling the new file lock flag, aFlag, to all client.
 *
 * @param     "MBackupObserver::TFileLockFlags aFlag"
 *            A file lock flag.
 */
EXPORT_C void CBaBackupServer::CloseAllFilesL(MBackupObserver::TFileLockFlags aFlag)
	{
	// Set the close all operation flag and the reregistration counter to zero
	iCloseAllOperationRunning = ETrue;
	iSessionLockReRegistrationCount = 0;
	
	// cleanup that calls RestartAll if required
	TDblQueIter<CSession2> iter(iSessionIter);
	iter.SetToFirst();

	for (CSession2* session=iter++; session!=NULL; session=iter++)
		{
		static_cast<CBaServBackupSession*>(session)->SignalReleaseAllFileLocksL(aFlag);
		}
	}

/**
 *
 * Signals to all clients of getting the lock of their respective files.
 *
 */
EXPORT_C void CBaBackupServer::RestartAll()
	{
	TDblQueIter<CSession2> iter(iSessionIter);
	iter.SetToFirst();

	for (CSession2* session=iter++; session!=NULL; session=iter++)
		{
		static_cast<CBaServBackupSession*>(session)->SignalRetakeAllFileLocks();
		}
	}

/**
 *
 * Allows any subclass of <code>CBaBackupServer</code> of completing the closing of files.
 * The server will lose the ownership of aClosedFiles object, which implies it is up to this virtual
 * function to deal with it in order to avoid a memory leak.
 *
 * @param     "CArrayFix<CBaServBackupSession::TClosedFile>* aClosedFiles"
 *            An array of closed files.
 */
EXPORT_C void CBaBackupServer::CompleteClosingFiles(CArrayFix<CBaServBackupSession::TClosedFile>* aClosedFiles)
	{
	delete aClosedFiles;
	}

/**
 *
 * Allows application framework backup code to check if all registerd files have beem updated
 *
 */
EXPORT_C TBool CBaBackupServer::HaveAllCloseAllFilesClientsReRegistered()
	{
	return((iRegisteredFilesCount == iSessionLockReRegistrationCount) ? ETrue : EFalse);
	}

void CBaBackupServer::CloseFileL(MBackupObserver::TFileLockFlags aFlag,const TDesC& aFileName)
	{
	TDblQueIter<CSession2> iter(iSessionIter);
	iter.SetToFirst();

	for (CSession2* session=iter++; session!=NULL; session=iter++)
		{
		static_cast<CBaServBackupSession*>(session)->SignalReleaseFileLockL(aFlag,aFileName);
		}
	}

void CBaBackupServer::RestartFile(const TDesC& aFileName)
	{
	TDblQueIter<CSession2> iter(iSessionIter);
	iter.SetToFirst();

	for (CSession2* session=iter++; session!=NULL; session=iter++)
		{
		static_cast<CBaServBackupSession*>(session)->SignalRetakeFileLocks(aFileName);
		}
	}

/**
 *
 * Creates a server-side client <code>CBaServBackupSession</code> session object by checking first if
 * the version of the server is compatible with the client.
 *
 * @param     "const TVersion &aVersion"
 *            Version information supplied by the client.
 */
EXPORT_C CSession2* CBaBackupServer::NewSessionL(const TVersion &aVersion, const RMessage2& /*aMessage*/) const
	{
	TVersion ver(KBakServMajorVN,KBakServMinorVN,KBakServBuildVN);
	if (!User::QueryVersionSupported(ver,aVersion))
		User::Leave(KErrNotSupported);
//
	return CBaServBackupSession::NewL();
	}

/**
 *
 * Signals to all clients the aBackupOperationAttributes.
 *
 * @param     "const TBackupOperationAttributes& aBackupOperationAttributes"
 *            Backup operation attributes.
 */
EXPORT_C void CBaBackupServer::SignalBackupOperation(const TBackupOperationAttributes& aBackupOperationAttributes)
	{
	TDblQueIter<CSession2> iter(iSessionIter);
	iter.SetToFirst();

	for (CSession2* session=iter++; session!=NULL; session=iter++)
		{
		static_cast<CBaServBackupSession*>(session)->SignalBackupOperation(aBackupOperationAttributes);
		}
	// Cache session id if starting backup
	if (aBackupOperationAttributes.iOperation == MBackupOperationObserver::EStart)
			iExtension->iCachedBusyClientId = iExtension->iUniqueBusyClientId;
	}

void CBaBackupServer::AddSession()
	{
	iShutdown->Cancel();
	++iSessionCount;
	}

void CBaBackupServer::DropSession()
	{
	if (--iSessionCount==0)
		{
		iShutdown->Start();
		}
	}

/*
 * Check to see if a CloseAll opertation is in progress
 * 
 */
TBool CBaBackupServer::IsCloseAllOperationRunning()
	{
	return iCloseAllOperationRunning;
	}

/*
 * Set value of IsCloseAllOperationRunning
 * 
 */
EXPORT_C void CBaBackupServer::SetCloseAllOperationRunningState(TBool aRunning)
	{
	iCloseAllOperationRunning = aRunning;
	}

/**
 *
 * This timer will only be created in cases of no derived backup server
 *
 */
void CBaBackupServer::StartCloseAllFilesOperationTimer(const RMessagePtr2& aPtr)
	{
	// Store the CloseAll message in case we need to Complete on a timeout
	iCloseAllFilesOperationTimer->SetMessage(aPtr);
	
	// If hardware use patchable constant if not default to 3 seconds
	iCloseAllFilesOperationTimer->Start(KBaBackupCloseallFilesTimeout, KBaBackupCloseallFilesTimeout, TCallBack(CBaBackupServer::CBaServCloseAllOperationTimer::OperationTimerCallBack, iCloseAllFilesOperationTimer));
	}

/**
 * Handle an error from CBaServBackupSession::ServiceL()
 * A bad descriptor error implies a badly programmed client, so panic it;
 * otherwise report the error to the client
 *
 * @param     "TInt aError"
 *            The error.
 */
EXPORT_C TInt CBaBackupServer::RunError(TInt aError)
	{
	if (aError==KErrBadDescriptor)
		{
		PanicClient(Message(), EBufferOverflow);
		}
	else
		{
		Message().Complete(aError);
		}
	//
	// The leave will result in an early return from CServer::RunL(), skipping
	// the call to request another message. So do that now in order to keep the
	// server running.
	ReStart();
	return KErrNone;	// handled the error fully
	}

//
// class CBaBackupServer::CBaServCloseAllOperationTimer
//

/**
@internalComponent
*/
CBaBackupServer::CBaServCloseAllOperationTimer* CBaBackupServer::CBaServCloseAllOperationTimer::NewL(CBaBackupServer* aBackupServer)
	{ // static
	CBaBackupServer::CBaServCloseAllOperationTimer* self=new(ELeave) CBaBackupServer::CBaServCloseAllOperationTimer();
	CleanupStack::PushL(self);
	self->ConstructL(aBackupServer);
	CleanupStack::Pop(); // self
	CActiveScheduler::Add(self);
	return self;
	}

/**
@internalComponent
*/

void CBaBackupServer::CBaServCloseAllOperationTimer::ConstructL(CBaBackupServer* aBackupServer)
	{
	CTimer::ConstructL();
	iBackupServer = aBackupServer;
	iOperationCount = 0;
	}
/**
@internalComponent
*/

TInt CBaBackupServer::CBaServCloseAllOperationTimer::GetOperationCount()
{
	return iOperationCount;	
}

/**
@internalComponent
*/

void CBaBackupServer::CBaServCloseAllOperationTimer::SetOperationCount(TInt aCount)
{
	iOperationCount = aCount;
}

/**
@internalComponent
*/

TInt CBaBackupServer::CBaServCloseAllOperationTimer::RunError(TInt aError)
	{
	if (aError==KLeaveWithoutAlert)
		{
		return KErrNone;
		}
	return aError;
	}
	
/**
@internalComponent
*/
CBaBackupServer::CBaServCloseAllOperationTimer::CBaServCloseAllOperationTimer()
	: CPeriodic(CActive::EPriorityStandard)
	{ ;	}
	
/**
@internalComponent
*/
CBaBackupServer::CBaServCloseAllOperationTimer::~CBaServCloseAllOperationTimer()
	{
	if (IsActive())
		Cancel();	
	}

/**
@internalComponent
*/
void CBaBackupServer::CBaServCloseAllOperationTimer::SetMessage(const RMessagePtr2& aPtr)
	{
	iCloseAllFilesMessage = aPtr;
	}

/**
@internalComponent
*/
RMessagePtr2 CBaBackupServer::CBaServCloseAllOperationTimer::Message()
	{
	return iCloseAllFilesMessage;
	}
	
/**
@internalComponent
*/
TInt CBaBackupServer::CBaServCloseAllOperationTimer::OperationTimerCallBack(TAny* aPtr)
	{ // static
	TRAP_IGNORE(REINTERPRET_CAST(CBaBackupServer::CBaServCloseAllOperationTimer*,aPtr)->HandleOperationTimerCallBack());
	return 0;
	}

/**
@internalComponent
*/
void CBaBackupServer::CBaServCloseAllOperationTimer::HandleOperationTimerCallBack()
	{
	TBool finished = iBackupServer->HaveAllCloseAllFilesClientsReRegistered();
	TInt retCode;
	if (finished || (iOperationCount == KBakServMaxOperationTimerLoops))
		{
		if (finished)
			{
			retCode = KErrNone;
			}
		else
			{
			retCode = KErrLocked;
			}
		
		// One way or another CloseAll is finished at this point
		iBackupServer->SetCloseAllOperationRunningState(EFalse);
		
		iCloseAllFilesMessage.Complete(retCode);
		Cancel();
		}
	else
		{
		iOperationCount++;
		if (IsActive())
			{
			Cancel();

			// If hardware use patchable constant if not default to 3 seconds
			Start(KBaBackupCloseallFilesTimeout, KBaBackupCloseallFilesTimeout, TCallBack(CBaBackupServer::CBaServCloseAllOperationTimer::OperationTimerCallBack,this));
			}
		}
	}

//
// class CBaServBackupMessageQueue
//

inline CBaServBackupMessageQueue::TQueueItem::TQueueItem(HBufC* aFileName,TInt aOperation)
	: iFileName(aFileName), iOperation(aOperation)
	{}

CBaServBackupMessageQueue* CBaServBackupMessageQueue::NewL()
	{ // static
	CBaServBackupMessageQueue* self=new(ELeave) CBaServBackupMessageQueue();
	return self;
	}

CBaServBackupMessageQueue::~CBaServBackupMessageQueue()
	{
	const TInt count=iQueue.Count();
	for (TInt ii=0;ii<count;ii++)
		{
		delete iQueue[ii].iFileName;
		}
	iQueue.Reset();
	iQueue.Close();
	}

void CBaServBackupMessageQueue::AddItemL(const TDesC& aFileName,MBackupObserver::TFileLockFlags aFlag)
	{
	const TInt count=iQueue.Count();
	for (TInt ii=0;ii<count;ii++)
		{
		TQueueItem& item=iQueue[ii];
		if(aFileName.MatchF(*item.iFileName) == KErrNone)
			{
			if (item.iOperation==MBackupObserver::EReleaseLockReadOnly && aFlag==MBackupObserver::EReleaseLockNoAccess)
				{
				item.iOperation=MBackupObserver::EReleaseLockNoAccess;
				}
			else if (item.iOperation!=MBackupObserver::EReleaseLockNoAccess)
				{
				HBufC* file=aFileName.AllocLC();
				TQueueItem item(file,aFlag);
				User::LeaveIfError(iQueue.Insert(item,ii));
				CleanupStack::Pop(); // file
				}
			return;
			}
		}
	HBufC* file=aFileName.AllocLC();
	HBufC* copy=aFileName.AllocLC();
	TQueueItem item(file,aFlag);
	User::LeaveIfError(iQueue.Append(item));
	item=TQueueItem(copy,-1);
	const TInt err=iQueue.Append(item);
	if (err!=KErrNone)
		{
		iQueue.Remove(iQueue.Count()-1);
		User::Leave(err);
		}
	CleanupStack::Pop(2); // copy, file
	}

void CBaServBackupMessageQueue::AddItem(const TDesC& aFileName)
	{
	const TInt count=iQueue.Count();
	for (TInt ii=0;ii<count;ii++)
		{
		TQueueItem& item=iQueue[ii];
		if (aFileName.MatchF(*item.iFileName) == KErrNone && item.iOperation==-1)
			{
			item.iOperation=0;
			break;
			}
		}
	}

TBool CBaServBackupMessageQueue::IsEmpty() const
	{
	return HeadIndex()==KErrNotFound;
	}

void CBaServBackupMessageQueue::RemoveHead()
	{
	const TInt index=HeadIndex();
	if (index!=KErrNotFound)
		{
		delete iQueue[index].iFileName;
		iQueue.Remove(index);
		iQueue.Compress();
		}
	}

void CBaServBackupMessageQueue::RemoveItem(const TDesC& aFileName)
	{
	const TInt count=iQueue.Count();
	for (TInt ii=count-1;ii>=0;ii--)
		{
		const TQueueItem& item=iQueue[ii];
		if (aFileName.MatchF(*item.iFileName) == KErrNone)
			{
			delete item.iFileName;
			iQueue.Remove(ii);
			iQueue.Compress();
			}
		}
	}

void CBaServBackupMessageQueue::GetHead(TDes& aFileName,MBackupObserver::TFileLockFlags& aFlag) const
	{
	aFileName.Zero();
	const TInt index=HeadIndex();
	if (index!=KErrNotFound)
		{
		const TQueueItem& item=iQueue[index];
		aFileName=item.iFileName->Des();
		aFlag=(MBackupObserver::TFileLockFlags)item.iOperation;
		}
	}

TInt CBaServBackupMessageQueue::HeadIndex() const
	{
	TInt index=KErrNotFound;
	const TInt count=iQueue.Count();
	for (TInt ii=0;ii<count;ii++)
		{
		const TQueueItem& item=iQueue[ii];
		if (item.iOperation!=ENoOp)
			{
			index=ii;
			break;
			}
		}
	return index;
	}

//
// class CBaServBackupSession
//

CBaServBackupSession* CBaServBackupSession::NewL()
	{ // static
	CBaServBackupSession* self=new(ELeave) CBaServBackupSession();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

/**
 *
 * Constructor.
 *
 * @param     "RThread aClient"
 *            The client thread for which this server-side client session is being constructed.
 */
EXPORT_C CBaServBackupSession::CBaServBackupSession()
	: CSession2()
	{
	iUniqueClientId = (TUint32)this;
	}

/**
 *
 * Destructor.
 *
 */
EXPORT_C CBaServBackupSession::~CBaServBackupSession()
	{
	CBaBackupServer* server=BackupServer();
	if (server->IsClientBusy(iUniqueClientId))
		{
		RestartAll();
		if (server->IsBackupOperationRunning())
			{
			TBackupOperationAttributes backupOpAtt(MBackupObserver::ETakeLock, MBackupOperationObserver::EAbort);
			server->SignalBackupOperation(backupOpAtt);
			server->SetBackupOperationRunning(EFalse);
			}
		server->SetBusy(0);
		}

	if (iFileLockObservers)
		{
		server->DecrementRegisteredFilesCount(iFileLockObservers->Count());	
		}

	delete iClosedFiles;
	delete iFileLockObservers;
	delete iReleasedFiles;
	delete iMessageQueue;
	delete iReRegistrationTimer;
	iReRegistrationTimer = 0;
	server->DropSession();
	}

/**
 *
 * Completes the session construction by creating a <code>CBaServBackupMessageQueue</code> object.
 *
 */
EXPORT_C void CBaServBackupSession::ConstructL()
	{
	iMessageQueue=CBaServBackupMessageQueue::NewL();

	// Cannot set iBackupServer in CReRegistrationTimer at this point but must be done before call to Start
	iReRegistrationTimer=CBaServBackupSession::CReRegistrationTimer::NewL(CActive::EPriorityStandard);
	}

/**
 *
 * Completes the construction of this <code>CBaServBackupSession</code> session by just doing a base call class.
 * It also notify the server that a new session has been created.
 *
 * @param		"const CServer& aServer"
 *				The server active object which is responsible for this session
 */
EXPORT_C void CBaServBackupSession::CreateL()
	{
	BackupServer()->AddSession();
	}

void CBaServBackupSession::SignalReleaseAllFileLocksL(MBackupObserver::TFileLockFlags aFlag)
	{
	CBaBackupServer* server=BackupServer();
	if (iFileLockObservers)
		{
		const TInt count=iFileLockObservers->Count();
		for (TInt ii=0;ii<count;ii++)
			{
			TFileName name=(*iFileLockObservers)[ii];
			server->CloseFileL(aFlag,name);
			}
		}
	}

void CBaServBackupSession::SignalRetakeAllFileLocks()
	{
//	CBaBackupServer* server=BackupServer();
	if (iReleasedFiles)
		{
		const TInt count=iReleasedFiles->Count();
		for (TInt ii=count-1;ii>=0;ii--)
			{
			TFileName name=(*iReleasedFiles)[ii];
//			server->RestartFile(name);
			SignalRetakeFileLocks(name);
			}
		}
	}

LOCAL_C void CleanupCloseFail(TAny* aPtr)
	{
	CDesCArray* array=REINTERPRET_CAST(CDesCArray*,aPtr);
	array->Delete(array->Count()-1);
	}

void CBaServBackupSession::SignalReleaseFileLockL(MBackupObserver::TFileLockFlags aFlag,const TDesC& aFileName)
	{
	TInt pos;
	if (iFileLockObservers && iFileLockObservers->Find(aFileName,pos)==KErrNone)
		{
		if (iReleasedFiles==NULL)
			{
			iReleasedFiles=new(ELeave) CDesCArraySeg(1);
			}
		const TBool addedReleasedFile=iReleasedFiles->Find(aFileName,pos)!=KErrNone;
		if (addedReleasedFile)
			{
			iReleasedFiles->AppendL(aFileName);
			CleanupStack::PushL(TCleanupItem(CleanupCloseFail,iReleasedFiles));
			}
		iMessageQueue->AddItemL(aFileName,aFlag);
		if (!iNotificationPullMsg.IsNull())
  			{
  			iNotificationPullMsg.Complete(KErrNone);

  			// Okay - if this is part of a CloseAll operation we need to kick off a ReRegistration timer
  			if (BackupServer()->IsCloseAllOperationRunning())
  				{
  				// Set backup server for the timer
  				iReRegistrationTimer->iBackupServer = BackupServer();

  				// If hardware use patchable constant if not default to 3 seconds
  				iReRegistrationTimer->Start(KBaBackupFileLockReRegistrationTimeout, KBaBackupFileLockReRegistrationTimeout, TCallBack(CBaServBackupSession::CReRegistrationTimer::ReRegistrationTimerCallBack, iReRegistrationTimer));  				
  				}

  			}
		if (addedReleasedFile)
			{
			CleanupStack::Pop(); // CleanupCloseFail
			}
		}
	}

void CBaServBackupSession::SignalRetakeFileLocks(const TDesC& aFileName)
	{
	if (iReleasedFiles)
		{
		const TInt count=iReleasedFiles->Count();
		for (TInt ii=count-1;ii>=0;ii--)
			{
			TFileName name=(*iReleasedFiles)[ii];
			if (name.MatchF(aFileName)==0)
				{
				iMessageQueue->AddItem(aFileName);
				if (!iNotificationPullMsg.IsNull())
  					{
  					iNotificationPullMsg.Complete(KErrNone);
  					}
				iReleasedFiles->Delete(ii);
				}
			}
		if (iReleasedFiles->Count()==0)
			{
			delete iReleasedFiles;
			iReleasedFiles=NULL;
			if (iClosedFiles==NULL && BackupServer()->IsClientBusy(iUniqueClientId))
				{
				BackupServer()->SetBusy(0);
				}
			}
		}
	}

/**
 *
 * Allows the session to handle the error.
 *
 * @param		"TInt aError"
 *				The error.
 */
EXPORT_C void CBaServBackupSession::HandleError(TInt aError)
	{
	if (aError==KLeaveWithoutAlert)
		{
		CBaServBackupScheduler::Current()->SetErrorHandler(NULL);
		}
	}

void CBaServBackupSession::DoServiceL(TCompletionType& aCompleteType)
	{
	switch (iClientMessage->Function())
		{
	case EBakOpCodeEventReady:
		{
		HandleEventReadyL();
		aCompleteType = ECompleteAsync;
		}
		break;
	case EBakOpCodeStopNotifications:
		{
		StopNotifications();
		}
		break;
	case EBakOpCodeGetEvent:
		GetEventL();
		break;
	case EBakOpCodeCloseAllFiles:
		{
		aCompleteType = CloseAllFilesL(iClientMessage->Message());
		}
		break;
	case EBakOpCodeRestartAll:
		RestartAll();
		break;
	case EBakOpCodeCloseFile:
		CloseFileL();
		break;
	case EBakOpCodeRestartFile:
		RestartFileL();
		break;
	case EBakOpCodeNotifyLockChange:
		NotifyLockChangeL();
		break;
	case EBakOpCodeNotifyLockChangeCancel:
		NotifyLockChangeCancelL();
		break;
	case EBakOpCodeNotifyBackupOperation:
		NotifyBackupOperationL();
		break;
	case EBakOpCodeCancelOutstandingBackupOperationEvent:
		{
		iBackupOperationObserverPresent = EFalse;
		if (!iBackupOperationMessagePtr.IsNull())
			{
			GetBackupOperationEventL(iBackupOperationMessagePtr);
			iBackupOperationMessagePtr.Complete(KErrCancel);
			}
		}
		break;
	case EBakOpCodeGetBackupOperationState:
		GetBackupOperationStateL();
		break;
	case EBakOpCodeBackupOperationEventReady:
		BackupOperationEventReadyL();
		aCompleteType = ECompleteAsync;
		break;
	case EBakOpCodeGetBackupOperationEvent:
		GetBackupOperationEventL(iClientMessage->Message());
		break;
	case EBakOpCodeSetBackupOperationObserverIsPresent:
		{
		iBackupOperationObserverPresent = iClientMessage->GetIntL(0);
		}
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	}


/**
 *
 * Handles the servicing of client requests passed to the backup server.
 *
 * @param		"const RMessage& aMessage"
 *				The message containing the client request.
 */
EXPORT_C void CBaServBackupSession::ServiceL(const RMessage2& aMessage)
	{
	
	TCompletionType completionType = ECompleteSync;
	
	BSUL::CClientMessage* clientMessage = 0;
	clientMessage = BSUL::CClientMessage::NewL(aMessage);
	
	//Push iClientMessage onto the cleanupstack. Although an instance variable, 
	//the lifetime of the object is contained to this function so it needs to
	//be pushed and popped here as it is not deleted in the destructor
	CleanupStack::PushL(clientMessage);
	
	//Validate the message
	TRAPD(error, clientMessage->ValidateL());
	
	iClientMessage = clientMessage;
	
	if(error == KErrNone)	
		{
		TRAP(error, DoServiceL(completionType));
		}
	
	if(completionType == ECompleteSync)
		{
		if (error==KLeaveWithoutAlert)
			{
			CBaServBackupScheduler::Current()->SetErrorHandler(NULL);
			}
		else
			{
			iClientMessage->CompleteRequestL(error);
			}
		}
	
	//Pop and destroy message
	CleanupStack::PopAndDestroy(clientMessage);
	clientMessage = NULL;
	iClientMessage = NULL;
	}

void CBaServBackupSession::HandleEventReadyL()
	{
	if (iReRegistrationTimer->IsActive())
		{ // If timer is still active (ie not timed out) we need to increment the counter and cancel the timer
		BackupServer()->IncrementFilesReRegistrationCount();	
		iReRegistrationTimer->Cancel();
		}
		// else timer is already inactive because it's expired
	
	if (iMessageQueue->IsEmpty())
		{
		iNotificationPullMsg = iClientMessage->Message();
		}	
	else
		{
		iClientMessage->CompleteRequestL(KErrNone);
		}
	}

const TInt KEventFileNameOffset=1;

void CBaServBackupSession::GetEventL()
	{
	TFileName fileName;
	MBackupObserver::TFileLockFlags fileFlag;
	if(!iMessageQueue->IsEmpty())
		{
		iMessageQueue->GetHead(fileName,fileFlag);
		TBuf<KEventFileNameOffset> num;
		num.Num((TInt)fileFlag);
		
		iClientMessage->WriteL(0,num,0);
		
		iClientMessage->WriteL(0,fileName,KEventFileNameOffset);
	
		iMessageQueue->RemoveHead();	
		}
	else
		{
		iClientMessage->PanicClient(KPanic,ENoEventToFetch);
		User::Leave(KLeaveWithoutAlert);
		}
	}

void CBaServBackupSession::CleanupCloseAllFiles(TAny* aPtr)
	{ // static
	CBaServBackupSession* self=REINTERPRET_CAST(CBaServBackupSession*,aPtr);
	delete self->iClosedFiles;
	self->iClosedFiles=NULL;
	self->BackupServer()->RestartAll();
	}

/**
 Asks the server to close all files. This function may leave in case the server is busy.
 If the requesting client is the current busy client and it has already requested CloseAll,
 this request will be ignored.
 
 @param	aMessage The reference to the message containing the client request: file lock.
 @leave KErrServerBusy if the requesting client is not the busy client. Plus other system-wide 
 errors.
 */
EXPORT_C TCompletionType CBaServBackupSession::CloseAllFilesL(const RMessage2& aMessage)
	{
	// Raise file lock notifications
	TRAPD(err,DoCloseAllFilesL(aMessage));
	
	if (err == KErrNone)
		{
		// Start timer to check all file locks re-registered before completing message
		BackupServer()->StartCloseAllFilesOperationTimer(aMessage);
		}
	else
		{
		// If the error is that the same client has already requested CloseAll, ignore this request
		// If not this error, recover the CloseAllOperationRunningState flag and leave with the error.
		if (err != KErrAlreadyExists) 
			{
			BackupServer()->SetCloseAllOperationRunningState(EFalse);
			User::Leave(err);
			}
		}

	return ECompleteAsync;
	}

void CBaBackupServer::IncrementRegisteredFilesCount()
	{
	iRegisteredFilesCount++;
	}

void CBaBackupServer::DecrementRegisteredFilesCount(TInt aNumberFiles)
	{
	iRegisteredFilesCount = iRegisteredFilesCount - aNumberFiles;
	}

void CBaBackupServer::IncrementFilesReRegistrationCount()
	{
	iSessionLockReRegistrationCount++;
	}

/**
 Function called by both base and derived backup sessions in order to start asynchronous 
 file lock notifications.
 The requesting client is set to the current busy client.
 
 @leave KErrServerBusy if the requesting client is not the current busy client or the server
 is under CloseAll operation. KErrAlreadyExists if the same client has sent CloseAll request.
 Plus other system-wide errors.
 */
 
EXPORT_C TCompletionType CBaServBackupSession::DoCloseAllFilesL(const RMessage2& /*aMessage*/)
	{
	CBaBackupServer* server=BackupServer();
	if (server->IsOtherClientBusy(iUniqueClientId))
		{
		User::Leave(KErrServerBusy);
		}
		
	if (BackupServer()->IsCloseAllOperationRunning())
		{
		User::Leave(KErrAlreadyExists);
		}		
		
	CleanupStack::PushL(TCleanupItem(CleanupCloseAllFiles,this));
	if (iClosedFiles)
		{
		delete iClosedFiles;
		iClosedFiles=NULL;
		}
	server->SetBusy(iUniqueClientId);
	
	MBackupObserver::TFileLockFlags fileFlag=
		(MBackupObserver::TFileLockFlags)iClientMessage->GetIntL(0);
	server->CloseAllFilesL(fileFlag);
	
	iClosedFiles=new(ELeave) CArrayFixSeg<CBaServBackupSession::TClosedFile>(1);
	CBaServBackupScheduler::Current()->SetErrorHandler(this);
	CleanupStack::Pop(); // CleanupCloseAllFiles
	return ECompleteAsync;
	}

/**
 *
 * Asks the server to signal all clients of getting the lock of their respective files.
 *
 */
EXPORT_C void CBaServBackupSession::RestartAll()
	{
	CBaBackupServer* server=BackupServer();	
	if (server->IsClientBusy(iUniqueClientId))
		{
		DoRestartAll();
		}
	}

void CBaServBackupSession::DoRestartAll()
	{
	CBaBackupServer* server=BackupServer();	
	server->RestartAll();
	delete iReleasedFiles;
	iReleasedFiles=NULL;
	if (iClosedFiles)
		{
		CArrayFix<CBaServBackupSession::TClosedFile>* closedFiles=iClosedFiles;
		iClosedFiles=NULL;
		server->CompleteClosingFiles(closedFiles); // takes ownership of closedFiles immediately
		}
	}

/**
Handle the client's request to close a file.
The request will be ignored if the requesting client the current busy client and the server is
under CloseAll operation.

@leave KErrServerBusy if the server is busy with the other client, plus other system-wide errors.
*/
void CBaServBackupSession::CloseFileL()
	{
	CBaBackupServer* server=BackupServer();
	if (server->IsOtherClientBusy(iUniqueClientId))
		{
		User::Leave(KErrServerBusy);
		}

	if (! server->IsCloseAllOperationRunning())
		{
		MBackupObserver::TFileLockFlags flag=
			static_cast<MBackupObserver::TFileLockFlags>(iClientMessage->GetIntL(2));
		TFileName fileName;
		GetFileNameL(fileName);
		server->SetBusy(iUniqueClientId);
		server->CloseFileL(flag,fileName);
		}
	}

void CBaServBackupSession::RestartFileL()
	{
	TFileName fileName;
	GetFileNameL(fileName);
	BackupServer()->RestartFile(fileName);
	}

/**
Handles the client's request of notification of a file lock change.
  
@leave KErrServerBusy if the server is under CloseAll operation, KLeaveWithoutAlert if the requested 
 file has been registered. Plus other system-wide errors.
*/
void CBaServBackupSession::NotifyLockChangeL()
	{
	if(BackupServer()->IsCloseAllOperationRunning())
		{
		User::Leave(KErrServerBusy);
		}
		
	TFileName fileName;
	GetFileNameL(fileName);
	if (iFileLockObservers==NULL)
		{
		iFileLockObservers=new(ELeave) CDesCArraySeg(1);
		}
	else
		{
		TInt pos;
		if(iFileLockObservers->Find(fileName,pos)== KErrNone)
			{
			iClientMessage->PanicClient(KPanic,EReqAlreadyOutstanding);
			User::Leave(KLeaveWithoutAlert);
			}		
		}
	iFileLockObservers->AppendL(fileName);
	BackupServer()->IncrementRegisteredFilesCount();
	}

LOCAL_C void RemoveFileName(CDesCArray& aArray,const TDesC& aFileName)
	{
	TInt pos;
	if (aArray.Find(aFileName,pos)==KErrNone)
		{
		aArray.Delete(pos);
		}
	}

void CBaServBackupSession::NotifyLockChangeCancelL()
	{
	TFileName fileName;
	GetFileNameL(fileName);
	if (iFileLockObservers)
		{
		RemoveFileName(*iFileLockObservers,fileName);
		BackupServer()->DecrementRegisteredFilesCount();
		}
	if (iReleasedFiles)
		{
		RemoveFileName(*iReleasedFiles,fileName);
		}
	iMessageQueue->RemoveItem(fileName);
  	}

void CBaServBackupSession::GetFileNameL(TDes& aFileName)
	{
	//The verification of this parameter has already been handled
	//by the CClientMessage class so we can safely read the value
	iClientMessage->ReadL(1,aFileName);
	}


void CBaServBackupSession::GetBackupOperationEventL(const RMessagePtr2& aPtr)
	{
	TPckg<TBackupOperationAttributes> backupOpAttPkg(iBackupOperationAttributes);
	
	aPtr.WriteL(0, backupOpAttPkg);
	}

void CBaServBackupSession::BackupOperationEventReadyL()
	{
	if (iBackupOperationObserverPresent)
		{
		iBackupOperationMessagePtr = iClientMessage->Message();
		}
	else
		{
		iClientMessage->CompleteRequestL(KErrNone);
		}
	}

void CBaServBackupSession::NotifyBackupOperationL()
	{
	CBaBackupServer* server=BackupServer();
	if (server->CBaBackupServer::IsOtherClientBusy(iUniqueClientId))
		{
		User::Leave(KErrServerBusy);
		}
	TPckg<TBackupOperationAttributes> backupOpAttPkg(iBackupOperationAttributes);
	
	iClientMessage->ReadL(0,backupOpAttPkg);

	const TBool backupOperationRunning = ((iBackupOperationAttributes.iOperation==MBackupOperationObserver::EStart) ? ETrue : EFalse);
	server->SetBackupOperationRunning(backupOperationRunning);
	server->SetBusy(backupOperationRunning ? iUniqueClientId : 0);
	server->SignalBackupOperation(iBackupOperationAttributes);
	if (!iBackupOperationMessagePtr.IsNull())
		{
		GetBackupOperationEventL(iBackupOperationMessagePtr);
		iBackupOperationMessagePtr.Complete(KErrCancel);
		}
	}

void CBaServBackupSession::GetBackupOperationStateL()
	{
	const TBool isRunning = BackupServer()->IsBackupOperationRunning();
	TPckgC<TBool> pkgObs(isRunning);

	iClientMessage->WriteL(0, pkgObs);
	}

void CBaServBackupSession::SignalBackupOperation(const TBackupOperationAttributes& aBackupOperationAttributes)
	{
	iBackupOperationAttributes = aBackupOperationAttributes;
	if (!iBackupOperationMessagePtr.IsNull())
		{
		TRAPD(err,GetBackupOperationEventL(iBackupOperationMessagePtr));
		iBackupOperationMessagePtr.Complete(err);
		}
	}

void CBaServBackupSession::StopNotifications()
	{
		if(!iNotificationPullMsg.IsNull())
		{// complete the registration message
		iNotificationPullMsg.Complete(KErrNone);
		}
	}

/**
@internalComponent
*/
CBaServBackupSession::CReRegistrationTimer* CBaServBackupSession::CReRegistrationTimer::NewL(TInt aPriority)
	{ // static
	CBaServBackupSession::CReRegistrationTimer* self=new(ELeave) CBaServBackupSession::CReRegistrationTimer(aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	CActiveScheduler::Add(self);
	return self;
	}

/**
@internalComponent
*/
TInt CBaServBackupSession::CReRegistrationTimer::RunError(TInt aError)
	{
	if (aError==KLeaveWithoutAlert)
		{
		return KErrNone;
		}
	return aError;
	}
	
/**
@internalComponent
*/
CBaServBackupSession::CReRegistrationTimer::CReRegistrationTimer(TInt aPriority)
	: CPeriodic(aPriority)
	{ }
	
/**
@internalComponent
*/
TInt CBaServBackupSession::CReRegistrationTimer::ReRegistrationTimerCallBack(TAny* aPtr)
	{ // static
	TRAP_IGNORE(REINTERPRET_CAST(CBaServBackupSession::CReRegistrationTimer*,aPtr)->HandleReRegistrationTimerCallBack());
	return 0;
	}

/**
@internalComponent
*/
void CBaServBackupSession::CReRegistrationTimer::HandleReRegistrationTimerCallBack()
	{
	iBackupServer->IncrementFilesReRegistrationCount();
	
	if (IsActive())
		{
		Cancel();
		}
	}

