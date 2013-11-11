// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "UF_STD.H"
#include <e32math.h>

const TUid KUidDictionaryFile={0x100000E4};

_LIT(KSystemIniFileLocationSpec,"Z:\\System\\System.ini");

const TUid KSystemIniFileUid = {0x1000010C};

// Thread contention resolution constants

const TInt KSpinCount=40;
const TInt KLockoutLimit=0xc0000;	// to keep the lock-out time down to ~1sec (this is microseconds)
const TInt KFailTime=0x1000;		// ~ time to fail to open file
const TInt KSpinLimit=KLockoutLimit-KSpinCount*KFailTime;

LOCAL_C TUint8 const WaitDistribution[8]={0,0,0,0,0x1f,0x3f,0x7f,0xff};	// wait-time distribution mask

class RYieldThread : public RThread
	{
public:
	inline void Yield() const {SetPriority(Priority());}
	};

/////////////////////////////////////////////
// CDictionaryFileStore
/////////////////////////////////////////////

LOCAL_C void EnsurePathL(RFs& aFs,const TDesC& aName)
	{
	TInt r=aFs.MkDirAll(TParsePtrC(aName).DriveAndPath());
	if (r!=KErrAlreadyExists)
		User::LeaveIfError(r);
	}

EXPORT_C CDictionaryFileStore* CDictionaryFileStore::SystemL(RFs& aFs)
/** Opens the system dictionary file store.

@param aFs Handle to a file server session.
@return A pointer to the system file based dictionary store object. */
	{
	CDictionaryFileStore* store=SystemLC(aFs);
	CleanupStack::Pop();
	return store;
	}

EXPORT_C CDictionaryFileStore* CDictionaryFileStore::SystemLC(RFs& aFs)
/** Opens the system dictionary file store and puts the pointer to the file store 
object onto the cleanup stack.

@param aFs Handle to a file server session. 
@return A pointer to the system file based dictionary store object. */
	{
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	User::LeaveIfError(parse.Set(drive.Name(), &KSystemIniFileLocationSpec, NULL));
	
	EnsurePathL(aFs, parse.FullName());
	return OpenLC(aFs, parse.FullName(), KSystemIniFileUid);
	}

EXPORT_C CDictionaryFileStore* CDictionaryFileStore::OpenL(RFs& aFs,const TDesC& aName,TUid aFileId)
	/** Creates a file based dictionary store object.
	
	If the file with the specified full path name exists, then an attempt is made 
	to open an existing file store contained within this file. Any existing file 
	store must satisfy the following conditions:
	
	it must be a valid dictionary store
	
	the third UID component of the file store type must match the specified UID; 
	this UID serves to differentiate between dictionary stores
	
	otherwise the function leaves with KErrCorrupt.
	
	If the file with the specified full path name does not exist, then an attempt 
	is made to create a new file and to create a file based dictionary within 
	it. The third UID component of the file store type is set to the specified 
	UID value.
	
	Note that the file is opened in exclusive access mode.
	
	@param aFs Handle to a file server session. 
	@param aName The full path name of the file. 
	@param aUid3 The UID used to differentiate between dictionary stores.
	@return A pointer to the file based dictionary store object. 
	@see TUid
	@see TUidType */
	{
	CDictionaryFileStore* self = CDictionaryFileStore::OpenLC(aFs,aName,aFileId);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CDictionaryFileStore* CDictionaryFileStore::OpenLC(RFs& aFs,const TDesC& aName,TUid aFileId)
	/** Creates a file based dictionary store object and puts the pointer onto the 
	cleanup stack.
	
	If the file with the specified full path name exists, then an attempt is made 
	to open an existing file store contained within this file. Any existing file 
	store must satisfy the following conditions:
	
	it must be a valid dictionary store
	
	the third UID component of the file store type must match the specified UID; 
	this UID serves to differentiate between dictionary stores
	
	otherwise the function leaves with KErrCorrupt.
	
	If the file with the specified full path name does not exist, then an attempt 
	is made to create a new file and to create a file based dictionary within 
	it. The third UID component of the file store type is set to the specified 
	UID value.
	
	Note that the file is opened in exclusive access mode.
	
	@param aFs Handle to a file server session. 
	@param aName The full path name of the file. 
	@param aUid3 The UID used to differentiate between dictionary stores.
	@return A pointer to the file based dictionary store object. 
	@see TUid
	@see TUidType */
	{
	CDictionaryFileStore* self = new(ELeave) CDictionaryFileStore();
	CleanupStack::PushL(self);
	self->ConstructL(aFs,aName,aFileId);
	return self;
	}

void CDictionaryFileStore::ConstructL(RFs& aFs,const TDesC& aName,TUid aFileId)
//
// try to open the file - if this fails KErrNotFound try to create it
// if the file is in use retry after a pause
//
	{
	RYieldThread thread;
	TInt64 seed;
	const TUidType type(KPermanentFileStoreLayoutUid,KUidDictionaryFile,aFileId);
	for (TInt wait=KLockoutLimit;;)
		{
		RFile file;
        //  When the file server write caching is enabled, the order of file write operations is not guaranteed.
        // This could cause data inconsistency in some circumstances,for example, when the power is lost in the 
        // middle of a database transaction.Therefore, the file write caching is disabled for this file to maintain integrity.

		TInt r=file.Open(aFs,aName,EFileShareExclusive|EFileWrite|EFileWriteDirectIO);
		switch (r)
			{
		case KErrNone:
			{
			TInt size;
			if (file.Size(size)==KErrNone && size!=0)
				{
				CFileStore* store=NULL;
				TRAP(r,store=CPermanentFileStore::FromL(file));
			    if (r==KErrNotSupported||r==KErrEof)
					r=KErrCorrupt; // treat a bad store file as corrupt
			    else if (r==KErrNone && store->Type()!=type)
					{
					// treat a wrong 3rd UID as corrupt
					delete store;
					store = NULL;
					r=KErrCorrupt;
					}
				if (r==KErrCorrupt)
					{
				// silently replace the entire file if it is corrupt
				//  When the file server write caching is enabled, the order of file write operations is not guaranteed.
				// This could cause data inconsistency in some circumstances,for example, when the power is lost in the 
				// middle of a database transaction.Therefore, the file write caching is disabled for this file to maintain integrity.
				//   
					r=file.Replace(aFs,aName,EFileShareExclusive|EFileWrite|EFileWriteDirectIO);
					if (r==KErrInUse)
						break;  // try again later...
					if (r==KErrNone)
						{
						CreateStoreL(file,type);
						return;
						}
					}
				__LEAVE_IF_ERROR(r);
				__ASSERT_DEBUG(store != NULL, User::Invariant());
				//coverity[use_after_free]
    			iStore = store;
				if (store->Root()==KNullStreamId)
					CDictionaryStore::ConstructL();
				}
			else
				CreateStoreL(file,type);
			return;
			}
		case KErrNotFound:
            //  When the file server write caching is enabled, the order of file write operations is not guaranteed.
            // This could cause data inconsistency in some circumstances,for example, when the power is lost in the 
            // middle of a database transaction.Therefore, the file write caching is disabled for this file to maintain integrity.
			r=file.Create(aFs,aName,EFileShareExclusive|EFileWrite|EFileWriteDirectIO);
			if (r==KErrNone)
				{
				CreateStoreL(file,type);
				return;
				}
			else if (r==KErrAlreadyExists)
				;	// try and open after delay
			else
				__LEAVE(r);
			break;
		case KErrInUse:
			break;
		default:
			__LEAVE(r);
			}
		wait-=KFailTime;
		if (wait<=0)
			break;		// waited too long
		if (wait>KSpinLimit)
			{			// straight back to retry
			thread.Yield();	// force another reschedule
			continue;
			}
		// random wait time...
		if (wait==KSpinLimit)
			{	// initialise random number generator
			TThreadId id=thread.Id();
			TUint idVal=*(const TUint*)&id;
			seed = MAKE_TINT64(idVal^TUint(this),idVal^TUint(&id));
			Math::Rand(seed);
			Math::Rand(seed);
			}
		TInt pause=Math::Rand(seed)>>11;
		pause=(pause&WaitDistribution[(pause>>8)&0x7])<<10;
		if (pause)
			{
			wait-=pause;
			User::After(pause);
			}
		}
	__LEAVE(KErrInUse);
	}

void CDictionaryFileStore::CreateStoreL(RFile& aFile,const TUidType& aType)
	{
	CFileStore* store = CPermanentFileStore::NewL(aFile);
	iStore = store;
	store->SetTypeL(aType);
	CDictionaryStore::ConstructL();
	}
