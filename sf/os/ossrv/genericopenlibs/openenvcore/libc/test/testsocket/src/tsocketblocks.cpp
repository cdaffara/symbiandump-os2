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



 #include "tsocket.h"
 #include <sys/un.h>

const char* tcpdata = "Hello Open C and OPENENV!";
void* myclie(void* pv);
int globalflag=0;
int flag=0;
const char * safdata="hello";

int doClient1(int port, int size);
int doServer1(int port);

const TUint KMaxLine = 100;
const TInt KReadError = -1000;
const TInt KWriteError = -1001;
const TInt KErrSocket = -1002;
const TInt KErrBind = -1003;
const TInt KErrGetSockName=-1004;
const TInt KErrListen=-1006;
const TInt KErrShutdown = -1007;
const TInt KErrRead = -1008;
const TInt KErrConnect = -1009;
const TInt KErrSockAtMark = -1011;
const TInt KErrGetPeerName = -1012;
const TInt KErrIoctl = -1013;
const TInt KErrGetSocketName = -1014;
const TInt KErrGetSockOpt = -1015;
const TInt KErrAccept = -1016;
const TInt KErrGetHostName = -1017;
const TInt KErrLseek = -1018;

const char KSoGet = 1;
const char KSoSet = 2;
const char KSoGetSet = 3;

int read(int newsock_fd,char *line);
int write(int sock_fd);

int read1(int newsock_fd,char *line);
int write1(int sock_fd);

static void* UDPThread(TAny* ptr);
static void* TCPThread(TAny* ptr);
static void* ClientThread(TAny* ptr);
static void* ShutDownThread(TAny* ptr);
static void* UDPThreadSendMsg(TAny* ptr);

static void* TCPThread1(TAny* ptr);
#define PORT 5000


// -----------------------------------------------------------------------------
// CTestSocket::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestSocket::ExampleL( )
    {
    _LIT( Ktsocket, "tsocket: ExampleL" );
    // Print to log file
    INFO_PRINTF1( Ktsocket );
    TPtrC string;
    _LIT( KParam, "Parameter: %S" );
    
    _LIT( Kstring, "Parameter1" );
    TBool res = GetStringFromConfig(ConfigSection(), Kstring, string );
    if(!res)  
		{
     	_LIT(Kerr , "Failed to read input file name") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
		}
     
    INFO_PRINTF2(KParam, &string );
    return KErrNone;

    }


TInt CTestSocket::UDP(  )
    {
    int sock_fd;
	unsigned int sender_len, receiver_len;
	sockaddr_in receiver_addr,sender_addr;
	char line[KMaxLine];
	TInt ret = KErrNone;
	int threadRetVal = 0;
	int error  ;
	pthread_t testThread;
	void *threadRetValPtr = (void*)&threadRetVal;
	sock_fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (sock_fd < 0)
        {
        ret = KErrSocket;
        goto close;
        }
	receiver_addr.sin_family = AF_INET;
	receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	receiver_addr.sin_port = 0;
	if (bind(sock_fd,(sockaddr*)&receiver_addr,sizeof(receiver_addr)) < 0)
		{
        ret = KErrBind;
        goto close;
		}
		
	receiver_len = sizeof(receiver_addr);
	if (getsockname(sock_fd, (sockaddr*)&receiver_addr, &receiver_len) < 0)
		{
		ret = KErrGetSockName;
		goto close;
		}
	pthread_create(&testThread, NULL, &UDPThread, (void*)(receiver_addr.sin_port));
	sender_len = sizeof(sender_addr);
	INFO_PRINTF1(_L("B recvfrom\n"));
	error = recvfrom(sock_fd,line,KMaxLine,0,(sockaddr*)&sender_addr,&sender_len);
	if (error < 0)
		{
		ret = KReadError;
        goto close;
		}
	pthread_join(testThread, &threadRetValPtr);	
	if (threadRetVal < 0)
		{		
		ret = threadRetVal;
		goto close;
		}
close:
	shutdown(sock_fd,SHUT_RDWR);
	close(sock_fd);
	return ret;    
    }

static void* UDPThread(TAny* aParam)
	{
	sockaddr_in receiver_addr;
	int sock_fd;
	char character = 'A';
	unsigned int cnt = 0;
	char line[KMaxLine];
    sock_fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if (sock_fd < 0)
		{
		goto close;
		}
	
    
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

	receiver_addr.sin_family = AF_INET;
	receiver_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);	
	receiver_addr.sin_port = (int)aParam;
	if (sendto(sock_fd,line,KMaxLine,0,(sockaddr*)&receiver_addr,sizeof(receiver_addr)) < 0)
		{
		goto close;
		}
close:		
    shutdown(sock_fd,SHUT_RDWR);
	close(sock_fd);
	return (void*)NULL;
	}

TInt CTestSocket::TCP( )
	{
	int sock_fd,newsock_fd;	
	int error;            
	unsigned int addr_len, serv_len;
	sockaddr_in serv_addr,new_socket;
	TInt ret = KErrNone;
	
	sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if (sock_fd < 0)
        {
        return KErrSocket;
        }
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = 0;

	pthread_t testThread;
	int threadRetVal;
	void *threadRetValPtr = (void*)&threadRetVal;
	
	if (bind(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
		{
		ret = KErrBind;
		goto close;
		}
	serv_len = sizeof(serv_addr);
	if (getsockname(sock_fd, (sockaddr*)&serv_addr, &serv_len) < 0)
		{
		ret = KErrGetSockName;
		goto close;
		}
	
	if (listen(sock_fd,1) < 0)
		{
		ret = KErrListen;
		goto close;
		}
	
	addr_len = sizeof(new_socket);
    
	// Create the thread and thread is client code 
	
	pthread_create(&testThread, NULL, &TCPThread, (void*)(serv_addr.sin_port));

	newsock_fd = accept(sock_fd,(sockaddr*)&new_socket,&addr_len); // Code blocks here
	error = write(newsock_fd);
	if (error < 0)
		{		
		ret = error;
		goto close;
		}
	
close:
	pthread_join(testThread, &threadRetValPtr);
	shutdown(sock_fd,SHUT_RDWR);
	close(sock_fd);
	return ret;	
	}



int write(int sock_fd)
    {
    char line[KMaxLine];    
    
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
    if (send(sock_fd,line,KMaxLine,0) < 0)
        {
        return KWriteError;
        }
        
    return KErrNone;
    }

static void* TCPThread(TAny* aParam)
	{
	sockaddr_in serv_addr;
	int sock_fd;
	char line[KMaxLine ];

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	serv_addr.sin_port = (int)aParam;

    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if (sock_fd < 0)
		{
		goto close;
		}
	if (connect(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
		{
		goto close;
		}
	if (read(sock_fd,line) < 0)
		{
		goto close;
		}
close:
	close(sock_fd);
	return (void*)NULL;
	}
	


int read(int newsock_fd,char *line)
    {    
    int ret;    
    int left = KMaxLine;   
    
    while (1)
        {
        
        ret = recv(newsock_fd,line,left,0);
        
        if (ret < 0)
            {
            return KReadError;
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
    return KErrNone;
    }
    
    
TInt CTestSocket::GetSocketName(  )
	{
	int sock_fd;
    sockaddr_in addr,ss;
    TInt ret = KErrNone;
	char paramets[10][256];
	unsigned int len = 0, addr_len = 0;
    
    /**
     * paramets[0]=socket family;
     * paramets[1]=sockettype
     * paramets[2]=protocol 
     */
	GetParameters(paramets);
    
	sock_fd=socket(atoi(paramets[0]), atoi(paramets[1]), atoi(paramets[2]) );
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;
    if (sock_fd < 0)
    	{
    	ret = KErrSocket;
    	goto close;
        }
    if ( bind(sock_fd,(sockaddr*)&addr,sizeof(addr)) < 0)
		{		
		ret = KErrBind;
		goto close;
		}
	addr_len = sizeof(addr);
	if (getsockname(sock_fd, (sockaddr*)&addr, &addr_len) < 0)
		{
		ret = KErrGetSockName;
		goto close;
		}
	
	len=sizeof(ss);
	if ( getsockname(sock_fd,(sockaddr*)&ss,&len) < 0)
		{
		ret = KErrGetSockName;
	 	return KErrGeneral;
		}
		
	ret = KErrNone;

close:
	close(sock_fd);
	return ret;
	}
	
TInt CTestSocket::GetSocketNameUsingFileDescriptor(  )
	{
	sockaddr_in ss;
	unsigned int len;
	int ret = KErrNone;
	
	len = sizeof(ss);

	// Wrong sockfd, errno shoud be ENOTSOCK
    ret = getsockname(0, (sockaddr*)&ss, &len);
    if (errno != ENOTSOCK)
    	{
    	ret = KErrGetSocketName;
        goto close;
    	}
    if (ret == KErrNone)
    	{
    	ret = KErrGetSocketName;
        goto close;
    	}
    	
	ret = KErrNone;

close:
	return ret;
	}
	
TInt CTestSocket::GetSocketNameUsingInvalidSocketDescriptor(  )
	{
	sockaddr_in ss;
	unsigned int len;
	int ret = KErrNone;
	
	len = sizeof(ss);

	// Invalid fd, errno shoud be EBADF
	ret = getsockname(-1, (sockaddr*)&ss, &len);	    
    if (errno != EBADF )
    	{
        ret = KErrGetSocketName;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrGetSocketName;
        goto close;
    	}
    	
	ret = KErrNone;

close:
	return ret;	
	}
    
TInt CTestSocket::GetSocketNameInvalidSocketBuffer(  )
	{
	int sock_fd;
    sockaddr_in addr,ss;
    TInt ret = KErrNone;
	char paramets[10][256];
	unsigned int len;
    
    /**
     * paramets[0]=socket family;
     * paramets[1]=sockettype
     * paramets[2]=protocol 
     */
	GetParameters(paramets);
    
	sock_fd=socket(atoi(paramets[0]), atoi(paramets[1]), atoi(paramets[2]) );
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;
    if (sock_fd < 0)
    	{
    	ret = KErrSocket;
    	goto close;
        }
    if ( bind(sock_fd,(sockaddr*)&addr,sizeof(addr)) < 0)
		{		
		ret = KErrBind;
		goto close;
		}
	len=sizeof(ss);
	ret = getsockname(sock_fd,(sockaddr*)0,&len);    
    if (errno != EFAULT )
    	{
        ret = KErrGetSocketName;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrGetSocketName;
        goto close;
    	}
    	
	ret = KErrNone;

close:
	close(sock_fd);
	return ret;
	}
    
TInt CTestSocket::GetSocketNameInvalidLength(  )
	{
	int sock_fd;
    sockaddr_in addr,ss;
    TInt ret = KErrNone;
	char paramets[10][256];
	unsigned int len;
    
    /**
     * paramets[0]=socket family;
     * paramets[1]=sockettype
     * paramets[2]=protocol 
     */
	GetParameters(paramets);
    
	sock_fd=socket(atoi(paramets[0]), atoi(paramets[1]), atoi(paramets[2]) );
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;
    if (sock_fd < 0)
    	{
    	ret = KErrSocket;
    	goto close;
        }
   if ( bind(sock_fd,(sockaddr*)&addr,sizeof(addr)) < 0)
		{		
		ret = KErrBind;
		goto close;
		}
	len=1;
	ret = getsockname(sock_fd,(sockaddr*)&ss,&len);	    
    if (errno != KErrNone )
    	{
        ret = KErrGetSocketName;
        goto close;
        }
  
    	
    ret = KErrNone;

close:
	close(sock_fd);
	return ret;
	}

TInt CTestSocket::GetPeerSocketName(  ) 
	{
	int sock_fd;
	int newsock_fd=0;
	int ret = KErrNone;
	char paramets[10][256];
    unsigned int addr_len, serv_len;    
	sockaddr_in serv_addr,new_socket,peer_addr;
	size_t peerlen = sizeof(peer_addr);
    
    /**
     * paramets[0]=socket family;
     * paramets[1]=sockettype
     * paramets[2]=protocol 
     * paramets[3]=for shutdown() socket
     */
	GetParameters(paramets);
    
	sock_fd = socket(atoi(paramets[0]), atoi(paramets[1]), atoi(paramets[2]) );  
   	if (sock_fd < 0)
       	{
   		return KErrSocket;
   		}
   	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = 0;
	if (bind(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
		{
		close(sock_fd);
		return KErrBind;
		}
	serv_len = sizeof(serv_addr);
	pthread_t testThread;
	int threadRetVal;
	void *threadRetValPtr = (void*)&threadRetVal;
	if (getsockname(sock_fd, (sockaddr*)&serv_addr, &serv_len) < 0)
		{
		ret = KErrGetSockName;
		goto close;
		}
		
	if (listen(sock_fd,1) < 0)
		{
		close(sock_fd);
		return KErrListen;
		}
	
	addr_len = sizeof(new_socket);
	// Create the thread and thread is client code 
	pthread_create(&testThread, NULL, &ShutDownThread, (void*)(serv_addr.sin_port));	
	newsock_fd = accept(sock_fd,(sockaddr*)&new_socket,&addr_len); // Code blocks here
    
    //for checking functionality of getpeername on shutdown socket
    if (atoi(paramets[3]) == 1)
		{
		shutdown(newsock_fd,SHUT_RDWR);
		pthread_join(testThread, &threadRetValPtr);
    	}
	if (getpeername(newsock_fd,(sockaddr *)&peer_addr,&peerlen) != 0)
		{
		if (atoi(paramets[3]) == 1) // Expected error
			{
			ret = KErrNone;
			}
		else
			{
			ret = KErrGetPeerName;
			}
		goto close;
		}
	send(newsock_fd, (void*)"a", 1, 0);
close:
	pthread_join(testThread, &threadRetValPtr);
	shutdown(newsock_fd,SHUT_RDWR) ;
	close(sock_fd);
	return ret;
	}
	
TInt CTestSocket::GetPeerSocketNameUsingFileDescriptor(  )
	{
	sockaddr_in ss;
	unsigned int len;
	int ret = KErrNone;
	
	len = sizeof(ss);
	ret = getpeername(0, (sockaddr*)&ss, &len);
	if (errno != ENOTSOCK)
    	{
    	ret = KErrGetPeerName;
        goto close;
    	}
    if (ret == KErrNone)
    	{
    	ret = KErrGetPeerName;
        goto close;
    	}        
    
    ret = KErrNone;

close:
	return ret;
	}
	
TInt CTestSocket::GetPeerSocketNameUsingInvalidSocketDescriptor(  )
	{
	sockaddr_in ss;
	unsigned int len;
	int ret = KErrNone;
	
	len = sizeof(ss);
	ret = getpeername(-1, (sockaddr*)&ss, &len);
	if (errno != EBADF )
    	{
        ret = KErrBind;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrBind;
        goto close;
    	}

	ret = KErrNone;

close:
	return ret;	
	}
		
TInt CTestSocket::GetPeerSocketNameforUnconnectedSocket(  )
	{
	sockaddr_in ss;
	unsigned int len;
	int ret = KErrNone;
	int err;
	len = sizeof(ss);
    int sock_fd;
   
    sock_fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (getpeername(sock_fd, (sockaddr*)&ss, &len) < 0)
		{
        err=errno;
        if(err!=ENOTCONN)
        	{
        	ret=KErrGetPeerName;
        	}        	
        else
        	{
        	ret = KErrNone;
        	}
        goto close;
	 	}
close:
	return ret;
	}
		
TInt CTestSocket::GetPeerSocketNameInvalidLengthSockAddr(  )
	{
	sockaddr_in ss;
	unsigned int len;
	int ret = KErrNone;
	int err;
	len = sizeof(ss);
    int sock_fd;
   
    sock_fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
     
    len = 0;
	if (getpeername(sock_fd, (sockaddr*)&ss, &len) < 0)
		{
        err=errno;
        if(err!= ENOTCONN)
        	{
        	ret=KErrGetPeerName;
        	goto close;
        	}        	
            else
        	{
        	ret = KErrNone;
        	}        
	 	}
	 // Invalid sockaddr, EFAULT is expected in errno
    len = sizeof(ss);
	if (getpeername(sock_fd, (sockaddr*)0, &len) < 0)
		{
        err=errno;
        if(err!=EFAULT)
        	{
        	ret=KErrGetPeerName;
        	goto close;
        	}        	
        else
        	{
        	ret = KErrNone;
        	}
	 	}
	 	
close:
	return ret;
	}
	
TInt CTestSocket::SocketTest( )
	{
	int sock_fd;
	char paramets[10][256];
	int ret = KErrNone;
	int error;
	int errnoExpected = 0;
    
    /**
     * paramets[0]=socket family;
     * paramets[1]=sockettype
     * paramets[2]=protocol 
     * paramets[3]=expected errno
     */
	GetParameters(paramets);
	errnoExpected = atoi(paramets[3]);
    sock_fd = socket(atoi(paramets[0]), atoi(paramets[1]), atoi(paramets[2]) );
    error = errno;
    if (errnoExpected == 0)
    	{
    	if (sock_fd < 0)
        	{
        	ret = KErrSocket;
        	goto close;
        	}
    	}
    else // Error expected
    	{
    	if (sock_fd != -1)
    		{
    		ret = KErrSocket;
        	goto close;
    		}
    	if (error != errnoExpected)
    		{
    		ret = KErrSocket;
        	goto close;
    		}    	
    	}
    ret = KErrNone;
close:
	close(sock_fd);
	return ret;
	}
  	
TInt CTestSocket::BindTest(  )
	{
	int sock_fd;
	int ret = KErrNone;
	sockaddr_in addr;
	
	char paramets[10][256];    
    /**
     * paramets[0]=socket family;
     * paramets[1]=sockettype
     * paramets[2]=protocol 
     */
	GetParameters(paramets);
	
	sock_fd = socket( atoi(paramets[0]), atoi(paramets[1]), atoi(paramets[2]) );  
    if (sock_fd < 0)
        {
        ret = KErrSocket;
        goto close;
        }
	
	addr.sin_family = atoi(paramets[0]);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;
	if (bind(sock_fd,(sockaddr*)&addr,sizeof(addr)) < 0)
		{
        ret = KErrBind;
        goto close;
	 	}	 
close:
	close(sock_fd);
	return ret;
	}
  	
TInt CTestSocket::MultipleBindOnSameSocket(  )
	{
	int sock_fd;
	int ret = KErrNone;
	sockaddr_in addr;
	int error;
	
	char paramets[10][256];    
    /**
     * paramets[0]=socket family;
     * paramets[1]=sockettype
     * paramets[2]=protocol 
     */
	GetParameters(paramets);
	
	sock_fd = socket(atoi(paramets[0]), atoi(paramets[1]), atoi(paramets[2]) );  
    if (sock_fd < 0)
        {
        ret = KErrSocket;
        goto close;
        }
	
	addr.sin_family = atoi(paramets[0]);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;
	if (bind(sock_fd,(sockaddr*)&addr,sizeof(addr)) < 0)
		{
        ret = KErrBind;
        goto close;
	 	}
	if (bind(sock_fd,(sockaddr*)&addr,sizeof(addr)) < 0)
		{
		error = errno;
		if (error != EADDRINUSE)
			{
			ret = KErrBind;
        	goto close;	
			}
        ret = KErrNone;
        goto close;
	 	}	 	
close:
	close(sock_fd);
	return ret;
	}
  	
TInt CTestSocket::BindInvalidAddress(  )
	{
	int sock_fd;
	int ret = KErrNone;
	sockaddr_in addr;
	int error;
	
	char paramets[10][256];    
    /**
     * paramets[0]=socket family;
     * paramets[1]=sockettype
     * paramets[2]=protocol 
     * paramets[3]=Address 
     */
	GetParameters(paramets);
	sock_fd = socket(atoi(paramets[0]), atoi(paramets[1]), atoi(paramets[2]) );  
    if (sock_fd < 0)
        {
        ret = KErrSocket;
        goto close;
        }
	
	addr.sin_family = atoi(paramets[0]);
	addr.sin_addr.s_addr = htonl(atoi(paramets[3]));
	addr.sin_port = 0;
	if (bind(sock_fd,(sockaddr*)&addr,sizeof(addr)) < 0)
		{		
		error = errno;
		if (error != EADDRNOTAVAIL)
			{
			ret = KErrBind;
        	goto close;	
			}
        ret = KErrNone;
        goto close;
	 	}
close:
	close(sock_fd);
	return ret;
	}
	
TInt CTestSocket::BindUsingFileDescriptor(  )
	{
	int ret = KErrNone;
	sockaddr_in addr;
	
	char paramets[10][256];    
    /**
     * paramets[0]=socket family;
     */	
	GetParameters(paramets);
	addr.sin_family = atoi(paramets[0]);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;
	if (bind(0,(sockaddr*)&addr,sizeof(addr)) < 0)
		{
        ret = KErrNone;
        goto close;
	 	}
close:
	return ret;
	}
	
TInt CTestSocket::BindUsingInvalidSocketDescriptor(  )
	{
	int ret = KErrNone;
	sockaddr_in addr;
	
	char paramets[10][256];    
    /**
     * paramets[0]=socket family;
     */	
	GetParameters(paramets);
	
	addr.sin_family = atoi(paramets[0]);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;
	if (bind(-1,(sockaddr*)&addr,sizeof(addr)) < 0)
		{
        ret = KErrNone;
        goto close;
	 	}
close:
	return ret;
	}


	
TInt CTestSocket::BindFailCases()
	{
	int sock_fd;
	int ret = KErrNone;
	struct sockaddr_in socketInfo;

    /* setup the sock_addr data structure */
    socketInfo.sin_family = AF_INET;
    socketInfo.sin_port = 0;
    socketInfo.sin_addr.s_addr = htonl( INADDR_ANY );

    /* create the new socket */
    sock_fd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sock_fd == -1 )
    	{
        ret = KErrSocket;
        goto close;
        }

	/* bind the socket */
	// Wrong socklen, errno shoud be EINVAL
    ret = bind(sock_fd, (struct sockaddr *)&socketInfo, 1);    
    if (ret == KErrNone)
    	{
    	ret = KErrBind;
        goto close;
    	}
    if (errno != EINVAL)
    	{
    	ret = KErrBind;
        goto close;
    	}
    	
    // Wrong socklen, errno shoud be EFAULT
    ret = bind(sock_fd, (struct sockaddr *)0, sizeof(socketInfo));
    if (ret == KErrNone)
    	{
    	ret = KErrBind;
        goto close;
    	}
    if (errno != EFAULT)
    	{
    	ret = KErrBind;
        goto close;
    	}
    	
    // Wrong sockfd, errno shoud be ENOTSOCK
    ret = bind(1, (struct sockaddr *)&socketInfo, sizeof(socketInfo));
    if (errno != ENOTSOCK)
    	{
    	ret = KErrBind;
        goto close;
    	}
    if (ret == KErrNone)
    	{
    	ret = KErrBind;
        goto close;
    	}
    	
    // Wrong sock_fd, errno shoud be EBADF
    ret = bind(sock_fd + 100, (struct sockaddr *)&socketInfo, sizeof(socketInfo)); 
    if (errno != EBADF )
    	{
        ret = KErrBind;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrBind;
        goto close;
    	}
    ret = KErrNone;
close:
	close(sock_fd);
	return ret;
	}
	    
TInt CTestSocket::ListenTest()
    {
    int sock_fd;
    int ret = KErrNone;
    sockaddr_in addr;
	
    /**
     * paramets[0]=socket family;
     * paramets[1]=sockettype
     * paramets[2]=protocol 
     */
    char paramets[10][256];
	GetParameters(paramets);
	
    sock_fd = socket( atoi(paramets[0]), atoi(paramets[1]), atoi(paramets[2]) );  
    if (sock_fd < 0)
       	{
        ret = KErrSocket;
        goto close;
        }
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;

	if ( bind(sock_fd,(sockaddr*)&addr,sizeof(addr)) < 0 )
		{
		ret = KErrBind;
		goto close;
    	}
    	
	if ( listen(sock_fd, 5) < 0 )
		{
		ret = KErrBind;
		goto close;
    	}
    	
close:
    close(sock_fd );
    return ret;  
   }
	
TInt CTestSocket::ListenUsingFileDescriptor(  )
	{
	int ret = KErrNone;
	
	if (listen(0, 5) < 0)
		{
		// Expected errno is ENOTSOCK
		if (errno != ENOTSOCK)
			{
			ret = KErrListen;
        	goto close;
			}
        ret = KErrNone;
        goto close;
	 	}
close:
	return ret;
	}
	
TInt CTestSocket::ListenUsingInvalidSocketDescriptor(  )
	{
	int ret = KErrNone;
	
	if (listen(-1, 5) < 0)
		{
		// Expected errno is EBADF
		if (errno != EBADF)
			{
			ret = KErrListen;
        	goto close;
			}
        ret = KErrNone;
        goto close;
	 	}
close:
	return ret;
	}

	
TInt CTestSocket::ListenUdp()
	{
	int sock_fd;
	sockaddr_in receiver_addr;
	TInt ret = KErrNone;

    sock_fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (sock_fd < 0)
        {
        ret = KErrSocket;
        goto close;
        }
	receiver_addr.sin_family = AF_INET;
	receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	receiver_addr.sin_port = 0;
	if (bind(sock_fd,(sockaddr*)&receiver_addr,sizeof(receiver_addr)) < 0)
		{
        ret = KErrBind;
        goto close;
		}
	if (listen(sock_fd,1) < 0)
		{
		if (errno != EOPNOTSUPP)
			{
			ret = KErrListen;
			goto close;
			}		
		}
	else
		{
		ret = KErrListen;
		goto close;
		}
	
	ret = KErrNone;
	
close:
	shutdown(sock_fd,SHUT_RDWR);
	close(sock_fd);
	return ret;
	}

TInt CTestSocket::ShutdownTest(  )    
	{
	int sock_fd;
	sockaddr_in addr;
	int ret  = KErrNone;
	
	/**
	 * parameter[0]=howto shutdown
	 */
    char paramets[10][256];
	GetParameters(paramets);
    
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);	
   	if (sock_fd < 0)
    	{
   		ret = KErrSocket;
   		goto close;
   		}
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;
	if (bind(sock_fd,(sockaddr*)&addr,sizeof(addr)) < 0)
		{		
   		ret = KErrBind;
   		goto close;
		}
	if ( shutdown(sock_fd,atoi(paramets[0])) < 0 )
		{
		ret = KErrShutdown;
		goto close;
		}
close:
	close(sock_fd);
	return ret;
	}

TInt CTestSocket::ShutDownTestWithInvalidShutdownOption(  )    
	{
	int sock_fd;
	sockaddr_in addr;
	int ret  = KErrNone;
	
	/**	 
	 * parameter[0]=howto shutdown
	 */
    char paramets[10][256];
	GetParameters(paramets);
	
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);	
   	if (sock_fd < 0)
    	{
   		ret = KErrSocket;
   		goto close;
   		}
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;
	if (bind(sock_fd,(sockaddr*)&addr,sizeof(addr)) < 0)
		{		
   		ret = KErrBind;
   		goto close;
		}
		
	if ( shutdown(sock_fd,-1) < 0 )
		{
		ret = KErrNone;
		goto close;
		}
close:
	close(sock_fd);
	return ret;
	}
	

TInt CTestSocket::ShutdownDisconnectedSocket(  )    
	{
	int sock_fd;
	int newsock_fd=0;
	sockaddr_in addr, new_addr;
	unsigned int addr_len;
	int ret = KErrNone;
	
    char paramets[10][256];
	GetParameters(paramets);
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;
	
	pthread_t testThread;
	int threadRetVal;
	void *threadRetValPtr = (void*)&threadRetVal;
		
		
	if (sock_fd < 0)
    	{
   		ret = KErrSocket;
   		goto close;
   		}
	
	if (bind(sock_fd,(sockaddr*)&addr,sizeof(addr)) < 0)
		{
   		ret = KErrBind;
   		goto close;
		}
	addr_len = sizeof(addr);
	if (getsockname(sock_fd, (sockaddr*)&addr, &addr_len) < 0)
		{
		ret = KErrGetSockName;
		goto close;
		}
	if (listen(sock_fd,1) < 0)
		{
   		ret = KErrListen;
   		goto close;
		}
		
	pthread_create(&testThread, NULL, &ShutDownThread, (void*)(addr.sin_port));
	addr_len = sizeof(new_addr);
	newsock_fd = accept(sock_fd,(sockaddr*)&new_addr,&addr_len); // Code blocks here	
	send(newsock_fd, (void*)"a", 1, 0);
	pthread_join(testThread, &threadRetValPtr);
	if (shutdown(newsock_fd,atoi(paramets[0])) < 0)
		{
		ret = KErrNone;
		goto close;
		}	
close:
	close(sock_fd);
	close(newsock_fd);
	return ret;
	}
	
TInt CTestSocket::ShutdownUsingFileDescriptor(  )
	{
	int ret = KErrNone;
	
	if (shutdown(0,SHUT_RDWR) < 0)
		{
        ret = KErrNone;
        goto close;
	 	}
close:
	return ret;
	}
	
TInt CTestSocket::ShutdownUsingInvalidSocketDescriptor(  )
	{
	int ret = KErrNone;
	
	if (shutdown(-1,SHUT_RDWR) < 0)
		{
        ret = KErrNone;
        goto close;
	 	}
close:
	return ret;
	
	}
	
void* ShutDownThread(TAny* aParam)
	{
	sockaddr_in serv_addr;
	int sock_fd;
	char line[1];
	int size = 1;
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);	
	serv_addr.sin_port = (int)aParam;
	TInt ret = KErrNone;

    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if (sock_fd < 0)
		{
		ret = KErrSocket;
		goto close;
		}

    ret = connect(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr));
	if (ret < 0)
		{
		ret = KErrConnect;
		goto close;
		}
	recv(sock_fd, line, size, 0);
close:
	shutdown(sock_fd,SHUT_RDWR); // 2 -> SHUT_RDWT
	close(sock_fd);
	return (void*)NULL;
	}

void* ClientThread(TAny* aParam)
	{
	sockaddr_in serv_addr;
	int sock_fd;
	char line[KMaxLine];
	TInt ret = KErrNone;
	int error = -1;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);	
	serv_addr.sin_port = (int)aParam;
    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if (sock_fd < 0)
		{
		ret = KErrSocket;
		goto close;
		}
    ret = connect(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr));
	if (ret < 0)
		{
		ret = KErrConnect;
		goto close;
		}
	error = read(sock_fd,line);
	if (error < 0)
		{
		ret = KErrRead;
		goto close;
		}
close:
	shutdown(sock_fd,SHUT_RDWR); // 2 -> SHUT_RDWT
	close(sock_fd);	
	return (void*)NULL;
	}

TInt CTestSocket::AcceptTest()
	{
	int sock_fd,newsock_fd;	
	int error;
	unsigned int addr_len, serv_len;
	sockaddr_in serv_addr,new_socket;
	TInt ret = KErrNone;
	int expectedErrno;
	char paramets[10][256];
	
	/**
     * paramets[0]=socket family;
     * paramets[1]=sockettype
     * paramets[2]=protocol 
     * paramets[3]=Expected errno
     */
     GetParameters(paramets);
	
	sock_fd = socket ( atoi(paramets[0]), atoi(paramets[1]), atoi(paramets[2]) );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = 0;
	// Create the thread and thread is client code
	pthread_t testThread;
	int threadRetVal;
	void *threadRetValPtr = (void*)&threadRetVal;
	if (sock_fd < 0)
		{
	  	ret = KErrGeneral;	  	
	  	goto close;
    	}
	if (bind(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
		{
		ret = KErrBind;
	  	goto close;
		}
	serv_len = sizeof(serv_addr);
	if (getsockname(sock_fd, (sockaddr*)&serv_addr, &serv_len) < 0)
		{
		ret = KErrGetSockName;
		goto close;
		}
	if (listen(sock_fd,1) < 0)
		{
		ret = KErrListen;
	  	goto close;
		}
	addr_len = sizeof(new_socket);
	pthread_create(&testThread, NULL, &ClientThread, (void*)(serv_addr.sin_port));
	expectedErrno = atoi(paramets[3]);
	newsock_fd = accept(sock_fd,(sockaddr*)&new_socket,&addr_len); // Code blocks here
	if (expectedErrno != 0)
		{
		if (errno != expectedErrno)
			{
			ret = KErrAccept;
	  		goto close;
			}
		}
	error = write(newsock_fd);
	if (error < 0)
		{		
		ret = KErrWrite;
	  	goto close;
		}
	
	pthread_join(testThread, &threadRetValPtr);

close:	
	shutdown(sock_fd,SHUT_RDWR); // 2 -> SHUT_RDWT
	close(sock_fd);
	return ret;	
	}

TInt CTestSocket::UDPAccept()
	{
	int sock_fd,newsock_fd;	
	unsigned int addr_len;
	sockaddr_in serv_addr,new_socket;
	TInt ret = KErrNone;
	
	sock_fd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = 0;
	if (sock_fd < 0)
		{
	  	ret = KErrGeneral;	  	
	  	goto close;
    	}
	if (bind(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
		{
		ret = KErrBind;
	  	goto close;
		}
	newsock_fd = accept(sock_fd,(sockaddr*)&new_socket,&addr_len); // Code blocks here
	if (newsock_fd > 0)
		{		
			ret = KErrAccept;
	  		goto close;
		}
		
close:	
	shutdown(sock_fd,SHUT_RDWR); // 2 -> SHUT_RDWT
	close(sock_fd);
	return ret;	
	}
	
TInt CTestSocket::AcceptUsingFileDescriptor(  )
	{
	int ret = KErrNone;
	sockaddr_in addr;
	unsigned int addr_len;
	addr_len = sizeof(addr);
	if ( accept(0,(sockaddr*)&addr,&addr_len) < 0 )
		{
        ret = KErrNone;
        goto close;
	 	}
close:
	return ret;
	}
	
TInt CTestSocket::AcceptUsingInvalidSocketDescriptor(  )
	{
	int ret = KErrNone;
	sockaddr_in addr;	
	unsigned int addr_len;
	
	addr_len = sizeof(addr);
	if ( accept (-1,(sockaddr*)&addr,&addr_len) < 0 )
		{
        ret = KErrNone;
        goto close;
	 	}
close:
	return ret;
	}
		
TInt CTestSocket::AcceptFailCases()
	{
	int sock_fd,newsock_fd;	
	unsigned int addr_len;
	sockaddr_in serv_addr,new_socket;
	TInt ret = KErrNone;
	
	sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if (sock_fd < 0)
        {
        return KErrSocket;
        }
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	
	serv_addr.sin_port = 0;

	if (bind(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
		{
		ret = KErrBind;
		goto close;
		}
	if (listen(sock_fd,1) < 0)
		{
		ret = KErrListen;
		goto close;
		}
	
	addr_len = sizeof(new_socket);    

	// Wrong sock_fd, errno shoud be EBADF
	newsock_fd = accept(sock_fd + 100,(sockaddr*)&new_socket,&addr_len);	
    if (errno != EBADF )
    	{
        ret = KErrBind;
        goto close;
        }
    if (newsock_fd == KErrNone)
    	{
    	ret = KErrBind;
        goto close;
    	}
    	
    // Wrong sockfd, errno shoud be ENOTSOCK
    newsock_fd = accept(0,(sockaddr*)&new_socket,&addr_len);
    if (errno != ENOTSOCK)
    	{
    	ret = KErrBind;
        goto close;
    	}
    if (newsock_fd == KErrNone)
    	{
    	ret = KErrBind;
        goto close;
    	}

	
   ret = KErrNone;
close:
	close(sock_fd);
	return ret;
	}

static void * SocketClient(void *)
	{
	sockaddr_in socket_addr;
	int socketfd;
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	socket_addr.sin_port = htons(12345);
	
	if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
		printf("Error creating socket\n");
		return NULL;
		}

	sleep(1);
	if(connect(socketfd,(sockaddr*)&socket_addr,sizeof(socket_addr)) < 0)
		{
		printf("Error connecting, errno = %d\n",errno);
		return NULL;
		}
	if(write(socketfd,"testing",strlen("testing")+1) == -1)
		{
		printf("Error writing to socket, errno = %d\n",errno);
		}
	close(socketfd);
	return NULL;
	}


/*
 * This test case checks if the accept() fails when length passed to it is 0. 
 * It verifies that the  errno returned is EINVAL.
 * */
TInt CTestSocket::AcceptTestZeroAddrLen()
	{
	int sock_fd,newsock_fd;	
	sockaddr_in serv_addr,new_socket;
	TInt ret = KErrNone;
	
	sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if (sock_fd < 0)
        {
        INFO_PRINTF2(_L("socket return with errno = %d"), errno);
        return KErrSocket;
        }
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	
	serv_addr.sin_port = htons(12345);
	if (bind(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
		{
		INFO_PRINTF2(_L("bind return with errno = %d"), errno);
		ret = KErrBind;
		goto close;
		}
	if (listen(sock_fd,1) < 0)
		{
		INFO_PRINTF2(_L("listen return with errno = %d"), errno);
		ret = KErrListen;
		goto close;
		}
	
	newsock_fd = accept(sock_fd,(sockaddr*)&new_socket,0);	
	if (newsock_fd >  0)
        {       
            ret = KErrAccept;
            goto close;
        }
	if (errno != EINVAL )
        {
        INFO_PRINTF2(_L("Accept return with errno = %d"), errno);
        ret = KErrAccept;
        goto close;
        }
    
	ret = KErrNone;
close:
	
    close(sock_fd);
    return ret;
    
	}

TInt CTestSocket::AcceptTestNullAddr()
	{
	int sock_fd,newsock_fd;	
	sockaddr_in serv_addr;
	TInt ret = KErrNone;
	
	sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if (sock_fd < 0)
        {
        INFO_PRINTF2(_L("socket return with errno = %d"), errno);
        return KErrSocket;
        }
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	
	serv_addr.sin_port = htons(12345);
	char buf[20];
	int readLen = 0;
	pthread_t threadId;
	if (bind(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
		{
		INFO_PRINTF2(_L("bind return with errno = %d"), errno);
		ret = KErrBind;
		goto close;
		}
	if (listen(sock_fd,1) < 0)
		{
		INFO_PRINTF2(_L("listen return with errno = %d"), errno);
		ret = KErrListen;
		goto close;
		}
	
	pthread_create(&threadId,NULL,SocketClient,NULL);
	newsock_fd = accept(sock_fd,0,0);	
    if (errno != 0 )
    	{
    	INFO_PRINTF2(_L("Accept return with errno = %d"), errno);
        ret = -1;
        goto close;
        }
    
    readLen = strlen("testing")+1;
    if(read(newsock_fd,buf, readLen) != readLen)
    	{
    	INFO_PRINTF2(_L("read return with errno = %d"), errno);
    	ret = -1;
    	goto close;
    	}
    close(newsock_fd);
close:
	pthread_join(threadId,NULL);
    close(sock_fd);
    return ret;
    
	}

	
TInt CTestSocket::ConnectTestFailCases()
	{
	sockaddr_in serv_addr;
	int sock_fd;
	int ret = KErrNone;
	char paramets[10][256];
    
    /**
     * paramets[0]=socket family;
     * paramets[1]=address
     */
	GetParameters(paramets);
	serv_addr.sin_family = atoi(paramets[0]);
	serv_addr.sin_addr.s_addr = htonl(atoi(paramets[1]));
	serv_addr.sin_port = 0;
	
    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if (sock_fd < 0)
		{
		ret = KErrSocket;
		goto close;
		}
	// Failure expected
    ret = connect(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr));
    if (ret == KErrNone)
    	{
    	ret = KErrConnect;
    	goto close;
    	}
    else
    	{
    	ret = KErrNone;
    	}
    	
    ret = connect(sock_fd,(sockaddr*)&serv_addr,0);
	if (errno != EINVAL )
    	{
        ret = KErrConnect;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrConnect;
        goto close;
    	}
    	
    ret = connect(sock_fd,(sockaddr*)0,sizeof(serv_addr));
	if (errno != EFAULT )
    	{
        ret = KErrConnect;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrConnect;
        goto close;
    	}
    	
    ret = KErrNone;
close:
	shutdown(sock_fd,SHUT_RDWR);
	close(sock_fd);
	return ret;
	}
	
TInt CTestSocket::ConnectUsingFileDescriptor( )
	{
	int ret = KErrNone;
	sockaddr_in addr;	
	
	ret = connect(0,(sockaddr*)&addr,sizeof(addr));
    if (ret < 0)
    	{
    	ret = KErrNone;
    	goto close;
    	}
close:
	return ret;
	}
	
TInt CTestSocket::ConnectUsingInvalidSocketDescriptor( )
	{
	int ret = KErrNone;
	sockaddr_in addr;
	
	ret = connect(-1,(sockaddr*)&addr,sizeof(addr));
    if (ret < 0)
    	{
    	ret = KErrNone;
    	goto close;
    	}
close:
	return ret;
	}
	
TInt CTestSocket::RecvTestFailCases()
	{  
	int sock_fd;
	char buf[5];
	sockaddr_in serv_addr;	
	char paramets[10][256];
	TInt ret = KErrNone;
    
    /**
     * paramets[0]=socket family;
     * paramets[1]=address
     * paramets[2]=protocol
     */
	GetParameters(paramets);
	
    sock_fd = socket ( atoi(paramets[0]), atoi(paramets[1]), atoi(paramets[2]) );
   	if (sock_fd < 0)
   		{
   		ret = KErrSocket;
   		goto close;
   		}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = 0;
	if ( bind(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0 )
		{
		ret = KErrBind;
		goto close;
		}
	if (recv(sock_fd,buf,5,0) < 0 )
		{
		ret = KErrNone;
		goto close;
		}
		
close:
	close(sock_fd);
	return ret;
	}
	
TInt CTestSocket::RecvUsingFileDescriptor( )
	{
	int ret = KErrNone;
	char buf[5];
	
	ret = recv(0,buf,5,0); // Code should fail here
    if (ret < 0)
    	{
    	ret = KErrNone;
    	goto close;
    	}
close:
	return ret;
	}
	
TInt CTestSocket::RecvUsingInvalidSocketDescriptor( )
	{
	int ret = KErrNone;
	char buf[5];
	
	ret = recv(-1,buf,5,0); // Code should fail here
    if (ret < 0)
    	{
    	ret = KErrNone;
    	goto close;
    	}
close:
	return ret;
	}
    
TInt CTestSocket::SendTestFailCases()
	{  
	int sock_fd;
	char buf[5];
	sockaddr_in serv_addr;
	TInt ret = KErrNone;

	char paramets[10][256];
    
    /**
     * paramets[0]=-1/O_NONBLOCK
     */
	GetParameters(paramets);
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = 0;
    
	sock_fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
   	if ( sock_fd < 0 )
   		{
   		ret = KErrSocket;
   		goto close;
   		}
	if ( bind( sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr) ) < 0 )
		{		
		ret = KErrBind;
		goto close;
		}
	if ( atoi(paramets[0]) == O_NONBLOCK )
		{
        fcntl( sock_fd, F_SETFL, O_NONBLOCK );
        }
	if ( send( sock_fd,buf,5,0 ) < 0)
		{
		if (errno == EPIPE)
			{
			ret = KErrNone;
			}
		goto close;
		}
close:
	close(sock_fd);
	return ret;  
	}
	
TInt CTestSocket::SendUsingFileDescriptor( )
	{
	int ret = KErrNone;
	char buf[5];
	
	ret = send(0,buf,5,0); // Code should fail here
    if (ret < 0)
    	{
    	ret = KErrNone;
    	goto close;
    	}
close:
	return ret;
	}
	
TInt CTestSocket::SendUsingInvalidSocketDescriptor( )
	{
	int ret = KErrNone;
	char buf[5];
	
	ret = send(-1,buf,5,0); // Code should fail here
    if (ret < 0)
    	{
    	ret = KErrNone;
    	goto close;
    	}
close:
	return ret;
	}

TInt CTestSocket::SendToTestFailCases()
	{
	int sock_fd;
    int ret = KErrNone;
    char line[KMaxLine];
    unsigned int receiver_len = 0;
    sockaddr_in receiver_addr,dest_addr;
    
    char paramets[10][256];
    
    /**
     * paramets[0]=socket family;
     * paramets[1]=address
     * paramets[2]=protocol
     * parameter[3]=Blocking/Nonblocking case
     * parameter[4]=Size
     * parameter[5]=flags, eg.MSG_OOB
     */
	GetParameters(paramets);
    
    sock_fd = socket( atoi(paramets[0]), atoi(paramets[1]), atoi(paramets[2]) );
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    receiver_addr.sin_port = 0;
    
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);	
    dest_addr.sin_port = receiver_addr.sin_port;
    int error = -1;
    if (sock_fd < 0)
		{
		ret = KErrSocket;
		goto close;
		}
	if ( atoi(paramets[3]) > 0)
		{
		if ( atoi(paramets[3]) == 1) // Set Non-block mode
			{
			fcntl(sock_fd,F_SETFL,O_NONBLOCK);
			}
		}	

	if (bind(sock_fd,(sockaddr*)&receiver_addr,sizeof(receiver_addr)) < 0)
		{		
		ret = KErrBind;
		goto close;
		}
	receiver_len = sizeof(receiver_addr);
	if (getsockname(sock_fd, (sockaddr*)&receiver_addr, &receiver_len) < 0)
		{
		ret = KErrGetSockName;
		goto close;
		}
		
	if ( atoi(paramets[4]) > 0) // set the send window size
		{
		int optionValue = KMaxLine - 50;
		setsockopt(sock_fd,SOL_SOCKET,SO_SNDBUF,&optionValue,sizeof(optionValue));
		}

	 error = sendto(sock_fd, line, KMaxLine, atoi(paramets[5]), (sockaddr*)&dest_addr, sizeof(dest_addr) );
	if (error < 0)
		{
		ret = KErrNone;
		goto close;
		}
close:
	shutdown(sock_fd,SHUT_RDWR); // 2 -> SHUT_RDWT	
	close(sock_fd);
	return ret;
	}
	
TInt CTestSocket::SendToUsingFileDescriptor( )
	{
	int ret = KErrNone;
    char line[KMaxLine];
    int dest_len;
    sockaddr_in dest_addr;
	
	dest_len = sizeof(dest_addr);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);	
	dest_addr.sin_port = 0;
	sendto( 0, line, KMaxLine, 0,(sockaddr*)&dest_addr,dest_len );
    if (ret < 0)
    	{
    	ret = KErrNone;
    	goto close;
    	}
close:
	return ret;
	}
	
TInt CTestSocket::SendToUsingInvalidSocketDescriptor( )
	{
	int ret = KErrNone;
    char line[KMaxLine];
    int dest_len;
    sockaddr_in dest_addr;
	
	dest_len = sizeof(dest_addr);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);	
	dest_addr.sin_port = 0;
	sendto( -1, line, KMaxLine, 0,(sockaddr*)&dest_addr,dest_len );
    if (ret < 0)
    	{
    	ret = KErrNone;
    	goto close;
    	}
close:
	return ret;
	}	

int client(int port)
{
int flag = 0;
int clientsockid = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
if (clientsockid < 0)
{
printf("[Client] Create Tcp socket fail");
return -1;
}

int result = -1;

struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_port = htons(port);
addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

do
{
int ret = connect(clientsockid, (sockaddr*) &addr, sizeof(addr));
if (ret < 0)
{
printf("[Client] connect to server fail\n");
break;
}

ret = send(clientsockid, tcpdata, strlen(tcpdata), flag);
if(ret!=strlen(tcpdata))
	globalflag++;
if (ret < 0)
{
printf("[Client] send data to server fail\n");
break;
}
//int bytes = ret;

printf("[Client] send %d bytes: %s\n",ret,tcpdata);
char buff[50];
char* p = buff;


ret = recv(clientsockid, p, 50, flag);
if (ret < 0)
{
printf("[Client] recv data from server fail\n");
break;
}
p += ret;
*p = '\0';               
printf("[Client] recv %d bytes:  %s\n", p-buff, buff);
result = 0;
}
while (0);
close(clientsockid);
return result;
}

void* myclie(void* pv)
{
client( (int)pv);
return 0;
}

TInt CTestSocket::TestSendReturnValue( )
	{
	// int  ret1=0;
	 pthread_t t;
	 int flag = 0;
	 _LIT(KWelcome,"Hello Open C and OPENENV!\n");
	 INFO_PRINTF1(KWelcome) ;
	 int servsockid = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	 if (servsockid < 0)
	 {
	 printf("[Server] Create Tcp socket fail\n");
	 return -1;
	 }
	 // reuseaddr
	 int set = 1;
	 setsockopt(servsockid, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set));

	// int result = -1;
	 struct sockaddr_in addr;
	 addr.sin_family = AF_INET;
	 addr.sin_port = htons(5000);
	 addr.sin_addr.s_addr = htonl(INADDR_ANY);

	 do
	 {
	 int ret = bind(servsockid, (sockaddr*) &addr, sizeof(addr));
	 if (ret < 0)
	 {
	 printf("[Server] server bind fail\n");
	 break;
	 }

	 ret = listen(servsockid, 2);
	 if (ret < 0)
	 {
	 printf("[Server] listen fail\n");
	 break;
	 }
	 
	 
	 ret = pthread_create(&t, 0 , myclie, (void*) 5000 );
	 

	 socklen_t len = sizeof(addr);
	 int clientid = accept(servsockid, (sockaddr*) &addr, &len);
	 if (clientid < 0)
	 {
	 printf("[Server] accept fail\n");
	 break;
	 }

	 char buff[50];
	 ret = recv(clientid, buff, 50, flag);
	 if (ret < 0)
	 {
	 printf("[Server] recv data from client fail\n");
	 close(clientid);
	 break;
	 }
	 else
	 {
	 buff[ret] = '\0';
	 printf("[Server] recv data from client %d bytes: %s\n", ret, buff);
	 }

	 ret = send(clientid, buff, ret, flag);
	 if (ret < 0)
	 {
	 printf("[Server] send data to client fail\n");
	 close(clientid);
	 break;
	 }
	 else
	 {
	 printf("[Server] send data to client %d bytes: %s\n", ret, buff);
	 }      
	 close(clientid);
	// result = 0;
	 }
	 while (0);
	 close(servsockid);
	
     _LIT(KWait,"wait...\n");
	 INFO_PRINTF1(KWait) ;
	 pthread_join(t, 0);
	 if(globalflag!=0)
		 {
		   _LIT(Kerr , "Send() API Failed to send the actual number of bytes sent") ;
		   INFO_PRINTF1(Kerr) ;
		   return KErrGeneral ;
		 }
	 
	 return 0;
	}  

 TInt CTestSocket::RecvFromTestFailCases()
	{	
	int ret = KErrNone;
	int sock_fd;
	char line[KMaxLine];
    
    char paramets[10][256];    
    /**
     * parameter[0] = Blocking/Nonblocking case
     */
	GetParameters(paramets);    
    
    sock_fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (sock_fd < 0)
		{		
		ret = KErrSocket;
		goto close;
		}
	if ( atoi(paramets[0]) == 1 ) // Set Non-block mode
		{
		fcntl(sock_fd,F_SETFL,O_NONBLOCK);
		}
	if (recvfrom(sock_fd,line,KMaxLine,0,(sockaddr*)NULL,0) < 0)
		{
		ret = KErrNone;
		goto close;
		}
close:	
	close(sock_fd);
	return ret;	
	}
 	
TInt CTestSocket::SocketOptions()
	{
	int sock_fd;
	long int parameter[10];
	int ret = KErrNone;
	int paramLength;

    unsigned int optionLen=0;	
	TPtrC String1,String2, String3, String4, String5, String6, String7;
    TBuf8<256> stringBuffer;
    stringBuffer.Copy(String1);
	_LIT( KString1, "Parameter1" );
     TBool res = GetStringFromConfig(ConfigSection(), KString1, String1);
     if(!res)  
     {
     	_LIT(Kerr , "Failed to read parameter 1") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
     }
    stringBuffer.Copy(String1);
	paramLength = String1.Length();
	char* text=(char *)(stringBuffer.Ptr());
	*(text + paramLength) ='\0';        
	parameter[0] = atol(text);
	
	_LIT( KString2, "Parameter2" );
     res = GetStringFromConfig(ConfigSection(), KString2, String2 );
     if(!res)  
     {
     	_LIT(Kerr , "Failed to read parameter 2") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
     }
    stringBuffer.Copy(String2);
	paramLength = String2.Length();
	text=(char *)(stringBuffer.Ptr());
	*(text + paramLength) ='\0';        
	parameter[1] = atol(text);
	
	_LIT( KString3, "Parameter3" );
     res = GetStringFromConfig(ConfigSection(), KString3, String3 );
     if(!res)  
     {
     	_LIT(Kerr , "Failed to read parameter 3") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
     }
    stringBuffer.Copy(String3);
	paramLength = String3.Length();
	text=(char *)(stringBuffer.Ptr());
	*(text + paramLength) ='\0';        
	parameter[2] = atol(text);
	
	_LIT( KString4, "Parameter4" );
     res = GetStringFromConfig(ConfigSection(), KString4, String4 );
     if(!res)  
     {
     	_LIT(Kerr , "Failed to read parameter 4") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
     }
    stringBuffer.Copy(String4);
	paramLength = String4.Length();
	text=(char *)(stringBuffer.Ptr());
	*(text + paramLength) ='\0';        
	parameter[3] = atol(text);
	
	_LIT( KString5, "Parameter5" );
     res = GetStringFromConfig(ConfigSection(), KString5, String5 );
     if(!res)  
     {
     	_LIT(Kerr , "Failed to read parameter 5") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
     }
    stringBuffer.Copy(String5);
	paramLength = String5.Length();
	text=(char *)(stringBuffer.Ptr());
	*(text + paramLength) ='\0';        
	parameter[4] = atol(text);
	
	_LIT( KString6, "Parameter6" );
     res = GetStringFromConfig(ConfigSection(), KString6, String6);
     if(!res)  
     {
     	_LIT(Kerr , "Failed to read parameter 6") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
     }
    stringBuffer.Copy(String6);
	paramLength = String6.Length();
	text=(char *)(stringBuffer.Ptr());
	*(text + paramLength) ='\0';        
	parameter[5] = atol(text);
	
	_LIT( KString7, "Parameter7" );
     res = GetStringFromConfig(ConfigSection(), KString7, String7 );
     if(!res)  
     {
     	_LIT(Kerr , "Failed to read parameter 7") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
     }
    stringBuffer.Copy(String7);
	paramLength = String7.Length();
	text=(char *)(stringBuffer.Ptr());
	*(text + paramLength) ='\0';        
	parameter[6] = atol(text);
	
	
	/*while ( aItem.GetNextString ( stringPtr ) == KErrNone )
		{
		stringBuffer.Copy(stringPtr);
        paramLength = stringPtr.Length();
        char* text=(char *)(stringBuffer.Ptr());
        *(text + paramLength) ='\0';        
        parameter[i] = atol(text);
        i++;
   		}
   		*/
   	/**
   	 * parameter[0] = domain
	 * parameter[1] = type
	 * parameter[2] = socket des, 
	 * for testing purpose if parameter[2] <= 0,cfg file is supplying sockfd.
	 * parameter[3] = level
	 * parameter[4] = option_name
	 * parameter[5] = option_value
	 * As of now only integer values are supported for option value	
	 * parameter[6] = Get or Set or Both
	 */

	if (parameter[2] > 0)
		{		
    	sock_fd = socket(parameter[0],parameter[1],IPPROTO_TCP);
    	if (sock_fd < 0)
			{
			return KErrSocket;
			}
		}
	else
		{
		sock_fd = parameter[3];
		}
	int level = parameter[3];
	int optionName = parameter[4];
	int optionValue = parameter[5];
	optionLen = sizeof(optionValue);
	int readOptionValue = 0;
	
	/**
	 * For options which allow get and set, first test case 
	 * will set and then get the value.
	 */
	
	if (parameter[6] == KSoGet)	
		{
		ret = getsockopt(sock_fd,level,optionName,(void*)&readOptionValue,&optionLen);
		if (ret != KErrNone)
			{
			ret = KErrGeneral;
			}
		}
	if (parameter[6] & KSoSet)
		{
		ret = setsockopt(sock_fd,level,optionName,&optionValue,optionLen);
		if (ret != KErrNone)
			{
			ret = KErrGeneral;
			}
		}
	if ((parameter[6] == KSoGetSet) & (ret == KErrNone))
		{		
		ret = getsockopt(sock_fd,level,optionName,(void*)&readOptionValue,&optionLen);
		if (ret != KErrNone)
			{
			ret = KErrGeneral;
			}
		else
			{
			if (readOptionValue == optionValue)
				{
				ret = KErrNone;
				}
			else
				{
				ret = KErrGeneral;
				}
			}
		}
	shutdown(sock_fd,SHUT_RDWR); // 2 -> SHUT_RDWT
	close(sock_fd);
	return ret;
	}

TInt CTestSocket::GetSockOptFailCases()
	{
	int sock_fd;
	int ret = KErrNone;
	int level = 0;
	int optionName = 0;
	int optionValue;
	unsigned int optionLen;	
	int error;

	sock_fd = socket(AF_INET,SOCK_STREAM,0);	
    if (sock_fd < 0)
        {
        ret = KErrSocket;
        goto close;
        }

	// errno shoud be EFAULT
	optionLen = sizeof(optionValue);
	ret = getsockopt(sock_fd,level,optionName,(void*)0,&optionLen);
	if (errno != EFAULT )
    	{
        ret = KErrGetSockOpt;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrGetSockOpt;
        goto close;
    	}
	// errno shoud be EINVAL
	optionLen = 0;
	ret = getsockopt(sock_fd,level,optionName,(void*)&optionValue,&optionLen);
	if (errno != EINVAL )
    	{
        ret = KErrGetSockOpt;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrGetSockOpt;
        goto close;
    	}

	// Wrong sockfd, errno shoud be ENOTSOCK
	optionLen = sizeof(optionValue);
    ret = getsockopt(1,level,optionName,(void*)&optionValue,&optionLen);
    if (errno != ENOTSOCK)
    	{
    	ret = KErrGetSockOpt;
        goto close;
    	}
    if (ret == KErrNone)
    	{
    	ret = KErrGetSockOpt;
        goto close;
    	}
    	
    // Wrong sock_fd, errno shoud be EBADF
    ret = getsockopt(sock_fd + 100,level,optionName,(void*)&optionValue,&optionLen);
    if (errno != EBADF )
    	{
        ret = KErrGetSockOpt;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrGetSockOpt;
        goto close;
    	}    
    	
    // Invalid level
    level = -1;
    optionName = SO_OOBINLINE;
    optionValue = 1;
    optionLen = sizeof(optionValue);
    ret = getsockopt(sock_fd,level,optionName,(void*)&optionValue,&optionLen);
    error = errno;
    if (error != ENOPROTOOPT )
    	{
        ret = KErrGetSockOpt;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrGetSockOpt;
        goto close;
    	}
    	
    // Invalid name
    level = SOL_TCP;
    optionName = -1;
    optionValue = 1;
    optionLen = sizeof(optionValue);
    ret = getsockopt(sock_fd,level,optionName,(void*)&optionValue,&optionLen);
    error = errno;
    if (error != ENOPROTOOPT )
    	{
        ret = KErrGetSockOpt;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrGetSockOpt;
        goto close;
    	}
    	
    ret = KErrNone;

close:
	close(sock_fd);
	return ret;
	}

TInt CTestSocket::SetSockOptFailCases()
	{
	int sock_fd;
	int ret = KErrNone;
	int level = 0;
	int optionName = 0;
	int optionValue;
	unsigned int optionLen;	
	int error;

	sock_fd = socket(AF_INET,SOCK_STREAM,0);	
    if (sock_fd < 0)
        {
        ret = KErrSocket;
        goto close;
        }

	// errno shoud be EFAULT
	optionLen = sizeof(optionValue);
	ret = setsockopt(sock_fd,level,optionName,(void*)0,optionLen);
	if (errno != EFAULT )
    	{
        ret = KErrGetSockOpt;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrGetSockOpt;
        goto close;
    	}
	// errno shoud be EINVAL
	optionLen = 0;
	ret = setsockopt(sock_fd,level,optionName,(void*)&optionValue,optionLen);
	if (errno != EINVAL )
    	{
        ret = KErrGetSockOpt;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrGetSockOpt;
        goto close;
    	}

	// Wrong sockfd, errno shoud be ENOTSOCK
	optionLen = sizeof(optionValue);
    ret = setsockopt(1,level,optionName,(void*)&optionValue,optionLen);
    if (errno != ENOTSOCK)
    	{
    	ret = KErrGetSockOpt;
        goto close;
    	}
    if (ret == KErrNone)
    	{
    	ret = KErrGetSockOpt;
        goto close;
    	}
    	
    // Wrong sockfd, errno shoud be EBADF
    ret = setsockopt(sock_fd + 100,level,optionName,(void*)&optionValue,optionLen);
    if (errno != EBADF )
    	{
        ret = KErrGetSockOpt;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrGetSockOpt;
        goto close;
    	}    
    	
    // Invalid level
    level = -1;
    optionName = SO_OOBINLINE;
    optionValue = 1;
    optionLen = sizeof(optionValue);
    ret = setsockopt(sock_fd,level,optionName,(void*)&optionValue,optionLen);
    error = errno;
    if (error != ENOPROTOOPT )
    	{
        ret = KErrGetSockOpt;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrGetSockOpt;
        goto close;
    	}
    	
    // Invalid name
    level = SOL_TCP;
    optionName = -10;
    optionValue = 1;
    optionLen = sizeof(optionValue);
    ret = setsockopt(sock_fd,level,optionName,(void*)&optionValue,optionLen);
    error = errno;
    if (error != ENOPROTOOPT )
    	{
    	INFO_PRINTF2(_L("ERROR NO: %d"), errno);
        ret = KErrGetSockOpt;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrGetSockOpt;
        goto close;
    	}
    	
    ret = KErrNone;

close:
	close(sock_fd);
	return ret;
	}

TInt CTestSocket::SockAtMark()
	{
	int sockfd;
	sockaddr_in selfAddr;
	int ret = KErrNone;
	
	sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);	
    if (sockfd < 0)
        {
        ret = KErrSocket;
        goto close;
        }
        
	selfAddr.sin_family = AF_INET;
	selfAddr.sin_addr.s_addr = INADDR_ANY;
	selfAddr.sin_port = 0;
	
	ret = bind(sockfd,(struct sockaddr*)&selfAddr, sizeof(selfAddr));
	if (ret != KErrNone)
		{
		ret = KErrBind;
		goto close;
		}
	ret = sockatmark(sockfd);
	if (ret != KErrNone)
		{
		ret = KErrSockAtMark;
		goto close;
		}
close:
	close(sockfd);
	return ret;
	}


TInt CTestSocket::BindResvPort()
	{
	int sockfd;
	sockaddr_in selfAddr;
	int ret = KErrNone;
	char paramets[10][256];
	
	/**
     * parameter[0] = Port number to bind
     */
	GetParameters(paramets);
	
	sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);	
    if (sockfd < 0)
        {
        ret = KErrSocket;
        goto close;
        }
        
	selfAddr.sin_family = AF_INET;
	selfAddr.sin_addr.s_addr = INADDR_ANY;
	selfAddr.sin_port = htons(atoi(paramets[0]));	/* echo */
	ret = bindresvport(sockfd, &selfAddr);
	if (ret != KErrNone)
		{
		ret = KErrSockAtMark;
		goto close;
		}
close:
	close(sockfd);
	return ret;
	}


TInt CTestSocket::BindResvPortFailCases()
	{
	sockaddr_in selfAddr;
	int ret = KErrNone;
        
	selfAddr.sin_family = AF_INET;
	selfAddr.sin_addr.s_addr = INADDR_ANY;
	selfAddr.sin_port = htons(1234);
	ret = bindresvport(0, &selfAddr);
	if (ret != KErrNone)
		{
		ret = KErrNone;
		goto close;
		}
        
	selfAddr.sin_family = AF_INET;
	selfAddr.sin_addr.s_addr = INADDR_ANY;
	selfAddr.sin_port = htons(1234);
	ret = bindresvport(5, &selfAddr);
	if (ret != KErrNone)
		{
		ret = KErrNone;
		goto close;
		}
close:
	return ret;
	}

TInt CTestSocket::HErrNoLocation()
	{
	//TInt *ptr = __h_errno_location();
	return KErrNone;
	}

void* UDPThreadSendMsg(TAny* aParam)
	{
	sockaddr_in receiver_addr;
	int sock_fd;
	char line[KMaxLine];
	struct msghdr msg;
	struct iovec iov;
	//TInt ret = KErrNone;
	int error;
    sock_fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    char character = 'A';
    unsigned int cnt = 0;
	if (sock_fd < 0)
		{
		
		//ret = KErrSocket;
		goto close;
		}
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
	receiver_addr.sin_family = AF_INET;
	receiver_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);	
	receiver_addr.sin_port = (int)aParam;
	
	msg.msg_name = &receiver_addr;
	msg.msg_namelen = sizeof(receiver_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_iov->iov_base = line;
	msg.msg_iov->iov_len = KMaxLine;
	msg.msg_control = 0;
	msg.msg_controllen = 0;
	msg.msg_flags = 0;
	error = sendmsg(sock_fd,&msg,0);
	if ( error < 0)
		{
		
		//ret = KWriteError;
		goto close;
		}
		
close:
	close(sock_fd);	
	return (void*)NULL;
	}

TInt CTestSocket::SendMsgRecvMsg()
	{
	TInt ret = KErrNone;
	int sock_fd;
	unsigned int sender_len, receiver_len;
	struct msghdr msg;
	struct iovec iov;
    sockaddr_in receiver_addr,sender_addr;
	char line[KMaxLine];
	int error = -1;
    sock_fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    receiver_addr.sin_port = 0;
    
    pthread_t testThread;
    int threadRetVal;
    void *threadRetValPtr = (void*)&threadRetVal;
    if (sock_fd < 0)
        {
        ret = KErrSocket;
        goto close;
        }
	
	if (bind(sock_fd,(sockaddr*)&receiver_addr,sizeof(receiver_addr)) < 0)
		{		
		shutdown(sock_fd,SHUT_RDWR);
		close(sock_fd);
		ret = KErrBind;
		goto close;
		}
	receiver_len = sizeof(receiver_addr);
	if (getsockname(sock_fd, (sockaddr*)&receiver_addr, &receiver_len) < 0)
		{
		ret = KErrGetSockName;
		goto close;
		}

	// Create the thread and thread is client code 	
	
	pthread_create(&testThread, NULL, &UDPThreadSendMsg, (void*)(receiver_addr.sin_port));
	
	sender_len = sizeof(sender_addr);
	msg.msg_name = &sender_addr;
	msg.msg_namelen = sender_len;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_iov->iov_base = line;
	msg.msg_iov->iov_len = 10;
	msg.msg_control = 0;
	msg.msg_controllen = 0;
	msg.msg_flags = 0;
	error = recvmsg(sock_fd,&msg,0);
	if (error < 0)
		{
		pthread_join(testThread, &threadRetValPtr);
		ret = KReadError;
		goto close;
		}
	
	pthread_join(testThread, &threadRetValPtr);

close:
	shutdown(sock_fd,SHUT_RDWR);
	close(sock_fd);	
	return ret;
	}	
	
TInt CTestSocket::TestRecv()
	{
	sockaddr_in serv_addr;
	int sock_fd;
	char line[2000];
	int ret = KErrNone;
	char address[] = "172.30.176.119";
	int left;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(address);
	serv_addr.sin_port = htons(7000);

    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if (sock_fd < 0)
		{
		ret = KErrSocket;
		goto close;
		}
	
	struct ifreq ifr;
	strcpy(ifr.ifr_name, "Ethernet with Daemon Dynamic IP");
	if ( ioctl(sock_fd,SIOCSIFNAME, &ifr) < 0)
		{
		ret = KErrIoctl;
		goto close;		
		}
	if ( ioctl(sock_fd,SIOCIFSTART , NULL) < 0)
		{
		ret = KErrIoctl;
		goto close;		
		}
		
	if (connect(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
		{
		ret = KErrConnect;
		goto close;
		}
	
	printf("How many bytes to read\n");
	scanf("%d", &left);
	
	if (recv(sock_fd,line,left,0) < 0)
		{
		ret = KErrRead;
		goto close;
		}
		
close:	
	ioctl(sock_fd, SIOCIFSTOP , NULL);
	close(sock_fd);
	ret = KErrNone;
	return ret;
	}
	
TInt CTestSocket::TestReadStream()
	{
	sockaddr_in serv_addr;
	int sock_fd;
	char line[2000];
	int ret = KErrNone;
	char address[] = "172.30.176.119";
	int left;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(address);
	serv_addr.sin_port = htons(7000);

    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if (sock_fd < 0)
		{
		ret = KErrSocket;
		goto close;
		}
	
	struct ifreq ifr;
	strcpy(ifr.ifr_name, "Ethernet with Daemon Dynamic IP");
	if ( ioctl(sock_fd,SIOCSIFNAME, &ifr) < 0)
		{
		ret = KErrIoctl;
		goto close;		
		}
	if ( ioctl(sock_fd,SIOCIFSTART , NULL) < 0)
		{
		ret = KErrIoctl;
		goto close;		
		}
		
	if (connect(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
		{
		ret = KErrConnect;
		goto close;
		}
	
	printf("How many bytes to read\n");
	scanf("%d", &left);
	
	if (read(sock_fd,line,left) < 0)
		{
		ret = KErrRead;
		goto close;
		}
		
close:	
	ioctl(sock_fd, SIOCIFSTOP , NULL);
	close(sock_fd);
	ret = KErrNone;
	return ret;
	}
	
TInt CTestSocket::TestReadDatagram()
	{
	sockaddr_in serv_addr;
	int sock_fd;
	char line[2000];
	int ret = KErrNone;
	char address[] = "172.30.176.119";
	int left;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(address);
	serv_addr.sin_port = htons(7000);

    sock_fd = socket(AF_INET,SOCK_DGRAM,0);
    if (sock_fd < 0)
		{
		ret = KErrSocket;
		goto close;
		}
	
	struct ifreq ifr;
	strcpy(ifr.ifr_name, "Ethernet with Daemon Dynamic IP");
	if ( ioctl(sock_fd,SIOCSIFNAME, &ifr) < 0)
		{
		ret = KErrIoctl;
		goto close;		
		}
	if ( ioctl(sock_fd,SIOCIFSTART , NULL) < 0)
		{
		ret = KErrIoctl;
		goto close;		
		}
		
	if (connect(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
		{
		ret = KErrConnect;
		goto close;
		}
	
	printf("How many bytes to read\n");
	scanf("%d", &left);
	
	if (read(sock_fd,line,left) < 0)
		{
		ret = KErrRead;
		goto close;
		}
		
close:	
	ioctl(sock_fd, SIOCIFSTOP , NULL);
	close(sock_fd);
	ret = KErrNone;
	return ret;
	}

TInt CTestSocket::GetHostName()
	{
	char hostname[256];
	int ret = KErrNone;
	
	ret = gethostname(hostname, 256);
	if (ret == KErrNone)
		{
		return ret;
		}
	else
		{
		return KErrGetHostName;
		}
	}

TInt CTestSocket::GetHostNameNull()
	{
	int ret = KErrNone;
	
	ret = gethostname((char*)0, 256);
	if (errno != EFAULT )
    	{
        ret = KErrGetHostName;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrGetHostName;
        goto close;
    	}

    ret = KErrNone;
    
close:
	return ret;
	}

TInt CTestSocket::GetHostNameZero()
	{
	char hostname[256];
	int ret = KErrNone;
	
	ret = gethostname(hostname, 0);
	if (errno != EFAULT )
    	{
        ret = KErrGetHostName;
        goto close;
        }
    if (ret == KErrNone)
    	{
    	ret = KErrGetHostName;
        goto close;
    	}

    ret = KErrNone;
    
close:
	return ret;
	}

void CTestSocket::GetParameters(char aParamets[10][256])
	{
	TPtrC string1, string2, string3, string4, string5, string6, string7;
	char *textPtr;
	TBuf8<256> stringBuffer;
	
	_LIT( Kstring1, "Parameter1" );
    TBool res = GetStringFromConfig(ConfigSection(), Kstring1, string1);
    if(!res)  return;
    
    stringBuffer.Copy(string1);
	textPtr = (char*)stringBuffer.Ptr();
	*( textPtr + stringBuffer.Length() ) = '\0';
	strcpy(aParamets[0], textPtr);
	
	_LIT( Kstring2, "Parameter2" );
    res = GetStringFromConfig(ConfigSection(), Kstring2, string2);
    if(!res)  
     	return;
    
    stringBuffer.Copy(string2);
	textPtr = (char*)stringBuffer.Ptr();
	*( textPtr + stringBuffer.Length() ) = '\0';
	strcpy(aParamets[1], textPtr);

	_LIT( Kstring3, "Parameter3" );
    res = GetStringFromConfig(ConfigSection(), Kstring3, string3);
    if(!res)  
     	return ;
    
    stringBuffer.Copy(string3);
	textPtr = (char*)stringBuffer.Ptr();
	*( textPtr + stringBuffer.Length() ) = '\0';
	strcpy(aParamets[2], textPtr);

	_LIT( Kstring4, "Parameter4" );
    res = GetStringFromConfig(ConfigSection(), Kstring4, string4);
    if(!res)  
     	return ;
    
    stringBuffer.Copy(string4);
	textPtr = (char*)stringBuffer.Ptr();
	*( textPtr + stringBuffer.Length() ) = '\0';
	strcpy(aParamets[3], textPtr);

	_LIT( Kstring5, "Parameter5" );
    res = GetStringFromConfig(ConfigSection(), Kstring5, string5);
    if(!res)  
     	return;
    
    stringBuffer.Copy(string5);
	textPtr = (char*)stringBuffer.Ptr();
	*( textPtr + stringBuffer.Length() ) = '\0';
	strcpy(aParamets[4], textPtr);

	_LIT( Kstring6, "Parameter6" );
    res = GetStringFromConfig(ConfigSection(), Kstring6, string6);
    if(!res)  
     	return ;
    
    stringBuffer.Copy(string6);
	textPtr = (char*)stringBuffer.Ptr();
	*( textPtr + stringBuffer.Length() ) = '\0';
	strcpy(aParamets[5], textPtr);
	
	_LIT( Kstring7, "Parameter7" );
    res = GetStringFromConfig(ConfigSection(), Kstring7, string7);
    if(!res)  
     	return;
    
    stringBuffer.Copy(string7);
	textPtr = (char*)stringBuffer.Ptr();
	*( textPtr + stringBuffer.Length() ) = '\0';
	strcpy(aParamets[6], textPtr);
	
	}
	
TInt CTestSocket::TestSktlseek()
	{
	int sock_fd;
	int ret=KErrNone;
	sock_fd=socket(AF_INET,SOCK_STREAM,0);
	if (sock_fd < 0)
        {
        ret = KErrSocket;
        return ret;
        }
    if (lseek(sock_fd,0,SEEK_CUR)<0)
    	{
		ret= KErrLseek;
		INFO_PRINTF2(_L("Lseek on socket returned %d"),ret );
		close(sock_fd);		
		return KErrNone;
    	}
    else
    	{
    	close(sock_fd);
    	return KErrGeneral;
    	}
    
	}

TInt CTestSocket::TestSockAfLocal()
	{
	int ret=KErrNone;
	int fd = -1;
    int rslt = -1;
    int sz = 0;
    const char path[] = "testipc";
    struct sockaddr_un server;
    char errstr[256];
    
    server.sun_family = PF_LOCAL;
    strncpy(server.sun_path, path, strlen(path)+1);
    sz = SUN_LEN(&server);
    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0)
    	{
       	sprintf( errstr, strerror(errno) );	// protocol not supported!!
		printf("%s", errstr);
    	}

    rslt = bind(fd, (struct sockaddr*)&server, sz);
    
    if (rslt)
       {
       printf("socket binding error\n");
       }
    unlink(path);
	close(fd);
	
	return ret;
	}

TInt CTestSocket::TestSktfstat()
	{
	int sock_fd,res;
	int ret=KErrNone;
	struct stat buf  ;
	sock_fd=socket(AF_INET,SOCK_STREAM,0);
	if (sock_fd < 0)
        {
        ret = KErrGeneral;
        INFO_PRINTF1(_L("socket creation failed"));
        }
    res=fstat(sock_fd,&buf);
    if(res == 0 && buf.st_mode == S_IFSOCK )
		{
		INFO_PRINTF1(_L("Test Passed"));
		ret = KErrNone;
		}
   	else
    	{
    	INFO_PRINTF1(_L("Test Failed"));
    	ret = KErrGeneral;
    	}
    close(sock_fd);
    return ret;
	}
TInt CTestSocket::TestSktfsync()
	{
	int sock_fd,res;
	int ret=KErrNone;
	sock_fd=socket(AF_INET,SOCK_STREAM,0);
	if (sock_fd < 0)
        {
        ret = KErrGeneral;
        INFO_PRINTF1(_L("socket creation failed"));
        }
    res=fsync(sock_fd);
    if(res == 0)
		{
		INFO_PRINTF1(_L("Test Passed"));
		ret = KErrNone;
		}
   	else
    	{
    	INFO_PRINTF1(_L("Test Failed"));
    	ret = KErrGeneral;
    	}
    close(sock_fd);
    return ret;
	}

TInt CTestSocket::TestGreaterThan16kReadWrite()
	{
	int sockfd = -1;
	char *buffer;
	struct sockaddr_in their_addr;
	int no_bytes = 0;
	fd_set fds;
	struct timeval tv;

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		{
		INFO_PRINTF1(_L("socket creation failed"));
        return KErrGeneral;
        }
	popen("z:\\sys\\bin\\greaterthan16kreadwriteserver.exe","r");
	sleep(5);

	memset(&their_addr, '\0', sizeof(their_addr));
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(PORT); // short, network byte order
	their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	errno = 0;

	if (connect(sockfd, (struct sockaddr *)&their_addr,sizeof(their_addr)) == -1)
		{
        INFO_PRINTF1(_L("connect failed"));
        return KErrGeneral;        
        }
	fcntl (sockfd, F_SETFL, O_NONBLOCK);

	FD_ZERO(&fds);
	FD_SET(sockfd,&fds);
	tv.tv_sec = 1;
	tv.tv_usec = 1;
	buffer = (char *)malloc(100*1024);
	while(1)
		{
		int rc = select(sockfd+1, &fds, NULL, NULL, &tv);
		int size = read(sockfd,buffer,10*1024);
		if(size == -1)
		break;
		if(size > 0)
		no_bytes += size;
		}
	free(buffer);
	close(sockfd);
	return 0;
	}

static void* call_select(void *arg)
	{
	int ret;
	fd_set fdset;
	struct sockaddr_in server;
	char buf[50];
	unsigned int len = sizeof(struct sockaddr);
	int fd = *(int*)arg;
	FD_ZERO(&fdset);
	FD_SET(fd,&fdset);
	ret = select(fd+1,&fdset,NULL,NULL,NULL);
	if (ret < 0)
		{
		printf("select failed");
		return NULL;
		}
	ret = recvfrom(fd,&buf,sizeof(buf),0,(struct sockaddr*)&server,&len);
	*(int*)arg = ret;
	return NULL;
	}

// Port shared by the function TestNotify
TInt Port; 
static void* call_ioctl(void *arg)
	{
	int ret;
	int fd = *(int*)arg;
	struct sockaddr_in server;
	char buf[50];
	unsigned int len = sizeof(struct sockaddr);
	unsigned status=1; //Is the socket ready to receive?
	ret = ioctl(fd,E32IOSELECT,(char*)&status);
	if (ret < 0)
		{
		printf("ioctl failed");
		return NULL;
		}
	ret = recvfrom(fd,&buf,sizeof(buf),0,(struct sockaddr*)&server,&len);
	*(int*)arg = ret;
	return NULL;
	}

static void* clientfun(void* /*param*/)
	{
	int sock;
	int ret;
	char buf[10] = "Hello";
	struct sockaddr_in server;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(Port);
	sock = socket(AF_INET,SOCK_DGRAM,0);
	sleep(5);
	ret = sendto(sock,&buf,sizeof(buf),0,(struct sockaddr*)&server,sizeof(struct sockaddr));
	ret = sendto(sock,&buf,sizeof(buf),0,(struct sockaddr*)&server,sizeof(struct sockaddr));
	if( ret == -1 )
	    {
        printf("sendto failed \n");
	    }
	close(sock);
	return NULL;
	}
TInt CTestSocket::TestNotify()
	{
	int fd,ret; 	 //Desc for open
	struct sockaddr_in addr;
	pthread_t spawn,client;
	void* call_select(void *);
	int threadRetVal = 0;
	void *threadRetValPtr = (void*)&threadRetVal;
	_LIT( KPort, "Port" );
	TBool res = GetIntFromConfig(ConfigSection(), KPort, Port);
	if(!res) 
			return KErrGeneral; 
	addr.sin_family = AF_INET;
	addr.sin_port = htons(Port);
	addr.sin_addr.s_addr = INADDR_ANY;
	fd  = socket(AF_INET , SOCK_DGRAM , 0);
	if ( fd < 0 )
		{
		INFO_PRINTF1(_L("Error in socket\n"));
		return -1; 
		}
	ret = bind (fd,(struct sockaddr*)&addr,sizeof(addr));
	if ( ret < 0 )
		{
		INFO_PRINTF1(_L("Error in bind\n"));
		return -1;
		}
	
	TPtrC TestCall;
	_LIT( KTestCall, "TestCall" );
	res = GetStringFromConfig(ConfigSection(), KTestCall, TestCall);
	if(!res) 
		return KErrGeneral;  
	ret = pthread_create(&client,NULL,&clientfun,NULL);
	if(!TestCall.Compare(_L("Select")))
		{
		INFO_PRINTF1(_L("Checking Select call"));
		ret = pthread_create(&spawn,NULL,&call_select,&fd);
		call_select(&fd);
		}
	else if(!TestCall.Compare(_L("Ioctl")))
		{
		INFO_PRINTF1(_L("Checking Ioctl call"));
		ret = pthread_create(&spawn,NULL,&call_ioctl,&fd);
		call_ioctl(&fd);
		}
	else
		{
		INFO_PRINTF1(_L("Invalid Paramater"));
		return KErrGeneral;
		}
	ret = pthread_join(spawn,&threadRetValPtr);
	ret = pthread_join(client,NULL);
	INFO_PRINTF2(_L("No. of bytes read %d\n"),fd);
	if( fd > 0)
		{
		INFO_PRINTF1(_L("Test Passed"));
		close(fd);
		return KErrNone;
		}
	else
		{
		INFO_PRINTF1(_L("Test Failed"));
		close(fd);
		return KErrGeneral;
		}
	}

/* Test 'peeking' at incoming message using MSG_PEEK flag
 * in a connected socket.
 */ 
TInt CTestSocket::TestRecvMsgPeekTCP( )
	{ 
	int flags;
	int serverSock;
	int serverPort;
	
	//Create server socket
	serverSock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( serverSock < 0 )
		return KErrSocket;
	
	//Set the server socket in non blocking mode
	flags = fcntl( serverSock, F_GETFL, 0 );
	if ( flags < 0 ) 
	    {
		close( serverSock );
		return KErrGetSockOpt;
	    }   

	if ( fcntl( serverSock, F_SETFL, flags | O_NONBLOCK ) < 0 ) 
	    {
		close( serverSock );
		return KErrGetSockOpt;
	    }  
	
	//Bind the server socket
	struct sockaddr_in addr;  
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl( INADDR_ANY );
	addr.sin_port = htons( 0 );
	if( bind( serverSock, ( struct sockaddr* ) &addr, sizeof( addr ) ) < 0 )
		{
		close( serverSock );
		return KErrBind;
		}
	
	//Listen server socket
	if( listen( serverSock, 1 ) < 0 )
		{
		close( serverSock );
		return KErrListen;
		}
 
	//Get the bind port number
	struct sockaddr_in sa;
	socklen_t sockAddrSize = sizeof( sa );
	memset(&sa, 0, sizeof(sa));
	
	if( getsockname( serverSock, ( struct sockaddr * ) &sa, &sockAddrSize ) < 0 )  
		{
		close( serverSock );
		return KErrBind;		
		}
	serverPort = ntohs( sa.sin_port );    
		
	//Create client socket
	int clientSock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	//Set the client socket in non blocking mode
	flags = fcntl( clientSock, F_GETFL, 0 );
	if ( flags < 0 ) 
	    {
		close( serverSock );
		close( clientSock );
		return KErrGetSockOpt;
	    }   

	if ( fcntl( clientSock, F_SETFL, flags | O_NONBLOCK ) < 0 ) 
	    {
	    close( serverSock );
		close( clientSock );
		return KErrGetSockOpt;
	    }  
	
	//Connect to the server
	struct sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
	servAddr.sin_port = htons( serverPort );
	connect( clientSock,( struct sockaddr* ) &servAddr, sizeof( servAddr ) );
  
	//Accept the server sock
	int newSock;
	struct sockaddr acceptAddr;
	unsigned int addrLen = sizeof( acceptAddr );  
	newSock = accept( serverSock, &acceptAddr, &addrLen );
	if( newSock < 0 )
		{
		close( serverSock );
		close( clientSock );
		return KErrAccept;
		}
    
	//Send some data
	const int bufSize = 50;
	char writeData[bufSize];       
	char readData[bufSize];
	memset( writeData, '@', bufSize );
	
	if( write( newSock, writeData, bufSize ) < 0 )
		{
		close( newSock );
		close( serverSock );
		close( clientSock );
		return KWriteError;		
		}
	
	//Wait for the data in client using select
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(clientSock, &fds);
	
	struct timeval tv;
	tv.tv_sec = 10; //10 seconds
	tv.tv_usec = 0;
	
	if( select( clientSock + 1, &fds, 0, 0, &tv ) <= 0 )
		{
		close( serverSock );
		close( clientSock );
		return KErrGetSockOpt;		
		}
	
	//Now, at the client side, peek at the data
	errno = 0;
	if( recv( clientSock, ( void* ) readData, bufSize, MSG_PEEK ) < 0 )
		{
		close( newSock );
		close( serverSock );
		close( clientSock );
		return KErrRead;		
		}
	
	//Now, try reading the data
	if( recv( clientSock, ( void* ) readData, bufSize, 0 ) < 0 )
		{
		close( newSock );
		close( serverSock );
		close( clientSock );
		return KErrRead;
		}

	close( clientSock );
	close( newSock );
	close( serverSock );
	return KErrNone;
	}

/* Test 'peeking' at incoming message using MSG_PEEK flag
 * in a connection-less socket.
 */ 
TInt CTestSocket::TestRecvMsgPeekUDP( )
	{ 
	int flags;
	int serverSock;
	int serverPort;
	
	//Create server socket
	serverSock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( serverSock < 0 )
		return KErrSocket;

	//Set the server socket in non blocking mode
	flags = fcntl( serverSock, F_GETFL, 0 );
	if ( flags < 0 ) 
	    {
		close( serverSock );
		return KErrGetSockOpt;
	    }   

	if ( fcntl( serverSock, F_SETFL, flags | O_NONBLOCK ) < 0 ) 
	    {
		close( serverSock );
		return KErrGetSockOpt;
	    }  
	
	//Bind the server socket
	struct sockaddr_in addr;  
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl( INADDR_ANY );
	addr.sin_port = htons( 0 );
	if( bind( serverSock, ( struct sockaddr* ) &addr, sizeof( addr ) ) < 0 )
		{
		close( serverSock );
		return KErrBind;
		}
	
	//Get the bind port number
	struct sockaddr_in sa;
	socklen_t sockAddrSize = sizeof( sa );
	memset(&sa, 0, sizeof(sa));
	
	if( getsockname( serverSock, ( struct sockaddr * ) &sa, &sockAddrSize ) < 0 )  
		{
		close( serverSock );
		return KErrBind;		
		}
	serverPort = ntohs( sa.sin_port );    
		
	//Create client socket
	int clientSock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	
	//Set the client socket in non blocking mode
	flags = fcntl( clientSock, F_GETFL, 0 );
	if ( flags < 0 ) 
	    {
		close( serverSock );
		close( clientSock );
		return KErrGetSockOpt;
	    }   

	if ( fcntl( clientSock, F_SETFL, flags | O_NONBLOCK ) < 0 ) 
	    {
	    close( serverSock );
		close( clientSock );
		return KErrGetSockOpt;
	    }  
	
	//Send some data
	const int bufSize = 50;
	char writeData[bufSize];       
	char readData[bufSize];
	memset( writeData, '@', bufSize );
	
	struct sockaddr_in sendAddr;    
	memset(&sendAddr, 0, sizeof(sendAddr));  
	sendAddr.sin_family = AF_INET;
	sendAddr.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
	sendAddr.sin_port = htons( serverPort );
	if( sendto( clientSock, writeData, bufSize, 0, ( struct sockaddr* ) &sendAddr, sizeof( sendAddr ) ) < 0 )
		{
		close( serverSock );
		close( clientSock );
		return KWriteError;		
		}
	
	//Wait for the data in server using select
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(serverSock, &fds);
	struct timeval tv;
	tv.tv_sec = 10; //10 seconds
	tv.tv_usec = 0;
	
	if( select( serverSock + 1, &fds, 0, 0, &tv ) <= 0 )
		{
		close( serverSock );
		close( clientSock );
		return KErrGetSockOpt;		
		}
	  
	struct sockaddr_in fromAddr;  
	unsigned int fromLen = sizeof( struct sockaddr_in );
	
	//Now, at the client side, peek at the data
	if( recvfrom( serverSock, readData, bufSize, MSG_PEEK, ( struct sockaddr* ) &fromAddr, &fromLen ) < 0 )
		{
		close( serverSock );
		close( clientSock );
		return KErrRead;		
		}
	
	//Now, try reading the data
	if( recvfrom( serverSock, readData, bufSize, 0, ( struct sockaddr* ) &fromAddr, &fromLen ) < 0 )
		{
		close( serverSock );
		close( clientSock );
		return KErrRead;
		}
	
	close( clientSock );
	close( serverSock );
	return KErrNone;
	}
int sockfd = -1;

#ifdef __SYMBIAN32__
void OpenInterface(char *ifname)
  {
  printf( " ...Opening Interface - START\n" );  
  ifreq ifr;
  strcpy(ifr.ifr_name, ifname);
  sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);  
  int err = ioctl(sockfd,SIOCSIFNAME, &ifr);
  if( err == -1 )
	   {
	   err = errno; // debug   
	   close( sockfd );
	   }
  err = ioctl(sockfd, SIOCIFSTART , &ifr);
  if( err == -1 )
	  {
	  err = errno; // debug   
	  close( sockfd );
	  }
  printf( " ...Opening Interface - END\n" );    
  }

void CloseInterface( char *ifname )
  {
  printf( " ...Closing Interface - START\n" );    
  ifreq ifr;
  strcpy(ifr.ifr_name, ifname);
  int err = ioctl(sockfd, SIOCIFSTOP, &ifr);
  if( err == -1 )
     {
     err = errno; // debug 
     }
  close(sockfd);
  printf( " ...Closing Interface - END\n" );  
  }
#endif

  
TInt CTestSocket::TestLargeUDP()
	{
	#if defined( __SYMBIAN32__ )  && defined ( __WINSCW__ )
	OpenInterface( "Ethernet with Daemon Dynamic IP" );
	#endif  
	TInt err = 0;
	TInt ret = KErrNone;
	TInt flags;
	TInt serverport =0;
	TInt sockoptdata = 49152;
	// Server socket code
	 // ======================================================================================== 
	 INFO_PRINTF1(_L( " ...UDP receiver start-up\n" ));    
	 INFO_PRINTF1(_L( "   ...Creating and initializing receiving socket\n" )); 
	 int server_sockfd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	 if(server_sockfd == -1)
		 {
		 _LIT(KServ,"socket creation failed: server_sockfd = %d,errno = %d");
		 INFO_PRINTF3(KServ,server_sockfd,errno);
		 ret = KErrGeneral;
		 return ret;
		 }
	 flags = fcntl(server_sockfd, F_GETFL, 0);
	 if (flags == -1)
		 {
		 INFO_PRINTF1(_L("fcntl(F_GETFL) FAILED\n"));
		 ret = KErrGeneral;
		 return ret;	
		 }
	 
	if(fcntl(server_sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
		{
		INFO_PRINTF1(_L("Make the socket nonblocking FAILED\n"));
		ret = KErrGeneral;
		return ret;
		}
	 
	 err = setsockopt(server_sockfd, SOL_SOCKET, SO_RCVBUF, (char *) &sockoptdata, sizeof(sockoptdata)); 
	 if(err == -1)
		 {
		 _LIT(KRcv," ...setsockopt(SO_RCVBUF) failed: err = %d, errno = %d\n");
		 INFO_PRINTF3(KRcv,err,errno);
		 ret = KErrGeneral;
		 return ret;
		 }
	 err = setsockopt(server_sockfd, SOL_SOCKET, SO_SNDBUF, (char *) &sockoptdata, sizeof(sockoptdata));
	  if(err == -1)
		  {
		  _LIT(KSnd," ...setsockopt(SO_SNDBUF) failed: err = %d, errno = %d\n");
		  INFO_PRINTF3(KSnd,err,errno);
		  ret = KErrGeneral;
		  return ret;
		  }
  
	 INFO_PRINTF1(_L( "   ...Binding listening socket\n" ));   
	 struct sockaddr_in servaddr;  
	 servaddr.sin_family = AF_INET;
	 servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	 servaddr.sin_port = htons(0);
	 err = bind(server_sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));     
	 if( err == -1 )
		  {
		  _LIT(KInfo,"     ...bind failed: err = %d, errno = %d\n");
		  INFO_PRINTF3( KInfo, err, errno );
		  ret = KErrGeneral;
		  return ret;	
		  }
	 INFO_PRINTF1( _L("   ...Check server port we are listening\n" ));    
	 // Determine local port 
	 struct sockaddr_in sa;
	 struct sockaddr *sockAddrPtr = (struct sockaddr *) &sa;
	 socklen_t sockAddrSize = sizeof(sa);
	  
	 memset(&sa, 0, sizeof(sa));
 	 if (getsockname(server_sockfd, sockAddrPtr, &sockAddrSize) == 0 )  
	    {
	    serverport = ntohs(sa.sin_port); 
	    _LIT(KPort,"Listening port: %d");
	    INFO_PRINTF2(KPort,serverport);
	    }  
	  
	 INFO_PRINTF1(_L( " ...UDP client start-up\n" ));    
	 INFO_PRINTF1( _L("   ...Creating and initializing sending socket\n" )); 	

	 int client_sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	 if(client_sockfd == -1)
		 {
		 _LIT(KCli,"socket creation failed: client_sockfd = %d,errno = %d");
		 INFO_PRINTF3(KCli,client_sockfd,errno);
		 ret = KErrGeneral;
		 return ret;
		 }
	flags = fcntl(client_sockfd, F_GETFL, 0);
	if (flags == -1)
		 {
		 INFO_PRINTF1(_L("fcntl(F_GETFL) FAILED\n"));
		 ret = KErrGeneral;
		 return ret;	
		 }
	if(fcntl(client_sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
		{
		INFO_PRINTF1(_L("Make the socket nonblocking FAILED\n"));
		ret = KErrGeneral;
		return ret;
		}
	err = setsockopt(client_sockfd, SOL_SOCKET, SO_RCVBUF, (char *) &sockoptdata, sizeof(sockoptdata));
 	 if(err == -1)
		 {
		 _LIT(KClircv," ...setsockopt(SO_RCVBUF) failed: err = %d, errno = %d\n");
		 INFO_PRINTF3(KClircv,err,errno);
		 ret = KErrGeneral;
		 return ret;
		 }
     err = setsockopt(client_sockfd, SOL_SOCKET, SO_SNDBUF, (char *) &sockoptdata, sizeof(sockoptdata));
     if(err == -1)
		 {
		 _LIT(KClisnd," ...setsockopt(SO_RCVBUF) failed: err = %d, errno = %d\n");
		 INFO_PRINTF3(KClisnd,err,errno);
		 ret = KErrGeneral;
		 return ret;
		 }
  
 	 INFO_PRINTF1(_L( "   ...Initializing buffers for writing and receiving\n" )); 
	 
	 TInt Parameter1;
	_LIT( KParameter1, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), KParameter1, Parameter1);
	const int bufsize = Parameter1;
    char* writedata = (char *) malloc(bufsize);
    char* rcvdata = (char *) malloc(bufsize);
	 
	 memset(writedata, '@', bufsize);
	  
	 _LIT(KInfo," ...Writing large buffer of data to socket. Bufsize: %d\n");
	 INFO_PRINTF2(KInfo, bufsize ); 
	  
	 struct sockaddr_in addr;    
	 memset(&addr, 0, sizeof(addr));  
	 addr.sin_family = AF_INET;
	 addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	 addr.sin_port = htons(serverport);
	 unsigned int addrlen = sizeof(addr);
	 int written =  sendto(client_sockfd, writedata, bufsize, 0,(struct sockaddr*)&addr,sizeof(addr));
	 _LIT(KErr,"error no is %d\n");
	 INFO_PRINTF2(KErr,errno);
	int receive = recvfrom(server_sockfd,rcvdata, bufsize, 0,(struct sockaddr *)&addr,&addrlen);
	if (receive < 0)
		{
		ret = KReadError;
        }
    if(written != -1)
	    {
	    if(written == receive )
			 {
			 INFO_PRINTF1(_L("Test Passed"));
			 ret = KErrNone;
			 }
		else
			{
			INFO_PRINTF1(_L("Test Failed"));
		 	ret = KErrGeneral;	
			}	
	    }
     else if( written == -1 && errno == EAGAIN)
		 {
		 INFO_PRINTF1(_L("Test Passed"));
		 ret = KErrNone;
		 }
	else
		{
		INFO_PRINTF1(_L("Test Failed"));
		ret = KErrGeneral;	
		}
	
	free(writedata);
	free(rcvdata);
	// Close sockets
	close( server_sockfd );
	close( client_sockfd );
	  
	#if defined( __SYMBIAN32__ )  && defined ( __WINSCW__ )
	CloseInterface( "Ethernet with Daemon Dynamic IP" );
	#endif
	 
	return ret;
    }

size_t bytesAvail( int client_sockfd )
  	{
  	size_t nbytes = 0;
  
	ioctl(client_sockfd, FIONREAD, (char *) &nbytes);
 	return nbytes;
  	}
  
TInt CTestSocket::TestFionread()
	{
	int server_sockfd = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	TInt ret = KErrNone;
  	struct sockaddr_in servaddr;  
  	servaddr.sin_family = AF_INET;
  	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  	servaddr.sin_port = htons(0);
  	int err = ::bind(server_sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));     
  	if( err == -1 )
    	INFO_PRINTF3(_L( "     ...bind failed: err = %d, errno = %d\n"), err, errno );
	struct sockaddr_in sa;
  	struct sockaddr *sockAddrPtr = (struct sockaddr *) &sa;
  
  	socklen_t sockAddrSize = sizeof(sa);
  
  	memset(&sa, 0, sizeof(sa));
  	int serverport = 0;
  	if (getsockname(server_sockfd, sockAddrPtr, &sockAddrSize) == 0 )  
    	{
    	serverport = ntohs(sa.sin_port);  
    	INFO_PRINTF2(_L( "     ...Listening port: %d\n"), serverport ); 
    	}  
    int client_sockfd;
 	client_sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
  	const int bufsize = 1000;
 	char writedata[bufsize];       
  	memset(writedata, '@', bufsize);

  	struct sockaddr_in addr;    
  	memset(&addr, 0, sizeof(addr));  
  	addr.sin_family = AF_INET;
  	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  	addr.sin_port = htons(serverport);
  
  	int written =  ::sendto(client_sockfd, writedata, bufsize, 0,(struct sockaddr*)&addr,sizeof(addr)+1);
  	fd_set fds;
  	FD_ZERO(&fds);
  	FD_SET(server_sockfd, &fds);
   
  	const int timeout = 5000; // 5 seconds

  	struct timeval tv;
  	tv.tv_sec = timeout / 1000;
  	tv.tv_usec = (timeout % 1000) * 1000;
      
  	int selectRet = select(server_sockfd + 1, &fds, 0, 0, &tv );           

	size_t avail = bytesAvail( server_sockfd );
	
	
	close( server_sockfd );
  	close( client_sockfd );

	if(avail == 1028)
		{
		INFO_PRINTF1(_L( "Test Case passed \n")); 
		}
	else
		{
		INFO_PRINTF1(_L( "Test Case passed \n")); 
		ret = KErrGeneral;
		}
	return ret;
	}
TInt CTestSocket::TestBind1()
	{
	TInt ret = KErrNone;
	int sock_fd1, sock_fd2;
	sock_fd1 = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);	
	sock_fd2 = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);	
	
	
	struct sockaddr_in sockAddrIPv4;
	struct sockaddr *sockAddrPtr = 0;
	socklen_t sockAddrSize = 0;
	memset(&sockAddrIPv4, 0, sizeof(sockAddrIPv4));
	sockAddrIPv4.sin_family = AF_INET;
	sockAddrIPv4.sin_port = htons(31180);
	sockAddrIPv4.sin_addr.s_addr = inet_addr("0.0.0.0");
	sockAddrSize = sizeof(sockAddrIPv4);
	sockAddrPtr = (struct sockaddr *) &sockAddrIPv4;    
	
    int bindResult1 = ::bind(sock_fd1, sockAddrPtr, sockAddrSize);	
	if( bindResult1 == -1 )
		{
		INFO_PRINTF2(_L("Bind1 Failed with errno %d"),errno);
		ret = KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("Bind1 Passed"));
		}
	errno = 0;
    int bindResult2 = ::bind(sock_fd2, sockAddrPtr, sockAddrSize);	  
	if( bindResult2 == -1 && errno == EADDRINUSE)
		{
		INFO_PRINTF1(_L("Bind2 Passed"));
		}    
	else
		{
		INFO_PRINTF2(_L("Bind2 Failed with errno %d"),errno);
		ret = KErrGeneral;	
		}	
		
	close( sock_fd1 );
	close( sock_fd2 );	
	return ret;
	}

/* Test connect() using a non-blocking socket. Tests the value returned 
 * when a duplicate connect is issued when the previous connect is already
 * in progress.
 */ 
TInt CTestSocket::TestConnectNonBlocking( )
	{ 
	int flags;
	int serverSock;
	int serverPort;
	int ret = 0;
	
	//Create server socket
	serverSock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( serverSock < 0 )
		return KErrSocket;
	
	//Set the server socket in non blocking mode
	flags = fcntl( serverSock, F_GETFL, 0 );
	if ( flags < 0 ) 
	    {
		close( serverSock );
		return KErrGetSockOpt;
	    }   

	if ( fcntl( serverSock, F_SETFL, flags | O_NONBLOCK ) < 0 ) 
	    {
		close( serverSock );
		return KErrGetSockOpt;
	    }  
	
	//Bind the server socket
	struct sockaddr_in addr;  
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl( INADDR_ANY );
	addr.sin_port = htons( 0 );
	if( bind( serverSock, ( struct sockaddr* ) &addr, sizeof( addr ) ) < 0 )
		{
		close( serverSock );
		return KErrBind;
		}
	
	//Listen server socket
	if( listen( serverSock, 1 ) < 0 )
		{
		close( serverSock );
		return KErrListen;
		}
	
	//Get the bind port number
	struct sockaddr_in sa;
	socklen_t sockAddrSize = sizeof( sa );
	memset(&sa, 0, sizeof(sa));
	
	if( getsockname( serverSock, ( struct sockaddr * ) &sa, &sockAddrSize ) < 0 )  
		{
		close( serverSock );
		return KErrBind;		
		}
	serverPort = ntohs( sa.sin_port );    
		
	//Create client socket
	int clientSock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	//Set the client socket in non blocking mode
	flags = fcntl( clientSock, F_GETFL, 0 );
	if ( flags < 0 ) 
	    {
		close( serverSock );
		close( clientSock );
		return KErrGetSockOpt;
	    }   

	if ( fcntl( clientSock, F_SETFL, flags | O_NONBLOCK ) < 0 ) 
	    {
	    close( serverSock );
		close( clientSock );
		return KErrGetSockOpt;
	    }  
	
	//Connect to the server
	struct sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
	servAddr.sin_port = htons( serverPort );
	ret = connect( clientSock,( struct sockaddr* ) &servAddr, sizeof( servAddr ) );
	//This should return -1 
	if( ret != -1 || errno !=  EINPROGRESS )
	    {
	    close( serverSock );
		close( clientSock );
		return KErrConnect;
		}  	
	
	//Try connecting the client multiple times before the server accpets
	ret = connect( clientSock,( struct sockaddr* ) &servAddr, sizeof( servAddr ) );
	//This should return -1 
	if( ret != -1 )
	    {
	    close( serverSock );
		close( clientSock );
		return KErrConnect;
		}  	
	//Try again..
	ret = connect( clientSock,( struct sockaddr* ) &servAddr, sizeof( servAddr ) );
	//This should return -1
	if( ret != -1 )
	    {
	    close( serverSock );
		close( clientSock );
		return KErrConnect;
		}  
 
	//Accept the server sock
	int newSock;
	struct sockaddr acceptAddr;
	unsigned int addrLen = sizeof( acceptAddr );  
	newSock = accept( serverSock, &acceptAddr, &addrLen );
	if( newSock < 0 )
		{
		close( serverSock );
		close( clientSock );
		return KErrAccept;
		}
	
	//Try connect again (now the socket is connected
	ret = connect( clientSock,( struct sockaddr* ) &servAddr, sizeof( servAddr ) );
	//This should return -1 
	if( ret != -1 )
	    {
	    close( newSock );
	    close( serverSock );
		close( clientSock );
		return KErrConnect;
		}  	
    
	//Send some data
	const int bufSize = 50;
	char writeData[bufSize];       
	char readData[bufSize];
	memset( writeData, '@', bufSize );
	
	if( write( newSock, writeData, bufSize ) < 0 )
		{
		close( newSock );
		close( serverSock );
		close( clientSock );
		return KWriteError;		
		}
	
	//Wait for the data in client using select
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(clientSock, &fds);
	
	struct timeval tv;
	tv.tv_sec = 10; //10 seconds
	tv.tv_usec = 0;
	
	if( select( clientSock + 1, &fds, 0, 0, &tv ) <= 0 )
		{
		close( serverSock );
		close( clientSock );
		return KErrGetSockOpt;		
		}
	
	//Now, try reading the data
	if( recv( clientSock, ( void* ) readData, bufSize, 0 ) < 0 )
		{
		close( newSock );
		close( serverSock );
		close( clientSock );
		return KErrRead;
		}

	close( clientSock );
	close( newSock );
	close( serverSock );
	return KErrNone;
	}



TInt CTestSocket::TCP1( )
	{
	int sock_fd,newsock_fd;	
	int error;            
	unsigned int addr_len, serv_len;
	sockaddr_in serv_addr,new_socket;
	TInt ret = KErrNone;
	
	sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if (sock_fd < 0)
        {
        return KErrSocket;
        }
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = 0;
	pthread_t testThread;
	int threadRetVal;
	void *threadRetValPtr = (void*)&threadRetVal;
		
	if (bind(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
		{
		ret = KErrBind;
		goto close;
		}
	serv_len = sizeof(serv_addr);
	if (getsockname(sock_fd, (sockaddr*)&serv_addr, &serv_len) < 0)
		{
		ret = KErrGetSockName;
		goto close;
		}
	
	if (listen(sock_fd,1) < 0)
		{
		ret = KErrListen;
		goto close;
		}
	
	addr_len = sizeof(new_socket);
    
	// Create the thread and thread is client code 
	pthread_create(&testThread, NULL, &TCPThread1, (void*)(serv_addr.sin_port));

	newsock_fd = accept(sock_fd,(sockaddr*)&new_socket,&addr_len); // Code blocks here
	error = write1(newsock_fd);
	if (error < 0)
		{		
		ret = error;
		goto close;
		}
	
close:
	pthread_join(testThread, &threadRetValPtr);
	shutdown(sock_fd,SHUT_RDWR);
	close(sock_fd);
	return ret;	
	}



int write1(int sock_fd)
    {
    char line[KMaxLine];    
    
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
    if (send(sock_fd,line,KMaxLine,0) < 0)
        {
        return KWriteError;
        }
        
    return KErrNone;
    }

static void* TCPThread1(TAny* aParam)
	{
	sockaddr_in serv_addr;
	int sock_fd,ret;
	char line[KMaxLine ];

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	serv_addr.sin_port = (int)aParam;

    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if (sock_fd < 0)
		{
		goto close;
		}
	if (connect(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
		{
		goto close;
		}
	if (read1(sock_fd,line) < 0)
		{
		goto close;
		}
	errno = 0;
	ret = connect(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr));
	if(ret != -1 && errno != EISCONN)
		{
		goto close;
		}

close:
	close(sock_fd);
	return (void*)NULL;
	}
	


int read1(int newsock_fd,char *line)
    {    
    int ret;    
    int left = KMaxLine;   
    
    while (1)
        {
        
        ret = recv(newsock_fd,line,left,0);
        
        if (ret < 0)
            {
            return KReadError;
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
    return KErrNone;
    }

/*
 * Test the ip address mapping to sockaddr_in structure
 * when the native code returns the address in ipv4 mapped
 * ipv6 format.
 */
TInt CTestSocket::TestV4MappedAddress()
	{
	const char* localIP = "127.0.0.1";
	
	int sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( sock < 0 )
		{
		ERR_PRINTF2(_L("socket() Failed, errno = %d"), errno);
		return KErrGeneral;
		}
	
	const int bufSize = 5;
	char writeData[bufSize];       
	
	//Send a packet to the loopback address. The sender socket will 
	//have the local host ip when queried using getsockname(). This
	//ip is returned by the native API in the ipv6 (ipv4 mapped) format. 
	
	struct sockaddr_in sendAddr;    
	memset(&sendAddr, 0, sizeof(sendAddr));  
	sendAddr.sin_family = AF_INET;
	sendAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sendAddr.sin_port = htons( 2222 );
	if(sendto(sock, writeData, bufSize, 0, (struct sockaddr*) &sendAddr, sizeof( sendAddr )) < 0)
		{
		ERR_PRINTF2(_L("sendto() Failed, errno = %d"), errno);
		close( sock );
		return KErrGeneral;		
		}
		
	struct sockaddr_in localAddr;
	unsigned int sizeAddr = sizeof(localAddr);
	if(getsockname(sock,(struct sockaddr*) &localAddr, &sizeAddr) != 0)
		{
		ERR_PRINTF2(_L("getsockname() Failed, errno = %d"), errno);
		close( sock );
		return KErrGeneral;	
		}
	
	if(strcmp(inet_ntoa(localAddr.sin_addr), localIP) != 0)
		{
		ERR_PRINTF1(_L("getsockname() did not retrieve the local host address"));
		close( sock );
		return KErrGeneral;			
		}

	close( sock );
	return KErrNone;
	}

void * myclie1(void* pv)
	{
    doClient1(5000,(int)pv);
    return 0;
	}

void * myserv1(void* /* pv */)
	{
    doServer1(5000);
    return 0;
	}

int doServer1(int port)
	{
    int s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0)
    	{
        printf("[S] Create Tcp socket fail\n");
        flag++;
        return -1;
    	}
    // reuseaddr
    int set = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set));

    int result = -1;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    do
    	{
        int ret = bind(s, (struct sockaddr*) &addr, sizeof(addr));
        if (ret < 0)
        	{
            printf("[S] server bind fail %d\n", errno);
            flag++;
            break;
        	}

        ret = listen(s, 2);
        if (ret < 0)
        	{
            printf("[S] listen fail %d\n", errno);
            flag++;
            break;
        	}

        socklen_t len = sizeof(addr);
        int cli = accept(s, (struct sockaddr*) &addr, &len);
        if (cli < 0)
        	{
            printf("[S] accept fail %d\n", errno);
            flag++;
            break;
        	}

        char buff[10];
        ret = read(cli, buff, 10);
        if (ret < 0)
        	{
            printf("[S] read data from socket fail %d\n", errno);
            close(cli);
            flag++;
            break;
        	}
        else
        	{
            printf("[S] read data from socket %d bytes\n", ret);
        	}

        ret = write(cli, buff, ret);
        if (ret < 0)
        	{
            printf("[S] write data on socket fail %d\n", errno);
            close(cli);
            flag++;
            break;
        	}
        else
        	{
            printf("[S] write data on socket %d bytes\n", ret);
        	}
		sleep(2);
		printf("[S] socket closed\n");
        close(cli);
        result =0;
    	}
    while (0);
    close(s);
    return result;
	}

int doClient1(int port, int size)
	{
    sleep(1);
    int c = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (c < 0)
    	{
        printf("[C] Create Tcp socket fail\n");
        flag++;
        return -1;
    	}

    int result = -1;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    do
    	{
        int ret = connect(c, (struct sockaddr*) &addr, sizeof(addr));
        if (ret < 0)
        	{
            printf("[C] connect to server fail %d\n", errno);
            flag++;
            break;
        	}

        ret = write(c, safdata, 5);
        if (ret < 0)
        	{
            printf("[C] write data on socket fail %d\n", errno);
            flag++;
            break;
        	}
        printf("[C] write on socket %d bytes\n", ret);
        char buff[10];
        char* p = buff;

        do
        	{
            int left = size + buff - p;
			if( left == 0 )
				left = 1;
            ret = read(c, p, left );
            if (ret < 0)
            	{
                printf("[C] read data from socket fail %d\n", errno);
                flag++;
                break;
            	}
            else
            	{
                printf("[C] read from socket %d bytes\n", ret);
                p += ret;
            	}
        	}
        while (ret > 0);
        // compare data
        int len = p - buff;
        if (len != 5 || memcmp(buff, safdata, 5) != 0)
        	{
            printf("[C] read data unmatch, length is %d/%d\n", len, 5);
            flag++;
            break;
        	}
        result = 0;
    	}
    while (0);
    close(c);

    return result;
	}

TInt CTestSocket::ReadTestFailCase()
	{
	pthread_t t[2];
    pthread_create((unsigned int *)&t[0], (pthread_attr_t *)NULL,myserv1, (void *)0);
    pthread_create((unsigned int *)&t[1], (pthread_attr_t *)NULL, myclie1, (void *)10);

    pthread_join(t[0],0);
    pthread_join(t[1],0);
    if(flag == 0)
    	return KErrNone;
    return KErrGeneral;
	}
/*
 * Test code to verify the fix for DEF128776. 
 * Scenario: On a connected socket, one thread makes a write
 * and another thread reads after a select. 
 */  
static void* Create_connection(void* /*aParam*/)
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

static void* socket_write(void* aSock)
	{
	int *sock_fd = (int*)aSock;
	char buf[] = "Write from second thread";
	int ret = write(*sock_fd,buf,sizeof(buf));
	if (ret < 0)
		{
		exit(1);
		}
	return NULL;
	}

TInt CTestSocket::TestNotify1()
	{
	int ret;
	pthread_t client,sockwrite;
	char buf[50];
	fd_set fds;
	FD_ZERO(&fds);
	int sock_fd;
	int listen_sock = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in host,dest;
	host.sin_port = htons(5002);
	host.sin_family = AF_INET;
	host.sin_addr.s_addr = inet_addr("127.0.0.1");
	ret = bind(listen_sock,(struct sockaddr*)&host,sizeof(host));
	if(ret<0)
		{
		INFO_PRINTF1(_L("Error in bind\n"));
		return -1; 
		}
	ret = listen(listen_sock,1);
	if(ret<0)
		{
		INFO_PRINTF1(_L("Error in listen\n"));
		return -1; 
		}
	unsigned size = sizeof(host);
	ret = pthread_create(&client,(pthread_attr_t*)NULL,&Create_connection,NULL);
	if(ret<0)
		{
		INFO_PRINTF2(_L("Error creating thread:%d\n"),errno);
		exit(1);
		}
//	This socket monitored by 2 threads - one for read, and one for write.
	sock_fd = accept(listen_sock,(struct sockaddr*)&dest,&size);
	close(listen_sock);
//	Spawn the write thread.
	ret = pthread_create(&sockwrite,(pthread_attr_t*)NULL,&socket_write,&sock_fd);
	if(ret<0)
		{
		INFO_PRINTF2(_L("Error creating thread:%d\n"),errno);
		return -1;
		}
//	Read thread.
	FD_SET(sock_fd,&fds);
	ret = select(sock_fd+1,&fds,NULL,NULL,NULL);
	if (ret && FD_ISSET(sock_fd,&fds))
		{
		ret = read(sock_fd,buf,sizeof(buf));
		TPtr8 ptr((TText8 *)buf, ret);
		}
	ret = pthread_join(sockwrite,(void**)NULL);
	ret = pthread_join(client,(void**)NULL);
	close(sock_fd);
	return KErrNone;
	}

TInt CTestSocket::TestSockFcntl()
	{
	// Socket code
	// ============
	INFO_PRINTF1(_L("Creating and initializing socket\n"));
	int sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	
	INFO_PRINTF1(_L("Configuring socket to non-blocking\n"));
	int flags1;
	int flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1)
		{
		INFO_PRINTF1(_L("fcntl(F_GETFL) FAILED\n"));
		return KErrGeneral;
		}
	flags = flags | O_NONBLOCK;
	if ((flags1 = fcntl(sockfd, F_SETFL, flags)) == -1) 
		{
		INFO_PRINTF1(_L("Make the socket nonblocking FAILED\n"));
		return KErrGeneral;
		} 
	
	if(flags1 != (flags & (O_APPEND|O_NONBLOCK|O_SYNC|O_ACCMODE)))
		{
		INFO_PRINTF1(_L("fcntl returned wrong flag values\n"));
		return KErrGeneral;
		}
		
	close(sockfd);
	return KErrNone;
	}

//#warning Replace with correct iap name
const char* g_IapName = "Ethernet with Daemon Dynamic IP";
//#warning Use proper ip
const char* g_Ip = "10.192.204.41";

TInt CTestSocket::create_udp_socket(int port)
    {
    int    s;
    struct ifreq ifr;
    int err = KErrNone;

    struct sockaddr_in host_address;
    s=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset((void*)&host_address, 0, sizeof(host_address));
    host_address.sin_family=PF_INET;
    host_address.sin_addr.s_addr=INADDR_ANY;
    host_address.sin_port=htons(port);
    
    strcpy(ifr.ifr_name, g_IapName);
    err = ioctl(s, SIOCSIFNAME, &ifr);
	INFO_PRINTF2(_L("SIOCSIFNAME completed with %d\n"),err);
    if(KErrNone != err)
    	{
    	return KErrGeneral;
    	}
    err = ioctl(s, SIOCIFSTART, &ifr);
	INFO_PRINTF2(_L("SIOCIFSTART completed with %d\n"),err);
    if(KErrNone != err)
    	{
    	return KErrGeneral;
    	}
    
    err = bind(s, (struct sockaddr*)&host_address, sizeof(host_address));
	INFO_PRINTF2(_L("bind completed with %d\n"),err);
    if(KErrNone != err)
    	{ 	
    	return KErrGeneral;
    	}
    return s;
    }

TInt CTestSocket::send_packet(int s)
    {
    int j;
    char buffer[1000];
    struct sockaddr_in target_host_address;

    target_host_address.sin_family=PF_INET;
    target_host_address.sin_port=htons(5000);

    if (inet_aton(g_Ip, &target_host_address.sin_addr)==0) 
        {
		INFO_PRINTF1(_L("inet_aton() failed\n"));
        return KErrGeneral;
        }
    
    for (j = 0; j < 100; j++) {
        buffer[j] = (unsigned char)((int) (255.0*rand()/(RAND_MAX+1.0)));
    }

    return sendto(s, buffer, 100, 0, 
        (struct sockaddr*)&target_host_address, sizeof(struct sockaddr));    
    }

TInt CTestSocket::close_socket(int fd)
    {
    // (doesn't really matter if we call SIOCIFSTOP or not)
    struct ifreq ifr;
    int err = KErrNone;
    strcpy(ifr.ifr_name, g_IapName);
    err = ioctl(fd, SIOCSIFNAME, &ifr);
	INFO_PRINTF2(_L("SIOCSIFNAME completed with %d\n"),err);
    if(KErrNone != err)
    	{
    	return KErrGeneral;
    	}
    err = ioctl(fd, SIOCIFSTOP, &ifr);
    INFO_PRINTF2(_L("SIOCIFSTOP completed with %d\n"),err);
    if(KErrNone != err)
    	{    	
    	return KErrGeneral;
    	}

    close(fd);
    return err;
    }

TInt CTestSocket::TestSockIoctl()
	{
    int val;
    int sockfd = -1;
    int err = KErrNone;
    
    sockfd = create_udp_socket(6667);
    if(sockfd < 0)
    	{
    	INFO_PRINTF2(_L("create_udp_socket returned %d\n"),sockfd);
    	return KErrGeneral;
    	}
    val = send_packet(sockfd);
    INFO_PRINTF2(_L("sent %d bytes\n"),val);
    err = close_socket(sockfd);
    if(KErrNone != err)
    	{
    	INFO_PRINTF2(_L("close_socket returned %d\n"),err);
    	return KErrGeneral;
    	}
    
    sleep(1);
    
    sockfd = -1;
    sockfd = create_udp_socket(6667);
    if(sockfd < 0)
    	{
    	INFO_PRINTF2(_L("create_udp_socket returned %d\n"),sockfd);
    	return KErrGeneral;
    	}
    val = send_packet(sockfd); // Causes iap query dialog
    INFO_PRINTF2(_L("sent %d bytes\n"),val);
    err = close_socket(sockfd);
    if(KErrNone != err)
    	{
    	INFO_PRINTF2(_L("close_socket returned %d\n"),err);
    	return KErrGeneral;
    	}
    
    return KErrNone;
	}

int sockfd1 = -1;

int checkip()
	{	
	int client_sockfd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	struct sockaddr_in addr;  
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr( "198.133.219.25" );
	addr.sin_port = htons(80);
	int err = connect(client_sockfd,(struct sockaddr*)&addr,sizeof(addr));
	
	// Determine local port 
	struct sockaddr_in sa;
	struct sockaddr *sockAddrPtr = (struct sockaddr *) &sa;
	socklen_t sockAddrSize = sizeof(sa);
	
	memset(&sa, 0, sizeof(sa));
	if (getsockname(client_sockfd, sockAddrPtr, &sockAddrSize) == 0 )  
		{
		printf( "     ...Listening IP: %s\n", inet_ntoa(sa.sin_addr)); 
		}
	
	shutdown( client_sockfd, SHUT_RDWR );  
	close( client_sockfd ); 
	return 0;
	}

void* client_connection (void *a)
	{
	int *serverport;            
	serverport = (int *) a;  /* type cast to a pointer to thdata */
	int ret;
	struct sockaddr_in host;
	int dest_fd;
	char buf[50];
	strcpy(buf, "HelloWorld?");
	
	host.sin_port = htons (*serverport);
	host.sin_family = AF_INET;
	host.sin_addr.s_addr = inet_addr ("10.253.2.197");
	
	dest_fd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ret = connect (dest_fd, (struct sockaddr *) &host, sizeof (host));
	if(ret!=0)
		{
		//this gives me KErrHostUnreach
		printf("The connect returned with errno = %d\n", errno);
		}
	ret = write(dest_fd,buf,12);
	if(ret < 0)
		{
		printf("write failed\n %d\n",errno);
		}
	else 
		{
		printf("write passed %d\n",ret);
		}
	shutdown( dest_fd, SHUT_RDWR );  
	close( dest_fd );
	return NULL;
	}

//ECONNRESET
TInt CTestSocket::SockErrnoTest()
	{
	#if defined( __SYMBIAN32__ )  && defined ( __WINSCW__ )
		OpenInterface( "Ethernet with Daemon Dynamic IP" ); 
	#endif  
	checkip();
	
	int err = 0;
	int server_sockfd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	// Receive OOB data inline
	int sockoptdata = 1;
	err = setsockopt(server_sockfd, SOL_SOCKET, SO_OOBINLINE, (char *) &sockoptdata, sizeof(sockoptdata));
	if( err == -1 )
		{
		INFO_PRINTF3(_L( "     ...setsockopt(SO_OOBINLINE) failed: err = %d, errno = %d\n"), err, errno );
		}
	   
	// Bind
	struct sockaddr_in addr;  
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(0);
	err = bind(server_sockfd,(struct sockaddr*)&addr,sizeof(addr));
	  
	// Listen
	err = listen(server_sockfd,1);
	
	INFO_PRINTF1(_L( "   ...Check local port we are listening\n" ));    
	
	// Determine local port 
	struct sockaddr_in sa;
	struct sockaddr *sockAddrPtr = (struct sockaddr *) &sa;
	socklen_t sockAddrSize = sizeof(sa);
	
	memset(&sa, 0, sizeof(sa));
	int serverport;
	if (getsockname(server_sockfd, sockAddrPtr, &sockAddrSize) == 0 )  
		{
		serverport = ntohs(sa.sin_port);  
		}
	pthread_t client;
	int pthread_ret = pthread_create (&client, (pthread_attr_t *) NULL, &client_connection,
	(void *) &serverport);
	
	unsigned int addr_len;  
	int newsockfd = accept( server_sockfd, (sockaddr*)&newsockfd,&addr_len); 
	sleep(2);
	shutdown( server_sockfd, SHUT_RDWR );
	close(server_sockfd);   
	
	const int bufsize = 20000;
	char readdata[bufsize];       
	
	int myerr = 0;
	TInt ret = KErrNone;
	while (myerr >= 0) 
		{        
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(newsockfd, &fds);
		
		const int timeout = 10000; 
		
		struct timeval tv;
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;
		
		int selectRet = select(newsockfd + 1, &fds, 0, 0, &tv );           
		if( selectRet == 0 )
			{
			myerr = -1;
			INFO_PRINTF1(_L( "   ...select time limit expired\n" ));  
			}
	    else
	    	{
	    	INFO_PRINTF2(_L( "   ...Data available for reading (selectRet=%d): \n"), selectRet);
	    	}       
	    
		size_t avail = bytesAvail( newsockfd );     
		sleep(2);
		int received = read( newsockfd, readdata, bufsize);
		if( received == -1 )
			{
			myerr = -1;
			ret = errno;
		    INFO_PRINTF2(_L( "   ...Read FAILED: errno %d\n"), errno );      
			}
		else
			{
			INFO_PRINTF2(_L( "   ...%d of data received from socket\n"), received );
			if( received == 0 )
				{
				myerr = -1;
				ret = errno;
				}
			}
		}
	shutdown( newsockfd, SHUT_RDWR );
	close( newsockfd );

	#if defined( __SYMBIAN32__ )  && defined ( __WINSCW__ )
	CloseInterface( "Ethernet with Daemon Dynamic IP" );
	#endif
	if(ret!= ECONNRESET)
		return ret;
	else
		return KErrNone;
}

/* test case to check socket send() api over a broken connection

expected result:If a socket client writes any data when the server side socket is closed,
then the send() api should return positive value for the first time and the next send() calls should fail returning -1.
*/



void* create_socket( TAny* aParam )
    {
    sockaddr_in serv_addr;
    int err1;
    int ret = 0;
    char *msg ="testing socket send";
    int sock_fd;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = (int)aParam;
    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if (sock_fd < 0)
        {
        ret = -1;
        goto close;
        }
    if (connect(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
        {
        ret = -1;
        goto close;
        }
    sleep(5);  
    err1 = send(sock_fd, msg, 5, 0);
    if(err1 < 0)
        {
        ret = -1;
        goto close;    
        }
    sleep(5);
    err1 = send(sock_fd, msg, 5, 0);
    if(err1 ==  -1 )
       {
        printf("send() failed returned errno = %d", errno);
       }
    else
        {
        ret = -1;
        }
    close:
    close(sock_fd);
    return (void*)ret;
    }


TInt CTestSocket::SockSendOnClosedConn()
    {
    int sock_fd,newsock_fd;
    unsigned int addr_len;  
    sockaddr_in serv_addr,new_socket;
    TInt ret = KErrNone;
    
    int threadRetVal = 0;
    void *threadRetValPtr = (void*)&threadRetVal;
    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if (sock_fd < 0)
        {
        return KErrSocket;
        }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(12349);
    pthread_t clientThread;
    if (bind(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
        {
        INFO_PRINTF2(_L("bind return with errno = %d"), errno);
        return  KErrBind;
        }
    if(-1 == listen(sock_fd, 1))
       {
         printf("error listen failed");
         close(sock_fd);
         return  -1;
       }

    pthread_create(&clientThread, NULL, &create_socket, (void*)(serv_addr.sin_port));
    addr_len = sizeof(new_socket);
    newsock_fd = accept(sock_fd,(sockaddr*)&new_socket,&addr_len);  
    if (errno != 0 )
        {
        INFO_PRINTF2(_L("Accept return with errno = %d"), errno);
        ret = -1;
        }
   
    close(newsock_fd);
    close(sock_fd);
    pthread_join(clientThread, &threadRetValPtr );
    if (threadRetVal < 0)
        {  
        INFO_PRINTF1(_L("SockSendOnClosedConn failed"));
        ret = threadRetVal;
        }
    return ret; 
    }


void* createNwrite_socket(TAny* aParam)
    {
    sockaddr_in serv_addr;
        int err1;
        int ret = 0;
        char *msg ="testing socket send";
        int sock_fd;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serv_addr.sin_port = (int)aParam;
        sock_fd = socket(AF_INET,SOCK_STREAM,0);
        if (sock_fd < 0)
            {
            ret = -1;
            goto close;
            }
        if (connect(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
            {
            ret = -1;
            goto close;
            }
        sleep(5);  
        err1 = write(sock_fd, msg, strlen(msg)+1);
        if(err1 < 0)
            {
            ret = -1;
            goto close;    
            }
        sleep(5);
        err1 = write(sock_fd, msg,strlen(msg)+1);
        if(err1 ==  -1 )
           {
            printf("write() failed returned errno = %d", errno);
           }
        else
            {
            ret = -1;
            }
        close:
        close(sock_fd);
        return (void*)ret;

    }
TInt CTestSocket::SockWriteOnClosedConn()
    {
    
        int sock_fd,newsock_fd;
        unsigned int addr_len;  
        sockaddr_in serv_addr,new_socket;
        TInt ret = KErrNone;
        int threadRetVal = 0;
        void *threadRetValPtr = (void*)&threadRetVal;
        sock_fd = socket(AF_INET,SOCK_STREAM,0);
        if (sock_fd < 0)
            {
            return KErrSocket;
            }
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serv_addr.sin_port = htons(12350);
        pthread_t clientThread;
        if (bind(sock_fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
            {
            INFO_PRINTF2(_L("bind return with errno = %d"), errno);
            return  KErrBind;
            }
        if(-1 == listen(sock_fd, 1))
           {
             printf("error listen failed");
             close(sock_fd);
             return  -1;
           }
        pthread_create(&clientThread, NULL, &createNwrite_socket, (void*)(serv_addr.sin_port));
           addr_len = sizeof(new_socket);
           newsock_fd = accept(sock_fd,(sockaddr*)&new_socket,&addr_len);  
           if (errno != 0 )
               {
               INFO_PRINTF2(_L("Accept return with errno = %d"), errno);
               ret = -1;
               }
          
           close(newsock_fd);
           close(sock_fd);
           pthread_join(clientThread, &threadRetValPtr );
           if (threadRetVal < 0)
               {  
               INFO_PRINTF1(_L("SockWriteOnClosedConn failed"));
               ret = threadRetVal;
               }
           return ret;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File


