/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/



#ifndef __RIPC_SESSION__
#define __RIPC_SESSION__

#include<e32std.h> //RSessionBase
#include<e32def.h> //Varargs
#include<sys/types.h>
#include<sys/sem.h>

struct msqid_ds;
struct shmid_ds;

class TChunk
	{
public:
TChunk(): iRef(0){}
TChunk(TInt aShmID): iShmID(aShmID), iRef(0){}
TInt iShmID;
RChunk iChunk;
TInt iRef;
	};


//-----------------------------------------------------------------------
//Class name: RIpcSession
//Description: It represents the session to Ipc Server. 
//-----------------------------------------------------------------------

class RIpcSession : public RSessionBase
	{
	public:
		RIpcSession():iIsConnected(EFalse)
			{iLock.CreateLocal();}
				
		//msgqueue functions
		int msgctl(int msqid, int cmd, struct msqid_ds *buf, int &aerrno);
		int msgget(key_t key, int msgflg, int& aerrno);
		ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg, int& aerrno);
		int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg, int& aerrno);
		
		//msgqueue functions
		int shmctl(int shmid, int cmd, struct shmid_ds *buf, int &aerrno);
		int shmget(key_t key, size_t size, int shmflg, int& aerrno);
		void *shmat(int shmid, const void *shmaddr, int shmflg, int& aerrno);
		int shmdt(const void *shmaddr, int& aerrno);
	
		//semqueue functions
		int semget(key_t key, int nsems, int semflags, int& aerrno);
		int semctl(int semid, int semnum, int cmd, union semun *arg, int& aerrno);
		int semop(int semid, struct sembuf *sops, size_t nsops, int& aerrno);
		
		RFastLock iLock;
	private:
		TInt AddToList(const TInt& aKey, const TInt& aErr, TAny*& aRetPtr);
		TInt OnDemandConnection();
		void Lock()
			{
			iLock.Wait();
			}
		void UnLock()
			{
			iLock.Signal();
			}
	private:

	TInt Connect();
	TInt iIsConnected;
	//todo put a lock around this list
	RArray<TChunk> iChunkList;
	};	
	
#endif //__RIPC_SESSION__

	
