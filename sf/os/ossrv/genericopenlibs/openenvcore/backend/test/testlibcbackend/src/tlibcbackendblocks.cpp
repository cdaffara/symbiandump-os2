// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tlibcbackendblocks.cpp
//



 /*
* ==============================================================================
*  Name        : tlibcbackendblocks.cpp
*  Part of     : backend / testlibcbackend
*
*  Description : ?Description
*  Version:   0.5
*
*  Copyright (C) 2002 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing,  adapting or translating, any 
*  or all of this material requires the prior written consent of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
*
* ============================================================================
*/

// INCLUDE FILES
#include <e32svr.h>
#include "tlibcbackend.h"
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdapis/netinet/in.h>
#include <stdapis/arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <semaphore.h>


#define	IN_SET_LOOPBACK_ADDR(a) \
	{ (a)->sin_addr.s_addr  = 0x0100007F; (a)->sin_family = AF_INET; }


const char* const KThreadOneLogFile = "C:\\ThreadOneLog.txt"; 
const char* const KThreadTwoLogFile = "C:\\ThreadTwoLog.txt"; 

const TInt KPortNum 					= 2222;
const TInt KCount 					 	= 10;
const char KFileName[50] 				= "C:\\File.txt";

#ifdef __ARMCC__
const char* const KReadProcess = "C:\\Sys\\Bin\\TPopenReadChild.exe";
const char* const KWriteProcess = "C:\\Sys\\Bin\\TPopenWriteChild.exe";
#endif //__ARMCC__

const short MODE_READ 			 = 1;
const short MODE_WRITE			 = 2;
const short MODE_READWRITE		 = 4;

void* SocketReadThreadEntryPoint( void* aParam );
void* SocketWriteThreadEntryPoint( void* aParam );
void* FileReadThreadEntryPoint( void* aParam );
void* FileWriteThreadEntryPoint( void* aParam );
void* ServerLockThreadEntryPoint( void* aParam );

struct TThreadParam
	{
	TInt iDataCount;
	sem_t iSemaphore;
	FILE* iFp;
	};


//Utility functions
void CTestLibcbackend::ReadIntParam(TInt &aInt)
{
	_LIT( Kparam, "Param%d" );
     TBuf<8> pNameBuf;
     TPtrC string;
     pNameBuf.Format(Kparam,++iParamCnt);
	 TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aInt);
	 if(!res)
	 	{
	 	_LIT(Kerr , "Unable to retrieve integer parameter") ;
	 	INFO_PRINTF1(Kerr);
	  	}
	 return;
}

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CTestLibcbackend::SocketIOTest
// Testing new backend with a scenario like this:
// create two thread which sends/receive data to each other
// This test case can be invoked with below syntax:
// SocketIO <NoOfMsgs:Integer> 
// Example: SocketIO 100
// -----------------------------------------------------------------------------
//
TInt CTestLibcbackend::SocketIOTest()
    {
    TInt dataCount = KCount;
    ReadIntParam( dataCount );
	int retVal = 0;
	pthread_t threadID1 = 0;
	pthread_t threadID2 = 0;
	pthread_attr_t threadAttr;
	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_JOINABLE );
	
	//Create a Read thread now
	retVal = pthread_create( &threadID1, &threadAttr, SocketReadThreadEntryPoint, 
		(void*)dataCount );
	INFO_PRINTF2(_L("ReadThread Creation returned with %d"), retVal);
	if( retVal == 0)
		{
		//Create Write thread
		retVal = pthread_create( &threadID2, &threadAttr, 
		SocketWriteThreadEntryPoint, (void*)dataCount );
		INFO_PRINTF2(_L("WriteThread Creation returned with %d"), retVal);
		//If its joinable thread and waitFlag id set
		if( KErrNone == retVal )
			{
			TInt exitReason = 0;
			retVal = pthread_join(threadID1, (void**)&exitReason );
			INFO_PRINTF3(_L("ReadThread join returned with %d and the exit reason is %d"), retVal, exitReason );
			retVal = pthread_join(threadID2, (void**)&exitReason );
			if (retVal != 0)
				{
				//If the thread fails , kill the first thread 
				pthread_join(threadID1, NULL);
				return KErrGeneral;
				}
			else
				{
				INFO_PRINTF3(_L("WriteThread join returned with %d and the exit reason is %d"), retVal, exitReason );
				}
			}
		}
	return retVal;
    }

// -----------------------------------------------------------------------------
// CTestLibcbackend::FileIOTest
// Testing new backend with a scenario like this:
// create two thread which read/write data to same file
// This test case can be invoked with below syntax:
// FileIO <NoOfFileIO:Integer> 
// Example: FileIO 100
// -----------------------------------------------------------------------------
//
TInt CTestLibcbackend::FileIOTest()
    {
	int retVal = 0;
    TInt dataCount = KCount;
    ReadIntParam( dataCount );
	TThreadParam threadParam;
	threadParam.iDataCount = dataCount;

	if( sem_init( &threadParam.iSemaphore, 0, 0 ) != KErrNone )
		{
		INFO_PRINTF2(_L("Semaphore creation Failed with %d"), errno );
		return KErrGeneral;
		}
	
	FILE* fp = fopen( KFileName, "w+");
	if (fp == NULL)
		{
		INFO_PRINTF1(_L("File open error!"));
		return KErrGeneral;
		}
	threadParam.iFp = fp;
	if(!fp)
		{
		return KErrGeneral;
		}
	
	pthread_t threadID1 = 0;
	pthread_t threadID2 = 0;
	pthread_attr_t threadAttr;
	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_JOINABLE );
	
	//Create a Read thread now
	retVal = pthread_create( &threadID1, &threadAttr, FileReadThreadEntryPoint, 
		(void*)&threadParam );
	INFO_PRINTF2(_L("ReadThread Creation returned with %d"), retVal);
	if( retVal == 0)
		{
		//Create Write thread
		retVal = pthread_create( &threadID2, &threadAttr, 
			FileWriteThreadEntryPoint, (void*)&threadParam );
		INFO_PRINTF2(_L("WriteThread Creation returned with %d"), retVal);
		//If its joinable thread and waitFlag id set
		if( KErrNone == retVal )
			{
			TInt exitReason = 0;
			retVal = pthread_join(threadID1, (void**)&exitReason );
			INFO_PRINTF3(_L("ReadThread join returned with %d and the exit reason is %d"), retVal, exitReason );
			retVal = pthread_join(threadID2, (void**)&exitReason );
			if (retVal != 0)
				{
				//If the thread fails , kill the first thread 
				pthread_join(threadID1, NULL);
				return KErrGeneral;
				}
			else
				{
				INFO_PRINTF3(_L("WriteThread join returned with %d and the exit reason is %d"), retVal, exitReason );
				}
			}
		}

	if( sem_destroy( &threadParam.iSemaphore ) != KErrNone )
		{
		INFO_PRINTF2(_L("sem_destroy Failed with %d"), errno );
		retVal = KErrGeneral;
		}
		
	fclose(fp);
	return retVal;
    }

// -----------------------------------------------------------------------------
// CTestLibcbackend::ServerLockTest
// Testing new backend's Server Session locks with multiple threads
// This test case can be invoked with below syntax:
// ServerLock <NoOfThreads:Integer> 
// Example: ServerLock 10
// -----------------------------------------------------------------------------
//
TInt CTestLibcbackend::ServerLockTest()
	{
	TInt retVal = KErrNone;
    TInt threadCount = KCount;
    ReadIntParam( threadCount );
	pthread_t threadID = 0;
	pthread_attr_t threadAttr;
	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_JOINABLE );
	
	for(int index = 0; index<threadCount && KErrNone == retVal; index++)
		{
		//Create a Read thread now
		retVal = pthread_create( &threadID, &threadAttr, ServerLockThreadEntryPoint, 
			(void*)threadCount );
		INFO_PRINTF2(_L("ServerLockThreadEntryPoint Creation returned with %d"), retVal);
		}
	TInt exitReason = 0;
	retVal = pthread_join(threadID, (void**)&exitReason );
	INFO_PRINTF3(_L("ServerLockThreadEntryPoint join returned with %d and the exit reason is %d"), retVal, exitReason );
	return retVal;
	}

// -----------------------------------------------------------------------------
// CTestLibcbackend::ServerLockTest
// Testing backend implementation of opening a directory
// This test case can be invoked with below syntax:
// OpenDir <Directory to be opened: String> <Mode:Integer> 
// Example: OpenDir c:\Logs\NotExisting MODE_WRITE
// -----------------------------------------------------------------------------
//
TInt CTestLibcbackend::OpenDirTest()
	{
	TInt retVal = KErrNone;
	TInt mode = MODE_READ;
	
	//Read Directory Name and mode from CFG file
	TPtrC dirName;
	_LIT( KaKey, "Param%d" );
     TBuf<8> pNameBuf;
     pNameBuf.Format(KaKey,++iParamCnt);
	 TBool res = GetStringFromConfig(ConfigSection(), pNameBuf, dirName);
	 if(!res)
	 	{
	 	_LIT(Kerr , "Unable to retrieve string parameter") ;
	 	INFO_PRINTF1(Kerr);
	  	}
	ReadIntParam( mode );
	switch( mode )
		{
		case MODE_WRITE:
			{
			mode = O_WRONLY;
			break;
			}
		case MODE_READWRITE:
			{
			mode = O_RDWR;
			break;
			}
		default:
		case MODE_READ:
			{
			mode = O_RDONLY;
			}
		}

	TBuf8<KMaxFileName> tempDir;
	TChar endChar('\0');
	tempDir.Copy(dirName);
	tempDir.Append(endChar);
	char* dir = (char*)tempDir.Ptr();
	
	char* file = "c:\\dircontents.txt";
	int fd = open(dir, mode);
	if( fd > 0 )
		{
		char buff[200];
		int retVal = read(fd, buff, 200);
		if( retVal > 0 )
			{
			FILE* fp  = fopen(file, "w");
			if( fp )
				{
				for(int i=0; i<retVal; i++)
				fprintf(fp, "%c", buff[i]);
				fclose(fp);
				}
			retVal = 0;
			}
		close(fd);
		}
	//It should fail with EISDIR
	else 
		{
		if( errno != EISDIR )
			{
			retVal = fd;
			}
		}
		
	return retVal;
	}

// -----------------------------------------------------------------------------
// CTestLibcbackend::ServerLockTest
// Testing backend implementation of popen API
// This test case can be invoked with below syntax:
// PopenPipe <Mode:Integer> 
// Example: PopenPipe MODE_WRITE
// -----------------------------------------------------------------------------
//
TInt CTestLibcbackend::PopenPipeTest()
	{
	TInt retVal = KErrNone;
	TInt mode = MODE_READ;
	char pipeMode = 'r';
	const char* command = NULL;
	ReadIntParam( mode );
	
//popen works only on device because of Symbian Emulator limitation
#ifdef __ARMCC__
	if ( mode == MODE_WRITE )
		{
		pipeMode = 'w';
		command = KWriteProcess;
		}
	else
		{
		pipeMode = 'r';
		command = KReadProcess;
		}

	FILE* fp = popen( command, &pipeMode);
	if( fp )
		{
		if( mode == MODE_WRITE )
			{
			//Child Pipe Should get this string!!
			fprintf(fp, "PROCESS");
			}
		else
			{
			//Child Pipe Should give this string!!
			char string[50];
			int ret = fscanf(fp, "%s", string);
			}
		//close the pipe
		pclose(fp);
		}
	//Error case
	else
		{
		int err = errno;
		retVal = KErrGeneral;
		}
#endif // __ARMCC__

	return retVal;
	}

// -----------------------------------------------------------------------------
// SocketReadThreadEntryPoint
// Thread Entry Function for Socket read thread
// -----------------------------------------------------------------------------
//
void* SocketReadThreadEntryPoint( void* aParam )
	{
	FILE* logFile;
	TInt retVal = KErrNone;
	TInt count = (TInt) aParam;
	TInt portNum = KPortNum + count;
	int serverFd = 0;
	int newFd = 0;
	size_t addrSize;
	struct sockaddr_in servAddr;
	struct sockaddr_in sockAddr;
	const char* buff = "Server --> Sending Data Item : ";
	char sendBuff[50];
	char recvBuff[100];
	const unsigned int maxRecv = 100;

	logFile = fopen(KThreadOneLogFile, "w");
	
	if(logFile == NULL)
		{
		return (void*) KErrGeneral;
		}

	fprintf(logFile, "Server Socket Thread\n");
	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if( serverFd == -1 )
		{
		fprintf(logFile, "Socket Creation Failed!!\n");
		retVal = KErrGeneral;
		}

	IN_SET_LOOPBACK_ADDR( &servAddr );
	servAddr.sin_port = htons( (TUint16) portNum );
	retVal |= bind( serverFd, (struct sockaddr*)&servAddr, sizeof(servAddr) );
	if( retVal != 0 )
		{
		fprintf(logFile, "Socket bind Failed!!\n");
		close( serverFd );
		retVal = KErrGeneral;
		}

	retVal |= listen( serverFd, 1 );
	if( retVal != 0 )
		{
		fprintf(logFile, "Socket listen Failed!!\n");
		close( serverFd );
		retVal = KErrGeneral;
		}

	addrSize = sizeof( sockAddr );
	newFd = accept( serverFd, (struct sockaddr*)&sockAddr, &addrSize);
	if( newFd == -1 )
		{
		fprintf(logFile, "Socket accept Failed!!\n");
		close( serverFd );
		retVal = KErrGeneral;
		}

	if( KErrNone == retVal )
		{
		//Now Receive and Send some Data
		for(TInt i=1; i<=count; i++)
			{
			TInt ret = recv(newFd, recvBuff, maxRecv, 0);
			if( ret == -1 )
				{
				fprintf(logFile, "Socket recv Failed!!\n");
				close( newFd );
				close( serverFd );
				retVal = KErrGeneral;
				break;
				}
			fprintf(logFile, "Socket received %s\n", recvBuff);
			sprintf(sendBuff, "%s %d", buff, i);
			ret = send(newFd, sendBuff, sizeof(sendBuff), 0);
			if( ret == -1 )
				{
				fprintf(logFile, "Socket send Failed!!\n");
				close( newFd );
				close( serverFd );
				retVal = KErrGeneral;
				break;
				}
			}
		}
	
	if( KErrNone == retVal )
		{
		close( newFd );
		close( serverFd );
		}
	fprintf(logFile, "Server Thread Completed with %d\n", retVal);
	fclose(logFile);
	return (void*)retVal;
	}

// -----------------------------------------------------------------------------
// SocketWriteThreadEntryPoint
// Thread Entry Function for Socket write thread
// -----------------------------------------------------------------------------
//
void* SocketWriteThreadEntryPoint( void* aParam ) 
	{
	FILE* logFile;
	TInt retVal = KErrNone;
	TInt count = (TInt) aParam;

	TInt portNum = KPortNum + count;
	int clientFd;
	size_t addrSize;
	struct sockaddr_in servAddr;
	const char* buff = "Client --> Sending Data Item : ";
	char sendBuff[50];
	char recvBuff[100];
	const unsigned int maxRecv = 100;

	IN_SET_LOOPBACK_ADDR( &servAddr );
	servAddr.sin_port = htons( (unsigned short)portNum );

	logFile = fopen(KThreadTwoLogFile, "w");
	if(logFile == 0)
		{
		return (void*) KErrGeneral;
		}

	fprintf(logFile, "Client Socket Thread\n");

	clientFd = socket(AF_INET, SOCK_STREAM, 0);
	if( clientFd == -1 )
		{
		fprintf(logFile, "Socket creation Failed!!\n");
		retVal = KErrGeneral;
		}
	
	addrSize = sizeof(servAddr);
	retVal = connect(clientFd, (struct sockaddr*)&servAddr, addrSize);
	//This should Pass
	if( retVal == -1 )
		{
		fprintf(logFile, "Socket connect Failed!!\n");
		close(clientFd);
		retVal = KErrGeneral;
		}

	if( KErrNone == retVal )
		{
		//Now Send and Receive some Data
		for(TInt i=1; i<=count; i++)
			{
			sprintf(sendBuff, "%s %d", buff, i);
			TInt ret = send(clientFd, sendBuff, sizeof(sendBuff), 0);
			if(ret == -1)
				{
				fprintf(logFile, "Socket send Failed!!\n");
				close( clientFd );
				retVal = KErrGeneral;
				break;
				}
			ret = recv(clientFd, recvBuff, maxRecv, 0);
			if(ret == -1)
				{
				fprintf(logFile, "Socket recv Failed!!\n");
				close( clientFd );
				retVal = KErrGeneral;
				break;
				}
			fprintf(logFile, "Socket received %s\n", recvBuff);
			}
		}

	if (KErrNone == retVal )
		{
		close( clientFd );
		}
		
	fprintf(logFile, "Client Thread Completed with %d\n", retVal);
	fclose(logFile);
	return (void*)retVal;
	}

// -----------------------------------------------------------------------------
// FileReadThreadEntryPoint
// Thread Entry Function for File read thread
// -----------------------------------------------------------------------------
//
void* FileReadThreadEntryPoint( void* aParam )
	{
	TInt retVal = KErrNone;
	char string[80];
	int count = 0;
	int index = 0;
	TThreadParam* pThreadParam = ( TThreadParam* ) aParam;
	FILE* fp = pThreadParam->iFp;

	for( ;index < pThreadParam->iDataCount; index++ )
		{
		//Wait for the other thread to write some data to file
		retVal = sem_wait(&pThreadParam->iSemaphore);
		if( retVal != 0 )
			{
			retVal = KErrGeneral;
			break;
			}
		fscanf(fp, "%s %d", string, &count);
		}

	return (void*)retVal;	
	}
	
// -----------------------------------------------------------------------------
// FileWriteThreadEntryPoint
// Thread Entry Function for File write thread
// -----------------------------------------------------------------------------
//
void* FileWriteThreadEntryPoint( void* aParam )
	{
	TInt retVal = KErrNone;
	char string[] = "Writing some ASCII Data : ";
	int index = 0;
	TThreadParam* pThreadParam = ( TThreadParam* ) aParam;
	FILE* fp = pThreadParam->iFp;
	//Open the File in write mode

	for( ;index < pThreadParam->iDataCount; index++ )
		{
		fprintf(fp, "%s %d", string, index);
		//Tell the other thread that data is ready for reading
		retVal = sem_post(&pThreadParam->iSemaphore);
		if( retVal != 0 )
			{
			retVal = KErrGeneral;
			break;
			}
		}

	return (void*)retVal;	
	}

// -----------------------------------------------------------------------------
// FileWriteThreadEntryPoint
// Thread Entry Function for File write thread
// -----------------------------------------------------------------------------
//
void* ServerLockThreadEntryPoint( void* /*aParam*/ )
	{
	const char* fileName = "c:\\Logs\\TestReport.txt";
	TInt retVal = KErrNone;
	struct stat buf;
	retVal = stat(fileName, &buf);
	return (void*)retVal;
	}
	
//  End of File
