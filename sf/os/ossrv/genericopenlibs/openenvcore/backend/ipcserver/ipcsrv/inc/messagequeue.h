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
* Name        : messagequeue.h
* Part of     : ipc server
* To implement the message queue
* 
*
*/



#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include <sys/msg.h>
#include "ipcutils.h"

class CMessageQueue;
class msg;

class CMsgQueueHandler : public CBase, MRequestHandler
	{
public:
	static MRequestHandler* NewL();
	void ServiceL(const RMessage2& aMessage);

private:
	//returns NULL if correspinding CMessageQueue object not found, otherwise valid *
	CMessageQueue* FindByKey(const TInt& aKey);
	CMessageQueue* FindById(const TInt& aId, TInt& aIndex);
	inline void MsgGet(const RMessage2& aMessage);
	inline void MsgCtl(const RMessage2& aMessage);

private:
	RPointerArray<CMessageQueue> iMQueue;
	};

	
class CMessageQueue : public CBase
	{
public:

	static CMessageQueue* NewL(const RMessage2& aMessage);
	static CMessageQueue* NewLC(const RMessage2& aMessage);

	void ConstructL(const RMessage2& aMessage);

	CMessageQueue()//
	{
	iMq_ds.msg_qbytes = KIpcMaxQueueSize;
	}

	virtual ~CMessageQueue();
	
	TInt SndMsg(const RMessage2& aMessage);
	TInt RcvMsg(const RMessage2& aMessage);
	
	TBool CheckPerm(const TInt aMsgFlag, const RMessage2& aMessage);

	inline TBool IsCUidOrUid(const RMessage2& aMessage);
	//	to get unique id of this message queue
	inline TUint32 UniqueId()
		{
		return iUniqueId;
		}

	inline TInt Key()
		{
		return iMq_ds.msg_perm.key;
		}

	void SetStat(const msqid_ds& aMsqDs);
	void GetStat(msqid_ds& aMsqDs);

public:
	TBool iExternal;
private:
	RArray<RMessage2> iReadWaitQ;
	RArray<RMessage2> iWriteWaitQ;
	msqid_ds iMq_ds;
	TInt iUniqueId;
	};

class msg
	{
public:
	msg():msg_data(NULL)
	{
	
	}
	virtual ~msg()
	{
	if ( msg_data )
		{
		delete (msg_data - sizeof(long int));
		}
	}
	struct msg* msg_next;
	long msg_type;
	short msg_len;
	//not null terminated, size id given by msg_len
	char* msg_data;
	};



	
#endif /*MESSAGEQUEUE_H_*/
	
