// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tlocalsocketblocks.cpp
//

#include "tlocalsocket.h"
#include <sys/un.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>

#include <sys/select.h>

#define SERVPATH "c:\\tmp22.a" 
#define SOCK_PATH "c:\\negsocket.txt"
#define SOCK_PATH1 "c:\\negsocket1.txt"
#define DATA "Hello World"
const TUint KMaxLine = 27;
const TUint KDataLength = sizeof(DATA);

// Util function to read from a socket
TInt read1(int newsock_fd,char *line)
    {    
    int ret;    
    int left = KMaxLine;   
    
    while (1)
        {
        
        ret = read(newsock_fd,line,left);
        
        if (ret < 0)
            {
            return -1;
            }
        else if (ret == 0)
            {
            break;
            }
        left -= ret;
        line += ret;
        if (left <= 0)
            {
            break;
            }
        }
    return 0;
    }
    
// Util function to write to a socket
TInt write1(int sock_fd)
    {
    char line[KMaxLine];    
    int x;
    char character = 'A';
    unsigned int cnt = 0;
    for(; cnt < KMaxLine - 1; cnt++)
        {
        line[cnt] = character;
        character++;
        if (character > 'Z')
            {
            character = 'A';
            }
        }
    line[cnt] = '\0';
    x=write(sock_fd,line,KMaxLine);
    if (x < 0)
        {
        return -1;
        }
        
    return 0;
    }
    

static TInt Recv1(int newsock_fd,char *line)
    {    
    int ret;    
    int left = KMaxLine;   
    
    while (1)
        {
        
        ret = recv(newsock_fd,line,left,0);
        
        if (ret < 0)
            {
            return -1;
            }
        else if (ret == 0)
            {
            break;
            }
        left -= ret;
        line += ret;
        if (left <= 0)
            {
            break;
            }
        }
    return 0;
    }
    
// Util function to write to a socket
static TInt Send1(int sock_fd)
    {
    char line[KMaxLine];    
    int x;
    char character = 'A';
    unsigned int cnt = 0;
    for(; cnt < KMaxLine - 1; cnt++)
        {
        line[cnt] = character;
        character++;
        if (character > 'Z')
            {
            character = 'A';
            }
        }
    line[cnt] = '\0';
    x=send(sock_fd,line,KMaxLine,0);
    if (x < 0)
        {
        return -1;
        }
        
    return 0;
    }


// Util function 
static void* TCPThreadWrite(TAny* aPath)
	{
	int sock_fd,len;
		
	struct sockaddr_un remote;
	int tmp;
	memset(&remote, 0x00, sizeof(struct sockaddr_un));    
	remote.sun_family=AF_UNIX;    
	strcpy((char*)&remote.sun_path, (const char *)aPath);
	
    sock_fd = socket(AF_UNIX,SOCK_STREAM,0);
    if (sock_fd < 0)
		{
		goto close;
		}
	len = sizeof(sockaddr_un);
	tmp = connect(sock_fd,(struct sockaddr *)&remote,len);
	if (tmp < 0)
		{
		goto close;
		}
	if (write1(sock_fd) < 0)
		{
		goto close;
		}
close:
	close(sock_fd);
	unlink((const char *)aPath);
	return (void*)NULL;
	}
	
// Util function	
static void* TCPThreadRead(TAny* aPath)
	{
	int sock_fd,len;
	char line[KMaxLine ];
	
	struct sockaddr_un remote;
	int tmp;
	memset(&remote, 0x00, sizeof(struct sockaddr_un));    
	remote.sun_family=AF_UNIX;    
    strcpy((char*)&remote.sun_path, (const char *)aPath);
    sock_fd = socket(AF_UNIX,SOCK_STREAM,0);
    if (sock_fd < 0)
		{
		goto close;
		}
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	tmp = connect(sock_fd,(struct sockaddr *)&remote,len);
	if (tmp < 0)
		{
		goto close;
		}
	if (read1(sock_fd,line) < 0)
		{
		goto close;
		}
close:
	close(sock_fd);
	unlink((const char *)aPath);
	return (void*)NULL;
	}	
	
//Util function to connect to the calling thread
void* ThreadConnect(void*)	
{
	int sockfd = -1;

	struct sockaddr_un dest;
	unlink(SOCK_PATH);
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockfd < 0)
		{
		return (void*)NULL;
		}
	memset(&dest, 0x00, sizeof(struct sockaddr_un));
	dest.sun_family=AF_UNIX; 
	strcpy((char*)&dest.sun_path, SOCK_PATH);   
	unlink(SOCK_PATH);
	if ( bind(sockfd,(struct sockaddr *)&dest,sizeof(dest))  < 0)
		{
		if(sockfd)
			close(sockfd);
		return (void*)NULL;
		}

	strcpy((char*)&dest.sun_path, SERVPATH);
	if(connect(sockfd, (struct sockaddr *)&dest, sizeof(dest)) < 0)
		{
		close(sockfd);
		return (void*)NULL;
		}
	sleep(1);
	if(sockfd)
		close(sockfd);
	return (void*)NULL;
}

//Util function to sendto() a datagram socket
void *ThreadSendto(void *)
{
	int sockfd = -1;
    struct sockaddr_un dest;
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(sockfd < 0)
    	return (void*)NULL;
	memset(&dest, 0x00, sizeof(struct sockaddr_un));    
	dest.sun_family=AF_UNIX;    
	strcpy((char*)&dest.sun_path, SERVPATH);
	socklen_t length = sizeof(struct sockaddr);
	sendto(sockfd, DATA, KDataLength, 0, (struct sockaddr*)&dest, length);
	if(sockfd)
		close(sockfd);
	return (void*)NULL;
}

//Util function to recvfrom() a datagram socket
void *ThreadRecvfrom(void *)
	{
	int sockfd = -1;
	char buf[KDataLength];
	struct sockaddr_un dest,from;
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(sockfd < 0)
    	return (void*)NULL;
    unlink(SERVPATH);
	memset(&dest, 0x00, sizeof(struct sockaddr_un));    
	dest.sun_family=AF_UNIX;    
	strcpy((char*)&dest.sun_path, SERVPATH);
	if ( bind(sockfd,(struct sockaddr *)&dest,sizeof(dest))  < 0)
		{
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return (void*)NULL;
		}

	socklen_t length = sizeof(from);
	recvfrom(sockfd, buf, KDataLength, 0, (struct sockaddr*)&from, &length);
	if(sockfd)
		close(sockfd);
	unlink(SERVPATH);
	return (void*)NULL;
	}

/**
* Function Name		: TestBind
* Description		: bind a local socket
* Return Value		: Test case should return KErrNone
*/
TVerdict CTestLocalSocket::TestBind( )
    {
    INFO_PRINTF1(_L("TestBind"));
    int fd = -1;
    int rslt = -1;
    int sz = 0,err;
    const char path[] = "C:\\TestBind";
    struct sockaddr_un server;
        
    server.sun_family = PF_LOCAL;
    strncpy(server.sun_path, path, strlen(path)+1);
    sz = SUN_LEN(&server);
    
    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0)
    	{
    	INFO_PRINTF2(_L("protocol not supported. Errno is %d"), errno);
    	return TestStepResult();
    	}

    rslt = bind(fd, (struct sockaddr*)&server, sz);
    
    if (rslt < 0)
       {
       INFO_PRINTF2(_L("socket binding error: %d"), errno);
       close(fd);
       unlink(path);
       return TestStepResult();
       }
    err=fcntl(fd, F_SETFL, O_NONBLOCK);
    if(err==-1)
    	{
    	INFO_PRINTF1(_L("fcntl failure"));
    	}
    close(fd);
    unlink(path);
	
    SetTestStepResult(EPass);
    return TestStepResult();
    }

/**
* Function Name		: TestLseek
* Description		: Perform LSeek on a local socket
* Return Value		: Test case should return KErrNone
*/
TVerdict CTestLocalSocket::TestLseek()
	{
	int sock_fd;
	const char path[] = "C:\\TestLSeek";
	struct sockaddr_un serveraddr;
    
    serveraddr.sun_family = PF_LOCAL;
    strncpy(serveraddr.sun_path, path, strlen(path)+1);
	sock_fd=socket(AF_UNIX, SOCK_STREAM, 0);  
    if (sock_fd < 0)
    	{
    	INFO_PRINTF2(_L("protocol not supported. Errno is %d"), errno);
    	return TestStepResult();
        }

    if (lseek(sock_fd,0,SEEK_CUR)<0)
    	{
		INFO_PRINTF1(_L("Lseek on socket success"));
		close(sock_fd);
		unlink(SERVPATH);
		SetTestStepResult(EPass);
    	}
    else
    	{
    	close(sock_fd);
    	unlink(SERVPATH);
    	INFO_PRINTF2(_L("Lseek fails with errno no: %d"), errno);
    	}
    return TestStepResult();
	}

/**
* Function Name		: TestFstat
* Description		: Perform FStat on a local socket
* Return Value		: Test case should return KErrNone
*/	
TVerdict CTestLocalSocket::TestFstat()
	{
	int sock_fd;
	const char path[] = "C:\\TestFStat";
	struct sockaddr_un serveraddr;
    
    serveraddr.sun_family = PF_LOCAL;
    strncpy(serveraddr.sun_path, path, strlen(path)+1);
	sock_fd=socket(AF_UNIX, SOCK_STREAM, 0);  
    if (sock_fd < 0)
    	{
    	INFO_PRINTF2(_L("Socket open failed with errno: %d"), errno);
    	return TestStepResult();
        }

    struct stat buf;
    if(fstat(sock_fd , &buf ) < 0 ) 
    	{
    	INFO_PRINTF1(_L("fstat on socket failure"));
		close(sock_fd);
		unlink(SERVPATH);
    	}
    else
    	{
    	close(sock_fd);
    	unlink(SERVPATH);
    	SetTestStepResult(EPass);
    	}
    return TestStepResult();
	}
	
/**
* Function Name		: TestThreadSocketRead
* Description		: Perform read on a local socket
* Return Value		: Test case should return KErrNone
*/	
TVerdict CTestLocalSocket::TestThreadSocketRead()
	{
	int sock_fd,newsock_fd;	
	int error;            
	unsigned int t;
	struct sockaddr_un serveraddr,remote;
	char line[KMaxLine ];
	sock_fd = socket(AF_UNIX,SOCK_STREAM,0);
	if (sock_fd < 0)
        {
        INFO_PRINTF2(_L("Socket open failed with errno: %d"), errno);
    	return TestStepResult();
        }

	pthread_t testThread;
	int threadRetVal;
	void *threadRetValPtr = (void*)&threadRetVal;
	
	memset(&serveraddr, 0x00, sizeof(struct sockaddr_un));    
	serveraddr.sun_family=AF_UNIX;    
	strcpy((char*)&serveraddr.sun_path, SERVPATH);    
	if ( bind(sock_fd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))  < 0)
		{		
		INFO_PRINTF2(_L("Socket bind failed with errno: %d"), errno);
		goto close;
		}
		
	if(listen(sock_fd,3) < 0)
		{
		ERR_PRINTF2(_L("Listen failed with errno: %d\n"),errno);
		goto close;
		}
	t=sizeof(remote);
	// Create the thread and thread is client code 
	pthread_create(&testThread, NULL, &TCPThreadWrite, (void*)(serveraddr.sun_path));
	
	newsock_fd = accept(sock_fd,(sockaddr*)&remote,&t); 
	if(newsock_fd < 0)
		{
		ERR_PRINTF2(_L("Accept failed %d\n"),errno);
		goto close;
		}
	error = read1(newsock_fd,line);
	if (error < 0)
		{		
		INFO_PRINTF2(_L("read failed with error %d"), error);
		goto close;
		}
	SetTestStepResult(EPass);
close:
	pthread_join(testThread, &threadRetValPtr);
	shutdown(sock_fd,SHUT_RDWR);
	close(sock_fd);
	unlink(SERVPATH);
	return TestStepResult();	
	}


/**
* Function Name		: TestThreadSocketRead
* Description		: Perform read on a local socket
* Return Value		: Test case should return KErrNone
*/	
TVerdict CTestLocalSocket::TestThreadSocketRecv()
	{
	int sock_fd,newsock_fd;	
	int error;            
	unsigned int t;
	struct sockaddr_un serveraddr,remote;
	char line[KMaxLine ];
	sock_fd = socket(AF_LOCAL,SOCK_STREAM,0);
	if (sock_fd < 0)
        {
        INFO_PRINTF2(_L("Socket open failed with errno: %d"), errno);
    	return TestStepResult();
        }

	memset(&serveraddr, 0x00, sizeof(struct sockaddr_un));    
	serveraddr.sun_family=AF_LOCAL;    
	strcpy((char*)&serveraddr.sun_path, SERVPATH);    
	
	pthread_t testThread;
	int threadRetVal;
	void *threadRetValPtr = (void*)&threadRetVal;
		
	if ( bind(sock_fd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))  < 0)
		{		
		INFO_PRINTF2(_L("Socket bind failed with errno: %d"), errno);
		goto close;
		}
		
	if(listen(sock_fd,3) < 0)
		{
		ERR_PRINTF2(_L("Listen failed with errno: %d\n"),errno);
		goto close;
		}
	
	t=sizeof(remote);
	// Create the thread and thread is client code 
	pthread_create(&testThread, NULL, &TCPThreadWrite, (void*)(serveraddr.sun_path));

	newsock_fd = accept(sock_fd,(sockaddr*)&remote,&t); 
	if(newsock_fd < 0)
		{
		INFO_PRINTF2(_L("accept bind failed with errno: %d"), errno);
		goto close;
		}
	error = Recv1(newsock_fd,line);
	if (error < 0)
		{		
		INFO_PRINTF2(_L("read failed with error %d"), error);
		goto close;
		}
	SetTestStepResult(EPass);
close:
	pthread_join(testThread, &threadRetValPtr);
	shutdown(sock_fd,SHUT_RDWR);
	close(sock_fd);
	unlink(SERVPATH);
	return TestStepResult();	
	}
	
/**
* Function Name		: TestThreadSocketWrite
* Description		: Perform write on a local socket
* Return Value		: Test case should return KErrNone
*/	
TVerdict CTestLocalSocket::TestThreadSocketWrite()
	{
	int sock_fd,newsock_fd;	
	int error;            
	unsigned int t;
	struct sockaddr_un serveraddr,remote;
	sock_fd = socket(AF_UNIX,SOCK_STREAM,0);
	if (sock_fd < 0)
        {
        INFO_PRINTF2(_L("Socket open failed with errno: %d"), errno);
    	return TestStepResult();
        }

	memset(&serveraddr, 0x00, sizeof(struct sockaddr_un));    
	serveraddr.sun_family=AF_UNIX;    
	strcpy((char*)&serveraddr.sun_path, SERVPATH);    

	pthread_t testThread;
	int threadRetVal;
	void *threadRetValPtr = (void*)&threadRetVal;
	
	if ( bind(sock_fd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))  < 0)
		{		
		INFO_PRINTF2(_L("BIND failed with errno: %d"), errno);
		goto close;
		}
		
	if(listen(sock_fd,3) < 0)
		{
		ERR_PRINTF2(_L("Listen failed with errno: %d\n"),errno);
		goto close;
		}
	
	t=sizeof(remote);
	// Create the thread and thread is client code 
	pthread_create(&testThread, NULL, &TCPThreadRead, (void*)(serveraddr.sun_path));

	newsock_fd = accept(sock_fd,(sockaddr*)&remote,&t); 
	error = write1(newsock_fd);
	if (error < 0)
		{		
		goto close;
		}
	SetTestStepResult(EPass);
close:
	pthread_join(testThread, &threadRetValPtr);
	shutdown(sock_fd,SHUT_RDWR);
	close(sock_fd);
	unlink(SERVPATH);
	return TestStepResult();	

	}

/**
* Function Name		: TestThreadSocketSend
* Description		: Perform Send on a local socket
* Return Value		: Test case should return KErrNone
*/	
TVerdict CTestLocalSocket::TestThreadSocketSend()
	{
	int sock_fd,newsock_fd;	
	int error;            
	unsigned int t;
	struct sockaddr_un serveraddr,remote;
	sock_fd = socket(AF_LOCAL,SOCK_STREAM,0);
	if (sock_fd < 0)
        {
        INFO_PRINTF2(_L("Socket open failed with errno: %d"), errno);
    	return TestStepResult();
        }

	memset(&serveraddr, 0x00, sizeof(struct sockaddr_un));    
	serveraddr.sun_family=AF_LOCAL;    
	strcpy((char*)&serveraddr.sun_path, SERVPATH);
	
	pthread_t testThread;
	int threadRetVal;
	void *threadRetValPtr = (void*)&threadRetVal;
		
	if ( bind(sock_fd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))  < 0)
		{		
		INFO_PRINTF2(_L("socket bind failed with errno: %d"), errno);
		goto close;
		}
		
	if(listen(sock_fd,3) < 0)
		{
		ERR_PRINTF2(_L("Listen failed with errno: %d\n"),errno);
		goto close;
		}
	
	t=sizeof(remote);
	// Create the thread and thread is client code 
	pthread_create(&testThread, NULL, &TCPThreadRead, (void*)(serveraddr.sun_path));

	newsock_fd = accept(sock_fd,(sockaddr*)&remote,&t);
	if(newsock_fd < 0)
		{
		INFO_PRINTF2(_L("accept failed with errno: %d"), errno);
		goto close;
		}
	error = Send1(newsock_fd);
	if (error < 0)
		{		
		INFO_PRINTF2(_L("Send failed with errno: %d"), errno);
		goto close;
		}
	
	SetTestStepResult(EPass);
close:
	pthread_join(testThread, &threadRetValPtr);
	shutdown(sock_fd,SHUT_RDWR);
	close(sock_fd);
	unlink(SERVPATH);
	return TestStepResult();	

	}


/**
* Function Name		: TestMultProc
* Description		: Perform read and write on a local socket across processes
* Return Value		: Test case should return KErrNone
*/
TVerdict CTestLocalSocket::TestMultProc()
	{
	char *cmd="Z:\\sys\\bin\\tsample.exe";
	int fp,n=0;
	int sock_fd,len;
	char line[KMaxLine ];
    struct sockaddr_un remote;
	
	memset(&remote, 0x00, sizeof(struct sockaddr_un));    
	remote.sun_family=AF_UNIX;    
	strcpy((char*)&remote.sun_path, "c:\\tmp.pipe");
	
    sock_fd = socket(AF_UNIX,SOCK_STREAM,0);
    INFO_PRINTF1(_L("socket in client called"));
    
    
    int fds[3]; 
    int tmp;
    int val;
    fp = popen3(cmd,NULL, NULL, fds);
    if(fp < 0)
    	{
    	INFO_PRINTF2(_L("popen3 failed with errno: %d"), errno);
    	goto close;
    	}
    sleep(1);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	tmp = connect(sock_fd,(struct sockaddr *)&remote,len);
	if (tmp < 0)
		{
		INFO_PRINTF2(_L("connect failed with errno: %d"), errno);
		goto close;
		}   
	
	if (read1(sock_fd,line) < 0)
		{
		INFO_PRINTF2(_L("read failed with errno: %d"), errno);
		goto close;
		}
	
	val=strcmp(line, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");	
	if(val)
		{
		INFO_PRINTF1(_L("string compare failed"));
		}
	else
		{
		SetTestStepResult(EPass);
		}
    INFO_PRINTF2(_L("n=%d"),n);    
	n=errno;
close:
    close(sock_fd);
	unlink(SERVPATH);
    return TestStepResult();	
	}
	
struct TThreadParam
	{
	TInt iDataCount;
	sem_t iSemaphore;
	};

//Util function
void* SocketReadThreadEntryPoint(void* aParam)
	{
	TInt retVal = 0;
	TInt count = 5;
    TThreadParam* pThreadParam = ( TThreadParam* ) aParam;

	TInt portNum = 0;
	int serverFd = 0;
	int newFd = 0;
	size_t addrSize;
	struct sockaddr_in servAddr;
	struct sockaddr_in sockAddr;
	const char* buff = "Server --> Sending Data Item : ";
	char sendBuff[50];
	char recvBuff[100];
	const unsigned int maxRecv = 100;

	serverFd = socket(PF_LOCAL, SOCK_STREAM, 0);
	if( serverFd == -1 )
		{
		retVal = -1;
		}

	(&servAddr)->sin_addr.s_addr  = 0x0100007F;
	(&servAddr)->sin_family = PF_LOCAL;

	servAddr.sin_port = htons( (TUint16) portNum );
	retVal |= bind( serverFd, (struct sockaddr*)&servAddr, sizeof(servAddr) );
	if( retVal != 0 )
		{
		close( serverFd );
		retVal = -1;
		}

	

	addrSize = sizeof( sockAddr );

	//Tell the other thread that data is ready for reading
	retVal = sem_post(&pThreadParam->iSemaphore);
	if( retVal != 0 )
		{
		close( serverFd );
		retVal = -1;
		}

	newFd = accept( serverFd, (struct sockaddr*)&sockAddr, &addrSize);
	if( newFd == -1 )
		{
		close( serverFd );
		retVal = -1;
	    return (void*)retVal;
		}

	if( 0 == retVal )
		{
		//Now Receive and Send some Data
		for(TUint i=1; i<=count; i++)
			{
			TInt ret = recv(newFd, recvBuff, maxRecv, 0);
			if( ret == -1 )
				{
				close( newFd );
				close( serverFd );
				retVal = -1;
				break;
				}
			sprintf(sendBuff, "%s %d", buff, i);
			ret = send(newFd, sendBuff, sizeof(sendBuff), 0);
			if( ret == -1 )
				{
				close( newFd );
				close( serverFd );
				retVal = -1;
				break;
				}
			}
		}

	if( 0 == retVal )
		{
		close( newFd );
		close( serverFd );
		}
	return (void*)retVal;
	}

//Util function
void* SocketWriteThreadEntryPoint(void* aParam)
	{
	TInt retVal = 0;
	TInt count = 5;
    TThreadParam* pThreadParam = ( TThreadParam* ) aParam;

	TInt portNum = 0;
	int clientFd;
	size_t addrSize;
	struct sockaddr_in servAddr;
	const char* buff = "Client --> Sending Data Item : ";
	char sendBuff[50];
	char recvBuff[100];
	const unsigned int maxRecv = 100;

	(&servAddr)->sin_addr.s_addr  = 0x0100007F;
	(&servAddr)->sin_family = PF_LOCAL;

	servAddr.sin_port = htons( portNum );

	clientFd = socket(PF_LOCAL, SOCK_STREAM, 0);
	if( clientFd == -1 )
		{
		retVal = -1;
		}

    //Wait for the other thread to write some data to file
    retVal = sem_wait(&pThreadParam->iSemaphore);
    if( retVal != 0 )
    	{
    	close(clientFd);
    	retVal = -1;
	    return (void*)retVal;
    	}

	addrSize = sizeof(servAddr);
	retVal = connect(clientFd, (struct sockaddr*)&servAddr, addrSize);
	if( retVal == -1 )
		{
		close(clientFd);
		retVal = -1;
		}

	if( 0 == retVal )
		{
		//Now Send and Receive some Data
		for(TUint i=1; i<=count; i++)
			{
			sprintf(sendBuff, "%s %d", buff, i);
			TInt ret = send(clientFd, sendBuff, sizeof(sendBuff), 0);
			if(ret == -1)
				{
				close( clientFd );
				retVal = -1;
				break;
				}
			ret = recv(clientFd, recvBuff, maxRecv, 0);
			if(ret == -1)
				{
				close( clientFd );
				retVal = -1;
				break;
				}
			}
		}

	if (0 == retVal )
		{
		close( clientFd );
		}

	return (void*)retVal;
	}

/**
* Function Name		: TestMultThread
* Description		: Perform read and write on a local socket across threads
* Return Value		: Test case should return KErrNone
*/
TVerdict CTestLocalSocket::TestMultThread()
	{
	int retVal = 0;
	
	
    TThreadParam threadParam;

	if( sem_init( &threadParam.iSemaphore, 0, 0 ) != 0 )
    	{
    	
    	//return -1;
    	return TestStepResult();
    	}

	pthread_t threadID1 = 0;
	pthread_t threadID2 = 0;
	pthread_attr_t threadAttr;
	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_JOINABLE );

	//Create a Read thread now
	retVal = pthread_create( &threadID1, &threadAttr, SocketReadThreadEntryPoint,
		(void*)&threadParam );
	
	if( retVal == 0)
		{
		//Create Write thread
		retVal = pthread_create( &threadID2, &threadAttr,
			SocketWriteThreadEntryPoint, (void*)&threadParam );
		
		//If its joinable thread and waitFlag id set
		if( 0 == retVal )
			{
			TInt exitReason = 0;
			retVal = pthread_join(threadID1, (void**)&exitReason );
			
			retVal = pthread_join(threadID2, (void**)&exitReason );
			
			}
		}

	if( sem_destroy( &threadParam.iSemaphore ) != 0 )
		{
		return TestStepResult();
		//retVal = -1;
		}

	SetTestStepResult(EPass);
	return TestStepResult();
	}

		

/*
* Testcase		: TestSetGetSockOpt
* Description	: Test setsockopt for different values of option_name
*					1. SO_SNDBUF
*					2. SO_RCVBUF
*					3. SO_DEBUG
*				  and upon succes of setsockopt, test getsockopt for the same
* Result		: KErrNone
*/ 
TVerdict CTestLocalSocket::TestSetGetSockOpt()
	{
	INFO_PRINTF1(_L("TestSetGetSockOpt"));
	int fd;
	int sz = 0, optionName = 0;
	int rslt;
	const char path[] = "C:\\TestSetGetSockOpt";
	struct sockaddr_un server;
        
    server.sun_family = PF_LOCAL;
    strncpy(server.sun_path, path, strlen(path)+1);
    sz = SUN_LEN(&server);
    
    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0)
    	{
    	INFO_PRINTF2(_L("protocol not supported. Errno is %d"), errno);
    	return TestStepResult();
    	}
   	   	
   	rslt = bind(fd, (struct sockaddr*)&server, sz);
    
    if (rslt)
       	{
       	INFO_PRINTF2(_L("socket binding error: %d"), errno);
       	unlink(path);
		close(fd);
       	return TestStepResult();
       	}
       
    int optionValue = KMaxLine;
    _LIT( KOptionName, "OptionName" );
	rslt = GetIntFromConfig(ConfigSection(), KOptionName, optionName);
	if(!rslt)
		{
	 	INFO_PRINTF1(_L("Could not read option name from ini file"));
	 	close(fd);
		unlink(path);
	 	return TestStepResult();
		}    
    
    rslt = setsockopt(fd, SOL_SOCKET, optionName, &optionValue, sizeof(optionValue));
	if(rslt < 0)
		{
		INFO_PRINTF2(_L("Setsockopt fails with error no: %d"), errno);
		}
	else
		{
		INFO_PRINTF1(_L("Setsockopt successful"));
		
		unsigned int optionLen = sizeof(optionValue);
  		int readOptionValue = 0;
    	rslt = getsockopt(fd, SOL_SOCKET, optionName, (void *)&readOptionValue, &optionLen);
    
    	if(rslt < 0)
			{
			INFO_PRINTF2(_L("Getsockopt fails with error no: %d"), errno);
			}
		else
			{
			INFO_PRINTF1(_L("Getsockopt successful"));
			SetTestStepResult(EPass);
			}
		}
	
	close(fd);
	unlink(path);
	return TestStepResult();
	}

/*
* Testcase		: TestSetSockOptNegative1
* Description	: Test setsockopt for an invalid value of option_name
* Result		: setsockopt returns setting errno to EINVAL, 
*				  Test case returns KErrNone
*/
TVerdict CTestLocalSocket::TestSetSockOptNegative1()
	{
	INFO_PRINTF1(_L("TestSetSockOptNegative1"));
	int fd;
	int sz = 0, optionName = 0;
	int rslt;
	const char path[] = "C:\\TestSetSockOptNegative1";
	struct sockaddr_un server;
        
    server.sun_family = PF_LOCAL;
    strncpy(server.sun_path, path, strlen(path)+1);
    sz = SUN_LEN(&server);
    
    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0)
    	{
    	INFO_PRINTF2(_L("protocol not supported. Errno is %d"), errno);
    	return TestStepResult();
    	}
   	   	
   	rslt = bind(fd, (struct sockaddr*)&server, sz);
    
    if (rslt)
       {
       INFO_PRINTF1(_L("socket binding error"));
       return TestStepResult(); 
       }
       
    int optionValue = KMaxLine;
    _LIT( KOptionName, "OptionName" );
	rslt = GetIntFromConfig(ConfigSection(), KOptionName, optionName);
	if(!rslt)
		{
	 	INFO_PRINTF1(_L("Could not read option name from ini file"));
	 	close(fd);
		unlink(path);
	 	return TestStepResult();
		}    
    
    rslt = setsockopt(fd, SOL_SOCKET, optionName, &optionValue, sizeof(optionValue));
	if(rslt < 0 && errno == ENOPROTOOPT)
		{
		INFO_PRINTF1(_L("setsockopt fails with right error value"));
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("Setsockopt fails with error no: %d"), errno);
		}
	
	close(fd);
	unlink(path);
	return TestStepResult();
	}
/*
* Testcase		: TestSetSockOptNegative2
* Description	: Test setsockopt for invalid value of socklen_t
* Result		: setsockopt returns -1 setting errno to EINVAL 
*				  Test case returns KErrNone
*/	
TVerdict CTestLocalSocket::TestSetSockOptNegative2()
	{
	INFO_PRINTF1(_L("TestSetSockOptNegative2"));
	int fd;
	int sz = 0;
	int rslt;
	const char path[] = "C:\\TestSetSockOptNegative2";
	struct sockaddr_un server;
        
    server.sun_family = PF_LOCAL;
    strncpy(server.sun_path, path, strlen(path)+1);
    sz = SUN_LEN(&server);
    
    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0)
    	{
    	INFO_PRINTF2(_L("protocol not supported. Errno is %d"), errno);
    	return TestStepResult();
    	}
   	   	
   	rslt = bind(fd, (struct sockaddr*)&server, sz);
    
    if (rslt)
       {
       INFO_PRINTF1(_L("socket binding error"));
       return TestStepResult();
       }
       
    int optionValue = KMaxLine;
    int sockLen=0;
    
    _LIT( KSocketLen, "SocketLen" );
	rslt = GetIntFromConfig(ConfigSection(), KSocketLen, sockLen);
	if(!rslt)
		{
	 	INFO_PRINTF1(_L("Could not read option name from ini file"));
	 	close(fd);
		unlink(path);
	 	return TestStepResult();
		}    
    rslt = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &optionValue, sockLen);
    if(rslt < 0 && errno == EINVAL)
		{
		INFO_PRINTF1(_L("Setsockopt fails with right error value"));
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("Setsockopt fails with error no: %d"), errno);
		}
	
	close(fd);
	unlink(path);
	return TestStepResult();
	}

/*
* Testcase		: TestSetSockOptNegative3
* Description	: Test setsockopt for value of level other than SOL_SOCKET
* Result		: setsockopt returns EINVAL, test case returns KErrNone
*/	
TVerdict CTestLocalSocket::TestSetSockOptNegative3()
	{
	INFO_PRINTF1(_L("TestSetSockOptNegative3"));
	int fd;
	int sz = 0;
	int rslt;
	const char path[] = "C:\\TestSetSockOptNegative3";
	struct sockaddr_un server;
        
    server.sun_family = PF_LOCAL;
    strncpy(server.sun_path, path, strlen(path)+1);
    sz = SUN_LEN(&server);
    
    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0)
    	{
    	INFO_PRINTF2(_L("protocol not supported. Errno is %d"), errno);
    	return TestStepResult();
    	}
   	   	
   	rslt = bind(fd, (struct sockaddr*)&server, sz);
    
    if (rslt)
       {
       INFO_PRINTF1(_L("socket binding error"));
       return TestStepResult();
       }
       
    int optionValue = KMaxLine;
      
    // pass invalid level = 3
    rslt = setsockopt(fd, 3, SO_SNDBUF, &optionValue, sizeof(optionValue));
        
    if(rslt < 0 && errno == ENOPROTOOPT)
		{
		INFO_PRINTF1(_L("Setsockopt fails with right error value"));
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("Setsockopt fails with error no: %d"), errno);
		}
	
	close(fd);
	unlink(path);
	return TestStepResult();
	}

/*
* Testcase		: TestSetSockOptNegative4
* Description	: Test setsockopt for option_value=NULL
* Result		: setsockopt returns EFAULT, test case returns KErrNone
*/	
TVerdict CTestLocalSocket::TestSetSockOptNegative4()
	{
	INFO_PRINTF1(_L("TestSetSockOptNegative4"));
	int fd;
	int sz = 0;
	int rslt;
	const char path[] = "C:\\TestSetSockOptNegative4";
	struct sockaddr_un server;
        
    server.sun_family = PF_LOCAL;
    strncpy(server.sun_path, path, strlen(path)+1);
    sz = SUN_LEN(&server);
    
    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0)
    	{
    	INFO_PRINTF2(_L("protocol not supported. Errno is %d"), errno);
    	return TestStepResult();
    	}
   	   	
   	rslt = bind(fd, (struct sockaddr*)&server, sz);
    
    if (rslt)
       {
       INFO_PRINTF1(_L("socket binding error"));
       return TestStepResult();
       }
       
    rslt = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, NULL, NULL);
        
    if(rslt < 0 && errno == EFAULT)
		{
		INFO_PRINTF1(_L("Setsockopt fails with right error value"));
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("Setsockopt fails with error no: %d"), errno);
		}
	
	close(fd);
	unlink(path);
	return TestStepResult();
	}

/*
* Testcase		: TestGetSockOptNegative1
* Description	: Test getsockopt for an invalid value of option_name
* Result		: getsockopt returns EINVAL, test case returns KErrNone
*/	
TVerdict CTestLocalSocket::TestGetSockOptNegative1()
	{
	INFO_PRINTF1(_L("TestGetSockOptNegative1"));
	int fd;
	int sz = 0;
	int rslt;
	const char path[] = "C:\\TestGetSockOptNegative1";
	struct sockaddr_un server;
        
    server.sun_family = PF_LOCAL;
    strncpy(server.sun_path, path, strlen(path)+1);
    sz = SUN_LEN(&server);
    
    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0)
    	{
    	INFO_PRINTF2(_L("protocol not supported. Errno is %d"), errno);
    	return TestStepResult();
    	}
   	   	
   	rslt = bind(fd, (struct sockaddr*)&server, sz);
    
    if (rslt)
       {
       INFO_PRINTF1(_L("socket binding error"));
       return TestStepResult();
       }
       
    int optionValue = KMaxLine;
    rslt = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &optionValue, sizeof(optionValue));
	if(rslt < 0)
		{
		INFO_PRINTF2(_L("Setsockopt fails with error no: %d"), errno);
		}
	else
		{
		INFO_PRINTF1(_L("Setsockopt successful"));
		
		unsigned int optionLen = sizeof(optionValue);
  		int readOptionValue = 0;
  		int invalidOptName = 0;
  		 _LIT( KOptionName, "OptionName" );
		rslt = GetIntFromConfig(ConfigSection(), KOptionName, invalidOptName);
		if(!rslt)
			{
	 		INFO_PRINTF1(_L("Could not read option name from ini file"));
	 		close(fd);
			unlink(path);
	 		return TestStepResult();
			}  
    	rslt = getsockopt(fd, SOL_SOCKET, invalidOptName, (void *)&readOptionValue, &optionLen);
    
    	if(rslt < 0 && errno == ENOPROTOOPT)
			{
			INFO_PRINTF1(_L("getsockopt fails with right error value"));
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF2(_L("getsockopt fails with error no: %d"), errno);
			}
		}
	
	close(fd);
	unlink(path);
	return TestStepResult();
	}

/*
* Testcase		: TestGetSockOptNegative2
* Description	: Test getsockopt for option_value=NULL
* Result		: getsockopt returns EFAULT, test case returns KErrNone
*/	
TVerdict CTestLocalSocket::TestGetSockOptNegative2()
	{
	INFO_PRINTF1(_L("TestGetSockOptNegative2"));
	int fd;
	int sz = 0;
	int rslt;
	const char path[] = "C:\\TestGetSockOptNegative2";
	struct sockaddr_un server;
        
    server.sun_family = PF_LOCAL;
    strncpy(server.sun_path, path, strlen(path)+1);
    sz = SUN_LEN(&server);
    
    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0)
    	{
    	INFO_PRINTF2(_L("protocol not supported. Errno is %d"), errno);
    	return TestStepResult();
    	}
   	   	
   	rslt = bind(fd, (struct sockaddr*)&server, sz);
    
    if (rslt)
       {
       INFO_PRINTF1(_L("socket binding error"));
       return TestStepResult();
       }
       
    int optionValue = KMaxLine;
    rslt = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &optionValue, sizeof(optionValue));
	if(rslt < 0)
		{
		INFO_PRINTF2(_L("Setsockopt fails with error no: %d"), errno);
		}
	else
		{
		INFO_PRINTF1(_L("Setsockopt successful"));
		
		unsigned int optionLen = sizeof(optionValue);
  		  		 		
    	rslt = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, NULL, &optionLen);
    
    	if(rslt < 0 && errno == EFAULT)
			{
			INFO_PRINTF1(_L("getsockopt fails with right error value"));
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF2(_L("getsockopt fails with error no: %d"), errno);
			}
		}
	
	close(fd);
	unlink(path);
	return TestStepResult();
	}

/*
* Testcase		: TestGetSockOptNegative3
* Description	: Test getsockopt for invalid value of socklen_t
* Result		: getsockopt returns EINVAL, test case returns KErrNone
*/	
TVerdict CTestLocalSocket::TestGetSockOptNegative3()
	{
	INFO_PRINTF1(_L("TestGetSockOptNegative3"));
	int fd;
	int sz = 0;
	int rslt;
	const char path[] = "C:\\TestGetSockOptNegative3";
	struct sockaddr_un server;
        
    server.sun_family = PF_LOCAL;
    strncpy(server.sun_path, path, strlen(path)+1);
    sz = SUN_LEN(&server);
    
    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0)
    	{
    	INFO_PRINTF2(_L("protocol not supported. Errno is %d"), errno);
    	return TestStepResult();
    	}
   	   	
   	rslt = bind(fd, (struct sockaddr*)&server, sz);
    
    if (rslt)
       {
       INFO_PRINTF1(_L("socket binding error"));
       return TestStepResult();
       }
       
    int optionValue = KMaxLine;
    rslt = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &optionValue, sizeof(optionValue));
	if(rslt < 0)
		{
		INFO_PRINTF2(_L("Setsockopt fails with error no: %d"), errno);
		}
	else
		{
		INFO_PRINTF1(_L("Setsockopt successful"));
		
		int readOptionValue = 0;
		unsigned int sockLen = 0;
  		
  	   	rslt = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void *)&readOptionValue, &sockLen);
    
	    if(rslt < 0 && errno == EINVAL)
			{
			INFO_PRINTF1(_L("getsockopt fails with right error value"));
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF2(_L("getsockopt fails with error no: %d"), errno);
			}
		}
	
	close(fd);
	unlink(path);
	return TestStepResult();
	}

/*
* Testcase		: TestGetSockOptNegative4
* Description	: Test setsockopt for value of level other than SOL_SOCKET
* Result		: setsockopt returns EINVAL, test case returns KErrNone
*/	
TVerdict CTestLocalSocket::TestGetSockOptNegative4()
	{
	INFO_PRINTF1(_L("TestGetSockOptNegative4"));
	int fd;
	int sz = 0;
	int rslt;
	const char path[] = "C:\\TestGetSockOptNegative4";
	struct sockaddr_un server;
        
    server.sun_family = PF_LOCAL;
    strncpy(server.sun_path, path, strlen(path)+1);
    sz = SUN_LEN(&server);
    
    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0)
    	{
    	INFO_PRINTF2(_L("protocol not supported. Errno is %d"), errno);
    	return TestStepResult();
    	}

   	rslt = bind(fd, (struct sockaddr*)&server, sz);
    
    if (rslt)
       {
       INFO_PRINTF1(_L("socket binding error"));
       return TestStepResult();
       }
       
    int optionValue = KMaxLine;
    rslt = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &optionValue, sizeof(optionValue));
	if(rslt < 0)
		{
		INFO_PRINTF2(_L("Setsockopt fails with error no: %d"), errno);
		}
	else
		{
		INFO_PRINTF1(_L("Setsockopt successful"));
		
		unsigned int optionLen = sizeof(optionValue);
		int readOptionValue = 0;
				  		
  		// pass invalid level = 3
  	   	rslt = getsockopt(fd, 3, SO_SNDBUF, (void *)&readOptionValue, &optionLen);
    
	    if(rslt < 0 && errno == ENOPROTOOPT)
			{
			INFO_PRINTF1(_L("getsockopt fails with right error value"));
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF2(_L("getsockopt fails with error no: %d"), errno);
			}
		}
	
	close(fd);
	unlink(path);
	return TestStepResult();
	}
/*
* Testcase		: TestLocalSockIoctl
* Description	: Test ioctl for localsocket
* Result		: ioctl retuns -1 settign errno to ENOSYS
*				  Test case returns KErrNone
**/
TVerdict CTestLocalSocket::TestLocalSockIoctl()
	{
	INFO_PRINTF1(_L("TestLocalSockIoctl"));
	int fd;
	int rslt=0;
	const char path[] = "C:\\TestLocalSockIoctl";
	struct sockaddr_un server;
	TRequestStatus status;
        
    server.sun_family = PF_LOCAL;
    strncpy(server.sun_path, path, strlen(path)+1);
    SUN_LEN(&server);
    
    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0)
    	{
    	INFO_PRINTF2(_L("protocol not supported. Errno is %d"), errno);
    	return TestStepResult();
    	}
	
	rslt = ioctl(fd, SIOCSIFNAME, &status);
	
	if (rslt < 0 && errno == ENOSYS)
		{
		INFO_PRINTF1(_L("ioctl successful"));
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("ioctl fails with error number: %d"), errno);
		}
		
	close(fd);
	unlink(path);
	return TestStepResult();
	}
	
/*
* Testcase		: TestLocalSockFcntl
* Description	: Test fcntl for localsocket
* Result		: fcntl retuns 0, test case returns KErrNone
**/
TVerdict CTestLocalSocket::TestLocalSockFcntl()
	{
	INFO_PRINTF1(_L("TestLocalSockFcntl"));
    int fd = -1;
    int rslt = -1;
    int sz,err = 0;
    const char path[] = "C:\\TestLocalSockFcntl";
    struct sockaddr_un server;
        
    server.sun_family = PF_LOCAL;
    strncpy(server.sun_path, path, strlen(path)+1);
    sz = SUN_LEN(&server);
    
    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
   
    if (fd < 0)
    	{
    	INFO_PRINTF2(_L("protocol not supported. Errno is %d"), errno);
    	return TestStepResult();
    	}

    rslt = bind(fd, (struct sockaddr*)&server, sz);
    
    if (rslt)
       {
       INFO_PRINTF1(_L("socket binding error"));
       return TestStepResult();
       }
    
    int fcntlGetFlag=0;
    _LIT(KFcntlGetFlag, "FcntlGetFlag");
    
    rslt = GetIntFromConfig(ConfigSection(), KFcntlGetFlag, fcntlGetFlag);
    if(!rslt)
		{
	 	INFO_PRINTF1(_L("Could not read from ini file"));
	 	close(fd);
		unlink(path);
	 	return TestStepResult();
		}
    
    int fcntlSetFlag = 0;
    _LIT(KFcntlSetFlag, "FcntlSetFlag");

    rslt = GetIntFromConfig(ConfigSection(), KFcntlSetFlag, fcntlSetFlag);
    if(!rslt)
    	{
    	INFO_PRINTF1(_L("Could not read from ini file"));
    	close(fd);
    	unlink(path);
    	return TestStepResult();
    	}
    
    int fcntlArg=0;
    _LIT(KFcntlArg, "FcntlArg");
    
    rslt = GetIntFromConfig(ConfigSection(), KFcntlArg, fcntlArg);
    if(!rslt)
    	{
    	INFO_PRINTF1(_L("Could not read FcntlArg from ini file"));
    	close(fd);
    	unlink(path);
    	return TestStepResult();
    	}
   
    int currentFlag = 0;
    currentFlag=fcntl(fd, fcntlGetFlag, 0);
    if(currentFlag == -1)
    	{
    	INFO_PRINTF2(_L("fcntl fails with %d"), errno);
    	close(fd);
    	unlink(path);
    	return TestStepResult();
    	}
        
    err=fcntl(fd, fcntlSetFlag, fcntlArg);
    if(err == -1)
    	{
    	INFO_PRINTF2(_L("fcntl fails with %d"), errno);
    	close(fd);
    	unlink(path);
    	return TestStepResult();
    	}
    currentFlag=fcntl(fd, fcntlGetFlag, 0);
    if(currentFlag == -1)
    	{
    	INFO_PRINTF2(_L("fcntl fails with %d"), errno);
    	close(fd);
    	unlink(path);
    	return TestStepResult();
    	}
   
    int result=0;
    _LIT(KResult, "Result");

    rslt = GetIntFromConfig(ConfigSection(), KResult, result);
    if(!rslt)
    	{
    	INFO_PRINTF1(_L("Could not read Result from ini file"));
    	close(fd);
    	unlink(path);
    	return TestStepResult();
    	}
    
    if(currentFlag & fcntlArg != result)
    	{
    	INFO_PRINTF1(_L("fcntl set failed "));
    	close(fd);
    	unlink(path);
    	return TestStepResult();
    	}
    close(fd);
    unlink(path);
    SetTestStepResult(EPass);
    return TestStepResult(); 
	}
	

/**
* Function Name		: TestNegativeBind1
* Description		: Negative test on UNIX sockets, with this combination unlink(),bind(),unlink(),bind()
* Return Value		: First bind() should be success and second bind() should fail with EINVAL, since it is already bound
* API Tested		: bind() 
*/	

TVerdict CTestLocalSocket::TestNegativeBind1( )
    { 
    int SocketFamily, ret, fd, s = -1, len;
    struct sockaddr_un local;
	ret = GetIntFromConfig(ConfigSection(), _L("SocketFamily"), SocketFamily);
	if(ret != 1)
		{
	 	ERR_PRINTF1(_L("Could not read the socket family name from ini file"));
	 	goto close;
		} 
    local.sun_family = SocketFamily;
    if(sizeof(local.sun_path) < strlen(SOCK_PATH))
    	{
    	ERR_PRINTF1(_L("The length of the path given exceeds the maximum limit"));
    	goto close;
    	}
	s = socket(SocketFamily, SOCK_STREAM, 0);
    if (s == -1) 
    	{
    	ERR_PRINTF2(_L("Error in creating the socket and errno is %d"),errno);
    	goto close;
    	}

    strcpy(local.sun_path, SOCK_PATH);
    unlink(SOCK_PATH);
    fd = open(SOCK_PATH, O_CREAT | O_RDWR, 0666);
    if (fd < 0)
		{
		ERR_PRINTF2(_L("Error in creating the file and errno is %d"),errno);
		goto close;
		}
    close(fd);
    unlink(SOCK_PATH);
    len = sizeof(struct sockaddr_un);
    ret = bind(s, (struct sockaddr *)&local, len);
    if (ret != 0) 
    	{
    	ERR_PRINTF2(_L("Bind1 failed and errno is set to %d"),errno);
    	goto close;
    	}
    INFO_PRINTF1(_L("Successfully able to bind for the first"));
    unlink(SOCK_PATH);
    ret = bind(s, (struct sockaddr *)&local, len);
    if ((ret != -1) || (errno != EINVAL)) 
     	{
     	ERR_PRINTF2(_L("Bind2 failed to return EINVAL and errno is set to %d"),errno);
     	goto close;
     	}
    INFO_PRINTF1(_L("bind() successfully returned EINVAL when called for the second time"));
    
    SetTestStepResult(EPass);
    close:
    if(s != -1)
    	{
    	close(s);
    	}
    unlink(SOCK_PATH);
    return TestStepResult();
    }

/**
* Function Name		: TestNegativeBind2
* Description		: Negative test on UNIX sockets, with this combination unlink(),bind(),bind()
* Return Value		: First bind() should be success and second bind() should fail with EINVAL, since it is already bound
* API Tested		: bind() 
*/	

TVerdict CTestLocalSocket::TestNegativeBind2( )
    { 
    int SocketFamily, ret, fd, s = -1, len;
    struct sockaddr_un local;
	ret = GetIntFromConfig(ConfigSection(), _L("SocketFamily"), SocketFamily);
	if(ret != 1)
		{
	 	ERR_PRINTF1(_L("Could not read the socket family name from ini file"));
	 	goto close;
		} 
    local.sun_family = SocketFamily;
    if(sizeof(local.sun_path) < strlen(SOCK_PATH))
    	{
    	ERR_PRINTF1(_L("The length of the path given exceeds the maximum limit"));
    	goto close;
    	}
	s = socket(SocketFamily, SOCK_STREAM, 0);
    if (s == -1) 
    	{
    	ERR_PRINTF2(_L("Error in creating the socket and errno is %d"),errno);
    	goto close;
    	}

    strcpy(local.sun_path, SOCK_PATH);
    unlink(SOCK_PATH);
    fd = open(SOCK_PATH, O_CREAT | O_RDWR, 0666);
    if (fd < 0)
		{
		ERR_PRINTF2(_L("Error in creating the file and errno is %d"),errno);
		goto close;
		}
    close(fd);
    unlink(SOCK_PATH);
    len = sizeof(struct sockaddr_un);
    ret = bind(s, (struct sockaddr *)&local, len);
    if (ret != 0) 
    	{
    	ERR_PRINTF2(_L("Bind1 failed and errno is set to %d"),errno);
    	goto close;
    	}
    INFO_PRINTF1(_L("Successfully able to bind for the first"));
    ret = bind(s, (struct sockaddr *)&local, len);
    if ((ret != -1) || (errno != EINVAL)) 
     	{
     	ERR_PRINTF2(_L("Bind2 failed to return EINVAL and errno is set to %d"),errno);
     	goto close;
     	}
    INFO_PRINTF1(_L("bind() successfully returned EINVAL when called for the second time"));
    
    SetTestStepResult(EPass);
    close:
    if(s != -1)
    	{
    	close(s);
    	}
    unlink(SOCK_PATH);
    return TestStepResult();
    }

/**
* Function Name		: TestNegativeBind3
* Description		: Negative test on UNIX sockets, with this combination bind(),unlink(),bind()
* Return Value		: First bind() should fail with EADDRINUSE and second bind() should be success
* API Tested		: bind() 
*/	

TVerdict CTestLocalSocket::TestNegativeBind3( )
    { 
    int SocketFamily, ret, fd, s = -1, len;
    struct sockaddr_un local;
	ret = GetIntFromConfig(ConfigSection(), _L("SocketFamily"), SocketFamily);
	if(ret != 1)
		{
	 	ERR_PRINTF1(_L("Could not read the socket family name from ini file"));
	 	goto close;
		} 
    local.sun_family = SocketFamily;
    if(sizeof(local.sun_path) < strlen(SOCK_PATH))
    	{
    	ERR_PRINTF1(_L("The length of the path given exceeds the maximum limit"));
    	goto close;
    	}
	s = socket(SocketFamily, SOCK_STREAM, 0);
    if (s == -1) 
    	{
    	ERR_PRINTF2(_L("Error in creating the socket and errno is %d"),errno);
    	goto close;
    	}

    strcpy(local.sun_path, SOCK_PATH);
    unlink(SOCK_PATH);
    fd = open(SOCK_PATH, O_CREAT | O_RDWR, 0666);
    if (fd < 0)
		{
		ERR_PRINTF2(_L("Error in creating the file and errno is %d"),errno);
		goto close;
		}
    close(fd);
    len = sizeof(struct sockaddr_un);
    ret = bind(s, (struct sockaddr *)&local, len);
    if ((ret != -1) || (errno != EADDRINUSE)) 
    	{
    	ERR_PRINTF2(_L("Bind1 failed to return EADDRINUSE and errno is set to %d"),errno);
    	goto close;
    	}
    INFO_PRINTF1(_L("Bind1 successfully returned EADDRINUSE when called for the first time"));
    unlink(SOCK_PATH);
    ret = bind(s, (struct sockaddr *)&local, len);
    if (ret != 0) 
     	{
     	ERR_PRINTF2(_L("Bind2 failed and errno is set to %d"),errno);
     	goto close;
     	}
    INFO_PRINTF1(_L("Successfully able to bind for the second time"));
    
    SetTestStepResult(EPass);
    close:
    if(s != -1)
    	{
    	close(s);
    	}
    unlink(SOCK_PATH);
    return TestStepResult();
    }

/**
* Function Name		: TestNegativeBind4
* Description		: Negative test on UNIX sockets, with this combination only bind(),bind()
* Return Value		: First bind() should fail with EADDRINUSE and second bind() should fail with EADDRINUSE
* API Tested		: bind() 
*/	

TVerdict CTestLocalSocket::TestNegativeBind4( )
    { 
    int SocketFamily, ret, fd, s = -1, len;
    struct sockaddr_un local;
	ret = GetIntFromConfig(ConfigSection(), _L("SocketFamily"), SocketFamily);
	if(ret != 1)
		{
	 	ERR_PRINTF1(_L("Could not read the socket family name from ini file"));
	 	goto close;
		} 
    local.sun_family = SocketFamily;
    if(sizeof(local.sun_path) < strlen(SOCK_PATH))
    	{
    	ERR_PRINTF1(_L("The length of the path given exceeds the maximum limit"));
    	goto close;
    	}
	s = socket(SocketFamily, SOCK_STREAM, 0);
    if (s == -1) 
    	{
    	ERR_PRINTF2(_L("Error in creating the socket and errno is %d"),errno);
    	goto close;
    	}

    strcpy(local.sun_path, SOCK_PATH);
    unlink(SOCK_PATH);
    fd = open(SOCK_PATH, O_CREAT | O_RDWR, 0666);
    if (fd < 0)
		{
		ERR_PRINTF2(_L("Error in creating the file and errno is %d"),errno);
		goto close;
		}
    close(fd);
    len = sizeof(struct sockaddr_un);
    ret = bind(s, (struct sockaddr *)&local, len);
    if ((ret != -1) || (errno != EADDRINUSE)) 
     	{
     	ERR_PRINTF2(_L("Bind1 failed to return EADDRINUSE and errno is set to %d"),errno);
     	goto close;
     	}
    INFO_PRINTF1(_L("Bind1 successfully returned EADDRINUSE when called for the first time"));
    ret = bind(s, (struct sockaddr *)&local, len);
    if ((ret != -1) || (errno != EADDRINUSE)) 
     	{
     	ERR_PRINTF2(_L("Bind2 failed to return EADDRINUSE and errno is set to %d"),errno);
     	goto close;
     	}
    INFO_PRINTF1(_L("Bind1 successfully returned EADDRINUSE when called for the second time"));

    SetTestStepResult(EPass);
    close:
    if(s != -1)
    	{
    	close(s);
    	}
    unlink(SOCK_PATH);
    return TestStepResult();
    }

/**
* Function Name		: TestNegativeBind5
* Description		: Negative test on UNIX sockets, with this combination unlink(x),bind(),unlink(y),bind()
* Return Value		: First bind() should be success and second bind() should fail with EINVAL
* API Tested		: bind() 
*/	

TVerdict CTestLocalSocket::TestNegativeBind5( )
    { 
    int SocketFamily, ret, fd, s = -1, len;
    struct sockaddr_un local;
	ret = GetIntFromConfig(ConfigSection(), _L("SocketFamily"), SocketFamily);
	if(ret != 1)
		{
	 	ERR_PRINTF1(_L("Could not read the socket family name from ini file"));
	 	goto close;
		} 
    local.sun_family = SocketFamily;
    if(sizeof(local.sun_path) < strlen(SOCK_PATH))
    	{
    	ERR_PRINTF1(_L("The length of the path given exceeds the maximum limit"));
    	goto close;
    	}
	s = socket(SocketFamily, SOCK_STREAM, 0);
    if (s == -1) 
    	{
    	ERR_PRINTF2(_L("Error in creating the socket and errno is %d"),errno);
    	goto close;
    	}

    strcpy(local.sun_path, SOCK_PATH);
    unlink(SOCK_PATH);
    fd = open(SOCK_PATH, O_CREAT | O_RDWR, 0666);
    if (fd < 0)
		{
		ERR_PRINTF2(_L("Error in creating the file and errno is %d"),errno);
		goto close;
		}
    close(fd);
    unlink(SOCK_PATH);
    len = sizeof(struct sockaddr_un);
    ret = bind(s, (struct sockaddr *)&local, len);
    if (ret != 0) 
    	{
    	ERR_PRINTF2(_L("Bind1 failed and errno is set to %d"),errno);
    	goto close;
    	}
    INFO_PRINTF1(_L("Successfully able to bind for the first"));
    strcpy(local.sun_path, SOCK_PATH1);
    ret = bind(s, (struct sockaddr *)&local, len);
    if ((ret != -1) || (errno != EINVAL)) 
     	{
     	ERR_PRINTF2(_L("Bind2 failed to return EINVAL and errno is set to %d"),errno);
     	goto close;
     	}
    INFO_PRINTF1(_L("bind() successfully returned EINVAL when called for the second time"));

    SetTestStepResult(EPass);
    close:
    if(s != -1)
    	{
    	close(s);
    	}
    unlink(SOCK_PATH);
    return TestStepResult();
    }

/**
* Function Name		: TestListenonUnixsoc
* Description		: Listen on UNIX socket
* Return Value		: listen() should just return 0.
*/	

TVerdict CTestLocalSocket::TestListenonUnixsoc( )
    {
    int ret, SocketFamily, s = -1, len;
    struct sockaddr_un local;
	ret = GetIntFromConfig(ConfigSection(), _L("SocketFamily"), SocketFamily);
	if(ret != 1)
		{
	 	ERR_PRINTF1(_L("Could not read the socket family name from ini file"));
	 	goto close;
		}         
    local.sun_family = SocketFamily;
    if(sizeof(local.sun_path) < strlen(SOCK_PATH))
    	{
    	ERR_PRINTF1(_L("The length of the path given exceeds the maximum limit"));
    	goto close;
    	}
    strcpy(local.sun_path, SOCK_PATH);
    unlink(SOCK_PATH);
    
    s = socket(SocketFamily, SOCK_STREAM, 0);
    if (s < 0)
    	{
    	ERR_PRINTF2(_L("protocol not supported. Errno is %d"), errno);
    	goto close;
    	}
    len = sizeof(struct sockaddr_un);
    ret = bind(s, (struct sockaddr*)&local, len);
    if (ret < 0)
       	{
        ERR_PRINTF2(_L("socket binding error: %d"), errno);
       	goto close;
       	}
    
    ret = listen(s, 5);
    if (ret != 0)
		{
		ERR_PRINTF2(_L("Listen fails with error no: %d"), errno);
		goto close;
		}
    INFO_PRINTF1(_L("Listen successful"));
	
	SetTestStepResult(EPass);
	close:
    if(s != -1)
    	{
    	close(s);
    	}
    unlink(SOCK_PATH);
    return TestStepResult();
    }



/**
* Function Name		: TestNonblockAcceptSetfl
* Description		: Nonblocking accept() by setting setfl flag
* Return Value		: accept() should not block and should succeed after multiple tries.
*/
TVerdict CTestLocalSocket::TestNonblockAcceptSetfl( )
	{
	int sockfd = -1, newsockfd = -1;
	pthread_t thread;
	int t1= 0, iof = -1;
	struct sockaddr_un src,cliaddr;
	unlink(SERVPATH);
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockfd < 0)
		{
		ERR_PRINTF2(_L("Socket Creation failed %d\n"), errno);
		return TestStepResult();
		}
	memset(&src, 0x00, sizeof(struct sockaddr_un));
	src.sun_family=AF_UNIX;    
	strcpy((char*)&src.sun_path, SERVPATH);    
	if ( bind(sockfd,(struct sockaddr *)&src,sizeof(src))  < 0)
		{
		ERR_PRINTF2(_L("Bind error = %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	if(listen(sockfd,5) < 0)
		{
		ERR_PRINTF2(_L("listen() fails with errno %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	if(pthread_create(&thread, NULL, ThreadConnect, (void*)t1) != 0)
		{
		ERR_PRINTF2(_L("pthread_create() fails with errno %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	socklen_t clilen = sizeof(cliaddr);
	if((iof = fcntl(sockfd, F_GETFL, 0)) != -1){
		if(fcntl(sockfd,F_SETFL,iof | O_NONBLOCK) < 0)
			{
			ERR_PRINTF2(_L("Setting non-blocking mode failed with errno %d\n"), errno);
			if(sockfd)
				close(sockfd);
			unlink(SERVPATH);
			return TestStepResult();
			}
	}
	else
		{
		ERR_PRINTF2(_L("fcntl failed with errno %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	do
		{
		newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
		if(newsockfd<0)
			INFO_PRINTF1(_L("Waiting on accept()...\n"));
		}while((newsockfd == -1) && (errno == EAGAIN));
	if(newsockfd < 0)
		{
		ERR_PRINTF2(_L("Accept failed with errno %d"),errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	pthread_join(thread,NULL);
	SetTestStepResult(EPass);
	if(sockfd)
		close(sockfd);
	unlink(SERVPATH);
	return TestStepResult();
	}


/**
* Function Name		: TestNonblockAcceptSelect
* Description		: Nonblocking accept() using select()
* Return Value		: accept() succeeds, testcase returns KErrNone
*/
TVerdict CTestLocalSocket::TestNonblockAcceptSelect()
	{
	int sockfd = -1, newsockfd = -1;
	fd_set readfds;
	int fdmax, t1 = 0;
	pthread_t thread;
	struct sockaddr_un src,cliaddr;
	unlink(SERVPATH);
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockfd < 0)
		{
		ERR_PRINTF2(_L("Socket Creation failed %d\n"), errno);
		return TestStepResult();
		}
	memset(&src, 0x00, sizeof(struct sockaddr_un));
	src.sun_family=AF_UNIX;    
	strcpy((char*)&src.sun_path, SERVPATH);    
	if ( bind(sockfd,(struct sockaddr *)&src,sizeof(src))  < 0)
		{
		ERR_PRINTF2(_L("Bind fails %d\n"),errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	if( listen(sockfd,5) < 0)
		{
		ERR_PRINTF2(_L("Listen fails %d\n"),errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	if(pthread_create(&thread, NULL, ThreadConnect, (void*)t1) != 0)
		{
		ERR_PRINTF2(_L("pthread_create() fails with errno %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	socklen_t clilen = sizeof(cliaddr);
	fdmax = sockfd;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	int result;
	do
		{
		FD_ZERO(&readfds);
		FD_SET(sockfd,&readfds);
		result = select(fdmax+1,&readfds,NULL,NULL,&tv);
		}while(result == -1 && errno == EINTR);
	
	if(result == -1)
		{
		ERR_PRINTF2(_L("Select failed with errno %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	
	if(result == 0)
		{
		ERR_PRINTF1(_L("Select timed out\n"));
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	if(result>0)
		if(FD_ISSET(sockfd,&readfds))
			{
			newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
			if(newsockfd < 0)
				{
				ERR_PRINTF2(_L("Accept fails %d\n"), errno);
				if(sockfd)
					close(sockfd);
				unlink(SERVPATH);
				return TestStepResult();
				}
			}
	SetTestStepResult(EPass);
	pthread_join(thread,NULL);
	if(sockfd)
		close(sockfd);
	unlink(SERVPATH);
	return TestStepResult();
	}

/**
* Function Name		: TestNonblockingRead
* Description		: Nonblocking read() using select()
* Return Value		: read() succeeds, testcase returns KErrNone
*/
TVerdict CTestLocalSocket::TestNonblockingRead()
	{
	int sockfd = -1, newsockfd = -1;
	int fdmax;
	char buffer[KMaxLine];
	fd_set readfds;
	pthread_t thread;
	struct sockaddr_un src,cliaddr;
	unlink(SERVPATH);
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockfd < 0)
		{
		ERR_PRINTF2(_L("Socket creation failed %d\n"),errno);
		return TestStepResult();
		}
	memset(&src, 0x00, sizeof(struct sockaddr_un));
	src.sun_family=AF_UNIX;    
	strcpy((char*)&src.sun_path, SERVPATH);  
	if ( bind(sockfd,(struct sockaddr *)&src,sizeof(src))  < 0)
		{
		ERR_PRINTF2(_L("Bind failed %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	if(listen(sockfd,5) < 0)
		{
		ERR_PRINTF2(_L("Listen failed %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	if(pthread_create(&thread, NULL, TCPThreadWrite, (void*)(src.sun_path)) != 0)
		{
		ERR_PRINTF2(_L("pthread_create() fails with errno %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	socklen_t clilen = sizeof(cliaddr);
	newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
	if(newsockfd < 0)
		{
		ERR_PRINTF2(_L("Accept failed %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	fdmax = newsockfd;
	int result;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 2 * 1000;
	do
		{
		FD_ZERO(&readfds);
		FD_SET(newsockfd,&readfds);
		result = select(fdmax+1,&readfds,NULL,NULL,&tv);
		}while(result == -1 && errno == EINTR);
	
	if(result == -1)
		{
		INFO_PRINTF2(_L("Select failed with errno %d"),errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	
	if(result == 0)
		{
		INFO_PRINTF1(_L("Select() timed out\n"));
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	
	if(result > 0)
		{
		int res2;
		if(FD_ISSET(newsockfd,&readfds))
			{
			res2 = read(newsockfd,buffer,KMaxLine);
			if(res2 != KMaxLine)
				{
				ERR_PRINTF2(_L("Read failed %d\n"),errno);
				if(sockfd)
					close(sockfd);
				unlink(SERVPATH);
				return TestStepResult();
				}
			}
		}
	pthread_join(thread,NULL);
	SetTestStepResult(EPass);
	if(sockfd)
		close(sockfd);
	unlink(SERVPATH);
	return TestStepResult();
	}

/**
* Function Name		: TestNonblockingRecv
* Description		: Nonblocking recv() using select()
* Return Value		: recv() succeeds, testcase returns KErrNone
*/
TVerdict CTestLocalSocket::TestNonblockingRecv()
	{
	int sockfd = -1, newsockfd = -1;
	int fdmax;
	char buffer[KMaxLine];
	fd_set readfds;
	pthread_t thread;
	struct sockaddr_un src,cliaddr;
	unlink(SERVPATH);
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockfd < 0)
		{
		ERR_PRINTF2(_L("Socket creation failed %d\n"),errno);
		return TestStepResult();
		}
	memset(&src, 0x00, sizeof(struct sockaddr_un));
	src.sun_family=AF_UNIX;    
	strcpy((char*)&src.sun_path, SERVPATH);  
	if ( bind(sockfd,(struct sockaddr *)&src,sizeof(src))  < 0)
		{
		ERR_PRINTF2(_L("Bind failed %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	if(listen(sockfd,5) < 0)
		{
		ERR_PRINTF2(_L("Listen failed %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	if(pthread_create(&thread, NULL, TCPThreadWrite, (void*)(src.sun_path)) != 0)
		{
		ERR_PRINTF2(_L("pthread_create() fails with errno %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	socklen_t clilen = sizeof(cliaddr);
	newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
	if(newsockfd < 0)
		{
		ERR_PRINTF2(_L("Accept failed %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	fdmax = newsockfd;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 2 * 1000;
	int result;
	do
		{
		FD_ZERO(&readfds);
		FD_SET(newsockfd,&readfds);
		result = select(fdmax+1,&readfds,NULL,NULL,&tv);
		}while(result == -1 && errno == EINTR);
	if(result == -1)
		{
		INFO_PRINTF2(_L("Select failed with errno %d"),errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	
	if(result == 0)
		{
		INFO_PRINTF1(_L("Select() timed out\n"));
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	
	if(result > 0)
		{
		int res2;
		if(FD_ISSET(newsockfd,&readfds))
			{
			res2 = recv(newsockfd,buffer,KMaxLine,NULL);
			if(res2 != KMaxLine)
				{
				ERR_PRINTF2(_L("Recv failed %d\n"),errno);
				if(sockfd)
					close(sockfd);
				unlink(SERVPATH);
				return TestStepResult();
				}
			}
		}
	pthread_join(thread,NULL);
	SetTestStepResult(EPass);
	if(sockfd)
		close(sockfd);
	unlink(SERVPATH);
	return TestStepResult();
	}

/**
* Function Name		: TestGetSockName
* Description		: Retrieve the address of the sockaddr structure and the length of this structure, bound to the specified socket
* Return Value		: getsockname() succeeds, testcase returns KErrNone
*/
TVerdict CTestLocalSocket::TestGetSockName()
	{
	int sockfd = -1;
	struct sockaddr_un src,cliaddr;
	unlink(SERVPATH);
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockfd < 0)
		{
		ERR_PRINTF2(_L("Socket creation failed %d\n"),errno);
		return TestStepResult();
		}
	memset(&src, 0x00, sizeof(struct sockaddr_un));
	memset(&cliaddr, 0x00, sizeof(struct sockaddr_un));
	src.sun_family=AF_UNIX; 
	strcpy((char*)&src.sun_path, SERVPATH);    
	if ( bind(sockfd,(struct sockaddr *)&src,sizeof(src))  < 0)
		{
		ERR_PRINTF2(_L("Bind error = %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	socklen_t clilen = sizeof(cliaddr);
	if(getsockname(sockfd,(struct sockaddr*)&cliaddr,&clilen) < 0)
		{
		ERR_PRINTF2(_L("Getsockname failed %d\n"),errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	if((cliaddr.sun_family == AF_UNIX)&&
	   (strcasecmp(cliaddr.sun_path,src.sun_path)==0)&&
	   (clilen == (sizeof(SERVPATH)+ sizeof(src.sun_family))))
			{
			INFO_PRINTF1(_L("Getsockname() succeeds\n"));
			SetTestStepResult(EPass);
			}
	else
		ERR_PRINTF2(_L("Getsockname: cleintlen is improper\n"),errno);
	if(sockfd)
		close(sockfd);
	unlink(SERVPATH);
	return TestStepResult();
	}


/**
* Function Name		: TestGetPeerName
* Description		: Retrieve the address of the sockaddr structure and the length of this structure, bound to the specified peer socket
* Return Value		: getpeername() succeeds, testcase returns KErrNone
*/
TVerdict CTestLocalSocket::TestGetPeerName()
	{
	int new_fd = -1, newsockfd = -1;
	pthread_t thread;
	socklen_t clilen2 = 0;

	unlink(SERVPATH);
    struct sockaddr_un src,cliaddr2;
    new_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(new_fd < 0)
    	{
    	ERR_PRINTF2(_L("Socket Creation failed %d\n"),errno);
    	return TestStepResult();
    	}
	memset(&cliaddr2, 0x00, sizeof(struct sockaddr_un));
	memset(&src, 0x00, sizeof(struct sockaddr_un));  
	src.sun_family=AF_UNIX; 
	strcpy((char*)&src.sun_path, SERVPATH);    
	if ( bind(new_fd,(struct sockaddr *)&src,sizeof(src))  < 0)
		{
		ERR_PRINTF2(_L("Bind failed = %d\n"),errno);
	 	if(new_fd)
			close(new_fd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	if(listen(new_fd,2) < 0)
		{
		ERR_PRINTF2(_L("Listen failed %d\n"),errno);
	 	if(new_fd)
			close(new_fd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	if(pthread_create(&thread, NULL, ThreadConnect, NULL) != 0)
		{
		ERR_PRINTF2(_L("pthread_create() fails with errno %d\n"), errno);
		if(new_fd)
			close(new_fd);
		unlink(SERVPATH);
		return TestStepResult();
		}

	newsockfd = accept(new_fd, (struct sockaddr*)&src, &clilen2);
	if(newsockfd < 0)
		{
		ERR_PRINTF2(_L("Accept failed %d\n"),errno);
	 	if(new_fd)
			close(new_fd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	clilen2 = sizeof(struct sockaddr_un);

    if(getpeername(newsockfd,(struct sockaddr*)&cliaddr2,&clilen2) < 0)
    	{
    	ERR_PRINTF2(_L("Getpeername failed %d\n"),errno);
     	if(new_fd)
    		close(new_fd);
    	unlink(SERVPATH);
    	return TestStepResult();
    	}
    if((cliaddr2.sun_family == AF_UNIX)&&
       (strcasecmp(cliaddr2.sun_path,SOCK_PATH) == 0)&& 
       (clilen2 == (sizeof(SOCK_PATH)+ sizeof(src.sun_family))))
			{
			SetTestStepResult(EPass);
			}

 	if(new_fd)
		close(new_fd);
	unlink(SERVPATH);

	pthread_join(thread,NULL);
	ERR_PRINTF1(_L("Coming at end3 %d\n"));
	return TestStepResult();
	}


/**
* Function Name		: TestThreadRecvfrom
* Description		: recvfrom from a datagram socket
* Return Value		: recvfrom() succeeds, testcase returns KErrNone
*/
TVerdict CTestLocalSocket::TestThreadRecvfrom()
	{
	int sockfd = -1;
	char buffer[KDataLength];
	pthread_t thread;
	struct sockaddr_un src,cliaddr;
	sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(sockfd < 0)
		{
		ERR_PRINTF2(_L("Socket Creation failed %d\n"),errno);
		return TestStepResult();
		}
	memset(&src, 0x00, sizeof(struct sockaddr_un));
	src.sun_family=AF_UNIX;    
	strcpy((char*)&src.sun_path, SERVPATH);   
	unlink(SERVPATH);
	if ( bind(sockfd,(struct sockaddr *)&src,sizeof(src))  < 0)
		{
		ERR_PRINTF2(_L("Bind failed = %d\n"),errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	if(pthread_create(&thread, NULL, ThreadSendto, NULL) != 0)
		{
		ERR_PRINTF2(_L("pthread_create() fails with errno %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	socklen_t clilen = sizeof(cliaddr);
	pthread_join(thread,NULL);
	if(recvfrom(sockfd, buffer, KDataLength, 0, (struct sockaddr*)&cliaddr, &clilen) < KDataLength)
		{
		ERR_PRINTF2(_L("Recvfrom failed %d\n"),errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	if(sockfd)
		close(sockfd);
	unlink(SERVPATH);
	return TestStepResult();
	}


/**
* Function Name		: TestThreadSendto
* Description		: sendto a datagram socket
* Return Value		: sendto() succeeds, testcase returns KErrNone
*/
TVerdict CTestLocalSocket::TestThreadSendto()
	{

	int sockfd = -1;
	pthread_t thread;
	struct sockaddr_un src;
	sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(sockfd < 0)
		{
		ERR_PRINTF2(_L("Socket creation failed %d\n"),errno);
		return TestStepResult();
		}
	memset(&src, 0x00, sizeof(struct sockaddr_un));
	src.sun_family=AF_UNIX;    
	strcpy((char*)&src.sun_path, SERVPATH);   
	if(pthread_create(&thread, NULL, ThreadRecvfrom, NULL) != 0)
		{
		ERR_PRINTF2(_L("pthread_create() fails with errno %d\n"), errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	sleep(1);

	if(sendto(sockfd, DATA, KDataLength, 0, (struct sockaddr*)&src, sizeof(src)) < KDataLength)
		{
		ERR_PRINTF2(_L("Sendto failed %d\n"),errno);
		if(sockfd)
			close(sockfd);
		unlink(SERVPATH);
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	pthread_join(thread,NULL);
	if(sockfd)
		close(sockfd);
	unlink(SERVPATH);
	return TestStepResult();
    }


/*
* Testcase		: TestLocalSockMultipleBind
* Description	: Test multiple binds for localsocket
* Result		: bind retuns 0, test case returns KErrNone
**/
TVerdict CTestLocalSocket::TestLocalSockMultipleBind()
	{
    int len = 0;
    int listenSocket1 = -1;
    int listenSocket2 = -1;
    struct sockaddr_un local;
    
    INFO_PRINTF1(_L("   ...Setting sockaddr_un for bind and listen"));
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, "c:\\some_local_socket");
    len = strlen(local.sun_path) + sizeof(local.sun_family) + 1;    
    
    INFO_PRINTF1(_L("   ...Ensuring that the file does not exist (unlink)")); 
    unlink(local.sun_path);
    
    INFO_PRINTF1(_L( "   ...Creating 1st listening UNIX domain socket (AF_UNIX)" ));    
    
    //===============================================================
    // Socket 1
    //===============================================================
    
    // create listening server socket
    if ((listenSocket1 = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    	{
        ERR_PRINTF2(_L( "Socket creation failed, errno: %d"), errno);
        goto close;
    	}
    
    INFO_PRINTF1(_L( "...Binding"));    
    if (bind(listenSocket1, (struct sockaddr *)&local, len) == -1)
    	{
        ERR_PRINTF2(_L("..Bind error :%d"), errno);
        goto close;
    	}

    INFO_PRINTF1(_L("...Listening"));  
    if (listen(listenSocket1, 5) == -1) 
    	{
        ERR_PRINTF2(_L("...Listen error is :%d"), errno);    
        goto close;
    	}
    
    //===============================================================
    // Forced unlink
    //===============================================================       
    INFO_PRINTF1(_L("...force unlink the file"));
    if (unlink(local.sun_path) == -1)
    	{    	
    	ERR_PRINTF2(_L("...forced unlink failed and errno :%d"), errno);
    	}
    else
    	{
    	INFO_PRINTF1(_L("...forced unlink() succeeded"));
    	}
         
    //===============================================================
    // Socket 2
    //===============================================================        
    INFO_PRINTF1(_L("...Creating 2nd listening UNIX domain socket (AF_UNIX)"));         
         
    // create listening server socket
    if ((listenSocket2 = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) 
    	{
        ERR_PRINTF2(_L("...Error in UNIX domain socket (AF_UNIX) creation, errno: %d"), errno);
        goto close;
    	}
    
    INFO_PRINTF1(_L("...Binding"));    
    if (bind(listenSocket2, (struct sockaddr *)&local, len) == -1) 
    	{
        ERR_PRINTF2(_L("...Bind error :%d"), errno);
    	goto close;
    	}

    INFO_PRINTF1(_L("...Listening\n"));  
    if (listen(listenSocket2, 5) == -1) 
    	{
        ERR_PRINTF2(_L("...Listen error is :%d"), errno);    
        goto close;
    	}
    	
  	SetTestStepResult(EPass);
  	close:
  	if(listenSocket1 > 0)
		close(listenSocket1);
  	if(listenSocket2 > 0)
		close(listenSocket2);
  	unlink(local.sun_path);	    
	return TestStepResult();
	}
//  End of File
