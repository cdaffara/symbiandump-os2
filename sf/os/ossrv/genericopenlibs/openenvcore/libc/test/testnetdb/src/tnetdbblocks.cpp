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



#include "tnetdb.h"


// for RHostResolver
#include <es_sock.h>
#include <in_sock.h>

// -----------------------------------------------------------------------------
// CTestNetdb::GetHostByNameNegativeTest
// Test for gethostbyname by passing NULL
// -----------------------------------------------------------------------------
//
TInt CTestNetdb::GethostbynameNegativeTest()
    {
    struct hostent *hp = NULL;
        
    hp = gethostbyname(NULL);
    
    if (hp != NULL)
	    {
	    INFO_PRINTF1(_L("gethostbyname() failed"));
		return KErrGeneral;
		}
	
	INFO_PRINTF2(_L("gethostbyname() failed with errno no: %d, Test case passed"), errno);
	return KErrNone;		// negative test case
    }
    
/*
*	Testcase	:	GetHostByNameLocalHost
*	Description	:	Verifies the result on passing "localhost" to gethostbyname
*	Result		:	gethostbyname fills up hostent struct with loopback IP 127.0.0.1
**/    

TInt CTestNetdb::GetHostByNameLocalHost()
	{
	struct hostent *h = NULL;
	struct sockaddr_in localaddr;
	char hostname[256]="";
	char ipaddr[20];
	int ret = KErrNone;
	
	//The hostname is always "LocalHost"
	gethostname(hostname,256);

	h=gethostbyname(hostname);
			
	if (h != NULL)
		{
		memcpy(&localaddr.sin_addr, h->h_addr_list[0], 4);
		strcpy(ipaddr,inet_ntoa(localaddr.sin_addr));
		TPtrC8 ipaddrPtr(reinterpret_cast<TUint8*>(ipaddr));
		TBuf8<40> ipAddrBuf;
		ipAddrBuf.Copy(ipaddrPtr);
		INFO_PRINTF1(_L("The correct loopback IP is returned"));
		ret = KErrNone;
		}
	else
		{
		INFO_PRINTF1(_L("gethostbyname returns NULL"));
		ret = KErrGeneral;
		}
		
	return ret;
	}


	
TInt CTestNetdb::GethostbyaddrTest()
	{
	// Test for gethostbyaddr, c
	unsigned long test_addr;
	struct in_addr *addrp=0;
	char test_address[50] ;
    TPtrC string;
    TInt paramLength;
    TBuf8<256> bufstring;
    char buf[20];
    TInt ret = KErrNone;
    
    _LIT( Kstring, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(Kstring,++iParamCnt);
    while ( GetStringFromConfig(ConfigSection(), pNameBuf, string ) )
        {
        bufstring.Copy(string);
        paramLength=string.Length();
        char* text=(char *)(bufstring.Ptr());
        *(text+paramLength)='\0';
        strcpy(test_address,text);
        pNameBuf.Format(Kstring,++iParamCnt);
        }  
	struct hostent *hp1=0;
    test_addr=inet_addr(test_address);
    hp1 = gethostbyaddr((const char *)(&test_addr),sizeof(test_addr),AF_INET);
    if (hp1 != NULL)
		{
		addrp = (struct in_addr*)(hp1->h_addr_list[0]);
		strcpy(buf,inet_ntoa(*addrp));
		if(strcmp(buf,"127.0.0.1") == 0)
			{
			INFO_PRINTF1(_L("gethostbyaddr() returns 127.0.0.1 for localhost"));
			INFO_PRINTF1(_L("gethostbyaddr() Pass\n"));
			}
		else
			{
			ERR_PRINTF1(_L("gethostbyaddr() Fail\n"));
			ret = KErrGeneral;;	
			}
		}
    else
	    {		    
		ERR_PRINTF1(_L("gethostbyaddr() Fail\n"));
		ret = KErrGeneral;
	    }
		return ret;
    }




TInt CTestNetdb::GetprotobynameTest()
    {
    protoent *p =0;
	char proto[50] = "" ;
    TPtrC string;
    TBuf8<256> bufstring;
    TInt paramLength=0;
    _LIT( Kstring, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(Kstring,++iParamCnt);
    while ( GetStringFromConfig(ConfigSection(), pNameBuf, string ) )
        {
        bufstring.Copy(string);
        paramLength=string.Length();
        char* text=(char *)(bufstring.Ptr());
        *(text+paramLength)='\0';
        strcpy(proto,text);
        pNameBuf.Format(Kstring,++iParamCnt);
        }  
   	p=getprotobyname(proto);
   	if(p==NULL)
   		{
   		return KErrGeneral;
   		}
     return KErrNone;
    }
    
 
TInt CTestNetdb::GetprotobynumberTest()
    {
    protoent *p =0;
	int proto=0;
	_LIT( Kproto, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(Kproto,++iParamCnt);
    GetIntFromConfig(ConfigSection(), pNameBuf, proto );
    
    p=getprotobynumber(proto);
   	if(p==NULL)
   		{
   		return KErrGeneral;
   		}
    return KErrNone;
    }   
    
    
 TInt CTestNetdb::GetservbynameTest()
    {
    int i=0;
	servent *p =0;
	char param[2][50]  ;
    TPtrC string;
    TBuf8<256> bufstring;
    TInt paramLength=0;
    _LIT( Kstring, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(Kstring,++iParamCnt);
    while ( GetStringFromConfig(ConfigSection(), pNameBuf, string ) )
        {
        bufstring.Copy(string);
        paramLength=string.Length();
        char* text=(char *)(bufstring.Ptr());
        *(text+paramLength)='\0';
        strcpy(param[i],text);
        i++;
        pNameBuf.Format(Kstring,++iParamCnt);
        }  
   	if(i==1)
   	p=getservbyname(param[0],0);
   	else
   	p=getservbyname(param[0],param[1]);
   	if(p==NULL)
   		{
   		return KErrGeneral;
   		}
    return KErrNone;
    }   
    
TInt CTestNetdb::GetservbyportTest()
    {
    servent *p =0;
	char *param=0;
	int port;
    TPtrC string;
    TBuf8<256> bufstring;
    TInt paramLength=0;
    _LIT( Kport, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(Kport,++iParamCnt);
    GetIntFromConfig(ConfigSection(), pNameBuf, port );
    _LIT( Kstring, "Param%d" );
    pNameBuf.Format(Kstring,++iParamCnt);

    while ( GetStringFromConfig(ConfigSection(), pNameBuf, string ) )
        {
        bufstring.Copy(string);
        paramLength=string.Length();
        char* text=(char *)(bufstring.Ptr());
        *(text+paramLength)='\0';
        param=(char *)malloc(sizeof(paramLength+1));
        strcpy(param,text);
        pNameBuf.Format(Kstring,++iParamCnt);
        }
	
   	p=getservbyport(port,param);
   	if(p==NULL)
   		{
   		free(param);
   		return KErrGeneral;
   		}
   	free(param);
    return KErrNone;
    }   
    
    
TInt CTestNetdb::GetaddrinfoTest()
    {
    char param[10][250]  ;
	char *hostname=0;
	char *servname=0;
	struct addrinfo hint;
	TInt paramLength;
	struct addrinfo *res=NULL;
	int paramint[5];
	int i=0;
    TPtrC string;
    TBuf8<256> bufstring;
    _LIT( Kstring, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(Kstring,++iParamCnt);
    TBool ret = GetStringFromConfig(ConfigSection(), pNameBuf, string );

    while(ret)
        {
        bufstring.Copy(string);
        paramLength=string.Length();
        char* text=(char *)(bufstring.Ptr());
        *(text+paramLength)='\0';
        strcpy(param[i],text);
        i++;
        if(i==2)
			{
			break;
			}
        pNameBuf.Format(Kstring,++iParamCnt);
        ret = GetStringFromConfig(ConfigSection(), pNameBuf, string );
        }  

   	if(strcmp(param[0],"0"))
   		{
   		hostname=(char *)malloc(sizeof(param[0])+1);
   		strcpy(hostname,param[0]);
   		}
   	if(strcmp(param[1],"0"))
   		{
   		servname=(char *)malloc(sizeof(param[1])+1);
   		strcpy(servname,param[1]);
   		}
   	
   	/*Filling the Hint structure*/
    i=0;
    _LIT( Kparamint, "Param%d" );

    pNameBuf.Format(Kparamint,++iParamCnt);
    while (GetIntFromConfig(ConfigSection(), pNameBuf, paramint[i++]))
		{
    	pNameBuf.Format(Kstring,++iParamCnt);	
		}
	hint.ai_flags=paramint[0];
    hint.ai_family=paramint[1];
    hint.ai_socktype=paramint[2];
    hint.ai_protocol=paramint[3];
    hint.ai_addrlen=0;
    hint.ai_addr=0;
    hint.ai_canonname=0;
    hint.ai_next=0;
    int err=getaddrinfo(hostname,servname,&hint,&res);
     if(err!=0)
    	{
    	if(hostname)
    		free(hostname);
    	if(servname)
    		free(servname);
    	if(res)
    		freeaddrinfo(res);
    	return KErrGeneral;
    	}
   	if(hostname)
   		free(hostname);
   	if(servname)
   		free(servname);
   	if(res)
   		freeaddrinfo(res);
   	return KErrNone;
    }   
        
           
    
      
TInt CTestNetdb::GetnameinfoTest()
    {
    struct sockaddr ss;
    TPtrC string;
    TBuf8<256> bufstring;
    char hostname[MAXBUF];
    char servname[MAXBUF];
    int paramint[3];
    int flag;
    int paramLength;
    int i=0;
    
    _LIT( Kstring, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(Kstring,++iParamCnt);
    while (GetIntFromConfig(ConfigSection(), pNameBuf, paramint[i]))
    	{
    	if(i==2)
			{
    		break;
			}
    	i++;
    	pNameBuf.Format(Kstring,++iParamCnt);
    	}
    i=0;
    ss.sa_family=paramint[i++];
    ss.sa_port=paramint[i++];;
    flag=paramint[i];
    
    pNameBuf.Format(Kstring,++iParamCnt);
    
    while (GetStringFromConfig(ConfigSection(), pNameBuf, string))
        {
        bufstring.Copy(string);
        paramLength=string.Length();
        char* text=(char *)(bufstring.Ptr());
        *(text+paramLength)='\0';
        strcpy(ss.sa_data,text);
        pNameBuf.Format(Kstring,++iParamCnt);
        }  
    int err=getnameinfo(&ss,sizeof(struct sockaddr_in),hostname,sizeof(hostname),servname,sizeof(servname),flag);
    if(err==0)
    	{
    	return KErrNone;
    	}
    return KErrGeneral;
    }     
    
 TInt CTestNetdb::GetnameinfoTest1()
    {
	char hostname1[80];
	struct sockaddr_in sdg;
    TPtrC address,name;
    TInt len;
	GetStringFromConfig(ConfigSection(),_L("address"), address);	
    GetStringFromConfig(ConfigSection(),_L("name"), name); 
	TBuf8<200> buf1,buf2;
	buf1.Copy(address);
	char* inputstr = (char*) buf1.Ptr();
	len=buf1.Length();
	inputstr[len]='\0';
    buf2.Copy(name);
	char* inputstr1 = (char*) buf2.Ptr();
	len=buf2.Length();
	inputstr1[len]='\0';
    sdg.sin_family=AF_INET;
    sdg.sin_port=htons(0);
    inet_aton(inputstr,&sdg.sin_addr);  
    int err= getnameinfo( (struct sockaddr*)&sdg, sizeof(struct sockaddr_in), hostname1, sizeof(hostname1), NULL,0,0);
    int i=strcmp(inputstr1,hostname1);
     if(i==0)
     {
     return KErrNone;
     }
     
     return KErrGeneral;
    
    }     
TInt CTestNetdb::SetserventTest()
    {
    int stayopen;
     	
    _LIT( Kstring, "Param%d" );
	TBuf<8> pNameBuf;
	pNameBuf.Format(Kstring,++iParamCnt);
	GetIntFromConfig(ConfigSection(), pNameBuf, stayopen);
     
    setservent(stayopen);
    return KErrNone;
    }     
        
TInt CTestNetdb::Gai_strerrorTest()
    {
    int errnum;
    const char *errordesc;
    const char* temp="Unknown error";
    _LIT( Kstring, "Param%d" );
	TBuf<8> pNameBuf;
	pNameBuf.Format(Kstring,++iParamCnt);
	GetIntFromConfig(ConfigSection(), pNameBuf, errnum);
    
    errordesc=gai_strerror(errnum);
    if(strcmp(errordesc,temp))
		{
        return KErrNone;
        }
    return KErrGeneral;
    }     
            

        
TInt CTestNetdb::FreeaddrinfoTest()
    {
    struct addrinfo *test_struct;
    test_struct=(struct addrinfo*)malloc (sizeof(struct addrinfo));
    test_struct->ai_next=NULL;
    test_struct->ai_canonname=NULL;
    freeaddrinfo(test_struct);
    return KErrNone;
    } 
    
    
     
TInt CTestNetdb::GetserventTest()
    {
    struct servent *p;
    int i;
    for(i=0;i<25;i++)
		{
     	p=getservent();
		}
    if(p)
		{
        return KErrNone;
		}
    else
		{
        return KErrGeneral;
		}
    }    


     
TInt CTestNetdb::EndserventTest()
    {
    struct servent *p;
    p=getservent();
    if(!p)
		{
        return KErrGeneral;
		}
    else
		{
        endservent();
		}
    return KErrNone;
    }

/*
 * Test the setdefaultif API by using getaddrinfo() and connect()
 */
TInt CTestNetdb::SetDefaultIfTest1()
	{
	TPtrC string;
	TBuf<8> pKeyBuf;
	TBuf8<50> domainNamBuf;
	TBuf8<IF_NAMESIZE> iapNameBuf;
	char *iapName;
	char *domainName;
	int portNum;
	struct ifreq ifReq;
	int err;
	
	//Read the default interface name
	_LIT( Kstring, "Param%d" );
	pKeyBuf.Format( Kstring,++iParamCnt );
	if( GetStringFromConfig( ConfigSection(), pKeyBuf, string ) )
		{
		iapNameBuf.Copy( string );
		iapNameBuf.ZeroTerminate();
        iapName = ( char* )( iapNameBuf.Ptr() );
	    }
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig() Failed (Reading IAP name)"));
		return KErrGeneral;	
		}
	
	//Read the remote server name
	_LIT( Kstring1, "Param%d" );
	pKeyBuf.Format( Kstring1,++iParamCnt );
	if( GetStringFromConfig( ConfigSection(), pKeyBuf, string ) )
		{
		domainNamBuf.Copy( string );
		domainNamBuf.ZeroTerminate();
        domainName = ( char* )( domainNamBuf.Ptr() );
	    }
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig() Failed (Reading domain name)"));
		return KErrGeneral;	
		}
	
	//Read the port number
	_LIT( Kstring2, "Param%d" );
	pKeyBuf.Format( Kstring2,++iParamCnt );
	if( !GetIntFromConfig( ConfigSection(), pKeyBuf, portNum ) )
		{
		ERR_PRINTF1(_L("GetIntFromConfig() Failed (Reading port number)"));
		return KErrGeneral;	
		}
	
	//Set the default interface name
	strcpy( ifReq.ifr_name, iapName);
	err = setdefaultif( &ifReq );
	if( err != 0 )
		{
		ERR_PRINTF2(_L("setdefaultif() Failed (Setting default IAP), errno = %d"), errno);
		return KErrGeneral;	
		}
	
	//Resolve the ip of the given domain name from the config file
	struct addrinfo *addrInfo;
	err = getaddrinfo( domainName, NULL, NULL, &addrInfo );
	if( err != 0 )
		{
		ERR_PRINTF2(_L("getaddrinfo() Failed with errno = %d"), errno);
		return KErrGeneral;	
		}
	
	struct addrinfo *tmpAddrInfo = addrInfo;
	char *serverIp = NULL;
	while( tmpAddrInfo != NULL )
		{
		if( tmpAddrInfo->ai_family == AF_INET )
			{
			sockaddr_in *inAddr = ( sockaddr_in* )( tmpAddrInfo->ai_addr );
			serverIp = inet_ntoa( inAddr->sin_addr );
			break;
			}
		
		tmpAddrInfo = tmpAddrInfo->ai_next;
		}
	freeaddrinfo( addrInfo );
	
	if( serverIp == NULL ) //Error if no ipv4 address is found
		{
		ERR_PRINTF1(_L("getaddrinfo() Failed - No valid ip is returned"));
		return KErrGeneral;	
		}
	
	//Create a socket and connect to the server
	int sockFd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( sockFd == -1 )
		{
		ERR_PRINTF2(_L("socket() Failed - errno = %d"), errno);
		return KErrGeneral;
		}
		
	
	INFO_PRINTF1(_L("Connecting to the remote server.."));
	struct sockaddr_in connAddr;
	connAddr.sin_family = AF_INET;
	connAddr.sin_addr.s_addr = inet_addr( serverIp );
	connAddr.sin_port = htons( portNum );
		
	err = connect( sockFd, (struct sockaddr*)&connAddr, sizeof( connAddr ) );
	if( err == -1 )
		{
		ERR_PRINTF2(_L("connect() Failed - errno = %d"), errno);
		close( sockFd );
		return KErrGeneral;
		}
	
	close( sockFd );
	
	//Remove the default interface
	err = setdefaultif( NULL );
	if( err != 0 )
		{
		ERR_PRINTF2(_L("setdefaultif() Failed (Removing default IAP), errno = %d"), errno);
		return KErrGeneral;	
		}
	
	INFO_PRINTF1(_L("SetDefaultIfTest1() Passed"));
	return KErrNone;
	}

/*
 * Test the setdefaultif API using invalid names
 */
TInt CTestNetdb::SetDefaultIfTest2()
	{
	TPtrC string;
	TBuf<8> pKeyBuf;
	TBuf8<IF_NAMESIZE> iapNameBuf;
	char *iapName;
	struct ifreq ifReq;
	int err;

	//Read the default interface name
	_LIT( Kstring, "Param%d" );
	pKeyBuf.Format( Kstring,++iParamCnt );
	if( GetStringFromConfig( ConfigSection(), pKeyBuf, string ) )
		{
		iapNameBuf.Copy( string );
		iapNameBuf.ZeroTerminate();
        iapName = ( char* )( iapNameBuf.Ptr() );
	    }
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig() Failed (Reading IAP name)"));
		return KErrGeneral;	
		}
		
	//Set the default interface name with out any valid name
	err = setdefaultif( &ifReq );
	if( err == 0 ) //negative test case
		{
		ERR_PRINTF1(_L("setdefaultif() succeeded (Setting default IAP with invalid name)"));
		return KErrGeneral;	
		}
	
	//Now try to remove the default interface
	err = setdefaultif( NULL );
	if( err != 0 ) 
		{
		ERR_PRINTF2(_L("setdefaultif() Failed (Removing default IAP), errno = %d"), errno);
		return KErrGeneral;	
		}
	
	//Set the default interface with a valid name
	strcpy( ifReq.ifr_name, iapName);
	err = setdefaultif( &ifReq );
	if( err != 0 )
		{
		ERR_PRINTF2(_L("setdefaultif() Failed (Setting default IAP), errno = %d"), errno);
		return KErrGeneral;	
		}
		
	//Set the default interface AGAIN with a valid name
	err = setdefaultif( &ifReq );
	if( err != 0 ) 
		{
		ERR_PRINTF2(_L("setdefaultif() Failed (Setting default IAP again), errno = %d"), errno);
		return KErrGeneral;	
		}

	//Remove the default interface
	err = setdefaultif( NULL );
	if( err != 0 ) 
		{
		ERR_PRINTF2(_L("setdefaultif() Failed (Removing default IAP), errno = %d"), errno);
		return KErrGeneral;	
		}
	
	//Remove the default interface AGAIN
	err = setdefaultif( NULL );
	if( err != 0 ) 
		{
		ERR_PRINTF2(_L("setdefaultif() Failed (Removing default IAP again), errno = %d"), errno);
		return KErrGeneral;	
		}
	
	INFO_PRINTF1(_L("SetDefaultIfTest2() Passed"));
	return KErrNone;
	}

/*
 * Test the getaddrinfo API for the case where the 
 * dns server returns multiple ip addresses
 */
TInt CTestNetdb::GetAddrInfoMultipleIPTest()
	{
	TPtrC string;
	TBuf<8> pKeyBuf;
	TBuf8<50> domainNamBuf;
	TBuf8<IF_NAMESIZE> iapNameBuf;
	char *iapName;
	char *domainName;
	struct ifreq ifReq;
	int err;
	
	//Read the default interface name
	_LIT( Kstring, "Param%d" );
	pKeyBuf.Format( Kstring,++iParamCnt );
	if( GetStringFromConfig( ConfigSection(), pKeyBuf, string ) )
		{
		iapNameBuf.Copy( string );
		iapNameBuf.ZeroTerminate();
        iapName = ( char* )( iapNameBuf.Ptr() );
	    }
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig() Failed (Reading IAP name)"));
		return KErrGeneral;	
		}
	
	//Read the remote server name
	_LIT( Kstring1, "Param%d" );
	pKeyBuf.Format( Kstring1,++iParamCnt );
	if( GetStringFromConfig( ConfigSection(), pKeyBuf, string ) )
		{
		domainNamBuf.Copy( string );
		domainNamBuf.ZeroTerminate();
        domainName = ( char* )( domainNamBuf.Ptr() );
	    }
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig() Failed (Reading domain name)"));
		return KErrGeneral;	
		}
	
	//Set the default interface
	strcpy( ifReq.ifr_name, iapName);
	err = setdefaultif( &ifReq );
	if( err != 0 ) 
		{
		ERR_PRINTF1(_L("setdefaultif() failed"));
		return KErrGeneral;	
		}
	
	//Get the addrinfo and verify if there is more than 1 ip returned
	struct addrinfo *res = NULL;
	struct addrinfo hint;
	memset(&hint, 0, sizeof(hint));

	hint.ai_family = AF_INET;
	hint.ai_flags = AI_CANONNAME;  
	err = getaddrinfo(domainName, NULL, &hint, &res);
	if( err != 0 ) 
		{
		ERR_PRINTF2(_L("getaddrinfo() Failed:  err = %d"), err);
		return KErrGeneral;	
		}
	
	struct addrinfo *curr = res;
	in_addr_t ip = 0;
	TBool found = EFalse;
    while(curr) 
    	{
        if(curr->ai_family == AF_INET)
        	{
	        sockaddr_in* sAddrIn = (sockaddr_in*) (curr->ai_addr);
	        if(ip == 0) //First ip, store it
	        	ip = sAddrIn->sin_addr.s_addr;
	        else if(sAddrIn->sin_addr.s_addr != ip)
	        	{//A different ip address found
	        	found = ETrue;
	        	break;
	        	}
        	}
        curr = curr->ai_next;
    	}
	
    freeaddrinfo(res);
    
	if(!found) 
		{
		ERR_PRINTF1(_L("getaddrinfo() did not return more than one ip"));
		return KErrGeneral;	
		}
	
	//Remove the default interface
	err = setdefaultif( NULL );
	if( err != 0 ) 
		{
		ERR_PRINTF2(_L("setdefaultif() Failed (Removing default IAP), errno = %d"), errno);
		return KErrGeneral;	
		}
	
	INFO_PRINTF1(_L("GetAddrInfoMultipleIPTest() Passed"));
	return KErrNone;
	}
TInt CTestNetdb::GetnameinfoTest_localhost()
    {
    struct sockaddr_in a;
	int error=0;
	char hostname[NI_MAXHOST]; 
	memset((void*)&a, 0 , sizeof(a));
	a.sin_family = PF_INET;
	a.sin_port=htons(0);
	if(inet_aton("127.0.0.1",&(a.sin_addr)) == 0)
		{
		INFO_PRINTF1(_L("inet_aton failed"));
		return KErrGeneral;
		} 
	error = getnameinfo((struct sockaddr*)&a, sizeof(struct sockaddr_in),hostname, sizeof(hostname), NULL,0,NI_NUMERICHOST);
	if(error != 0 || strcmp(hostname,"127.0.0.1"))
		{
		INFO_PRINTF1(_L("getnameinfo failed"));
		return KErrGeneral;
		}

	INFO_PRINTF1(_L("getnameinfo passed"));
	return KErrNone;
	}


/************************************************************************************
Negative Tests for getaddrinfo()
To verify whether getaddrinfo() returns valid errors with different inputs: 
EAI_NONAME, EAI_FAMILY, EAI_BADHINTS, EAI_BADFLAGS, EAI_FAIL
*************************************************************************************/
TInt CTestNetdb::GetAddrInfoNegativeTests()
    {
    char param[10][250]  ;
	char *hostname = 0;
	char *servname = 0;
	struct addrinfo hints;
	TInt hostLength, servLength;
	struct addrinfo *result = NULL;
	int hintstructmembers[5];
    TPtrC string;
    TBuf8<256> bufstring;
    TInt ExpectedError = 0;
    int i = 0;//host name
    _LIT( Khoststring, "host" );
    TBool ret = GetStringFromConfig(ConfigSection(), Khoststring, string );
    bufstring.Copy(string);
    hostLength=string.Length();
    char* hosttext=(char *)(bufstring.Ptr());
    *(hosttext+hostLength)='\0';
    strcpy(param[i],hosttext);
	
	i = 1;//server name
	_LIT( Kservstring, "serv" );
    ret = GetStringFromConfig(ConfigSection(), Kservstring, string );
    bufstring.Copy(string);
    servLength=string.Length();
    char* servtext=(char *)(bufstring.Ptr());
    *(servtext+servLength)='\0';
    strcpy(param[i],servtext);
    
   	if(strcmp(param[0], "0"))
   		{
   		hostname=(char *)malloc(strlen(param[0])+1);
   		strcpy(hostname,param[0]);
   		}
   	if(strcmp(param[1], "0"))
   		{
   		servname=(char *)malloc(strlen(param[1])+1);
   		strcpy(servname,param[1]);
   		}
   	
   	/*Filling the Hint structure*/
   	memset(&hints, 0, sizeof(hints));
    i = 0;
    TBuf<8> pNameBuf;
    _LIT( Khint, "hint%d" );
    pNameBuf.Format(Khint,++iParamCnt);
    while (GetIntFromConfig(ConfigSection(), pNameBuf, hintstructmembers[i++]))
		{
    	pNameBuf.Format(Khint,++iParamCnt);	
		}
	hints.ai_flags = hintstructmembers[0];
    hints.ai_family = hintstructmembers[1];
    hints.ai_socktype = hintstructmembers[2];
    hints.ai_protocol = hintstructmembers[3];
    hints.ai_addrlen = hintstructmembers[4];
    /*Retrieving expected error value for the test case*/
    _LIT( KExpectedResult, "expectedresult" );
    ret = GetIntFromConfig(ConfigSection(), KExpectedResult, ExpectedError);
    /* resolve the domain name into a list of addresses */
    TInt error = getaddrinfo(hostname, servname, &hints, &result);
 	const char* errorstring = gai_strerror(error);
 	TPtrC8 stringname((TText8*)errorstring, strlen(errorstring));
 	TBuf16<100> name;
 	name.Copy(stringname);
    if (ExpectedError == error)
	    {   
        INFO_PRINTF3(_L("getaddrinfo() returns: %d %S"), error, &name);
        INFO_PRINTF1(_L("getaddrinfo negative tests passed"));
	    }
	else
		{
		INFO_PRINTF1(_L("getaddrinfo negative tests failed"));
		} 
	if(hostname)
		free(hostname);
	if(servname)
		free(servname);
	if(result)
		freeaddrinfo(result); 
    return KErrNone;
	}
