/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TestHarness_H__
#define __TestHarness_H__

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>

#if 0

#define MSG(n)  \
{ \
	if(aThreadData->iCommonData->iTestModuleBase) \
	    { \
	  	aThreadData->iCommonData->iTestModuleBase->TestModuleIf().Printf( 0, _L(""),  n); \
    }\
}
				  
				  
#define TRACE(n) \
{ \
	TBuf<255> msg; \
	msg.Format(_L(n)); \
	TestModuleIf().Printf( 0, _L(""),msg); \
} 

#else
#define MSG(n) 
#define TRACE(n)

#endif 



enum ThreadCommand
	{
	ELastCommand,
	EThreadCreate,
	EThreadDestroy,
	EVerifyResult,
	ESetErrno,
	EVerifyErrno,
	ECheckValue,

	EVerifyTimeOutOrPostSignal,

	EStartLog,
	EStopLog,
	ENoOp,

	ELoop,

	EWaitForSignal,
	EPostSignal,
	EWaitTillSuspended,
	
	EBusySpin,
	EVerifySpin,
	EVerifyNoSpin,
	EWaitTillSpin,
	EStopSpin,

    ESemClose,
	ESemDestroy,
	ESemGetValue,
	ESemInit,
	ESemOpen,
	ESemPost,
	ESemTimedWait,
	ESemMicroSleepWait,
	ESemTryWait,
	ESemUnlink,
	ESemWait,
 
	EMutexDestroy,

	EMutexInitNULL,
	EMutexInitDefault,
	EMutexInitErrorCheck,
	EMutexInitRecursive,
		
	EMutexInitStaticDefault,
	EMutexInitStaticErrorCheck,
	EMutexInitStaticRecursive,
	
	EMutexLock,
	EMutexTimedLock,
	EMutexTryLock,
	EMutexUnlock,

	EMutexAttrDestroy,
	EMutexAttrGetType,
	EMutexAttrGetPShared,
	EMutexAttrSetType,
	EMutexAttrSetPShared,
	EMutexAttrInit,

	EOnceInit,
	ECallOnce,
	EMicroSleepTime,
	EErrno,

	ECondBroadcast,
	ECondDestroy,
	ECondInit,
	ECondInitStatic,
	ECondSignal,
	ECondWait,
	ECondTimedWait,
	ECondAttrDestroy,
	ECondAttrInit,

	ESem274Init,
	ESem274Destroy,
	ESem282,
	ESem284B,
	ESem697B,
	ESem698B,
	ESemPostAsNeeded,
	ESem701A,
	ESem701B,

	EMutex330,
	ECond403Init,
	ECond403Destroy,
	ECond452A,
	ECond452B,
	ECond453A,
	ECond453B,
	ECond676A,
	ECond676B,
	ECond677A,
	ECond677B,
	ECond679A,
	ECond679B,
	ECond680A,
	ECond680B,
	ECond681A,
	ECond681B,
	ECond683A,
	ECond683B,



	EPrintSuspendedStatus,
	EFStat,
	EStat,
	EWStat,
	EWriteName,
	EWriteWName,
	EWriteFd,
	EStop,
	ESemPost284
	};

enum HarnessThread
	{
	EThread1,
	EThread2,
	EThread3,
	EThread4,
	EThread5,
	EThreadMain,
	ENoThread,
	};



struct HarnessCommand
	{
	HarnessThread iThread;
	ThreadCommand iAction;
	void*         iArgument;
	};

class CTestModuleBase;

#ifdef HARNESS_LOG
struct HarnessBuffer
	{
	bool before;
	struct HarnessCommand iCommand;
	struct HarnessBuffer* iNext;
	};
#endif


struct CommonData
	{
	pthread_mutex_t* iStaticMutex;
	pthread_mutex_t* iErrorCheckMutex;
	pthread_mutex_t* iRecursiveMutex;
	pthread_cond_t*  iStaticCondVar;
	pthread_once_t   iOnceControl;
	CTestModuleBase* iTestModuleBase;
#ifdef HARNESS_LOG		
	FILE*            iFile;
	pthread_mutex_t  iFileMutex;
	HarnessBuffer*   iHead;
#endif	
	CommonData()
	:iStaticMutex(NULL)
	,iErrorCheckMutex(NULL)
	,iRecursiveMutex(NULL)
	,iStaticCondVar(NULL)
	,iTestModuleBase(NULL)
#ifdef HARNESS_LOG	
	,iHead(NULL)
#endif	
	{
#ifdef	HARNESS_LOG
		int ret;
		iFile = fopen("C:\\Logs\\testframework\\P_Alone.txt","w");
		if(!iFile)
			{
			printf("Unable to open log file!\n");
			}
		ret = pthread_mutex_init(&iFileMutex,NULL);
		if(ret != 0)
			{
			printf("Unable to init file mutex!\n");
			}
#endif		
	}
	
	~CommonData()
	{
#ifdef HARNESS_LOG	
		char buff[255];
		HarnessBuffer* node = iHead;
		HarnessBuffer* temp;
		while(node)
			{
		
			 sprintf(buff,"thread %d command %d\n",node->iCommand.iThread,node->iCommand.iAction);
			 if(node->before)
				{
		 		if(iFile)
					{
		   	 		fprintf(iFile,"+");
					}
				}
			 else
				{
				if(iFile)
					{
		   			fprintf(iFile,"-");
					}
				}
			if(iFile)
				{
		   		fprintf(iFile,buff);	
				}
		  	
		    temp = node;
		    node = node->iNext;
		    delete temp;
			}
		pthread_mutex_destroy(&iFileMutex);
		if(iFile)
			{
			fclose(iFile);	
			}
#endif		
	}

#ifdef HARNESS_LOG
	void BeforeRun(HarnessCommand& aCommand)
		{
		Buffer(aCommand,true);		
		}
	void AfterRun(HarnessCommand& aCommand)
		{
		Buffer(aCommand,false);		
		}

	void Buffer(HarnessCommand& aCommand, bool before)
		{
		HarnessBuffer* node = new HarnessBuffer;
		pthread_mutex_lock(&iFileMutex);
		node->iCommand = aCommand;
		node->iNext = iHead;
		node->before = before;
		iHead = node;
		pthread_mutex_unlock(&iFileMutex);
		}
#endif
	};



struct ThreadData
	{
	sem_t* iSignalSemaphore;
	sem_t* iSuspendSemaphore;

	sem_t*                iTestSemaphore;
	pthread_mutex_t*      iTestMutex;
	pthread_mutexattr_t*  iTestMutexAttr;
	pthread_cond_t*       iTestCondVar;
	pthread_mutexattr_t*  iDefaultAttr;
	pthread_mutexattr_t*  iErrorcheckAttr;
	pthread_mutexattr_t*  iRecursiveAttr;

	pthread_condattr_t*   iCondAttr;
	//bool                  iSpinFlag;
	bool                  iSuspending;
	int                   iSpinCounter;
	HarnessCommand*       iCommandArr;
	int                   iCurrentCommand;
	HarnessThread         iSelf;
	int                   iValue;
	int                   iRetValue;
	int                   ierrno;
	int                   iExpectederrno;
	int                   iTimes;
	bool                  iStopped;

	CommonData*           iCommonData;
	void*                 iInitTwiceData;

	ThreadData*           iTDArr[EThreadMain];
	pthread_t             iIdArr[EThreadMain];

	};

void* StartFn(void* arg);


int NewThread(ThreadData* aThreadData, HarnessThread aThreadId);
int DeleteThread(ThreadData* aThreadData, HarnessThread aThreadId);

int Loop(ThreadData* aThreadData, ThreadCommand aCommand);
int RunCommand(ThreadData* aThreadData, HarnessCommand aCommand);
int ProcessCommands(ThreadData* aThreadData);

void CopyArr(HarnessCommand* aDstArr, HarnessCommand* aSrcArr);
IMPORT_C int LoadHarnessActual(HarnessCommand* aCommandArr, CTestModuleBase* aBase);

#ifdef WINDOWS
#define LoadHarness(x) LoadHarnessActual(x,NULL)
#else
#define LoadHarness(x) LoadHarnessActual(x,NULL)
#endif 

enum TestReturn
	{
	KUnexpectedResult = 1,
	KPrematureError,
	KNoPermission,
	KNoArgument,
	KNotSupported,
	KIllegalCommand,
	KValueMismatch,
	KNoMemory,
	KSpinChanged,
    KSpinNotChanged,
	};

struct semParam_t
	{
	int pshared;
	int value;
	};







#endif // __TestHarness_H__
