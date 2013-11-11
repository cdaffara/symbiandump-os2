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
// generic backup server
// 
//

#if !defined(__BAKSRV_H__)
#define __BAKSRV_H__

#include <e32std.h>
#include <e32base.h>
#include <babackup.h>
#include <badesca.h>
#include <bsul/clientmessage.h>

//Forward declarations
class BSUL::CClientMessage;

/**
@internalTechnology
*/
const TInt KBakServMajorVN      =0;
const TInt KBakServMinorVN      =1;
const TInt KBakServBuildVN      =1;
const TInt KBakServMessageSlots	=4;		// no async messages to backup server

/**
@internalTechnology
*/
enum TBaBackupServPanic
	{
	EReqAlreadyOutstanding	=0,
	EReqDoesNotExist		=1,
	EBadFileName			=2,
	ELocksAlreadyReleased	=3,
	EBufferOverflow			=4,
	ENoEventToFetch			=5,
	EHandleNotClosed		=6,
	};

class CBaServBackupSession;

/**
@internalTechnology
*/
enum TCompletionType
	{
	ECompleteSync	=0x00,
	ECompleteAsync	=0x01,
	};


/**
@internalTechnology
*/
class CBaServBackupScheduler : public CActiveScheduler
	{
public:
	IMPORT_C CBaServBackupScheduler();
	inline static CBaServBackupScheduler* Current();
	IMPORT_C void SetErrorHandler(CBaServBackupSession* aErrorHandler);
	IMPORT_C ~CBaServBackupScheduler();
public: // from CActiveScheduler
	IMPORT_C virtual void Error(TInt aError) const;
private:
	CBaServBackupSession* iErrorHandler;
	};

/**
@internalTechnology
*/
NONSHARABLE_CLASS(CBaServBackupMessageQueue) : public CBase
	{
public:
	static CBaServBackupMessageQueue* NewL();
	~CBaServBackupMessageQueue();
	void AddItemL(const TDesC& aFileName,MBackupObserver::TFileLockFlags aFlag);
	void AddItem(const TDesC& aFileName);
	TBool IsEmpty() const;
	void GetHead(TDes& aFileName,MBackupObserver::TFileLockFlags& aFlag) const;
	void RemoveHead();
	void RemoveItem(const TDesC& aFileName);
private:
	TInt HeadIndex() const;
private:
	enum TOperation
		{
		ENoOp		=-1,
		ETakeLock	=0,
		};
	class TQueueItem
		{
	public:
		inline TQueueItem(HBufC* aFileName,TInt aOperation);
	public:
		HBufC* iFileName;
		TInt iOperation;
		};
private:
	RArray<TQueueItem> iQueue;
	};
	 
class CBaBackupServer;

/**
@internalTechnology
*/
class CBaServBackupSession : public CSession2

	{
public:
	class TClosedFile
		{
	public:
		inline TBool operator==(const TClosedFile& aClosedFile) const;
	public:
		TFileName iDocName;
		TUid iUid;
		};
	enum TLockType
		{ENull,ERelease,ETake,EPendingReleaseReadOnly,EPendingReleaseNoAccess,EPendingTake};
	class TFileLock
		{
	public:
		RMessagePtr2 iMessagePtr;
		TLockType iRequest;
		HBufC* iFileName;
		};
public:
	class CReRegistrationTimer;
public:
	IMPORT_C ~CBaServBackupSession();
	inline CBaBackupServer* BackupServer() const;
public:
	IMPORT_C virtual void HandleError(TInt aError);
protected: // from CSession
	IMPORT_C virtual void ServiceL(const RMessage2& aMessage);
	IMPORT_C virtual void CreateL();
protected:
	IMPORT_C virtual void ConstructL();
	IMPORT_C virtual TCompletionType CloseAllFilesL(const RMessage2& aMessage);
	IMPORT_C TCompletionType DoCloseAllFilesL(const RMessage2& aMessage);
	IMPORT_C virtual void RestartAll();
protected:
	IMPORT_C CBaServBackupSession();
	inline CArrayFix<TClosedFile>* ClosedFiles() const;
	inline void SetClosedFiles(CArrayFix<TClosedFile>* aClosedFiles);
	inline TInt UniqueClientId() const;
public:
	static CBaServBackupSession* NewL();
	void SignalReleaseAllFileLocksL(MBackupObserver::TFileLockFlags aFlag);
	void SignalRetakeAllFileLocks();
	void SignalReleaseFileLockL(MBackupObserver::TFileLockFlags aFlag,const TDesC& aFileName);
	void SignalRetakeFileLocks(const TDesC& aFileName);
	void SignalBackupOperation(const TBackupOperationAttributes& aBackupOperationAttributes);
private:
	static void CleanupCloseAllFiles(TAny* aPtr);
	void HandleEventReadyL();
	void GetEventL();
	void RestartFileL();
	void NotifyLockChangeL();
	void NotifyLockChangeCancelL();
	void GetFileNameL(TDes& aFileName);
	void BackupOperationEventReadyL();
	void GetBackupOperationEventL(const RMessagePtr2& aPtr);
	void GetBackupOperationStateL();
	void DoRestartAll();
	void NotifyBackupOperationL();
	void DoServiceL(TCompletionType& aCompleteType);
	void CloseFileL();
	void StopNotifications();
private:
	TUint32 iUniqueClientId;
	CBaServBackupMessageQueue* iMessageQueue;
	RMessagePtr2 iNotificationPullMsg;
	CArrayFix<TClosedFile>* iClosedFiles;
	CDesCArray* iFileLockObservers;
	CDesCArray* iReleasedFiles;
	RMessagePtr2 iBackupOperationMessagePtr;
	TBackupOperationAttributes iBackupOperationAttributes;
	TBool iBackupOperationObserverPresent;
	CReRegistrationTimer* iReRegistrationTimer; /* File lock re-registration timer */
	BSUL::CClientMessage* iClientMessage;
	
	};

class CShutdownServer;

/**
@internalTechnology
*/
class CBaBackupServerExt : public CBase  
	{
public:
	TUint32 iCachedBusyClientId;
	TUint32 iUniqueBusyClientId;
	};

//
// CBaBackupServer
//


/**
@internalTechnology
*/
class CBaBackupServer : public CServer2

	{
public:
	class CBaServCloseAllOperationTimer;
public:
	IMPORT_C static CBaBackupServer* NewL();
	IMPORT_C ~CBaBackupServer();
	IMPORT_C void SetBusy(TUint32 aUniqueClientId);
	IMPORT_C TBool IsClientBusy(TUint32 aUniqueClientId) const;
	IMPORT_C void SignalBackupOperation(const TBackupOperationAttributes& aBackupOperationAttributes);
	IMPORT_C TBool HaveAllCloseAllFilesClientsReRegistered();
	IMPORT_C void SetCloseAllOperationRunningState(TBool aRunning);
public:
	inline TBool IsBackupOperationRunning() const;
	inline void SetBackupOperationRunning(TBool aBackupOperationRunning);
public:
	IMPORT_C virtual void CloseAllFilesL(MBackupObserver::TFileLockFlags aFlag);
	IMPORT_C virtual void RestartAll();
	IMPORT_C virtual void ConstructL();
	IMPORT_C virtual void CompleteClosingFiles(CArrayFix<CBaServBackupSession::TClosedFile>* aClosedFiles);
	IMPORT_C virtual TBool IsOtherClientBusy(TUint32 aUniqueClientId) const;
public:
	void CloseFileL(MBackupObserver::TFileLockFlags aFlag,const TDesC& aFileName);
	void RestartFile(const TDesC& aFileName);
	void AddSession();
	void DropSession();
	void IncrementRegisteredFilesCount();
	void DecrementRegisteredFilesCount(TInt aNumberFiles = 1);
	void IncrementFilesReRegistrationCount();
	void StartCloseAllFilesOperationTimer(const RMessagePtr2& aPtr);
	TBool IsCloseAllOperationRunning();
public: // from CServer
	IMPORT_C TInt RunError(TInt aError);
protected:
	IMPORT_C CBaBackupServer(TInt aPriority);
protected: // from CServer
	IMPORT_C CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
private:
	CBaBackupServerExt* iExtension;
	TBool iBackupOperationRunning;
	TInt iSessionCount;
	CShutdownServer* iShutdown;
	TInt16 iRegisteredFilesCount;
	TInt16 iSessionLockReRegistrationCount; /* Number of file lock sessions currently registered */
	CBaServCloseAllOperationTimer* iCloseAllFilesOperationTimer; /* Base file lock re-registration timer */
	TBool iCloseAllOperationRunning; /* Indicates that a CloseAll operation is in progress */
	};

inline CBaServBackupScheduler* CBaServBackupScheduler::Current()
	{ // static
	return static_cast<CBaServBackupScheduler*>(CActiveScheduler::Current());
	}

inline CBaBackupServer* CBaServBackupSession::BackupServer() const
	{
	return static_cast<CBaBackupServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

inline CArrayFix<CBaServBackupSession::TClosedFile>* CBaServBackupSession::ClosedFiles() const
	{
	return iClosedFiles;
	}

inline void CBaServBackupSession::SetClosedFiles(CArrayFix<TClosedFile>* aClosedFiles)
	{
	if(!iClosedFiles)
		{
		iClosedFiles=aClosedFiles;
		}
	}

inline TInt CBaServBackupSession::UniqueClientId() const
	{
	return iUniqueClientId;
	}

inline TBool CBaServBackupSession::TClosedFile::operator==(const TClosedFile& aClosedFile) const
	{
	return iDocName==aClosedFile.iDocName && iUid==aClosedFile.iUid;
	}

inline TBool CBaBackupServer::IsBackupOperationRunning() const
	{
	return iBackupOperationRunning;
	}

inline void CBaBackupServer::SetBackupOperationRunning(TBool aBackupOperationRunning)
	{
	iBackupOperationRunning = aBackupOperationRunning;
	}

#endif
