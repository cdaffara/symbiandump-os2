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

#include "tsemaphore.h"
#include <sys/stat.h> 
#include <sys/wait.h> 
#include <fcntl.h>
#include <stdio.h>

#define MAXVAL 3
#define UNIX_BASE   TTime(MAKE_TINT64(0x00dcddb3,0x0f2f8000))
#define MESSAGE_Q_KEY 1000
#define WITH_ERROR 1
#define WITHOUT_ERROR 0
#define IPC_KEY 1000
#define IPC_CREAT_EXCL 3000
#define IPC_NO_CREAT_EXCL 2000
#define INVALID_SEM_COMMAND 1024
#define TH_CASE_SEMOP_NEGATIVE_VAL 1024
#define TH_CASE_SEMOP_ZERO_VAL 1025

#define MAX_SEM_OPS 3
#define RETURN_ON_ERROR(err) if ( err ) \
            { \
            ERR_PRINTF2(KFunc, err); \
            return err; \
            }
TInt iFlag;  
          
TInt CTestSemaphore::SemKey(  TInt& aKey )
    {
    _LIT(KFunc, "SemKey");
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

TInt CTestSemaphore::SemNum(  TInt& aSemNum )
    {
    _LIT(KFunc, "SemNum");
    INFO_PRINTF1 ( KFunc);
    _LIT( KaSemNum, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(KaSemNum,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aSemNum);
	 
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore number") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    
    return KErrNone;
    }
    
TInt CTestSemaphore::SemFlag(  TInt& aFlag )
    {
    _LIT(KFunc, "SemFlag");
    INFO_PRINTF1 ( KFunc);
    _LIT( KaFlag, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(KaFlag,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aFlag);
	 
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore flag") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    
    
    return KErrNone;
    }

TInt CTestSemaphore::SemOpVal(  TInt& aSemOp )
    {
    _LIT(KFunc, "SemOpVal");
    INFO_PRINTF1 ( KFunc);
    
    _LIT( KaSemOp, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(KaSemOp,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aSemOp);
	
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore operation value");
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral ;
		}
    return KErrNone;
    }

TInt CTestSemaphore::ErrCode(  TInt& aErrCode )
    {
    _LIT(KFunc, "ErrCode");
    INFO_PRINTF1 ( KFunc());
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


// -----------------------------------------------------------------------------
// CTestSemaphore::SemNCreate
// -----------------------------------------------------------------------------
//
TInt CTestSemaphore::SemNCreate(  )
    {
    _LIT(KFunc, "SemCreate");
    INFO_PRINTF1 ( KFunc);
    
    TInt key;
    TInt semFlag;
    TInt terror;
    TInt retVal = KErrNone;
    TInt numberOfSem = 0;
    // retrieve passed parameters
    retVal = SemKey( key);
    RETURN_ON_ERROR(retVal);

    retVal = SemNum( iSemNum);
    RETURN_ON_ERROR(retVal);

    retVal = SemFlag( semFlag);
    RETURN_ON_ERROR(retVal);

    retVal = ErrCode( terror);
    RETURN_ON_ERROR(retVal);

    retVal = SemNum( numberOfSem);
    RETURN_ON_ERROR(retVal);
    
    TInt semid = 0;

    int startIndex = 0;
    TInt index = 0;
    for(index = 0; index < MAX_IPC_OBJ; index++)
        {
        if(iNumberOfCreateSem[index] == -1)
            {
            startIndex = index;
            break;
            }
        }


    for(index = 0; index < numberOfSem; index++)
        {
        semid = semget(key +index, iSemNum, semFlag);
        if(semid == -1) 
            {
            break;
            }
        iNumberOfCreateSem[startIndex + index] = semid;            
        }

    if (semid == -1)
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
            
            if ( errCode == errno)
                {
                retVal = KErrNone;
                }
            }
        }
    else
        {
        if(terror == WITH_ERROR)
            {
            retVal = errno;
            }
        }

    if(retVal != KErrNone && index < numberOfSem && numberOfSem > 1)
        {
        // do the cleanup
        for(index = startIndex; index < MAX_IPC_OBJ; index++)
            {
            if(iNumberOfCreateSem[index] > 0)
                semctl(iNumberOfCreateSem[index], iSemNum, IPC_RMID);
            }
        }

    //DebugLogPrintL ( KFunc(), OUT, retVal);
    return retVal;
    }

// -----------------------------------------------------------------------------
// CTestSemaphore::SemNClose
// -----------------------------------------------------------------------------
//
TInt CTestSemaphore::SemNClose(  )
    {
    _LIT(KFunc, "SemClose");
    INFO_PRINTF1 ( KFunc);

    TInt retVal = KErrNone;
    TInt terror;
    
    retVal = ErrCode( terror);
    RETURN_ON_ERROR(retVal);
    TInt err = KErrNone;
    for(TInt index = 0; index < MAX_IPC_OBJ; index++)
        {
        if(iNumberOfCreateSem[index] > 0)
            err = semctl(iNumberOfCreateSem[index], iSemNum, IPC_RMID);
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
        if(terror == WITH_ERROR)
            {
            retVal = errno;
            }
        }
	return retVal;
    }
  
 // -----------------------------------------------------------------------------
// CTestSemaphore::SemCreate
// -----------------------------------------------------------------------------
//
TInt CTestSemaphore::SemCreate(  )
    {
    _LIT(KFunc, "SemCreate");
    INFO_PRINTF1 ( KFunc);
    
    TInt key;
    TInt semFlag;
    TInt terror;
    TInt retVal = KErrNone;
    
    // retrieve passed parameters
    retVal = SemKey( key);
    RETURN_ON_ERROR(retVal);

    retVal = SemNum( iSemNum);
    RETURN_ON_ERROR(retVal);

    retVal = SemFlag( semFlag);
    RETURN_ON_ERROR(retVal);

    retVal = ErrCode( terror);
    RETURN_ON_ERROR(retVal);
    
    TInt semid = semget(key, iSemNum, semFlag);
    
    if (semid == -1)
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
            
            if ( errCode == errno)
                {
                retVal = KErrNone;
                }
            }
        }
    else
        {
        iNumberOfCreateSem[0] = semid;
        iKey = key;
        if(terror == WITH_ERROR)
            {
            retVal = errno;
            }
        }
	return retVal;
    }

// -----------------------------------------------------------------------------
// CTestSemaphore::SemOp
// -----------------------------------------------------------------------------
//
TInt CTestSemaphore::SemOp(  )
    {
    _LIT(KFunc, "SemOp");
    INFO_PRINTF1 ( KFunc);
    
    TInt key = iNumberOfCreateSem[0];
    TInt semnops;
    TInt terror;
    TInt retVal = KErrNone;
    
    // retrieve passed parameters
    if(iReadSemIdFlag)
        {
        retVal = SemKey( key);
        RETURN_ON_ERROR(retVal);
        }
    // nsops
    _LIT( Ksemnops, "Param%d" );
	TBuf<8> pNameBuf;
	pNameBuf.Format(Ksemnops,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, semnops);
	
	 if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semnops") ;
	 	ERR_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    
    
    if(semnops > MAX_SEM_OPS)
        {
        return KErrGeneral;
        }
        
    struct sembuf lSemBuf[MAX_SEM_OPS];
    
    for(TInt index = 0; index < semnops; index++)
        {
        TInt sem_num;
        TInt sem_op;
        TInt sem_flg;
		_LIT( Ksem_num, "Param%d" );
		TBuf<8> pNameBuf;
		pNameBuf.Format(Ksem_num,++iParamCnt);
		TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, sem_num);
		
		if(!res)
			{
		 	_LIT(Kerr , "Unable to retrieve sem_num") ;
		 	ERR_PRINTF1(Kerr) ;
		 	return KErrGeneral ;
			}
    	
    	_LIT( Ksem_op, "Param%d" );
		pNameBuf.Zero();
		pNameBuf.Format(Ksem_op,++iParamCnt);
		res = GetIntFromConfig(ConfigSection(), pNameBuf, sem_op);
		
		if(!res)
			{
		 	_LIT(Kerr , "Unable to retrieve sem_op") ;
		 	ERR_PRINTF1(Kerr) ;
		 	return KErrGeneral ;
			}
        
        _LIT( Ksem_flg, "Param%d" );
		pNameBuf.Zero();
		pNameBuf.Format(Ksem_flg,++iParamCnt);
		res = GetIntFromConfig(ConfigSection(), pNameBuf, sem_flg);
		
		if(!res)
			{
		 	_LIT(Kerr , "Unable to retrieve sem_flg") ;
		 	ERR_PRINTF1(Kerr) ;
		 	return KErrGeneral ;
			}
        
        
        lSemBuf[index].sem_num = sem_num;
        lSemBuf[index].sem_op = sem_op;
        lSemBuf[index].sem_flg = sem_flg;
        }
        
    retVal = ErrCode( terror);
    RETURN_ON_ERROR(retVal);
    iSemopStartFlag = ETrue;
    TInt err = semop(key, &lSemBuf[0], semnops);
    
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
            if ( errCode == errno)
                {
                retVal = KErrNone;
                }
            }
        }
    else
        {
        if(terror == WITH_ERROR)
            {
            retVal = errno;
            }
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CTestSemaphore::SemCtl
// -----------------------------------------------------------------------------
//
TInt CTestSemaphore::SemCtl(  )
    {
    _LIT(KFunc, "SemCtl");
    INFO_PRINTF1 ( KFunc);


    TInt key = iNumberOfCreateSem[0];
    TInt semCmd,err1;
    TInt terror;
    TInt retVal = KErrNone;
    TInt semNum;
    // retrieve passed parameters
    if(iReadSemIdFlag)
        {
        retVal = SemKey( key);
        RETURN_ON_ERROR(retVal);
        }

    retVal = SemNum( semNum);
    RETURN_ON_ERROR(retVal);
    
    // semaphore command
    retVal = SemNum( semCmd);
    RETURN_ON_ERROR(retVal);

    retVal = ErrCode( terror);
    RETURN_ON_ERROR(retVal);
    
	union semun psemun;
	psemun.array = NULL;
	psemun.buf = NULL;
	TInt expectedReturn = 0;
    TInt semNumbers = 0;
    TInt seq = 0;
    TBool freeReq = EFalse;
    TBuf<8> pNameBuf;
    TBool res ;
    _LIT( KexpectedReturn, "Param%d" );
    switch(semCmd)
        {
        case IPC_RMID:
        	
	  		pNameBuf.Format(KexpectedReturn,++iParamCnt);
	 		res = GetIntFromConfig(ConfigSection(), pNameBuf, expectedReturn);
			if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve expected return value") ;
			 	ERR_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            
            break;

        case GETNCNT:
        case GETZCNT:
        case GETVAL:
        case INVALID_SEM_COMMAND:
        	
	  		pNameBuf.Format(KexpectedReturn,++iParamCnt);
	 		res = GetIntFromConfig(ConfigSection(), pNameBuf, expectedReturn);
			if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve expected return value") ;
			 	ERR_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            break;
        case SETVAL:
            {
            
	  		pNameBuf.Format(KexpectedReturn,++iParamCnt);
	 		TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expectedReturn);
			if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve expected return value") ;
			 	ERR_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            
            INFO_PRINTF1 ( _L("SETVAL"));
            
            _LIT( Kval, "Param%d" );
	 		pNameBuf.Format(Kval,++iParamCnt);
	 		res = GetIntFromConfig(ConfigSection(), pNameBuf, psemun.val);
			if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve semun val") ;
			 	ERR_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            
            break;
            }
        case GETPID:
            {
            RThread thread;
            expectedReturn = thread.Id();
            break;
            }
        case GETALL:
            {
            pNameBuf.Format(KexpectedReturn,++iParamCnt);
	 		TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expectedReturn);
			if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve expected return value") ;
			 	ERR_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
			_LIT( KsemNumbers, "Param%d" );
	 		pNameBuf.Zero();
     		pNameBuf.Format(KsemNumbers,++iParamCnt);
	 		res = GetIntFromConfig(ConfigSection(), pNameBuf, semNumbers);
			if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve sem numbers") ;
			 	ERR_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}	 
            
            psemun.array = (unsigned short*)malloc(semNumbers * sizeof(unsigned short));
            freeReq = ETrue;
            }
            break;
        case SETALL:
            {
            pNameBuf.Format(KexpectedReturn,++iParamCnt);
	 		TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expectedReturn);
			if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve expected return value") ;
			 	ERR_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            _LIT( KsemNumbers, "Param%d" );
	 		pNameBuf.Zero();
     		pNameBuf.Format(KsemNumbers,++iParamCnt);
	 		res = GetIntFromConfig(ConfigSection(), pNameBuf, semNumbers);
			if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve sem numbers") ;
			 	ERR_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}	 
            
            psemun.array = (unsigned short*)malloc(semNumbers * sizeof(unsigned short));
            freeReq = ETrue;
            for(TInt index = 0; index < semNumbers; index++)
                {
                TInt val = 0;
                _LIT( Kval, "Param%d" );
		 		TBuf<8> pNameBuf;
	     		pNameBuf.Format(Kval,++iParamCnt);
		 		TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, val);
				if(!res)
					{
				 	_LIT(Kerr , "Unable to retrieve sem val") ;
				 	ERR_PRINTF1(Kerr) ;
				 	return KErrGeneral ;
					}	 
                psemun.array[index] = val;
                }
            }
            break;
        case IPC_STAT:
            {
            
     		pNameBuf.Format(KexpectedReturn,++iParamCnt);
	 		TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expectedReturn);
			if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve expected return value") ;
			 	ERR_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            psemun.buf = (struct semid_ds*)malloc(sizeof(struct semid_ds));
            freeReq = ETrue;
            }
            break;
        case IPC_SET:
            {
            pNameBuf.Format(KexpectedReturn,++iParamCnt);
	 		TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expectedReturn);
			if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve expected return value") ;
			 	ERR_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            psemun.buf = (struct semid_ds*)malloc(sizeof(struct semid_ds));
            freeReq = ETrue;
            // fetch the data.
            TInt err = semctl(key, semNum, IPC_STAT, psemun);
            RETURN_ON_ERROR(err);
            _LIT( Kseq, "Param%d" );
	 		pNameBuf.Zero();
     		pNameBuf.Format(Kseq,++iParamCnt);
	 		res = GetIntFromConfig(ConfigSection(), pNameBuf, seq);
			if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve expected seq value") ;
			 	ERR_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
            ((struct semid_ds*)psemun.buf)->sem_perm.mode = seq;
            }
            break;
        
        default:
            break;
        }
        
    err1 = semctl(key, semNum, semCmd, psemun);
    
    if(expectedReturn != err1)
        {
        return KErrGeneral;
        }    
        
    if (freeReq)
        {
        if (semCmd == IPC_SET)
            {
            TInt err = semctl(key, semNum, IPC_STAT, psemun);
            RETURN_ON_ERROR(err);
            if(((struct semid_ds*)psemun.buf)->sem_perm.mode != seq)
                {
                return KErrGeneral;
                }
            }
        else if (semCmd == GETALL)
            {
            for(TInt index = 0; semCmd == GETALL && index < semNumbers; index++)
                {
                TInt val = 0;
                _LIT( Kval, "Param%d" );
	     		pNameBuf.Format(Kval,++iParamCnt);
		 		TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, val);
				if(!res)
					{
				 	_LIT(Kerr , "Unable to retrieve sem val") ;
				 	ERR_PRINTF1(Kerr) ;
				 	return KErrGeneral ;
					}
                if (val != psemun.array[index])
                    {
                    return KErrGeneral;
                    }
                }
            }
        else
            {
            }
        //since array and buf is within union, there is no issue.
        free(psemun.array);
        psemun.array = NULL;
        }
        
    if (err1 == -1)
        {
        if(terror == WITHOUT_ERROR)
            {
            retVal = errno;
            }
        else
            {
            TInt errCode = 0;
            _LIT( KerrCode, "Param%d" );
    		pNameBuf.Format(KerrCode,++iParamCnt);
	 		TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, errCode);
			
			if(!res)
				{
			 	_LIT(Kerr , "Unable to retrieve eror code") ;
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
        if(terror == WITH_ERROR)
            {
            retVal = errno;
            }
        }
        
	return retVal;
    }
    
// -----------------------------------------------------------------------------
// CTestSemaphore::SemClose
// -----------------------------------------------------------------------------
//
TInt CTestSemaphore::SemClose(  )
    {
    _LIT(KFunc, "SemClose");
    INFO_PRINTF1 ( KFunc);

    TInt retVal = KErrNone;
    TInt terror;
    
    retVal = ErrCode( terror);
    RETURN_ON_ERROR(retVal);
    
    TInt err = semctl(iNumberOfCreateSem[0], iSemNum, IPC_RMID);
    if (err == -1)
        {
        if(terror == WITHOUT_ERROR)
            {
            retVal = errno;
            }
        }
    else
        {
        if(terror == WITH_ERROR)
            {
            retVal = errno;
            }
        }
        
	return retVal;
    }


// -----------------------------------------------------------------------------
// CTestSemaphore::ReadSemId
// -----------------------------------------------------------------------------
//
TInt CTestSemaphore::ReadSemId(  )
    {
    _LIT(KFunc, "ReadSemId");
    INFO_PRINTF1 ( KFunc);
    _LIT( KiReadSemIdFlag, "Param%d" );
	TBuf<8> pNameBuf;
	pNameBuf.Format(KiReadSemIdFlag,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, iReadSemIdFlag);
	
	 if(!res)
	 {
	 	_LIT(Kerr , "Unable to retrieve SemIdFlag") ;
	 	INFO_PRINTF1(Kerr) ;
	 	iReadSemIdFlag = EFalse;
	 }
    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CTestSemaphore::CreateThreadL
// -----------------------------------------------------------------------------
//
TInt CTestSemaphore::CreateThreadL(  )
	{
    _LIT(KFunc, "CreateThreadL");
    INFO_PRINTF1 ( KFunc);
    TInt threadType, err;
    
    _LIT( KthreadType, "Param%d" );
	TBuf<8> pNameBuf;
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
        case GETNCNT: 
            err = pthread_create((unsigned int *)&iThreadId,(pthread_attr_t *)NULL,ThreadEntryFunctionSemOpGetNcnt,(void*)this);
            break;
        case GETZCNT: 
            err = pthread_create((unsigned int *)&iThreadId,(pthread_attr_t *)NULL,ThreadEntryFunctionSemOpGetZcnt,(void*)this);
            break;
        case TH_CASE_SEMOP_NEGATIVE_VAL:
            err = pthread_create((unsigned int *)&iThreadId,(pthread_attr_t *)NULL,ThreadEntryFunctionSemOpNegativeVal,(void*)this);
            break;
        case TH_CASE_SEMOP_ZERO_VAL:
            err = pthread_create((unsigned int *)&iThreadId,(pthread_attr_t *)NULL,ThreadEntryFunctionSemOpZeroVal,(void*)this);
            break;
        default:
            err = KErrNone;
            break;
        }

    if ( err )
        {
        ERR_PRINTF2 ( _L("pthread_create error"), err);
        }
    return err;
	}

void CTestSemaphore::SendSignal()
    {
    iExitVal = 1;
    }

TInt CTestSemaphore::WaitForThreadCompleteL(  )
    {
    _LIT(KFunc, "WaitForThreadCompleteL");
    INFO_PRINTF1 ( KFunc);
    iExitVal = 0;
    while (!iExitVal)
        {
        //spin over here.
        _LIT(Kerr , "Unable to retrieve eror code %d") ;
	 	INFO_PRINTF2(Kerr,iExitVal) ;
        User::After (100000);
        }
    iExitVal = 0;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestSemaphore::ThreadEntryFunctionSemOpGetNcntW
// -----------------------------------------------------------------------------
//    
void* CTestSemaphore::ThreadEntryFunctionSemOpGetNcnt(void* arg)
	{
    CTestSemaphore *self = static_cast<CTestSemaphore*> (arg);
    struct sembuf lSemBuf[MAX_SEM_OPS];
    TInt semid = semget(self->iKey, self->iSemNum, IPC_CREAT|IPC_W|IPC_R);
    if(semid != -1)    
        {
        // semaphore is created successfully.
        }
    lSemBuf[0].sem_num = 0;
    lSemBuf[0].sem_op = -1;
    lSemBuf[0].sem_flg = 0;
    // thread is supposed to block over here.
    TInt err = semop(semid, &lSemBuf[0], 1);
    if (err == -1)
        {
        }
    User::After(500000);
    self->SendSignal();
    return NULL;
	}    



// -----------------------------------------------------------------------------
// CTestSemaphore::ThreadEntryFunctionSemOpNegativeVal
// -----------------------------------------------------------------------------
//    
void* CTestSemaphore::ThreadEntryFunctionSemOpZeroVal(void* arg)
	{
    CTestSemaphore *self = static_cast<CTestSemaphore*> (arg);
    struct sembuf lSemBuf[MAX_SEM_OPS];
    TInt semid = semget(self->iKey, self->iSemNum, IPC_CREAT|IPC_W|IPC_R);
    if(semid != -1)    
        {
        // semaphore is created successfully.
        }
    lSemBuf[0].sem_num = 0;
    lSemBuf[0].sem_op = -1;
    lSemBuf[0].sem_flg = 0;
    while(!self->iSemopStartFlag)
		{
        User::After(50000);
		}
    // thread is supposed to block over here.
    TInt err = semop(semid, &lSemBuf[0], 1);
    if (err == -1)
        {
        }
    return NULL;
	}    

// -----------------------------------------------------------------------------
// CTestSemaphore::ThreadEntryFunctionSemOpNegativeVal
// -----------------------------------------------------------------------------
//    
void* CTestSemaphore::ThreadEntryFunctionSemOpNegativeVal(void* arg)
	{
    CTestSemaphore *self = static_cast<CTestSemaphore*> (arg);
    struct sembuf lSemBuf[MAX_SEM_OPS];
    TInt semid = semget(self->iKey, self->iSemNum, IPC_CREAT|IPC_W|IPC_R);
    if(semid != -1)    
        {
        // semaphore is created successfully.
        }
    lSemBuf[0].sem_num = 0;
    lSemBuf[0].sem_op = 1;
    lSemBuf[0].sem_flg = 0;
    while(!self->iSemopStartFlag)
		{
        User::After(50000);
		}
    // thread is supposed to block over here.
    TInt err = semop(semid, &lSemBuf[0], 1);
    if (err == -1)
        {
        }
    return NULL;
	}    

// -----------------------------------------------------------------------------
// CTestSemaphore::ThreadEntryFunctionSemOpGetZcnt
// -----------------------------------------------------------------------------
//    
void* CTestSemaphore::ThreadEntryFunctionSemOpGetZcnt(void* arg)
	{
    CTestSemaphore *self = static_cast<CTestSemaphore*> (arg);
    struct sembuf lSemBuf[MAX_SEM_OPS];
    TInt semid = semget(self->iKey, self->iSemNum, IPC_CREAT|IPC_W|IPC_R);
    if(semid != -1)    
        {
        // semaphore is created successfully.
        }
    lSemBuf[0].sem_num = 0;
    lSemBuf[0].sem_op = 0;
    lSemBuf[0].sem_flg = 0;
    // thread is supposed to block over here.
    TInt err = semop(semid, &lSemBuf[0], 1);
    if (err == -1)
        {
        }   
    User::After(500000);
    self->SendSignal();
    return NULL;
	}    

TInt CTestSemaphore::IntgTest1()
	{
	TInt err = SemCreate();
	if(!err)
		{
		err = SemClose();				
		}
	return err;
	}

TInt CTestSemaphore::IntgTest2()
	{
	TInt err = SemCreate();
	if(!err)
		{
		err = SemCreate();
		if(!err)
			{
			err = SemClose();
			}
		}
	return err;
	}

TInt CTestSemaphore::IntgTest3()
	{
	TInt err = ReadSemId();
	if(!err)
		{
		err = SemCtl(); 	
		}
	return err;	
	}


TInt CTestSemaphore::IntgTest4()
	{
	TInt err = SemCreate();
	if(!err)
		{
		err = ReadSemId();
		if(!err)
			{
			err = SemCtl();
			if(!err)
				{
				err = SemClose();
				}
			}
		}
	return err;
	}

TInt CTestSemaphore::IntgTest5()
	{
	TInt err = SemCreate();
	if(!err)
		{
		err = ReadSemId();
		if(!err)
			{
			err = SemCtl();
			if(!err) 
				{
				err = SemCtl();	
				if(!err)
					{
					err = SemClose();
					}
				}
			}
		}
	return err;
	}

TInt CTestSemaphore::IntgTest6()
	{
	TInt err = SemCreate();
	if(err)
		{
		return err;
		}
	err = ReadSemId();
	if(err)
		{
		return err;
		}
	err = SemOp();
	if(err)
		{
		return err;
		}
	err = CreateThreadL();

	if(err)
		{
		return err;
		}
	err = SemCtl();
	if(err)
		{
		return err;
		}
	err = SemOp();
	if(err)
		{
		return err;
		}
	err = WaitForThreadCompleteL();
	if(err)
		{
		return err;
		}
	err = SemClose();
	return err;
	}

TInt CTestSemaphore::IntgTest7()
	{
	TInt err = SemCreate();
	if(err)
		{
		return err;
		}
	err = ReadSemId();
	if(err)
		{
		return err;
		}
	err = SemOp();
	if(err)
		{
		return err;
		}
	err = SemCtl();
	if(err)
		{
		return err;
		}
	err = SemClose();
	return err;
	}

TInt CTestSemaphore::IntgTest8()
	{
	TInt err = SemCreate();
	if(!err)
		{
			err = ReadSemId();
			if(!err)
				{
				err = SemCtl();
				}	
		}
	return err;
	}

TInt CTestSemaphore::IntgTest9()
	{
	TInt err = SemCreate();
	if(err)
		{
		return err;
		}
	err = ReadSemId();
	if(err)
		{
		return err;
		}
	err = SemCtl();
	if(err)
		{
		return err;
		}
	err = SemOp();
	if(err)
		{
		return err;
		}
	err = SemCtl();
	return err;
	}

TInt CTestSemaphore::IntgTest10()
	{
	TInt err = SemCreate();
	if(err)
		{
		return err;
		}
	err = ReadSemId();
	if(err)
		{
		return err;
		}
	err = SemOp();
	if(err)
		{
		return err;
		}
	err = SemCtl();
	return err;
	}

TInt CTestSemaphore::IntgTest11()
	{
	TInt err = SemCreate();
	if(err)
		{
		return err;
		}
	err = ReadSemId();
	if(err)
		{
		return err;
		}
	err = CreateThreadL();
	if(err)
		{
		return err;
		}
	err = SemOp();
	if(err)
		{
		return err;
		}
	err = SemCtl();
	return err;
	}

TInt CTestSemaphore::IntgTest12()
{
	TInt err = SemCreate();
	if(err)
		{
		return err;
		}
	err = ReadSemId();
	if(err)
		{
		return err;
		}
	err = SemCtl();
	if(err)
		{
		return err;
		}
	err = CreateThreadL();
	if(err)
		{
		return err;
		}
	err = SemOp();
	if(err)
		{
		return err;
		}
	err = SemCtl();
	return err;
	}

TInt CTestSemaphore::IntgTest13()
	{
	TInt err = SemCreate();
	if(err)
		{
		return err;
		}
	err = SemNClose ();
	return err;
	}

TInt CTestSemaphore::IntgTest14()
	{
	 key_t key;
    int semid;
    union semun arg;
	INFO_PRINTF1(_L("IntgTest14"));
	char *path = NULL;
	path = "c:\\tstdapis\\tsemaphore.ini";

	if ((key = ftok(path, 'P')) == -1) 
    	{
        ERR_PRINTF1(_L("ftok error!"));
        _LIT(KFunc, "ftok failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
        return KErrGeneral;
    	}

    /* create a semaphore set with 1 semaphore: */
    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) 
    	{
        ERR_PRINTF1(_L("semget error!"));
        return KErrGeneral;
    	}

    /* initialize semaphore #0 to 1: */
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1)
     	{
        ERR_PRINTF1(_L("semctl error!"));
        return KErrGeneral;
    	}
    return KErrNone;
	}

TInt CTestSemaphore::ReadIntParam(TInt& aParam)
	{
    _LIT(KFunc, "ReadIntParam");
    INFO_PRINTF1 ( KFunc());
    _LIT( KaParam, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(KaParam,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aParam);
	 
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    return KErrNone;	
	}


//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemgetCreateKey()
// Description   : 	This test creates an array of specified number of semaphores 
//					with the specified key, and deletes them.
// Param1		 :  Key to use for semget
// Param2		 :  Number of semaphores in array
//-------------------------------------------------------------------------------	
TInt CTestSemaphore::SemgetCreateKey()
	{	
    int key = 0;
    int numberOfSem = 0;
    int sem_id = 0;
	TBuf<8> pNameBuf;
	
	_LIT( KaSemNum, "Param%d" );
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, key);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore key") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	
	res = GetIntFromConfig(ConfigSection(), pNameBuf, numberOfSem);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}

	if ((sem_id = semget(key, numberOfSem, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

	return KErrNone;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemgetCreatePrivate()
// Description   : 	This test creates a private array of specified number of semaphores 
//					and deletes them.
// Param1		 :  Number of semaphores in array
//-------------------------------------------------------------------------------	
TInt CTestSemaphore::SemgetCreatePrivate()
	{
    int numberOfSem = 0;
    int sem_id = 0;
	TBuf<8> pNameBuf;
	
	_LIT( KaSemNum, "Param%d" );
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, numberOfSem);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}

	if ((sem_id = semget(IPC_PRIVATE, numberOfSem, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

	return KErrNone;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemgetGetPrivate()
// Description   : 	This test creates a private array of specified number of 
//					semaphores and deletes them.
// Param1		 :  Number of semaphores in array
//-------------------------------------------------------------------------------	
TInt CTestSemaphore::SemgetGetPrivate()
	{
    int numberOfSem = 0;
    int sem_id = 0;
	TBuf<8> pNameBuf;
	
	_LIT( KaSemNum, "Param%d" );
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, numberOfSem);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}

	if ((sem_id = semget(IPC_PRIVATE, numberOfSem, 0)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

	return KErrNone;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemgetCreateKeyMax()
// Description   : 	This test tries to create upto specified number of arrays of 
//					specified number of semaphores with the specified key, 
//					and deletes them. Creation must fail before it reaches the 
//					specified count, and ENOSPC must be returned on failure.
// Param1		 :  Starting value of key to use for semget
// Param2		 :  Number of semaphores in array
// Param3		 :  Number of arrays to create
//-------------------------------------------------------------------------------	
TInt CTestSemaphore::SemgetCreateKeyMax()
	{	
    int key = 0;
    int numberOfSem = 0;
	int arrayCount = 0;
	TBuf<8> pNameBuf;
	int	rval = KErrNone;
	
	_LIT( KaSemNum, "Param%d" );
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, key);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore key") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	res = GetIntFromConfig(ConfigSection(), pNameBuf, numberOfSem);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
    pNameBuf.Format(KaSemNum,++iParamCnt);
	res = GetIntFromConfig(ConfigSection(), pNameBuf, arrayCount);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore array count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
	int idx = 0;
	int* sem_id = 0;
	int limitHit = 0;
	int createdCount = arrayCount;
	
	sem_id = (int*)malloc(arrayCount * sizeof(int));
	if(sem_id == 0)
		{
		_LIT(Kerr , "Out of memory") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrNoMemory ;
		}
		
	for(idx=0;idx<arrayCount;idx++)
		{
		if ((sem_id[idx] = semget(key+idx, numberOfSem, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
			{
			_LIT(KFunc, "semaphore create failed with errno %d\n");
			INFO_PRINTF2(KFunc, errno);
			if(errno == ENOSPC)
				{
				limitHit = 1;
				}
			createdCount = idx;
			break;
			}
		}
	if(limitHit == 0)
		rval = KErrGeneral;
		
	for(idx=0;idx<createdCount;idx++)
		{
		if (semctl(sem_id[idx], 0, IPC_RMID) == -1) 
			{
			_LIT(KFunc, "semaphore delete failed with errno %d\n");
			INFO_PRINTF2(KFunc, errno);
			rval = KErrGeneral;
			}
		}
		
	free(sem_id);
	sem_id = 0;
	
	return rval;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemgetCreatePvtMax()
// Description   : 	This test tries to create upto specified number of private 
//					arrays of specified number of semaphores, and deletes them. 
//					Creation must fail before it reaches the specified count, 
//					and ENOSPC must be returned on failure.
// Param1		 :  Number of semaphores in array
// Param2		 :  Number of arrays to create
//-------------------------------------------------------------------------------	
TInt CTestSemaphore::SemgetCreatePvtMax()
	{
    int numberOfSem = 0;
	int arrayCount = 0;
	TBuf<8> pNameBuf;
	int	rval = KErrNone;
	
	_LIT( KaSemNum, "Param%d" );
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, numberOfSem);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
    pNameBuf.Format(KaSemNum,++iParamCnt);
	res = GetIntFromConfig(ConfigSection(), pNameBuf, arrayCount);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore array count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
	int idx = 0;
	int* sem_id = 0;
	int limitHit = 0;
	int createdCount = arrayCount;
	
	sem_id = (int*)malloc(arrayCount * sizeof(int));
	if(sem_id == 0)
		{
		_LIT(Kerr , "Out of memory") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrNoMemory ;
		}
		
	for(idx=0;idx<arrayCount;idx++)
		{
		if ((sem_id[idx] = semget(IPC_PRIVATE, numberOfSem, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
			{
			_LIT(KFunc, "semaphore create failed with errno %d\n");
			INFO_PRINTF2(KFunc, errno);
			if(errno == ENOSPC)
				{
				limitHit = 1;
				}
			createdCount = idx;
			break;
			}
		}
	if(limitHit == 0)
		rval = KErrGeneral;
		
	for(idx=0;idx<createdCount;idx++)
		{
		if (semctl(sem_id[idx], 0, IPC_RMID) == -1) 
			{
			_LIT(KFunc, "semaphore delete failed with errno %d\n");
			INFO_PRINTF2(KFunc, errno);
			rval = KErrGeneral;
			}
		}
		
	free(sem_id);
	sem_id = 0;
	
	return rval;
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemgetGetNonExisting()
// Description   : 	This test tries to get a non-existing semaphore with 
//					specified key and size.
// Param1		 :  Key to use for semget
// Param2		 :  Number of semaphores in array
//-------------------------------------------------------------------------------	
TInt CTestSemaphore::SemgetGetNonExisting()
	{
    int key = 0;
    int numberOfSem = 0;
	TBuf<8> pNameBuf;
	
	_LIT( KaSemNum, "Param%d" );
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, key);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore key") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	res = GetIntFromConfig(ConfigSection(), pNameBuf, numberOfSem);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}

	int sem_id;
	
	sem_id = semget(key ,numberOfSem, IPC_CREAT & 0);
	
	if(sem_id == -1)
		{
		if(errno == ENOENT)
			{
			_LIT(KFunc, "semget failed with the proper errno\n");
	      	INFO_PRINTF1(KFunc);
	       	return KErrNone;
			}
		else
			{
			_LIT(KFunc, "semget failed with wrong errno\n");
	      	INFO_PRINTF1(KFunc);
	       	return KErrGeneral;
			}
		}
	else
		{
		_LIT(KErr,"semget did not fail. key exists!\n");
		INFO_PRINTF1(KErr);
		return KErrGeneral;
		}
	}

//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemgetCreateInvalidSizeK()
// Description   : 	This test tries to create a semaphore with 
//					specified key and an invalid size.
// Param1		 :  Key to use for semget
// Param2		 :  Number of semaphores in array (invalid value)
//-------------------------------------------------------------------------------	
TInt CTestSemaphore::SemgetCreateInvalidSizeK()
	{
    int key = 0;
    int numberOfSem = 0;
	TBuf<8> pNameBuf;
	
	_LIT( KaSemNum, "Param%d" );
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, key);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore key") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	res = GetIntFromConfig(ConfigSection(), pNameBuf, numberOfSem);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
	int sem_id;
	sem_id = semget(key ,numberOfSem, IPC_CREAT | IPC_EXCL | 0666);
	if (sem_id == -1) 
		{
		if(errno == EINVAL)
			{
			_LIT(KFunc, "semaphore set the proper errno\n");
      		INFO_PRINTF1(KFunc);
       		return KErrNone;
			}
		_LIT(KFunc, "semaphore failed to set the errno but returned -1 \n");
      	INFO_PRINTF1(KFunc);
       	return KErrGeneral;
    	}
    
	return KErrGeneral;
	}
	

//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemgetCreateInvalidSizeP()
// Description   : 	This test tries to create a private semaphore array with 
//					an invalid size.
// Param1		 :  Number of semaphores in array (invalid value)
//-------------------------------------------------------------------------------	
TInt CTestSemaphore::SemgetCreateInvalidSizeP()
	{
    int numberOfSem = 0;
	TBuf<8> pNameBuf;
	
	_LIT( KaSemNum, "Param%d" );
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, numberOfSem);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
	int sem_id;
	sem_id = semget(IPC_PRIVATE ,numberOfSem, IPC_CREAT | IPC_EXCL | 0666);
	if (sem_id == -1) 
		{
		if(errno == EINVAL)
			{
			_LIT(KFunc, "semaphore set the proper errno\n");
      		INFO_PRINTF1(KFunc);
       		return KErrNone;
			}
		_LIT(KFunc, "semaphore failed to set the errno but returned -1 \n");
      	INFO_PRINTF1(KFunc);
       	return KErrGeneral;
    	}
    
	return KErrGeneral;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemgetGetMore()
// Description   : 	This test creates a semaphore array with specified key and 
//					size and then tried to get a larger array with same key.
// Param1		 :  Key to use for semget
// Param2		 :  Number of semaphores in array
// Param3		 :  Number of semaphore to get (invalid value)
//-------------------------------------------------------------------------------
TInt CTestSemaphore::SemgetGetMore()
	{
    int key = 0;
    int numberOfSem = 0;
    int numberOfSem2 = 0;
	TBuf<8> pNameBuf;
	
	_LIT( KaSemNum, "Param%d" );
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, key);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore key") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	res = GetIntFromConfig(ConfigSection(), pNameBuf, numberOfSem);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
    pNameBuf.Format(KaSemNum,++iParamCnt);
	res = GetIntFromConfig(ConfigSection(), pNameBuf, numberOfSem2);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
	int sem_id,sem_id1,ret=KErrNone;
	
	if ((sem_id = semget(key, numberOfSem, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
		
    sem_id1 = semget(key, numberOfSem2, 0);
    if (sem_id1 == -1) 
		{
		if(errno == EINVAL)
    		{
			_LIT(KFunc, "semaphore set the proper errno \n");
      		INFO_PRINTF1(KFunc);
      		}
    	else
    		{
    		_LIT(KFunc, "semaphore create failed with errno %d\n");
      		INFO_PRINTF2(KFunc, errno);
       		ret = KErrGeneral;
    		}
    	}
    if (semctl(sem_id, numberOfSem, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}

	return ret;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemgetCreateExisting()
// Description   : 	This test creates a semaphore array with specified key and 
//					size and then tried to create one more array with same key.
// Param1		 :  Key to use for semget
// Param2		 :  Number of semaphores in first array
// Param3		 :  Number of semaphore in second array
//-------------------------------------------------------------------------------
TInt CTestSemaphore::SemgetCreateExisting()
	{
    int key = 0;
    int numberOfSem = 0;
    int numberOfSem2 = 0;
	TBuf<8> pNameBuf;
	
	_LIT( KaSemNum, "Param%d" );
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, key);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore key") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
    pNameBuf.Format(KaSemNum,++iParamCnt);
	res = GetIntFromConfig(ConfigSection(), pNameBuf, numberOfSem);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
    pNameBuf.Format(KaSemNum,++iParamCnt);
	res = GetIntFromConfig(ConfigSection(), pNameBuf, numberOfSem2);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
	int sem_id1,sem_id2;
	
	if ((sem_id1 = semget(key, numberOfSem, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    else
    	{
    	sem_id2 = semget(key, numberOfSem2, IPC_CREAT | IPC_EXCL | 0666);
    	if(sem_id2 == -1 && errno == EEXIST)
    		{  		
			_LIT(KFunc, "semaphore set the proper the errno\n");
      		INFO_PRINTF1(KFunc);
    		}
    	else
    		{  		
			_LIT(KFunc, "semaphore didnt set the proper the errno\n");
      		INFO_PRINTF1(KFunc);
       		return KErrGeneral;
    		}
    	}
    if (semctl(sem_id1, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

	return KErrNone;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemgetGetExisting()
// Description   : 	This test creates an array of specified number of semaphores 
//					with specified key and tries to get it again with same key.
// Param1		 :  Key for the semaphore array
// Param2		 :  Number of semaphores to create
// Param3 		 :  Number of semaphores to get
//-------------------------------------------------------------------------------	
TInt CTestSemaphore::SemgetGetExisting()
	{
	int key = 0;
    int numberOfSem = 0;
    int numberOfSem2 = 0;
    int sem_id1 = 0;
    int sem_id2 = 0;
	TBuf<8> pNameBuf;
	
	_LIT( KaSemNum, "Param%d" );
	
	pNameBuf.Format(KaSemNum,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, key);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve semaphore key") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
    pNameBuf.Format(KaSemNum,++iParamCnt);
	res = GetIntFromConfig(ConfigSection(), pNameBuf, numberOfSem);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve create semaphore count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
    pNameBuf.Format(KaSemNum,++iParamCnt);
	res = GetIntFromConfig(ConfigSection(), pNameBuf, numberOfSem2);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve get semaphore count") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}

	if ((sem_id1 = semget(key, numberOfSem, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	sem_id2 = semget(key, numberOfSem2, 0);
	if (sem_id2 == -1) 
		{
		_LIT(KFunc, "semaphore get failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
		
    if (semctl(sem_id1, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

	return KErrNone;
	}

//SEMCTL

//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemctlInvalidId()
// Description   : 	This test tries to call IPC_RMID with an invalid Id.
// Param1		 :  Id to delete (invalid)
//-------------------------------------------------------------------------------	
TInt CTestSemaphore::SemctlInvalidId()
	{
    int sem_id = 0;
	TBuf<8> pNameBuf;
	
	_LIT( KaSemNum, "Param%d" );
	
	pNameBuf.Format(KaSemNum,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, sem_id);
	if(!res)
		{
	 	_LIT(Kerr , "Unable to retrieve sem id") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
			
	if ((semctl(sem_id,1 , IPC_RMID) == -1) && errno == EINVAL ) 
    	{
		_LIT(KFunc, "semctl set the proper errno\n");
      	INFO_PRINTF1(KFunc);
       	return KErrNone;
    	}

	return KErrGeneral;
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemctlInvalidArgsK()
// Description   : 	This test tries to pass an invalid cmd.
// Param1		 :  Key for semaphore array
// Param2		 :  Count to pass to semctl
// Param3		 :  Command to pass to semctl
//-------------------------------------------------------------------------------	
TInt CTestSemaphore::SemctlInvalidArgsK()
	{
    int sem_id = 0;
	
    int key = 0;
	int num = 0;	
	int cmd = 0;
	
	TInt rval = KErrNone;

	TInt res = ReadIntParam(key);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve key") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
				
	res = ReadIntParam(num);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve num") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
	res = ReadIntParam(cmd);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve cmd") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
	if ((sem_id = semget(key,1, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    if (semctl(sem_id, num, cmd) == -1) 
		{
		if(errno == EINVAL) 
			{
			_LIT(KFunc, "semctl set the proper errno\n");
			INFO_PRINTF1(KFunc);
			}
		else
			{
			_LIT(KFunc, "semctl returned improper errno\n");
			INFO_PRINTF1(KFunc);
			rval = KErrGeneral;
			}

		}
	else
		{
		_LIT(KFunc, "semctl returned success (error expected)\n");
		INFO_PRINTF1(KFunc);
		rval = KErrGeneral;
		}
	if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore1 delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    	
	return rval;
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemctlInvalidArgsP()
// Description   : 	This test tries to pass an invalid cmd.
// Param1		 :  Count to pass to semctl
// Param2		 :  Command to pass to semctl
//-------------------------------------------------------------------------------	
TInt CTestSemaphore::SemctlInvalidArgsP()
	{
    int sem_id = 0;
	int num = 0;	
	int cmd = 0;
	
	TInt rval = KErrNone;

	TInt res = ReadIntParam(num);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve num") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
	res = ReadIntParam(cmd);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve cmd") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
	if ((sem_id = semget(IPC_PRIVATE,1, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    if (semctl(sem_id, num, cmd) == -1) 
		{
		if(errno == EINVAL) 
			{
			_LIT(KFunc, "semctl set the proper errno\n");
			INFO_PRINTF1(KFunc);
			}
		else
			{
			_LIT(KFunc, "semctl returned improper errno\n");
			INFO_PRINTF1(KFunc);
			rval = KErrGeneral;
			}

		}
	else
		{
		_LIT(KFunc, "semctl returned success (error expected)\n");
		INFO_PRINTF1(KFunc);
		rval = KErrGeneral;
		}
	if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore1 delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    	
	return rval;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemctlStatK()
// Description   : 	This test creates a sem array, stats it and verifies the 
//					returned values
// Param1		 :  Key for creating array
// Param2		 :  Number of semaphores in array
//-------------------------------------------------------------------------------
TInt CTestSemaphore::SemctlStatK()
	{
	int sem_id,ret=KErrNone;
	struct semid_ds semid_ds1;
	int mode;
	union semun  
		{
		int val;
        struct semid_ds *buf;
        ushort *array;
        } arg;
	int key = 0, num = 0;

	TInt res = ReadIntParam(key);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve key") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}

	res = ReadIntParam(num);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve num") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
	mode = IPC_R|IPC_CREAT | IPC_EXCL ;
	mode = mode | 0666;
	if ((sem_id = semget(key, num, mode)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

    arg.buf = & semid_ds1;
    
    if (semctl(sem_id, 0, IPC_STAT,arg) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
  if(arg.buf->sem_nsems != num)
    	{
    	ret = KErrGeneral;
    	}
    if(arg.buf->sem_otime != 0)
    	{
    	ret = KErrGeneral;
    	}
    if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
	return ret;
	} 

//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemctlStatP()
// Description   : 	This test creates a private sem array, stats it and verifies 
//					the returned values
// Param1		 :  Number of semaphores in array
//-------------------------------------------------------------------------------
TInt CTestSemaphore::SemctlStatP()
	{
	int sem_id,ret=KErrNone;
	struct semid_ds semid_ds1;
	int mode;
	union semun  
		{
		int val;
        struct semid_ds *buf;
        ushort *array;
        } arg;
	int num = 0;

	TInt res = ReadIntParam(num);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve num") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
	mode = IPC_R|IPC_CREAT | IPC_EXCL ;
	mode = mode | 0666;
	if ((sem_id = semget(IPC_PRIVATE, num, mode)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

    arg.buf = & semid_ds1;
    
    if (semctl(sem_id, 0, IPC_STAT,arg) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
  if(arg.buf->sem_nsems != num)
    	{
    	ret = KErrGeneral;
    	}
    if(arg.buf->sem_otime != 0)
    	{
    	ret = KErrGeneral;
    	}
    if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
	return ret;
	} 
	
//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemctlSetK()
// Description   : 	This test creates a sem array, set values, and then stats it 
//					and verifies the returned values
// Param1		 :  Key for creating array
// Param2		 :  Number of semaphores in array
//-------------------------------------------------------------------------------
TInt CTestSemaphore::SemctlSetK()
	{
	int sem_id,ret=KErrNone;
	struct semid_ds semid_ds1;

	union semun  
		{
		int val;
        struct semid_ds *buf;
        ushort *array;
        } arg,arg1;
	int key = 0, num = 0;

	TInt res = ReadIntParam(key);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve key") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}

	res = ReadIntParam(num);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve num") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
	if ((sem_id = semget(key, num, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

    arg.buf = & semid_ds1;
    
    if (semctl(sem_id, 0, IPC_STAT,arg) == -1) 
    	{
		_LIT(KFunc, "semaphore IPC_STAT failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    arg.buf->sem_perm.mode = IPC_W | IPC_R | IPC_M;
    
    if(semctl(sem_id,0,IPC_SET,arg) == -1)
    	{
		_LIT(KFunc, "semaphore IPC_SET failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
    arg1.buf = & semid_ds1;	
    
    if (semctl(sem_id, 0, IPC_STAT,arg1) == -1) 
    	{
		_LIT(KFunc, "semaphore IPC_STAT failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
    if(arg1.buf->sem_perm.mode != (IPC_W | IPC_R | IPC_M) )
    	{
    	ret = KErrGeneral;
    	}
    if(arg1.buf->sem_nsems != num)
    	{
    	ret = KErrGeneral;
    	}
    if(arg1.buf->sem_otime != 0)
    	{
    	ret = KErrGeneral;
    	}
    	
    if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
	return ret;
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemctlSetP()
// Description   : 	This test creates a sem array, set values, and then stats it 
//					and verifies the returned values
// Param1		 :  Number of semaphores in array
//-------------------------------------------------------------------------------
TInt CTestSemaphore::SemctlSetP()
	{
	int sem_id,ret=KErrNone;
	struct semid_ds semid_ds1;

	union semun  
		{
		int val;
        struct semid_ds *buf;
        ushort *array;
        } arg,arg1;
	int num = 0;

	TInt res = ReadIntParam(num);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve num") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
	if ((sem_id = semget(IPC_PRIVATE, num, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

    arg.buf = & semid_ds1;
    
    if (semctl(sem_id, 0, IPC_STAT,arg) == -1) 
    	{
		_LIT(KFunc, "semaphore IPC_STAT failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    arg.buf->sem_perm.mode = IPC_W | IPC_R | IPC_M;
    
    if(semctl(sem_id,0,IPC_SET,arg) == -1)
    	{
		_LIT(KFunc, "semaphore IPC_SET failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
    arg1.buf = & semid_ds1;	
    
    if (semctl(sem_id, 0, IPC_STAT,arg1) == -1) 
    	{
		_LIT(KFunc, "semaphore IPC_STAT failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
    if(arg1.buf->sem_perm.mode != (IPC_W | IPC_R | IPC_M) )
    	{
    	ret = KErrGeneral;
    	}
    if(arg1.buf->sem_nsems != num)
    	{
    	ret = KErrGeneral;
    	}
    if(arg1.buf->sem_otime != 0)
    	{
    	ret = KErrGeneral;
    	}
    	
    if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
	return ret;
	}

//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemctlGetValK()
// Description   : 	This test creates a sem array, gets its values, and logs it
// Param1		 :  Key of the semaphore array
// Param2		 :  Number of semaphores in array
// Param3 		 :  Invalid semnum to check
//-------------------------------------------------------------------------------
TInt CTestSemaphore::SemctlGetValK()
	{
	int sem_id,ret=KErrNone,get=0;
	int key = 0, num = 0, inval = 0;

	TInt res = ReadIntParam(key);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve key") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}

	res = ReadIntParam(num);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve num") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}

	res = ReadIntParam(inval);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve inval") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
		
	if ((sem_id = semget(key, num, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

	for(int idx=0;idx<num;idx++)
		{
		get = semctl(sem_id, idx, GETVAL);
		if (get == -1) 
			{
			_LIT(KFunc, "semaphore GETVAL failed with errno %d\n");
			INFO_PRINTF2(KFunc, errno);
			ret = KErrGeneral;
			}
		_LIT(KFunc, "semaphore variable semval[%d] is %d\n");
		INFO_PRINTF3(KFunc, idx, get);
		}
	get = semctl(sem_id, inval, GETVAL);
	if (get == -1)
		{
		if(errno == EINVAL)
			{
			_LIT(KFunc, "semctl set the proper errno\n");
			INFO_PRINTF1(KFunc);
			}
		else
			{
			_LIT(KFunc, "semctl failed with errno %d\n");
			INFO_PRINTF2(KFunc, errno);
			ret = KErrGeneral;
			}
		}
	else
		{
		_LIT(KFunc, "semctl succeeded (failure expected)");
		INFO_PRINTF1(KFunc);
		ret = KErrGeneral;
		}
	
	
    if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    return ret;
	} 
	
//-------------------------------------------------------------------------------
// Function Name : 	CTestSemaphore::SemctlGetValP()
// Description   : 	This test creates a private sem array, gets its values, and 
//					logs it
// Param1		 :  Number of semaphores in array
// Param2 		 :  Invalid semnum to check
//-------------------------------------------------------------------------------
TInt CTestSemaphore::SemctlGetValP()
	{
	int sem_id,ret=KErrNone,get=0;
	int num = 0, inval = 0;

	TInt res = ReadIntParam(num);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve num") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}

	res = ReadIntParam(inval);
	if(res!=KErrNone)
		{
	 	_LIT(Kerr , "Unable to retrieve inval") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
		
		
	if ((sem_id = semget(IPC_PRIVATE, num, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

	for(int idx=0;idx<num;idx++)
		{
		get = semctl(sem_id, idx, GETVAL);
		if (get == -1) 
			{
			_LIT(KFunc, "semaphore GETVAL failed with errno %d\n");
			INFO_PRINTF2(KFunc, errno);
			ret = KErrGeneral;
			}
		_LIT(KFunc, "semaphore variable semval[%d] is %d\n");
		INFO_PRINTF3(KFunc, idx, get);
		}
	get = semctl(sem_id, inval, GETVAL);
	if (get == -1)
		{
		if(errno == EINVAL)
			{
			_LIT(KFunc, "semctl set the proper errno\n");
			INFO_PRINTF1(KFunc);
			}
		else
			{
			_LIT(KFunc, "semctl failed with errno %d\n");
			INFO_PRINTF2(KFunc, errno);
			ret = KErrGeneral;
			}
		}
	else
		{
		_LIT(KFunc, "semctl succeeded (failure expected)");
		INFO_PRINTF1(KFunc);
		ret = KErrGeneral;
		}
	
	
    if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    return ret;
	} 
	
// successful SETVAL

TInt CTestSemaphore::semctltest8()
	{
	int sem_id,ret=KErrNone,get=0;

	union semun  
		{
		int val;
        struct semid_ds *buf;
        ushort *array;
        } arg;

	if ((sem_id = semget(IPC_PRIVATE,1, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

    arg.val = 3;
    
    get = semctl(sem_id, 0, GETVAL);
    if (get == -1) 
    	{
		_LIT(KFunc, "semaphore GETVAL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    _LIT(KFunc, "semaphore variable semval before change is %d\n");
	INFO_PRINTF2(KFunc, get);
	
	get = semctl(sem_id, 0, SETVAL,arg);
    if (get == -1) 
    	{
		_LIT(KFunc, "semaphore SETVAL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    get = semctl(sem_id, 0, GETVAL);
    if (get != 3) 
    	{
		_LIT(KFunc, "semaphore GETVAL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    _LIT(KFunc1, "semaphore variable semval after change is %d\n");
	INFO_PRINTF2(KFunc1, get);
		
    if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    return ret;
	}


// SETVAL with errno ERANGE

TInt CTestSemaphore::semctltest9()
	{
	int sem_id,ret=KErrNone,get=0,get1=0;

	union semun  
		{
		int val;
        struct semid_ds *buf;
        ushort *array;
        } arg;

	if ((sem_id = semget(IPC_PRIVATE,1, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

    arg.val = -3;
    
    get1 = semctl(sem_id, 0, GETVAL);
    if (get1 == -1) 
    	{
		_LIT(KFunc, "semaphore GETVAL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    _LIT(KFunc, "semaphore variable semval before change is %d\n");
	INFO_PRINTF2(KFunc, get1);
	
	get = semctl(sem_id, 0, SETVAL,arg);
    if (get != -1 && errno != ERANGE) 
    	{
		_LIT(KFunc, "semaphore SETVAL didnt set the proper errno");
      	INFO_PRINTF1(KFunc);
       	ret = KErrGeneral;
    	}
    
    get = semctl(sem_id, 0, GETVAL);
    if (get != get1) 
    	{
		_LIT(KFunc, "semaphore GETVAL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    _LIT(KFunc1, "semaphore variable semval after change is %d\n");
	INFO_PRINTF2(KFunc1, get);
		
    if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    return ret;
	}
	
// for getpid 

TInt CTestSemaphore::semctltest10()
	{
   	
	int sem_id,ret=KErrNone,get=0;

	if ((sem_id = semget(IPC_PRIVATE,1, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    	
    get = semctl(sem_id, 0, GETPID);
    if (get == -1) 
    	{
		_LIT(KFunc, "semaphore GETPID failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}	
	
	if(get != 0)
		{
		_LIT(KFunc, "semaphore GETPID didnt get the proper pid\n");
      	INFO_PRINTF1(KFunc);
       	ret = KErrGeneral;
    	}
    
    if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
	return ret;
	}

// for getncnt
TInt CTestSemaphore::semctltest11()
	{
	
	int sem_id,ret=KErrNone,get=0;

	if ((sem_id = semget(IPC_PRIVATE,1, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    	
    get = semctl(sem_id, 0, GETNCNT);
    if (get != 0) 
    	{
		_LIT(KFunc, "semaphore GETNCNT failed with errno %d\n");
      	INFO_PRINTF2(KFunc, get);
       	ret = KErrGeneral;
    	}	
	
	if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
	return ret;
	}
	
// for getzcnt
TInt CTestSemaphore::semctltest12()
	{
	
	int sem_id,ret=KErrNone,get=0;

	if ((sem_id = semget(IPC_PRIVATE,1, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    	
    get = semctl(sem_id, 0, GETZCNT);
    if (get != 0) 
    	{
		_LIT(KFunc, "semaphore GETZCNT failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}	
	
    if (semctl(sem_id, 0, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
	return ret;
	}

// proper SETVAL and GETVAL

TInt CTestSemaphore::semctltest13()
	{

	union semun 
		{
    	int val;
    	struct semid_ds *buf;
    	unsigned short  *array;
		};
	int semid;
	union semun tmpo;
	int ret = KErrNone,ret1;
    unsigned short tmp[4];
    unsigned short tmpi[4];
    
    tmp[0] = 3;
    tmp[1] = 7;
	tmp[2] = 9;
	tmp[3] = 19;
	
	semid = semget(IPC_PRIVATE ,4, IPC_CREAT | IPC_EXCL | 0666);
	if(semid == -1)
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	
	tmpo.array = &tmp[0];
	
	ret1 = semctl(semid, 3, SETALL ,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore SETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
	
	
	tmpo.array = &tmpi[0];
	ret1 = semctl(semid, 3,GETALL  ,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    if(tmpi[0] != tmp[0] || tmpi[1] != tmp[1] || tmpi[2] != tmp[2] || tmpi[3] != tmp[3])
    	{
		_LIT(KFunc, "semaphore SETALL didnt set the proper values %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    if (semctl(semid, 3, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
	return ret;
	}
	
// SETVAL and GETVAL which sets the errno ERANGE

TInt CTestSemaphore::semctltest14()
	{
	
	union semun 
		{
    	int val;
    	struct semid_ds *buf;
    	unsigned short  *array;
		};
	int semid;
	union semun tmpo;
	int ret=KErrNone,ret1;
    unsigned short tmp[3];
        
    tmp[0] = 3;
    tmp[1] = 7;
	tmp[2] = 9;
	
	semid = semget(IPC_PRIVATE ,4, IPC_CREAT | IPC_EXCL | 0666);
	if(semid == -1)
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	
	tmpo.array = &tmp[0];
	
	ret1 = semctl(semid, 3, SETALL ,tmpo);
	if(ret1 != -1 && errno != ERANGE)
		{
		_LIT(KFunc, "semaphore SETALL didnt set the proper errno\n");
      	INFO_PRINTF1(KFunc);
       	ret = KErrGeneral;
    	}
		
	if (semctl(semid, 3, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
	return ret;
	}

					// SEMOP

// check for the errno E2BIG					
TInt CTestSemaphore::semoptest1()
	{
	int msq_id,ret=KErrNone,semnops=35,err;
	struct sembuf lSemBuf[35];
	
	if ((msq_id = semget(IPC_PRIVATE,35, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	for(int i=0;i<semnops;i++)
		{
		lSemBuf[i].sem_num = i;
        lSemBuf[i].sem_op = 1;
        lSemBuf[i].sem_flg = 0;
		}
    err = semop(msq_id, &lSemBuf[0], semnops);
    
    if(err != -1 && errno != E2BIG)
    	{
		_LIT(KFunc, "semaphore operation failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    if (semctl(msq_id, 34, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
	return ret;
	}

// check for the errno EFBIG with sem_num < 0
				
TInt CTestSemaphore::semoptest2()
	{
	int msq_id,ret=KErrNone,semnops=3;
	struct sembuf lSemBuf[3];
	
	if ((msq_id = semget(IPC_PRIVATE,3, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	for(int i=0;i<semnops;i++)
		{
		lSemBuf[i].sem_num = i-1;
        lSemBuf[i].sem_op = 1;
        lSemBuf[i].sem_flg = 0;
		}
    TInt err = semop(msq_id, &lSemBuf[0], semnops);
    
    if(err != -1 && errno != EFBIG)
    	{
		_LIT(KFunc, "semaphore operation failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    if (semctl(msq_id, 2, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
	return ret;
	}
	
// check for the errno EFBIG with sem_num >= nsems
				
TInt CTestSemaphore::semoptest3()
	{
	int msq_id,ret=KErrNone,semnops=3;
	struct sembuf lSemBuf[3];
	
	if ((msq_id = semget(IPC_PRIVATE,3, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	for(int i=0;i<semnops;i++)
		{
		lSemBuf[i].sem_num = semnops+3;
        lSemBuf[i].sem_op = 1;
        lSemBuf[i].sem_flg = 0;
		}
    TInt err = semop(msq_id, &lSemBuf[0], semnops);
    
    if(err != -1 && errno != EFBIG)
    	{
		_LIT(KFunc, "semaphore operation failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    if (semctl(msq_id, 2, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
	return ret;
	}
	
// check for the errno EIDRM 
				
TInt CTestSemaphore::semoptest4()
	{
	int msq_id,ret=KErrNone,semnops=3;
	struct sembuf lSemBuf[3];
	
	if ((msq_id = semget(IPC_PRIVATE,3, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	for(int i=0;i<semnops;i++)
		{
		lSemBuf[i].sem_num =i;
        lSemBuf[i].sem_op = 1;
        lSemBuf[i].sem_flg = 0;
		}
	
	if (semctl(msq_id, 2, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
    TInt err = semop(msq_id, &lSemBuf[0], semnops);
    
    if(err != -1 && errno != EIDRM)
    	{
		_LIT(KFunc, "semaphore operation failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
 	return ret;
	}
	
// check for the errno EINVAL with invalid semid
				
TInt CTestSemaphore::semoptest5()
	{
	int msq_id=-1,ret=KErrNone,semnops=3;
	struct sembuf lSemBuf[3];
	
	for(int i=0;i<semnops;i++)
		{
		lSemBuf[i].sem_num =i;
        lSemBuf[i].sem_op = 1;
        lSemBuf[i].sem_flg = 0;
		}
	
	TInt err = semop(msq_id, &lSemBuf[0], semnops);
    
    if(err != -1 && errno != EINVAL)
    	{
		_LIT(KFunc, "semaphore operation failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
   	
 	return ret;
	}
	
// If sem_op is a positive integer and the calling process has alter permission, the value of sem_op shall be added to semval
TInt CTestSemaphore::semoptest6()
	{
	int msq_id,ret=KErrNone,semnops=2;
	int ret1,i;
	struct sembuf lSemBuf[3];
	union semun tmpo;
	unsigned short tmpi[2];

	if ((msq_id = semget(IPC_PRIVATE,2, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    
    _LIT(KStmt1,"Before Semop operations ");
    INFO_PRINTF1(KStmt1);
    tmpo.array = &tmpi[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmpi[i]);
    	}
       
	for(int i=0;i<semnops;i++)
		{
		lSemBuf[i].sem_num =1;
        lSemBuf[i].sem_op = 1;
        lSemBuf[i].sem_flg = 0;
		}
	
	TInt err = semop(msq_id, &lSemBuf[0], semnops);
    
    if(err == -1)
    	{
		_LIT(KFunc, "semaphore operation failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
   	
   	tmpo.array = &tmpi[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    _LIT(KStmt,"After Semop operations ");
    INFO_PRINTF1(KStmt);
    tmpo.array = &tmpi[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmpi[i]);
    	}
    
    if( tmpi[0] != 0 && tmpi[1] != 2)
    	{
		_LIT(KFunc, "semaphore semop failed and didnt modified the semval\n");
      	INFO_PRINTF1(KFunc);
       	ret = KErrGeneral;
    	}
   	if (semctl(msq_id, 1, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
 	return ret;
	}


// If sem_op is 0 and the calling process has read permission, 
// If semval is 0, semop() shall return immediately. ( with out any flags )

TInt CTestSemaphore::semoptest7()
	{
	int msq_id,ret=KErrNone,semnops=2;
	int ret1,i;
	struct sembuf lSemBuf[3];
	union semun tmpo;
	unsigned short tmpi[2];
	unsigned short tmp1[2];

	if ((msq_id = semget(IPC_PRIVATE,2, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    
    _LIT(KStmt1,"Before Semop operations ");
    INFO_PRINTF1(KStmt1);
    tmpo.array = &tmpi[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmpi[i]);
    	}
       
	for(int i=0;i<semnops;i++)
		{
		lSemBuf[i].sem_num =1;
        lSemBuf[i].sem_op = 0;
        lSemBuf[i].sem_flg = 0;
		}
	
	TInt err = semop(msq_id, &lSemBuf[0], semnops);
    
    if(err == -1)
    	{
		_LIT(KFunc, "semaphore operation failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
   	
    _LIT(KStmt,"After Semop operations ");
    INFO_PRINTF1(KStmt);
    tmpo.array = &tmp1[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmp1[i]);
    	}
    
    if( tmp1[0] != tmpi[0] && tmp1[1] != tmpi[1])
    	{
		_LIT(KFunc, "semaphore semop failed and didnt modified the semval\n");
      	INFO_PRINTF1(KFunc);
       	ret = KErrGeneral;
    	}
   	if (semctl(msq_id, 1, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
 	return ret;
	}

// If sem_op is 0 and the calling process has read permission, 
// If semval is 0, semop() shall return immediately. ( with IPC_NOWAIT flags )

TInt CTestSemaphore::semoptest8()
	{
	int msq_id,ret=KErrNone,semnops=2;
	int ret1,i;
	struct sembuf lSemBuf[3];
	union semun tmpo;
	unsigned short tmpi[2];
	unsigned short tmp1[2];

	if ((msq_id = semget(IPC_PRIVATE,2, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    
    _LIT(KStmt1,"Before Semop operations ");
    INFO_PRINTF1(KStmt1);
    tmpo.array = &tmpi[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmpi[i]);
    	}
       
	for(int i=0;i<semnops;i++)
		{
		lSemBuf[i].sem_num =1;
        lSemBuf[i].sem_op = 0;
        lSemBuf[i].sem_flg = IPC_NOWAIT;
		}
	
	TInt err = semop(msq_id, &lSemBuf[0], semnops);
    
    if(err == -1)
    	{
		_LIT(KFunc, "semaphore operation failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
   	
    _LIT(KStmt,"After Semop operations ");
    INFO_PRINTF1(KStmt);
    tmpo.array = &tmp1[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmp1[i]);
    	}
    
    if( tmp1[0] != tmpi[0] && tmp1[1] != tmpi[1])
    	{
		_LIT(KFunc, "semaphore semop failed and modified the semval\n");
      	INFO_PRINTF1(KFunc);
       	ret = KErrGeneral;
    	}
   	if (semctl(msq_id, 1, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
 	return ret;
	}

// If sem_op is 0 and the calling process has read permission,
// If semval is non-zero and (sem_flg &IPC_NOWAIT) is non-zero, semop() shall return immediately.

TInt CTestSemaphore::semoptest9()
	{
	int msq_id,ret=KErrNone,semnops=2;
	int ret1,i,get;
	struct sembuf lSemBuf[3];
	union semun tmpo,arg;
	unsigned short tmpi[2];
	unsigned short tmp1[2];

	if ((msq_id = semget(IPC_PRIVATE,2, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    
    _LIT(KStmt1,"Before Semop operations ");
    INFO_PRINTF1(KStmt1);
    tmpo.array = &tmpi[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmpi[i]);
    	}
    
    // setval to be done
    arg.val = 3;
	
	get = semctl(msq_id, 1, SETVAL,arg);
    if (get == -1) 
    	{
		_LIT(KFunc, "semaphore SETVAL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
       
	for(int i=0;i<semnops;i++)
		{
		lSemBuf[i].sem_num =1;
        lSemBuf[i].sem_op = 0;
        lSemBuf[i].sem_flg = IPC_NOWAIT;
		}
	
	TInt err = semop(msq_id, &lSemBuf[0], semnops);
    
    if(err != -1 && errno != EAGAIN)
    	{
		_LIT(KFunc, "semaphore operation failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
   	
    _LIT(KStmt,"After Semop operations ");
    INFO_PRINTF1(KStmt);
    tmpo.array = &tmp1[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmp1[i]);
    	}
    
    if( tmp1[0] != 0 && tmp1[1] != 3)
    	{
		_LIT(KFunc, "semaphore semop failed and didnt modified the semval\n");
      	INFO_PRINTF1(KFunc);
       	ret = KErrGeneral;
    	}
   	if (semctl(msq_id, 1, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
 	return ret;
	}


// If sem_op is a negative integer and the calling process has alter permission,
// If semval is greater than or equal to the absolute value of sem_op, the absolute value of sem_op is subtracted from semval.

TInt CTestSemaphore::semoptest10()
	{
	int msq_id,ret=KErrNone,semnops=1;
	int ret1,i,get;
	struct sembuf lSemBuf[3];
	union semun tmpo,arg;
	unsigned short tmpi[2];
	unsigned short tmp1[2];

	if ((msq_id = semget(IPC_PRIVATE,2, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    
    _LIT(KStmt1,"Before Semop operations ");
    INFO_PRINTF1(KStmt1);
    tmpo.array = &tmpi[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmpi[i]);
    	}
     
 //   for(i=0;i<2;i++)
  //  	{
    	lSemBuf[0].sem_num = 1;
	lSemBuf[0].sem_op = -1;
	lSemBuf[0].sem_flg = 0;

    //	}
	
	arg.val = 3;
	
	get = semctl(msq_id, 1, SETVAL,arg);
    if (get == -1) 
    	{
		_LIT(KFunc, "semaphore SETVAL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
	TInt err = semop(msq_id, &lSemBuf[0], semnops);
    
    if(err == -1)
    	{
		_LIT(KFunc, "semaphore operation failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
   	
    _LIT(KStmt,"After Semop operations ");
    INFO_PRINTF1(KStmt);
    tmpo.array = &tmp1[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmp1[i]);
    	}
    
    if( tmp1[0] != tmpi[0] && tmp1[1] != (tmpi[1] - 2))
    	{
		_LIT(KFunc, "semaphore semop failed and didnt modified the semval\n");
      	INFO_PRINTF1(KFunc);
       	ret = KErrGeneral;
    	}
   	if (semctl(msq_id, 1, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
 	return ret;
	}

// If sem_op is a negative integer and the calling process has alter permission,
// If semval is less than the absolute value of sem_op and (sem_flg &IPC_NOWAIT) is non-zero, semop() shall return immediately.

TInt CTestSemaphore::semoptest11()
	{
	int msq_id,ret=KErrNone,semnops=1;
	int ret1,i;
	struct sembuf lSemBuf[3];
	union semun tmpo;
	unsigned short tmpi[2];
	unsigned short tmp1[2];

	if ((msq_id = semget(IPC_PRIVATE,2, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    
    _LIT(KStmt1,"Before Semop operations ");
    INFO_PRINTF1(KStmt1);
    tmpo.array = &tmpi[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmpi[i]);
    	}
       
	lSemBuf[0].sem_num = 1;
	lSemBuf[0].sem_op = -1;
	lSemBuf[0].sem_flg = IPC_NOWAIT;

	TInt err = semop(msq_id, &lSemBuf[0], semnops);
    
    if(err != -1 && errno != EAGAIN)
    	{
		_LIT(KFunc, "semaphore operation failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
   	
    _LIT(KStmt,"After Semop operations ");
    INFO_PRINTF1(KStmt);
    tmpo.array = &tmp1[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmp1[i]);
    	}
    
    if( tmp1[0] != tmpi[0] && tmp1[1] != tmpi[1] )
    	{
		_LIT(KFunc, "semaphore semop failed and modified the semval\n");
      	INFO_PRINTF1(KFunc);
       	ret = KErrGeneral;
    	}
   	if (semctl(msq_id, 1, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
 	return ret;
	}

						// THREADS
						
// If sem_op is 0 and the calling process has read permission,

// If semval is non-zero and (sem_flg &IPC_NOWAIT) is 0, semop() shall increment the semzcnt associated with the specified semaphore and suspend execution of the calling thread until one of the following occurs:
// The value of semval becomes 0, at which time the value of semzcnt associated with the specified semaphore shall be decremented.


void* ThreadEntryOp(void* arg)
	{
	int semnops=2;    
    int msgq_id = (int)arg;
    struct sembuf lSemBuf[3];
    
    for(int i=0;i<semnops;i++)
		{
		lSemBuf[i].sem_num = 1;
        lSemBuf[i].sem_op  = 0;
        lSemBuf[i].sem_flg = 0;
		}
	
	TInt err = semop(msgq_id, &lSemBuf[0], 2); // blocks here 
 
    // check semzcnt
    int get = semctl(msgq_id, 1, GETZCNT);

    return NULL;
	}

TInt ThreadSemOpTest(int msgq_id)
	{
	
	int get;
	union semun arg;
	int ret = KErrNone;
	
	// check semzcnt 
	get = semctl(msgq_id, 1, GETZCNT);
	if( get != 1 )
		{
		ret = KErrGeneral;
		}
		
	// make semval to 0
	arg.val = 0;
	get = semctl(msgq_id, 1, SETVAL,arg);
    if (get == -1) 
    	{
       	ret = KErrGeneral;
    	}
    	
	// wait ;
	sleep(10);
	return ret;
	}
	
TInt SemOpThreadMain( int id )
	{

    TInt err = KErrNone;
    int threadRetVal = 0,get;
	void *threadRetValPtr = (void*)&threadRetVal;
    pthread_t testThread;
    err = pthread_create(&testThread,(pthread_attr_t *)NULL,ThreadEntryOp,(TAny*)id);
    if (!err)
    	{
    	sleep(2);
        err = ThreadSemOpTest(id);
        }
        
    get = semctl(id, 1, GETZCNT);
	if( get != 0 )
		{
		err = KErrGeneral;
		}
    pthread_join(testThread, &threadRetValPtr);        
    return err;
	}
	


TInt CTestSemaphore::semoptest12()
	{
	int msq_id,ret=KErrNone;
	int ret1,i,get;
	union semun tmpo,arg;
	unsigned short tmpi[2];
	
	if ((msq_id = semget(IPC_PRIVATE,2, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    
    _LIT(KStmt1,"Before Semop operations ");
    INFO_PRINTF1(KStmt1);
    tmpo.array = &tmpi[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmpi[i]);
    	}
    
    // setval to be done
    arg.val = 3;
	
	get = semctl(msq_id, 1, SETVAL,arg);
    if (get == -1) 
    	{
		_LIT(KFunc, "semaphore SETVAL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
       
	
	
	ret = SemOpThreadMain(msq_id);
	sleep(2);
	if (semctl(msq_id, 1, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
	
 	return ret;
	}



// If sem_op is 0 and the calling process has read permission, one of the following shall occur:

//If semval is non-zero and (sem_flg &IPC_NOWAIT) is 0, semop() shall increment the semzcnt associated with the specified semaphore and suspend execution of the calling thread until one of the following occurs:
//The semid for which the calling thread is awaiting action is removed from the system. When this occurs, errno shall be set equal to [EIDRM] and -1 shall be returned.

void* ThreadEntryOp1(void* arg)
	{
	int semnops=2,err;    
    int msgq_id = (int)arg;
    struct sembuf lSemBuf[3];
    
    for(int i=0;i<semnops;i++)
		{
		lSemBuf[i].sem_num = 1;
        lSemBuf[i].sem_op  = 0;
        lSemBuf[i].sem_flg = 0;
		}
	
	err = semop(msgq_id, &lSemBuf[0], 2); // blocks here 
    
    if(err == -1 && errno == EIDRM)
    	{
       	iFlag=1;
    	}
    
    return NULL;
	}

TInt ThreadSemOpTest1(int msgq_id)
	{
	
	int get;
	int ret = KErrNone;
	
	// check semzcnt 
	get = semctl(msgq_id, 1, GETZCNT);
	if( get != 1 )
		{
		ret = KErrGeneral;
		}
		
	// delete the semaphore
	if (semctl(msgq_id, 1, IPC_RMID) == -1) 
    	{
		ret = KErrGeneral;
    	}
    	
	// wait ;
	sleep(10);
	return ret;
	}
	
TInt SemOpThreadMain1( int id )
	{

    TInt err = KErrNone;
    int threadRetVal = 0;
	void *threadRetValPtr = (void*)&threadRetVal;
    pthread_t testThread;
    err = pthread_create(&testThread,(pthread_attr_t *)NULL,ThreadEntryOp1,(TAny*)id);
    if (!err)
    	{
    	sleep(2);
        err = ThreadSemOpTest1(id);
        }

    pthread_join(testThread, &threadRetValPtr);        
    return err;
	}

TInt CTestSemaphore::semoptest13()
	{
	int msq_id,ret=KErrNone;
	int ret1,i,get;
	union semun tmpo,arg;
	unsigned short tmpi[2];
	
	iFlag = 0;
	if ((msq_id = semget(IPC_PRIVATE,2, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    
    _LIT(KStmt1,"Before Semop operations ");
    INFO_PRINTF1(KStmt1);
    tmpo.array = &tmpi[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmpi[i]);
    	}
    
    // setval to be done
    arg.val = 3;
	
	get = semctl(msq_id, 1, SETVAL,arg);
    if (get == -1) 
    	{
		_LIT(KFunc, "semaphore SETVAL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}

	ret = SemOpThreadMain1(msq_id);
	sleep(2);
	if(!iFlag)
		{
		ret = KErrGeneral;
		}
	
 	return ret;
	}


// If sem_op is a negative integer and the calling process has alter permission, one of the following shall occur:
// If semval is less than the absolute value of sem_op and (sem_flg &IPC_NOWAIT) is 0, semop() shall increment the semncnt associated with the specified semaphore and suspend execution of the calling thread until one of the following conditions occurs:
// The semid for which the calling thread is awaiting action is removed from the system. When this occurs, errno shall be set equal to [EIDRM] and -1 shall be returned.

void* ThreadEntryOp2(void* arg)
	{
	int semnops=2;    
    int msgq_id = (int)arg;
    struct sembuf lSemBuf[3];
    
    for(int i=0;i<semnops;i++)
		{
		lSemBuf[i].sem_num = 1;
        lSemBuf[i].sem_op  = -1;
        lSemBuf[i].sem_flg = 0;
		}
	
	TInt err = semop(msgq_id, &lSemBuf[0], 2); // blocks here 
    
    if(err == -1 && errno == EIDRM)
    	{
       	iFlag=1;
    	}
    
    return NULL;
	}

TInt ThreadSemOpTest2(int msgq_id)
	{
	
	int get;
	int ret = KErrNone;
	
	// check semzcnt 
	get = semctl(msgq_id, 1, GETNCNT);
	if( get != 1 )
		{
		ret = KErrGeneral;
		}
		
	// delete the semaphore
	if (semctl(msgq_id, 1, IPC_RMID) == -1) 
    	{
		ret = KErrGeneral;
    	}
    	
	// wait ;
	sleep(10);
	return ret;
	}
	
TInt SemOpThreadMain2( int id )
	{

    TInt err = KErrNone;
    int threadRetVal = 0;
	void *threadRetValPtr = (void*)&threadRetVal;
    pthread_t testThread;
    err = pthread_create(&testThread,(pthread_attr_t *)NULL,ThreadEntryOp2,(TAny*)id);
    if (!err)
    	{
    	sleep(2);
        err = ThreadSemOpTest2(id);
        }
        
    pthread_join(testThread, &threadRetValPtr);        
    return err;
	}
	


TInt CTestSemaphore::semoptest14()
	{
	int msq_id,ret=KErrNone;
	int ret1,i;
	union semun tmpo;
	unsigned short tmpi[2];
	
	iFlag = 0;
	if ((msq_id = semget(IPC_PRIVATE,2, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    
    _LIT(KStmt1,"Before Semop operations ");
    INFO_PRINTF1(KStmt1);
    tmpo.array = &tmpi[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmpi[i]);
    	}
    
	ret = SemOpThreadMain2(msq_id);
	sleep(2);
	if(!iFlag)
		{
		ret = KErrGeneral;
		}
	
 	return ret;
	}

// If sem_op is a negative integer and the calling process has alter permission, one of the following shall occur:
// If semval is less than the absolute value of sem_op and (sem_flg &IPC_NOWAIT) is 0, semop() shall increment the semncnt associated with the specified semaphore and suspend execution of the calling thread until one of the following conditions occurs:
// The value of semval becomes greater than or equal to the absolute value of sem_op. When this occurs, the value of semncnt associated with the specified semaphore shall be decremented, the absolute value of sem_op shall be subtracted from semval 

void* ThreadEntryOp3(void* arg)
	{
	int get;    
    int msgq_id = (int)arg;
    struct sembuf lSemBuf[3];

	lSemBuf[0].sem_num = 0;
        lSemBuf[0].sem_op  = 0;
        lSemBuf[0].sem_flg = 0;
	lSemBuf[1].sem_num = 1;
        lSemBuf[1].sem_op  = -1;
        lSemBuf[1].sem_flg = 0;
	TInt err = semop(msgq_id, &lSemBuf[0], 2); // blocks here 
    
    if(err == -1)
    	{
       	iFlag = 0 ;
    	}
    
    // check semzcnt
    get = semctl(msgq_id, 1, GETNCNT);
	if( get != 0 )
		{
		iFlag = 0;
		}
	// get the semval 
	get = semctl(msgq_id, 1, GETVAL);
    if (get != 2) 
    	{
		iFlag = 0;
    	}
		
    return NULL;
	}

TInt ThreadSemOpTest3(int msgq_id)
	{
	
	int get;
	union semun arg;
	int ret = KErrNone;
	
	// check semzcnt 
	get = semctl(msgq_id, 1, GETNCNT);
	if( get != 1 )
		{
		ret = KErrGeneral;
		}
		
	// make semval to 3
	arg.val = 3;
	get = semctl(msgq_id, 1, SETVAL,arg);
    if (get == -1) 
    	{
       	ret = KErrGeneral;
    	}
    	
	// wait ;
	sleep(10);
	return ret;
	}
	
TInt SemOpThreadMain3( int id )
	{

    TInt err = KErrNone;
    int threadRetVal = 0,get;
	void *threadRetValPtr = (void*)&threadRetVal;
    pthread_t testThread;
    err = pthread_create(&testThread,(pthread_attr_t *)NULL,ThreadEntryOp3,(TAny*)id);
    if (!err)
    	{
    	sleep(2);
        err = ThreadSemOpTest3(id);
        }
        
    get = semctl(id, 1, GETZCNT);
	if( get != 0 )
		{
		err = KErrGeneral;
		}
    pthread_join(testThread, &threadRetValPtr);        
    return err;
	}
	


TInt CTestSemaphore::semoptest15()
	{
	int msq_id,ret=KErrNone;
	int ret1,i;
	union semun tmpo;
	unsigned short tmpi[2];
	iFlag = 1;
	
	if ((msq_id = semget(IPC_PRIVATE,2, IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    
    _LIT(KStmt1,"Before Semop operations ");
    INFO_PRINTF1(KStmt1);
    tmpo.array = &tmpi[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmpi[i]);
    	}
    
	ret = SemOpThreadMain3(msq_id);
	sleep(2);
	if (semctl(msq_id, 1, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
	if (!iFlag)
		{
		ret = KErrGeneral;
		}
 	return ret;
	}
