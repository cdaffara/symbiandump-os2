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

const char KSoGet = 1;
const char KSoSet = 2;
const char KSoGetSet = 3;

int read(int newsock_fd,char *line);
int write(int sock_fd);

static void* UDPThread(TAny* ptr);
static void* TCPThread(TAny* ptr);
static void* ClientThread(TAny* ptr);
static void* ShutDownThread(TAny* ptr);
static void* UDPThreadSendMsg(TAny* ptr);

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
    int error ;
	unsigned int sender_len, receiver_len;
	sockaddr_in receiver_addr,sender_addr;
	char line[KMaxLine];
	TInt ret = KErrNone;
	pthread_t testThread;
	int threadRetVal = 0;
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
	char line[KMaxLine];
	char character = 'A';
	unsigned int cnt = 0;
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
	pthread_t testThread;
	int threadRetVal;
	void *threadRetValPtr = (void*)&threadRetVal;
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
	unsigned int len, addr_len;
    
    /**
     * paramets[0]=socket family;
     * paramets[1]=sockettype
     * paramets[2]=protocol 
     */
	GetParameters(paramets);
    
	sock_fd=socket(atoi(paramets[0]), atoi(paramets[1]), atoi(paramets[2]) );  
    if (sock_fd < 0)
    	{
    	ret = KErrSocket;
    	goto close;
        }
        
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;
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
    if (sock_fd < 0)
    	{
    	ret = KErrSocket;
    	goto close;
        }
        
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;
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
    if (sock_fd < 0)
    	{
    	ret = KErrSocket;
    	goto close;
        }
        
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;
	if ( bind(sock_fd,(sockaddr*)&addr,sizeof(addr)) < 0)
		{		
		ret = KErrBind;
		goto close;
		}
		
	len=1;
	ret = getsockname(sock_fd,(sockaddr*)&ss,&len);	    
    if (errno != EINVAL )
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
    // Invalid len, EINVAL is expected in errno
    len = 0;
	if (getpeername(sock_fd, (sockaddr*)&ss, &len) < 0)
		{
        err=errno;
        if(err!=EINVAL)
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
	pthread_t testThread;
	int threadRetVal;
	void *threadRetValPtr = (void*)&threadRetVal;
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
	int error;
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
     pthread_t testThread;
     int threadRetVal;
     void *threadRetValPtr = (void*)&threadRetVal;
     	
	sock_fd = socket ( atoi(paramets[0]), atoi(paramets[1]), atoi(paramets[2]) );
	if (sock_fd < 0)
		{
	  	ret = KErrGeneral;	  	
	  	goto close;
    	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = 0;
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
	if (sock_fd < 0)
		{
	  	ret = KErrGeneral;	  	
	  	goto close;
    	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = 0;
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

	// Wrong socklen, errno shoud be EINVAL
	addr_len = 1;
    newsock_fd = accept(sock_fd,(sockaddr*)&new_socket,&addr_len);
    if (newsock_fd == KErrNone)
    	{
    	ret = KErrBind;
        goto close;
    	}
    if (errno != EINVAL)
    	{
    	ret = KErrBind;
        goto close;
    	}
    	
    // Wrong sockaddr, errno shoud be EFAULT
    newsock_fd = accept(sock_fd,(sockaddr*)0,&addr_len);
    if (newsock_fd == KErrNone)
    	{
    	ret = KErrBind;
        goto close;
    	}
    if (errno != EFAULT)
    	{
    	ret = KErrBind;
        goto close;
    	}

    ret = KErrNone;
close:
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
		ret = KErrNone;
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
    unsigned int receiver_len;
    sockaddr_in receiver_addr,dest_addr;
    
    char paramets[10][256];
    int error;
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
		
	if ( atoi(paramets[4]) > 0) // set the send window size
		{
		int optionValue = KMaxLine - 50;
		setsockopt(sock_fd,SOL_SOCKET,SO_SNDBUF,&optionValue,sizeof(optionValue));
		}

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);	
	dest_addr.sin_port = receiver_addr.sin_port;
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
    optionName = -1;
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
	int error;
	//TInt ret = KErrNone;
	char character = 'A';
	unsigned int cnt = 0;

    sock_fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
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
	int error;
	int sock_fd;
	unsigned int sender_len, receiver_len;
	struct msghdr msg;
	struct iovec iov;
    sockaddr_in receiver_addr,sender_addr;
	char line[KMaxLine];
	pthread_t testThread;
	int threadRetVal;
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
	

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
