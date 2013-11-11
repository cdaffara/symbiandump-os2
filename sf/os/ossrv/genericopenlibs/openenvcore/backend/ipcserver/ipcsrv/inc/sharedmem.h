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
* Name        : sharedmem.h
* Part of     : ipc server
* To implement the shared memory
* 
*
*/



#ifndef SHAREDMEM_H_
#define SHAREDMEM_H_

#include <sys/shm.h>
#include "ipcutils.h"

class CSharedMem;

class CSharedMemHandler : public CBase, MRequestHandler
	{
public:
	static MRequestHandler* NewL();
	void ServiceL(const RMessage2& aMessage);

private:
	//returns NULL if correspinding CMessageQueue object not found, otherwise valid *
	CSharedMem* FindByKey(const TInt& aKey);
	CSharedMem* FindById(const TInt& aId, TInt& aIndex);
	void ShmGet(const RMessage2& aMessage);
	void ShmCtl(const RMessage2& aMessage);

private:
	RPointerArray<CSharedMem> iShmList;
	};

	
class CSharedMem : public CBase
	{
public:

	static CSharedMem* NewL(const RMessage2& aMessage);
	static CSharedMem* NewLC(const RMessage2& aMessage);

	void ConstructL(const RMessage2& aMessage);

	virtual ~CSharedMem();
	
	void ShmAtL(const RMessage2& aMessage);
	void ShmDtL(const RMessage2& aMessage);
	
	TBool CheckPerm(const TInt aMsgFlag, const RMessage2& aMessage);

	inline TBool IsCUidOrUid(const RMessage2& aMessage);
	//	to get unique id of this message queue
	inline TUint32 UniqueId()
		{
		return iUniqueId;
		}

	inline TInt Key()
		{
		return iShm_ds.shm_perm.key;
		}

	inline TBool CheckSize(TInt aSize);
	
	void SetStat(const shmid_ds& aMsqDs);
	void GetStat(shmid_ds& aMsqDs);
	bool Getnattch();
#ifdef SYMBIAN_OE_LIBRT
	void SetSize(const RMessage2& aMessage);
	TInt GetSize(void);
#endif //SYMBIAN_OE_LIBRT
public:
	TBool iExternal;
private:
	shmid_ds iShm_ds;
	RChunk iChunk;
	TInt iUniqueId;
#ifdef SYMBIAN_OE_LIBRT
	TInt iShmSize;
#endif //SYMBIAN_OE_LIBRT
	};



#endif /*SHAREDMEM_H_*/
