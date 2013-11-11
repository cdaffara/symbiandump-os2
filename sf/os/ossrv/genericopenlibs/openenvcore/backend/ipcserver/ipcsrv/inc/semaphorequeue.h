/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : semaphorequeue.h
* Part of     : ipc server
* To implement the semaphore queue
* 
*
*/



#ifndef SEMAPHORE_QUEUE_H_
#define SEMAPHORE_QUEUE_H_


#include <sys/sem.h>
#include "ipcutils.h"

enum TSemaphoreCheck
	{
	ESEM_ZERO = 1,
	ESEM_BLOCK = 2,
	ESEM_RELEASE = 4,
	ESEM_NOSPACE = 8,
	ESEM_LIMIT = 16,
	ESEM_NOBLOCK,
	ESEM_SEMOPSUCCESS 
	};
	
//structure to for the status of each blocked request
struct ReqData
{
	TUint8  index;   //Blocked on which semval
	TUint16 value;   //if non-zero then blocked for semval to increase by value
};

//structure for the semaphore adjustment value
struct SemAdj
{
	TUint	semid;	//Semaphore Id or IPC id
	TUint8	semnum;	//Semaphore number in the Array
	TInt16	adjval;	//The adjustment value
};

//CSemAdjust Class
class CSemAdjust : public CBase
	{
public:
	static CSemAdjust* NewLC(CSession2* asession, TUint asemid, TUint8 asemnum, TUint16 aval);
	void ConstructL(CSession2* asession, TUint asemid, TUint8 asemnum, TUint16 aval);
	~CSemAdjust();
	
	void ChangeSemAjustValL(TUint asemid, TUint8 asemnum, TUint16 aval);
	RPointerArray<SemAdj>& GetSemAdjStruct();
	TBool CheckSessionPtr(CSession2* asession);
	inline TInt& GetSessionSemUndoCnt(){return iSemUndoCnt;}
	
	
private:
	CSession2* iSessionPtr;
	RPointerArray<SemAdj> iSemAdj;
	TInt iSemUndoCnt;
	};

//Foward Declaration 	
class CSemQueueHandler;

//CSemaphoreQueue class
class CSemaphoreQueue : public CBase
	{
public:	
	static CSemaphoreQueue* NewL(const RMessage2& aMessage);
	static CSemaphoreQueue* NewLC(const RMessage2& aMessage);
	TBool CheckPerm(const TInt aMsgFlag, const RMessage2& aMessage);
	void ChangeSemaphoreVal (TInt nsops, struct sembuf *asops, TInt& astatus, const RMessage2& aMessage);
	TInt CheckSemaphoreL(TInt nsops, struct sembuf *asops, TBool astat=EFalse);
	void AddL(const RMessage2& aMessage);
	void CheckOtherRequestL (void);
	void ConstructL(const RMessage2& aMessage);
	void AdjSemVal(TInt asemnum, TInt aval);
	TInt SemUndo(TInt nsops, struct sembuf *asops, const RMessage2& aMessage);
	void Delete(const RMessage2& aMessage);
	CSemQueueHandler& GetHandler(const RMessage2& aMessage);

		
	// To check for rightfull owner of the IPC
	inline TBool IsCUidOrUid(const RMessage2& aMessage)
		{
		TInt id = IpcUtils::GetPid(aMessage);
		return id == isem_ds.sem_perm.cuid 
			|| id == isem_ds.sem_perm.uid ;
		}

	//	To get unique id of this Semaphore queue
	inline TUint32 UniqueId()
		{
		return iUniqueId;
		}
	//	To get the key value as given by the user
	inline TInt Key()
		{
		return isem_ds.sem_perm.key;
		}
	inline TInt NSems()
		{
		return isem_ds.sem_nsems;
		}

	void SetStatL(const semid_ds& aSemDs);
	void SetStatL(TInt aval, TInt aindex, const RMessage2& aMessage);
	void  GetStat(semid_ds& aSemDs);
	TBool GetStat(TInt acmd, TInt aindex, TInt& aresult);

	virtual ~CSemaphoreQueue();
		
private:
	RPointerArray<struct ReqData> iReq_QData;
	RArray<RMessage2> isemWaitQ;
	semid_ds isem_ds;
	TInt iUniqueId;
	};
	

//CSemQueueHandler
class CSemQueueHandler : public CBase, public MRequestHandler
	{
public:
	static MRequestHandler* NewL();
	void ServiceL(const RMessage2& aMessage);
	void AdjustSemVal(CSession2* asession);
	void CreateSemUndoStructL(CSession2* asession, TUint asemid, TUint8 asemnum, TUint16 aval);
	inline TInt GetSystemSemUndoCnt(){ return iTotalSemUndoCnt;}
	TInt GetProcessSemUndoCnt(CSession2* aSession);
	void DeleteSemUndoStruct(TUint asemid, TUint8 asemnum);
	

private:
	CSemaphoreQueue* FindByKey(const TInt& aKey);
	CSemaphoreQueue* FindById(const TInt& aId, TInt& aIndex);
	void SemGetL(const RMessage2& aMessage);
	void SemOpL(const RMessage2& aMessage);
	void SemCtlL(const RMessage2& aMessage);
	CSemQueueHandler(){iTotalSemUndoCnt = 0;}
	TInt MaxNSems();
	
private:
	TInt iTotalSemUndoCnt;
	RPointerArray<CSemaphoreQueue> iSQueue;
	RPointerArray<CSemAdjust> iSem_QAdjust;
	};
	
#endif /*SEMAPHORE_QUEUE_H_*/
