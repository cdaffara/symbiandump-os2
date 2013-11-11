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

#if !defined(__BABACKUP_H__) 
#define __BABACKUP_H__

#include <e32std.h>
#include <e32base.h>

/** 
Backup server wrapper observer interface.

MBackupObserver defines the interface to be implemented by any code that will
hold file locks or may attempt to access files regardless of other device state
e.g. a phone app may always attempt to access the contacts database for caller id

@publishedAll
@released
*/
class MBackupObserver
	{
public:
/** Flags indicating how to change a file lock. */
	enum TFileLockFlags
		{
	/** Owner can reopen/resume writing to the file. */
		ETakeLock				=0,
	/** Owner should stop writing to the file. */
		EReleaseLockReadOnly	=0x01,
	/** Owner should close the file. */
		EReleaseLockNoAccess	=0x02,
		};
public:
	/** Called by the server to request the file owner to change the lock on the specified 
	file.
	
	@param aFileName Filename of affected file
	@param aFlags Flag indicating how to change the file lock */
	virtual void ChangeFileLockL(const TDesC& aFileName,TFileLockFlags aFlags)=0;
	};

class TBackupOperationAttributes;

/** Backup operation observer interface.

Programs should implement this interface to receive notifications when a backup 
or restore operation either starts or ends.

@see CBaBackupSessionWrapper::RegisterBackupOperationObserverL() 
@publishedAll
@released
*/
class MBackupOperationObserver
	{
public:
/** Backup operation event types. */
	enum TOperationType
		{
	/** None. */
		ENone		=0x00,
	/** Operation start. */
		EStart		=0x01,
	/** Operation end. */
		EEnd		=0x02,
	/** Operation aborted. */
		EAbort		=0x04,
		};
public:
	/** Called when a backup or restore operation either starts or ends.
	
	@param aBackupOperationAttributes Operation attributes */
	virtual void HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes)=0;
private:
	IMPORT_C virtual void Reserved1();
	};


/** 
Attributes for a backup operation.

@see MBackupOperationObserver 
@publishedAll
@released
*/
class TBackupOperationAttributes
	{
public:
	inline TBackupOperationAttributes();
	inline TBackupOperationAttributes(MBackupObserver::TFileLockFlags aFileFlag, MBackupOperationObserver::TOperationType aOperation);
public:
	/** File locking flags requested by the operation. */
	MBackupObserver::TFileLockFlags iFileFlag;
	/** Operation type. */
	MBackupOperationObserver::TOperationType iOperation;
	};


class RBaBackupSession;
class CBaLockChangeNotifier;
class CBaBackupOperationNotifier;

/**
CBaBackupSessionWrapper provides the APIs necessary to enable a backup or restore/install
by allowing broadcast requests for some or all files to either remain untouched or have
their locks released for an arbitrary period.

The class should be used by both code driving a backup/install and any engines that will
hold files locks or may access files regardless of the presence of (non-system) client apps

@publishedAll
@released
*/
class CBaBackupSessionWrapper : public CBase
	{
public:
	IMPORT_C static CBaBackupSessionWrapper* NewL();
	IMPORT_C ~CBaBackupSessionWrapper();
	IMPORT_C void RegisterFileL(const TDesC& aFileName,MBackupObserver& aObserver);
	IMPORT_C void DeregisterFile(const TDesC& aFileName);
	IMPORT_C void CloseAll(MBackupObserver::TFileLockFlags aFlags,TRequestStatus& aStatus);
	IMPORT_C void RestartAll();
	IMPORT_C void CloseFileL(const TDesC& aFileName,MBackupObserver::TFileLockFlags aFlags);
	IMPORT_C void RestartFile(const TDesC& aFileName);
	IMPORT_C void RegisterBackupOperationObserverL(MBackupOperationObserver& aBackupOperationObserver);
	IMPORT_C void DeRegisterBackupOperationObserver(MBackupOperationObserver& aBackupOperationObserver);
	IMPORT_C TBool IsBackupOperationRunning() const;
	IMPORT_C void NotifyBackupOperationL(const TBackupOperationAttributes& aBackupOperationAttributes);
private:
	CBaBackupSessionWrapper();
	void ConstructL();
private:
	RBaBackupSession* iBackupSession;
	CBaLockChangeNotifier* iLockChangeNotifier;
	TAny* iReserved;  // needed to preserve BC
	CBaBackupOperationNotifier* iBackupOperationNotifier;
	};


//
// TBackupOperationAttributes
//

inline TBackupOperationAttributes::TBackupOperationAttributes()
	: iFileFlag(MBackupObserver::ETakeLock), iOperation(MBackupOperationObserver::ENone)
	/** Default constructor. */
	{
	}

inline TBackupOperationAttributes::TBackupOperationAttributes(MBackupObserver::TFileLockFlags aFileFlag, MBackupOperationObserver::TOperationType aOperation)
	: iFileFlag(aFileFlag), iOperation(aOperation)
	/** Constructor that specifies operation attributes.
	
	@param aFileFlag File locking flags requested by the operation
	@param aOperation Operation type */
	{
	}



#endif

