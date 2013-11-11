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



#include "tlink.h"
#include <errno.h>

// LOCAL FUNCTION PROTOTYPES
//static int ReadPathName(TPtrC& String);


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ReadPathName :Reads path name given the StifItemParser buffer
// Returns: KErrNone: On Success
//          KErrGeneral: On Failure                    
// -----------------------------------------------------------------------------
//
int CTestLink::ReadPathName(TPtrC& String)
	{	
	_LIT( KString, "Param%d" );
	TBuf<8> pNameBuf;
    pNameBuf.Format(KString,++iParamCnt);
	TBool res = GetStringFromConfig(ConfigSection(), pNameBuf, String );
	if(!res)  
		{
     	_LIT(Kerr , "Failed to read parent file name") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
		}
	return KErrNone ;
	}
/**
 * ReadIntParam	:	Reads integer variable from ini file
 * Returns		:	KErrNone on success 
 * 					KErrGeneral	on failure
 */
int CTestLink::ReadIntParam(TInt &intParam)
	{
	_LIT( KInt, "Param%d" );
	TBuf<8> pIntBuf;
    pIntBuf.Format(KInt,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pIntBuf, intParam );
	if(!res)  
		{
     	_LIT(Kerr , "Failed to read ini file name") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
		}
	return KErrNone ;	
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//-----------------------------------------------------------------------------
//  Test case : Link, create's a link to a specified file path             
//  return : KErrNone on success 
//   		: KErrGeneral on error
//                
//----------------------------------------------------------------------------- 

TInt CTestLink::Link()
	{

	TPtrC String ;
       
    ReadPathName(String);
    TBuf8<100> string1;
    string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0';
	
	ReadPathName(String);
    TBuf8<100> string2;
    string2.Copy(String);
	char* file2 = (char*) string2.Ptr();
	file2[string2.Length()]='\0';
	FILE* fp=fopen("c:\\LOGS\\Test.txt","w");
	if(!fp)
	{
		INFO_PRINTF2(_L("Unable to create file %d"),errno);
		return KErrGeneral ;		
	}
	fclose(fp);
    if(link(file1, file2))
		{  
		_LIT(Kerr2, "failed to create link ");
		ERR_PRINTF1(Kerr2);  
		INFO_PRINTF2(_L("Unable to create link file %d"),errno);
		return KErrGeneral ;
		}     
	INFO_PRINTF1(_L(" Link creation passed\n"));
    return KErrNone;
	}

//---------------------------------------------------------------------------------
//  Test case : Link1, Negative case to check existing link file
//  return : KErrGeneral 
 
//---------------------------------------------------------------------------------/

TInt CTestLink::Link1()
	{

	TPtrC String ;
       
   	ReadPathName(String);         
    TBuf8<100> string1;
    string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0';
	TInt retVal=KErrGeneral;
	ReadPathName(String);             
    TBuf8<100> string2;
    string2.Copy(String);
	char* file2 = (char*) string2.Ptr();
	file2[string2.Length()]='\0';
				      
    if(link(file1, file2))
		{
		int n = errno;
		if(EEXIST == errno)
    		{
    		
    		_LIT(Kerr, "Test Case Passed : returned - Link Already Exists");
    		ERR_PRINTF1(Kerr);
    		retVal = KErrNone;
    		}
    	else
    		{
    		_LIT(Kerr, "Test Case Failed : returned Invalid Error Value");
    		ERR_PRINTF1(Kerr);
			INFO_PRINTF2(_L("The errno set is %d"),errno);
    		retVal= KErrGeneral;
    		}
		}
    return retVal;
	}

//------------------------------------------------------------------------------
//  Test case : Link2, Negative case to check non-existing path
//  return : KErrGeneral 
//                
//------------------------------------------------------------------------------
TInt CTestLink::Link2()
	{

	TPtrC String ;
       
    ReadPathName(String);         
    TBuf8<100> string1;
    string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0';
	TInt retVal=KErrGeneral;
	ReadPathName(String);            
    TBuf8<100> string2;
    string2.Copy(String);
	char* file2 = (char*) string2.Ptr();
	file2[string2.Length()]='\0';
				      
    if(link(file1, file2))
		{
    	if(ENOENT == errno)
    		{
    		_LIT(Kerr, "Test Case Passed : returned - ENOENT");
    		ERR_PRINTF1(Kerr);
    		retVal = KErrNone;
    		}
    	else
    		{
    		_LIT(Kerr, "Test Case Failed : returned Invalid Error Value");
    		ERR_PRINTF1(Kerr);
    		retVal = KErrGeneral;
    		}
		}
    return retVal;
	}

//----------------------------------------------------------------------------------
//  Test case : Link3, Negative case to check relative path of parent file
//  return : KErrGeneral 
                
//----------------------------------------------------------------------------------/

TInt CTestLink::Link3()
	{
	TPtrC String ;
    ReadPathName(String);         
    TBuf8<100> string1;
    string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0';
	TInt retVal=KErrGeneral;
	ReadPathName(String);            
    TBuf8<100> string2;
    string2.Copy(String);
	char* file2 = (char*) string2.Ptr();
	file2[string2.Length()]='\0';
				      
    if(link(file1, file2))
		{
		if(ENOENT == errno)
    		{
    		_LIT(Kerr, "Test Case Passed : returned - ENOENT");
    		ERR_PRINTF1(Kerr);
    		retVal = KErrNone;
    		}
    	else
    		{
    		_LIT(Kerr, "Test Case Failed : returned Invalid Error Value");
    		ERR_PRINTF1(Kerr);
    		retVal = KErrGeneral;
    		}
		}
    return retVal;
	}

//----------------------------------------------------------------------------------
//  Test case : Link4, Negative case to check empty parent path
//  return : KErrGeneral 
           
//----------------------------------------------------------------------------------

TInt CTestLink::Link4()
	{

	TPtrC String ;
           	
	ReadPathName(String);            
    TBuf8<100> string2;
    string2.Copy(String);
	char* file2 = (char*) string2.Ptr();
	file2[string2.Length()]='\0';
	TInt retVal=KErrGeneral;			      
    if(link("", file2))
		{
		if(ENOENT == errno)
    		{
    		
    		_LIT(Kerr, "Test Case Passed : returned - ENOENT");
    		ERR_PRINTF1(Kerr);
    		retVal = KErrNone;
    		}
    	else
    		{
    		_LIT(Kerr, "Test Case Failed : returned Invalid Error Value");
    		ERR_PRINTF1(Kerr);
    		retVal = KErrGeneral;
    		}
		}
    return retVal;
	}

//----------------------------------------------------------------------------------
//  Test case : Link5, Negative case to check empty link path
//  return : KErrGeneral 
                
//----------------------------------------------------------------------------------

TInt CTestLink::Link5()
	{

	TPtrC String ;
           	
	ReadPathName(String);         
    TBuf8<100> string1;
    string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0';
	TInt retVal=KErrGeneral;			      
    if(link(file1, ""))
		{
		if(ENOENT == errno)
    		{
    		_LIT(Kerr, "Test Case Passed : returned - ENOENT");
    		ERR_PRINTF1(Kerr);
    		retVal = KErrNone;
    		}
    	else
    		{
    		_LIT(Kerr, "Test Case Failed : returned Invalid Error Value");
    		ERR_PRINTF1(Kerr);
    		retVal = KErrGeneral;
    		}
		}
    return retVal;
	}

//------------------------------------------------------------------------------------

//  Test case : Link6, Negative case to check relative path of link file
//  return : KErrGeneral 
//------------------------------------------------------------------------------------                
TInt CTestLink::Link6()
	{
	TPtrC String ;
    ReadPathName(String);         
    TBuf8<100> string1;
    string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0';
	
	ReadPathName(String);            
    TBuf8<100> string2;
    string2.Copy(String);
	char* file2 = (char*) string2.Ptr();
	file2[string2.Length()]='\0';
	TInt retVal = KErrGeneral;			      
	// This test case should fail with error EACCES on hardware as 
	// on hardware path z: is taken by default
    if(link(file1, file2))
		{
		if( errno == EEXIST)
    		{
    		_LIT(Kerr, "Test Case failed with return value - EEXIST");
    		ERR_PRINTF1(Kerr);
    		retVal = KErrNone;
    		}
    	else if( errno == ENOENT)
    		{
    		_LIT(Kerr, "Test Case failed with return value - ENOENT");
    		ERR_PRINTF1(Kerr);
    		retVal = KErrNone;
    		}
    	else if(errno == EACCES)
    		{
    		_LIT(Kerr, "Test Case failed with return value - EACCES");
    		ERR_PRINTF1(Kerr);
    		retVal = KErrNone;
    		}
    	else
    		{
    		_LIT(Kerr, "Test Case Failed : returned Invalid Error Value");
    		ERR_PRINTF1(Kerr);
    		retVal = KErrGeneral;
    		}
		}
	else
		{
		INFO_PRINTF1(_L("link successful!!"));
		retVal = KErrNone;
		}
    return retVal ;
	}

//------------------------------------------------------------------------------------

//  Test case : Link7, Negative case to check opening/reading/writing of a private 
//              link file. 
//  return : KErrGeneral 
//------------------------------------------------------------------------------------                
TInt CTestLink::Link7()
	{
	const char* linkname="\\mylink";
	TInt retVal = KErrGeneral;
	
	unlink(linkname);
	system("test_touch");

	// open link file
	int fd = open(linkname, O_RDWR);
	if (fd < 0)
		{
		_LIT(Kerr, "Opening link-file Failed ");
		INFO_PRINTF1(Kerr);
		retVal = KErrNone;
		}
	// Going inside else effectively means we have got an invalid "fd".
	else
		{
		do
			{
			char buff[10];
			// read from link file.
			int bytes = read(fd, buff, 10);
			if (bytes < 0)
				{
				_LIT(Kerr, "Reading link-file Failed ");
				INFO_PRINTF1(Kerr);
				
				// write to link file
				bytes = write(fd, "1234567890", 10);
				if (bytes < 0)
					{
					_LIT(Kerr, "Writing link-file Failed ");
					INFO_PRINTF1(Kerr);
					retVal = KErrNone;			
					}
				else
					{
					_LIT(Kerr, "Write to link file Successful ");
					INFO_PRINTF1(Kerr);
					}				
				}
			else
				{
				_LIT(Kerr, "Read from link file Successful ");
				INFO_PRINTF1(Kerr);
				}
			}
		while (0);
		}
	
	close(fd);
	return retVal ;
	}

//-----------------------------------------------------------------------------------

//  Test case : Reads/Writes the parent file through(by opening) Link/parent file
//  return : KErrNone on success                 
//-----------------------------------------------------------------------------------

TInt CTestLink::ReadWriteLink ()
	{
	TPtrC String ;
       
    ReadPathName(String);         
    TBuf8<100> string1;
    string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0';
	
	ReadPathName(String);             
    TBuf8<100> string2;
    string2.Copy(String);
	char* file2 = (char*) string2.Ptr();
	file2[string2.Length()]='\0';
		
	char* wrbuff = "This is a write through link file --- ";
	char* wrbuff1 = "This is a write through parent file";
	char rdbuff[25];
	int nbytes1 = strlen(wrbuff);
	int nbytes2 = strlen(wrbuff1);	
	
	// Open and write/read through link file
	int fd1, fd2;
    if((fd1 = open(file2, O_CREAT | O_RDWR, 0666) ) < 0)
		{
		_LIT(Kerr , "Failed to open link file") ;
     	ERR_PRINTF1(Kerr);
     	return KErrGeneral ;
		}
    if(0 > write(fd1, wrbuff, strlen(wrbuff)))
		{
    	_LIT(Kerr , "write through link file failed") ;
     	ERR_PRINTF1(Kerr);
     	return KErrGeneral ;
		}
    if(0 > read(fd1, rdbuff, nbytes1))
		{
    	_LIT(Kerr , "read through link file failed") ;
     	ERR_PRINTF1(Kerr);
     	return KErrGeneral ;
		}    
    _LIT(Kerrnone , "--- read value through link file --- ") ;    	
    INFO_PRINTF1( Kerrnone);
    
    close(fd1);
    
    // Open and write/read through parent file
    if((fd2 = open(file1, O_RDWR | O_APPEND, 0666)) < 0)
		{
    	_LIT(Kerr , "Failed to open parent file") ;
     	ERR_PRINTF1(Kerr);
		INFO_PRINTF2(_L("The errno set is %d"),errno);
     	return KErrGeneral ;
		}    
    if(0 > write(fd2, wrbuff1, strlen(wrbuff1)))
		{
    	_LIT(Kerr , "write through parent file failed") ;
     	ERR_PRINTF1(Kerr);
     	return KErrGeneral ;
		}
    if(0 > read(fd2, rdbuff, nbytes2))
		{
    	_LIT(Kerr , "read through parent file failed") ;
     	ERR_PRINTF1(Kerr);
     	return KErrGeneral ;
		}        
	_LIT(Kerrnone1 , "--- read value through parent file --- ") ;    	
    INFO_PRINTF1( Kerrnone1);
   
    close(fd2);	
    
    return KErrNone;
	}

//-----------------------------------------------------------------------------------

//  Test case : Reads the contents of the Link file
//  return : 0 on success 
                
//-----------------------------------------------------------------------------------

TInt CTestLink::ReadLink()
	{
	TPtrC String;
	char rdbuff[25];
      
    ReadPathName(String);         
    TBuf8<100> string1;
    string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0';
	int fd;
	if(readlink(file1, rdbuff, (sizeof(char)*25)) < 0)
		{
		_LIT(Kerr , "readlink failed") ;    
		INFO_PRINTF2(_L("The errno set is %d"),errno);
    	ERR_PRINTF1(Kerr);
    	return KErrGeneral ;
		}
	if((fd = open("C:\\Logs\\ConsoleOut.txt", O_CREAT | O_RDWR, 0666)) < 0)
		{
    	_LIT(Kerr , "Failed to open console file") ;
     	ERR_PRINTF1(Kerr);
     	return KErrGeneral ;
		}
    if(0 > write(fd, rdbuff, strlen(rdbuff)))
		{
    	_LIT(Kerr , "Write to console file failed") ;
     	ERR_PRINTF1(Kerr);
		unlink("C:\\Logs\\ConsoleOut.txt");
		close(fd);
     	return KErrGeneral ;
		}
	close(fd);
	unlink("C:\\Logs\\ConsoleOut.txt");
	return KErrNone;
	}

//-----------------------------------------------------------------------------------

//  Test case : ReadLink2, Negative case to check non-existing Link File
//     return : KErrGeneral 
                
//-----------------------------------------------------------------------------------

TInt CTestLink::ReadLink1()
	{
	TPtrC String;
	char rdbuff[25];
	TInt retVal=KErrGeneral;
   
    ReadPathName(String);         
    TBuf8<100> string1;
    string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0';
	
	if(readlink(file1, rdbuff, (sizeof(char)*25)) < 0)
		{
		if(ENOENT == errno)
    		{
    		_LIT(Kerr, "Test Case Passed : returned - Path Not Found");
    		ERR_PRINTF1(Kerr);
    		retVal = KErrNone;
    		}
    	else
    		{
    		_LIT(Kerr, "Test Case Failed : returned Invalid Error Value");
    		ERR_PRINTF1(Kerr);
    		retVal = KErrGeneral;
    		}    	
		}
	return retVal;
	}

//-----------------------------------------------------------------------------------

//  Test case : removes the link from a file
//  return : 0 on success 
                
//-----------------------------------------------------------------------------------

TInt CTestLink::Unlink()
	{
	TPtrC String;
    ReadPathName(String);         
    TBuf8<100> string1;
    string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0';
	
	if(unlink(file1) < 0)
		{
		_LIT(Kerr , "unlink failed") ;    	
    	ERR_PRINTF1(Kerr);
    	return KErrGeneral ;
		}	
	return KErrNone;
	}

//-----------------------------------------------------------------------------------

//  Test case : Negative testcase to check unlink on a non-existing link
//  return : 0 on success 
                
//-----------------------------------------------------------------------------------

TInt CTestLink::Unlink1()
	{
	TPtrC String;
    ReadPathName(String);         
    TBuf8<100> string1;
    string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0';
	
	if(unlink(file1) < 0)
		{
		if(ENOENT == errno)
    		{
    		_LIT(Kerr, "Test Case Passed : returned - ENOENT");
    		ERR_PRINTF1(Kerr);
    		return KErrNone;
    		}
    	else
    		{
    		_LIT(Kerr, "Test Case Failed : returned Invalid Error Value");
    		ERR_PRINTF1(Kerr);
    		return KErrGeneral;
    		}    
		}	
	return KErrNone;
	}

//-----------------------------------------------------------------------------------

//  Test case : lstat, Get file information of a link file and not the
//  			  reference it is pointing to
//  return : KErrGeneral 
                
//-----------------------------------------------------------------------------------

TInt CTestLink::linkstat()
	{
	TPtrC String;
	struct stat buf;	
       
    ReadPathName(String);         
    TBuf8<100> string1;
    string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0';
	
	if(0 > lstat(file1, &buf))
		{
		_LIT(Kerr, "Test Case failed : lstat failed");
    	ERR_PRINTF1(Kerr);
    	return KErrGeneral;
		}
	//Test for __xstat
	int ver = 5;
	if(0 > __xstat(ver,file1, &buf))
		{
		_LIT(Kerr, "Test Case failed : __xstat failed");
    	ERR_PRINTF1(Kerr);
    	return KErrGeneral;
		}
	//Test for __lxstat
	if(0 > __lxstat(ver, file1, &buf))
		{
		_LIT(Kerr, "Test Case failed : __lxstat failed");
    	ERR_PRINTF1(Kerr);
    	return KErrGeneral;
		}
	//Test for stat
	FILE* fp=fopen("c:\\logs\\test.txt","w");
	if(!fp)
		{
		INFO_PRINTF1(_L("Unable to create file"));
		return KErrGeneral ;		
		}
	fclose(fp);

	int ret = stat("c:\\logs\\test.txt", NULL);
	if (ret == -1)
		{
		if (errno == EFAULT)
			{
			INFO_PRINTF1(_L("stat() failed with error EFAULT"));
			ret = KErrNone ;	
			}
		else
			{
			INFO_PRINTF1(_L("stat() failed !!!"));
			ret = KErrGeneral ;	
			}
		}
	unlink("c:\\logs\\test.txt");
	return ret;
	}

//-----------------------------------------------------------------------------------

//  Test case : Create a FIFO and remove the FIFO using unlink function with arguments
//  as relative path
//  return : KErrNone 
                
//-----------------------------------------------------------------------------------

TInt CTestLink::TestUnlinkFifo()

{
	TPtrC String ;
       
    ReadPathName(String);
    TBuf8<100> string1;
    string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0';
	TInt ret=KErrGeneral;	
	
    mode_t mode = 0666;
    if(mkdir("c:\\logs\\UNLINKFIFO" , mode) < 0 )  
 	  	{
      	ERR_PRINTF1(_L("Directory creation failed"));
      	return KErrGeneral;
      	}
  		
  	if(chdir("c:\\logs\\UNLINKFIFO") < 0 )  
  		{
     	ERR_PRINTF1(_L("Failed to change working directory"));
		rmdir("c:\\logs\\UNLINKFIFO");
     	return KErrGeneral;
  		}
  		
    if (mkfifo(file1, mode) == -1) 
    	{
        INFO_PRINTF1(_L("mkfifo() failed"));
		rmdir("c:\\logs\\UNLINKFIFO");
        return KErrGeneral;
    	}
    	
    if(unlink(file1))
    	{
        ERR_PRINTF1(_L("unlink on link fifo failed"));
        ret = KErrGeneral;
    	}
    else
		{
		INFO_PRINTF1(_L("Unlink on link fifo succeeded"));		
		ret = KErrNone;
		}
		
	if(rmdir("c:\\logs\\UNLINKFIFO") < 0 )  
  		{
     	INFO_PRINTF1(_L("Rmdir failed "));
     	ret =  KErrNone;
  		}
  			
	return ret;
}

/**
 * Testcase name	:	ReadLinkTest
 * Description		:	tests the return value of readlink in 3 cases
 * 						1. bufsize passed to readlink < length of content of symbolic link
 * 						2. bufsize passed to readlink = length of content of symbolic link
 * 						3. bufsize passed to readlink > length of content of symbolic link
 * Retrun			: 	KErrNone when readlink returns expected value
 * 						KErrGeneral	when readlink returns incorrect value
 */ 
TInt CTestLink::ReadLinkTest()
{
	TPtrC String ;
	TInt buflen=0;

	ReadPathName(String);
	TBuf8<100> string1;
	string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0';
	
	ReadPathName(String);
	TBuf8<100> string2;
	string2.Copy(String);
	char* file2 = (char*) string2.Ptr();
	file2[string2.Length()]='\0';
	
	ReadIntParam(buflen);
	
	FILE* fp=fopen(file1, "w");	
	if(!fp)
	{
		INFO_PRINTF1(_L("Unable to create file"));
		return KErrGeneral ;		
	}
	fclose(fp);
	if(symlink(file1, file2))
		{  
		_LIT(Kerr2, "failed to create link ");
		ERR_PRINTF1(Kerr2);
		unlink(file1);
		return KErrGeneral ;
		}
	else
		{
		char buf[128];
		int len=0;
		
		if((len = readlink(file2, buf, buflen)) < 0)
			{
			_LIT(Kerr3, "failed to read link ");
			ERR_PRINTF1(Kerr3);
			unlink(file1);
			unlink(file2);
			return KErrGeneral;
			}
		else
			{
			if(((buflen <= strlen(file1)) && (len != buflen)) || ((buflen > strlen(file1)) && (len != strlen(file1))))
				{
				_LIT(Kerr4, "read link returned incorrect count");
				ERR_PRINTF1(Kerr4);	
				return KErrGeneral;				
				}
			}
		}
	unlink(file1);
	unlink(file2);
	return KErrNone;
}

