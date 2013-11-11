// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SqlBur.h"
#include "SqlAssert.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlBurTraces.h"
#endif
#include "SqlTraceDef.h"

#define UNUSED_ARG(arg) arg = arg

_LIT(KSqlBurBackupExt,  ".bak");
_LIT(KSqlBurRestoreDir, "temprestore");
_LIT(KSqlBurAllFiles,   "*");

const TUint K8to16bitShift = 1;

//Extracts and returns 32-bit integer from aNumBuf buffer.
static TUint32 GetNumUint32L(const TDesC& aNumBuf)
	{
	TLex lex(aNumBuf);
	lex.SkipSpace();
	TUint32 num = 0xFFFFFFFF;
	__SQLLEAVE_IF_ERROR2(lex.Val(num, EHex));
	return num;
	}

//Extracts and returns 64-bit integer from aNumBuf buffer.
static TInt64 GetNumInt64L(const TDesC& aNumBuf)
	{
	TLex lex(aNumBuf);
	lex.SkipSpace();
	TInt64 num = -1;
	__SQLLEAVE_IF_ERROR2(lex.Val(num, EHex));
	return num;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////   CSqlBurEventMonitor    //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** 
Standard two phase construction. Creates a CSqlBurEventMonitor instance.
@param aInterface A reference to an interface that is used for retrieving list of databases to be sent for backup.
@return An instance of the backup notifier
@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may also leave with some other system-wide error codes.
*/
CSqlBurEventMonitor* CSqlBurEventMonitor::NewL(MSqlSrvBurInterface& aInterface)
	{
	CSqlBurEventMonitor* self = new (ELeave) CSqlBurEventMonitor(aInterface);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	SQL_TRACE_BUR(OstTrace1(TRACE_INTERNALS, CSQLBACKUPNOTIFIER_NEWL, "0x%X;CSqlBurEventMonitor::NewL", (TUint)self));
	return self;
	}

/**
Releases the allocated resources.
*/
CSqlBurEventMonitor::~CSqlBurEventMonitor()
	{
	SQL_TRACE_BUR(OstTrace1(TRACE_INTERNALS, CSQLBACKUPNOTIFIER_CSQLBACKUPNOTIFIER2, "0x%X;CSqlBurEventMonitor::~CSqlBurEventMonitor", (TUint)this));
	Cancel();
	iBurProperty.Close();
	DestroyContent();
	}

/**
Initializes data members with their default values. 
@param aInterface A reference to an interface that is used for retrieving list of databases to be sent for backup.
*/		
CSqlBurEventMonitor::CSqlBurEventMonitor(MSqlSrvBurInterface& aInterface) :
	CActive(EPriorityStandard), 
	iBurInterface(aInterface)
	{
	}

/**
Initializes the created CSqlBurEventMonitor object.  
@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may also leave with some other system-wide error codes.
*/	
void CSqlBurEventMonitor::ConstructL()
	{
	__SQLLEAVE_IF_ERROR(iBurProperty.Attach(KSqlBurPropertyCategoryUid, KSqlBurBackupRestoreKey));
	CActiveScheduler::Add(this);
	iBurProperty.Subscribe(iStatus);
	SetActive();
	}

/** 
RunL() is called when the value of the {KUidSystemCategory, KUidBackupRestoreKey} gets changed.
That indicates: a backup or a restore is about to begin.

How the function works:
 - When a backup or restore notification is received, the function will subscribe again for notifications from
   the backup and restore property and will read the property status;
 - If the property status is conn::EBURUnset or conn::EBURNormal, the function will destroy iSqlBurCallback
   and iActiveBackupClient interfaces. No more callbacks will be reseived from the backup and restore server.
   This is the end of the backup or restore processing;
 - If the property status is conn::EBURBackupFull, conn::EBURBackupPartial, conn::EBURRestoreFull or 
   conn::EBURRestorePartial, the function will create iSqlBurCallback and iActiveBackupClient interface
   (iActiveBackupClient's NewL() receives iSqlBurCallback as an input parameter, registering this way the callback
   in the backup and restore server to be called later, when sending or retrieving data to/from the server).
   If the property read and the interface creation operations have been successful, the function will call 
   ConfirmReadyForBURL(KErrNone) to notify the backup and restore server that the SQL server is ready to send/retrieve 
   backup/restore data.
   If the current notification is that a backup is about to begin, after the confirmation the backup and restore server will
   call CSqlBurCallback::InitialiseGetProxyBackupDataL() once per {client secure id, drive} 
   followed by CSqlBurCallback::GetBackupDataSectionL() calls to retrieve the backup data. 
   If the current notification is that a restore is about to begin, after the confirmation the backup and restore server will
   call CSqlBurCallback::InitialiseRestoreProxyBaseDataL() once per {client secure id, drive} 
   followed by CSqlBurCallback::RestoreBaseDataSectionL() calls to send the restore data.
     
The current implementation has one design flaw. If a backup or restore notification is received, there are at lest 3
places before the ConfirmReadyForBURL() call, where the code may leave: 
 - the "property get" operation;
 - the iSqlBurCallback creation;
 - the iActiveBackupClient creation;
If a leave occurs at some of the mentioned places, that leave will be trapped by the current CActiveScheduler object
and CSqlBurEventMonitor::RunError() will be called with the leaved error code.
Problem #1: CSqlBurEventMonitor::RunError() won't do anything with the error (apart from printing a trace in the OST builds).
            The error is silently suppressed. The backup or restore won't start. But the client won't see any notification
            for that problem.
Problem #2: ConfirmReadyForBURL() won't be called. According to the backup and restore documentation, if
            ConfirmReadyForBURL() is called with KErrNone parameter, that's a confirmation for the backup and restore
            server to start the processing. If ConfirmReadyForBURL() is called with an error different than KErrNone,
            that's a confirmation for the backup and restore server that the client is not ready. No backup or restore
            will be started. The remote backup client will be notified about the problem.
After an investigation it was found that the same problems do exist in all active backup clients, none of them has 
solved the problems. Then, the code here will be kept as it is, it might be too dangerous to do a change right now.

@see CSqlBurEventMonitor::RunError()
@see CSqlBurCallback
@see CActiveBackupClient
@see CSqlBurCallback::InitialiseGetProxyBackupDataL()
@see CSqlBurCallback::GetBackupDataSectionL()
@see CSqlBurCallback::InitialiseRestoreProxyBaseDataL()
@see CSqlBurCallback::RestoreBaseDataSectionL()

@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may also leave with some other system-wide error codes.
*/
void CSqlBurEventMonitor::RunL()
	{
	SQL_TRACE_BUR(OstTrace1(TRACE_INTERNALS, CSQLBACKUPNOTIFIER_RUNL_ENTRY, "Entry;0x%X;CSqlBurEventMonitor::RunL", (TUint)this));
	iBurProperty.Subscribe(iStatus);
	SetActive();
	TInt status;
	__SQLLEAVE_IF_ERROR(iBurProperty.Get(status));
	status &= conn::KBURPartTypeMask;
#ifdef _SQL_RDEBUG_PRINT
	SQL_TRACE_BUR(OstTraceExt2(TRACE_INTERNALS, CSQLBACKUPNOTIFIER_RUNL1, "0x%X;CSqlBurEventMonitor::RunL;status=%d", (TUint)this, status));
#else
	SQL_TRACE_BUR(OstTraceExt2(TRACE_INTERNALS, CSQLBACKUPNOTIFIER_RUNL2, "0x%X;CSqlBurEventMonitor::RunL;status=%{TBURPartType}", (TUint)this, status));
#endif	    
	switch(status)
		{
		case conn::EBURBackupFull:
		case conn::EBURBackupPartial:
		case conn::EBURRestoreFull:
		case conn::EBURRestorePartial:
			{
			// we only do full backups and full restores
			if(!(iSqlBurCallback && iActiveBackupClient))
				{
				DestroyContent();
				TRAPD(err, CreateContentL());
				if(err != KErrNone)
					{
					DestroyContent();
					__SQLLEAVE(err);
					}
				}
			iActiveBackupClient->ConfirmReadyForBURL(KErrNone);
			}
			break;
		//case conn::EBURUnset:
		//case conn::EBURNormal:
		default:
			DestroyContent();
			break;
		}
	SQL_TRACE_BUR(OstTrace1(TRACE_INTERNALS, CSQLBACKUPNOTIFIER_EXIT, "Exit;0x%X;CSqlBurEventMonitor::RunL", (TUint)this));
	SQL_BUR_TEST_STOP();
	}

/** 
Cancels the subscribtion for {KUidSystemCategory, KUidBackupRestoreKey} property changes.
*/
void CSqlBurEventMonitor::DoCancel()
	{
	iBurProperty.Cancel();
	}

/**
No-op. The method does nothing with the reported from CSqlBurEventMonitor::RunL() error  
(apart from logging a trace in OST builds).
Actually, the right action is to return KErrNone (as it is implemented), otherwise the default implementation of 
CActiveScheduler::Error() will panic the current thread.

@see CActiveScheduler::Error()
@see CSqlBurEventMonitor::RunL()
 
@return The RunL() error, if the RunL() call leaves.
@param The RunL() error
*/
TInt CSqlBurEventMonitor::RunError(TInt aError)
	{
	UNUSED_ARG(aError);
	SQL_TRACE_BUR(OstTraceExt2(TRACE_INTERNALS, CSQLBACKUPCLIENT_RUNERROR, "0x%X;CSqlBurEventMonitor::RunError;aError=%d", (TUint)this, aError));
	SQL_BUR_TEST_SET_ERROR(aError); 
	SQL_BUR_TEST_STOP();
	return KErrNone;
	}

/**
Creates iActiveBackupClient and iSqlBurCallback objects.
*/
void CSqlBurEventMonitor::CreateContentL()
	{
	iSqlBurCallback = CSqlBurCallback::NewL(iBurInterface);
	iActiveBackupClient = conn::CActiveBackupClient::NewL(iSqlBurCallback);
	}

/**
Destroys iActiveBackupClient and iSqlBurCallback objects.
*/
void CSqlBurEventMonitor::DestroyContent()
	{
	delete iActiveBackupClient;
	iActiveBackupClient = NULL;
	delete iSqlBurCallback;
	iSqlBurCallback = NULL;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////   CSqlBackupClient    /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/** 
Creates new CSqlBurCallback instance.
The CSqlBurEventMonitor object monitors the state of the {KUidSystemCategory, KUidBackupRestoreKey} 
property. When a backup or a restore is about to begin, the CSqlBurEventMonitor object creates a
CSqlBurCallback instance, establishes a connection with the B&R server and passes a pointer to
the CSqlBurCallback callback to the BYR conenction.
The CSqlBurCallback methods will be called during the backup/restore for sending/retrieving data.
    
@param aInterface A reference to an interface that is used for retrieving list of databases to be sent for backup.
@return A pointer to the created CSqlBurCallback instance
@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may also leave with some other system-wide error codes.
*/
CSqlBurCallback* CSqlBurCallback::NewL(MSqlSrvBurInterface& aInterface)
	{
	CSqlBurCallback* self = new (ELeave) CSqlBurCallback(aInterface);
	SQL_TRACE_BUR(OstTrace1(TRACE_INTERNALS, CSQLBACKUPCLIENT_NEWLC, "0x%X;CSqlBurCallback::NewL", (TUint)self));
	return self;
	}

/**
Initializes CSqlBurCallback data members with their default values. 
@param aInterface A reference to an interface that is used for retrieving list of databases to be sent for backup.
*/		
CSqlBurCallback::CSqlBurCallback(MSqlSrvBurInterface& aInterface) :
	iInterface(aInterface)
	{
	}

/** 
Releases the allocated resources.
*/
CSqlBurCallback::~CSqlBurCallback()
	{
	SQL_TRACE_BUR(OstTraceExt2(TRACE_INTERNALS, CSQLBACKUPCLIENT_CSQLBACKUPCLIENT2, "0x%X;CSqlBurCallback::~CSqlBurCallback;iFile.SubSessionHandle()=0x%X", (TUint)this, (TUint)iFile.SubSessionHandle()));
	BackupCleanup();
	(void)RestoreCleanup();
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////       Full backup   //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** 
This is called to let us know that the given SID is to be backed up.
We ask the SQL server (using iInterface, see MSqlSrvBurInterface for more details) 
for a list of databases that want to be backed up.

The backup is initiated by a notification received in CSqlBurEventMonitor::RunL() method.
InitialiseGetProxyBackupDataL() is called once per {client secure id, drive} and each 
InitialiseGetProxyBackupDataL() call is followed after that by a set of CSqlBurCallback::GetBackupDataSectionL() calls,
made from the backup and restore client dll.

During GetBackupDataSectionL() calls the CSqlBurCallback object will read the content of the databases from the list,
retrieved from the MSqlSrvBurInterface::GetBackUpListL() call and send the content to the backup and restore server.     

@see MSqlSrvBurInterface
@see CSqlBurEventMonitor::RunL()
@see CSqlBurCallback::GetBackupDataSectionL()
@see CSqlServer::GetBackUpListL() 

@param aSid the UID of the application to backup
@param aDrive the drive to be backed up
@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may also leave with some other system-wide error codes.
*/
void CSqlBurCallback::InitialiseGetProxyBackupDataL(TSecureId aSid, TDriveNumber aDrive)
	{
	SQL_TRACE_BUR(OstTraceExt3(TRACE_INTERNALS, CSQLBACKUPCLIENT_INITIALIZEGETPROXYBACKUPDATAL, "0x%X;CSqlBurCallback::InitialiseGetProxyBackupDataL;aSid=0x%X;aDrive=%d", (TUint)this, (TUint)aSid.iId, (TInt)aDrive));
	BackupCleanup();
	iInterface.GetBackUpListL(aSid, aDrive, iFileList);
	iFileIndex = 0;
	iState = EBackupNoFileOpen;
	iBackupError = KErrNone;
	}

/** 
This is supposed to allow the B&R framework to know in advance how much
data is coming - but unfortunately there is no way to know this
at this stage since we don't even know yet what SID is being processed
So we just answer some number to make the BUE happy. It doesn't
actually rely on this number so there is no risk - the aFinishedFlag
indicates the end of data, not the value returned here. It is
supposed to allow the BUE to optimise its behaviour by know up front
the data volume.

@see CSqlBurEventMonitor::RunL()
@see CSqlBurCallback::InitialiseGetProxyBackupDataL()

@param aDrive Unused parameter (the drive number is logged in OST builds).
@return an arbitrary number (1024 at the moment)
*/
TUint CSqlBurCallback::GetExpectedDataSize(TDriveNumber aDrive)
	{
	UNUSED_ARG(aDrive);
	// we have no idea at this point - we even don't know who is to be backed up yet
	const TUint KArbitraryNumber = 1024;
	SQL_TRACE_BUR(OstTraceExt3(TRACE_INTERNALS, CSQLBACKUPCLIENT_GETEXPECTEDDATASIZE, "0x%X;CSqlBurCallback::GetExpectedDataSize;aDrive=%d;rc=%u", (TUint)this, (TInt)aDrive, KArbitraryNumber));
	return KArbitraryNumber;
	}

/** 
This is the backup state machine
Because the data has to be sent back in sections and the various
components of the dataflow may straddle chunks, we have to keep
track of where we are between each transfer - a state machine is
the simplest and most understandable implementation.

Please note how the function processes the errors occuring during the backup.
If an error occurs, the error is not propagated back to the B&R server immediatelly.
The error is stored in iBurError data member and is reported at the end of the backup process.
The reason for such unusual error reporting poicy is: the SQL server performs full backup of possibly more
than one database file. If an error occurs during the backup of the first file for example, the backup
process should not stop at that point. All files will be processed and then at the end, the error will be reproted.

In details, the function runs a state machine, where:
 - every file in the list retrieved in InitialiseGetProxyBackupDataL() is opened;
 - the file is read and 32-bit checksum over the file data - calculated;
 - a file backup header is prepared, including there the file size, file name, file name length, protocol verison number
   and the checksum. The header is sent to the backup restore server;
 - the file data is read and sent to the backup and restore server;
 - during the described above sequence no leave ever occurs. The error that occurs during the file processing,
   is stored into a data member of CSqlBurCallback class. At the end, after the last file in the list is processed,
   the backup and restore server will get a notification (via a User::Leave() call) regarding the error;
   The used error reporting policy allows all files to be process without interrupting the backup process.
   For example, if there are 5 files to be sent to the backup and restore server, an error that occurs during the 
   processing of file #3, won't prevent files #4 and #5 from being sent for backup.       

@see CSqlBurEventMonitor::RunL()
@see CSqlBurCallback::InitialiseGetProxyBackupDataL()

@param aBuffer Output parameter, the buffer where the data will be put to be passed back
@param aFinishedFlag Set to true when all data has been submitted for backup
@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may also leave with some other system-wide error codes.
*/
void CSqlBurCallback::GetBackupDataSectionL(TPtr8& aBuffer, TBool& aFinishedFlag)
	{
	// don't assume they set it to false
	aFinishedFlag=EFalse;
	// any files to backup
	if(iFileList.Count()==0)
		{
		// nothing to backup
		SQL_TRACE_BUR(OstTrace1(TRACE_INTERNALS, CSQLBACKUPCLIENT_GETBACKUPDATASECTIONL1, "0x%X;CSqlBurCallback::GetBackupDataSectionL;file count is 0", (TUint)this));
		aFinishedFlag = ETrue;
		BackupCleanup();
		return;
		}

	// run the state machine
	for(TInt bufFreeSpace=aBuffer.MaxSize()-aBuffer.Size(); bufFreeSpace>0; bufFreeSpace=aBuffer.MaxSize()-aBuffer.Size())
		{
		switch(iState)
			{
			case EBackupNoFileOpen: // open a file for processing
				{
				if(iFileIndex>=iFileList.Count())
					{// all files have been processed - send the finished flag
					SQL_TRACE_BUR(OstTraceExt2(TRACE_INTERNALS, CSQLBACKUPCLIENT_GETBACKUPDATASECTIONL2, "0x%X;CSqlBurCallback::GetBackupDataSectionL;all files processed;iBackupError=%d", (TUint)this, iBackupError));
					aFinishedFlag = ETrue;
					BackupCleanup();
					__SQLLEAVE_IF_ERROR(iBackupError);
					return;
					}
				// open the database file to send
				TPtrC fname = iFileList[iFileIndex]->Des();
				TInt err = iFile.Open(iInterface.Fs(), fname, EFileRead | EFileShareExclusive);
				SQL_TRACE_BUR(OstTraceExt5(TRACE_INTERNALS, CSQLBACKUPCLIENT_GETBACKUPDATASECTIONL3, "0x%X;CSqlBurCallback::GetBackupDataSectionL;BEGIN;fname=%S;iFileIndex=%d;iFile.SubSessionHandle()=0x%X;err=%d", (TUint)this, __SQLPRNSTR(fname), iFileIndex, (TUint)iFile.SubSessionHandle(), err));
				if(KErrNone != err)
					{
					// there's nothing we can do if we can't open the file so we just skip it
					SetBackupError(err);
					++iFileIndex;
					break;
					}
				iState=EBackupOpenNothingSent;
				break;
				}
			case EBackupOpenNothingSent: // nothing sent (so far) for this file - send the header info
				{
				TInt64 fileSize;
				TInt err = iFile.Size(fileSize);
				if(KErrNone != err)
					{
					SetBackupError(err);
					iState = EBackupEndOfFile;
					break;
					}
				
				TUint64 checksum64 = 0;
				err = CheckSum(iFile, checksum64);
				SQL_TRACE_BUR(OstTraceExt3(TRACE_INTERNALS, CSQLBACKUPCLIENT_GETBACKUPDATASECTIONL4, "0x%X;CSqlBurCallback::GetBackupDataSectionL;CheckSum();iFileIndex=%d;err=%d", (TUint)this, iFileIndex, err));
				if(err != KErrNone)
					{
					//An error occured while reading the file (or there was not enough memory for the read buffer)
					SetBackupError(err);
					iState = EBackupEndOfFile;
					break;
					}
				// Only grab last 4 bytes of the checksum - enough to be satisfied that the backup and restore worked ok
				TUint32 checksum32 = checksum64 & KMaxTUint32;

                // build the header - this is an instance member because it
                // has to persist over multiple calls to this method
				TPtrC fname = iFileList[iFileIndex]->Des();
				iBuffer.Format(_L("%8x%8x%4x%16lx%8x%S"),
					checksum32,					// %8x
					KSqlBurMagicNum,			// %8x
					KSqlBurHeaderVersion,		// %4x
					fileSize,					// %16lx
					fname.Length(),				// %8x
					&fname);					// %S
				SQL_TRACE_BUR(OstTraceExt4(TRACE_INTERNALS, CSQLBACKUPCLIENT_GETBACKUPDATASECTIONL5, "0x%X;CSqlBackupClient::GetBackupDataSectionL;fileName=%S;hdrPtr=|%S|;fileSize=%lld", (TUint)this, __SQLPRNSTR(fname), __SQLPRNSTR(iBuffer), fileSize));
				
				// we need it to look like an 8bit buffer
				TPtr8 hdrPtr8((TUint8*)iBuffer.Ptr(), iBuffer.Size(), iBuffer.Size());
							
				TInt len = Min(hdrPtr8.Size(), bufFreeSpace);
				
				// append the header to the buffer (only till it's full)
				aBuffer.Append(hdrPtr8.Ptr(), len);
				
				// decide what needs to happen next
				// if complete then we need data, otherwise we need to put
				// the rest of the header in the next chunk
				if(hdrPtr8.Size() <= bufFreeSpace)
					{
					iState = EBackupOpenAllHeaderSent;
					}
				else
					{
					// we need to keep track of how much of the header has
					// been sent so that we only send the reminder on the next
					// iteration
					iHeaderSent = len;
					iState = EBackupOpenPartHeaderSent;
					}
				break;
				}
			case EBackupOpenPartHeaderSent: // need to send the rest of the header
				{
				// get back the header - this is already loaded with the necessary info
				// from the previous state we were in - EBackupOpenNothingSent
				
				// we need it to look like an 8bit buffer
				TPtr8 hdrPtr8((TUint8*)iBuffer.Ptr(), iBuffer.Size(), iBuffer.Size());
				
				// how many bytes have we yet to send?
				TInt bytesRemaining = hdrPtr8.Size() - iHeaderSent;
				TInt len = Min(bytesRemaining, bufFreeSpace);
				aBuffer.Append(hdrPtr8.Ptr() + iHeaderSent, len);
				
				if(bytesRemaining <= bufFreeSpace)
					{
					iHeaderSent = 0; // ready for next header
					iState = EBackupOpenAllHeaderSent;
					}
				else
					{
					iHeaderSent += len; // ready to do round again
					//iState=EBackupOpenPartHeaderSent; same state as now!
					}
				break;
				}
			case EBackupOpenAllHeaderSent: // need to send some data
				{
				TPtr8 ptr((TUint8*)aBuffer.Ptr() + aBuffer.Size(), 0, bufFreeSpace);
				TInt err = iFile.Read(ptr);
				if(err != KErrNone)
					{
					//An error occured while reading the file 
					SetBackupError(err);
					iState = EBackupEndOfFile;
					SQL_TRACE_BUR(OstTraceExt3(TRACE_INTERNALS, CSQLBACKUPCLIENT_GETBACKUPDATASECTIONL6, "0x%X;CSqlBurCallback::GetBackupDataSectionL;File read;iFileIndex=%d;err=%d", (TUint)this, iFileIndex, err));
					break;
					}
				TInt bytesRead = ptr.Size();
				aBuffer.SetLength(aBuffer.Size() + bytesRead);
				// EOF
				if(bytesRead == 0)
					{
					iState = EBackupEndOfFile;
					break;
					}
				break;
				}
			case EBackupEndOfFile:
				{
				SQL_TRACE_BUR(OstTraceExt3(TRACE_INTERNALS, CSQLBACKUPCLIENT_GETBACKUPDATASECTIONL7, "0x%X;CSqlBurCallback::GetBackupDataSectionL;END;iFile.SubSessionHandle()=0x%X;iFileIndex=%d", (TUint)this, (TUint)iFile.SubSessionHandle(), iFileIndex));
				iFile.Close();
				++iFileIndex; // move on to next file
				iState = EBackupNoFileOpen; // go round again
				break;
				}
			default:
				__ASSERT_DEBUG(EFalse, __SQLPANIC(ESqlPanicInternalError));
				break;
			}//end of the "switch" statement
		}//end of the "for" statement
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////       Full restore   /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** 
Called when the BUE wants to start sending data to us.
Creates the folder (if the folder does not exist) where the temporary files will be created during the restore process.
Deletes all files from the restore folder.

The restore is initiated by a notification received in CSqlBurEventMonitor::RunL() method.
InitialiseRestoreProxyBaseDataL() is called once per {client secure id, drive} and each 
InitialiseRestoreProxyBaseDataLL() call is followed after that by a set of CSqlBurCallback::RestoreBaseDataSectionL() 
calls, made from the backup and restore client dll.

During RestoreBaseDataSectionLL() calls the CSqlBurCallback object will receive data from the backup and resore server.     

@see CSqlBurEventMonitor::RunL()
@see CSqlBurCallback::RestoreBaseDataSectionL()

@param aSid the UID of the application that is to be restored. Not used (only logged in OST builds).
@param aDrive the drive to restore.
@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may also leave with some other system-wide error codes.
*/
void CSqlBurCallback::InitialiseRestoreProxyBaseDataL(TSecureId aSid, TDriveNumber aDrive)
	{
	UNUSED_ARG(aSid);
	UNUSED_ARG(aDrive);
	SQL_TRACE_BUR(OstTraceExt3(TRACE_INTERNALS, CSQLBACKUPCLIENT_INITIALIZERESTOREPROXYBASEDATAL, "0x%X;CSqlBurCallback::InitialiseRestoreProxyBaseDataL;aSid=0x%X;aDrive=%d", (TUint)this, (TUint)aSid.iId, (TInt)aDrive));
	iBuffer.Zero();
	iState = ERestoreExpectChecksum;
	iRestoreDrive = aDrive;
	iRestoreId = aSid;
	//Create the directory for the temporary files created during the restore process.
	TFileName privatePath;
	__SQLLEAVE_IF_ERROR(iInterface.Fs().PrivatePath(privatePath));
	TDriveUnit driveUnit(iRestoreDrive);
	TDriveName driveName = driveUnit.Name();
	privatePath.Insert(0, driveName);
	__SQLLEAVE_IF_ERROR(iParse.Set(KSqlBurRestoreDir, &privatePath, 0));
	iRestoreDir.Copy(iParse.FullName());
	iRestoreDir.Append(KPathDelimiter);
	TInt err = iInterface.Fs().MkDirAll(iRestoreDir);
	if(err != KErrAlreadyExists)
		{
		__SQLLEAVE_IF_ERROR(err);
		}
	//Cleanup the restore directory
	err = RestoreCleanup();
	if(err != KErrNotFound)
		{
		__SQLLEAVE_IF_ERROR(err);
		}
	}

/** 
This is called by BUE when the restore has completed.

@see CSqlBurEventMonitor::RunL()
@see CSqlBurCallback::InitialiseRestoreProxyBaseDataL()

@param aDrive the drive that is being restored. Not used (only logged in OST builds).
*/
void CSqlBurCallback::RestoreComplete(TDriveNumber aDrive)
	{
	UNUSED_ARG(aDrive);
	SQL_TRACE_BUR(OstTraceExt2(TRACE_INTERNALS, CSQLBACKUPCLIENT_RESTORECOMPLETE, "0x%X;CSqlBurCallback::RestoreComplete;aDrive=%d", (TUint)this, (TInt)aDrive));
	iRestoreDrive = TDriveNumber(-1);
	iRestoreId = TSecureId(KNullUid);
	}

/** 
This is repeatedly called by the BUE to send us chunks of restore data (for the current SID)
Becuase the data is spread over chunks we need to manage the state across mutiple calls
to this method so we use a state machine.

The function runs the state machine and for each file block detected in the coming data, the function does:
 - creates a temporary file in the restore directory (created by InitialiseRestoreProxyBaseDataL());
 - stores the file data in the created temporary file;
 - During the 2 steps descirbed above, if an error occurs, that erro will be reproted to the backup and restore
   server (via a User::Leave() call);
 - When all data is received and stored in temporary files in the restore directory, 
   for each received file the function will:
   = move the original database file to the restore directory with a ".bak" extension added to the file name;
   = move the temporary file, which has the same name as the original database file, to the location of the
     original database file - the SQL server private data cage;
   = delete the file with the ".bak" extension;
   The three steps described above are implemented as "all or none" operation - if an error occurs during step (2),
   the content of the original database file will be restored from the file with the ".bak" extension.   

@see CSqlBurEventMonitor::RunL()
@see CSqlBurCallback::InitialiseRestoreProxyBaseDataL()

@param aInBuffer Buffer with data to be restored
@param aFinishedFlag Set when there is not more data to restore
@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may also leave with some other system-wide error codes.
*/
void CSqlBurCallback::RestoreBaseDataSectionL(TDesC8& aInBuffer, TBool aFinishedFlag)
	{
	// used to walk the buffer
	// got a new buffer - because each time this method is called, we have a
	// fresh chunk of data
	TInt inBufferPos = 0;

	// to mark when the state machine is through
	TBool done = EFalse;
	
	// check whether this is an empty restore
	if(aFinishedFlag && aInBuffer.Size() == 0)
		{
		return;
		}

	TInt iterations = 0;
	
	// run the state machine
	do
		{
		// how many bytes are there available in the buffer for processing?
		TInt bytesAvailable = aInBuffer.Size() - inBufferPos;
		// the reason why we are testing finishedFlag is because we must
		// make sure we re-enter the machine to do the tidyup
		if(bytesAvailable <= 0 && !aFinishedFlag)
			{
			// ran out of data in the chunk
			// so we return and wait for more data to arrive
			return;
			}
		switch(iState)
			{
			case ERestoreExpectChecksum: // 16 bytes (the header is UTF16 encoded, 8 unicode characters for the checksum)
				{
				const TInt KCheckSumStrLen = 8;
				CopyBufData(aInBuffer, inBufferPos, iBuffer, KCheckSumStrLen);
				if(iBuffer.Length() == KCheckSumStrLen)
					{
					iChecksum = ::GetNumUint32L(iBuffer);
					iState = ERestoreExpectOldFileSize;
					iBuffer.Zero();
					}
				break;
				}
			case ERestoreExpectOldFileSize: // 16 bytes (the header is UTF16 encoded, 8 unicode characters for 32-bit old file size)
				{
				const TInt KOldFileSizeStrLen = 8;
				CopyBufData(aInBuffer, inBufferPos, iBuffer, KOldFileSizeStrLen);
				if(iBuffer.Length() == KOldFileSizeStrLen)
					{
					TUint32 oldFileSize = ::GetNumUint32L(iBuffer);
					if(oldFileSize == KSqlBurMagicNum)
						{
						iState = ERestoreExpectVersion;
						}
					else
						{
						iFileSize = oldFileSize;	
						iState = ERestoreExpectFileNameSize;
						}
					iBuffer.Zero();
					}
				break;
				}	
			case ERestoreExpectVersion:
				{
				const TInt KVersionStrLen = 4;
				CopyBufData(aInBuffer, inBufferPos, iBuffer, KVersionStrLen);
				if(iBuffer.Length() == KVersionStrLen)
					{
					//Ignore the version: ::GetNumUint32L(iBuffer);	
					//At this stage we know that the version is 2+
					iState = ERestoreExpectFileSize;
					iBuffer.Zero();
					}
				break;
				}
			case ERestoreExpectFileSize:
				{
				const TInt KFileSizeStrLen = 16;
				CopyBufData(aInBuffer, inBufferPos, iBuffer, KFileSizeStrLen);
				if(iBuffer.Length() == KFileSizeStrLen)
					{
					iFileSize = GetNumInt64L(iBuffer);	
					iState = ERestoreExpectFileNameSize;
					iBuffer.Zero();
					}
				SQL_TRACE_BUR(OstTraceExt2(TRACE_INTERNALS, CSQLBACKUPCLIENT_RESTOREBASEDATASECTONL1, "0x%X;CSqlBurCallback::RestoreBaseDataSectionL;iFileSize=%lld", (TUint)this, iFileSize));
				break;
				}
			case ERestoreExpectFileNameSize: // the size of the file name to restore
				{
				const TInt KFileNameLenStrLen = 8;
				CopyBufData(aInBuffer, inBufferPos, iBuffer, KFileNameLenStrLen);
				if(iBuffer.Length() == KFileNameLenStrLen)
					{
					iFileNameSize = GetNumUint32L(iBuffer);		
					iState = ERestoreExpectFileName;
					iBuffer.Zero();
					}
				break;
				}
			case ERestoreExpectFileName:  // the name of the file to restore
				{
				CopyBufData(aInBuffer, inBufferPos, iBuffer, iFileNameSize);
				SQL_TRACE_BUR(OstTraceExt4(TRACE_INTERNALS, CSQLBACKUPCLIENT_RESTOREBASEDATASECTONL2, "0x%X;CSqlBurCallback::RestoreBaseDataSectionL;BEGIN;iBuffer=%S;iBuffer.Length()=%d;iFileNameSize=%d", (TUint)this, __SQLPRNSTR(iBuffer), iBuffer.Length(), iFileNameSize));
				if(iBuffer.Length() == iFileNameSize)
					{
					iState = ERestoreExpectData;
					TParse parse;
					__SQLLEAVE_IF_ERROR(parse.Set(iBuffer, 0, 0));
					__SQLLEAVE_IF_ERROR(iParse.Set(parse.NameAndExt(), &iRestoreDir, 0));
					TPtrC fname(iParse.FullName());
					//The database is restored first to a temporary file, in the restore folder, on the same drive.
					__SQLLEAVE_IF_ERROR(iFile.Replace(iInterface.Fs(), fname, EFileWrite | EFileShareExclusive));
					SQL_TRACE_BUR(OstTraceExt3(TRACE_INTERNALS, CSQLBACKUPCLIENT_RESTOREBASEDATASECTONL3, "0x%X;CSqlBurCallback::RestoreBaseDataSectionL;fname=%S;iFile.SubSessionHandle()=0x%X", (TUint)this, __SQLPRNSTR(fname), (TUint)iFile.SubSessionHandle()));
					iBuffer.Zero();
					}
				break;
				}
			case ERestoreExpectData: // now for the data
				{
				TInt len = Min((aInBuffer.Size() - inBufferPos), iFileSize);
				TInt err = iFile.Write(aInBuffer.Mid(inBufferPos, len));
				if(err != KErrNone)
					{
					(void)RestoreCleanup();
					__SQLLEAVE(err);
					}
				inBufferPos += len;
				iFileSize -= len;
				if(iFileSize == 0)
					{
					iState = ERestoreComplete;
					}
				break;
				}
			case ERestoreComplete: // file completely restored
				{
				TUint64 checkSum64 = 0;
			    TInt restoreErr = iFile.Flush();
				if(restoreErr == KErrNone)
					{
					// calculate the checksum
					restoreErr = CheckSum(iFile, checkSum64);
					}
				iFile.Close();
				if(restoreErr != KErrNone)
					{
					(void)RestoreCleanup();
					__SQLLEAVE(restoreErr);
					}
				SQL_TRACE_BUR(OstTraceExt2(TRACE_INTERNALS, CSQLBACKUPCLIENT_RESTOREBASEDATASECTONL4, "0x%X;CSqlBurCallback::RestoreBaseDataSectionL;END;iFile.SubSessionHandle()=0x%X", (TUint)this, (TUint)iFile.SubSessionHandle()));
				TUint32 checkSum32 = checkSum64 & KMaxTUint32;
                if(checkSum32 != iChecksum)
                    {
					(void)RestoreCleanup();
                    __SQLLEAVE(KErrCorrupt);
                    }
				if((aInBuffer.Size() - inBufferPos) > 0)
					{//There are bytes to be consumed in the input buffer
					iState = ERestoreExpectChecksum;
					break;
					}
				SQL_TRACE_BUR(OstTrace1(TRACE_INTERNALS, CSQLBACKUPCLIENT_RESTOREBASEDATASECTONL5, "0x%X;CSqlBurCallback::RestoreBaseDataSectionL;aFinishedFlag=ETrue", (TUint)this));
				//End of data. We have all data restored in the restore folder.
				//The final step of the "restoring files" process consists of the following sub-steps:
				// - Rename the database file to be restored to a file with ".bak" extension
				// - Rename the file with the restored data to the database file
				// - Delete the file with ".bak" extension
				//Do not leave during the restore process! Restore as much files as possible.
				//The only excpetion is TParse::Set() - if it fails it is a fatal error, the
				//restored file path cannot be constructed.
				__ASSERT_DEBUG(iRestoreDrive != TDriveNumber(-1), __SQLPANIC(ESqlPanicInternalError));
				__ASSERT_DEBUG(iRestoreId != TSecureId(KNullUid), __SQLPANIC(ESqlPanicInternalError));
				//Include the aUid and the "*" mask
				TUidName uidName = (static_cast <TUid> (iRestoreId)).Name();
				TBuf<KMaxUidName + sizeof(KSqlBurAllFiles)> fileNameMask(uidName);
				fileNameMask.Append(KSqlBurAllFiles);
				__SQLLEAVE_IF_ERROR(iParse.Set(fileNameMask, &iRestoreDir, 0));
				CDir* dir = NULL;
				TPtrC searchPattern(iParse.FullName());
				SQL_TRACE_BUR(OstTraceExt2(TRACE_INTERNALS, CSQLBACKUPCLIENT_RESTOREBASEDATASECTONL55, "0x%X;CSqlBurCallback::RestoreBaseDataSectionL;search pattern=%S", (TUint)this, __SQLPRNSTR(searchPattern)));
				restoreErr = iInterface.Fs().GetDir(searchPattern, KEntryAttNormal, ESortNone, dir);
				if(restoreErr == KErrNone)
					{
					SQL_TRACE_BUR(OstTraceExt2(TRACE_INTERNALS, CSQLBACKUPCLIENT_RESTOREBASEDATASECTONL6, "0x%X;CSqlBurCallback::RestoreBaseDataSectionL;restored files=%d", (TUint)this, dir->Count()));
					for(TInt i=0;i<dir->Count();++i)
						{
						const TEntry& entry = (*dir)[i];
						__SQLLEAVE_IF_ERROR(iParse.Set(entry.iName, &iRestoreDir, 0));
						TFileName dbName(iParse.FullName());
						SQL_TRACE_BUR(OstTraceExt2(TRACE_INTERNALS, CSQLBACKUPCLIENT_RESTOREBASEDATASECTONL7, "0x%X;CSqlBurCallback::RestoreBaseDataSectionL;restored file=%S", (TUint)this, __SQLPRNSTR(dbName)));
						TInt pos = dbName.Find(KSqlBurRestoreDir);
						__ASSERT_DEBUG(pos >= 0, __SQLPANIC(ESqlPanicInternalError));
						dbName.Delete(pos, KSqlBurRestoreDir().Length() + 1);//"+1" for the path delimitier
						SQL_TRACE_BUR(OstTraceExt2(TRACE_INTERNALS, CSQLBACKUPCLIENT_RESTOREBASEDATASECTONL8, "0x%X;CSqlBurCallback::RestoreBaseDataSectionL;database=%S", (TUint)this, __SQLPRNSTR(dbName)));
						TFileName bakDbName(iParse.FullName());
						bakDbName.Append(KSqlBurBackupExt);
						SQL_TRACE_BUR(OstTraceExt2(TRACE_INTERNALS, CSQLBACKUPCLIENT_RESTOREBASEDATASECTONL9, "0x%X;CSqlBurCallback::RestoreBaseDataSectionL;backup file=%S", (TUint)this, __SQLPRNSTR(dbName)));
						//Now, dbName contains the original database (full path), iParse - the restored file,
						//bakDbName - backup file name
						TInt err = iInterface.Fs().Rename(dbName, bakDbName);
						if(err == KErrNone || err == KErrNotFound)
							{
							err = iInterface.Fs().Rename(iParse.FullName(), dbName);
							if(err == KErrNone)
								{//commit: delete the backup database file
								SQL_TRACE_BUR(OstTraceExt2(TRACE_INTERNALS, CSQLBACKUPCLIENT_RESTOREBASEDATASECTONL10, "0x%X;CSqlBurCallback::RestoreBaseDataSectionL;Commit;file=%S", (TUint)this, __SQLPRNSTR(dbName)));
								(void)iInterface.Fs().Delete(bakDbName);
								}
							else
								{//rollback: restore the original database file
								err = iInterface.Fs().Rename(bakDbName, dbName);
								SQL_TRACE_BUR(OstTraceExt3(TRACE_INTERNALS, CSQLBACKUPCLIENT_RESTOREBASEDATASECTONL11, "0x%X;CSqlBurCallback::RestoreBaseDataSectionL;Rollback;file=%S;err=%d", (TUint)this, __SQLPRNSTR(dbName), err));
								}
							}
						if(err != KErrNone && err != KErrNotFound)
							{
							if(restoreErr == KErrNone)
								{
								restoreErr = err;
								}
							}
						}//for(...)
					delete dir;
					}//iInterface.Fs().GetDir(...)
				done = ETrue;
				(void)RestoreCleanup();
				if(restoreErr != KErrNone)
					{
					__SQLLEAVE(restoreErr);
					}
				break;
				}
			default:
				__ASSERT_DEBUG(EFalse, __SQLPANIC(ESqlPanicInternalError));
				break;
			}//switch(...)
		if((aInBuffer.Size() - inBufferPos) == bytesAvailable)
			{//No bytes have been consumed from the buffer. 
			if(++iterations > 1 && !done)
				{//This is the second iteration in the loop where no bytes have been consumed from the input buffer. 
				 //But the "done" flag is still false. Corrupted archive.
				__SQLLEAVE(KErrCorrupt);
				}
			}
		} while(!done);
	}

/** 
The operation was terminated - we should tidyup here (as best we can)
Backup: close the file, free the allocated memory for the file names.
Restore: since the final restore step is a non-leaving one, nothing special needs to be done here - 
RestoreCleanup() is called to close the file and delete if there are any temporary files left.
*/	
void CSqlBurCallback::TerminateMultiStageOperation()
	{
	BackupCleanup();
	(void)RestoreCleanup();
	}

/** 
We do our own checksumming so we don't need this
@return the checksum
@param aDrive the drive affected (unused)
*/
TUint CSqlBurCallback::GetDataChecksum(TDriveNumber /* aDrive */)
	{
	// not required - not implemented
	const TUint KArbitraryNumber = 1024;
	return KArbitraryNumber;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////     Incremental backup/restore      ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** 
We don't support incremental backup
*/
void CSqlBurCallback::GetSnapshotDataL(TDriveNumber /* aDrive */, TPtr8& /* aBuffer */, TBool& /* aFinishedFlag */)
	{
	__SQLLEAVE(KErrNotSupported);
	}

/** 
We don't support incremental backup
*/
void CSqlBurCallback::InitialiseGetBackupDataL(TDriveNumber /* aDrive */)
	{
	__SQLLEAVE(KErrNotSupported);
	}

/** 
We don't support incremental backup
*/
void CSqlBurCallback::InitialiseRestoreBaseDataL(TDriveNumber /* aDrive */)
	{
	__SQLLEAVE(KErrNotSupported);
	}

/** 
We don't support incremental backup
*/
void CSqlBurCallback::InitialiseRestoreIncrementDataL(TDriveNumber /* aDrive */)
	{
	__SQLLEAVE(KErrNotSupported);
	}

/** 
We don't support incremental backup
*/
void CSqlBurCallback::RestoreIncrementDataSectionL(TDesC8& /* aBuffer */, TBool /* aFinishedFlag */)
	{
	__SQLLEAVE(KErrNotSupported);
	}

/** 
We don't support incremental backup
*/
void CSqlBurCallback::AllSnapshotsSuppliedL()
	{
	}

/** 
We don't support incremental backup
*/
void CSqlBurCallback::ReceiveSnapshotDataL(TDriveNumber /* aDrive */, TDesC8& /* aBuffer */, TBool /* aFinishedFlag */)
	{
	__SQLLEAVE(KErrNotSupported);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////     Helper functions      //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** 
A simple checksumming algorithm to allow a degree
of trust that the backup and restore worked.
Note the file pointer will be back at the start when the function call completes successfully.
In case of an error, the position of the file pointer is undetermined.

@param aOpenFile Already opened database file on which the checksum is calculated.
@param aCheckSum Output parameter. The checksum is returned in this parameter.
@return KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may also return some other system-wide error codes.
*/
TInt CSqlBurCallback::CheckSum(const RFile64& aOpenFile, TUint64& aCheckSum) const
	{
	// scoot through the database file building the checksum
	aCheckSum = 0;
	TInt64 seekPos = 0; // rewind first
	TInt err = aOpenFile.Seek(ESeekStart, seekPos);
	if(err != KErrNone)
		{
		return err;
		}
	const TUint KCheckSumBlockSize = 4 * 1024;
	HBufC8* buf = HBufC8::New(KCheckSumBlockSize);
	if(!buf)
		{
		return KErrNoMemory;
		}
	TPtr8 ptr = buf->Des();
	for(;;)
		{
		err = aOpenFile.Read(ptr);
		if(err != KErrNone)
			{
			delete buf;
			return err;
			}
		TInt len = ptr.Length();
		if(len == 0)
			{
			break;
			}
		// calculate the checksum
		for(TInt i=0;i<len;++i)
			{
			aCheckSum = (aCheckSum << 1) | (aCheckSum >> 63);
			aCheckSum += ptr[i];
 			}
		};		
	delete buf;
	// restore file position
	seekPos = 0;
	err = aOpenFile.Seek(ESeekStart,seekPos);
	return err;
	}

/**
Reads the content of aInBuf from position aInBufReadPos and stores the data into aOutBuf.
aDataLen is the length of the data. If the input buffer does not contain all the data, then only the
available data will be copied to the output buffer.

How the function works. It is called during the restore process and aInBuf parameter contains a block of raw
data sent by the B&R server. The calling function, RestoreBaseDataSectionL(), uses a state 
machine to processes the incoming data. At particular moment RestoreBaseDataSectionL() will process the data header 
and will have to read "aDataLen" 16-bit characters at position "aInBufReadPos". If there are "aDataLen" characters
at position "aInBufReadPos" and enough free space in "aOutBuf", CopyBufData() will copy all of them,  
otherwise CopyBufData() will copy as much characters as possible (in which case RestoreBaseDataSectionL() will
stay in the same state, waiting for more data from the B&R server).

@param aInBuf        8-bit buffer with input data
@param aInBufReadPos The position in the buffer from which the read operation starts. 
                     When the "buffer read" operatio completes, aInBufReadPos is updated with the 
                     number of bytes read from the input buffer. 
@param aOutBuf       16-bit output buffer. The data read from the input buffer is stored in the output buffer.
@param aDataLen      How much bytes to be read from the input buffer. Note that if there is not enough
                     data in the input buffer, the function will read as much as possible from the input buffer.
                     The aInBufReadPos in/out parameter will be updated with the actual number of bytes read.                     
*/
void CSqlBurCallback::CopyBufData(const TDesC8& aInBuf, TInt& aInBufReadPos, TDes& aOutBuf, TInt aDataLen)
	{
    __ASSERT_DEBUG(aInBufReadPos >= 0, __SQLPANIC(ESqlPanicBadArgument));
    __ASSERT_DEBUG(aDataLen > 0, __SQLPANIC(ESqlPanicBadArgument));
	
	TInt needed = (aDataLen - aOutBuf.Length()) << K8to16bitShift;
	TInt available = aInBuf.Size() - aInBufReadPos;
	TInt len = Min(needed, available);
	TPtrC8 ptr8 = aInBuf.Mid(aInBufReadPos, len);
	aInBufReadPos += len;
	
	len >>= K8to16bitShift;
	aOutBuf.Append((const TUint16*)ptr8.Ptr(), len);
	}

/**
Cleans up the allocated during the backup resources - file handles, buffers allocated for the file names.
*/
void CSqlBurCallback::BackupCleanup()
	{
	for(TInt i=0;i<iFileList.Count();++i)
		{
		delete iFileList[i];
		}
	iFileList.Close();
	iFile.Close();
	}

/**
Deletes created during the restore temporary files.
*/
TInt CSqlBurCallback::RestoreCleanup()
	{
	if(iRestoreDir.Find(KSqlBurRestoreDir) < 0)
		{//iRestoreDir is not initialized - that means RestoreCleanup() was called either from the
		 //destructor or from the TerminateMultistageOperation() during a backup.
		return KErrNone;
		}
	iFile.Close();
	CFileMan* fm = NULL;
	TRAPD(err, fm = CFileMan::NewL(iInterface.Fs()));
	if(err == KErrNone)
		{
		TFileName allFiles;
		allFiles.Copy(iRestoreDir);
		allFiles.Append(KSqlBurAllFiles);
		err = fm->Delete(allFiles);
		delete fm;
		}
	return err;
	}

/**
Stores the error occured during backup for furhter processing.
Please note that the function asserts if the aError parameter is KErrNone.
Call the function only with a real error.

@param aError The backup error to be stored 
*/
void CSqlBurCallback::SetBackupError(TInt aError)
	{
	__ASSERT_DEBUG(aError != KErrNone, __SQLPANIC(ESqlPanicBadArgument));
	if(aError != KErrNotFound && aError != KErrPathNotFound)
		{
		if(iBackupError == KErrNone || aError == KErrDiskFull || aError == KErrCorrupt)
			{
			iBackupError = aError;
			}
		}
	}
