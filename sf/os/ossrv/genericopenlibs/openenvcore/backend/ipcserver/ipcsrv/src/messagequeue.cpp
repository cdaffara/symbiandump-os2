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
// Name        : messagequeue.cpp
// Part of     : ipc server
// To implement the message queue
// 
//

#include "messagequeue.h"


//message queue handler class

MRequestHandler* CMsgQueueHandler::NewL()
	{
	return new(ELeave) CMsgQueueHandler;
	}

void CMsgQueueHandler::ServiceL(const RMessage2& aMessage)
	{
	CMessageQueue* tempMq;
	switch (aMessage.Function())
		{
		case EMsgGet:
			MsgGet(aMessage);
			break;

		case EMsgCtl:
			MsgCtl(aMessage);
			break;
		
		case EMsgRcv:

			TInt index;
			tempMq = FindById (aMessage.Int0 (), index);
			if ( tempMq)
				{
				if ( tempMq->CheckPerm( EUserRead | EOthersRead, aMessage) )
					{
					//message wil; be completed by SendRcv itself if required
					tempMq->iExternal = ETrue;
					tempMq->RcvMsg (aMessage);
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

		case EMsgSnd:
		
			TInt index2;
			tempMq	= FindById (aMessage.Int0 (), index2);
			if ( tempMq)
				{
				if ( tempMq->CheckPerm( EUserWrite | EOthersWrite, aMessage))
					{
					//message wil; be completed by SendMsg itself if required
					tempMq->iExternal = ETrue;
					tempMq->SndMsg (aMessage);
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
			
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	}

void CMsgQueueHandler::MsgCtl(const RMessage2& aMessage)
	{
	TInt index;
	CMessageQueue* tempMq = FindById (aMessage.Int0 (), index);
	if ( tempMq)
		{
		TInt ret = KErrNone;

		switch ( aMessage.Int1 ())
			{
			case IPC_STAT:

				if ( tempMq->CheckPerm (
						EUserRead | EOthersRead,
						aMessage))
					{
					msqid_ds buf;
					tempMq->GetStat (buf);
					TPckg<msqid_ds> pBuf(buf);
					aMessage.WriteL (2, pBuf);
					}
				else
					{
					ret = KErrAccessDenied;
					}
				break;

			case IPC_SET:

				if ( tempMq->IsCUidOrUid (aMessage))
					{
					msqid_ds buf;
					TPckg<msqid_ds> pBuf(buf);
					aMessage.ReadL (2, pBuf);
					tempMq->SetStat (buf);
					}
				else
					{
					ret = KErrPermissionDenied;
					}
				break;

			case IPC_RMID:

				if ( tempMq->IsCUidOrUid (aMessage))
					{
					delete tempMq;
					iMQueue.Remove(index);
					iMQueue.GranularCompress ();
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



void CMsgQueueHandler::MsgGet(const RMessage2& aMessage)
	{
	TInt tkey = aMessage.Int0 ();

	//		get flag to check whether we need to creat M Q	
	TInt tmsgflg = aMessage.Int1 ();

	
	TBool creatMq = EFalse;

	if ( IPC_PRIVATE == tkey)
		{
		creatMq = ETrue;
		}
	else
		{
		CMessageQueue* msqExist = FindByKey(tkey);
		if ( msqExist)
			{
			if ( (tmsgflg & IPC_CREAT) && ( tmsgflg & IPC_EXCL) )
				{
				aMessage.Complete (KErrAlreadyExists);
				return;
				}

			//compair the operation specified
			//with the operation of existing flag.
			//coverity[result_independent_of_operands]
			if ( !tmsgflg || (tmsgflg | IPC_CREAT == IPC_CREAT) ||
							msqExist->CheckPerm (tmsgflg, aMessage) )
				{
				TBuf8<MAXINT> temp;
				temp.AppendNum(msqExist->UniqueId ());
				aMessage.WriteL(2, temp);
				aMessage.Complete (KErrNone);
				return;
				}
			else
				{
				aMessage.Complete (KErrPermissionDenied);
				return;
				}

			}
		else
			{
			if ( tmsgflg & IPC_CREAT)
				{
				creatMq = ETrue;
				}
			else
				{
				aMessage.Complete (KErrNotFound);
				return;
				}
			}
		}
	if ( creatMq)
		{
		if ( KIpcMaxNumberOfQueues == iMQueue.Count ())
			{//max no of message queue is achived, 
			aMessage.Complete (KErrDiskFull);
			return;
			}
		//create a message queue
		CMessageQueue *tMsgQ = PosixIpcFactory::CreateMessageQueueL (aMessage);
		iMQueue.AppendL (tMsgQ);
		TBuf8<MAXINT> temp;
		temp.AppendNum (tMsgQ->UniqueId ());
		aMessage.WriteL (2, temp);
		aMessage.Complete (KErrNone);
		}

	}

//returns CMessageQueue pointer corresponding to the key
CMessageQueue* CMsgQueueHandler::FindByKey(const TInt& aKey)
	{
	TInt max =  iMQueue.Count();
	
	for (TInt idx = 0; idx < max; idx++  )
		{
		if ( iMQueue[idx]->Key() == aKey )
			{
			return iMQueue[idx];
			}
		}
	return NULL;
	}

//returns CMessageQueue pointer corresponding to the message Q id
CMessageQueue* CMsgQueueHandler::FindById(const TInt& aId, TInt& aIndex)
	{
	TInt max =  iMQueue.Count();
	
	for (TInt idx = 0; idx < max; idx++  )
		{
		if ( iMQueue[idx]->UniqueId() == aId )
			{
			aIndex = idx;			
			return iMQueue[idx];
			}
		}
	return NULL;
	}

//CMessageQueue class functions
CMessageQueue* CMessageQueue::NewL(const RMessage2& aMessage)
	{
	CMessageQueue* self = CMessageQueue::NewLC (aMessage);
	CleanupStack::Pop ();
	return self;
	}

CMessageQueue* CMessageQueue::NewLC(const RMessage2& aMessage)
	{
	CMessageQueue* self = new (ELeave) CMessageQueue;
	CleanupStack::PushL (self);
	self->ConstructL (aMessage);
	return self;
	}

void CMessageQueue::ConstructL(const RMessage2& aMessage)
	{
	//Generate a pseudo random number for the unique id
	iUniqueId = IpcUtils::GetRandom( );

	iMq_ds.msg_perm.key = aMessage.Int0();
	
	iMq_ds.msg_ctime = IpcUtils::CurrentTimeL();
	iMq_ds.msg_perm.seq = iUniqueId;
	iMq_ds.msg_perm.cuid = iMq_ds.msg_perm.uid = IpcUtils::GetPid(aMessage);
	iMq_ds.msg_perm.mode = aMessage.Int1();	//msgfld value
	}


TInt CMessageQueue::RcvMsg(const RMessage2& aMessage)
	{
	long msgType = 0;
	
	TPtr8 tptr((unsigned char*)&msgType, sizeof(long), sizeof(long));
	aMessage.ReadL(1, tptr);
	
	TBool msgFound = EFalse;
	msg* tmsg = NULL;
	msg* tprevMsg = NULL;

	
	if ( !iMq_ds.msg_first )
		{
		//if there is no message in the Q
		goto label1;
		}
		
	if ( !msgType )
		{//return first message from Q;
		//there will be at least one msg as we have 
		//checked for this in above if condition
				
		//set tmsg to first msg
		tmsg = iMq_ds.msg_first;
		msgFound = ETrue;
		}
	else if ( msgType > 0 )	
		{//		first message of msg type

		tmsg = iMq_ds.msg_first;
		msg* prev = NULL;
		while (tmsg)
			{
 			if ( tmsg->msg_type == msgType )
 				{
 				msgFound = ETrue;
 				tprevMsg = prev;
 				break;
 				}
 			prev = tmsg;
 			tmsg = tmsg->msg_next;
			}
		}
		else
			{//msgType is -ve
			 //smallet message which is of type< = |msgType|
			msg* tmsg2 = iMq_ds.msg_first;
			msg* prev = NULL;
//			decrease it by one so that we can apply uniform 
//			algo, to find smallest type message which is less then this no
			msgType--;
			msgType = -msgType;
			TInt lowestType = msgType;
			while (tmsg2)
				{
				if ( tmsg2->msg_type < lowestType)
					{
					tprevMsg = prev;
					tmsg = tmsg2;
					lowestType = tmsg2->msg_type;
					msgFound = ETrue;
					}
				prev = tmsg2;
				tmsg2 = tmsg2->msg_next;
				}
			}
	
label1:	if ( !msgFound )
		{
		if ( aMessage.Int2 ()& IPC_NOWAIT)
			{
			aMessage.Complete (ENOMSG);
			return ETrue;
			}
		else
			{
			//put this request on the queue
			  if ( iExternal )
				{
				iReadWaitQ.Append (aMessage);
				}
			  return EFalse;
			}
		}
	else
		{
		TInt msgSize = aMessage.GetDesLengthL(1);
		msgSize -= sizeof(long);
		TInt asize = tmsg->msg_len;
		if ( msgSize < tmsg->msg_len)
			{
			if ( aMessage.Int2() & MSG_NOERROR )
				{//truncate to msgSize
				tmsg->msg_len = msgSize;
				}
			else
				{
				aMessage.Complete(E2BIG);
				return ETrue;
				}
			}
		//update data struct
		iMq_ds.msg_lrpid = IpcUtils::GetPid(aMessage);
		iMq_ds.msg_qnum--;
		iMq_ds.msg_rtime = IpcUtils::CurrentTimeL();
		iMq_ds.msg_cbytes -=  (asize + sizeof(long));

		
		
		TPtr8 tptr( (unsigned char*)(tmsg->msg_data - sizeof(long)), tmsg->msg_len + sizeof(long), tmsg->msg_len + sizeof(long));
		aMessage.WriteL (1, tptr);
		aMessage.Complete (KErrNone);
		
		if ( tprevMsg)
			{
			tprevMsg->msg_next = tmsg->msg_next;
			// if this is last msg
			if ( tmsg == iMq_ds.msg_last)
				{
				iMq_ds.msg_last = tprevMsg;
				}
			}
		else
			{//this is first msg
			iMq_ds.msg_first = iMq_ds.msg_first->msg_next;
			//if first is null them last must be null
			if ( !iMq_ds.msg_first)
				{
				iMq_ds.msg_last = NULL;
				}
			}
		delete tmsg;
		
		//check if something is there on write Q
		//complete requests as much as you can 
		TInt max = iWriteWaitQ.Count();
		//todo if sndmsg is also succesful and it calls 
		//rcv again and that too successful then this for 
		//loop would be called again and this can go to a forever loop
		//and we end up deleting a element many times
		for (TInt i = 0; i < max; i++)
			{
			iExternal = EFalse;
			if ( SndMsg(iWriteWaitQ[i]) )
				{
				iWriteWaitQ.Remove(i);
				}
			}
		return ETrue;
		}
	}

TInt CMessageQueue::SndMsg(const RMessage2& aMessage)
	{
	TInt msgSize = aMessage.GetDesLengthL(1);
	
	if ( iMq_ds.msg_cbytes + msgSize  > iMq_ds.msg_qbytes )
		 {//if Queue is full
		  if ( aMessage.Int2() & IPC_NOWAIT )
			  {
			  aMessage.Complete(EAGAIN);
			  return ETrue;
			  }
		  else
			  {
			  //put this request on the queue
			  if ( iExternal )
				{
				iWriteWaitQ.Append (aMessage);
				}
			  return EFalse;
			  }
		 }
	else
		{
		iMq_ds.msg_lspid = IpcUtils::GetPid(aMessage);
		iMq_ds.msg_qnum++;
		iMq_ds.msg_stime = IpcUtils::CurrentTimeL();
		iMq_ds.msg_cbytes += msgSize;

		//create message
		msg* tmsg = new msg;
		User::LeaveIfNull(tmsg);
		CleanupStack::PushL(tmsg);
		tmsg->msg_data = new char[msgSize];
		User::LeaveIfNull(tmsg->msg_data);
		tmsg->msg_len = msgSize - sizeof(long int); 
		tmsg->msg_next = NULL;//as this will be last message in the Q
		TPtr8 tptr((unsigned char*)tmsg->msg_data, msgSize, msgSize);
		aMessage.ReadL(1, tptr);
		CleanupStack::Pop();
		tmsg->msg_type = *(long*)tmsg->msg_data;
		//move pointer to actual data.
		tmsg->msg_data = tmsg->msg_data + sizeof ( long int);
		
		aMessage.Complete(KErrNone);
		//add message in message link list
		//if message exist mean add after last one

		if ( iMq_ds.msg_last )
			{
			iMq_ds.msg_last->msg_next = tmsg;
			iMq_ds.msg_last = tmsg;
			}
		else
			{//there is no message in the Q
			iMq_ds.msg_first = iMq_ds.msg_last = tmsg;
			}
		
//		try to satisfy existing request if any we can do.
		TInt max = iReadWaitQ.Count();
		for (TInt i = 0; i < max; i++)
			{
			iExternal = EFalse;
			if ( RcvMsg(iReadWaitQ[i]) )
				{
				iReadWaitQ.Remove(i);
				}
			}
		return ETrue;
		}
	}

CMessageQueue::~CMessageQueue()
	{
	//delete all the message which are
	//present in this msg queue
	msg* cmsg = iMq_ds.msg_first;
	msg* tmsg = NULL;
	 while (cmsg)
		 {
		 tmsg = cmsg->msg_next;
		 delete cmsg;
		 cmsg = tmsg;
		 }
//	 should complete all the messages which are on read or write Q
//	 with errno = EIDRM	 
	 TInt i = 	iReadWaitQ.Count() - 1;
	 for ( ; i >= 0; i--)
		 {
		 iReadWaitQ[i].Complete(EIDRM);
		 }

	 i = 	iWriteWaitQ.Count() - 1;
	 for ( ; i >= 0; i--)
		 {
		 iWriteWaitQ[i].Complete(EIDRM);
		 }
	//empty up read and write Queue 
	iReadWaitQ.Close();
	iWriteWaitQ.Close();
	}

//changes msq_ds of this message queue as per aMsqDs
void CMessageQueue::SetStat(const msqid_ds& aMsqDs)
	{
	//gid is ignored
	iMq_ds.msg_perm.uid = aMsqDs.msg_perm.uid;
	iMq_ds.msg_perm.mode = aMsqDs.msg_perm.mode;
	iMq_ds.msg_qbytes    = aMsqDs.msg_qbytes;
	}

//gets msq_ds of this message queue into aMsqDs
void CMessageQueue::GetStat(msqid_ds& aMsqDs)
	{
	aMsqDs = iMq_ds;
	aMsqDs.msg_first = NULL;
	aMsqDs.msg_last = NULL;
	}



TBool CMessageQueue::CheckPerm(const TInt aMsgFlag, const RMessage2& aMessage)
	{
    unsigned short tId = IpcUtils::GetPid(aMessage);
    
	if ( tId ==	iMq_ds.msg_perm.cuid 
			|| tId == iMq_ds.msg_perm.uid )
		{
		//if no read write is set in aMsgFlag then denie permission
		if ( !(aMsgFlag & EUserRead)  && !(aMsgFlag & EUserWrite))
			{
			return EFalse;
			}
		//if its a creator or the permission is granted by creator (uid = application SecureId)
		if ( aMsgFlag & EUserRead )
			{
			if (! (iMq_ds.msg_perm.mode & EUserRead) )
				{
				return EFalse;				
				}
			}
		if ( aMsgFlag & EUserWrite )
			{
			if (! (iMq_ds.msg_perm.mode & EUserWrite) )
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
			if (! (iMq_ds.msg_perm.mode & EOthersRead) )
				{
				return EFalse;				
				}
			}
		
		if ( aMsgFlag & EOthersWrite )
			{
			if (! (iMq_ds.msg_perm.mode & EOthersWrite) )
				{
				return EFalse;				
				}
			}
		return ETrue;
		}
	}

TBool CMessageQueue::IsCUidOrUid(const RMessage2& aMessage)
	{
	TInt id = IpcUtils::GetPid(aMessage);
	return id == iMq_ds.msg_perm.cuid 
			|| id == iMq_ds.msg_perm.uid ;
	}

