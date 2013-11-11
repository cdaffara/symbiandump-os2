// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sharedmem.cpp
// Part of     : ipc server
// To implement the shared memory
// 
//

#include <e32math.h>
#include "sharedmem.h"

//message queue handler class

MRequestHandler* CSharedMemHandler::NewL()
	{
	return new(ELeave) CSharedMemHandler;
	}

void CSharedMemHandler::ServiceL(const RMessage2& aMessage)
	{
	CSharedMem* tempShm = NULL;
	TInt index;

	switch (aMessage.Function())
		{
		case EShmGet:
			ShmGet(aMessage);
			break;

		case EShmCtl:

			ShmCtl(aMessage);
			break;
		
		case EShmAt:

			tempShm = FindById (aMessage.Int0 (), index);
			if ( tempShm && !tempShm->iExternal)
				{
				if ( tempShm->CheckPerm( EUserWrite | EOthersWrite, aMessage) )
					{
					tempShm->ShmAtL (aMessage);
					}
				else
					{
					aMessage.Complete (KErrPermissionDenied);
					}
				}
			else
				{
				aMessage.Complete (KErrArgument);
				}

			break;

		case EShmDt:
		
			tempShm = FindById (aMessage.Int0 (), index);
			if ( tempShm )
				{
				if ( tempShm->CheckPerm( EUserWrite | EOthersWrite, aMessage) )
					{
					tempShm->ShmDtL (aMessage);
					if ( !tempShm->Getnattch()  && tempShm->iExternal )
						{
						delete tempShm;
						iShmList.Remove(index);
						iShmList.GranularCompress ();
						aMessage.Complete (KErrNone);
						}
					}
				else
					{
					aMessage.Complete (KErrPermissionDenied);
					}
				}
			else
				{
				aMessage.Complete (KErrArgument);
				}

			break;
		
#ifdef SYMBIAN_OE_LIBRT
		case ESetShmSize:
			tempShm = FindByKey (aMessage.Int0());
			if (tempShm && !tempShm->iExternal)
				{
				if (tempShm->CheckPerm(EUserWrite | EOthersWrite, aMessage))
					{
					tempShm->SetSize(aMessage);
					aMessage.Complete(KErrNone);
					}
				else
					{
					aMessage.Complete(KErrPermissionDenied);
					}
				}
			else
				{
				aMessage.Complete(KErrArgument);
				}
			break;
			
		case EGetShmSize:
			tempShm = FindByKey(aMessage.Int0());
			if (tempShm && !tempShm->iExternal)
				{				
				TBuf8<MAXINT> temp;
				temp.AppendNum(tempShm->GetSize());
				aMessage.WriteL(1, temp);
				aMessage.Complete (KErrNone);
				}
			else
				{
				aMessage.Complete(KErrArgument);
				}
			break;
#endif //SYMBIAN_OE_LIBRT	
			
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	}

void CSharedMemHandler::ShmCtl(const RMessage2& aMessage)
	{
	TInt index;
	
	CSharedMem* tempShm = FindById (aMessage.Int0 (), index);
	if ( tempShm )
		{
		TInt ret = KErrNone;
  		switch ( aMessage.Int1 () )
			{
			case IPC_STAT:
				if ( tempShm->CheckPerm (
						EUserRead | EOthersRead,
						aMessage))
					{
					shmid_ds buf={0};
					tempShm->GetStat (buf);
					TPckg<shmid_ds> pBuf(buf);
					aMessage.WriteL (2, pBuf);
					}
				else
					{
					ret = KErrAccessDenied;
					}
				break;

			case IPC_SET:

				if ( tempShm->IsCUidOrUid (aMessage))
					{
					shmid_ds buf;
					TPckg<shmid_ds> pBuf(buf);
					aMessage.ReadL (2, pBuf);
					buf.shm_ctime = IpcUtils::CurrentTimeL();					
					tempShm->SetStat (buf);
					}
				else
					{
					ret = KErrPermissionDenied;
					}
				break;

			case IPC_RMID:

				if ( tempShm->IsCUidOrUid (aMessage) )
					{
					if( !tempShm->Getnattch() )
						{
						delete tempShm;
						iShmList.Remove(index);
						iShmList.GranularCompress ();
						}
					else
						{
						tempShm->iExternal = ETrue;
						}
					}
				else
					{
					ret = KErrPermissionDenied;		
					}
				break;
				//no need of default as client is 
				//checking for this
			}
		aMessage.Complete (ret);

		}
	else
		{
		aMessage.Complete (KErrArgument);
		}
	}



void CSharedMemHandler::ShmGet(const RMessage2& aMessage)
	{
	TInt tkey = aMessage.Int0 ();

	//	get flag to check whether we need to creat M Q	
	TInt tflg = aMessage.Int1 ();

	CSharedMem* shmExist = FindByKey (tkey);
	TBool creatMq = EFalse;

	if ( IPC_PRIVATE == tkey)
		{
		creatMq = ETrue;
		}
	else
		if ( shmExist)
			{
			if ( (tflg & IPC_CREAT) && ( tflg & IPC_EXCL))
				{
				aMessage.Complete (KErrAlreadyExists);
				return;
				}

			//compare the operation specified
			//with the operation of existing flag.
			if ( !tflg || (tflg | IPC_CREAT) == IPC_CREAT 
							|| shmExist->CheckSize(aMessage.Int3()))
				{
				if (shmExist->CheckPerm (tflg, aMessage) )
					{
					TBuf8<MAXINT> temp;
					temp.AppendNum(shmExist->UniqueId ());
					aMessage.WriteL(2, temp);
					aMessage.Complete (KErrNone);
					}
				else
					{
					aMessage.Complete (KErrPermissionDenied);
					}
				return;
				}
			else
				{
				aMessage.Complete (KErrArgument);
				return;
				}

			}
		else
			{
			if ( tflg & IPC_CREAT)
				{
				creatMq = ETrue;
				}
			else
				{
				aMessage.Complete (KErrNotFound);
				return;
				}
			}

	if ( creatMq)
		{
		if ( KIpcMaxShmObjects == iShmList.Count ())
			{
			//max no of message queue is achived, 
			aMessage.Complete (KErrDiskFull);
			return;
			}
		//create a shared memory object
		CSharedMem *tShm = PosixIpcFactory::CreateSharedMemoryL (aMessage);
		CleanupStack::PushL(tShm);
		iShmList.AppendL (tShm);
		TBuf8<MAXINT> temp;
		temp.AppendNum (tShm->UniqueId ());
//		todo - handle the case where writeL leaves
		aMessage.WriteL (2, temp);
		CleanupStack::Pop();
		aMessage.Complete (KErrNone);
		}
	}

//returns CSharedMem pointer corresponding to the key
CSharedMem* CSharedMemHandler::FindByKey(const TInt& aKey)
	{
	TInt max =  iShmList.Count();
	
	for (TInt idx = 0; idx < max; idx++  )
		{
		if ( iShmList[idx]->Key() == aKey )
			{
			return iShmList[idx];
			}
		}
	return NULL;
	}

//returns CSharedMem pointer corresponding to the message Q id
CSharedMem* CSharedMemHandler::FindById(const TInt& aId, TInt& aIndex)
	{
	TInt max =  iShmList.Count();

	for (TInt idx = 0; idx < max; idx++  )
		{
		if ( iShmList[idx]->UniqueId() == aId )
			{
			aIndex = idx;
			return iShmList[idx];
			}
		}
	return NULL;
	}

//CSharedMem class functions
CSharedMem* CSharedMem::NewL(const RMessage2& aMessage)
	{
	CSharedMem* self = CSharedMem::NewLC (aMessage);
	CleanupStack::Pop ();
	return self;
	}

CSharedMem* CSharedMem::NewLC(const RMessage2& aMessage)
	{
	CSharedMem* self = new (ELeave) CSharedMem;
	CleanupStack::PushL (self);
	self->ConstructL (aMessage);
	return self;
	}

void CSharedMem::ConstructL(const RMessage2& aMessage)
	{	
	//Generate a pseudo random number for the unique id
	iUniqueId = IpcUtils::GetRandom();
	
	iShm_ds.shm_segsz = aMessage.Int3();
	User::LeaveIfError(iChunk.CreateGlobal(KNullDesC, iShm_ds.shm_segsz, KIpcMaxShmSize));
	Mem::FillZ(iChunk.Base(), iShm_ds.shm_segsz);
	iShm_ds.shm_perm.key = aMessage.Int0();
	iShm_ds.shm_ctime = IpcUtils::CurrentTimeL();
	iShm_ds.shm_atime = iShm_ds.shm_dtime = 0;
	iShm_ds.shm_perm.seq = iUniqueId;
	iShm_ds.shm_perm.cuid = iShm_ds.shm_perm.uid = IpcUtils::GetPid(aMessage);
	iShm_ds.shm_perm.cgid = iShm_ds.shm_perm.gid = 0;
	iShm_ds.shm_perm.mode = aMessage.Int1();//msgfld value
	iShm_ds.shm_cpid = IpcUtils::GetPid(aMessage);
	iShm_ds.shm_lpid = 0;
	iShm_ds.shm_nattch = 0;
	}

bool CSharedMem::Getnattch()
	{
	if(iShm_ds.shm_nattch > 0 )
		{
		return true;
		}	
	return false;
	}
	

void CSharedMem::ShmAtL(const RMessage2& aMessage)
	{
	iShm_ds.shm_nattch++;
	iShm_ds.shm_atime = IpcUtils::CurrentTimeL();
	iShm_ds.shm_lpid = IpcUtils::GetPid(aMessage);
	aMessage.Complete(iChunk);
	}

void CSharedMem::ShmDtL(const RMessage2& aMessage)
	{
	iShm_ds.shm_nattch--;
	iShm_ds.shm_dtime = IpcUtils::CurrentTimeL();
	iShm_ds.shm_lpid = IpcUtils::GetPid(aMessage);
	if(!iExternal)
		{
		aMessage.Complete(KErrNone);
		}
	}

CSharedMem::~CSharedMem()
	{
	iChunk.Close();
	}

//changes msq_ds of this message queue as per aMsqDs
void CSharedMem::SetStat(const shmid_ds& aShmDs)
	{

	//gid is ignored
	iShm_ds.shm_perm.uid = aShmDs.shm_perm.uid;
	iShm_ds.shm_perm.mode = aShmDs.shm_perm.mode;
	iShm_ds.shm_lpid = aShmDs.shm_lpid;
	iShm_ds.shm_ctime = aShmDs.shm_ctime;
	}

//gets msq_ds of this message queue into aMsqDs
void CSharedMem::GetStat(shmid_ds& aShmDs)
	{
	TInt lpid = aShmDs.shm_lpid;
	aShmDs = iShm_ds;
	iShm_ds.shm_lpid = lpid;
	}


// function to check access permissions as specified by aMsgFlag


TBool CSharedMem::CheckPerm(const TInt aMsgFlag, const RMessage2& aMessage)
	{
    unsigned short tId = IpcUtils::GetPid(aMessage);
    
	if ( tId ==	iShm_ds.shm_perm.cuid 
			|| tId == iShm_ds.shm_perm.uid )
		{
		//if no read write is set in aMsgFlag then denie permission
		if ( !(aMsgFlag & EUserRead)  && !(aMsgFlag & EUserWrite))
			{
			return EFalse;
			}
		//if its a creator or the permission is granted by creator (uid = application SecureId)
		if ( aMsgFlag & EUserRead )
			{
			if (! (iShm_ds.shm_perm.mode & EUserRead) )
				{
				return EFalse;				
				}
			}
		if ( aMsgFlag & EUserWrite )
			{
			if (! (iShm_ds.shm_perm.mode & EUserWrite) )
				{
				return EFalse;				
				}
			}
		return ETrue;
		}
	else
		{// we should check for others permission

		//if no read write is set in aMsgFlag then denie permission
		if ( !(aMsgFlag & EOthersRead)  && !(aMsgFlag & EOthersWrite))
			{
			return EFalse;
			}
		
		if ( aMsgFlag & EOthersRead )
			{
			if (! (iShm_ds.shm_perm.mode & EOthersRead) )
				{
				return EFalse;				
				}
			}
		
		if ( aMsgFlag & EOthersWrite )
			{
			if (! (iShm_ds.shm_perm.mode & EOthersWrite) )
				{
				return EFalse;				
				}
			}
		return ETrue;
		}
	}

TBool CSharedMem::IsCUidOrUid(const RMessage2& aMessage)
	{
	TInt id = IpcUtils::GetPid(aMessage);
	return id == iShm_ds.shm_perm.cuid 
			|| id == iShm_ds.shm_perm.uid ;
	}

TBool CSharedMem::CheckSize(TInt aSize)
	{
	return  (iShm_ds.shm_segsz && (iShm_ds.shm_segsz >= aSize));
	}

#ifdef SYMBIAN_OE_LIBRT
void CSharedMem::SetSize(const RMessage2& aMessage)
	{
	iShmSize = aMessage.Int1();
	}

TInt CSharedMem::GetSize(void)
	{
	TInt size = iShmSize;
	return size;
	}
#endif //SYMBIAN_OE_LIBRT
