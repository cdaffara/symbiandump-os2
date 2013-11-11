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
// Name        : semaphorequeue.cpp
// Part of     : ipc server
// To implement the semaphore queue
// 
//

#include "semaphorequeue.h"
#include "ipcsession.h"


//-------------------------------------------------------------------------------
// Function Name : 	CSemQueueHandler::ServiceL
// Description   : 	Function to delegate the responsiblity of possible operations
//	                on the semaphore.
//-------------------------------------------------------------------------------	
void CSemQueueHandler::ServiceL(const RMessage2& aMessage)
	{

	switch (aMessage.Function())
		{
		case ESemGet:
			SemGetL(aMessage);
			break;

		case ESemCtl:
			SemCtlL(aMessage);
			break;
		
		case ESemOp:
			SemOpL(aMessage);
			break;
			
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	}

//-------------------------------------------------------------------------------
// Function Name : 	CSemQueueHandler::NewL()
// Description   : 	Static function to create the handler for the semaphore
//	                
//-------------------------------------------------------------------------------	

MRequestHandler* CSemQueueHandler::NewL()
	{
	return new(ELeave) CSemQueueHandler;
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSemQueueHandler::SemGetL()
// Description   : 	Function to get the semaphore handle. If the semaphore is 
//	                already created with given key, the same sempahore id is 
//					returned if the user has not specified IPC_EXCL
//-------------------------------------------------------------------------------	

void CSemQueueHandler::SemGetL(const RMessage2& aMessage)
	{
		TInt tkey;
		tkey = aMessage.Int0();
		TInt tnsems = aMessage.Int1();
		TInt tflag = aMessage.Int2();
		TBool tcreatSem = EFalse;
		
		if ( IPC_PRIVATE == tkey)
			{
			tcreatSem = ETrue;
			}
		else
			{
			CSemaphoreQueue* tsemExist = FindByKey (tkey);
			if( tsemExist )
				{
				if ( (tflag & IPC_CREAT) && ( tflag & IPC_EXCL) )
					{
					aMessage.Complete (EEXIST);
					}
				else if ( tsemExist->NSems() < tnsems )
					{
					aMessage.Complete (EINVAL);
					}
				else if ( !tflag || (tflag | IPC_CREAT) == IPC_CREAT ||
									tsemExist->CheckPerm (tflag, aMessage) )
					{
					TBuf8<MAXINT> temp;
					temp.AppendNum(tsemExist->UniqueId ());
					aMessage.WriteL(3, temp);
					aMessage.Complete (KErrNone);
					}
				else
					{
					aMessage.Complete (EACCES);
					}
				return;
				}
			else
				{
				if ( tflag & IPC_CREAT )
					{
					tcreatSem = ETrue;
					}
				else
					{
					aMessage.Complete (ENOENT);
					return;
					}
				}
			}

		if ( tcreatSem )
			{
			if ( MaxNSems() + tnsems  > KIpcMaxSystemSem || iSQueue.Count() >= KIpcMaxSemArray)
				{
				//max no of semaphore queue is achived, 
				aMessage.Complete (ENOSPC);
				return;
				}
			if( tnsems > KIpcMaxSem || tnsems <= 0 )
				{
				aMessage.Complete (EINVAL);
				return;
				}
			//create a semaphore queue
			CSemaphoreQueue *tSemQ = PosixIpcFactory::CreateSemaphoreQueueL (aMessage);
			iSQueue.AppendL (tSemQ);
			TBuf8<MAXINT> temp;
			temp.AppendNum (tSemQ->UniqueId());
			aMessage.WriteL (3, temp);
			aMessage.Complete (KErrNone);
			}
		return;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CSemQueueHandler::SemOpL()
// Description   : 	Function to the perform the operation on the given semaphore.
//	                If all operation can be done, otherwise the requested is 
//					queued. 
//-------------------------------------------------------------------------------	

void CSemQueueHandler::SemOpL(const RMessage2& aMessage)
	{
		TInt tindex;
		TInt nsops = aMessage.Int2();
		TInt semid = aMessage.Int0 ();
		
		if( nsops > KIpcMaxSemOp )
			{
			aMessage.Complete(E2BIG);
			return;
			}
			
		if ( semid < 0 )
			{
			aMessage.Complete(EINVAL);
			return;			
			}
			
		RBuf8 buf;
		buf.CreateL(sizeof(struct sembuf)*nsops);
		CleanupClosePushL(buf);
		
		CSemaphoreQueue* tSemQ = FindById (semid, tindex);
		aMessage.ReadL(1, buf);
		struct sembuf *sops = (struct sembuf*) buf.Ptr();
		
		
		if(tSemQ)
			{
			TInt maxnsems = tSemQ->NSems();
			TInt j;
			//Check appropriate permission is there for the request
			for(j=0; j<nsops; j++)
				{
				TInt prem;
				if( sops[j].sem_op != 0 )
					{
					prem = tSemQ->CheckPerm(EUserWrite|EOthersWrite, aMessage);
					}
				else
					{
					prem = tSemQ->CheckPerm(EUserRead|EOthersRead, aMessage);
					}
				if( EFalse == prem )
					{
					aMessage.Complete(EACCES);
					break;;
					}
				if( sops[j].sem_num >=  maxnsems )
					{
					aMessage.Complete(EFBIG);
					break;
					}
				}
			if( j!=nsops )
				{
				CleanupStack::PopAndDestroy(&buf);
				return;
				}
			//Check if all the requested operations can be done 
			//as requested by the user.
			TInt astatus = tSemQ->CheckSemaphoreL(nsops, sops, ETrue);
			if( ESEM_SEMOPSUCCESS != astatus )
				{
				if( ESEM_NOBLOCK == astatus )
					{
					aMessage.Complete(EAGAIN);
					}
				else if( ESEM_LIMIT == astatus )
					{
					aMessage.Complete(ERANGE);
					}
				else
					{
					TRAPD(ret, tSemQ->AddL(aMessage));
					if( KErrNone != ret )
						{
						aMessage.Complete(ret);
						}
					}
					CleanupStack::PopAndDestroy(&buf);
					return;
				}
			else //Do the semop operation
				{
				TUint8 ret;
				ret = tSemQ->SemUndo(nsops, sops, aMessage);
				if( ret )
					{
					if( ret == ESEM_LIMIT )
						{
						aMessage.Complete(EINVAL);	
						}
					else
						{
						aMessage.Complete(ENOSPC);
						}
					CleanupStack::PopAndDestroy(&buf);
					return;
					}
				astatus = 0;
				tSemQ->ChangeSemaphoreVal(nsops, sops, astatus, aMessage);
				if( astatus & (ESEM_ZERO | ESEM_RELEASE) )
					{
					tSemQ->CheckOtherRequestL();
					}
				aMessage.Complete(KErrNone);
				CleanupStack::PopAndDestroy(&buf);
				return;
				}
			
			}
		else
			{
			aMessage.Complete(EINVAL);
			CleanupStack::PopAndDestroy(&buf);
			}
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSemQueueHandler::SemCtlL()
// Description   : 	Function to provide control operations over the semaphore. 
//	                Can get and set the values of the semaphores. 
//-------------------------------------------------------------------------------	

void CSemQueueHandler::SemCtlL(const RMessage2& aMessage)
	{
	TInt tindex;
	TInt result;
	TBuf8<MAXINT> temp;
	struct semid_ds tmp;
	
	TInt semid = aMessage.Int0();
	TInt semnum = aMessage.Int1();
	TInt cmd = aMessage.Int2();
	
			
	if( semid < 0 || semnum < 0 )
		{
		aMessage.Complete(EINVAL);
		return;
		}
	
	CSemaphoreQueue* tSemQ = FindById (semid, tindex);
	
	if( tSemQ )
		{
		
		if( semnum > tSemQ->NSems() )
		{
		aMessage.Complete(EINVAL);
		return;
		}
		
		switch(cmd)
			{
				case IPC_STAT:	
					if( tSemQ->CheckPerm(EUserRead|EOthersRead, aMessage) )
						{
						tSemQ->GetStat(tmp);
						TPtr8 ptr1((TUint8*)&tmp, sizeof(struct semid_ds), sizeof(struct semid_ds));
						aMessage.WriteL(3, ptr1);
						aMessage.Complete(KErrNone);
						}
					else
						{
						aMessage.Complete(EACCES);
						}
					break;
				case IPC_SET:
					if( tSemQ->CheckPerm(EUserWrite, aMessage) )
						{
						TPtr8 ptr2((TUint8*)&tmp, sizeof(struct semid_ds), sizeof(struct semid_ds));
						aMessage.ReadL(3,ptr2);
						tSemQ->SetStatL(tmp);
						aMessage.Complete(KErrNone);
						}
					else
						{
						aMessage.Complete(EPERM);
						}
					break;
					
				case SETVAL:
					if( tSemQ->CheckPerm(EUserWrite|EOthersWrite, aMessage) )
						{
						TInt val = aMessage.Int3();
						if( val < 0 || val > KIpcMaxSemVal )
							{
							aMessage.Complete(ERANGE);
							return;	
							}
						tSemQ->SetStatL(val, semnum, aMessage);
						aMessage.Complete(KErrNone);
						}
					else
						{
						aMessage.Complete(EACCES);
						}
					break;
				case SETALL:
					if( tSemQ->CheckPerm(EUserWrite|EOthersWrite, aMessage) )
						{
						RBuf8 buf;
						TUint16 *tmp;
						TInt count = tSemQ->NSems();
						buf.CreateL(sizeof(TUint16)*count);
						CleanupClosePushL(buf);
						aMessage.ReadL(3,buf);
						tmp = (TUint16*)buf.Ptr();
						for(TInt i = 0;i<count;i++)
							{
							if( tmp[i] > KIpcMaxSemVal )
								{
								aMessage.Complete(ERANGE);
								CleanupStack::PopAndDestroy(&buf);
								return;
								}
							}
						for(TInt i = 0;i<count;i++)
							{
							tSemQ->SetStatL(tmp[i], i, aMessage);
							}
						aMessage.Complete(KErrNone);
						CleanupStack::PopAndDestroy(&buf);
						}
						else
							{
							aMessage.Complete(EACCES);
							}
						break;
				case IPC_RMID:
					if( tSemQ->IsCUidOrUid(aMessage) )
						{
						iSQueue.Remove(tindex);
						tSemQ->Delete(aMessage);
						delete tSemQ;
						aMessage.Complete(KErrNone);
						}
					else
						{
						aMessage.Complete(EPERM);
						}
					break;
					
				default:
					if( tSemQ->CheckPerm(EUserRead|EOthersRead, aMessage) )
						{
						TInt ret;
						ret = tSemQ->GetStat(cmd, semnum, result);
						if( ret )
							{
							temp.AppendNum (result);
							aMessage.WriteL(3,temp);
							aMessage.Complete(KErrNone);	
							}
						else
							{
							aMessage.Complete(EINVAL);	
							}
						
						}
					else
						{
						aMessage.Complete(EACCES);
						}
					break;
			}
		}
		else
			{
			aMessage.Complete(EINVAL);
			}
	}
	

//-------------------------------------------------------------------------------
// Function Name : 	CSemQueueHandler::FindByKey()
// Description   : 	Helper function to find the handle to the semaphore based on 
//	                given key number. This key is given by the user at the time 
//					IPC creation.
//-------------------------------------------------------------------------------	

CSemaphoreQueue* CSemQueueHandler::FindByKey(const TInt& aKey)
	{
	TInt tmax = iSQueue.Count();

	for(TInt idx = 0; idx < tmax ; idx++)
		{
		if( iSQueue[idx]->Key() == aKey )
			{
			return iSQueue[idx];
			}
		}
	return NULL;
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSemQueueHandler::FindById()
// Description   : 	Find the handle to the semaphore based on the unique ID. This 
//	                ID is created with the given key by the user and a random 32
//					bit number. 
//-------------------------------------------------------------------------------	

CSemaphoreQueue* CSemQueueHandler::FindById(const TInt& aId, TInt& aIndex)
	{
	TInt tmax =  iSQueue.Count();
	for(TInt idx = 0; idx < tmax; idx++ )
		{
		if ( iSQueue[idx]->UniqueId() == aId )
			{
			aIndex = idx;			
			return iSQueue[idx];
			}
		}
	return NULL;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CSemQueueHandler::MaxNSems()
// Description   : 	This function shall get the total number of semaphore created
//	                in the whole system. This check may be required to check 
//					against the system imposed limit.
//-------------------------------------------------------------------------------	

TInt CSemQueueHandler::MaxNSems()
	{
	TInt max = 0;
	for(TInt i=0;i<iSQueue.Count();i++)
		{
		max += iSQueue[i]->NSems();
		}
	return max;
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSemQueueHandler::AdjustSemVal()
// Description   : 	This is cleanup function and will invoked at the time of 
//	                session clean.Each Session contain structure for each semapa
//					on which semop has been called with SEM_UNDO flag. This struc
//					shall contain the negated value of requested operation on the
//					semaphore.
//-------------------------------------------------------------------------------	

void CSemQueueHandler::AdjustSemVal(CSession2* asession)
	{
	TInt index;
	TInt ret;
	CSemaphoreQueue* tsemq;
	RPointerArray<CSemaphoreQueue> tsemque;
	
	TInt count = iSem_QAdjust.Count();
	
	for( TInt idx=0; idx<count;idx++ )
		{		
		if( iSem_QAdjust[idx]->CheckSessionPtr(asession) ) 
			{
			RPointerArray<SemAdj>& tsemadj = iSem_QAdjust[idx]->GetSemAdjStruct();
			CSemAdjust* tmp = iSem_QAdjust[idx];
			iSem_QAdjust.Remove(idx);
			for( TInt jdx=0;jdx<tsemadj.Count();jdx++ )
				{	
				tsemq = FindById(tsemadj[jdx]->semid,index);
				if( tsemq )
					{
					iTotalSemUndoCnt--;
					tsemq->AdjSemVal(tsemadj[jdx]->semnum, tsemadj[jdx]->adjval);		
					ret = tsemque.InsertInAddressOrder(tsemq);
					if(ret != KErrNone && ret != KErrAlreadyExists)
						{
						User::Leave(ret);
						}
					}
				}
			for( TInt jdx=0;jdx<tsemque.Count();jdx++ )
				{
				tsemque[jdx]->CheckOtherRequestL();
				}
			delete tmp;
			break;
			}
		}
	tsemque.Close();
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSemQueueHandler::CreateSemUndoStructL()
// Description   : 	This shall create the new CSemAdjust object if not created 
//	                with the given session otherwise will create SemAdj struct if
//					not created for the given SemId and SemNum.
//-------------------------------------------------------------------------------	

void CSemQueueHandler::CreateSemUndoStructL(CSession2* asession, TUint asemid, TUint8 asemnum, TUint16 aval)
	{
	TInt count = iSem_QAdjust.Count();
	
	for(TInt idx=0;idx<count;idx++)
		{
		if( iSem_QAdjust[idx]->CheckSessionPtr(asession) )
			{
			iSem_QAdjust[idx]->ChangeSemAjustValL(asemid, asemnum, aval);
			iTotalSemUndoCnt++;
			return;
			}
		}
	
	CSemAdjust *tmp = CSemAdjust::NewLC(asession, asemid, asemnum, aval);
	iSem_QAdjust.AppendL(tmp);
	CleanupStack::Pop ();
	iTotalSemUndoCnt++;
	return;
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSemQueueHandler::GetProcessSemUndoCnt()
// Description   : 	This function shall get the total number of semaphore created
//	                in the whole system. This check may be required to check 
//					against the system imposed limit.
//-------------------------------------------------------------------------------	
	
TInt CSemQueueHandler::GetProcessSemUndoCnt(CSession2* asession)
	{
	TInt count = iSem_QAdjust.Count();
	for(TInt i=0;i<count;i++)
		{
		if( iSem_QAdjust[i]->CheckSessionPtr(asession) )
			{
			return iSem_QAdjust[i]->GetSessionSemUndoCnt();
			}
		}
	return 0;
	}
	

//-------------------------------------------------------------------------------
// Function Name : 	CSemQueueHandler::DeleteSemUndoStruct()
// Description   : 	Shall delete the SemAdj structure when the semaphore value
//	                becomes 0 or when SETVAL is done for the particular semaphore
//					number.
//-------------------------------------------------------------------------------	
void CSemQueueHandler::DeleteSemUndoStruct(TUint asemid, TUint8 asemnum)
	{
	TInt count = iSem_QAdjust.Count();
	
	for( TInt idx=0; idx<count;idx++ )
		{		
		RPointerArray<SemAdj>& tsemadj=iSem_QAdjust[idx]->GetSemAdjStruct();
		for(TInt jdx=0;jdx<tsemadj.Count();jdx++)
			{
			if( tsemadj[jdx]->semid == asemid && tsemadj[jdx]->semnum == asemnum)
				{
				SemAdj* tmpsem = tsemadj[jdx];
				iTotalSemUndoCnt--;
				iSem_QAdjust[idx]->GetSessionSemUndoCnt()--;
				tsemadj.Remove(jdx);
				delete tmpsem;
				}
			}
		if( tsemadj.Count()==0 )
			{
			CSemAdjust* tmpcsem = iSem_QAdjust[idx];
			iSem_QAdjust.Remove(idx);
			delete tmpcsem;
			}
		}

	}



//-------------------------CSemaphoreQueue Class Implementation-----------------

//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::CheckPerm()
// Description   : 	This function shall check the permission to be performed on 
//	                the given semaphore. 
//-------------------------------------------------------------------------------	


TBool CSemaphoreQueue::CheckPerm(const TInt aSemFlag, const RMessage2& aMessage)
	{
    unsigned short tId = IpcUtils::GetPid(aMessage);
	if ( tId ==	isem_ds.sem_perm.cuid 
			|| tId == isem_ds.sem_perm.uid )
		{
		//if no read write is set in aMsgFlag then deny permission
		if ( !(aSemFlag & EUserRead)  && !(aSemFlag & EUserWrite))
			{
			return EFalse;
			}
		//if its a creator or the permission is granted by creator (uid = application SecureId)
		if ( aSemFlag & EUserRead )
			{
			if (! (isem_ds.sem_perm.mode & EUserRead) )
				{
				return EFalse;				
				}
			}
		if ( aSemFlag & EUserWrite )
			{
			if (! (isem_ds.sem_perm.mode & EUserWrite) )
				{
				return EFalse;				
				}
			}
		return ETrue;
		}
	else
		{// we should check for others permission

		//if no read write is set in aSemFlag then denie permission
		if ( !(aSemFlag & EOthersRead)  && !(aSemFlag & EOthersWrite))
			{
			return EFalse;
			}
		
		if ( aSemFlag & EOthersRead )
			{
			if (! (isem_ds.sem_perm.mode & EOthersRead) )
				{
				return EFalse;				
				}
			}
		
		if ( aSemFlag & EOthersWrite )
			{
			if (! (isem_ds.sem_perm.mode & EOthersWrite) )
				{
				return EFalse;				
				}
			}
		return ETrue;
		}
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::AddL()
// Description   : 	Utility function to add the requested to the maintained list 
//	                of requests.
//-------------------------------------------------------------------------------	
void CSemaphoreQueue::AddL( const RMessage2& aMessage )
	{
	isemWaitQ.AppendL(aMessage); 
	}

//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::NewL()
// Description   : 	Static function to support the 2 phase construction of the 
//	                semaphore object.
//-------------------------------------------------------------------------------	

CSemaphoreQueue* CSemaphoreQueue::NewL(const RMessage2& aMessage)
	{
	CSemaphoreQueue* self = CSemaphoreQueue::NewLC(aMessage);
	CleanupStack::Pop ();
	return self;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::NewLC()
// Description   : 	Static Function to support the 2 phase contruction of the 
//	                semaphore object.
//-------------------------------------------------------------------------------	
CSemaphoreQueue* CSemaphoreQueue::NewLC(const RMessage2& aMessage)
	{
	CSemaphoreQueue* self  = new (ELeave) CSemaphoreQueue;
	CleanupStack::PushL (self);
	self->ConstructL (aMessage);
	return self;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::ConstructL()
// Description   : 	The actual contruction of the semaphore object. This function
//	                implements the 2nd phase contruction of the object
//-------------------------------------------------------------------------------	
void CSemaphoreQueue::ConstructL(const RMessage2& aMessage)
	{
	//Generate a pseudo random number for the unique id
	iUniqueId = IpcUtils::GetRandom( );
	
	isem_ds.sem_perm.key = aMessage.Int0();
	isem_ds.sem_nsems = aMessage.Int1();
	isem_ds.sem_perm.mode = aMessage.Int2();
	isem_ds.sem_ctime = IpcUtils::CurrentTimeL();
	isem_ds.sem_otime = 0;
	//Create the array of semaphore as requested by user
	isem_ds.sem_base = new (ELeave) sem[isem_ds.sem_nsems];
	Mem::FillZ(isem_ds.sem_base,isem_ds.sem_nsems * sizeof(sem));
	isem_ds.sem_perm.seq = iUniqueId;
	isem_ds.sem_perm.cuid = isem_ds.sem_perm.uid = IpcUtils::GetPid(aMessage);
	//We do not support group
	isem_ds.sem_perm.gid = isem_ds.sem_perm.cgid = 0;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::ChangeSemaphoreVal()
// Description   : 	This function shall commit all changes to the semaphore value 
//	                as the user had requested in the semop call with the sembuf
//					structure. 
//-------------------------------------------------------------------------------	


void CSemaphoreQueue::ChangeSemaphoreVal (TInt nsops, struct sembuf *asops, 
											TInt& astatus, const RMessage2& aMessage)
	{
		astatus = 0;
		for(TInt i=0; i<nsops; i++)
			{
			isem_ds.sem_base->sempid = IpcUtils::GetPid(aMessage);
			if(asops[i].sem_op < 0)
				{					
				isem_ds.sem_base[asops[i].sem_num].semval += asops[i].sem_op;
				if( isem_ds.sem_base[asops[i].sem_num].semval == 0)
					{
					GetHandler(aMessage).DeleteSemUndoStruct(UniqueId(), asops[i].sem_num);
					astatus |= ESEM_ZERO;
					}
				continue;
				}
				else if( asops[i].sem_op > 0 )
					{
					isem_ds.sem_base[asops[i].sem_num].semval += asops[i].sem_op;
					astatus |= ESEM_RELEASE;
					continue;
					}
				else //if semaphore operation specified by the user is zero
					{
					continue;
					}		
			}
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::CheckSemaphoreL()
// Description   : 	This function shall check if the all the requested operations 
//	                by the user can be done. Either all or none shall be done and 
//					requested shall be blocked if sem_flg & IPC_NOWAIT is zero.
//-------------------------------------------------------------------------------	

TInt CSemaphoreQueue::CheckSemaphoreL(TInt nsops, struct sembuf *asops, TBool astat)
	{
	RArray<TInt> tsemval;
	for(TInt i = 0; i < isem_ds.sem_nsems; i++)
	{
		tsemval.Append(isem_ds.sem_base[i].semval);
	}
	
	isem_ds.sem_otime = IpcUtils::CurrentTimeL();
	for(TInt i = 0; i < nsops; i++)
		{
			if( asops[i].sem_op < 0 )
				{
					if( tsemval[asops[i].sem_num] + asops[i].sem_op < 0)
						{
							tsemval.Close();
							if( asops[i].sem_flg & IPC_NOWAIT )
								{
								return ESEM_NOBLOCK;
								}
							else
								{
								if(astat)
									{
									struct ReqData *tmp;
									tmp = new (ELeave) (struct ReqData);
									CleanupStack::PushL(tmp);
									tmp->index = asops[i].sem_num;
									tmp->value = asops[i].sem_op;
									iReq_QData.AppendL(tmp);
									CleanupStack::Pop();
									isem_ds.sem_base[asops[i].sem_num].semncnt++;
									}
								return ESEM_BLOCK;
								}
							
						}
					else
						{
						tsemval[asops[i].sem_num] += asops[i].sem_op;
						}					
				}
			else if( asops[i].sem_op > 0 )
				{
				tsemval[asops[i].sem_num] =+ asops[i].sem_op;
				if( tsemval[asops[i].sem_num] > KIpcMaxSemVal )
					{
					tsemval.Close();
					return ESEM_LIMIT;
					}
				}
			else
				{
					if(asops[i].sem_op == 0)
						{
						if( tsemval[asops[i].sem_num] != 0 )
							{
							tsemval.Close();
							if( asops[i].sem_flg &IPC_NOWAIT )
								{
								return ESEM_NOBLOCK;
								}	
							else
								{
								if(astat)
									{
									struct ReqData *tmp;
									tmp = new (ELeave) (struct ReqData);
									CleanupStack::PushL(tmp);
									tmp->index = asops[i].sem_num;
									tmp->value = asops[i].sem_op;
									iReq_QData.AppendL(tmp);
									CleanupStack::Pop();
									isem_ds.sem_base[asops[i].sem_num].semzcnt++;	
									}
								return ESEM_BLOCK;
								}
							}
						}
				}
		}
	tsemval.Close();
	return ESEM_SEMOPSUCCESS;
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::CheckSemUndoLimit()
// Description   : 	This function shall the system imposed limit for the sem_undo
//	                structure created for each session or process and overall
//					system imposed limit. On success sem undo structure shall be
//					created for each semaphore with SEM_UNDO flag
//-------------------------------------------------------------------------------	

TInt CSemaphoreQueue::SemUndo(TInt nsops, struct sembuf *asops, const RMessage2& aMessage)
	{
	TInt count = 0;
	for(TInt idx=0;idx<nsops;idx++)
		{
		if(asops[idx].sem_flg & SEM_UNDO)
			{	
			count++;
			}
		}
	
	if(0 == count)
		{
		return 0;
		}
	if( count + GetHandler(aMessage).GetSystemSemUndoCnt() > KIpcMaxSystemUn )
		{
		return ESEM_LIMIT;
		}
	else if ( count + GetHandler(aMessage).GetProcessSemUndoCnt(aMessage.Session()) > KIpcMaxUn )
		{
		return ESEM_NOSPACE;
		}
		
	for(TInt idx=0;idx<nsops; idx++)
		{
		if( asops[idx].sem_flg & SEM_UNDO )
			{
			GetHandler(aMessage).CreateSemUndoStructL(aMessage.Session(), UniqueId(), asops[idx].sem_num, asops[idx].sem_op);
			}
		}
	return 0;
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::CheckOtherRequestL()
// Description   : 	Function to check other requests which are queued, if they can
//	                be completed. The check shall be done for all the requests in
//					queue, till for any other request there is semaphore release 
//					semaphore value has become 0, which means the requests are 
//					again checked for 1st.
//-------------------------------------------------------------------------------	

void CSemaphoreQueue::CheckOtherRequestL()
	{
	TInt nsems;
	struct sembuf *sops;
	TInt status;
	TInt ret = 0;
	
	RBuf8 buf;
	buf.CreateL(sizeof(struct sembuf)*6);
	CleanupClosePushL(buf);
	
Label1: status = 0;
	for(TInt i=0;i<isemWaitQ.Count(); i++)
		{
		isemWaitQ[i].ReadL(1, buf);
		sops = (struct sembuf*) buf.Ptr();
		nsems = isemWaitQ[i].Int2();
		ret = CheckSemaphoreL(nsems, sops);
		if( ESEM_SEMOPSUCCESS == ret)
			{
			ret = SemUndo(nsems, sops, isemWaitQ[i]);
			if( ret )
				{
				if(ret == ESEM_LIMIT)
					{
					isemWaitQ[i].Complete(EINVAL);	
					}
				else
					{
					isemWaitQ[i].Complete(ENOSPC);
					}
				isemWaitQ.Remove(i);
				continue;
				}
			ChangeSemaphoreVal(nsems, sops, status, isemWaitQ[i]);
			struct ReqData* tmp = iReq_QData[i];
			if( tmp->value != 0 )
				{
				isem_ds.sem_base[tmp->index].semncnt--;
				}
			else
				{
				isem_ds.sem_base[tmp->index].semzcnt--;
				}
			iReq_QData.Remove(i);
			delete tmp;
			isemWaitQ[i].Complete(KErrNone);
			isemWaitQ.Remove(i);
			if( status & (ESEM_ZERO | ESEM_RELEASE) )
				{
				goto Label1;
				}
			}
		}		
	CleanupStack::PopAndDestroy(&buf);
	}
	

//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::SetStatL()
// Description   : 	Function to the set the value of the user id, group id and 
//	                permission mode.
//-------------------------------------------------------------------------------	

void CSemaphoreQueue::SetStatL(const semid_ds& aSemDs)
	{
	isem_ds.sem_ctime = IpcUtils::CurrentTimeL();
	isem_ds.sem_perm.uid = aSemDs.sem_perm.uid;
	isem_ds.sem_perm.gid = aSemDs.sem_perm.gid;
	isem_ds.sem_perm.mode = aSemDs.sem_perm.mode;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::SetStatL()
// Description   : 	Function to set the value to the semaphore.Setting the new val
//	                would mean to clean up any adjustment value set for the semaph
//					ore. Also any other ajustment val with other session also need
//					to be clean too.
//-------------------------------------------------------------------------------	

void CSemaphoreQueue::SetStatL(TInt aval, TInt aindex, const RMessage2& aMessage)
	{
	isem_ds.sem_ctime = IpcUtils::CurrentTimeL();
	isem_ds.sem_base[aindex].semval = aval;
	
	GetHandler(aMessage).DeleteSemUndoStruct(UniqueId(),aindex);
	
	//Check for other requests
	for(TInt i=0;i<isemWaitQ.Count();i++)
		{
		GetHandler(aMessage).DeleteSemUndoStruct(UniqueId(),aindex);
		}
	
	CheckOtherRequestL();
	return;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::GetStat()
// Description   : 	Gets the ipc_perm structure and semop time and time when last
//	                semaphore value was changed. Also gets the number of semaphor
//					es in the semaphore object
//-------------------------------------------------------------------------------	

void CSemaphoreQueue::GetStat(semid_ds& aSemDs)
	{
	memcpy(&(aSemDs.sem_perm), &(isem_ds.sem_perm), sizeof(struct ipc_perm)); 
	aSemDs.sem_nsems = isem_ds.sem_nsems;
	aSemDs.sem_otime = isem_ds.sem_otime;
	aSemDs.sem_ctime = isem_ds.sem_ctime;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::GetStat()
// Description   : 	Function to get the value of the specified semaphore object 
//	                member
//-------------------------------------------------------------------------------	

TBool CSemaphoreQueue::GetStat(TInt acmd, TInt aindex, TInt& aresult)
	{			
	switch(acmd)
		{
			case GETVAL:
				aresult = isem_ds.sem_base[aindex].semval;
				break;
			case GETPID:
				aresult = isem_ds.sem_base[aindex].sempid;
				break;
			case GETNCNT:
				aresult = isem_ds.sem_base[aindex].semncnt;
				break;
			case GETZCNT: 
				aresult = isem_ds.sem_base[aindex].semzcnt;
				break;
			case GETSIZE:
				aresult = isem_ds.sem_nsems;
				break;
			default:
				return EFalse;
		}
	return ETrue;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::Delete()
// Description   : 	Function to cleanup the semaphore object created. All the 
//	                requestes are completed and the sem undo structure maintained
//					at the session level is cleanup. 
//-------------------------------------------------------------------------------	
	
void CSemaphoreQueue::Delete(const RMessage2& aMessage)
	{
		TInt count = isemWaitQ.Count();
		TInt semid = UniqueId();
		
		for(TInt j=0;j<isem_ds.sem_nsems;j++)
			{
			GetHandler(aMessage).DeleteSemUndoStruct(semid,j);
			}
		
		for(TInt i=0; i<count;i++)
			{
			for(TInt j=0;j<isem_ds.sem_nsems;j++)
				{
				GetHandler(aMessage).DeleteSemUndoStruct(semid,j);	
				}
			isemWaitQ[i].Complete(EIDRM);
			}		
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::AdjSemVal()
// Description   : 	In case of session exit the adjustment value is added back to
//	                the semaphore value to restore its scantity.
//-------------------------------------------------------------------------------	

void CSemaphoreQueue::AdjSemVal(TInt aindex, TInt aval)
	{
	if((isem_ds.sem_base[aindex].semval+aval) >= 0)
		{
		isem_ds.sem_base[aindex].semval += aval;	
		}
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::~CSemaphoreQueue()
// Description   : 	Destructor to delete and Request status object queued for each
//	                request object 
//-------------------------------------------------------------------------------	

CSemaphoreQueue::~CSemaphoreQueue()
	{
	delete(isem_ds.sem_base);
	isemWaitQ.Close();
	iReq_QData.ResetAndDestroy();
	iReq_QData.Close();
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSemaphoreQueue::GetHandler()
// Description   : 	Returns the handle to the CSemQueueHandler object
//-------------------------------------------------------------------------------	
	
CSemQueueHandler& CSemaphoreQueue::GetHandler(const RMessage2& aMessage)
	{
	MRequestHandler *tsemqHandler = ((static_cast<CIpcSession*>(aMessage.Session()))->Server().GetSemHandler());
	CSemQueueHandler *tsemq = static_cast<CSemQueueHandler*>(tsemqHandler);
	return *tsemq;
	}



//------------------------------CSemAdjust--------------------------------------

//-------------------------------------------------------------------------------
// Function Name : 	CSemAdjust::NewLC()
// Description   : 	Static interface to create the CSemAdjust object.
//-------------------------------------------------------------------------------	

CSemAdjust* CSemAdjust::NewLC(CSession2* asession, TUint asemid, TUint8 asemnum, TUint16 aval)
	{
	CSemAdjust* self = new (ELeave) CSemAdjust;
	CleanupStack::PushL (self);
	self->ConstructL (asession, asemid, asemnum, aval);
	return self;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CSemAdjust::ConstructL()
// Description   : 	Support the 2 phase construction for the CSemAdjust object.
//-------------------------------------------------------------------------------	

void CSemAdjust::ConstructL(CSession2* asession, TUint asemid, TUint8 asemnum, TUint16 aval)
	{
	iSessionPtr = asession;
	SemAdj *tmp = new (ELeave) SemAdj;
	tmp->semid = asemid;
	tmp->semnum = asemnum;
	tmp->adjval = -1*aval;
	iSemAdj.AppendL(tmp);
	iSemUndoCnt = 1;
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSemAdjust::ChangeSemAjustValL()
// Description   : 	Static interface to create the CSemAdjust object.
//-------------------------------------------------------------------------------	

void CSemAdjust::ChangeSemAjustValL(TUint asemid, TUint8 asemnum, TUint16 aval)
	{
	SemAdj *tmp;
	TInt count = iSemAdj.Count();
	for(TInt idx=0;idx<count;idx++)
		{
		if(iSemAdj[idx]->semid == asemid && iSemAdj[idx]->semnum == asemnum)
			{
			iSemAdj[idx]->adjval -=  aval;
			return;
			}
		}
	tmp = new (ELeave) SemAdj;
	CleanupStack::PushL(tmp);
	tmp->semid  = asemid;
	tmp->semnum = asemnum;
	tmp->adjval = -1*aval;
	iSemAdj.AppendL(tmp);
	CleanupStack::Pop();
	iSemUndoCnt++;
	return;
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSemAdjust::~CSemAdjust()
// Description   : 	Destructor to clean up the object SemAdj object created. 
//-------------------------------------------------------------------------------	
CSemAdjust::~CSemAdjust()
	{
	iSemAdj.ResetAndDestroy();
	iSemAdj.Close();
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSemAdjust::GetSemAdjStruct()
// Description   : 	Get the handle to the SemAdj object for the required session 
//					pointer. Success return ETrue else EFalse.
//-------------------------------------------------------------------------------	
RPointerArray<SemAdj>& CSemAdjust::GetSemAdjStruct()
	{
		return iSemAdj;
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CSemAdjust::CheckSessionPtr()
// Description   : 	Checks for the given session pointer for the match. Success 
//					return 1 else 0.
//-------------------------------------------------------------------------------	
TBool CSemAdjust::CheckSessionPtr(CSession2* asession)
	{
	return (asession == iSessionPtr);
	}





