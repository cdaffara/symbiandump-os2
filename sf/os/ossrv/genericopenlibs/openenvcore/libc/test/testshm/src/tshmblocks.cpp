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



#include "tshm.h"

#define SHM_KEY 1001
#define WITH_ERROR 1
#define WITHOUT_ERROR 0
#define USE_DEFAULT_ID 1
#define USE_DEFAULT_MAP_ADDR 1
#define SHMSZ 100
#define IPC_KEY 1000
#define IPC_CREAT_EXCL 3000
#define IPC_NO_CREAT_EXCL 2000
#define RETURN_ON_ERROR(err) if ( err ) \
		{ \
        ERR_PRINTF2 (_L("Error return:%d"), err); \
        return err; \
        }


TInt CTestShm::ShmKey(TInt& aKey )
    {
    _LIT(KFunc, "ShmKey");
    INFO_PRINTF1 ( KFunc);
       
    _LIT( KaKey, "Param%d" );
    TBuf<8> pNameBuf;
    pNameBuf.Format(KaKey,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aKey);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve aKey") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    
    return KErrNone;
    }

TInt CTestShm::ShmSize( TInt& aSize )
    {
    _LIT(KFunc, "ShmSize");
    ERR_PRINTF1 ( KFunc);
    
    _LIT( KaSize, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(KaSize,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aSize);
	 
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve aSize") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    return KErrNone;
    }
        
TInt CTestShm::ShmFlag( TInt& aFlag )
    {
    _LIT(KFunc, "ShmFlag");
    ERR_PRINTF1 ( KFunc);
    
    _LIT( KaFlag, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(KaFlag,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aFlag);
	 
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve key") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    return KErrNone;
    }
  
TInt CTestShm::ShmAddr(char** aAddr )
    {
    TInt addr;
    _LIT(KFunc, "ShmAddr");
    INFO_PRINTF1 ( KFunc);
    
    
    _LIT( Kaddr, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(Kaddr,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, addr);
	 
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve addr") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    
    if ( addr == USE_DEFAULT_MAP_ADDR )
        {
        *aAddr = NULL;
        }
    else 
        {
        *aAddr = (char *)addr;
        }    
	    return KErrNone;
    }        
    
TInt CTestShm::ShmId(TInt& aId )
    {
    _LIT(KFunc, "ShmId");
    INFO_PRINTF1 ( KFunc);
    
    
     _LIT( KaId, "Param%d" );
	 TBuf<8> pNameBuf;
     pNameBuf.Format(KaId,++iParamCnt);
	 TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aId);
	 
	 if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve aId") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    if ( aId == USE_DEFAULT_ID )
        {
        aId = iNoOfShmIds[0];
        }    
    return KErrNone;
    }

TInt CTestShm::ShmCmd( TInt& aCmd )
    {
    _LIT(KFunc, "ShmCmd");
    INFO_PRINTF1 ( KFunc);
    
    
    _LIT( KaCmd, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(KaCmd,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aCmd);
	 
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve aCmd") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    return KErrNone;
    }    
TInt CTestShm::ShmSegs( TInt& aSegs )
    {
    _LIT(KFunc, "ShmSegs");
    INFO_PRINTF1 ( KFunc);
    
    _LIT( KaSegs, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(KaSegs,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aSegs);
	 
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve aSegs") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    return KErrNone;
    }    
        
TInt CTestShm::ErrCode( TInt& aErrCode )
    {
    _LIT(KFunc, "ErrCode");
    INFO_PRINTF1 ( KFunc);
    
    
    _LIT( KaErrCode, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(KaErrCode,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aErrCode);
	 
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve error code") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    return KErrNone;
    }

TInt CTestShm::CheckShmStats(struct shmid_ds& buf)
	{
    _LIT(KFunc, "CheckShmStats");
    INFO_PRINTF1 ( KFunc);
    
    if ( buf.shm_segsz != SHMSZ )
        {
        return KErrGeneral;
        }
    if ( buf.shm_nattch != 1 )
        {
        return KErrGeneral;
        }
    if ( buf.shm_perm.key != IPC_KEY )
        {
        return KErrGeneral;
        }
    return KErrNone;
	}

void CTestShm::SetShmParms(struct shmid_ds* /*buf*/)
{
}    
// -----------------------------------------------------------------------------
// CTestShm::ShmCreate
// -----------------------------------------------------------------------------
//
TInt CTestShm::ShmCreate()
    {
    _LIT(KFunc, "ShmCreate");
    INFO_PRINTF1 ( KFunc);
    
    TInt key;
    TInt shmSize;
    TInt shmFlag;
    TInt shmSegs;
    TInt terror;
    TInt err;
    TInt retVal = KErrGeneral;
    TInt shmid = -1;
    TInt i = 0;

    // retrieve passed parameters
    err = ShmKey( key);
    RETURN_ON_ERROR(err);

    err = ShmSize( shmSize);
    RETURN_ON_ERROR(err);
    
    err = ShmFlag( shmFlag);
    RETURN_ON_ERROR(err);
    
    err = ShmSegs( shmSegs);
    RETURN_ON_ERROR(err);

    err = ErrCode( terror);
    RETURN_ON_ERROR(err);

    int startIndex = 0;
    TInt index = 0;
    for(index = 0; index < MAX_IPC_OBJ; index++)
        {
        if(iNoOfShmIds[i] == -1)
            {
            startIndex = index;
            break;
            }
        }
    for (i=0; i < shmSegs; i++)
        {
        shmid = shmget(key, shmSize, shmFlag);
        if (shmid == -1)
			{
            break;         
			}
        iNoOfShmIds[startIndex + i] = shmid;
        }
        

    if (shmid == -1)
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
			 	_LIT(Kerr , "Unable to retrieve eror code") ;
			 	INFO_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            
            RETURN_ON_ERROR(KErrNone);
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
            retVal = KErrNone;
            } 
        }
        
    if(retVal != KErrNone && i < shmSegs && shmSegs > 1)
        {
        // do the cleanup
        for(TInt index = startIndex; index < MAX_IPC_OBJ; index++)
            {
            if(iNoOfShmIds[index] > 0)
                shmctl(iNoOfShmIds[index], IPC_RMID, 0);
            }
        }        
    
    //DebugLogPrintL ( KFunc(), OUT, retVal);
    return retVal;
    }
    
    // -----------------------------------------------------------------------------
// CTestShm::ShmClose
// -----------------------------------------------------------------------------
//
TInt CTestShm::ShmClose(  )
    {
    _LIT(KFunc, "ShmClose");
    INFO_PRINTF1( KFunc);

    TInt retVal = KErrGeneral;
    TInt err;
    TInt terror;
    
    err = ErrCode(terror);
    RETURN_ON_ERROR(err);
    
    for(TInt index = 0; index < MAX_IPC_OBJ; index++)
        {
        if(iNoOfShmIds[index] > 0)
			{
            err = shmctl(iNoOfShmIds[index], IPC_RMID, 0);
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
        if(terror != WITH_ERROR)
            {
            retVal = KErrNone;
            }        
        }
        
    return retVal;
    }


// -----------------------------------------------------------------------------
// CTestShm::ShmAttach
// -----------------------------------------------------------------------------
//
TInt CTestShm::ShmAttach(  )
    {
    _LIT(KFunc, "ShmAttach");
    INFO_PRINTF1( KFunc);
    
    TInt shmId;
    char* shmAddr = NULL;
    TInt shmFlag;
    TInt terror;
    TInt err;
    TInt retVal = KErrGeneral;
    
    // retrieve passed parameters
    err = ShmId( shmId);
    RETURN_ON_ERROR(err);

    err = ShmAddr( &shmAddr);
    RETURN_ON_ERROR(err);
    
    err = ShmFlag( shmFlag);
    RETURN_ON_ERROR(err);

    err = ErrCode( terror);
    RETURN_ON_ERROR(err);
    
    char* shm = (char *)shmat(shmId, shmAddr, shmFlag);
    
    if ( shm == (char *) -1 )
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
			 	_LIT(Kerr , "Unable to retrieve eror code") ;
			 	INFO_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            
            RETURN_ON_ERROR(KErrNone);
            if ( errCode == errno)
                {
                retVal = KErrNone;
                }
            }
        }
    else
        {
        iShmAddr = shm;
        if(terror != WITH_ERROR)
            {
            retVal = KErrNone;
            } 
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CTestShm::ShmDetach
// -----------------------------------------------------------------------------
//
TInt CTestShm::ShmDetach()
    {
    _LIT(KFunc, "ShmDetach");
    INFO_PRINTF1( KFunc);
    
    TInt terror;
    TInt err;
    TInt retVal = KErrGeneral;
    
    // retrieve passed parameters
    err = ErrCode( terror);
    RETURN_ON_ERROR(err);
    
    err = shmdt(iShmAddr);
    
    if (err == -1)
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
			 	_LIT(Kerr , "Unable to retrieve eror code") ;
			 	INFO_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            
            RETURN_ON_ERROR(KErrNone);
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
            retVal = KErrNone;
            } 
        }
        
	return retVal;
    }

// -----------------------------------------------------------------------------
// CTestShm::ShmControl
// -----------------------------------------------------------------------------
//
TInt CTestShm::ShmControl()
    {
    _LIT(KFunc, "ShmControl");
    INFO_PRINTF1 ( KFunc);
    
    TInt shmId;
    TInt shmCmd;
    TInt terror;
    TInt err;
    struct shmid_ds buf = {0};
    TInt retVal = KErrGeneral;
    
    // retrieve passed parameters
    err = ShmId( shmId);
    RETURN_ON_ERROR(err);
 
    err = ShmCmd( shmCmd);
    RETURN_ON_ERROR(err);
    
    if ( shmCmd == IPC_SET )
        {
        SetShmParms(&buf);
        }
    
    err = ErrCode( terror);
    RETURN_ON_ERROR(err);
    
    err = shmctl(shmId, shmCmd, &buf);
    
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
			 	_LIT(Kerr , "Unable to retrieve eror code") ;
			 	INFO_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            
            RETURN_ON_ERROR(KErrNone);
            if ( errCode == errno)
                {
                retVal = KErrNone;
                }
            }    
        }
    else
        {
        if ( shmCmd == IPC_STAT )
            {
            retVal = CheckShmStats(buf);
            }
        else if ( terror != WITH_ERROR )
            {
            retVal = KErrNone;
            }    
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CTestShm::CreateThreadL
// -----------------------------------------------------------------------------
//
TInt CTestShm::CreateThreadL()
	{
    _LIT(KFunc, "CreateThreadL");
    INFO_PRINTF1 ( KFunc);
    TInt threadType;
    TInt err=KErrGeneral;
    _LIT( KthreadType, "Param%d" );
	TBuf<8> pNameBuf;
	pNameBuf.Format(KthreadType,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, threadType);

	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve thread type") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    
    switch(threadType)
        {
        case 1: 
            err = pthread_create((unsigned int *)&iThreadId,(pthread_attr_t *)NULL,ThreadEntryFunctionRead,(void*)this);
            err = ShmServerWrite();
            break;
        case 2:
            User::After(100000);
            break;
        case 3:
            User::After(100000);
            break;
        default:
            err = KErrNone;
            break;            
        }
    if ( err )
        {
        ERR_PRINTF2 ( _L("pthread_create %d"), err);
        }
    return err;
	}

// -----------------------------------------------------------------------------
// CTestShm::ShmWrite
// -----------------------------------------------------------------------------
//
TInt CTestShm::ShmServerWrite()
	{    
    _LIT(KFunc, "ShmWrite");
    TInt retVal = KErrGeneral;  
    INFO_PRINTF1 ( KFunc);
    TInt shmId = shmget (IPC_KEY, SHMSZ, IPC_CREAT|0666);
    if ( shmId < 0 )
		{
       retVal = errno;
       ERR_PRINTF2 ( _L("ShmWrite:shmget error: %d"), retVal);
       return retVal;
		}
    char *shmAddr = (char *)shmat(shmId, NULL, 0);
    if ( shmAddr == (char *) -1 )
		{
        retVal = errno;
        ERR_PRINTF2 ( _L("ShmWrite:shmat error: %d"), retVal);
        return retVal;
		}
    char* s = shmAddr;
	for (char c = 'a'; c <= 'h'; c++)
		{
        *s++ = c;
		}
    *s = '\0';
    
    TInt tmout = 0;
    while ( tmout < 5 )
		{
       User::After(500000);
       tmout++;
		}

    if ( *shmAddr != '*' )
		{
        ERR_PRINTF2 ( _L("Timeout error: %d"), retVal);        
		}
    else
		{
        retVal = KErrNone;
		}
    
    TInt err = shmdt(shmAddr);
    
    if ( err < 0 )
		{
        retVal = errno;
        ERR_PRINTF2 ( _L("ShmWrite:shmdt error: %d"), retVal);
		}
      
    err = shmctl(shmId, IPC_RMID, 0);  
    if ( err < 0 )
		{
        retVal = errno;
        ERR_PRINTF2 ( _L("ShmWrite:shmctl error: %d"), retVal);
		}
    return retVal;
	}
// -----------------------------------------------------------------------------
// CTestShm::ThreadEntryFunctionRead
// -----------------------------------------------------------------------------
//    
void* ::ThreadEntryFunctionRead(void* /*arg*/)
	{
    User::After(500000);
    
    TInt shmId = shmget (IPC_KEY, SHMSZ, 0666);
    
    if ( shmId < 0 )
		{
       return NULL;
		}
    
    char *shmAddr = (char *)shmat(shmId, NULL, 0);
    
    if ( shmAddr == (char *) -1 )
		{
        return NULL;
		}
    
    User::After(500000);
    
    if (strcmp(shmAddr, "abcdefgh") == 0)
		{
        *shmAddr = '*';
		}
    
    shmdt(shmAddr);
    
    return NULL;
	}    
      

TInt CTestShm::IntgTest1()
	{
	TInt err = ShmCreate();
	if(!err)
		{
		err = ShmClose();
		}
	return err;
	}

TInt CTestShm::IntgTest2()
	{
	TInt err = ShmCreate();
	if(!err) 
		{
		err = ShmCreate();
		if(!err)
			{
			err = ShmClose();
			}
		}
	return err;
	}

TInt CTestShm::IntgTest3()
	{
	TInt err = ShmCreate();
	if(!err) 
		{
		err = ShmAttach();
		if(!err) 
			{
			err = ShmDetach();
			if(!err)
				{
				err = ShmClose();
				}
			}
		}
	return err;
	}

TInt CTestShm::IntgTest4()
	{
	TInt err = ShmCreate();
	if(!err) 
		{
		err = ShmAttach();
		if(!err)
			{
			err = ShmClose();
			}
		}
	return err;
	}

TInt CTestShm::IntgTest5()
	{
	TInt err = ShmCreate();
	if(!err) 
		{
		err = ShmAttach();
		if(!err) 
			{
			err = ShmControl();
			if(!err) 
				{
				err = ShmDetach();
				if(!err)
					{
					ShmClose();
					}
				}
			}
		}
	return err;
	}

TInt CTestShm::IntgTest6()
	{
	TInt err = ShmCreate();
	if(!err) 
		{
		err = ShmAttach();
		if(!err) 
			{
			err = ShmDetach();
			if(!err)
				{
				err = ShmClose();
				if(!err)
					{
					err = ShmControl();
					}
				}
			}
		}
	return err;
	}

TInt CTestShm::IntgTest7()
	{
	errno=0;
	TInt err = ShmCreate();
	if(!err) 
		{
		errno=0;
		err = ShmControl();
		if(!err)
			{
			errno=0;
			err = ShmClose();
			}
		}
	return err;
	}

typedef struct
{
	void *ptr;
	FILE *outFile;
	int   err[4];
} pthread_arg_t;

//#define myerror perror
int CTestShm::myerror(const char* prompt, FILE* outFile)
{
    int err = errno;
    char* p= strerror(err);

    fprintf(outFile, "%s [%d] :%s\n", prompt, err, p);
    return err;
}

void* body_entry_point(void *pv)
{
	TInt err;
	err = ((CTestShm*)(((pthread_arg_t*)pv)->ptr))->body(pv);
	((pthread_arg_t*)pv)->err[3] = err;
	return pv;
}

int CTestShm::body(void* pv)
{
	TInt key = 2000;
	TInt shmSize = 1024;
	TInt err = KErrNone;
	pthread_arg_t arg;
	
	arg.outFile = ((pthread_arg_t*)pv)->outFile;
	arg.ptr     = ((pthread_arg_t*)pv)->ptr;
	
    // retrieve passed parameters
	
    int id = shmget(key, shmSize, IPC_CREAT|0666);
    
    if (id < 0)
        return myerror("[M] shmget", arg.outFile);
    else
    	fprintf(arg.outFile, "[M] create shm [%d], get [%d]\n", key, id);
    	//INFO_PRINTF3(_L("[M] create shm [%d], get [%d]\n"), key, id);
    sleep(2);

    void* p = shmat(id, 0, 0);

    if (p == (void*)(-1))
        return myerror("[M] shmat", arg.outFile);
    else
    	fprintf(arg.outFile, "[M] attach shm[%d], get %p\n", id, p);
    	//INFO_PRINTF3(_L("[M] attach shm[%d], get %p\n"), id, p);
    sleep(3);

    if (shmdt(p) < 0)
        return myerror("[M] shmdt", arg.outFile);
    else
    	fprintf(arg.outFile, "[M] detach %p, from shm[%d]\n", p, id);
    	//INFO_PRINTF3(_L("[M] detach %p, from shm[%d]\n"), p, id);
    sleep(3);

    if (shmctl(id, IPC_RMID, 0) < 0)
    	fprintf(arg.outFile, "[M] shmctl(rmid)\n");
    	//INFO_PRINTF1(_L("[M] shmctl(rmid)"));
    else
    	fprintf(arg.outFile, "[M] remove shm[%d]\n", id);
    	//INFO_PRINTF2(_L("[M] remove shm[%d]\n"), id);

    return err;
}

void* myfunc1_entry_point(void* pv)
{
	TInt err;
	err = ((CTestShm*)(((pthread_arg_t*)pv)->ptr))->myfunc1(pv);
	((pthread_arg_t*)pv)->err[0] = err;
	return pv;
}

int CTestShm::myfunc1(void* pv)
{
    sleep(1);

	TInt key = 2000 ;
	TInt shmSize = 1024 ;
	TInt err = KErrNone;
	pthread_arg_t arg;
	
	arg.outFile = ((pthread_arg_t*)pv)->outFile;
	arg.ptr     = ((pthread_arg_t*)pv)->ptr;
	
    // retrieve passed parameters

    int id = shmget(key, shmSize, 0666);
    if (id < 0)
    	return myerror("[C1] shmget", arg.outFile);
    else
		fprintf(arg.outFile, "[C1] open shm [%d], get [%d]\n", key, id);
    	//INFO_PRINTF3(_L("[C1] open shm [%d], get [%d]\n"), key, id);

    sleep(2);

    void* p = shmat(id, 0, 0);
    if (p == (void*)(-1))
        return myerror("[C1] shmat", arg.outFile);
    else
    	fprintf(arg.outFile, "[C1] attach shm[%d], get %p\n", id, p);
    	//INFO_PRINTF3(_L("[C1] attach shm[%d], get %p\n"), id, p);

    sleep(3);

    if (shmdt(p) < 0)
        return myerror("[C1] shmdt", arg.outFile);
    else
    	fprintf(arg.outFile, "[C1] detach %p, from shm[%d]\n", p, id);
    	//INFO_PRINTF3(_L("[C1] detach %p, from shm[%d]\n"), p, id);

    sleep(2);

    if (shmctl(id, IPC_RMID, 0) < 0)
    	fprintf(arg.outFile, "[C1] shmctl(rmid)\n");
    	//INFO_PRINTF1(_L("[C1] shmctl(rmid)"));
    else
    	fprintf(arg.outFile, "[C1] remove shm[%d]\n", id);
    	//INFO_PRINTF2(_L("[C1] remove shm[%d]\n"), id);

    return err;
}

void* myfunc2_entry_point(void* pv)
{
	TInt err;
	err = ((CTestShm*)(((pthread_arg_t*)pv)->ptr))->myfunc2(pv);
	((pthread_arg_t*)pv)->err[1] = err;
	return pv;
}

int CTestShm::myfunc2(void* pv)
{
    sleep(1);
	
    TInt key = 2000;
	TInt shmSize = 1024;
	TInt err = KErrNone;
	pthread_arg_t arg;
	
	arg.outFile = ((pthread_arg_t*)pv)->outFile;
	arg.ptr     = ((pthread_arg_t*)pv)->ptr;
	
    // retrieve passed parameters

    int id = shmget(key, shmSize, 0666);
    if (id < 0)
        return myerror("[C2] shmget", arg.outFile);
    else
    	fprintf(arg.outFile, "[C2] open shm [%d], get [%d]\n", key, id);
    	//INFO_PRINTF3(_L("[C2] open shm [%d], get [%d]\n"), key, id);

    sleep(2);

    void* p = shmat(id, 0, 0);
    if (p == (void*)(-1))
        return myerror("[C2] shmat", arg.outFile);
    else
    	fprintf(arg.outFile, "[C2] attach shm[%d], get %p\n", id, p);
    	//INFO_PRINTF3(_L("[C2] attach shm[%d], get %p\n"), id, p);

    sleep(1);

    if (shmdt(p) < 0)
        return myerror("[C2] shmdt", arg.outFile);
    else
    	fprintf(arg.outFile, "[C2] detach %p, from shm[%d]\n", p, id);
    	//INFO_PRINTF3(_L("[C2] detach %p, from shm[%d]\n"), p, id);

    sleep(4);

    if (shmctl(id, IPC_RMID, 0) < 0)
    	fprintf(arg.outFile, "[C2] shmctl(rmid)\n");
    	//INFO_PRINTF1(_L("[C2] shmctl(rmid)"));
    else
    	fprintf(arg.outFile, "[C2] remove shm[%d]\n", id);
    	//INFO_PRINTF2(_L("[C2] remove shm[%d]\n"), id);

    return err;
}

void* myfunc3_entry_point(void* pv)
{
	TInt err;
	err = ((CTestShm*)(((pthread_arg_t*)pv)->ptr))->myfunc3(pv);
	((pthread_arg_t*)pv)->err[2] = err;
	return pv;
}

int CTestShm::myfunc3(void* pv)
{
    sleep(1);

    TInt key =  2000;
	TInt shmSize = 1024;
	TInt err = KErrNone;
	pthread_arg_t arg;
	
	arg.outFile = ((pthread_arg_t*)pv)->outFile;
	arg.ptr     = ((pthread_arg_t*)pv)->ptr;
	
    // retrieve passed parameters
    
    int id = shmget(key, shmSize, 0666);
    if (id < 0)
        return myerror("[C3] shmget", arg.outFile);
    else
    	fprintf(arg.outFile, "[C3] open shm [%d], get [%d]\n", key, id);
	    //INFO_PRINTF3(_L("[C3] open shm [%d], get [%d]\n"), key, id);
    
    sleep(5);

    void* p = shmat(id, 0, 0);
    if (p == (void*)(-1))
        return myerror("[C3] shmat", arg.outFile);
    else
    	fprintf(arg.outFile, "[C3] attach shm[%d], get %p\n", id, p);
    	//INFO_PRINTF3(_L("[C3] attach shm[%d], get %p\n"), id, p);

    sleep(1);

    if (shmdt(p) < 0)
        return myerror("[C3] shmdt", arg.outFile);
    else
    	fprintf(arg.outFile, "[C3] detach %p, from shm[%d]\n", p, id);
    	//INFO_PRINTF3(_L("[C3] detach %p, from shm[%d]\n"), p, id);

    sleep(1);

    if (shmctl(id, IPC_RMID, 0) < 0)
    	fprintf(arg.outFile, "[C3] shmctl(rmid)\n");
    	//INFO_PRINTF1(_L("[C3] shmctl(rmid)"));
    else
    	fprintf(arg.outFile, "[C3] remove shm[%d]\n", id);
    	//INFO_PRINTF2(_L("[C3] remove shm[%d]\n"), id);

    return err;
}

TInt CTestShm::CreateThreadL1()
	{
	INFO_PRINTF1(_L("        0 1 2 3 4 5 6 7 8\n")) ;	
	INFO_PRINTF1(_L("main    C   A     D     R\n")) ;	
	INFO_PRINTF1(_L("cs 1      O   A     D   R\n")) ;	
	INFO_PRINTF1(_L("cs 2      O   A D       R\n")) ;	
	INFO_PRINTF1(_L("cs 3      O         A D R\n")) ;
	
	pthread_t thr[4];
	pthread_arg_t arg;
		
	FILE* outFile = fopen("C:\\create_thread.txt", "w+");
	
	if(NULL == outFile)
		{
		return KErrGeneral;		
		}
	
	arg.ptr = (void*)this;
	arg.outFile = outFile;
	
	pthread_create(&thr[0], 0, myfunc1_entry_point, (void*)&arg);
	pthread_create(&thr[1], 0, myfunc2_entry_point, (void*)&arg);
	pthread_create(&thr[2], 0, myfunc3_entry_point, (void*)&arg);
	pthread_create(&thr[3], 0, body_entry_point, (void*)&arg);
	
	pthread_join(thr[0], (void**)&arg);
	if(KErrNone != arg.err[0])
		{
		return arg.err[0];
		}

	pthread_join(thr[1], (void**)&arg);
	if(KErrNone != arg.err[1])
		{
		return arg.err[1];
		}

	pthread_join(thr[2], (void**)&arg);
	if(KErrNone != arg.err[2])
		{
		return arg.err[2];
		}

	pthread_join(thr[3], (void**)&arg);
	if(KErrNone != arg.err[3])
		{
		return arg.err[3];
		}

	fclose(outFile);	
	return 0;
	}
				 // SHMGET
				 
//The argument key is equal to IPC_PRIVATE.

TInt CTestShm::shmgettest1()
	{
	int shmid,ret = KErrNone;
	int shmSize=100;
	
	shmid = shmget(IPC_PRIVATE, shmSize, IPC_CREAT|IPC_EXCL|0666);
	if (shmid == -1)
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}
    
	_LIT(KFunc, "shmget passed IPC_PRIVATE\n");
    INFO_PRINTF1(KFunc);
    
    if(shmctl(shmid,IPC_RMID,(struct shmid_ds *) NULL) == -1)
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
    return ret;
	}

//shmget with the specified key 

TInt CTestShm::shmgettest2()
	{
	int shmid,ret = KErrNone;
	int shmSize=100;
	key_t key = 1000;
	
	shmid = shmget(key, shmSize, IPC_CREAT|IPC_EXCL|0666);
	if (shmid == -1)
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}
    
	_LIT(KFunc, "shmget passed for the specified key\n");
    INFO_PRINTF1(KFunc);
    
    if(shmctl(shmid,IPC_RMID,(struct shmid_ds *) NULL) == -1)
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
    return ret;
	}


// [EEXIST] 

TInt CTestShm::shmgettest3()
	{
	int shmid,ret = KErrNone;
	int shmSize=100;
	
	shmid = shmget(1000, shmSize, IPC_CREAT|IPC_EXCL|0666);
	if (shmid == -1)
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}
    
	_LIT(KFunc, "shmget passed IPC_PRIVATE\n");
    INFO_PRINTF1(KFunc);
    if(shmget(1000, shmSize, IPC_CREAT|IPC_EXCL|0666) == -1 && errno == EEXIST)
    	{  		
		_LIT(KFunc, "shared memory set the proper the errno\n");
      	INFO_PRINTF1(KFunc);
    	}
    else
    	{   	  		
		_LIT(KFunc, "shared memory didnt set the proper the errno\n");
      	INFO_PRINTF1(KFunc);
      	ret = KErrGeneral;
    	}
    if(shmctl(shmid,IPC_RMID,(struct shmid_ds *) NULL) == -1)
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
    return ret;
	}

//EINVAL] 
//A shared memory segment is to be created and the value of size is less than the system-imposed minimum or greater than the system-imposed maximum. 

TInt CTestShm::shmgettest4()
	{
	int shmid;
	int shmSize=0;
	
	shmid = shmget(IPC_PRIVATE, shmSize, IPC_CREAT|IPC_EXCL|0666);
	if (shmid != -1 && errno != EINVAL) 
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}
    
	_LIT(KFunc, "shmget passed IPC_PRIVATE %d\n");
    INFO_PRINTF2(KFunc,errno);
    
    return KErrNone;
	}

// look for maximum imposed limit 
TInt CTestShm::shmgettest5()
	{
	int shmid;
	int shmSize=100000000; // TO DO
	
	shmid = shmget(IPC_PRIVATE, shmSize, IPC_CREAT|IPC_EXCL|0666);
	if (shmid != -1 && errno != ENOMEM) 
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}

	_LIT(KFunc, "shmget passed IPC_PRIVATE %d\n");
    INFO_PRINTF2(KFunc,errno);
    
    return KErrNone;
	}

//[EINVAL] 
//No shared memory segment is to be created and a shared memory segment exists for key but the size of the segment associated with it is less than size and size is not 0. 

TInt CTestShm::shmgettest6()
	{
	int shmid1,shmid2;
	int shmSize=100; 
	int ret = KErrNone;
	
	shmid1 = shmget(SHM_KEY, shmSize, IPC_CREAT|IPC_EXCL|0666);
	if (shmid1 == -1 ) 
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}
    shmSize--;
    shmid2 = shmget(SHM_KEY, shmSize, 0666);
	if (shmid2 != -1 && errno != EINVAL) 
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
	_LIT(KFunc, "shmget passed IPC_PRIVATE\n");
    INFO_PRINTF1(KFunc);
    
    if(shmctl(shmid1,IPC_RMID,(struct shmid_ds *) NULL) == -1)
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
    return ret;
	}

//[ENOENT] 
//A shared memory identifier does not exist for the argument key and (shmflg &IPC_CREAT) is 0. 

TInt CTestShm::shmgettest7()
	{
	int shmid;
	int shmSize=100;
	
	shmid = shmget(10000, shmSize, IPC_CREAT & 0);
	if (shmid != -1 && errno != ENOENT ) 
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}

    return KErrNone;
	}

//[ENOSPC] 
//A shared memory identifier is to be created, but the system-imposed limit on the maximum number of allowed shared memory identifiers system-wide would be exceeded. 

TInt CTestShm::shmgettest8()
	{
	int shm_id[1512];
	int shmSize=100;
	
	int i=0,j=0;
	int ret = KErrNone;
	for(i=0;;i++)
		{
		if ((shm_id[i] = shmget(IPC_PRIVATE, shmSize, IPC_CREAT|IPC_EXCL|0666)) == -1) 
			{
			if(errno == ENOSPC)
				{
				_LIT(KFunc, "shared memory create set the proper errno %d\n");
      			INFO_PRINTF2(KFunc,i);
       			break;
				}
			else
				{
				_LIT(KFunc, "shared memory create didnt set the proper errno i %d errno %d\n");
      			INFO_PRINTF3(KFunc,i,errno);
      			ret=KErrGeneral;
       			break;
				}
    		}
		}
	
	for(j=0;j<i;j++)
		{
		if (shmctl(shm_id[j],IPC_RMID,(struct shmid_ds *) NULL) == -1) 
    		{
			_LIT(KFunc, "semaphore delete failed with errno:%d j:%d id:%d\n");
      		INFO_PRINTF4(KFunc, errno,j,shm_id[j]);
       		ret= KErrGeneral;
       		break;
    		}
		}
    	     	
	return ret;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CTestShm::ShmgetCreateKey()
// Description   : 	This test creates a shared memory instance with the 
//					specified key and size, and deletes it.
// Param1		 :  Key to use for shmget (passed through the ini file)
// Param2		 :  Size of the shared memory (passed through the ini file)
//---------------------------------------------------------------------------------
TInt CTestShm::ShmgetCreateKey()
	{
	int shmid;
	int key;
	int size;
	TBuf<8> pNameBuf;
	
	iParamCnt = 0;
	_LIT( KaShmNum, "Param%d" );
	
	pNameBuf.Format(KaShmNum,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, key);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve shm key") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
	pNameBuf.Format(KaShmNum,++iParamCnt);
	res = GetIntFromConfig(ConfigSection(), pNameBuf, size);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve shm size") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
	shmid = shmget(key, size, IPC_CREAT);
	if (shmid < 0 ) 
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}

	_LIT(KFunc, "shmget passed for the specified key\n");
    INFO_PRINTF1(KFunc);
    
    if(shmctl(shmid,IPC_RMID,(struct shmid_ds *) NULL) == -1)
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}
    
    return KErrNone;
	}


					// SHMCTL

// [EINVAL] -  shmid is not a valid shared memory identifier

TInt CTestShm::shmctltest1()
	{
	int shmid=-1,ret = KErrNone,retn;
	
	retn = shmctl(shmid,IPC_RMID,(struct shmid_ds *) NULL);
		
	if( retn != -1 && errno != EINVAL )
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
    return ret;
	}

// [EINVAL] -  value of cmd is not a valid command. 

TInt CTestShm::shmctltest2()
	{
	int shmid,ret = KErrNone,retn;
	int shmSize = 100;
	
	shmid = shmget(IPC_PRIVATE, shmSize, IPC_CREAT|IPC_EXCL|0666);
	if (shmid == -1)
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}
    
	retn = shmctl(shmid,NULL,(struct shmid_ds *) NULL);
		
	if( retn != -1 && errno != EINVAL )
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
    return ret;
	}

// with flag IPC_STAT
//The values of shm_perm.cuid, shm_perm.uid, shm_perm.cgid, and shm_perm.gid are set equal to the effective user ID and effective group ID, respectively, of the calling process.

//The value of shm_ctime is set equal to the current time.


TInt CTestShm::shmctltest3()
	{
	int shmid,ret = KErrNone,retn;
	int shmSize = 100;
	struct shmid_ds shmid_ds1, *buf;
	int mode = IPC_CREAT|IPC_EXCL|0666;
	
    buf = &shmid_ds1;

	shmid = shmget(IPC_PRIVATE, shmSize, mode);
	if (shmid == -1)
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}
    
	retn = shmctl(shmid,IPC_STAT,buf);
		
	if( retn == -1)
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}

	if(buf->shm_segsz != shmSize)
		{
		ret = KErrGeneral;
		}
    
    if(buf->shm_lpid != 0)
		{
		ret = KErrGeneral;
		}
	if(buf->shm_nattch != 0)
		{
		ret = KErrGeneral;
		}
	if(buf->shm_atime != 0)
		{
		ret = KErrGeneral;
		}
	if(buf->shm_dtime != 0)
		{
		ret = KErrGeneral;
		}
	
	if(buf->shm_perm.key != IPC_PRIVATE)
		{
		ret = KErrGeneral;
		}
	
	if(buf->shm_perm.mode != mode)
		{
		ret = KErrGeneral;
		}
			
	if(shmctl(shmid,IPC_RMID,(struct shmid_ds *) NULL) == -1)
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
    return ret;
	}

// IPC_SET

TInt CTestShm::shmctltest4()
	{
	int shmid,ret = KErrNone,retn;
	int shmSize = 100;
	struct shmid_ds shmid_ds1, *buf , *buf1;
	int mode = IPC_CREAT|IPC_EXCL|0666;
	
    buf = &shmid_ds1;
	buf1 = &shmid_ds1;
	
	shmid = shmget(IPC_PRIVATE, shmSize, mode);
	if (shmid == -1)
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}
    
	retn = shmctl(shmid,IPC_STAT,buf);
		
	if( retn == -1)
    	{
		_LIT(KFunc, "shmctl IPC_STAT failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
	
	// setting the mode value 
	
	buf->shm_perm.mode = 0660;
	
	retn = shmctl(shmid, IPC_SET,buf);
	if( retn == -1)
    	{
		_LIT(KFunc, "shmctl IPC_SET failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
    retn = shmctl(shmid,IPC_STAT,buf1);
		
	if( retn == -1)
    	{
		_LIT(KFunc, "shmctl IPC_STAT2 failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
	if(buf1->shm_segsz != shmSize)
		{
		ret = KErrGeneral;
		}
    
	if(buf1->shm_nattch != 0)
		{
		ret = KErrGeneral;
		}
	if(buf1->shm_atime != 0)
		{
		ret = KErrGeneral;
		}
	if(buf1->shm_dtime != 0)
		{
		ret = KErrGeneral;
		}
	
	if(buf1->shm_perm.key != IPC_PRIVATE)
		{
		ret = KErrGeneral;
		}
	
	if(buf1->shm_perm.mode != 432)
		{
		ret = KErrGeneral;
		}
	
			
	if(shmctl(shmid,IPC_RMID,(struct shmid_ds *) NULL) == -1)
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
    return ret;
	}

// [ EACCES ]

TInt CTestShm::shmctltest5()
	{
	int shmid,ret = KErrNone,retn;
	int shmSize = 100;
	struct shmid_ds shmid_ds1, *buf;
	int mode = IPC_CREAT|IPC_EXCL;
	
    buf = &shmid_ds1;
	
	shmid = shmget(SHM_KEY, shmSize, mode);
	if (shmid == -1)
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}
    
	retn = shmctl(shmid,IPC_STAT,buf);
		
	if( retn != -1 && errno != EACCES )
    	{
		_LIT(KFunc, "shmctl1 failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
	if(shmctl(shmid,IPC_RMID,(struct shmid_ds *) NULL) == -1)
    	{
		_LIT(KFunc, "shmctl2 failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
    return ret;
	}


				// SHMAT

// shmat with normal behaviour
				
TInt CTestShm::shmattest1()
	{
	int shmid,ret = KErrNone;
	int shmSize=100;
	char *shm_addr;

	shmid = shmget(IPC_PRIVATE, shmSize, IPC_CREAT|IPC_EXCL|0666);
	if (shmid == -1)
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}
    
	_LIT(KFunc, "shmget passed IPC_PRIVATE\n");
    INFO_PRINTF1(KFunc);
    
    if((shm_addr = (char *)shmat(shmid, NULL, 0)) == (void *)-1) 
    	{
		_LIT(KFunc, "shmat failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}

    if(shmdt(shm_addr) == -1) 
    	{
		_LIT(KFunc, "shmdt failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}

    if(shmctl(shmid,IPC_RMID,(struct shmid_ds *) NULL) == -1)
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
    return ret;
	}

// shamat :: check for the variable shm_nattch

TInt CTestShm::shmattest2()
	{
	int shmid,ret = KErrNone,retn;
	int shmSize=100;
	char *shm_addr;
	struct shmid_ds shmid_ds1, *buf;
	buf = &shmid_ds1;

	shmid = shmget(IPC_PRIVATE, shmSize, IPC_CREAT|IPC_EXCL|0666);
	if (shmid == -1)
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}
    
	_LIT(KFunc, "shmget passed IPC_PRIVATE\n");
    INFO_PRINTF1(KFunc);
    
    // get the value of shm_nattch
    retn = shmctl(shmid,IPC_STAT,buf);
		
	if( retn == -1)
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
    if(buf->shm_nattch != 0)
		{
		ret = KErrGeneral;
		}
		
    if((shm_addr = (char *)shmat(shmid, NULL, 0)) == (void *)-1) 
    	{
		_LIT(KFunc, "shmat failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
	
	// get the value of shm_nattch
	retn = shmctl(shmid,IPC_STAT,buf);
		
	if( retn == -1)
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
    if(buf->shm_nattch != 1)
		{
		ret = KErrGeneral;
		}
	
    if(shmdt(shm_addr) == -1) 
    	{
		_LIT(KFunc, "shmdt failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
	
	// get the value of shm_nattch
	retn = shmctl(shmid,IPC_STAT,buf);
		
	if( retn == -1)
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
    if(buf->shm_nattch != 0)
		{
		ret = KErrGeneral;
		}
	
    if(shmctl(shmid,IPC_RMID,(struct shmid_ds *) NULL) == -1)
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
    return ret;
	}


// [EINVAL]

TInt CTestShm::shmattest3()
	{
	int shmid=-1,ret = KErrGeneral;
	char *shm_addr;
	
	shm_addr = (char *)shmat(shmid, NULL, 0);
    if(shm_addr == (void *)-1 && errno == EINVAL) 
    	{
		_LIT(KFunc, "shmat set the proper errno\n");
      	INFO_PRINTF1(KFunc);
      	ret = KErrNone;
    	}

    return ret;
	}


// COMBINATION OF SHMAT AND SHMDT WITHOUT THREADS

TInt CTestShm::shmattest4()
	{
	int shmid,ret = KErrNone;
	int shmSize=100;
	char *shm_addr;

	shmid = shmget(IPC_PRIVATE, shmSize, IPC_CREAT|IPC_EXCL|0666);
	if (shmid == -1)
		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}
    
	_LIT(KFunc, "shmget passed IPC_PRIVATE\n");
    INFO_PRINTF1(KFunc);
    
    if((shm_addr = (char *)shmat(shmid, NULL, 0)) == (void *)-1) 
    	{
		_LIT(KFunc, "shmat failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
	
	if(strcpy(shm_addr, "some_random_data") == NULL)
		{
		_LIT(KFunc, "failed to write in the shared memory segment with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}

	if(shmdt(shm_addr) == -1) 
    	{
		_LIT(KFunc, "shmdt failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}

    if(shmctl(shmid,IPC_RMID,(struct shmid_ds *) NULL) == -1)
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
    return ret;
	}

// COMBINATION OF SHMAT AND SHMDT WITH THREADS

void* ShmReadThread(void* shmid2)
	{
   	int shmid,shmid1;
   	char* ch;
   	char name[10];
	int iFlag=0;

   	shmid = shmget(SHM_KEY, 1024, IPC_CREAT|0666);
   	if( shmid == -1)
   		{
   		iFlag = -1 ;
   		}
 
   	shmid1 = (int)shmid2;
   	ch = (char*)shmat(shmid1, 0, 0);
   	shmid1 = errno;
   	if((int)ch == -1)
   		{
   		iFlag = -1 ;
   		}
   		
   	sleep(5);	
   	
   	strcpy(name, ch);
   	
   	if(strcmp(name,"abc") != 0)
   		{
   		iFlag = -1;
   		}
   		
   	if(shmdt(ch) == -1) 
    	{
   		iFlag = -1;
   		}
   	
   	if(iFlag == - 1)
   		{
   		strcpy(ch, "*");
   		}
   	else
   		{
   		strcpy(ch, "!");
   		}
   	
   	return 0;
	}

TInt CTestShm::shmattest5()
	{
	char* ch;
   	int shmid;
   	int threadRetVal = 0,ret = KErrNone;
   	pthread_t threadid;


   	shmid = shmget(SHM_KEY, 1024, IPC_CREAT|0666);
   	if( shmid == -1)
   		{
		_LIT(KFunc, "shmget failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	return KErrGeneral;
    	}

   	ch = (char*)shmat(shmid, 0, 0);

   	if((int)ch == -1)
   		{
       	shmctl(shmid, IPC_RMID, NULL);
       	return KErrGeneral;
   		}
   		
   	if(strcpy(ch, "abc") == NULL)
   		{
		_LIT(KFunc, "failed to write in the shared memory segment with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
   	pthread_create(&threadid,(pthread_attr_t *)NULL,ShmReadThread, (void*)shmid);
   
   	pthread_join(threadid, (void**)threadRetVal);
   	
   	if((strcmp(ch,"!")))
   		{
   		ret = KErrGeneral;
   		}
   	
   	if(shmdt(ch) == -1) 
    	{
		_LIT(KFunc, "shmdt failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    	
   	if(shmctl(shmid,IPC_RMID,(struct shmid_ds *) NULL) == -1)
    	{
		_LIT(KFunc, "shmctl failed with errno %d\n");
      	INFO_PRINTF2(KFunc,errno);
      	ret = KErrGeneral;
    	}
    
   	return ret;
	}

				// SHMDT 
				
TInt CTestShm::shmdttest1()
	{
	int retn,ret = KErrGeneral;
	char *shm_addr=NULL;

	retn = shmdt(shm_addr);
	
	if(retn == -1 && errno == EINVAL)
		{
		_LIT(KFunc, "shmdt set the proper errno\n");
      	INFO_PRINTF1(KFunc);
      	ret = KErrNone;
    	}

    return ret;
	}
	
/*shared memory operations and retrieving the shmid_ds structure*/
TInt CTestShm::shmopsvalidatetest()
	{
	int shmem_size;
	int sleep_value;
	struct shmid_ds shmDesc;
	int	shmid;			/* (Unique) Shared memory identifier */
	char *shmptr;		/* Shared memory segment address */
	char *ptr;			/* Index into shared memory segment */
	char value = 0;		/* Value written into shared memory segment */

	_LIT( Kshmem_size, "shmem_size" );
	TBool res = GetIntFromConfig(ConfigSection(), Kshmem_size, shmem_size);
	if(!res)
		{
		_LIT(Kerr , "Failed to read shmem_size") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}

	_LIT( Ksleep_value, "sleep_value" );
	res = GetIntFromConfig(ConfigSection(), Ksleep_value, sleep_value);
	if(!res)
		{
		_LIT(Kerr , "Failed to read sleep_value") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}

	pid_t pid;
	if((pid = getpid()) < 0 )
		{
	  	INFO_PRINTF2(_L("getpid system call failed %d\n"), __LINE__);
	  	return KErrGeneral;
		}

	INFO_PRINTF2(_L("\nSize of shared memory segment (%d bytes)\n"), shmem_size);
	if ((shmid = shmget (IPC_PRIVATE, shmem_size, SHM_R|SHM_W)) < 0)
		{		
	  	INFO_PRINTF2(_L("shmget failed %d\n"), __LINE__);
	  	return KErrGeneral;		
		}
	
	INFO_PRINTF1(_L("\nretrieve shared memory segment structure after shmget()\n"));
	if (shmctl (shmid, IPC_STAT,&shmDesc) < 0)
		{		
	  	INFO_PRINTF2(_L("shmctl failed %d\n"), __LINE__);
	  	return KErrGeneral;		
		}
	else
		{
		INFO_PRINTF2(_L("\nThe USER ID = %d\n"), shmDesc.shm_perm.uid); 
		INFO_PRINTF2(_L("The GROUP ID = %d\n"), shmDesc.shm_perm.gid); 
		INFO_PRINTF2(_L("The creator's ID = %d\n"), shmDesc.shm_perm.cuid); 
		INFO_PRINTF2(_L("The creator's group ID = %d\n"), shmDesc.shm_perm.cgid);
		INFO_PRINTF2(_L("The operation permissions = 0%o\n"), shmDesc.shm_perm.mode); 
		INFO_PRINTF1(_L("The slot usage sequence\n")); 
		INFO_PRINTF2(_L("number = %d\n"),  shmDesc.shm_perm.seq); 
		INFO_PRINTF2(_L("The key= %d\n"), shmDesc.shm_perm.key); 
		INFO_PRINTF2(_L("The segment size = %d\n"), shmDesc.shm_segsz); 
		INFO_PRINTF2(_L("The pid of last shmop = %d\n"), shmDesc.shm_lpid); 
		INFO_PRINTF2(_L("The pid of creator = %d\n"), shmDesc.shm_cpid); 
		INFO_PRINTF2(_L("The current # attached = %d\n"), shmDesc.shm_nattch); 
		INFO_PRINTF2(_L("The last shmat time = %d\n"), shmDesc.shm_atime); 
		INFO_PRINTF2(_L("The last shmdt time = %d\n"),  shmDesc.shm_dtime); 
		INFO_PRINTF2(_L("The last change time = %d\n"),  shmDesc.shm_ctime);
		if (shmDesc.shm_perm.uid != pid)
			{
			INFO_PRINTF1(_L("Owner user id shm_perm.uid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_perm.gid != 0)
			{
			INFO_PRINTF1(_L("Owner group id shm_perm.gid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_perm.cuid != pid)
			{
			INFO_PRINTF1(_L("Creator user id shm_perm.cuid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_perm.cgid != 0)
			{
			INFO_PRINTF1(_L("Creator group id shm_perm.cgid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_lpid != 0)
			{
			INFO_PRINTF1(_L("Process ID of last shared memory operation shm_lpid is not 0\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_cpid != pid)
			{
			INFO_PRINTF1(_L("Process ID of creator shm_cpid is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_nattch != 0)
			{
			INFO_PRINTF1(_L("Number of current attaches shm_nattch is not 0\n"));
			}
		if (shmDesc.shm_atime != 0)
			{
			INFO_PRINTF1(_L("time of last shmat() shm_atime is not 0\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_dtime != 0)
			{
			INFO_PRINTF1(_L("time of last shmdt() shm_dtime is not 0\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_ctime <= 0)
			{
			INFO_PRINTF1(_L("time of last shmctl() shm_ctime is not set\n"));
			return KErrGeneral ;
			}
		}	
	
	sleep(sleep_value);
	INFO_PRINTF1(_L("\nAttach shared memory segment to process\n"));
	if ((shmptr = (char*)shmat (shmid, 0, 0)) == (void *)-1)
		{
		INFO_PRINTF2(_L("shmat failed %d\n"), __LINE__);
	  	return KErrGeneral;
		}
 
	INFO_PRINTF1(_L("\n\tIndex through shared memory segment ...\n"));
	for (ptr=shmptr; ptr < (shmptr + shmem_size); ptr++)
		*ptr = value++;
    
	INFO_PRINTF1(_L("\n\tGeting shared memory infomation after shmat()\n"));
	if (shmctl (shmid, IPC_STAT,&shmDesc) < 0)
		{
		INFO_PRINTF2(_L("shctl failed %d\n"), __LINE__);
	  	return KErrGeneral;
		}
	else
		{
		INFO_PRINTF2(_L("\nThe USER ID = %d\n"), shmDesc.shm_perm.uid); 
		INFO_PRINTF2(_L("The GROUP ID = %d\n"), shmDesc.shm_perm.gid); 
		INFO_PRINTF2(_L("The creator's ID = %d\n"), shmDesc.shm_perm.cuid); 
		INFO_PRINTF2(_L("The creator's group ID = %d\n"), shmDesc.shm_perm.cgid);
		INFO_PRINTF2(_L("The operation permissions = 0%o\n"), shmDesc.shm_perm.mode); 
		INFO_PRINTF1(_L("The slot usage sequence\n")); 
		INFO_PRINTF2(_L("number = %d\n"),  shmDesc.shm_perm.seq); 
		INFO_PRINTF2(_L("The key= %d\n"), shmDesc.shm_perm.key); 
		INFO_PRINTF2(_L("The segment size = %d\n"), shmDesc.shm_segsz); 
		INFO_PRINTF2(_L("The pid of last shmop = %d\n"), shmDesc.shm_lpid); 
		INFO_PRINTF2(_L("The pid of creator = %d\n"), shmDesc.shm_cpid); 
		INFO_PRINTF2(_L("The current # attached = %d\n"), shmDesc.shm_nattch); 
		INFO_PRINTF2(_L("The last shmat time = %d\n"), shmDesc.shm_atime); 
		INFO_PRINTF2(_L("The last shmdt time = %d\n"),  shmDesc.shm_dtime); 
		INFO_PRINTF2(_L("The last change time = %d\n"),  shmDesc.shm_ctime);
		if (shmDesc.shm_perm.uid != pid)
			{
			INFO_PRINTF1(_L("Owner user id shm_perm.uid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_perm.gid != 0)
			{
			INFO_PRINTF1(_L("Owner group id shm_perm.gid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_perm.cuid != pid)
			{
			INFO_PRINTF1(_L("Creator user id shm_perm.cuid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_perm.cgid != 0)
			{
			INFO_PRINTF1(_L("Creator group id shm_perm.cgid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_lpid != pid)
			{
			INFO_PRINTF1(_L("Process ID of last shared memory operation shm_lpid is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_cpid != pid)
			{
			INFO_PRINTF1(_L("Process ID of creator shm_cpid is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_nattch != 1)
			{
			INFO_PRINTF1(_L("Number of current attaches shm_nattch is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_atime <= shmDesc.shm_ctime)
			{
			INFO_PRINTF1(_L("time of last shmat() shm_atime is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_dtime != 0)
			{
			INFO_PRINTF1(_L("time of last shmdt() shm_dtime is not 0\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_ctime <= 0)
			{
			INFO_PRINTF1(_L("time of last shmctl() shm_ctime is not set\n"));
			return KErrGeneral ;
			}
		}
	sleep(sleep_value);
	INFO_PRINTF1(_L("\n\tRetrieving shared memory infomation after shmat() for IPC_SET\n"));
	if (shmctl (shmid, IPC_SET,&shmDesc) < 0)
		{
		INFO_PRINTF2(_L("shmctl failed %d\n"), __LINE__);
	  	return KErrGeneral;
		}
	if (shmctl (shmid, IPC_STAT,&shmDesc) < 0)
		{
		INFO_PRINTF2(_L("shmctl failed %d\n"), __LINE__);
	  	return KErrGeneral;
		}
	else
		{
		INFO_PRINTF2(_L("\nThe USER ID = %d\n"), shmDesc.shm_perm.uid); 
		INFO_PRINTF2(_L("The GROUP ID = %d\n"), shmDesc.shm_perm.gid); 
		INFO_PRINTF2(_L("The creator's ID = %d\n"), shmDesc.shm_perm.cuid); 
		INFO_PRINTF2(_L("The creator's group ID = %d\n"), shmDesc.shm_perm.cgid);
		INFO_PRINTF2(_L("The operation permissions = 0%o\n"), shmDesc.shm_perm.mode); 
		INFO_PRINTF1(_L("The slot usage sequence\n")); 
		INFO_PRINTF2(_L("number = %d\n"),  shmDesc.shm_perm.seq); 
		INFO_PRINTF2(_L("The key= %d\n"), shmDesc.shm_perm.key); 
		INFO_PRINTF2(_L("The segment size = %d\n"), shmDesc.shm_segsz); 
		INFO_PRINTF2(_L("The pid of last shmop = %d\n"), shmDesc.shm_lpid); 
		INFO_PRINTF2(_L("The pid of creator = %d\n"), shmDesc.shm_cpid); 
		INFO_PRINTF2(_L("The current # attached = %d\n"), shmDesc.shm_nattch); 
		INFO_PRINTF2(_L("The last shmat time = %d\n"), shmDesc.shm_atime); 
		INFO_PRINTF2(_L("The last shmdt time = %d\n"),  shmDesc.shm_dtime); 
		INFO_PRINTF2(_L("The last change time = %d\n"),  shmDesc.shm_ctime);
		if (shmDesc.shm_perm.uid != pid)
			{
			INFO_PRINTF1(_L("Owner user id shm_perm.uid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_perm.gid != 0)
			{
			INFO_PRINTF1(_L("Owner group id shm_perm.gid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_perm.cuid != pid)
			{
			INFO_PRINTF1(_L("Creator user id shm_perm.cuid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_perm.cgid != 0)
			{
			INFO_PRINTF1(_L("Creator group id shm_perm.cgid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_lpid != pid)
			{
			INFO_PRINTF1(_L("Process ID of last shared memory operation shm_lpid is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_cpid != pid)
			{
			INFO_PRINTF1(_L("Process ID of creator shm_cpid is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_nattch != 1)
			{
			INFO_PRINTF1(_L("Number of current attaches shm_nattch is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_atime <= 0)
			{
			INFO_PRINTF1(_L("time of last shmat() shm_atime is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_dtime != 0)
			{
			INFO_PRINTF1(_L("time of last shmdt() shm_dtime is not 0\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_ctime <= shmDesc.shm_atime)
			{
			INFO_PRINTF1(_L("time of last shmctl() shm_ctime is not set\n"));
			return KErrGeneral ;
			}
		}	
	sleep(sleep_value);
	INFO_PRINTF1(_L("\nDetach shared memory segment to process\n"));
	if ( shmdt (shmptr) == -1)
		{
		INFO_PRINTF2(_L("shmdt failed %d\n"), __LINE__);
	  	return KErrGeneral;
		}
	INFO_PRINTF1(_L("\nRetrieving shared memory infomation after shmdt()\n"));
	if (shmctl (shmid, IPC_STAT,&shmDesc) < 0)
		{
		INFO_PRINTF2(_L("shmctl failed %d\n"), __LINE__);
	  	return KErrGeneral;
		}
	else
		{
		INFO_PRINTF2(_L("\nThe USER ID = %d\n"), shmDesc.shm_perm.uid); 
		INFO_PRINTF2(_L("The GROUP ID = %d\n"), shmDesc.shm_perm.gid); 
		INFO_PRINTF2(_L("The creator's ID = %d\n"), shmDesc.shm_perm.cuid); 
		INFO_PRINTF2(_L("The creator's group ID = %d\n"), shmDesc.shm_perm.cgid);
		INFO_PRINTF2(_L("The operation permissions = 0%o\n"), shmDesc.shm_perm.mode); 
		INFO_PRINTF1(_L("The slot usage sequence\n")); 
		INFO_PRINTF2(_L("number = %d\n"),  shmDesc.shm_perm.seq); 
		INFO_PRINTF2(_L("The key= %d\n"), shmDesc.shm_perm.key); 
		INFO_PRINTF2(_L("The segment size = %d\n"), shmDesc.shm_segsz); 
		INFO_PRINTF2(_L("The pid of last shmop = %d\n"), shmDesc.shm_lpid); 
		INFO_PRINTF2(_L("The pid of creator = %d\n"), shmDesc.shm_cpid); 
		INFO_PRINTF2(_L("The current # attached = %d\n"), shmDesc.shm_nattch); 
		INFO_PRINTF2(_L("The last shmat time = %d\n"), shmDesc.shm_atime); 
		INFO_PRINTF2(_L("The last shmdt time = %d\n"),  shmDesc.shm_dtime); 
		INFO_PRINTF2(_L("The last change time = %d\n"),  shmDesc.shm_ctime);
		if (shmDesc.shm_perm.uid != pid)
			{
			INFO_PRINTF1(_L("Owner user id shm_perm.uid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_perm.gid != 0)
			{
			INFO_PRINTF1(_L("Owner group id shm_perm.gid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_perm.cuid != pid)
			{
			INFO_PRINTF1(_L("Creator user id shm_perm.cuid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_perm.cgid != 0)
			{
			INFO_PRINTF1(_L("Creator group id shm_perm.cgid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_lpid != pid)
			{
			INFO_PRINTF1(_L("Process ID of last shared memory operation shm_lpid is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_cpid != pid)
			{
			INFO_PRINTF1(_L("Process ID of creator shm_cpid is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_nattch != 0)
			{
			INFO_PRINTF1(_L("Number of current attaches shm_nattch is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_atime >= shmDesc.shm_dtime)
			{
			INFO_PRINTF1(_L("time of last shmat() shm_atime is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_dtime <= shmDesc.shm_ctime)
			{
			INFO_PRINTF1(_L("time of last shmdt() shm_dtime is not 0\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_ctime <= shmDesc.shm_atime)
			{
			INFO_PRINTF1(_L("time of last shmctl() shm_ctime is not set\n"));
			return KErrGeneral ;
			}
		}		
	sleep(sleep_value);
	INFO_PRINTF1(_L("\n\tGeting shared memory infomation after shdt() for IPC_SET\n"));
	if (shmctl (shmid, IPC_SET,&shmDesc) < 0)
		{
		INFO_PRINTF2(_L("shmctl failed %d\n"), __LINE__);
	  	return KErrGeneral;
		}
	if (shmctl (shmid, IPC_STAT,&shmDesc) < 0)
		{
		INFO_PRINTF2(_L("shmctl failed %d\n"), __LINE__);
	  	return KErrGeneral;
		}
	else
		{
		INFO_PRINTF2(_L("\nThe USER ID = %d\n"), shmDesc.shm_perm.uid); 
		INFO_PRINTF2(_L("The GROUP ID = %d\n"), shmDesc.shm_perm.gid); 
		INFO_PRINTF2(_L("The creator's ID = %d\n"), shmDesc.shm_perm.cuid); 
		INFO_PRINTF2(_L("The creator's group ID = %d\n"), shmDesc.shm_perm.cgid);
		INFO_PRINTF2(_L("The operation permissions = 0%o\n"), shmDesc.shm_perm.mode); 
		INFO_PRINTF1(_L("The slot usage sequence\n")); 
		INFO_PRINTF2(_L("number = %d\n"),  shmDesc.shm_perm.seq); 
		INFO_PRINTF2(_L("The key= %d\n"), shmDesc.shm_perm.key); 
		INFO_PRINTF2(_L("The segment size = %d\n"), shmDesc.shm_segsz); 
		INFO_PRINTF2(_L("The pid of last shmop = %d\n"), shmDesc.shm_lpid); 
		INFO_PRINTF2(_L("The pid of creator = %d\n"), shmDesc.shm_cpid); 
		INFO_PRINTF2(_L("The current # attached = %d\n"), shmDesc.shm_nattch); 
		INFO_PRINTF2(_L("The last shmat time = %d\n"), shmDesc.shm_atime); 
		INFO_PRINTF2(_L("The last shmdt time = %d\n"),  shmDesc.shm_dtime); 
		INFO_PRINTF2(_L("The last change time = %d\n"),  shmDesc.shm_ctime);
		if (shmDesc.shm_perm.uid != pid)
			{
			INFO_PRINTF1(_L("Owner user id shm_perm.uid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_perm.gid != 0)
			{
			INFO_PRINTF1(_L("Owner group id shm_perm.gid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_perm.cuid != pid)
			{
			INFO_PRINTF1(_L("Creator user id shm_perm.cuid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_perm.cgid != 0)
			{
			INFO_PRINTF1(_L("Creator group id shm_perm.cgid not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_lpid != pid)
			{
			INFO_PRINTF1(_L("Process ID of last shared memory operation shm_lpid is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_cpid != pid)
			{
			INFO_PRINTF1(_L("Process ID of creator shm_cpid is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_nattch != 0)
			{
			INFO_PRINTF1(_L("Number of current attaches shm_nattch is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_atime >= shmDesc.shm_dtime)
			{
			INFO_PRINTF1(_L("time of last shmat() shm_atime is not set\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_dtime >= shmDesc.shm_ctime)
			{
			INFO_PRINTF1(_L("time of last shmdt() shm_dtime is not 0\n"));
			return KErrGeneral ;
			}
		if (shmDesc.shm_ctime <= shmDesc.shm_atime)
			{
			INFO_PRINTF1(_L("time of last shmctl() shm_ctime is not set\n"));
			return KErrGeneral ;
			}
		}	
	INFO_PRINTF1(_L("\nRelease shared memory\n"));
	if (shmctl (shmid, IPC_RMID, 0) < 0)
		{
		INFO_PRINTF2(_L("shmctl failed %d\n"), __LINE__);
	  	return KErrGeneral;
		}	
	INFO_PRINTF1(_L("\nsuccessful!\n"));
    return 0;
	}
