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
// Name        : ipcclient.cpp
// Part of     : ipc server
// ipc server - client interface 
// 
//



#include "ipcclient.h"
#include "ipcutils.h"
#include "lposix.h"
#include <sys/msg.h>
#include <sys/shm.h>
#include <e32atomics.h>
#include "sysif.h"


// A version must be specified when creating a session with the server

const TUint KIpcServerMajorVersionNumber = 1;
const TUint KIpcServerMinorVersionNumber = 0;
const TUint KIpcServerBuildVersionNumber = 0;


_LIT(KServerName,"LibCIpcServer");

const TUid KServerUid3={0x10207370};


//-------------------------------------------------------------------------------
// Function Name : 	StartServer()
// Description   : 	Static function Start the server process. Simultaneous 
//					launching of two such processes should be detected when the 
//					second one attempts to create the server object, failing with
//					KErrAlreadyExists.
//-------------------------------------------------------------------------------	


 

static TInt StartServer()
    {
    const TUidType serverUid(KNullUid,KNullUid,KServerUid3);
    RProcess server;
    TInt r = server.Create(KServerName,KNullDesC);
    if ( r != KErrNone )
    	{
    	return r;
    	}
    TRequestStatus stat;
    server.Rendezvous(stat);
    if ( stat != KRequestPending )
    	{
       	server.Kill(0);     // abort startup
    	}
    else
       	{
       	server.Resume();    // logon OK - start the server
       	}
    User::WaitForRequest(stat);     // wait for start or death
    
    // we can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    
    r = ( server.ExitType() == EExitPanic ) ? KErrGeneral : stat.Int();
    server.Close();
    return r;
    }

//-------------------------------------------------------------------------------
// Function Name : 	RIpcSession::Connect()
// Description   : 	The Client process uses this exported function to connect 
//					to the server. If the server is not started then the server
//					stared by calling the above static function. The client process
//					tills it gets signal from the server.
//-------------------------------------------------------------------------------	


TInt RIpcSession::Connect()
	{
	TInt retry = 2; // A maximum of two iterations of the loop required
	for(;;)
		{
			TInt r = CreateSession(KServerName,TVersion(KIpcServerMajorVersionNumber,
										KIpcServerMinorVersionNumber,
										KIpcServerBuildVersionNumber), -1,
										EIpcSession_Sharable);
						
			if ( (KErrNotFound != r) && (KErrServerTerminated != r) )
				{
				return r;
				}
			if ( --retry == 0 )
				{
				return r;
				}
			r = StartServer();
			if ( (KErrNone != r) && (KErrAlreadyExists != r) )
				{
				return r;
				}
		}
	}

//-------------------------------------------------------------------------------
// Function Name : 	RStdioSession::OnDemandConnection()
// Description   : 	To create connection to server on demand
//					
//-------------------------------------------------------------------------------	
TInt RIpcSession::OnDemandConnection()
	{
	/*
	 * Using memory barriers for safe double-checked locking. On SMP, the memory writes may be reordered between processors even if the compiler does it safely
	 */
	TInt err = KErrNone;
	TInt v = EFalse;
	if(__e32_atomic_load_acq32(&iIsConnected))		// read value, guaranteed to be observed before any subsequent memory accesses
		return err;	// someone else has constructed the object, so we are finished
	Lock();
	if (!iIsConnected)
		{
		err = Connect ();	// no - we must construct the object
		if ( KErrNone == err)
			{
			v = ETrue;		//update the value of p with iIsConnected later.
			}
		__e32_atomic_store_rel32(&iIsConnected, v); // write value, guaranteed to be observed after any preceding memory accesses
																		// i.e. accesses involved in constructing the object are observed first
																		// i.e. no-one sees a partially constructed object
		}
	UnLock();
	return err;
	}

//-------------------------------------------------------------------------------
// Function Name : 	RIpcSession::msgctl()
// Description   : 	To implement msgctl for msgqueue.
//					
//-------------------------------------------------------------------------------	
int RIpcSession::msgctl(int msqid, int cmd, struct msqid_ds *buf, int& aerrno)
	{
	TInt err = KErrNone;

	//if connection to server fails
	if (KErrNone != OnDemandConnection())
		{
		aerrno = EIO;
		return -1;
		}

	TPckg<msqid_ds> pBuf(*buf);	
	switch ( cmd)
		{
		case IPC_STAT:
		case IPC_SET:
		
			err = SendReceive (EMsgCtl, TIpcArgs (msqid, cmd, &pBuf));
			break;

		case IPC_RMID:
			err = SendReceive (EMsgCtl, TIpcArgs (msqid, cmd));
			break;

		default:
			//cmd is invalid
			aerrno = EINVAL;
			return -1;

		}

	if ( KErrNone != err)
		{
		MapError (err, aerrno);
		return -1;
		}
	
	return 0;
	}


//-------------------------------------------------------------------------------
// Function Name : 	RIpcSession::msgget()
// Description   : To implement msgget for message queue.
//					
//-------------------------------------------------------------------------------	
int RIpcSession::msgget(key_t key, int msgflg, int& aerrno)
	{
	TInt err = KErrNone;
	
	//if connection to server fails
	if (KErrNone != OnDemandConnection())
		{
		aerrno = EIO;
		return -1;
		}

	
	TBuf8<MAXINT> retBuf;
	err =	SendReceive(EMsgGet, TIpcArgs(key, msgflg, &retBuf));
	
	
	if ( KErrNone != err )
		{
		MapError(err, aerrno);
		return -1;
		}
	
	TInt ret;
	TLex8 lex(retBuf);
	lex.Val(ret	);
	return ret;
	}


//-------------------------------------------------------------------------------
// Function Name : 	RIpcSession::msgget()
// Description   : To implement msgget for message queue.
//					
//-------------------------------------------------------------------------------	
ssize_t RIpcSession::msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg , int& aerrno)
	{
	TInt err = KErrNone;
	//if connection to server fails
	if (KErrNone != OnDemandConnection())
		{
		aerrno = EIO;
		return -1;
		}
	
	*((long*)msgp) = msgtyp;
	
	//as msgsz is the size of actual msg and msgp also contains msg type 
	//which is of type long
	TPtr8 temp( (unsigned char*)msgp, msgsz + sizeof(long), msgsz + sizeof(long));
	
	err = SendReceive(EMsgRcv, TIpcArgs(msqid, &temp, msgflg));
	
	
	if ( err > 0 )
		{
		aerrno = err;
		return -1;
		}
	else if ( KErrNone != err )
		{
		MapError(err, aerrno);
		return -1;
		}
	
	return temp.Length() - sizeof(long);
	}


//-------------------------------------------------------------------------------
// Function Name : 	RIpcSession::msgget()
// Description   : To implement msgget for message queue.
//					
//-------------------------------------------------------------------------------	
int RIpcSession::msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg, int& aerrno)
	{
	
	
	if ( (int)msgsz < 0 || msgsz > KIpcMaxMsgSize || *((long*)msgp) < 1)
		{
		aerrno = EINVAL;
		return -1;
		}
	
	TInt err = KErrNone;
	//if connection to server fails
	if (KErrNone != OnDemandConnection())
		{
		aerrno = EIO;
		return -1;
		}
	//as msgsz is the size of actual msg and msgp also contains msg type 
	//which is of type long
	TPtr8 temp( (unsigned char*)msgp, msgsz + sizeof(long), msgsz + sizeof(long));
	TBuf8<MAXINT> retBuf;
	
	err = SendReceive(EMsgSnd, TIpcArgs(msqid, &temp, msgflg));
	
	if ( err > 0 )
		{
		aerrno = err;
		return -1;
		} else if ( KErrNone != err )
		{
		MapError(err, aerrno);
		return -1;
		}
	
	return 0;
	}

//shared memory APIs

//-------------------------------------------------------------------------------
// Function Name : 	RStdioSession::shmctl()
// Description   : 	To implement shmctl for shared memory.
//					
//-------------------------------------------------------------------------------	
int RIpcSession::shmctl(int shmid, int cmd, struct shmid_ds *buf, int& aerrno)
	{
	TInt err = KErrNone;

	//if connection to server fails
	if (KErrNone != OnDemandConnection())
		{
		aerrno = EIO;
		return -1;
		}
	
	TPckg<shmid_ds> pBuf(*buf);	
	switch ( cmd)
		{
		case IPC_STAT:
		case IPC_SET:
		
			err = SendReceive (EShmCtl, TIpcArgs (shmid, cmd, &pBuf));
			break;

		case IPC_RMID:
			err = SendReceive (EShmCtl, TIpcArgs (shmid, cmd));			
			if( err == KErrNone )
				{
				TChunk tChunk(shmid);
				Lock();
				TInt idx = iChunkList.Find (tChunk);
				if ( KErrNotFound != idx )
					{
					if ( --iChunkList[idx].iRef )
						{
						iChunkList[idx].iChunk.Close();
						iChunkList.Remove(idx);
						}
					}
				UnLock();
				}
			break;

		default:
			//cmd is invalid
			aerrno = EINVAL;
			return -1;

		}

	if ( KErrNone != err)
		{
		MapError (err, aerrno);
		return -1;
		}
	
	return 0;
	}


//-------------------------------------------------------------------------------
// Function Name : 	RIpcSession::shmget()
// Description   : To implement shmget for shared memory.
//					
//-------------------------------------------------------------------------------	
int RIpcSession::shmget(key_t key, size_t size, int shmflg, int& aerrno)
	{
	TInt err = KErrNone;
	
	if ( size <=0 || size > KIpcMaxShmSize)
		{
		aerrno = EINVAL;
		return -1;
		}
	//if connection to server fails
	if (KErrNone != OnDemandConnection())
		{
		aerrno = EIO;
		return -1;
		}
	
	//todo - optimisation - make a package and send
	
	TBuf8<MAXINT> retBuf;
	
	err =	SendReceive(EShmGet, TIpcArgs(key, shmflg, &retBuf, size));
		
	if ( KErrNone != err )
		{
		MapError(err, aerrno);
		return -1;
		}
	
	TInt ret;
	TLex8 lex(retBuf);
	lex.Val(ret	);
	return ret;
	}

//-------------------------------------------------------------------------------
// Function Name : 	RIpcSession::shmat()
// Description   : To implement shmat for shared memory.
//					shmaddr is ignored as this is not supported by Symbian.
//-------------------------------------------------------------------------------	
void* RIpcSession::shmat(int shmid, const void */*shmaddr*/, int /*shmflg*/, int& aerrno)
	{
	TInt err = KErrNone;
	
	//if connection to server fails
	if (KErrNone != OnDemandConnection())
		{
		aerrno = EIO;
		return NULL;
		}
	
	err = SendReceive(EShmAt, TIpcArgs(shmid));
	
	TAny* retPtr = reinterpret_cast<char*> (-1);
	
	err = AddToList(shmid, err, retPtr);

	if ( KErrNone != err )
		{
		MapError(err, aerrno);
		}

	return retPtr;
	}


//-------------------------------------------------------------------------------
// Function Name : RIpcSession::AddToList()
// Description   : Shall create the handle to the RChunk created at the IPC server
//				   or else if already created shall return it the base pointer.	
//-------------------------------------------------------------------------------	

TInt RIpcSession::AddToList(const TInt& aId, const TInt& aErr, TAny*& aRetPtr)
	{
	TChunk tChunk(aId);
	TInt err = KErrNone;
	Lock();
	TInt idx = iChunkList.Find (tChunk);
	
	if ( KErrNotFound == idx )
		{
		UnLock();
		RChunk lChunk;
		err = lChunk.SetReturnedHandle(aErr);
		if(KErrNone == err)
			{
			tChunk.iChunk = lChunk;
			err = tChunk.iChunk.Duplicate (RThread(), EOwnerProcess);
			if ( KErrNone == err)
				{
				aRetPtr = tChunk.iChunk.Base ();
				RHeap* oldHeap = User::SwitchHeap(Backend()->Heap());
				iChunkList.Append (tChunk);
				iChunkList[iChunkList.Find (tChunk)].iRef++;
				User::SwitchHeap(oldHeap);
				}
			lChunk.Close();	
			}
		}
	else
		{
		iChunkList[idx].iRef++;
		aRetPtr = iChunkList[idx].iChunk.Base();
		UnLock();
		}

	return err;
	}


//-------------------------------------------------------------------------------
// Function Name : 	RIpcSession::shmdt()
// Description   : To implement shmdt for shared memory.
//					
//-------------------------------------------------------------------------------	
int RIpcSession::shmdt(const void *shmaddr, int& aerrno)
	{
	TInt err = KErrNone;
	//if connection to server fails
	if (KErrNone != OnDemandConnection())
		{
		aerrno = EIO;
		return -1;
		}
	
	//find in list and then close RChunk and
	// ask server to update corresponding data.
	Lock();
	TInt i = iChunkList.Count() - 1 ;
	TBool found = EFalse;
	while( i != -1 )
		{
		if ( iChunkList[i].iChunk.Base() == shmaddr )
			{
			found = ETrue;
			break;
			}
		i--;
		}
	
	if ( found )
		{
		err = SendReceive(EShmDt, TIpcArgs(iChunkList[i].iShmID));

		if ( KErrNone == err)
			{
			//decrease reference count and if zero
			//close RChunk and remove TChunk 
			if (0 == (--iChunkList[i].iRef))
				{
				iChunkList[i].iChunk.Close();
				iChunkList.Remove(i);
				}
			}
		}
	else
		{
		aerrno = EINVAL;
		UnLock();
		return -1;
		}
	UnLock();
	if ( KErrNone != err )
		{
		MapError(err, aerrno);
		return -1;
		}
	
	return 0;
	}


//-------------------------------------------------------------------------------
// Function Name : 	RIpcSession::semget()
// Description   : 	Function to implement semget for semaphore.
//					
//-------------------------------------------------------------------------------


int RIpcSession::semget(key_t key, int nsems, int semflags, int& aerrno)
	{
	//if connection to server fails
	if (KErrNone != OnDemandConnection())
		{
		aerrno = EIO;
		return -1;
		}
		
	TBuf8<MAXINT> retBuf;
	TInt err =	SendReceive(ESemGet, TIpcArgs(key, nsems, semflags, &retBuf));
	
	if ( err > KErrNone )
		{
		aerrno = err;
		return -1;
		}
	
	TInt ret;
	TLex8 lex(retBuf);
	lex.Val(ret);

	return ret;
	}


//-------------------------------------------------------------------------------
// Function Name : 	RIpcSession::semop()
// Description   : 	Function to implement semop for the semaphore.
//					
//-------------------------------------------------------------------------------

int RIpcSession::semop(int semid, struct sembuf *sops, size_t nsops, int& aerrno)
	{
	//if connection to server fails
	if (KErrNone != OnDemandConnection())
		{
		aerrno = EIO;
		return -1;
		}
		
	TPtr8 aptr((TUint8*)sops, sizeof(struct sembuf) * nsops, sizeof(struct sembuf) * nsops);
			
	TInt err = SendReceive(ESemOp, TIpcArgs(semid, &aptr, nsops));
	
	if ( err > KErrNone )
		{
		aerrno = err;
		return -1;
		}
	return err;
	}


//-------------------------------------------------------------------------------
// Function Name : 	RIpcSession::semctl()
// Description   : 	To implement semctl for semaphore queue.
//					
//-------------------------------------------------------------------------------

int RIpcSession::semctl(int semid, int semnum, int cmd, union semun *arg, int& aerrno)
	{
	//if connection to server fails
	if (KErrNone != OnDemandConnection())
		{
		aerrno = EIO;
		return -1;
		}
		
	TInt ret1 = 0, ret2;
	TBuf8<MAXINT> buf;
	TLex8 lex;
	TInt err = KErrNone;
	switch(cmd)
		{
			case SETVAL:
				err = SendReceive(ESemCtl, TIpcArgs(semid, semnum, SETVAL, arg->val));
				break;
				
			case GETALL:
				err = SendReceive(ESemCtl, TIpcArgs(semid,1,GETSIZE, &buf));
					if(err)
					{
					break;
					}
				lex.Assign(buf);
				lex.Val(ret2);
				for (TInt i = 0; i < ret2 && !err; i++)
					{
					err = SendReceive(ESemCtl, TIpcArgs(semid, i, GETVAL, &buf));
					lex.Assign(buf);
					lex.Val(ret1);
					arg->array[i] = ret1;
					}
				ret1 = 0;
				break;
				
			case SETALL:
				err = SendReceive(ESemCtl, TIpcArgs(semid,1,GETSIZE, &buf));
				if(err)
					{
					break;
					}

				lex.Assign(buf);
				lex.Val(ret2);
				{
				TPtr8 ptr((TUint8*)arg->array, sizeof(unsigned short) * ret2, sizeof(unsigned short) * ret2);
				ret2--;
				err = SendReceive(ESemCtl, TIpcArgs(semid, ret2, SETALL, &ptr));
				}
				break;
				
			case IPC_STAT:
				{
				TPtr8 ptr2((TUint8*)arg->buf, 0, sizeof(struct semid_ds));
				err = SendReceive(ESemCtl, TIpcArgs(semid, semnum, IPC_STAT, &ptr2));
				}
				break;
				
			case IPC_SET:
				{
				TPtr8 ptr3((TUint8*)arg->buf, sizeof(struct semid_ds), sizeof(struct semid_ds));
				err = SendReceive(ESemCtl, TIpcArgs(semid,semnum,IPC_SET , &ptr3));
				}
				break;
			
			default:
				err = SendReceive(ESemCtl, TIpcArgs(semid, semnum, cmd, &buf));
				lex.Assign(buf);
				lex.Val(ret1);
				break;
		}
		
	if ( err > KErrNone )
		{
		aerrno = err;
		return -1;
		}
	return ret1;
	}

//-------------------------------------------------------------------------------
// Function Name : 	RIpcSession::RegisterSockAddrWithIPCSvr()
// Description   : 	Registers Local socket address with IPCS.
//					Returns -1 in case of error
//-------------------------------------------------------------------------------

TInt RIpcSession::RegisterSockAddrWithIPCSvr(TDesC8& aPath,TUint aPortNum)
	{

	if (KErrNone != OnDemandConnection())
		{
		return KErrCouldNotConnect;
		}

	return SendReceive(EFileSocketAddPath, TIpcArgs(aPortNum, &aPath));

	}

//-------------------------------------------------------------------------------
// Function Name : 	RIpcSession::GetLocalSockAddr()
// Description   : 	Get Local socket path associated with aPortNum from IPCS.
//					
//-------------------------------------------------------------------------------

TInt RIpcSession::GetLocalSockAddrByPort(TDes8& aAddr,TUint aPortNum)
	{
	
	if (KErrNone != OnDemandConnection())
		{
		return KErrCouldNotConnect;
		}

	return SendReceive(EFileSocketGetPathByPort, TIpcArgs(aPortNum, &aAddr));

	}
//-------------------------------------------------------------------------------
// Function Name : 	RIpcSession::GetLocalSockPortByPath()
// Description   : 	Get Local socket port associated with path from IPCS.
//-------------------------------------------------------------------------------

TInt RIpcSession::GetLocalSockPortByPath(TDesC8& aAddr,TUint& aPortNum)
	{
	
	if (KErrNone != OnDemandConnection())
		{
		return KErrCouldNotConnect;
		}
	
	TPckgBuf<TUint> pckg;
	TInt ret;
	ret = SendReceive(EFileSocketGetPortByPath, TIpcArgs(&aAddr,&pckg));
    if(ret == KErrNone)
    	{
    	aPortNum = pckg();
    	}
    
    return ret;
	}
//-------------------------------------------------------------------------------
// Function Name : 	RIpcSession::RemLocalSockAddr()
// Description   : 	Remove Local socket address from IPCS.
//-------------------------------------------------------------------------------

TInt RIpcSession::RemLocalSockAddr(TDesC8& aPath)
	{
	if (KErrNone != OnDemandConnection())
		{
		return KErrCouldNotConnect;
		}
	return SendReceive(EFileSocketRemovePath, TIpcArgs(&aPath));
	}

#ifdef SYMBIAN_OE_LIBRT
//-------------------------------------------------------------------------------
// Function Name : RIpcSession::SetShmSize()
// Description   : //To Do
//-------------------------------------------------------------------------------	

TInt RIpcSession::SetShmSize(TInt key, TInt size, TInt& aErr)
	{
	//if connection to server fails
	if (KErrNone != OnDemandConnection())
		{
		aErr = EIO;
		return -1;
		}
			
	TInt err = SendReceive(ESetShmSize, TIpcArgs(key, size));
	
	if ( err > KErrNone )
		{
		aErr = err;
		return -1;
		}
	return err;	
	}

//-------------------------------------------------------------------------------
// Function Name : RIpcSession::GetShmSize()
// Description   : //To Do
//-------------------------------------------------------------------------------	

TInt RIpcSession::GetShmSize(TInt key, TInt& aErr)
	{
	//if connection to server fails
	if (KErrNone != OnDemandConnection())
		{
		aErr = EIO;
		return -1;
		}
	TBuf8<MAXINT> retBuf;		
	TInt err = SendReceive(EGetShmSize, TIpcArgs(key, &retBuf));
	
	if ( err > KErrNone )
		{
		aErr = err;
		return -1;
		}
	TInt ret;
	TLex8 lex(retBuf);
	lex.Val(ret);
	return ret;
	}
#endif //SYMBIAN_OE_LIBRT
