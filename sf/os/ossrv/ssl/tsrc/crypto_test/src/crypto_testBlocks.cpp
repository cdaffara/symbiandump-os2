/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.

* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this 
* list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
* this list of conditions and the following disclaimer in the documentation 
* and/or other materials provided with the distribution.
* Neither the name of Nokia Corporation nor the names of its contributors 
* may be used to endorse or promote products derived from this software 
* without specific prior written permission.

* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Description:  ?Description
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "crypto_test.h"
#include "tcrypto_test.h"
#include <stdio.h>
#include <string.h>
#include  <stdlib.h>
#include <errno.h>
// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ccrypto_test::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ccrypto_test::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Ccrypto_test::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ccrypto_test::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "BNTest", Ccrypto_test::BnTest ),
        ENTRY( "DESTest", Ccrypto_test::DesTest ),
        ENTRY( "DHTest", Ccrypto_test::DhTest ),
        ENTRY( "DSATest", Ccrypto_test::DsaTest ),
        ENTRY( "ENGINETest", Ccrypto_test::EngineTest ),
        ENTRY( "EVPTest", Ccrypto_test::EvpTest ),
        ENTRY( "EXPTest", Ccrypto_test::ExpTest ),
        ENTRY( "HMACTest", Ccrypto_test::HmacTest ),
        ENTRY( "MD2Test", Ccrypto_test::Md2Test ),
        ENTRY( "MD5Test", Ccrypto_test::Md5Test ),
        ENTRY( "RANDTest", Ccrypto_test::RandTest ),
        ENTRY( "RC2Test", Ccrypto_test::Rc2Test ),
        ENTRY( "RC4Test", Ccrypto_test::Rc4Test ),
        ENTRY( "RSATest", Ccrypto_test::RsaTest ),
        ENTRY( "SHATest", Ccrypto_test::ShaTest ),
        ENTRY( "SHA1Test", Ccrypto_test::Sha1Test ),
		ENTRY( "SHA256Test", Ccrypto_test::Sha256Test ),
		ENTRY( "SHA512Test", Ccrypto_test::Sha512Test ),

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }
    
#ifdef __cplusplus
extern "C"
{
#endif	
    
FILE *fp_stdout=NULL;
FILE *fp_stderr=NULL;

#ifdef __cplusplus
}
#endif
int cryptotest_init(char *mod)
{
    char str[200];
    int len=0;
    if(!mod)
    return 1;
    len=strlen(LOG_STDOUT);
    strcpy(str,LOG_STDOUT);
    strcat(str,"_");
    strcat(str,mod);
    len+=strlen(mod);
    str[len+1]='\0';
	fp_stdout = fopen(str,"a+b");
	if(!fp_stdout)
	return 1;
	fp_stderr=fp_stdout;
	return 0;
}

void cryptotest_deinit(void)
{
    fclose(fp_stdout);
    fp_stderr=NULL;
    fp_stdout=NULL;
}
char ** MakeArgs(CStifItemParser& aItem,int * argc)
{
	char *ini_cmd[256];
	char **cmd_line;
	   
	TInt len=0;
	TPtrC string;
    TBuf8<50> buf1;
	char* argument;
	int cnt=0;
	int i;
	
	//parse through the parameters of cfg to find the number and strings of cmd line
		
	while(aItem.GetNextString(string)!=-1)
	{
		buf1.Copy(string);
		argument=(char *)buf1.Ptr();
		len=buf1.Length();
		argument[len]='\0';
		
		ini_cmd[cnt]=(char *)malloc(sizeof(char)*len+1);
		if(ini_cmd[cnt]==NULL)
		{
			for(i=0;i<cnt;i++)
			{
				if(ini_cmd[i])
			  free(ini_cmd[i]);
		  }
			return NULL;
		}	
		strcpy(ini_cmd[cnt],(const char *)argument);
		
		cnt++;
	}
	
	//allocate memory for the command line ragged array
	
	cmd_line=(char **)malloc(cnt*sizeof(char *));
	if(cmd_line==NULL)
	{
		for(i=0;i<cnt;i++)
  	   {
	  	if(ini_cmd[i])
		  free(ini_cmd[i]);
	   }
    return NULL;
	}	
	
	//initialize the array
	
	for(i=0;i<cnt;i++) cmd_line[i]=ini_cmd[i];
	
	//initialize argc
	
	*argc=cnt;
	
	return cmd_line;
	
}
//-----------------------------------------------------------------------------
//function function for destroying argv
//-----------------------------------------------------------------------------
void DeleteArgs(char ** cmd_line,int argc)
{
	int i;
	for(i=0;i<argc;i++) free(cmd_line[i]);
	
	free(cmd_line);
}


// -----------------------------------------------------------------------------
// Ccrypto_test::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


TInt Ccrypto_test::BnTest( CStifItemParser&/* aItem*/ )
    {
    TInt ret=1;
    
    
    if(!cryptotest_init("bn"))
    {
     ret = bn_main(0,NULL);
     cryptotest_deinit();

    }
    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    }



TInt Ccrypto_test::DesTest( CStifItemParser&/* aItem*/ )
    {
    TInt ret=1;
    
    
    if(!cryptotest_init("des"))
    {
    ret = des_main(0,NULL);
    cryptotest_deinit();
	
    }
    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    return ret;

    }
    
    
TInt Ccrypto_test::DhTest( CStifItemParser&/* aItem*/ )
    {
    TInt ret=1;
    
      
    if(!cryptotest_init("dh"))
    {
    ret = dh_main(0,NULL);
    cryptotest_deinit();
    }
    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    }    
    
TInt Ccrypto_test::DsaTest( CStifItemParser&/* aItem*/ )
    {
    TInt ret=1;
    if(!cryptotest_init("dsa"))
    {
    ret = dsa_main(0,NULL);
    cryptotest_deinit();
    
    }
    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;
    

    }      
    
    
TInt Ccrypto_test::EngineTest( CStifItemParser&/* aItem*/ )
    {
    TInt ret=1;
    
    if(!cryptotest_init("eng"))
    {
    ret = engine_main(0,NULL);
    cryptotest_deinit();
    	
    }
    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    }      
    
TInt Ccrypto_test::EvpTest( CStifItemParser& aItem )
    {
    TInt ret=1;
    int argc=0;
    char** argv=NULL;

    argv = MakeArgs(aItem,&argc);
    if(!argv)
    {
      return KErrNoMemory;    
    }	
    if(!cryptotest_init("evp"))
    {
    ret = evp_main(argc,argv);
    cryptotest_deinit();
    
    }
    DeleteArgs(argv,argc);
    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;
    }     
    
    
    
TInt Ccrypto_test::ExpTest( CStifItemParser& /*aItem */)
    {
    TInt ret=1;
    
    if(!cryptotest_init("exp"))
    {
    ret = exp_main(0,NULL);
    cryptotest_deinit();
   
    }
   if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    }          
TInt Ccrypto_test::HmacTest( CStifItemParser& /*aItem */)
    {
    TInt ret=1;
    
    if(!cryptotest_init("hmac"))
    {
    ret = hmac_main(0,NULL);
    cryptotest_deinit();
    	
    }
    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    }     

             
TInt Ccrypto_test::Md2Test( CStifItemParser& /*aItem */)
    {
    TInt ret=1;
    
    if(!cryptotest_init("md2"))
    {
    ret = md2_main(0,NULL);
    cryptotest_deinit();
    	
    }
    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    }   
TInt Ccrypto_test::Md5Test( CStifItemParser& /*aItem */)
    {
    TInt ret=1;

    if(!cryptotest_init("md5"))
    {
    ret = md5_main(0,NULL);
    cryptotest_deinit();
    	
    }
    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    }   
TInt Ccrypto_test::RandTest( CStifItemParser& /*aItem */)
    {
    TInt ret=1;

    if(!cryptotest_init("rand"))
    {
    ret = rand_main(0,NULL);
    cryptotest_deinit();
    	
    }
    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    }      
    
    
TInt Ccrypto_test::Rc2Test( CStifItemParser& /*aItem */)
    {
    TInt ret=1;

    if(!cryptotest_init("rc2"))
    {
    ret = rc2_main(0,NULL);
    cryptotest_deinit();
    	
    }

    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    }      
    
TInt Ccrypto_test::Rc4Test( CStifItemParser& /*aItem */)
    {
    TInt ret=1;

    if(!cryptotest_init("rc4"))
    {
    ret = rc4_main(0,NULL);
    cryptotest_deinit();
    	
    }

    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    }     

TInt Ccrypto_test::RsaTest( CStifItemParser& /*aItem */)
    {
    TInt ret=1;

    if(!cryptotest_init("rsa"))
    {
    ret = rsa_main(0,NULL);
    cryptotest_deinit();
    	
    }

    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    }      
    
TInt Ccrypto_test::ShaTest( CStifItemParser& /*aItem */)
    {
    TInt ret=1;

    if(!cryptotest_init("sha"))
    {
    ret = sha_main(0,NULL);
    cryptotest_deinit();
    	
    }

    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    }     

TInt Ccrypto_test::Sha1Test( CStifItemParser& /*aItem */)
    {
    TInt ret=1;

    if(!cryptotest_init("sha1"))
    {
    ret = sha1_main(0,NULL);
    cryptotest_deinit();
    	
    }

    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    }                                         
    
TInt Ccrypto_test::Sha256Test( CStifItemParser& /*aItem */)
    {
    TInt ret=1;

    if(!cryptotest_init("sha256"))
    {
    ret = sha256_main(0,NULL);
    cryptotest_deinit();
    	
    }

    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    }       
    
TInt Ccrypto_test::Sha512Test( CStifItemParser& /*aItem */)
    {
    TInt ret=1;

    if(!cryptotest_init("sha512"))
    {
    ret = sha512_main(0,NULL);
    cryptotest_deinit();
    	
    }

    if(ret==1&&errno==ENOMEM)
    {
    	return KErrNoMemory;
    }
    return ret;

    }   

                                       
// -----------------------------------------------------------------------------
// Ccrypto_test::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Ccrypto_test::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
