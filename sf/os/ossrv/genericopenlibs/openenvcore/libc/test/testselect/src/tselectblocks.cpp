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



#include "tselect.h"

// CONSTANTS
const TUint KTestPort = 2080;

TInt CTestSelect::TestTimeout()
    {    
    
    // Create temporary variables in stack
    TInt timesecs;
    TInt timemicrosecs;
  	
    timemicrosecs = 0;
    timesecs = 10;
  	
    struct timeval tv;
    tv.tv_sec = timesecs;
    tv.tv_usec = timemicrosecs;
    
    time_t time1,time2;
    int err = time(&time1);
    
    int fd = select(1, NULL, NULL,NULL, &tv);  
    err = time(&time2);
    if ( ((time2 - time1) >= timesecs) && fd != -1 )
        {                    
        return KErrNone; 
        }
    else
        {       
        return KErrGeneral; 
        }        
    }
        
    
TInt CTestSelect::TestReadFileDesc()
    {
    
    TInt timesecs = 5;
    TInt timemicrosecs = 0;

    struct timeval tv;
    tv.tv_sec = timesecs;
    tv.tv_usec = timemicrosecs;    
    
    // read only
    int fd = open("c:\\tselect.cfg", O_RDONLY|O_CREAT);
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    int maxfd = fd + 1;
    int ret = select(maxfd, &readfds, NULL, NULL,&tv);    
    if ( ret != -1 && FD_ISSET(fd, &readfds))    
        {
        _LIT(KMsg1, "Read on file fd Test Passed");
        INFO_PRINTF1(KMsg1);
        return KErrNone; 
        }
    else
        {
        _LIT(KMsg1, "Read on file fd Test Failed");
        INFO_PRINTF1(KMsg1);
        return KErrGeneral; 
        }      
    }  
    
TInt CTestSelect::TestWriteFileDesc()
    {
    
    TInt timesecs = 2;
    TInt timemicrosecs = 0;	
    
    struct timeval tv;
    tv.tv_sec = timesecs;
    tv.tv_usec = timemicrosecs;    
        
    // write check        
    int fd = open("c:\\tselect.cfg", O_RDONLY);
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(fd, &writefds);
    int maxfd = fd + 1;
    int ret = select(maxfd, NULL,&writefds, NULL,&tv);    
    if ( ret != -1 && FD_ISSET(fd, &writefds))    
        {
        _LIT(KMsg1, "Write on file fd Test Passed");
        INFO_PRINTF1(KMsg1);
        return KErrNone; 
        }
    else
        {
        _LIT(KMsg1, "Write on file fd Test failed");
        INFO_PRINTF1(KMsg1);
        return KErrGeneral; 
        }    
    }
    
TInt CTestSelect::TestReadWriteFileDesc()
    {    
    // both read and write
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;    
    
    int fd = open("c:\\tselect.cfg", O_RDONLY);
    fd_set readfds;
    FD_ZERO(&readfds);        
    FD_SET(fd, &readfds);
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(fd, &writefds);
    int maxfd = fd + 1;
    int ret = select(maxfd, &readfds, &writefds, NULL,&tv);    
    if ( ret != -1 && FD_ISSET(fd, &readfds) && FD_ISSET(fd, &writefds) )    
        {
        _LIT(KMsg1, "R/W on file fd Test passed");
        INFO_PRINTF1(KMsg1);       
        return KErrNone; 
        }
    else
        {
        _LIT(KMsg1, "R/W on file fd Test failed");
        INFO_PRINTF1(KMsg1);
        return KErrGeneral; 
        }
    }	     
   


TInt CTestSelect::TestPipeReadDesc()
    {   

    int fd[2];		  
    INFO_PRINTF1(_L("Before Pipe "));   
    if ( pipe(fd) < 0 )
        {
        return -1;
        }          

    char a = 's';
    write(fd[1],&a,1);
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd[0], &readfds);                     
    int ret = select(fd[0] +1, &readfds, NULL, NULL, NULL );    
    if ( ret != -1 && FD_ISSET(fd[0], &readfds))    
        {        
        _LIT(KMsg1, "Read on pipe fd Test passed");
        INFO_PRINTF1(KMsg1);                 
        ret = KErrNone; 
        }
    else
        {
        _LIT(KMsg1, "Read on pipe fd Test failed");
        INFO_PRINTF1(KMsg1);   
        ret = KErrGeneral; 
        }                
    close(fd[0]);
    close(fd[1]);
    return ret;
    
    }

//Test case added to check when write end of a pipe is closed 
TInt CTestSelect::TestPipeReadDesc1()
	{
    int fd[2];		  
    INFO_PRINTF1(_L("Before Pipe "));   
    if ( pipe(fd) < 0 )
        {
        return -1;
        }          

    char a = 's';
    write(fd[1],&a,0);
    fd_set readfds,exceptfds;
    FD_ZERO(&readfds);
    FD_ZERO(&exceptfds);
    FD_SET(fd[0], &readfds); 
    FD_SET(fd[0], &exceptfds);
    FD_SET(fd[1], &exceptfds);
    close(fd[1]);
    int ret = select(fd[0] +1, &readfds, NULL, &exceptfds, NULL );    
    if ( ret != -1 && FD_ISSET(fd[0], &exceptfds))    
        {        
        _LIT(KMsg1, "Read on pipe fd Test passed and passed to set EAnyException on write end of a pipe being closed\n");
        INFO_PRINTF1(KMsg1);                 
        ret = KErrNone; 
        }
    else
        {
        _LIT(KMsg1, "Failed to set EAnyException on write end being closed\n");
        INFO_PRINTF1(KMsg1);   
        ret = KErrGeneral; 
        }                
    close(fd[0]);
    return ret;
	}

   
TInt CTestSelect::TestPipeWriteDesc()
    {       
    int fd[2];		  
    INFO_PRINTF1(_L("Before Pipe "));   
    if ( pipe(fd) < 0 )
        {
        return -1;
        } 
        
    INFO_PRINTF1(_L("Pipe Created"));               
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(fd[1], &writefds);                     
    int ret = select(fd[1] +1, NULL, &writefds, NULL, NULL );    
    iSelectflag = 1;
    
    if ( ret != -1 && FD_ISSET(fd[1], &writefds))    
        {
        _LIT(KMsg1, "Write on pipe fd Test passed");
        INFO_PRINTF1(KMsg1);               
        ret = KErrNone; 
        }
    else
        {
        _LIT(KMsg1, "Write on pipe fd Test failed");
        INFO_PRINTF1(KMsg1);  
        ret = KErrGeneral; 
        }                 
        
    close(fd[0]);
    close(fd[1]);
    
    return ret;
    }  

void* CTestSelect::ServerThread(TAny* ptr)
    {
    CTestSelect* thisptr = (CTestSelect*)ptr;
    int sock_fd,newsock_fd;	
    unsigned int addr_len;
    sockaddr_in serv_addr,new_socket;

    sock_fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (sock_fd < 0)
        {		
	return NULL;//KErrGeneral;
        }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(KTestPort);	

    if (bind(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
        {
        shutdown(sock_fd,SHUT_RDWR); // 2 -> SHUT_RDWT
        close(sock_fd);
        return NULL;//KErrBind;
        }
    
    if (listen(sock_fd,1) < 0)
        {
        shutdown(sock_fd,SHUT_RDWR); // 2 -> SHUT_RDWT
        close(sock_fd);
        return NULL;//KErrListen;
        }
    
    addr_len = sizeof(new_socket);
    thisptr->iAcceptflag = 1;
	
    newsock_fd = accept(sock_fd,(sockaddr*)&new_socket,&addr_len); // Code blocks here
    char a = 'S';
    write(newsock_fd, &a, 1);
    thisptr->iWriteflag = 1;
	
    while(!thisptr->iSelectflag)
        {
	usleep(10000);  
	}
    close(sock_fd);
	close(newsock_fd);
    return NULL;//KErrNone;	
    }
	
TInt CTestSelect::TestSocketDesc()
    {    
	struct timeval tv;
    struct sockaddr_in testaddr;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    memset( &testaddr,0, sizeof( testaddr));       
       // now connect to the server
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    INFO_PRINTF1(_L("socket in client called"));
    
    pthread_t testThread;
    pthread_create(&testThread, NULL, &ServerThread, (void*)this);   
    
    testaddr.sin_family = AF_INET;
    testaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);	
    testaddr.sin_port = htons(KTestPort);
    while ( !iAcceptflag )
        {
        usleep(10000);    
        }    
        
    INFO_PRINTF1(_L("connect called"));        

    TInt err=connect(fd,(struct sockaddr*)&testaddr, sizeof(testaddr));    
       
    fd_set readfds,writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(fd, &readfds);
    FD_SET(fd, &writefds);
    int maxfd = fd + 1;
    while ( !iWriteflag )
        {
        usleep(10000);
        }
        
    INFO_PRINTF1(_L("before select "));   
          
    int ret = select(maxfd, &readfds, NULL, NULL,&tv);    
  
    INFO_PRINTF1(_L("after select"));  
    iSelectflag = 1;    
    if ( ret != -1 && FD_ISSET(fd, &readfds))    
        {
        ret = KErrNone; 
        }
    else
        {
        ret = KErrGeneral; 
        }     
        
    close(fd);
   	int *p;
   	pthread_join(testThread,(void**)&p);
    return ret;                 
    }  
    
 //Test case added for select not hanging for zero timeout- DEF122924
TInt CTestSelect::TestSocketDesc2()
    {    
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    INFO_PRINTF1(_L("socket in client called"));
        
    fd_set readfds,writefds,errfds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&errfds);
    FD_SET(fd, &readfds);
    int maxfd = fd + 1;
    INFO_PRINTF1(_L("before select "));   
    int ret = select(maxfd, &readfds, &writefds, &errfds,&tv);    
    INFO_PRINTF1(_L("select passes, does not hang."));         
    close(fd);
    return ret;                 
    }  

int FlgReadWrite = 0;
int FlgRead = 0;
#define ReadWriteErr2 7
#define ReadWriteErr1 6 
#define Write2Err 5 
#define Write1Err 4
#define Read1Err  3
#define PipeReadDesc 8
#define PipeWriteDesc 9


void* CreateConnection(void* /*aParam*/)
	{
//	Only make a write on the sock_fd
	int ret;
	struct sockaddr_in host;
	int dest_fd;
	char buf[50];
	host.sin_port = htons(5002);
	host.sin_family = AF_INET;
	host.sin_addr.s_addr = inet_addr("127.0.0.1");
	dest_fd = socket(AF_INET,SOCK_STREAM,0);
	ret = connect(dest_fd,(struct sockaddr*)&host,sizeof(host));
	ret = read(dest_fd,buf,sizeof(buf));
	ret = write(dest_fd,buf,ret);
	close(dest_fd);
	return NULL;
	}

void* SocketReadWrite(void* aSock)
	{
	// Note, here "aSock" is being treated as an array of FDs.
	// The FD at the 0th index is the write-fd, and the FD at 
	// the 1st index is the read-fd. 
	int sock_fd = *((int*)aSock);
	fd_set fds;
	int ret;	
	char buf[] = "Write from third thread";
	char buf2[50];
	
	struct timeval timeout;	
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	
//  Setting the write-fd. 
	FD_ZERO(&fds);
	FD_SET(sock_fd,&fds);
	
	fd_set readfds;
	FD_ZERO(&readfds);
	//FD_SET(((int*)aSock)[0],&readfds);
	FD_SET(((int*)aSock)[1],&readfds);
	FD_SET(((int*)aSock)[PipeReadDesc],&readfds);
	
	((int*)aSock)[ReadWriteErr1] = 0;
	((int*)aSock)[ReadWriteErr2] = 0;
	
	//while(FlgReadWrite == 0)
	//while(FlgRead == 0)
		{		
		}	

	ret = select(((int*)aSock)[PipeReadDesc] + 1,&readfds,NULL,NULL,&timeout);	
	//ret = select(((int*)aSock)[PipeReadDesc] + 1,&readfds,&fds,NULL,&timeout);
	//ret = select(((int*)aSock)[1] + 1,&readfds,NULL,NULL,&timeout);
#if 0
	if (ret && FD_ISSET(sock_fd,&fds))
		{
		ret = write(sock_fd,buf,sizeof(buf));
		fprintf( (FILE*)(((int*)aSock)[2]), "WRITING SocketReadWrite END - have written = %d and %s\n",ret,buf);		
		if (ret < 0)
			((int*)aSock)[ReadWriteErr1] = ret;		
		}
#endif	
	if(ret && FD_ISSET(((int*)aSock)[1],&readfds))
		{
		ret = read(((int*)aSock)[1],buf2,sizeof(buf2));
		fprintf( (FILE*)(((int*)aSock)[2]), "READING SocketReadWrite END - have read = %d and %s\n",ret,buf2);	
		if(ret < 0)
			((int*)aSock)[ReadWriteErr2] = ret;				
		}
	
	if(ret && FD_ISSET(((int*)aSock)[PipeReadDesc],&readfds))
		{
		ret = read(((int*)aSock)[PipeReadDesc],buf,sizeof(buf));
		fprintf( (FILE*)(((int*)aSock)[2]), "READING SocketReadWrite END - have read = %d and %s\n",ret,buf);	
		if(ret < 0)
			((int*)aSock)[ReadWriteErr2] = ret;				
		}
	
	if(ret == -1)	
		{
		fprintf( (FILE*)(((int*)aSock)[2]), "WRITING SocketReadWrite ERROR END - have written = %d \n",ret);
		((int*)aSock)[ReadWriteErr1] = ret;
		}
	return aSock;
	}

void* SocketWrite1(void* aSock)
	{
	int *sock_fd = (int*)aSock;
	((int*)aSock)[Write1Err] = KErrNone;

	char buf[] = "Write from first thread";
	int ret = 0;
//	sleep(3);
	ret = write(*sock_fd,buf,sizeof(buf));
	FlgRead = 1;
	if (ret < 0)
		((int*)aSock)[Write1Err] = ret;
	else
		((int*)aSock)[Write1Err] = KErrNone;
	
	return aSock;
	}

void* SocketWrite2(void* aSock)
	{	
//	int *sock_fd = (int*)aSock;
	((int*)aSock)[Write2Err] = KErrNone;
	
	char buf[] = "Write from second thread";
//	sleep(2);
	int ret = write(((int*)aSock)[1],buf,sizeof(buf));
	FlgReadWrite = 1;
	if (ret < 0)
		((int*)aSock)[Write2Err] = ret;
	else
		((int*)aSock)[Write2Err] = KErrNone;
	
	return aSock;	
	}

void* PipeWrite(void* aPipe)
	{
	//int *pipe_fd = (int*)aPipe;
	char buf[] = "Pipe Write";
	sleep(1);
	int ret = write(((int*)aPipe)[PipeWriteDesc],buf,sizeof(buf));
	FlgReadWrite = 1;
	if (ret < 0)
		((int*)aPipe)[Write2Err] = ret;
	else
		((int*)aPipe)[Write2Err] = KErrNone; 
	return aPipe;
	}

void* SocketRead1(void* aSock)
	{
	int sock_fd = *((int*)aSock);
	fd_set fds;
	char buf[50];
	int ret;
		
	struct timeval timeout;	
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	
	((int*)aSock)[Read1Err] = KErrNone;
	FD_ZERO(&fds);
	FD_SET(sock_fd,&fds);

	//while(FlgRead == 0)
		{		
		}	
	
	ret = select(sock_fd+1,&fds,NULL,NULL,&timeout);	
	
	if (ret && FD_ISSET(sock_fd,&fds))
		{
		ret = read(sock_fd,buf,sizeof(buf));		
		if(ret <= 0)
			{
			((int*)aSock)[Read1Err] = ret;
			fprintf( (FILE*)(((int*)aSock)[2]), "READING SocketRead END - have read = %d \n",ret);			
			}
		else
			{
			fprintf( (FILE*)(((int*)aSock)[2]), "READING SocketRead END - have read = %d and %s\n",ret,buf);
			}
		}
	else if(ret == -1)
		{
		fprintf( (FILE*)(((int*)aSock)[2]), "READING SocketRead ERROR END - have read = %d \n",ret);
		((int*)aSock)[Read1Err] = ret;
		}	
	return aSock;
	}

TInt CTestSelect::TestSocketDesc3()
	{
//	__UHEAP_MARK;
	int ret;
	char buf[50];
	fd_set fds;
	FD_ZERO(&fds);	
	int sock_fds[10];
	int listen_sock = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in host,dest;
	pthread_t client1, client2, sockwrite1, sockwrite2, sockread1, pipewrite, sockreadwrite;
	int err = KErrNone;

	struct timeval timeout;	
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	
    FILE* outFile = fopen("c:\\logs\\TestSocketDesc3.txt", "w+");
    sock_fds[2] = (int)outFile; 
	
	host.sin_port = htons(5002);
	host.sin_family = AF_INET;
	host.sin_addr.s_addr = inet_addr("127.0.0.1");
	ret = bind(listen_sock,(struct sockaddr*)&host,sizeof(host));
	if(ret<0)
		{
		INFO_PRINTF1(_L("Error in bind\n"));
		return KErrGeneral ;		
		}
	ret = listen(listen_sock,1);
	if(ret<0)
		{
		INFO_PRINTF1(_L("Error in listen\n"));
		return KErrGeneral ; 
		}
	unsigned size = sizeof(host);

/*************************** First Client Connection ***************************/
	ret = pthread_create(&client1,(pthread_attr_t*)NULL,&CreateConnection,NULL);
	if(ret<0)
		{
		INFO_PRINTF2(_L("Error creating thread for client1:%d\n"),errno);		
		return KErrGeneral ;
		}
//	This socket monitored by 4 threads - two for read, and two for write.
	sock_fds[0] = accept(listen_sock,(struct sockaddr*)&dest,&size);
/********************************************************************************/
	
/*************************** Second Client Connection ***************************/	
	ret = pthread_create(&client2,(pthread_attr_t*)NULL,&CreateConnection,NULL);
	if(ret<0)
		{
		INFO_PRINTF2(_L("Error creating thread for client2:%d\n"),errno);
		return KErrGeneral ;
		}
//	This socket monitored by 2 threads - one for read, and one for write.
	sock_fds[1] = accept(listen_sock,(struct sockaddr*)&dest,&size);
/********************************************************************************/
	
/*************************** Pipe Creation and Writing **************************/
    int fd[2];
    //char buf1[50]= "Write from main thread";
    INFO_PRINTF1(_L("Before Pipe "));   
    if ( pipe(fd) < 0 )
        {
        return KErrGeneral;
        }	
//    write(fd[1],buf1,sizeof(buf1));
    sock_fds[PipeReadDesc] = fd[0];
    sock_fds[PipeWriteDesc]= fd[1];
/********************************************************************************/	
	
//	Creating the 1st Read thread.	
	ret = pthread_create(&sockread1,(pthread_attr_t*)NULL,&SocketRead1,sock_fds);

//	sleep(3);
//	Creating the Read/Write thread.	
	ret = pthread_create(&sockreadwrite,(pthread_attr_t*)NULL,&SocketReadWrite,sock_fds);

//	Creating the 1st Write thread.
	ret = pthread_create(&sockwrite1,(pthread_attr_t*)NULL,&SocketWrite1,sock_fds);
	if(ret<0)
		{
		INFO_PRINTF2(_L("Error creating thread for Write1:%d\n"),errno);		
		return KErrGeneral ;
		}			
	
//	Creating the 2nd Write thread.	
	ret = pthread_create(&sockwrite2,(pthread_attr_t*)NULL,&SocketWrite2,sock_fds);

//	Creating the 3rd Write thread.	
	ret = pthread_create(&pipewrite,(pthread_attr_t*)NULL,&PipeWrite,sock_fds);	

//	The 2nd Read thread.	
	FD_SET(sock_fds[0],&fds);
	
	ret = select(sock_fds[0]+1,&fds,NULL,NULL,&timeout);	
	if (ret && FD_ISSET(sock_fds[0],&fds))
		{
		ret = read(sock_fds[0],buf,sizeof(buf));
		if(ret > 0)			
			{
			fprintf(outFile,"READING socket main END - have read = %d and %s\n",ret,buf);
			//INFO_PRINTF3(_L("READING socket main END - have read = %d and %s\n"),ret,buf);
			}
		else
			{
			fprintf(outFile,"READING socket main END - have read = %d \n",ret);
			//INFO_PRINTF2(_L("READING socket main END - have read = %d \n"),ret);
			}		
		}
	else
		{
		fprintf(outFile,"READING socket main ERROR END - have read = %d \n",ret);
		//INFO_PRINTF2(_L("READING socket main ERROR END - have read = %d \n"),ret);
		}
			
	pthread_join(sockwrite1,(void**)&sock_fds);
	if(KErrNone != sock_fds[Write1Err])
		{
		err = KErrGeneral ;
		}
	pthread_join(sockread1,(void**)&sock_fds);
	if(KErrNone != sock_fds[Read1Err] && KErrNone == err)
		{
		err = KErrGeneral ;
		}
	pthread_join(client1,(void**)NULL);
	pthread_join(sockreadwrite,(void**)&sock_fds);
	if((KErrNone != sock_fds[ReadWriteErr1] || KErrNone != sock_fds[ReadWriteErr2]) && KErrNone == err)
		{
		err = KErrGeneral ;
		}
	pthread_join(sockwrite2,(void**)&sock_fds);
	if(KErrNone != sock_fds[Write2Err] && KErrNone == err)
		{
		err = KErrGeneral ;
		}
	pthread_join(client2,(void**)NULL);
	pthread_join(pipewrite,(void**)&sock_fds);
	if(KErrNone != sock_fds[Write2Err] && KErrNone == err)
		{
		err = KErrGeneral ;
		}
	
	close(sock_fds[0]);
	close(sock_fds[1]);		
	fclose(outFile);
	close(fd[0]);
	close(fd[1]);
	close(listen_sock);
//	__UHEAP_MARKEND;
    return err;
	}

TInt CTestSelect::TestMultipleDesc()
    {
    
    return KErrNone;
    
    } 

TInt CTestSelect::TestConnectErrorDesc()
	{
	TInt ret = KErrGeneral;
	struct sockaddr_in host;
	int dest_fd;
	host.sin_port = htons(122);
	host.sin_family = AF_INET;
	host.sin_addr.s_addr = inet_addr("127.0.0.1");
	dest_fd = socket(AF_INET,SOCK_STREAM,0);
	
	TInt blocking;
	
    if(!GetIntFromConfig(ConfigSection(), _L("blocking"),blocking ))
    	{
    	ERR_PRINTF1(_L("Reading case from .ini fails"));
    	return ret;
    	}
	
	if(blocking == 0)
		{
		fcntl( dest_fd, F_SETFL, O_NONBLOCK );
		}
	
	ret = connect(dest_fd,(struct sockaddr*)&host,sizeof(host));
	if(ret == -1)
		{
		INFO_PRINTF2(_L("Connect errno value is %d\n"), errno);		
		if((blocking == 0) && (errno != EINPROGRESS))
			return KErrGeneral;
		}
    
	fd_set readfds;
	fd_set writefds;
	fd_set exceptfds;
	
    FD_ZERO(&writefds);
    FD_SET(dest_fd, &writefds);
    FD_ZERO(&readfds);
    FD_SET(dest_fd, &readfds);
    FD_ZERO(&exceptfds);
    FD_SET(dest_fd, &exceptfds);
    
    int maxfd = dest_fd + 1;
    ret = select(maxfd, &readfds, &writefds, NULL,NULL);    
	if(ret == -1)
		{
		INFO_PRINTF2(_L("select errno value is %d\n"), errno);
		return KErrGeneral;
		}
	else
		{
		INFO_PRINTF2(_L("select ret value is %d\n"), ret);
		}
	
    FD_ZERO(&writefds);
    FD_SET(dest_fd, &writefds);
    FD_ZERO(&readfds);
    FD_SET(dest_fd, &readfds);
    FD_ZERO(&exceptfds);
    FD_SET(dest_fd, &exceptfds);

	ret = select(maxfd, &readfds, NULL, &exceptfds, NULL);
	if(ret == -1)
		{
		INFO_PRINTF2(_L("select errno value is %d\n"), errno);
		return KErrGeneral;
		}
	else
		{
		INFO_PRINTF2(_L("select ret value is %d\n"), ret);
		}

    FD_ZERO(&writefds);
    FD_SET(dest_fd, &writefds);
    FD_ZERO(&readfds);
    FD_SET(dest_fd, &readfds);
    FD_ZERO(&exceptfds);
    FD_SET(dest_fd, &exceptfds);

	ret = select(maxfd, NULL, &writefds, &exceptfds, NULL);
	if(ret == -1)
		{
		INFO_PRINTF2(_L("select errno value is %d\n"), errno);
		return KErrGeneral;
		}
	else
		{
		INFO_PRINTF2(_L("select ret value is %d\n"), ret);
		}

    FD_ZERO(&writefds);
    FD_SET(dest_fd, &writefds);
    FD_ZERO(&readfds);
    FD_SET(dest_fd, &readfds);
    FD_ZERO(&exceptfds);
    FD_SET(dest_fd, &exceptfds);
    
	ret = select(maxfd, NULL, NULL, &exceptfds, NULL);
	if(ret == -1)
		{
		INFO_PRINTF2(_L("select errno value is %d\n"), errno);
		return KErrGeneral;
		}
	else
		{
		INFO_PRINTF2(_L("select ret value is %d\n"), ret);
		}		
	
	ret = connect(dest_fd,(struct sockaddr*)&host,sizeof(host));
	if(ret == -1)
		{
		INFO_PRINTF2(_L("Connect errno value is %d\n"), errno);	
		}	
	
	close(dest_fd);	
	return 0;
	}

TInt CTestSelect::TestSelectOnBadPipeEnd1()
    {
    int fds[2],rc,max;
    fd_set readfds,writefds;
    
    if (pipe(fds) == -1)
    {
       INFO_PRINTF1(_L("Pipe creation failed!\n")) ;
    }
    /* fds[0] - opened for read */
    /* fds[1] - opened for write */
    write(fds[1],"hello",5);  // Writing data onto the pipe, thus setting the read event bit
    if(fds[0] > fds[1])
          max = fds[0] + 1;
    else 
    	  max = fds[1] + 1;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(fds[0],&readfds); 
    FD_SET(fds[0],&writefds); // issue here is:-- trying to set the write operation bit on the fixed read end of the pipe
    // OR.. FD_SET(fds[1],&writefds); FD_SET(fds[1],&readfds); //similar issue here is:-- trying to set the read operation bit on the fixed write end of the pipe
    rc = select(max,&readfds,&writefds, NULL, NULL);
    if (rc == -1)
    {
       INFO_PRINTF1(_L("select failed\n")) ;
       return KErrGeneral ;
    }	
    else
    {
       if(FD_ISSET(fds[0],&readfds))
            INFO_PRINTF1(_L("ready for reading\n\n")) ;
    }
    INFO_PRINTF2(_L("Return value from select is %d\n"), rc);
    close(fds[0]);
    close(fds[1]);
    return 0;
    } 

////One more Test case added to check for writability at the read end of the pipe or readability for the write end of the pipe while notifying the event

void *WritingToPipe(void * p)
  { 
  int *fd;
  fd = (int *)p;
  sleep(3);
  write( *(fd + 1),"hello",5);
  return 0;
  } 

TInt CTestSelect::TestSelectOnBadPipeEnd2()
    {
    int fds[2],rc, max, *p,t;
    fd_set readfds,writefds;
    pthread_t threads1;
    int threadRetVal = 0;
    
    if (pipe(fds) == -1)
    {
       INFO_PRINTF1(_L("Pipe creation failed!\n")) ;
    }
    p = &fds[0];
    /* fds[0] - opened for read */
    /* fds[1] - opened for write */
    // creating the new thread for writing data onto the pipe when select is actually waiting for someone to write into the pipe
    t = pthread_create(&threads1, NULL, WritingToPipe, (void *)p);
        if (t)
        {
            INFO_PRINTF2(_L("ERROR; return code from pthread_create() is %d\n"), t);
        }  
    if(fds[0] > fds[1])
         max = fds[0] + 1;
    else 
    	 max = fds[1] + 1;    
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(fds[0],&readfds); 
    FD_SET(fds[0],&writefds); // issue here is:-- trying to set the write operation bit on the fixed read end of the pipe
    // OR.. FD_SET(fds[1],&writefds);  FD_SET(fds[1],&readfds); // similar issue here is:-- trying to set the read operation bit on the fixed write end of the pipe
    const int timeout = 5000; // 5 seconds
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    rc = select(max,&readfds,&writefds, NULL, &tv);
    if (rc == -1)
    {
       INFO_PRINTF1(_L("select failed\n")) ;
       return KErrGeneral ;
    }	
    else
    {
       if(FD_ISSET(fds[0],&readfds))
           INFO_PRINTF1(_L("ready for reading\n\n")) ;
    }
    INFO_PRINTF2(_L("Return value from select is %d\n"), rc);
    pthread_join(threads1, (void**)threadRetVal);
    close(fds[0]);
    close(fds[1]);
    return 0;
    } 

/* -----------------------------------------------------------------------------
 *Function Name :TestSelectLargeFd
 *API Tested :select()
 *TestCase Description:Testing the behaviour of select with nfds set to FD_SETSIZE.
 *(FD_SETSIZE is macro whose value is the maximum number of file descriptors that a fd_set object can hold information about.) 
 * -----------------------------------------------------------------------------
 */   
TInt CTestSelect::TestSelectLargeFd()
	{
	struct timeval to;
    to.tv_sec = 0;
    to.tv_usec = 100; 
    fd_set readfds; 
    FD_ZERO(&readfds);
    TInt ret = KErrGeneral;
    TInt fd;
    if(!GetIntFromConfig(ConfigSection(), _L("fd"),fd ))
    	{
    	ERR_PRINTF1(_L("Reading case from .ini fails"));
    	return ret;
    	}
   	TInt retVal = select(fd, &readfds, NULL, NULL, &to); 
    TInt expVal;
    if(!GetIntFromConfig(ConfigSection(), _L("expVal"),expVal ))
    	{
    	ERR_PRINTF1(_L("Reading case from .ini fails"));
    	return ret;
    	}
    TInt err;
    if(!GetIntFromConfig(ConfigSection(), _L("err"),err ))
    	{
    	ERR_PRINTF1(_L("Reading case from .ini fails"));
    	return ret;
    	}
   	if(retVal != expVal && errno != err)
   		{
    	ERR_PRINTF4(_L("Expected: %d and  Result: %d ,errno: %d"),expVal, retVal,errno);
   		return ret;
   		}
   	ret = KErrNone ;
    return ret ;
	}
/* -----------------------------------------------------------------------------
 *Function Name :TestSelectRedirDescWrite
 *API Tested :select()
 *TestCase Description:Testing the behaviour of select with redir desc as the fd 
 * -----------------------------------------------------------------------------
 */   
TInt CTestSelect::TestSelectRedirDescWrite()
    {
    TBool stdinpass = EFalse;
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;
    int maxfd = STDIN_FILENO;
   // int select_ret = 0;
    maxfd = STDIN_FILENO;
    FD_ZERO(&writefds);
    FD_SET(STDIN_FILENO,&writefds);
    /*select_ret =*/ select(maxfd+1,&readfds,&writefds,&exceptfds,NULL);
    if( FD_ISSET(STDIN_FILENO,&writefds) )
        {        
        stdinpass = ETrue;
        INFO_PRINTF1(_L("select on writefds of STDIN passed\n")) ;
        }
    else
        {
        INFO_PRINTF1(_L("select on writefds of STDIN failed\n")) ;
        return KErrGeneral;
        }
    maxfd = STDOUT_FILENO;
    FD_ZERO(&writefds);
    FD_SET(STDOUT_FILENO,&writefds);
    /*select_ret = */ select(maxfd+1,NULL,&writefds,NULL,NULL);
    if( FD_ISSET(STDOUT_FILENO,&writefds) )
        {
        if( stdinpass )
            {
            INFO_PRINTF1(_L("select on writefds of STDOUT passed\n")) ;
            return KErrNone;
            }
        else
            {
            INFO_PRINTF1(_L("select on writefds of STDOUT failed\n")) ;            
            }
        }
    return KErrGeneral;
    }

/* -----------------------------------------------------------------------------
 *Function Name :TestSelectConnectFailure
 *API Tested :select()
 *TestCase Description:Testing the behaviour of select on non blocking socket
 *when connect fails
 * -----------------------------------------------------------------------------
 */   
TInt CTestSelect::TestSelectConnectFailure()
    {
    // Loop back
    char *addr="127.0.0.1";
    int sock_fd;
    int check = 0;
    int flags;
    fd_set writefds;
    fd_set readfds;
    fd_set exceptfds;
    int max;
    int select_ret = 0;
   // int ret = KErrGeneral;    
    struct sockaddr_in serv_addr; 
    serv_addr.sin_family = AF_INET;     
    serv_addr.sin_addr.s_addr = inet_addr(addr); 
    serv_addr.sin_port = htons(80);
    sock_fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);  
    if (-1 == (flags = fcntl(sock_fd, F_GETFL, 0)))
        {
        flags = 0;
        }
    check = fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK);
    if ( check != -1 )
        {
        connect(sock_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
        }
    max = sock_fd + 1;
    // readfds,writefds and exceptfds are requested
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    FD_SET(sock_fd, &readfds);
    FD_SET(sock_fd, &writefds);
    FD_SET(sock_fd, &exceptfds);
    select_ret = select(max,&readfds,&writefds,&exceptfds,NULL);
    if( !(select_ret == 3 && FD_ISSET(sock_fd,&readfds) 
            && FD_ISSET(sock_fd,&writefds) && FD_ISSET(sock_fd,&exceptfds)) )
        {
        return KErrGeneral;
        }
    // readfds and writefds are requested
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(sock_fd, &readfds);
    FD_SET(sock_fd, &writefds);
    select_ret = select(max,&readfds,&writefds,NULL,NULL);
    if(!(select_ret == 2 && FD_ISSET(sock_fd,&readfds) 
            && FD_ISSET(sock_fd,&writefds)))
        {
        return KErrGeneral;
        }
    // Only readfds is requested
    FD_ZERO(&readfds);
    FD_SET(sock_fd, &readfds);
    select_ret = select(max,&readfds,NULL,NULL,NULL);
    if(!(select_ret == 1 && FD_ISSET(sock_fd,&readfds)))
        {
        return KErrGeneral;
        }
    // Only writefds is requested
    FD_ZERO(&writefds);
    FD_SET(sock_fd, &writefds);
    select_ret = select(max,NULL,&writefds,NULL,NULL);
    if(!(select_ret == 1 && FD_ISSET(sock_fd,&writefds)))
        {
        return KErrGeneral;
        }
    return KErrNone;
    }

