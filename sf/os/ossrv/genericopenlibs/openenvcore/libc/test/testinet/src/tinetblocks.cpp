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



#include "tinet.h"


TInt CTestInet::Inet_addr_with_valid_input()
    {
    unsigned long nbo_value;
  	char valid_addr[3][50];
  	int i=0;
  	int total_param;
  	TPtrC string;
    TBuf8<256> bufstring;
    bufstring.Copy(string);
    CnvUtfConverter::ConvertFromUnicodeToUtf8(bufstring,string); 
    TInt paramLength=htons(80);
    _LIT( Kstring, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(Kstring,++iParamCnt);
    
    while ( GetStringFromConfig(ConfigSection(), pNameBuf, string ) )
        {
        bufstring.Copy(string);
        paramLength=string.Length();
        char* text=(char *)(bufstring.Ptr());
        *(text+paramLength)='\0';
         strcpy(valid_addr[i],text);
         i++;
         pNameBuf.Format(Kstring,++iParamCnt);
        }
  	total_param=i;
  	for(i = 0;i < total_param;i++)
		{
       	nbo_value=inet_addr(valid_addr[i]);
    	if(nbo_value==0xffffffff)
    	    {
    	    return KErrGeneral;
    	    }
    	INFO_PRINTF2(_L("the numerical value of address is %x"), nbo_value);
		}
     return KErrNone;
    }




TInt CTestInet::Inet_addr_with_invalid_input()
    {
    unsigned long nbo_value;
  	char invalid_addr[3][20];
  	int i=0;
  	int total_param;
  	TPtrC string;
    TBuf8<256> bufstring;
    bufstring.Copy(string);
    CnvUtfConverter::ConvertFromUnicodeToUtf8(bufstring,string); 
    TInt paramLength;
    
    _LIT( Kstring, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(Kstring,++iParamCnt);
    
    while ( GetStringFromConfig(ConfigSection(), pNameBuf, string ) )
        {
        bufstring.Copy(string);
        paramLength=string.Length();
        char* text=(char *)(bufstring.Ptr());
        *(text+paramLength)='\0';
         strcpy(invalid_addr[i],text);
         i++;
         pNameBuf.Format(Kstring,++iParamCnt);
        }
  	total_param=i;
  	for(i = 0;i < total_param;i++)
		{
       	nbo_value=inet_addr(invalid_addr[i]);
    	if(nbo_value!=0xffffffff)
    	    {
    	    return KErrGeneral;

    	    }
    	INFO_PRINTF2(_L("the numerical value of address is %x"), nbo_value);
		}
     return KErrNone;
    }


//Test case to check ntoa function 
 TInt CTestInet::Inet_ntoaTest()
    {
    struct in_addr x;
    char buf[20];
  	int i=0;
  	int total_param;
  	unsigned long array[10];
  	char valid_addr[10][20];
  	TPtrC string;
    TBuf8<256> bufstring;
    bufstring.Copy(string);
    CnvUtfConverter::ConvertFromUnicodeToUtf8(bufstring,string); 
    TInt paramLength;
   
    //Parameter supplied has to be in Decimal number only
    _LIT( Kstring, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(Kstring,++iParamCnt);
    while (GetStringFromConfig(ConfigSection(), pNameBuf, string))
        {
        bufstring.Copy(string);
        paramLength=string.Length();
        char* text=(char *)(bufstring.Ptr());
        *(text+paramLength)='\0';
         strcpy(valid_addr[i],text);
         i++;
         pNameBuf.Format(Kstring,++iParamCnt);
        }
  	total_param=i;
  	
  	for( i=0;i<total_param;i++)
  	    {
  	    array[i]=atol(valid_addr[i]);
  	    }
  	
   	for( i=0;i<total_param;i++)
   	    {
   	    x.s_addr=array[i];
        strcpy(buf,inet_ntoa(x));
   	    INFO_PRINTF2(_L("the IP address is %s" ),buf);
    	if(!buf)
    	    {
    	    return KErrGeneral;
    	    }
    	}
     return KErrNone;
    }
    
    
 TInt CTestInet::Inet_ptonTest()
    {
    char addr[10][50];
  	int param;
  	int i=0;
  	int err;
  	TPtrC string;
    TBuf8<256> bufstring;
    in6_addr NetworkByteAddr;
    bufstring.Copy(string);
    CnvUtfConverter::ConvertFromUnicodeToUtf8(bufstring,string); 
    TInt paramLength;
    _LIT( Kparam, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(Kparam,++iParamCnt);
	GetIntFromConfig(ConfigSection(), pNameBuf, param );
	
	_LIT( Kstring, "Param%d" );
    pNameBuf.Format(Kstring,++iParamCnt);
    
	while (GetStringFromConfig(ConfigSection(), pNameBuf, string))
        {
        bufstring.Copy(string);
        paramLength=string.Length();
        char* text=(char *)(bufstring.Ptr());
        *(text+paramLength)='\0';
         strcpy(addr[i],text);
         i++;
         pNameBuf.Format(Kstring,++iParamCnt);
        }
   		err=inet_pton(param,addr[0],&NetworkByteAddr);
    	if(err==0 || err==-1)
    	    {
    	    return KErrGeneral;
    	    }
      	return KErrNone;
    }
       
    
TInt CTestInet::Inet_ntopTest()
    {
    unsigned char* addr = NULL;
  	int param;
  	int i=0;
  	char result[50];
  	const char* error;
  	socklen_t size;
  	
  	TPtrC string;
    TBuf8<256> bufstring;
    in6_addr NetworkByteAddr;
    bufstring.Copy(string);
    CnvUtfConverter::ConvertFromUnicodeToUtf8(bufstring,string); 
    TInt paramLength;
    
    _LIT( Kparam, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(Kparam,++iParamCnt);
	GetIntFromConfig(ConfigSection(), pNameBuf, param );
	
	_LIT( Kstring, "Param%d" );
    pNameBuf.Format(Kstring,++iParamCnt);
    
    while ( GetStringFromConfig(ConfigSection(), pNameBuf, string))
        {
        bufstring.Copy(string);
        paramLength=string.Length();
        addr=(unsigned char *)(bufstring.Ptr());
        *(addr+paramLength)='\0';
        i++;
        pNameBuf.Format(Kstring,++iParamCnt);
        }
   strcpy((char *)NetworkByteAddr.s6_addr,(char *)addr);
    size=sizeof(result);
    error=inet_ntop(param,&NetworkByteAddr.s6_addr,result,size);
    if(error==NULL)
    	{
       	 return KErrGeneral;
    	}
     return KErrNone;
    }   
  


//  End of File
