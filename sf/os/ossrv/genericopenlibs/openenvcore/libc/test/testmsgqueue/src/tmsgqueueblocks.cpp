/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "tmsgqueue.h"
#include <string.h>
#include <unistd.h>
#define MESSAGE_Q_KEY 1000
#define MESSAGE_Q_KEY1 1001
#define MESSAGE_Q_KEY_TH 1002
#define WITH_ERROR 1
#define WITHOUT_ERROR 0
#define MSGSZ 128
#define IPC_KEY 1000
#define IPC_CREAT_EXCL 3000
#define IPC_NO_CREAT_EXCL 2000
#define USE_DEFAULT_ID 1

static void* ThreadEntryRead(TAny* arg);

TInt MsgQueueGet();
TInt MsgQueueThread(int a);
TInt MsgQueueSend(int a);
TInt MsgQueueRecieve();
TInt MsgQueueDelete();
        
#define RETURN_ON_ERROR(err) if ( err ) \
            { \
            ERR_PRINTF2 ( KFunc, err); \
            return err; \
            }
            
typedef struct 
		{
        long    mtype;
        char    mtext[4097];
        } message_buf;
         
TInt CTestMsgqueue::MsgKey(  TInt& aKey )
    {
    _LIT(KFunc, "MsgKey");
    INFO_PRINTF1 ( KFunc);
    _LIT( KaKey, "Param%d" );
    TBuf<8> pNameBuf;
    pNameBuf.Format(KaKey,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aKey);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve aKey") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral;
		}
    return KErrNone;
    }
    
TInt CTestMsgqueue::MsgFlag(  TInt& aFlag )
    {
    _LIT(KFunc, "MsgFlag");
    INFO_PRINTF1 ( KFunc);
    _LIT( KaFlag, "Param%d" );
	TBuf<16> pNameBuf;
    pNameBuf.Format(KaFlag,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aFlag);
	 
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve message flag") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral;
		}
    return KErrNone;
    }
TInt CTestMsgqueue::MsgId(  TInt& aId )
    {
    _LIT(KFunc, "MsgId");
    INFO_PRINTF1 ( KFunc);
    _LIT( KaId, "Param%d" );
	TBuf<16> pNameBuf;
    pNameBuf.Format(KaId,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aId);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve message Id") ;
	 	INFO_PRINTF1(Kerr) ;
		}
    if ( aId == USE_DEFAULT_ID )
        {
        aId = iNoOfMsqIds[0];
        }    
    return KErrNone;
    }

TInt CTestMsgqueue::MsgCmd(  TInt& aCmd )
    {
    _LIT(KFunc, "MsgCmd");
    INFO_PRINTF1( KFunc);
    _LIT( KaCmd, "Param%d" );
	TBuf<16> pNameBuf;
	pNameBuf.Format(KaCmd,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aCmd);
	
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve command") ;
	 	INFO_PRINTF1(Kerr) ;
		}
    return KErrNone;
    } 
TInt CTestMsgqueue::MsgType(  TInt& aType )
    {
    _LIT(KFunc, "MsgType");
    INFO_PRINTF1( KFunc);
    _LIT( KaType, "Param%d" );
	TBuf<16> pNameBuf;
    pNameBuf.Format(KaType,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aType);
	 
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve message type") ;
	 	INFO_PRINTF1(Kerr) ;
		return KErrGeneral;
		}
	 
    return KErrNone;
    }    
TInt CTestMsgqueue::MsgLen(  TInt& aLen )
    {
    _LIT(KFunc, "MsgLen");
    INFO_PRINTF1( KFunc);
    _LIT( KaLen, "Param%d" );
	TBuf<16> pNameBuf;
    pNameBuf.Format(KaLen,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aLen);
	 
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve message length") ;
	 	INFO_PRINTF1(Kerr) ;
		return KErrGeneral;
		}
    return KErrNone;
    }
TInt CTestMsgqueue::MsgSegs(  TInt& aSegs )
    {
    _LIT(KFunc, "MsgSegs");
    INFO_PRINTF1( KFunc);
    _LIT( KaSegs, "Param%d" );
	TBuf<16> pNameBuf;
    pNameBuf.Format(KaSegs,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aSegs);
	 
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve message flag") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral;
		}
    return KErrNone;
    }       
TInt CTestMsgqueue::ErrCode(  TInt& aErrCode )
    {
    _LIT(KFunc, "ErrCode");
    INFO_PRINTF1( KFunc);
    _LIT( KaErrCode, "Param%d" );
	TBuf<16> pNameBuf;
    pNameBuf.Format(KaErrCode,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aErrCode);
	 
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve error code") ;
	 	INFO_PRINTF1(Kerr) ;
		return KErrGeneral;
		}
    return KErrNone;
    }
    
TInt CTestMsgqueue::CheckMessageQueueStats(struct msqid_ds& buf)
	{
    _LIT(KFunc, "CheckMessageQueueStats");
    INFO_PRINTF1( KFunc);
    
    if ( buf.msg_cbytes != 0 )
        {
        return KErrGeneral;
        }
    if ( buf.msg_qnum != 0 )
        {
        return KErrGeneral;
        }
    if ( buf.msg_perm.key != IPC_KEY )
        {
        return KErrGeneral;
        }
    if ( buf.msg_qbytes != 2097152 )
        {
        return KErrGeneral;
        }   
    return KErrNone;
}

void CTestMsgqueue::SetMessageQueueParms(struct msqid_ds* /*buf*/)
	{
	}    

    
// -----------------------------------------------------------------------------
// CTestMsgqueue::MessageQueueCreate
// -----------------------------------------------------------------------------
//
TInt CTestMsgqueue::MessageQueueCreate(  )
    {
    _LIT(KFunc, "MessageQueueCreate");
    INFO_PRINTF1( KFunc);
    
    TInt key;
    TInt msgFlag;
    TInt msgSegs;
    TInt terror;
    TInt err;
    TInt i = 0;
    TInt msgid = -1;
    TInt retVal = KErrGeneral;
    
    // retrieve passed parameters
    err = MsgKey( key);
    RETURN_ON_ERROR(err);

    err = MsgFlag( msgFlag);
    RETURN_ON_ERROR(err);

    err = MsgSegs( msgSegs);
    RETURN_ON_ERROR(err);
    
    err = ErrCode( terror);
    RETURN_ON_ERROR(err);
    int startIndex = 0;
    TInt index = 0;
    for(index = 0; index < MAX_IPC_OBJ; index++)
        {
        if(iNoOfMsqIds[i] == -1)
            {
            startIndex = index;
            break;
            }
        }
    errno=0;
    for (i=0; i < msgSegs; i++)
        {
        msgid = msgget(key, msgFlag);
        if (msgid == -1)
			{
            break;        
			}
        iNoOfMsqIds[startIndex + i] = msgid;
        }    
    
    
    if (msgid == -1)
        {
        if(terror == WITHOUT_ERROR)
            {
            retVal = errno;
            }
        else
            {
            TInt errCode = 0;
            _LIT( KerrCode, "Param%d" );
			TBuf<8> pNameBuf;
		    pNameBuf.Format(KerrCode,++iParamCnt);
			TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, errCode);
			 
			if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve error code") ;
			 	INFO_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
		          
            if ( errCode == errno)
                {
                retVal = KErrNone;
                }
			}
        }
    else
        {
        if(terror != WITH_ERROR)
            {
            retVal = errno;
            }
        }
    	errno=0;    
    if(retVal != KErrNone && i < msgSegs && msgSegs > 1)
        {
        // do the cleanup
        for(index = startIndex; index < MAX_IPC_OBJ; index++)
            {
            if(iNoOfMsqIds[index] > 0)
				{
			
                msgctl(iNoOfMsqIds[index], IPC_RMID, 0);
				}
            }
        }        
        
	return retVal;
    }
    
// -----------------------------------------------------------------------------
// CTestMsgqueue::MessageQueueClose
// -----------------------------------------------------------------------------
//
TInt CTestMsgqueue::MessageQueueClose(  )
    {
    _LIT(KFunc, "MessageQueueClose");
    INFO_PRINTF1( KFunc);

    TInt retVal = KErrGeneral;
    TInt terror;
    TInt err;
    
    err = ErrCode( terror);
    RETURN_ON_ERROR(err);
    errno=0;
    for(TInt index = 0; index < MAX_IPC_OBJ; index++)
        {
        if(iNoOfMsqIds[index] >= 0)
			{
            err = msgctl(iNoOfMsqIds[index], IPC_RMID, 0);
			}
        }
    
    if (err == -1)
        {
        if(terror == WITHOUT_ERROR)
            {
            retVal = errno;
            }
        }
    else
        {
        errno = 0;
        if(terror != WITH_ERROR)
            {
            retVal = errno;
            }
        }
        
	return retVal;
    }

// -----------------------------------------------------------------------------
// CTestMsgqueue::MessageQueueControl
// -----------------------------------------------------------------------------
//
TInt CTestMsgqueue::MessageQueueControl(  )
    {
    _LIT(KFunc, "MessageQueueControl");
    INFO_PRINTF1( KFunc);
    
    TInt msqId;
    TInt msqCmd;
    TInt terror;
    TInt err;
    struct msqid_ds buf = {0};
    TInt retVal = KErrGeneral;
    
    // retrieve passed parameters
    err = MsgId( msqId);
    RETURN_ON_ERROR(err);
 
    err = MsgCmd( msqCmd);
    RETURN_ON_ERROR(err);
    
    if ( msqCmd == IPC_SET )
        {
        SetMessageQueueParms(&buf);
        }
    
    err = ErrCode( terror);
    RETURN_ON_ERROR(err);
    
    err = msgctl(msqId, msqCmd, &buf);
    
    if ( err == -1 )
        {
        if(terror == WITHOUT_ERROR)
            {
            retVal = errno;
            }
        else
            {
            TInt errCode = 0;
            _LIT( KerrCode, "Param%d" );
			 TBuf<8> pNameBuf;
		     pNameBuf.Format(KerrCode,++iParamCnt);
			 TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, errCode);
			 
			 if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve error code") ;
			 	INFO_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            if ( errCode == errno)
                {
                retVal = KErrNone;
                }
            }    
        }
    else
        {
        if ( msqCmd == IPC_STAT )
            {
            retVal = CheckMessageQueueStats(buf);
            }
        else if ( terror != WITH_ERROR )
            {
            retVal = KErrNone;
            }    
        }
    return retVal;
    }

void MessageCreate(TInt msgLen, TInt msgType, message_buf* buf  )
	{
    TInt i;
    
    buf->mtype = msgType;
    if (msgLen > 0 && msgLen <= 4096)
        {
        for (i=0; i< msgLen; i++)
            {
            buf->mtext[i] = 'a';
            }
        buf->mtext[i] = '\0';    
        }
    else 
        {
         buf->mtext[0] = '\0';
        }
	}
// -----------------------------------------------------------------------------
// CTestMsgqueue::CreateThread
// -----------------------------------------------------------------------------
//
TInt CTestMsgqueue::CreateThread(  )
	{
    _LIT(KFunc, "CreateThread");
    INFO_PRINTF1( KFunc);
    TInt threadType = -1;
    message_buf buf = {0};
    _LIT( KthreadType, "Param%d" );
	TBuf<16> pNameBuf;
	TInt err = KErrNone;
	pNameBuf.Format(KthreadType,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, threadType);
	
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve eror code") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    switch(threadType)
        {
        case 1: 
            iBlocked = 0;
            err = pthread_create((unsigned int *)&iThreadId,(pthread_attr_t *)NULL,ThreadEntryFunctionRead,(void*)this);
            if (!err)
                {
                err = SendMessage();
                }
            break;
        case 2:
            err = pthread_create((unsigned int *)&iThreadId,(pthread_attr_t *)NULL,ThreadEntryFunctionDestroyQ,(void*)this);
            if (!err)
                {
                err = RecvMessage();
                }
            break;
        case 3:
            iBlocked = 0;
            err = pthread_create((unsigned int *)&iThreadId,(pthread_attr_t *)NULL,ThreadEntryFunctionWrite,(void*)this);
            while(!iBlocked)
                {
                User::After(100000);
                }
            // fake msgctl...
            iBlocked = 0;
            msgctl(iNoOfMsqIds[0], IPC_STAT, (struct msqid_ds *)&buf);
            err = MessageQueueRecv();
            /*while(!iBlocked)
                {
                User::After(100000);
                }*/
            err = (iErrno) ?KErrGeneral : KErrNone;
            break;
        case 4:
            iBlocked = 0;
            err = pthread_create((unsigned int *)&iThreadId,(pthread_attr_t *)NULL,ThreadEntryFunctionWrite,(void*)this);
            while(!iBlocked)
                {
                User::After(100000);
                }
            // fake msgctl...
            iBlocked = 0; // again we want to block
            msgctl(iNoOfMsqIds[0], IPC_STAT, (struct msqid_ds *)&buf);
            err = msgctl(iNoOfMsqIds[0], IPC_RMID, 0);/*
            while(!iBlocked)
                {
                User::After(100000);
                }*/
            if (err == KErrNone) 
                {
                err = (iErrno == EIDRM) ?KErrNone : KErrGeneral;
                }
            break;            
        default:
            err = KErrNone;
            break;            
        }
    if ( err )
        {
        ERR_PRINTF2 ( _L("pthread_create error:%d"), err);
        }
    return err;
	}

// -----------------------------------------------------------------------------
// CTestMsgqueue::SendMessage
// -----------------------------------------------------------------------------
//
TInt CTestMsgqueue::SendMessage()
	{
    _LIT(KFunc, "SendMessage");
    INFO_PRINTF1( KFunc);
    
    TInt msqId;
    TInt msgType;
    TInt msqFlag;
    TInt terror;
    TInt err;
    message_buf buf = {0};
    TInt msgLen;
    TInt retVal = KErrGeneral;
    
    // retrieve passed parameters
    err = MsgId( msqId);
    RETURN_ON_ERROR(err);
  
    err = MsgLen( msgLen);
    RETURN_ON_ERROR(err);
     
    err = MsgType( msgType);
    RETURN_ON_ERROR(err);
    
    err = MsgFlag( msqFlag);
    RETURN_ON_ERROR(err);
    
    err = ErrCode( terror);
    RETURN_ON_ERROR(err);
    
    MessageCreate(msgLen, msgType, &buf);
    
    while (iBlocked == 0)
        {
        User::After(100000);
        }
    err = msgsnd(msqId, &buf, msgLen, msqFlag);
    
    if ( err == -1 )
        {
        if(terror == WITHOUT_ERROR)
            {
            retVal = errno;
            }
        else
            {
            TInt errCode = 0;
            _LIT( KerrCode, "Param%d" );
			 TBuf<8> pNameBuf;
		     pNameBuf.Format(KerrCode,++iParamCnt);
			 TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, errCode);
			 
			 if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve error code") ;
			 	INFO_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            if ( errCode == errno)
                {
                retVal = KErrNone;
                }
            }    
        }
    else
        {
        if ( terror != WITH_ERROR)
            {
            TInt tmout = 0;
            while (iBlocked && (tmout < 5))
                {
                User::After(10000);
                tmout++;
                }
            retVal = (iBlocked == 0) ?KErrNone: KErrGeneral;    
            }       
        }

    return retVal;
	}
// -----------------------------------------------------------------------------
// CTestMsgqueue::RecvMessage
// -----------------------------------------------------------------------------
//
TInt CTestMsgqueue::RecvMessage()
	{
    _LIT(KFunc, "RecvMessage");
    INFO_PRINTF1( KFunc);
    
    TInt msgType;
    TInt msqFlag = 0;
    TInt err;
    TInt terror;
    message_buf buf = {0};
    TInt msgLen = 128;
    TInt retVal = KErrGeneral;
    
    err = ErrCode( terror);
    RETURN_ON_ERROR(err);
    
    msgType = 0; //any message
    msqFlag =  ~IPC_NOWAIT; //Blocking
    
    iBlocked = 1;
    
    
    char array[128+1];
    for(int idx=0; idx<128; idx++)
	    {
    	array[idx]='a';    	
    	}
    array[128]='\0';	

	    
    err = msgrcv(iNoOfMsqIds[0], &buf, msgLen, msgType, msqFlag);    
    while (!iBlocked)
        {
        User::After(10000);
        }

    if ( err == -1 )
        {
        if(terror == WITHOUT_ERROR)
            {
            retVal = errno;
            }
        else
            {
            TInt errCode = 0;
            _LIT( KerrCode, "Param%d" );
			 TBuf<8> pNameBuf;
		     pNameBuf.Format(KerrCode,++iParamCnt);
			 TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, errCode);
			 
			 if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve error code") ;
			 	INFO_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            if ( errCode == errno)
                {
                retVal = KErrNone;
                }
            }    
        }
    else
        {
        if ( terror != WITH_ERROR && (msgLen>0 && msgLen<256) && !(strncmp(&buf.mtext[0], &array[0], msgLen)))
		    {            
            retVal = KErrNone;    
            }       
        }

    return retVal;
	}
//-----------------------------------------------------------------------------
//CTestMsgqueue::ThreadEntryFunctionRead
//-----------------------------------------------------------------------------
     
void* CTestMsgqueue::ThreadEntryFunctionRead(void* arg)
	{    
    TInt msgType;
    TInt msqFlag = 0;
    TInt err;
    message_buf buf = {0};
    TInt msgLen = 128;
    CTestMsgqueue *self = static_cast<CTestMsgqueue*> (arg);
    
    TInt msqId = msgget(IPC_KEY, 0);
    if ( msqId < 0)
        {
        return NULL;
        }
    msgType = 0; //any message
    msqFlag =  ~IPC_NOWAIT; //Blocking
    
    self->iBlocked = 1;    
    
    err = msgrcv(msqId, &buf, msgLen, msgType, msqFlag);
    
    if ((err > 0) && (self->iBlocked))
        {
        self->iBlocked = 0;
        }    
    return NULL;
	}
//-----------------------------------------------------------------------------
//CTestMsgqueue::ThreadEntryFunctionWrite
//-----------------------------------------------------------------------------
     
void* CTestMsgqueue::ThreadEntryFunctionDestroyQ(void* arg)
	{    
    message_buf buf = {0};
    CTestMsgqueue *self = static_cast<CTestMsgqueue*> (arg);
    
    while (!self->iBlocked)
        {
        User::After(10000);
        }
    self->iBlocked = 0;
    TInt msqId = msgget(IPC_KEY, 0);
    if ( msqId < 0)
        {
        return NULL;
        }
    
    msgctl(msqId, IPC_STAT, (struct msqid_ds *)&buf);
    msgctl(msqId, IPC_RMID, 0);
    self->iBlocked = 1;
    
    return NULL;
	}
//-----------------------------------------------------------------------------
//CTestMsgqueue::ThreadEntryFunctionWrite
//-----------------------------------------------------------------------------
     
void* CTestMsgqueue::ThreadEntryFunctionWrite(void* arg)
	{  
    TInt msgType;
    TInt msqFlag = 0;
    HBufC8* messPtr = HBufC8::NewLC(65295);
    char *bufPtr = (char*)messPtr->Des().PtrZ();
    
    struct msg_buf{
         long    mtype;
         char    mtext[1];
         };
    struct msg_buf *pMess = (struct msg_buf *)bufPtr;
    TInt msgLen = 4090;
    CTestMsgqueue *self = static_cast<CTestMsgqueue*> (arg);
    
    TInt msqId = msgget(IPC_KEY, 0);
    if ( msqId < 0)
        {
        return NULL;
        }
    msgType = 1; //any message
    msqFlag =  ~IPC_NOWAIT; //Blocking
    
    memset(pMess, 0, 65295);
    pMess->mtype = msgType;
    pMess->mtext[msgLen + 4] = '\0';
    self->iBlocked = 1;
    msgsnd(msqId, pMess, msgLen, msqFlag);
    self->iBlocked = 1;
    self->iErrno = errno;
    CleanupStack::PopAndDestroy();
    return NULL;
	}
// -----------------------------------------------------------------------------
// CTestMsgqueue::MessageQueueSend
// -----------------------------------------------------------------------------
//
TInt CTestMsgqueue::MessageQueueSend(  )
    {
    _LIT(KFunc, "MessageQueueSend");
    INFO_PRINTF1( KFunc);
    
    TInt msqId = -1;
    TInt msgType;
    TInt msqFlag;
    TInt terror;
    TInt err;
    message_buf buf = {0};
    TInt msgLen;
    TInt retVal = KErrGeneral;
    
    // retrieve passed parameters
    err = MsgId( msqId);
    RETURN_ON_ERROR(err);
  
    err = MsgLen( msgLen);
    RETURN_ON_ERROR(err);
     
    err = MsgType( msgType);
    RETURN_ON_ERROR(err);
    
    err = MsgFlag( msqFlag);
    RETURN_ON_ERROR(err);
    
    err = ErrCode( terror);
    RETURN_ON_ERROR(err);
    
    MessageCreate(msgLen, msgType, &buf);
    
    err = msgsnd(msqId, &buf, msgLen, msqFlag);
    
    if ( err == -1 )
        {
        if(terror == WITHOUT_ERROR)
            {
            retVal = errno;
            }
        else
            {
            TInt errCode = 0;
            _LIT( KerrCode, "Param%d" );
			 TBuf<8> pNameBuf;
		     pNameBuf.Format(KerrCode,++iParamCnt);
			 TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, errCode);
			 
			 if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve error code") ;
			 	INFO_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            if ( errCode == errno)
                {
                retVal = KErrNone;
                }
            }    
        }
    else
        {
        if ( terror != WITH_ERROR )
            {
            retVal = KErrNone;
            }       
        }
    
    return retVal;
    }

// -----------------------------------------------------------------------------
// CTestMsgqueue::MessageQueueRecv
// -----------------------------------------------------------------------------
//
TInt CTestMsgqueue::MessageQueueRecv(  )
    {
    _LIT(KFunc, "MessageQueueRecv");
    INFO_PRINTF1( KFunc);
    
    TInt msqId;
    TInt msgType;
    TInt msqFlag;
    TInt terror;
    TInt err;
    message_buf buf = {0};
    TInt msgLen;
    TInt retVal = KErrGeneral;
    
    // retrieve passed parameters
    err = MsgId( msqId);
    RETURN_ON_ERROR(err);
  
    err = MsgLen( msgLen);
    RETURN_ON_ERROR(err);
     
    err = MsgType( msgType);
    RETURN_ON_ERROR(err);
    
    err = MsgFlag( msqFlag);
    RETURN_ON_ERROR(err);
    
    err = ErrCode( terror);
    RETURN_ON_ERROR(err);
    
    char array[256];
    for(int idx=0; idx<255; idx++)
	    {
    	array[idx]='a';    	
    	}
    array[255]='\0';	
    
    err = msgrcv(msqId, &buf, msgLen, msgType, msqFlag);

    if ( err == -1 )
        {
        if(terror == WITHOUT_ERROR)
            {
            retVal = errno;
            }
        else
            {
            TInt errCode = 0;
            _LIT( KerrCode, "Param%d" );
			 TBuf<8> pNameBuf;
		     pNameBuf.Format(KerrCode,++iParamCnt);
			 TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, errCode);
			 
			 if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve error code") ;
			 	INFO_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            if ( errCode == errno)
                {
                retVal = KErrNone;
                }
            }    
        }
    else
        {                      	
        if ( terror != WITH_ERROR && ( msgType <= 0 || buf.mtype == msgType))           
            {            
           	if((msgLen>0 && msgLen<256) && !(strncmp(&buf.mtext[0], &array[0], msgLen)))
            	retVal = KErrNone;
            }    
        }

    return retVal;
    }


TInt CTestMsgqueue::IntgTest1()
	{

	TInt err = MessageQueueCreate();
	if(!err)
		{
		err = MessageQueueClose();
		}
	return err;
	}

TInt CTestMsgqueue::IntgTest2()
	{
	TInt err = MessageQueueCreate();
	if(!err)
		{
		err = MessageQueueCreate();
			if(!err) 
				{
				err = MessageQueueClose();
				}
		}
	return err;
	}


TInt CTestMsgqueue::IntgTest3()
	{
	TInt err = MessageQueueCreate();
	if(!err)
		{
			err = MessageQueueControl();
			if(!err) 
				{
				err = MessageQueueClose();
				}
		}
	return err;
	}

TInt CTestMsgqueue::IntgTest4()
	{
	TInt err = MessageQueueCreate();
	if(!err)
		{
		err = MessageQueueSend();
		if(!err)
			{
			err = MessageQueueRecv();
			if(!err)
				{
				err = MessageQueueClose();
				}
			}
		}
	return err;
	}

TInt CTestMsgqueue::IntgTest5()
	{
	TInt err = MessageQueueCreate();
	if(!err)
		{
			err = MessageQueueSend();
			if(!err)
				{
				err = MessageQueueClose();
				}	
		}
	return err;
	}

TInt CTestMsgqueue::IntgTest6()
	{
	TInt err = MessageQueueCreate();
	if(!err)
		{
		err = MessageQueueSend();
		if(!err)
			{
			err = MessageQueueSend();
			if(!err)
				{
				err = MessageQueueClose();
				}
			}
		}
	return err;
	}

TInt CTestMsgqueue::IntgTest7()
	{
	TInt err = MessageQueueCreate();
	if(!err)
		{
			err = MessageQueueRecv();
			if(!err)
				{
				err = MessageQueueClose();
				}	
		}
	return err;
	}

TInt CTestMsgqueue::IntgTest8()
	{
	TInt err = MessageQueueCreate();
	if(!err)
		{
		err = MessageQueueSend();
		if(!err)
			{
			err = MessageQueueRecv();
			if(!err) 
				{
				err = MessageQueueRecv();	
				if(!err)
					{
					err = MessageQueueClose();
					}
				}
			}
		}
	return err;
	}

TInt CTestMsgqueue::IntgTest9()
	{
	TInt err = MessageQueueCreate();
	if(!err)
		{
		err = MessageQueueSend();
		if(!err)
			{
			err = MessageQueueRecv();
			if(!err) 
				{
				err = MessageQueueControl();	
				if(!err)
					{
					err = MessageQueueClose();
					}
				}
			}
		}
	return err;
	}


TInt CTestMsgqueue::IntgTest10()
	{
	TInt err = MessageQueueCreate();
	if(err)
		{
		return err;
		}
	err = MessageQueueSend();
	if(err)
		{
		return err;
		}
	err = MessageQueueSend();
	if(err)
		{
		return err;
		}
	err = MessageQueueSend();
	if(err)
		{
		return err;
		}
	err = MessageQueueRecv();
	if(err) return err;
	if(err)
		{
		return err;
		}
	if(err)	return err;
	if(err)
		{
		return err;
		}
	if(err)	return err;
	if(err)
		{
		return err;
		}
	MessageQueueClose();
	return err;
	}

TInt CTestMsgqueue::IntgTest11()
	{
	TInt err = MessageQueueCreate();
	if(err)
		{
		return err;
		}
	err = MessageQueueSend();
	if(err)
		{
		return err;
		}
	err = MessageQueueSend();
	if(err)
		{
		return err;
		}
	err = MessageQueueSend();
	if(err)
		{
		return err;
		}
	err = MessageQueueRecv();
	if(err)
		{
		return err;
		}
	err = MessageQueueRecv();
	if(err)
		{
		return err;
		}
	err = MessageQueueRecv();
	if(err)
		{
		return err;
		}
	err = MessageQueueRecv();
	if(err)
		{
		return err;
		}
	err = MessageQueueControl();
	if(err)
		{
		return err;
		}
	err = MessageQueueClose();
	return err;
	}

TInt CTestMsgqueue::IntgTest12()
	{
	TInt err = MessageQueueCreate();
	if(err)
		{
		return err;
		}
	err = MessageQueueSend();
	if(err)
		{
		return err;
		}
	err = MessageQueueSend();
	if(err)
		{
		return err;
		}
	err = MessageQueueRecv();
	if(err)
		{
		return err;
		}
	err = MessageQueueRecv();
	if(err)
		{
		return err;
		}
	err = MessageQueueControl();
	if(err)
		{
		return err;
		}
	err = MessageQueueClose();
	return err;
	}

TInt CTestMsgqueue::IntgTest13()
	{
	TInt err = MessageQueueCreate();
	if(!err)
		{
			err = CreateThread();
			if(!err)
				{
				err = MessageQueueClose();
				}
		}
	return err;
	}

TInt CTestMsgqueue::IntgTest14()
	{
	TInt err = MessageQueueCreate();
	if(!err)
		{
		err = CreateThread();
		}
	return err;
	}

TInt CTestMsgqueue::IntgTest15()
	{
	TInt err = MessageQueueCreate();
	if(!err)
		{
		err = MessageQueueSend();
		if(!err)
			{
			err = CreateThread();
			if(!err)
				{
				err = MessageQueueClose();
				}
			}
		}
	return err;
	}

TInt CTestMsgqueue::IntgTest16()
	{
	TInt err = MessageQueueCreate();
	if(!err)
		{
			for(int i=0;i<512;i++)
				{
				INFO_PRINTF2(_L("i is %d\n"),i);
				err = MessageQueueSend();
				if(err!=0)
					{
					break;
					}
				}
			if(!err)
				{
				err = CreateThread();
				}
		}
	return err;
	}

TInt CTestMsgqueue::IntgTest17()
	{
	TInt err = MessageQueueCreate();
	if(!err)
		{
		err = MessageQueueSend();
		if(!err)
			{
			err = MessageQueueRecv();
			if(!err) 
				{
				err = MessageQueueSend();	
				if(!err)
					{
					err = MessageQueueClose();
					}
				}
			}
		}
	return err;
	}

TInt CTestMsgqueue::IntgTest6_1()
	{
	TInt err = MessageQueueCreate();
	if(!err)
		{
		err = MessageQueueSend();
		if(!err)
			{
			for(int i=0;i<16;i++)
				{
				err = MessageQueueSend();
				if(err!=0)
					{
					break;
					}
				}
			if(!err)
				{
				err = MessageQueueClose();
				}
			}
		}
	return err;
	}
	

/*
TInt CTestMsgqueue::IntgTest16()
	{
	int msq_id;
	if ((msq_id = msgget(MESSAGE_Q_KEY, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    int err;
    message_buf buf = {0};
    MessageCreate(4096, 1, &buf);
    for(int i=0;i<16;i++)
    	{
    	err = msgsnd(msq_id, &buf, 4096, 2048);
    	}
    if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	}

nt CTestMsgqueue::IntgTest6_1()
	{
	TInt err = MessageQueueCreate();
	if(!err)
		{
		err = MessageQueueSend();
		if(!err)
			{
			err = MessageQueueSend();
			err = MessageQueueSend();
			err = MessageQueueSend();
			err = MessageQueueSend();
			if(!err)
				{
				err = MessageQueueClose();
				}
			}
		}
	return err;
	}
*/
//////////////////////////////////////////////////////////////////////////

					// MSGGET 
					
// with the specified key

TInt CTestMsgqueue::msggettest1()
	{
	int msq_id;
	
	if ((msq_id = msgget(MESSAGE_Q_KEY, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

	return KErrNone;
	}

// with the IPC_PRIVATE flag

TInt CTestMsgqueue::msggettest2()
	{
	int msq_id;
	
	if ((msq_id = msgget(IPC_PRIVATE , IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	return KErrNone;
	}

// Max no.of msg ids per process 
// Check for the errno ENOSPC

TInt CTestMsgqueue::msggettest3()
	{
	int msq_id[1512];
	
	int i=0,j=0;
	int ret = KErrNone;
	for(i=0;;i++)
		{
		if ((msq_id[i] = msgget(IPC_PRIVATE , IPC_CREAT | 0666)) == -1) 
			{
			if(errno == ENOSPC)
				{
				_LIT(KFunc, "Message Q create set the proper errno %d\n");
      			INFO_PRINTF2(KFunc,i);
       			break;
				}
			else
				{
				_LIT(KFunc, "Message Q create didnt set the proper errno \n");
      			INFO_PRINTF1(KFunc);
      			ret=KErrGeneral;
       			break;
				}
    		}
		}
	
	for(j=0;j<i;j++)
		{
		if (msgctl(msq_id[j], IPC_RMID, NULL) == -1) 
    		{
			_LIT(KFunc, "Message Q delete failed with errno %d \n");
      		INFO_PRINTF2(KFunc, errno);
       		ret= KErrGeneral;
    		}
		}
    	     	
	return ret;
	}

// check for the errno ENOENT

TInt CTestMsgqueue::msggettest4()
	{
	
	int msq_id;
	
	msq_id = msgget(MESSAGE_Q_KEY , IPC_CREAT & 0);
	
	if (msq_id == -1 && errno == ENOENT) 
		{
		_LIT(KFunc, "Message Q set the proper the errno\n");
      	INFO_PRINTF1(KFunc);
       	return KErrNone;
    	}
    return KErrGeneral;
	
	}

// check for the errno EEXIST

TInt CTestMsgqueue::msggettest5()
	{
	int msq_id1,msq_id2;
	
	if ((msq_id1 = msgget(MESSAGE_Q_KEY, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    else
    	{
    	msq_id2 = msgget(MESSAGE_Q_KEY, IPC_CREAT | IPC_EXCL | 0666);
    	if(msq_id2 == -1 && errno == EEXIST)
    		{  		
			_LIT(KFunc, "Message Q set the proper the errno\n");
      		INFO_PRINTF1(KFunc);
    		}
    	else
    		{  		
			_LIT(KFunc, "Message Q didnt set the proper the errno\n");
      		INFO_PRINTF1(KFunc);
       		return KErrGeneral;
    		}
    	}
    if (msgctl(msq_id1, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

	return KErrNone;
	}

// for the defect 

TInt CTestMsgqueue::msggettest6()
	{
	int msq_id1;
	
	if ((msq_id1 = msgget(0, 0)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    
    if (msgctl(msq_id1, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

	return KErrNone;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CTestMsgqueue::MsggetCreateKey()
// Description   : 	This test creates a message queue instance with the 
//					specified key, and deletes it.
// Param1		 :  Key to use for msgget (passed through the ini file)
//---------------------------------------------------------------------------------
TInt CTestMsgqueue::MsggetCreateKey()
	{
	int msgid;
	int key;
	TBuf<8> pNameBuf;
	
	iParamCnt = 0;
	_LIT( KaMsgNum, "Param%d" );
	
	pNameBuf.Format(KaMsgNum,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, key);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve msg key") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
	msgid = msgget(key, IPC_CREAT);
	if( msgid < 0 )
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	
    if (msgctl(msgid, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	    
    return KErrNone;
	}

						// MSGCTL

// check for the errno EINVAL with invalid <id>
				
TInt CTestMsgqueue::msgctltest1()
	{
	
	if ((msgctl(-1, IPC_RMID, NULL) == -1) && errno == EINVAL ) 
    	{
		_LIT(KFunc, "msgctl set the proper errno\n");
      	INFO_PRINTF1(KFunc);
       	return KErrNone;
    	}

	return KErrGeneral;
	}
	
// check for the errno EINVAL with invalid <cmd>
				
TInt CTestMsgqueue::msgctltest2()
	{
	int msq_id;
	
	if ((msq_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    if ((msgctl(msq_id, -5, NULL) == -1) && errno == EINVAL) 
    	{
		_LIT(KFunc, "msgctl set the proper errno\n");
      	INFO_PRINTF1(KFunc);
      	if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    		{
			_LIT(KFunc, "Message Q delete failed with errno %d\n");
      		INFO_PRINTF2(KFunc, errno);
       		return KErrGeneral;
    		}
       	return KErrNone;
    	}
	if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    	
	return KErrGeneral;
	}

// get the details of the message queue with flag IPC_STAT 

TInt CTestMsgqueue::msgctltest3()
	{
	int msq_id,ret=KErrNone;
	struct msqid_ds buf;
	int msgflg = IPC_R|IPC_CREAT | IPC_EXCL | 0666;
	if ((msq_id = msgget(MESSAGE_Q_KEY, msgflg)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    if (msgctl(msq_id, IPC_STAT, &buf) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
	
	if ( buf.msg_cbytes != 0 )
        {
        ret= KErrGeneral;
        }
    if ( buf.msg_qnum != 0 )
        {
        ret= KErrGeneral;
        }
    if ( buf.msg_perm.key != MESSAGE_Q_KEY )
        {
        ret= KErrGeneral;
        }
   if ( buf.msg_qbytes != 2097152 )
        {
        ret= KErrGeneral;
        } 
    if( buf.msg_lspid != 0 )
    	{
        ret= KErrGeneral;
        } 	
    if( buf.msg_lrpid != 0 )
    	{
        ret= KErrGeneral;
        }
    if( buf.msg_stime != 0 )
    	{
        ret= KErrGeneral;
        }
    if( buf.msg_rtime != 0 )
    	{
        ret= KErrGeneral;
        }

	if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
	return ret;
	}

// set the max # of bytes on the queue with IPC_SET flag

TInt CTestMsgqueue::msgctltest4()
	{
	int msq_id;
	struct msqid_ds buf;
	struct msqid_ds buf1;
	if ((msq_id = msgget(MESSAGE_Q_KEY, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    if (msgctl(msq_id, IPC_STAT, &buf) == -1) 
    	{
		_LIT(KFunc, "Message Q set failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    buf1=buf;
    buf1.msg_qbytes = 65546;
    if (msgctl(msq_id, IPC_SET, &buf1) == -1) 
    	{
		_LIT(KFunc, "Message Q set failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    	
    if (msgctl(msq_id, IPC_STAT, &buf) == -1) 
    	{
		_LIT(KFunc, "Message Q get failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	
	if ( buf.msg_cbytes != 0 )
        {
        return KErrGeneral;
        }
    if ( buf.msg_qnum != 0 )
        {
        return KErrGeneral;
        }
    if ( buf.msg_perm.key != MESSAGE_Q_KEY )
        {
        return KErrGeneral;
        }
    if ( buf.msg_qbytes != 65546 )
        {
        return KErrGeneral;
        } 
	if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	return KErrNone;
	}

// check for the errno EACCESS without read permissions over IPC_STAT

TInt CTestMsgqueue::msgctltest5()
	{
	int msq_id,ret=KErrNone,x;
	struct msqid_ds buf;
	if ((msq_id = msgget(IPC_PRIVATE, 0  )) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
   	x=msgctl(msq_id, IPC_STAT, &buf);     	
    if ((x != -1) && errno != EACCES) 
    	{
		_LIT(KFunc, "Message Q didnt set the proper errno\n");
      	INFO_PRINTF1(KFunc);
       	ret= KErrGeneral;
    	}
	
	if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
	return ret;
	}
	
	
					// MSGSND
					
// check for the errno EIDRM

TInt CTestMsgqueue::msgsndtest1()
	{
	int msq_id,len;
	struct {
       long mtype;
       char mtext[128];
   } msg_buf;

	msg_buf.mtype = 1; /* message identifier */
    strcpy(msg_buf.mtext, "some_data_to_send"); /* data */
    len = strlen(msg_buf.mtext)+1;

	if ((msq_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    
	if ((msgsnd(msq_id, (struct msgbuf *)&msg_buf, len, IPC_NOWAIT) != -1) && errno != EIDRM) 
		{
		_LIT(KFunc, "msgsnd failed in setting errno\n");
      	INFO_PRINTF1(KFunc);
       	return KErrGeneral;
    	}
	return KErrNone;
	}

// check for the errno EINVAL with invalid <id>
				
TInt CTestMsgqueue::msgsndtest2()
	{
	int msq_id=-1,len,ret=KErrGeneral;
	struct {
       long mtype;
       char mtext[128];
   	} msg_buf;

	msg_buf.mtype = 1; /* message identifier */
    strcpy(msg_buf.mtext, "some_data_to_send"); /* data */
    len = strlen(msg_buf.mtext)+1;
    
	if ((msgsnd(msq_id, (struct msgbuf *)&msg_buf, len, IPC_NOWAIT)  == -1) && errno == EINVAL ) 
    	{
		_LIT(KFunc, "msgsnd set the proper errno\n");
      	INFO_PRINTF1(KFunc);
       	ret=KErrNone;
    	}

	return ret;
	}

// check for the errno EINVAL with mtype<1
				
TInt CTestMsgqueue::msgsndtest3()
	{
	int msq_id=0,len,ret=KErrNone;
	struct {
       long mtype;
       char mtext[128];
   	} msg_buf;

	msg_buf.mtype = 0; /* message identifier */
    strcpy(msg_buf.mtext, "some_data_to_send"); /* data */
    len = strlen(msg_buf.mtext)+1;
    
    if ((msq_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
    	
	if ((msgsnd(msq_id, (struct msgbuf *)&msg_buf, len, IPC_NOWAIT)  == -1) && errno == EINVAL ) 
    	{
		_LIT(KFunc, "msgsnd set the proper errno\n");
      	INFO_PRINTF1(KFunc);
    	}
	if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
	return ret;
	}

// check for the errno EINVAL with msgsz<0
TInt CTestMsgqueue::msgsndtest4()
	{
	int msq_id=0,ret=KErrNone;
	struct {
       long mtype;
       char mtext[128];
   	} msg_buf;

	msg_buf.mtype = 1; /* message identifier */
    strcpy(msg_buf.mtext, "some_data_to_send"); /* data */
    int len = strlen(msg_buf.mtext)+1;
    
    if ((msq_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
    	
	if ((msgsnd(msq_id, (struct msgbuf *)&msg_buf, (size_t)-1, IPC_NOWAIT)  == -1) && errno == EINVAL ) 
    	{
		_LIT(KFunc, "msgsnd set the proper errno\n");
      	INFO_PRINTF1(KFunc);
    	}
	if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
	return ret;
	}

// successful msgsnd

TInt CTestMsgqueue::msgsndtest5()
	{
	int msq_id=0,len,ret=KErrNone;
	struct msqid_ds buf;
	struct {
       long mtype;
       char mtext[128];
   	} msg_buf;

	msg_buf.mtype = 1; /* message identifier */
    strcpy(msg_buf.mtext, "some_data_to_send"); /* data */
    len = strlen(msg_buf.mtext)+1;
    
    if ((msq_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
    	
	if (msgsnd(msq_id, (struct msgbuf *)&msg_buf, len, IPC_NOWAIT)  != 0) 
    	{
		_LIT(KFunc, "msgsnd failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
    if (msgctl(msq_id, IPC_STAT, &buf) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	/*
	if ( buf.msg_cbytes != len )
        {
        ret= KErrGeneral;
        }*/
    if ( buf.msg_qnum != 1 )
        {
        ret= KErrGeneral;
        }
    if ( buf.msg_perm.key != IPC_PRIVATE )
        {
        ret= KErrGeneral;
        }
    if ( buf.msg_qbytes != 2097152 )
        {
        ret= KErrGeneral;
        }

 	if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
	return ret;
	}

// successful 2 msgsnd

TInt CTestMsgqueue::msgsndtest6()
	{
	int msq_id=0,len,ret=KErrNone;
	struct msqid_ds buf;
	struct {
       long mtype;
       char mtext[128];
   	} msg_buf;

	msg_buf.mtype = 1; /* message identifier */
    strcpy(msg_buf.mtext, "some_data_to_send"); /* data */
    len = strlen(msg_buf.mtext)+1;
    
    if ((msq_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
    	
	if (msgsnd(msq_id, (struct msgbuf *)&msg_buf, len, IPC_NOWAIT)  != 0) 
    	{
		_LIT(KFunc, "msgsnd failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
    if (msgsnd(msq_id, (struct msgbuf *)&msg_buf, len, IPC_NOWAIT)  != 0) 
    	{
		_LIT(KFunc, "msgsnd failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
    if (msgctl(msq_id, IPC_STAT, &buf) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	/*
	if ( buf.msg_cbytes != (2*len) )
        {
        ret= KErrGeneral;
        }
        */
    if ( buf.msg_qnum != 2 )
        {
        ret= KErrGeneral;
        }
    if ( buf.msg_perm.key != IPC_PRIVATE )
        {
        ret= KErrGeneral;
        }
    if ( buf.msg_qbytes != 2097152 )
        {
        ret= KErrGeneral;
        }

    if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
	return ret;
	}
	
					// MSGRCV
					

// check for the errno EIDRM

TInt CTestMsgqueue::msgrcvtest1()
	{
	int msq_id,len;
	struct {
       long mtype;
       char mtext[128];
   } msg_buf;

	msg_buf.mtype = 1; /* message identifier */
    strcpy(msg_buf.mtext, "some_data_to_send"); /* data */
    len = strlen(msg_buf.mtext)+1;

	if ((msq_id = msgget(MESSAGE_Q_KEY, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    
	if ((msgrcv(msq_id, (struct msgbuf *)&msg_buf, len,1, 0) != -1) && errno != EIDRM) 
		{
		_LIT(KFunc, "msgsnd failed in setting errno\n");
      	INFO_PRINTF1(KFunc);
       	return KErrGeneral;
    	}
	return KErrNone;
	}

// check for the errno E2BIG        

TInt CTestMsgqueue::msgrcvtest2()
	{
	int msq_id=0,len,ret=KErrNone;
	struct {
       long mtype;
       char mtext[128];
   	} msg_buf;

	msg_buf.mtype = 1; /* message identifier */
    strcpy(msg_buf.mtext, "some_data_to_send"); /* data */
    len = strlen(msg_buf.mtext)+1;
    
    if ((msq_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
    	
	if (msgsnd(msq_id, (struct msgbuf *)&msg_buf, len, IPC_NOWAIT)  != 0) 
    	{
		_LIT(KFunc, "msgsnd failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
    if ((msgrcv(msq_id, (struct msgbuf *)&msg_buf, 15,1, 0) != -1) || errno != E2BIG) 
		{
		_LIT(KFunc, "msgrcv failed in setting errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
       	return KErrGeneral;
    	}
    
	if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
	return ret;
	}

// check for the errno EINVAL

TInt CTestMsgqueue::msgrcvtest3()
	{
	int msq_id=-1,len,ret=KErrNone;
	struct {
       long mtype;
       char mtext[128];
   	} msg_buf;

	msg_buf.mtype = 1; /* message identifier */
    strcpy(msg_buf.mtext, "some_data_to_send"); /* data */
    len = strlen(msg_buf.mtext)+1;
    
    if ((msgrcv(msq_id, (struct msgbuf *)&msg_buf, len,1, 0) != -1) && errno != EINVAL) 
		{
		_LIT(KFunc, "msgsnd failed in setting errno\n");
      	INFO_PRINTF1(KFunc);
       	return KErrGeneral;
    	}
    
	return ret;
	}
	

// check for the errno ENOMSG

TInt CTestMsgqueue::msgrcvtest4()
	{
	int msq_id,len;
	struct {
       long mtype;
       char mtext[128];
   } msg_buf,msg_buf1;

	msg_buf.mtype = 1; /* message identifier */
    strcpy(msg_buf.mtext, "some_data_to_send"); /* data */
    len = strlen(msg_buf.mtext)+1;

	if ((msq_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    
    if (msgsnd(msq_id, (struct msgbuf *)&msg_buf, len, IPC_NOWAIT)  != 0) 
    	{
		_LIT(KFunc, "msgsnd failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}
    	    
	if ((msgrcv(msq_id, (struct msgbuf *)&msg_buf1, len,3, IPC_NOWAIT) != -1) && errno != ENOMSG) 
		{
		_LIT(KFunc, "msgsnd failed in setting errno\n");
      	INFO_PRINTF1(KFunc);
       	return KErrGeneral;
    	}
    	
    if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    
	return KErrNone;
	}

				// MSGSND & MSGRCV 
				
TInt CTestMsgqueue::sndrcvtest1()
	{
	int msq_id=0,len,ret=KErrNone;
	struct msqid_ds buf= {0};
	struct {
       long mtype;
       char mtext[128];
   	} msg_buf,msg_buf1;

	msg_buf.mtype = 1; // message identifier 
    strcpy(msg_buf.mtext, "some_data_to_send"); // data 
    len = strlen(msg_buf.mtext)+1;
    
    if ((msq_id = msgget(MESSAGE_Q_KEY1, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
    	
	if (msgsnd(msq_id, (struct msgbuf *)&msg_buf, len, IPC_NOWAIT)  != 0) 
    	{
		_LIT(KFunc, "msgsnd failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
    if ((msgrcv(msq_id, (struct msgbuf *)&msg_buf1, len,1, 0) == -1)) 
		{
		_LIT(KFunc, "msgsnd failed in setting errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
       	return KErrGeneral;
    	}
    
    if(msg_buf1.mtype != 1)
    	{
    	return KErrGeneral;
    	}
    if( (strcmp(msg_buf1.mtext,msg_buf.mtext)) )
    	{
    	return KErrGeneral;
    	}
    if ( buf.msg_qnum != 0 )
        {
        ret= KErrGeneral;
        }
    if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
	return ret;
	}
/*

TInt CTestMsgqueue::sndrcvtest1()
	{
	int msq_id=0,len,ret=KErrNone;
	struct msqid_ds buf= {0};
	struct {
       long mtype;
       char mtext[128];
   	} msg_buf,msg_buf1;

	msg_buf.mtype = 1; // message identifier 
    strcpy(msg_buf.mtext, "some_data_to_send"); // data 
    len = strlen(msg_buf.mtext)+1;
    
    if ((msq_id = msgget(MESSAGE_Q_KEY1, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
    	
	if (msgsnd(msq_id, (struct msgbuf *)&msg_buf, len, IPC_NOWAIT)  != 0) 
    	{
		_LIT(KFunc, "msgsnd failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
    if ((msgrcv(msq_id, (struct msgbuf *)&msg_buf1, len-1,1, MSG_NOERROR) == -1)) 
		{
		_LIT(KFunc, "msgsnd failed in setting errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
       	return KErrGeneral;
    	}
    
    if(msg_buf1.mtype != 1)
    	{
    	return KErrGeneral;
    	}
    if( (strcmp(msg_buf1.mtext,msg_buf.mtext)) )
    	{
    	return KErrGeneral;
    	}
    if ( buf.msg_qnum != 0 )
        {
        ret= KErrGeneral;
        }
    if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
	return ret;
	}*/
	
// If msgtyp is 0, the first message on the queue shall be received.
TInt CTestMsgqueue::sndrcvtest2()
	{
	int msq_id=0,len1,len2,ret=KErrNone;
	struct msqid_ds buf= {0};
	struct {
       long mtype;
       char mtext[128];
   	} msg_buf,msg_buf1,msg_buf2;

	msg_buf.mtype = 1; /* message identifier */
    strcpy(msg_buf.mtext, "some_data_to_send1"); /* data */
    len1 = strlen(msg_buf.mtext)+1;
    
    msg_buf1.mtype = 2; /* message identifier */
    strcpy(msg_buf1.mtext, "some_data_to_send2"); /* data */
    len2 = strlen(msg_buf1.mtext)+1;
    
    if ((msq_id = msgget(MESSAGE_Q_KEY1, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
    	
	if (msgsnd(msq_id, (struct msgbuf *)&msg_buf, len1, IPC_NOWAIT)  != 0) 
    	{
		_LIT(KFunc, "msgsnd1 failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
    if (msgsnd(msq_id, (struct msgbuf *)&msg_buf1, len2, IPC_NOWAIT)  != 0) 
    	{
		_LIT(KFunc, "msgsnd2 failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
    if ((msgrcv(msq_id, (struct msgbuf *)&msg_buf2, 100,0, 0) == -1)) 
		{
		_LIT(KFunc, "msgsnd failed in setting errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
       	return KErrGeneral;
    	}
    
    if (msgctl(msq_id, IPC_STAT, &buf) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    	
    if(msg_buf2.mtype != 1)
    	{
    	return KErrGeneral;
    	}
    if( (strcmp(msg_buf2.mtext,msg_buf.mtext)) )
    	{
    	return KErrGeneral;
    	}
    if ( buf.msg_qnum != 1 )
        {
        ret= KErrGeneral;
        }
    if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
	return ret;
	}
	
// If msgtyp is greater than 0, the first message of type msgtyp shall be received.

TInt CTestMsgqueue::sndrcvtest3()
	{
	int msq_id=0,len1,len2,len3,ret=KErrNone;
	
	struct {
       long mtype;
       char mtext[128];
   	} msg_buf,msg_buf1,msg_buf2,msg_buf3;

	msg_buf.mtype = 1; /* message identifier */
    strcpy(msg_buf.mtext, "some_data_to_send1"); /* data */
    len1 = strlen(msg_buf.mtext)+1;
    
    msg_buf1.mtype = 2; /* message identifier */
    strcpy(msg_buf1.mtext, "some_data_to_send2"); /* data */
    len2 = strlen(msg_buf1.mtext)+1;
    
    msg_buf3.mtype = 2; /* message identifier */
    strcpy(msg_buf3.mtext, "some_data_to_send3"); /* data */
    len3 = strlen(msg_buf3.mtext)+1;
    
    if ((msq_id = msgget(MESSAGE_Q_KEY1, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
    	
	if (msgsnd(msq_id, (struct msgbuf *)&msg_buf, len1, IPC_NOWAIT)  != 0) 
    	{
		_LIT(KFunc, "msgsnd1 failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
    if (msgsnd(msq_id, (struct msgbuf *)&msg_buf1, len2, IPC_NOWAIT)  != 0) 
    	{
		_LIT(KFunc, "msgsnd2 failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
    if (msgsnd(msq_id, (struct msgbuf *)&msg_buf3, len3, IPC_NOWAIT)  != 0) 
    	{
		_LIT(KFunc, "msgsnd3 failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
    if ((msgrcv(msq_id, (struct msgbuf *)&msg_buf2, 100,2, 0) == -1)) 
		{
		_LIT(KFunc, "msgsnd failed in setting errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
       	return KErrGeneral;
    	}
    
    if(msg_buf2.mtype != 2)
    	{
    	return KErrGeneral;
    	}
    if( (strcmp(msg_buf2.mtext,msg_buf1.mtext)) )
    	{
    	return KErrGeneral;
    	}

    if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
	return ret;
	}

// If msgtyp is less than 0, the first message of the lowest type that is 
// less than or equal to the absolute value of msgtyp shall be received.

TInt CTestMsgqueue::sndrcvtest4()
	{
	int msq_id=0,len1,len2,len3,ret=KErrNone;

	struct {
       long mtype;
       char mtext[128];
   	} msg_buf,msg_buf1,msg_buf2,msg_buf3;

	msg_buf.mtype = 3; /* message identifier */
    strcpy(msg_buf.mtext, "some_data_to_send1"); /* data */
    len1 = strlen(msg_buf.mtext)+1;
    
    msg_buf1.mtype = 2; /* message identifier */
    strcpy(msg_buf1.mtext, "some_data_to_send2"); /* data */
    len2 = strlen(msg_buf1.mtext)+1;
    
    msg_buf3.mtype = 1; /* message identifier */
    strcpy(msg_buf3.mtext, "some_data_to_send3"); /* data */
    len3 = strlen(msg_buf3.mtext)+1;
    
    if ((msq_id = msgget(MESSAGE_Q_KEY1, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "Message Q create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
    	
	if (msgsnd(msq_id, (struct msgbuf *)&msg_buf, len1, IPC_NOWAIT)  != 0) 
    	{
		_LIT(KFunc, "msgsnd1 failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
    if (msgsnd(msq_id, (struct msgbuf *)&msg_buf1, len2, IPC_NOWAIT)  != 0) 
    	{
		_LIT(KFunc, "msgsnd2 failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
    if (msgsnd(msq_id, (struct msgbuf *)&msg_buf3, len3, IPC_NOWAIT)  != 0) 
    	{
		_LIT(KFunc, "msgsnd3 failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
    if ((msgrcv(msq_id, (struct msgbuf *)&msg_buf2, 100,-2, 0) == -1)) 
		{
		_LIT(KFunc, "msgsnd failed in setting errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
       	return KErrGeneral;
    	}
    
    if(msg_buf2.mtype != 1)
    	{
    	return KErrGeneral;
    	}
    if( (strcmp(msg_buf2.mtext,msg_buf3.mtext)) )
    	{
    	return KErrGeneral;
    	}/*
    if ( buf.msg_qnum != 2 )
        {
        ret= KErrGeneral;
        }*/
    if (msgctl(msq_id, IPC_RMID, NULL) == -1) 
    	{
		_LIT(KFunc, "Message Q delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret= KErrGeneral;
    	}
	return ret;
	}

						// THREADS
						
TInt MsgQueueGet()
    {
	int msgq_id;
    if ((msgq_id = msgget(MESSAGE_Q_KEY_TH, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		return 0;
    	}
    return msgq_id;
    }

void* ThreadEntryRead(void* arg)
	{    
    TInt msgType;
    TInt msqFlag = 0;
    message_buf buf = {0};
    TInt msgLen = 128;
    
    msgType = 0; //any message
    msqFlag =  ~IPC_NOWAIT; //Blocking
    int msgq_id = (int)arg;
    int err = msgrcv(msgq_id, &buf, msgLen, msgType, msqFlag);
    
    if(buf.mtype != 1)
    	{
    	exit(1);
    	}
    if( (strcmp(buf.mtext,"some_data_to_send")) )
    	{
    	exit(1);
    	}
    return NULL;
	}

TInt MsgQueueThread( int id )
	{

    TInt err = KErrNone;
    int threadRetVal = 0;
	void *threadRetValPtr = (void*)&threadRetVal;
    pthread_t testThread;
    err = pthread_create(&testThread,(pthread_attr_t *)NULL,ThreadEntryRead,(TAny*)id);
    if (!err)
    	{
    	sleep(5);
        err = MsgQueueSend(id);
        }
    pthread_join(testThread, &threadRetValPtr);        
    return err;
	}

TInt MsgQueueSend(int msgq_id)
	{
	
    TInt len;
    
    struct {
       long mtype;
       char mtext[128];
   	} msg_buf;

	msg_buf.mtype = 1; /* message identifier */
    strcpy(msg_buf.mtext, "some_data_to_send"); /* data */
    len = strlen(msg_buf.mtext)+1;
    
    if (msgsnd(msgq_id, (struct msgbuf *)&msg_buf, len, 0)  != 0) 
    	{
      	return -1;
    	}
    
    return 0;
	}
	
TInt MsgQueueDelete(int msgq_id)
	{
	User::After(100000);
	if (msgctl(msgq_id, IPC_RMID, NULL) == -1) 
    	{
       	return -1;
       	}
    return 0;
    	
    }
	
TInt CTestMsgqueue::thsndrcvtest1()
	{
	
	int err=0,id;
	
	id = MsgQueueGet();
	
	if(id)
		{
		
		err = MsgQueueThread(id);
		if(!err)
			{
			err = MsgQueueDelete(id);
			if(!err)
				{
				return KErrNone;
				}
			}
		}
		
	return KErrGeneral;
	}
