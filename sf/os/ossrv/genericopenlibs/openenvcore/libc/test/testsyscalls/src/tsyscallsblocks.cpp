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


#include "tsyscalls.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <utime.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <sys/syslimits.h>
#include <pthread.h>
#include <sys/types.h>  
#include <sys/utsname.h>

#define CLOCKS_PER_SEC 128

/*******************************************************************************

  Test case : open1 creates a file Prog1.txt  in cdrive
              opens the file file in read-write mode 
              
       return : 0 on success 

                
*******************************************************************************/                           

TInt CTestSyscalls :: Creat(  )
	{
    int fd ;
    TPtrC String ;
    mkdir("C:\\Test" , S_IWUSR);
    _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
    if(!res)  
		{
     	_LIT(Kerr , "Failed to read input file name") ;
     	INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
    TBuf8<100> string1;
	string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';
	 	 
		
    // Print to UI
   
    _LIT( KOpen1, "In create" );
  
    unlink(file) ; 
    INFO_PRINTF1( KOpen1 );
   
    if((fd = creat(file ,  0666))  < 0) 
		{
        _LIT(Kerr , "failed to open the file ") ;
        INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    close(fd) ;
    return KErrNone ;
    
    }


/*******************************************************************************

  Test case : open1 creates a file Prog1.txt  in cdrive
              opens the file file in read-write mode 
              
       return : 0 on success 

                
*******************************************************************************/                           

TInt CTestSyscalls::open1(  )
    {
	int fd ;
    TPtrC String ;
    TInt IsDirectory ;        // This variable is used to find wheather a directory is 
                              //is opened    
    TInt Expected ; 
     
    _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
    if(!res)  
		{
     	_LIT(Kerr , "Failed to read input file name") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
		}
     
    TBuf8<100> string1;
	string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';
	 
	_LIT( KIsDirectory, "IsDirectory" );
	res = GetIntFromConfig(ConfigSection(), KIsDirectory, IsDirectory);
	 
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read directory flag") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 
	_LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read directory flag") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 
	if(Expected < 0 ) 
		{
		fd = open(NULL , O_CREAT | O_RDWR  , 0444) ;
	   
		if(fd < 0)  
			{
	   		if(errno != EFAULT) 
	   			{
	   			_LIT(Kerr , "Failed to set the errno value for open system call") ;
	   			INFO_PRINTF1(Kerr) ;
	   			return KErrGeneral ;
	   			}
	   		return KErrNone ; 
			}
		_LIT(Kerr , "open for NULL address should fail") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	 
		
    _LIT( KTsyscalls, "Tsyscalls" );
    INFO_PRINTF1(KTsyscalls) ;
    _LIT( KOpen1, "In open1" );
  
    unlink(file) ; 
    INFO_PRINTF1( KOpen1 );
   
    if((fd = open(file , O_CREAT | O_RDWR , 0666))  < 0) 
		{
        if(IsDirectory && (errno == EISDIR) )   
			{
        	_LIT(Kerrnone , "Directory open test case passes ") ;
        	INFO_PRINTF1(Kerrnone) ;
        	return KErrNone ;
			}
        else if(IsDirectory) 
			{
        	_LIT(Kerr , "Directory open test case fails to set err value") ;
        	INFO_PRINTF1(Kerr) ;
        	return KErrGeneral ;
			}
		_LIT(Kerr , "failed to open the file ") ;
		INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
   
    close(fd) ;
    return KErrNone ;
    
    }


//-------------------------------------------------------------------------------

//  Test case : open2 creates a file
//              opens in write only mode
//              return : 0 on success 
//              1 failure
//--------------------------------------------------------------------------------                  
TInt CTestSyscalls :: open2()
	{
	int fd ;
	TPtrC String ;
    _LIT( KString, "String" );
     TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(res)
		{
	    TBuf8<100> string1;
		
		string1.Copy(String);
		char* file = (char*) string1.Ptr();
		file[string1.Length()]='\0';
	    _LIT( KOpen3, "In open3" );
	  
	    INFO_PRINTF1( KOpen3 );
	    unlink(file);

	    if((fd = open(file , O_CREAT | O_WRONLY , 0666))  < 0) 
			{
			return KErrGeneral ;
			}
	    close(fd) ;
		}
    return KErrNone ;
    
	}

//-------------------------------------------------------------------------------

//  Test case : open3 creates a file
//              opens in readonly mode
//              return : 0 on success 

//--------------------------------------------------------------------------------                  
TInt CTestSyscalls :: open3()
	{
	int fd ;
	TPtrC String ;
     
	_LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(res)
		{
	    TBuf8<100> string1;
		string1.Copy(String);
		char* file = (char*) string1.Ptr();
		file[string1.Length()]='\0';
		
		_LIT( KOpen3, "In open3" );
	    INFO_PRINTF1( KOpen3 );
	    
	    unlink(file);
	    if((fd = open(file ,  O_RDONLY | O_CREAT , 0666))  < 0) 
			{
	        return KErrGeneral ;
			}
	    close(fd) ;
	    return KErrNone  ;
		}
	else
		{
		return KErrGeneral;
		}
	}



//-------------------------------------------------------------------------------

//  Test case : open4 creates a file  (Negative testcase )
//   
//  Tries to open a fails  in invalid  mode           
//             
// Create a file Exclusively
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: open4()
	{
	int fd ;
   
	TBuf8<100> string1;
	TPtrC String ;
    
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res)
		{
	    _LIT(Kerr , " Failed to  read input file name from ini file ") ;
	    INFO_PRINTF1(Kerr) ;
	    return KErrGeneral ;
		}
	    
	string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';
	_LIT( KOpen4, "In open4" );
	INFO_PRINTF1( KOpen4 );
    if((fd = open(file , O_CREAT | O_RDWR  , 0666))  < 0) 
		{
        _LIT(Kerr , "Failed to o given file name") ;
        INFO_PRINTF1(Kerr) ;
        return  KErrGeneral  ;
		}
    close(fd) ;
    
    //Now try to open the file exclusively , this open sustem call should fail
    
    fd = open(file , O_CREAT | O_EXCL | O_RDWR , 0666) ;
    if(fd < 0 )  
		{
    	if(errno != EEXIST) 
    		{
    		_LIT(Kerr , "Failed to set errno value for open syscall") ;
    		INFO_PRINTF1(Kerr) ;
    		return KErrGeneral ;
    		}
    	return KErrNone ;
		}
	_LIT(Kerr , "Failed to open a file in O_EXCL mode ") ;
	INFO_PRINTF1(Kerr) ;
	return KErrGeneral ;
	}


//-------------------------------------------------------------------------------

//  Test case : open5 creates a file  (Negative testcase )
//   
//  Tries to open a fails  in invalid  path
//             
// 
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: open5()
	{
	int fd ;
	
    TInt ret = KErrNone ;
	TBuf8<100> string1;
    TPtrC String ;
    
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );

    if(!res)    
		{
    	_LIT(Kerr , "Failed to read input file name from ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
	string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';
	
	_LIT( KOpen5, "In open5" );
   
    INFO_PRINTF1( KOpen5 );
    unlink(file);

    
    if((fd = open(file , O_CREAT | O_RDONLY , 0666))  < 0) 
		{
        if(errno != ENOENT)   
			{
        	_LIT(Kinvalidpath , "failed to  set errno value ") ;
        	INFO_PRINTF1(Kinvalidpath);
        	ret = KErrGeneral ;
			}
        }
    else 
		{
       _LIT(Kerr , "File opened in an invalid path test fails ") ;
	   close(fd);
       INFO_PRINTF1(Kerr) ;
       ret = KErrGeneral ;
		}
	close(fd) ;
	return ret ;    //This ret will be set to KErrNone if all test cases are though
	}


//-------------------------------------------------------------------------------

//  Test case : open6 truncates the file //   
//  
//             
// 
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: open6()
	{
	int fd ;
	TBuf8<100> string1;
    TPtrC String ;
    
    _LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );

    if(!res)   
		{
    	_LIT(Kerr , "Failed to read input file value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
	string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';
	
    _LIT( KOpen5, "In open6" );
    INFO_PRINTF1( KOpen5 );
  
    unlink(file);

    if((fd = open(file , O_CREAT|O_RDWR , 0666))  < 0) 
		{
        _LIT(Kinvalidarg , "Failed to open specified file \n")  ;
        INFO_PRINTF1(Kinvalidarg) ;
        return KErrGeneral ;
		}
    if(write(fd , "hELLO" ,6) < 0) 
		{
		_LIT(Kerr , "Failed to write to the file D:\\Prog1.txt") ;
		INFO_PRINTF1(Kerr) ;
		close(fd) ;
		return KErrGeneral ;
		}
    close(fd) ;
    if((fd = open(file , O_CREAT|O_RDWR|O_TRUNC , 0666))  < 0)  
		{
    	_LIT(Kerr , "Failed to tuncate the specified file ") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    //seek  and find out the  size of the file
    
    if(lseek(fd , 0 , SEEK_END) != 0 )  
		{
    	_LIT(Kerr , "file could not be truncated ") ;
    	INFO_PRINTF1(Kerr) ;
		close(fd);
    	return  KErrGeneral ;
		}
	close(fd);
    return KErrNone ; 
    
	}

/**
 * Testcase 	:	OpenTruncate1
 * Description	: 	Open a file with O_CREAT | ORDWR flag
 * 					Re-open the same file with O_TRUNC
 * 					stat on the file to check whether the file is trunctaed or not
 * Return value	:	KErrNone
 */
TInt CTestSyscalls :: OpenTruncate1()
	{
	int fd1, fd2, ret;
	TBuf8<128> string1;
	TPtrC String ;
	struct stat fInfo;
	
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	
	if(!res)   
		{
	   	_LIT(Kerr , "Failed to read input file value from the ini file") ;
	   	INFO_PRINTF1(Kerr);
	   	return KErrGeneral;
		}
	string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';
	
	if((fd1 = open(file, O_CREAT | O_RDWR)) < 0)
		{
		INFO_PRINTF2(_L("file open failed with error number %d"), errno);
		return KErrGeneral;
		}
	
	if ((write(fd1, "Symbian", 7)) != 7)
		{
		INFO_PRINTF2(_L("write failed with error number %d"), errno);
		close(fd1);
		unlink(file);
		return KErrGeneral;
		}
	
	// re-open the same file with O_CREAT
	if((fd2 = open(file, O_TRUNC | O_WRONLY)) < 0)
		{
		INFO_PRINTF2(_L("file open with O_TRUNC failed with error number %d"), errno);
		close(fd1);
		unlink(file);
		return KErrGeneral;
		}
	
	ret = stat(file, &fInfo);
	
	if (ret == -1)
		{
		INFO_PRINTF2(_L("stat failed with error number %d"), errno);
		close(fd1);
		unlink(file);
		return KErrGeneral;		
		}
	
	if(fInfo.st_size != 0)
		{
		INFO_PRINTF1(_L("The file is not truncated. Testcase failed"));
		close(fd1);
		unlink(file);
		return KErrGeneral;
		}
	
	close(fd2);
	unlink(file);
	return KErrNone;
	}

/**
 * Testcase 	:	OpenTruncate2
 * Description	: 	open a non existing file with flag O_TRUNC
 * 					It should not create a new file 
 * Return value	:	KErrNone
 */
TInt CTestSyscalls :: OpenTruncate2()
	{
	int fd1, ret;
	TBuf8<128> string1;
	TPtrC String ;
		
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	
	if(!res)   
		{
	   	_LIT(Kerr , "Failed to read input file value from the ini file") ;
	   	INFO_PRINTF1(Kerr);
	   	return KErrGeneral;
		}
	string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';
	
	if((fd1 = open(file, O_TRUNC)) < 0)
		{
		INFO_PRINTF1(_L("Test case passed"));
		ret = KErrNone;
		}
	else
		{
		INFO_PRINTF1(_L("open with only flag O_TRUNC should not creat a new file. Test case failed"));
		close(fd1);
		unlink(file);
		ret = KErrGeneral;
		}
	
	close(fd1);
	unlink(file);
	return ret;
	}
//-----------------------------------------------------------------------------
//  Try to open a read only file in  Write only mode

//---------------------------------------------------------------------------

TInt CTestSyscalls :: open7()
	{
	int fd ;
	TInt ret = KErrNone ;
	TBuf8<100> string1;
    TPtrC String ;
    
    _LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );

    if(!res)   
		{
    	_LIT(Kerr , "Failed to read input file value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    
	string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';
	
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	
    _LIT( KOpen7 , "In open7" );
    INFO_PRINTF1( KOpen7 );
    
    unlink(file) ;
    
    //First create read only file , close it and they try to 
    //open in W_ONLY mode 
    
    if((fd = open(file , O_CREAT|O_RDWR , 0444))  < 0) 
		{
        _LIT(Kinvalidarg , "Failed to open specified file \n")  ;
        INFO_PRINTF1(Kinvalidarg) ;
        return KErrGeneral ;
		}
    
    close(fd) ;
    
    if((fd = open(file , O_CREAT|O_WRONLY ) ) < 0) 
		{
         if(errno !=EACCES)  
			{
         	_LIT(Kerr , "Failed to set errno value ") ;
         	INFO_PRINTF1(Kerr);
         	ret = KErrGeneral ;
			}        
		}
    else
		{
        _LIT(KErr , "Opened a read only file in Write mode hence failed ") ;
		close(fd);
		unlink(file);
        INFO_PRINTF1(KErr);
    	ret = KErrGeneral ;
		}
    
    close(fd) ;
    chmod(file , S_IWUSR);
    unlink(file) ;
    return ret ; 
    
	}



/**
* Function Name :	OpenInAppendMode
* API Tested 	:	write
* TestCase Description:	This test case is added as part of fix for DEF114524
*						1. A file is opened in Append mode twice
*						2. Few bytes are written into file using two different FIDs
*						3. File is closed using both the FIDs
* Expected result:	
*/
TInt CTestSyscalls::OpenInAppendMode()
	{
	int fd[2];
	char Buf[10+1];
	TInt ret = KErrNone;
	TBuf8<100> string1;
    TPtrC String;
    
    _LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String);

    if(!res)   
		{
    	_LIT(Kerr , "Failed to read input file value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    
	string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';
	
	
	if( (fd[0] = open(file, O_WRONLY | O_APPEND | O_CREAT)) < 0)
		{
		_LIT(KFail, "Failed to open file in append mode");
        INFO_PRINTF1(KFail);
        return KErrGeneral;
		}
		
	if((fd[1] = open(file, O_WRONLY | O_APPEND | O_CREAT)) < 0)
		{
		_LIT(KFail, "Failed to open file in append mode");
        INFO_PRINTF1(KFail);
        return KErrGeneral;
		}
	
	if((write(fd[0], "Test", 4)) != 4)
		{
		INFO_PRINTF1(_L("Write API did not write expected number of characters into file"));
		close(fd[0]);
        return KErrGeneral;
		}
	
	if((write(fd[1], "Append", 6)) != 6)
		{
		INFO_PRINTF1(_L("Write API did not write expected number of characters into file"));
		close(fd[0]);
		close(fd[1]);
        return KErrGeneral;
		}
	
	close(fd[0]);
	close(fd[1]);
	
	if((fd[0] = open(file , O_CREAT | O_RDWR , 0666))  < 0) 
		{
		_LIT(Kopen , "Failed to open file " ) ;
		INFO_PRINTF1(Kopen) ;
        return KErrGeneral ;
		}
	
	memset(Buf, 0, sizeof(Buf));
	if(read(fd[0], Buf, 10) == 10) 
		{
		Buf[10]='\0';
        if(strcmp(Buf, "TestAppend"))
			{
    		ret = KErrGeneral ;
			}
		else
			{
			ret = KErrNone;
			}
		}
	else
		{
		INFO_PRINTF1(_L("read() failed"));
		ret = KErrGeneral ;
		}	
	
	close(fd[0]);
	unlink(file);
	return ret;
	}


//-------------------------------------------------------------------------------

//  Test case : Write  Writes specified no of chars to the file 
//            : returns 0 on success    
//            : 2 on failure      
//             
// 
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: write1()
	{
	int fd ;
	TInt Expected ;          //Expected valued to be returned by syscall
	TInt RetVal ;            //Actual value returned by syscall
	TBuf8<100> string1;
    TPtrC String ;
    
   //first read file name , buffer and then 
   //actual no of bytes to be written
     _LIT( KString, "String" );
     TBool res = GetStringFromConfig(ConfigSection(), KString, String );
     if(!res)   
		{
    	_LIT(Kerr , "Failed to read input file value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
      
	string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';
	_LIT( Kwrite1, "In write1" );
    INFO_PRINTF1( Kwrite1 );
    
    //Read no of bytes 
    TInt nBytes ;
    _LIT( KnBytes, "nBytes" );
	res = GetIntFromConfig(ConfigSection(), KnBytes, nBytes);
    
    if(!res)    
		{
    	_LIT(Kerr , "Failed to read input number of bytes  from the inifile ") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ; 
		}
    
    //Read actual string to be written 
    
    TBuf8<100> Buf;
    TPtrC buff    ;
    
    _LIT( Kbuff, "buff" );
    res = GetStringFromConfig(ConfigSection(), Kbuff, buff);
    if(!res) 
		{
    	 _LIT(Kerr , "Failed to read actual buffer form the ini file") ;
    	 INFO_PRINTF1(Kerr) ;
    	 return KErrGeneral ;
		}
	Buf.Copy(buff);
	char* Buffer = (char*) Buf.Ptr();
	Buffer[Buf.Length()]='\0';  
	
	
	//Now read expected value 
	_LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
    unlink(file);
    
    if((fd = open(file  , O_CREAT | O_RDWR , 0666))  < 0) 
		{
        _LIT(Kerr , "Failed to open specifed file \n") ;
        INFO_PRINTF1(Kerr);
        return KErrGeneral ;
		}
    
    if((RetVal = write(fd , Buffer , nBytes) ) < 0)  
		{
         _LIT(Kerr , "Failed to write specifed no of bytes to file ") ;
         INFO_PRINTF1(Kerr) ;
         if(nBytes > 0)
			{
			 close(fd);
			return KErrGeneral;
			}
		}
        
    close(fd) ;
    if(RetVal == Expected)
		{
		return KErrNone ;
		}
    
    return KErrGeneral ;
    
	}



//-------------------------------------------------------------------------------

//  Test case : Writes specified no of chars to the invalid fd 
//                 
//             
// 
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: write2()
	{
    _LIT( Kwrite2, "In write2" );
    INFO_PRINTF1( Kwrite2 );
    TInt fd ;
    
    _LIT( Kfd, "fd" );
	TBool res = GetIntFromConfig(ConfigSection(), Kfd, fd);
    
    if(!res)   
		{
    	_LIT(Kerr , "Failed to read input file value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    
    if(write(fd , "Hello" , 6) < 0)  
		{
        if(errno == EBADF) 
			{
           _LIT(Kok , "Invalid file descriptor passed \n") ;
        	INFO_PRINTF1(Kok) ;
        	if(write(-1 , "Hello" , 6) < 0) 
        		{
        		if(errno == EBADF) 
					{
					_LIT(Kok , "Invalid file descriptor passed \n") ;
					INFO_PRINTF1(Kok) ;
        			}
        		else 
					{
					_LIT(Kerr , "failed to set errno value \n") ;
        			INFO_PRINTF1(Kerr) ; 
        			return KErrGeneral ;
        			}
        	
				}
			}
        else 
			{
            _LIT(Kerr , "failed to set errno value \n") ;
        	INFO_PRINTF1(Kerr) ; 
        	return KErrGeneral ;
        	}
		}
    return KErrNone ;
    }


//-------------------------------------------------------------------------------

//  Test case : Read in an invalid Buffer address
//--------------------------------------------------------------------------------                  



TInt CTestSyscalls :: write3()
	{
 	int fd ;
 
 	TPtrC String ;
    TBuf8<100> string1;
 	
    _LIT( Kread3, "In write3" );
    INFO_PRINTF1( Kread3 );
  	
  	_LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
     
  	if(!res) 
		{
       _LIT(Kerr , "Failed to read the input file name from ini file") ;
       INFO_PRINTF1(Kerr) ;	
		}
    
	string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';
	
	unlink(file);
  
  	if((fd = open(file , O_CREAT | O_RDWR , 0666))  < 0) 
		{
       _LIT(Kerr , "Failed to open file " ) ;
       INFO_PRINTF1(Kerr) ;
        return KErrGeneral ;
		}
    
    if(write(fd , NULL , 3) < 0)  
		{
        if(errno != EFAULT)   
			{
        	_LIT(Kerr , "Failed to set the errno value for invalid address") ;
        	INFO_PRINTF1(Kerr) ;
			close(fd);
        	return KErrGeneral ;
			}
    	
		}
    
    _LIT(Kok , "Read with invalid buffer address passed") ;
    INFO_PRINTF1(Kok) ; 
    close(fd) ;
    return KErrNone ;
    
    
	}




//-------------------------------------------------------------------------------
//  Test case : Write to file opened in R_ONLY MODE , 
//                      
//             
// 
//--------------------------------------------------------------------------------                  

TInt CTestSyscalls :: write5()
	{
 	int fd ;
	TInt ret = KErrNone ;
	TBuf8<100> string1;
    TPtrC String ;
    
    //Read the input file name 
    _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
    
    if(!res)   
		{
    	_LIT(Kerr , "Failed to read input file value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    
	string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';

    _LIT( Kwrite5, "In write5" );
    INFO_PRINTF1( Kwrite5 );
    unlink(file) ;
    
  	if((fd = open(file , O_CREAT | O_RDONLY , 0666))  < 0) 
		{
       _LIT(Kwrite5 , "Failed to open file " ) ;
       INFO_PRINTF1(Kwrite5) ;
        return KErrGeneral ;
		}
    
    if(write(fd , "Hello" , 25) < 6)  
		{
		int d = errno;
        if(errno == EBADF) 
			{
			ret = KErrNone ;	
			}
    	else 
    		{
    		_LIT(Kerr , "Failed to set the errno value to EBADF") ;
    		INFO_PRINTF1(Kerr);
			close(fd);
    		ret = KErrGeneral ;
    		}
		}
    close(fd) ;
    unlink(file) ;
    return ret ;
    }


//-------------------------------------------------------------------------------

//  Test case : read specified bytes from a file, 
//              return 0  on success
//				3 on failure
// 
//--------------------------------------------------------------------------------                  



TInt CTestSyscalls :: read1()
	{
 	int fd ;
 	TInt Retval ;
 	char Buf[20] ;
	TBuf8<100> string1;
    TPtrC String ;
    
    //Read the input file name 
    _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
    
    if(!res)   
		{
    	_LIT(Kerr , "Failed to read input file value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    
	string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';
	
	TInt nbytes ;
	//read no of bytes to be read 
	
	_LIT( Knbytes, "nbytes" );
	res = GetIntFromConfig(ConfigSection(), Knbytes, nbytes);
    
    if(!res) 
		{
		_LIT(Kerr , "Failed to read input no of bytes to be read ") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	
	//read Expected return value
	TInt Expected ;
	
	_LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
    
	if(!res) 
		{
		_LIT(Kerr , "Failed to read input param Expected value \n") ;
		ERR_PRINTF1(Kerr);
		return KErrGeneral ;
		}
    _LIT( Kread1, "In read1" );
    INFO_PRINTF1( Kread1 );
    unlink(file);
    
  	if((fd = open(file , O_CREAT | O_RDWR , 0666))  < 0) 
		{
		_LIT(Kopen , "Failed to open file " ) ;
		INFO_PRINTF1(Kopen) ;
        return KErrGeneral ;
		}
    
    if(( write(fd , "HELLO HELLO HELLO HELLO HELLO HELLO HELLO"  , 20) )<0 ) 
		{
    	_LIT(Kerr , "Failed to write to the specified file ") ;
    	INFO_PRINTF1(Kerr) ;
		close(fd);
    	return KErrGeneral ;
		}
    close(fd) ; //open again and try to read (or do an lseek)
    
    
  	if((fd = open(file , O_CREAT | O_RDWR , 0666))  < 0) 
		{
		_LIT(Kopen , "Failed to open file " ) ;
		INFO_PRINTF1(Kopen) ;
        return KErrGeneral ;
		}
    
    if((Retval = read(fd , Buf , nbytes) ) < 0)  
		{
    	_LIT(Kerr , "Failed to read from the specified file \n") ;
		close(fd);
    	INFO_PRINTF1(Kerr);
    	}
    
    if(Retval != Expected) 
		{
    	_LIT(Kerr , "Expected and the actual return values dont match") ;
    	ERR_PRINTF1(Kerr);
		close(fd);
    	return KErrGeneral ;
		}
    else
		{
    	_LIT(Kerr , "Expected and the actual return values match") ;
    	INFO_PRINTF1(Kerr);
		}
   
    close(fd) ;
    return KErrNone ;
	}


//-------------------------------------------------------------------------------

//  Test case :  Read from the given fd , no of bytes > size of file
//--------------------------------------------------------------------------------                  



TInt CTestSyscalls :: read2()
	{
 	int fd ;
 	char Buf[20] ;
	TInt ret = KErrNone ;
	TBuf8<100> string1;
    TPtrC String ; 
    _LIT( Kread2, "In read2" );
    INFO_PRINTF1( Kread2 ) ;
    
    //Read the input file name
    _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
 
    if(!res) 
		{
		_LIT(Kerr , "Failed to read the input file name from ini file") ;
		INFO_PRINTF1(Kerr) ;	
		}
    
	string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';
	
	unlink(file);
  	if((fd = open(file , O_CREAT | O_RDWR | O_TRUNC, 0666))  < 0) 
		{ 
		_LIT(Kerr , "Failed to open the given file ") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
    
    
    //write  20 bytes to the file  and then try to read 30 bytes from the file
    if( write(fd , "HELLO HELLO HELLO HELLO HELLO HELLO HELLO asdf adsf"  , 20)  != 20 ) 
		{
    	_LIT(Kerr , "Failed to write to the specified file ") ;
    	INFO_PRINTF1(Kerr) ;
		close(fd);
    	return KErrGeneral ;
		}
    
    //Now reset the file pointer back to zero 
    if(lseek(fd , 0 , SEEK_SET) < 0 )  
		{
    	_LIT(Kerr , "Failed to reset the file pointer " ) ;
    	INFO_PRINTF1(Kerr) ;
		close(fd);
    	return KErrGeneral ;
		}
    ret = read(fd , Buf , 30) ;
    
    if(ret != 20)
		{
        _LIT(Kerr , "No of bytes returned is greater than or equal to 30 ") ;
        INFO_PRINTF1(Kerr) ;
		close(fd);
    	return KErrGeneral ;
		}
	close(fd) ;
    return KErrNone ;
	}


//-------------------------------------------------------------------------------

//  Test case : Read in an invalid Buffer address
//--------------------------------------------------------------------------------                  



TInt CTestSyscalls :: read3()
	{
 	int fd ;
 	TPtrC String ;
    TBuf8<100> string1;
 	
    _LIT( Kread3, "In read3" );
    INFO_PRINTF1( Kread3 );
  	
  	_LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );

  	if(!res) 
		{
		_LIT(Kerr , "Failed to read the input file name from ini file") ;
		INFO_PRINTF1(Kerr) ;	
		}
    
	string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';
	
	unlink(file);
  
  	if((fd = open(file , O_CREAT | O_RDWR , 0666))  < 0) 
		{
		_LIT(Kerr , "Failed to open file " ) ;
		INFO_PRINTF1(Kerr) ;
        return KErrGeneral ;
		}
    
    if(read(fd , NULL , 3) < 0)  
		{
        if(errno != EFAULT)   
			{
        	_LIT(Kerr , "Failed to set the errno value for invalid address") ;
        	INFO_PRINTF1(Kerr) ;
			close(fd);
        	return KErrGeneral ;
			}
    	}
    
    _LIT(Kok , "Read with invalid buffer address passed") ;
    INFO_PRINTF1(Kok) ; 
    close(fd) ;
    return KErrNone ;
    }



//-------------------------------------------------------------------------------

//  Test case :  Read from the from invalid arguments 
//--------------------------------------------------------------------------------                  



TInt CTestSyscalls :: read4()
	{
 	int fd ;
 	TInt ret = KErrNone ;
 	char Buf[20] ;
    TBuf8<50> string1;
    TPtrC String ; 

    _LIT( Kread4, "In read4" );
    INFO_PRINTF1( Kread4 );
	if(read(-1 , Buf , 3) < 3)  
		{
        if(errno != EBADF) 
			{
        	_LIT(Kerr , "Failed to set errno value ") ;
        	INFO_PRINTF1(Kerr) ;
        	ret = KErrGeneral ;
			}
      
		}
     
     //Read the file name  from the ini file 
    _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
  
    if(!res) 
		{
		_LIT(Kerr , "Failed to read the input file name from ini file") ;
		INFO_PRINTF1(Kerr) ;	
		}  
    
    string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0'; 
	  
     
    if((fd = open(file , O_CREAT | O_WRONLY , 0666))  < 0) 
		{
		_LIT(Kopen , "Failed to open file " ) ;
		INFO_PRINTF1(Kopen) ;
		return KErrGeneral  ;
		}
    
    if(read(fd , Buf , 6)  < 0) 
		{
    	if(errno != EBADF) 
    		{
    		_LIT(KErr , "Read from invalid fd failed \n") ;
    		INFO_PRINTF1(KErr) ;
			close(fd);
    		ret = KErrGeneral ;
    		}
		}
    
	else   
		{
		_LIT(Kerr , "Read from a write only file des should fail") ;
		INFO_PRINTF1(Kerr) ;
		ret = KErrGeneral ;
		}
    close(fd);
    return ret ; 

	}
//-------------------------------------------------------------------------------

//  Test case : open a directory
//            : returns 0 on success
//              4 on failure            
//--------------------------------------------------------------------------------                  

TInt CTestSyscalls :: Opendir()
	{
 	DIR *DirHandle ;
 
 	TBuf8<50> string1;
    TPtrC String ; 
    
    TInt Expected ;

    _LIT( Kopendir, "In opendir" );
    INFO_PRINTF1( Kopendir );
  
    //Read the directory  name  from the ini file 
    _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
    if(!res) 
		{
       	_LIT(Kerr , "Failed to read the input file name from ini file") ;
       	INFO_PRINTF1(Kerr) ;	
		}  
    
    string1.Copy(String);
	char* directory = (char*) string1.Ptr();
	directory[string1.Length()]='\0';   
    
    _LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);

    if(!res) 
		{
    	_LIT(Kerr , "Failed to read expected return value from ini file ") ;
    	INFO_PRINTF1(Kerr) ;
    	return  KErrGeneral ;
		}
    int status = mkdir(directory,S_IRWXU | S_IRWXG );
    
    if (status != 0 )    
    	{
    	int d = errno;
    	if (errno == EEXIST)
    		{
    		INFO_PRINTF1(_L("Directory already present")) ;
    		}
    	else
    		{
    		ERR_PRINTF1(_L("Can't create directory"));
	    	return  KErrGeneral ;
	   		}
       	}
    DirHandle = opendir(directory)  ;
    if(Expected  == 0)     //for negative test cases 
		{
       	if(Expected != (TInt) DirHandle)  
       		{
       	    _LIT(Kerr , "Expected and the return values are different") ;
       	    INFO_PRINTF1(Kerr) ;
			rmdir(directory);
       	    return  KErrGeneral ; 
       		}
       	}
     else 
		{
     	if(DirHandle == (DIR *)NULL ) 
     		{
     		_LIT(Kerr , "Failed to open given directory") ;
     		INFO_PRINTF1(Kerr) ;
			rmdir(directory);
     		return KErrGeneral ;
     		}
		}
	closedir(DirHandle);
    rmdir(directory);
    return KErrNone ;
	}	


//------------------------------------------------------------------------
//
// test : Closedir
//------------------------------------------------------------------------


TInt CTestSyscalls :: Closedir() 
	{
   
    DIR *DirHandle ;
  	TBuf8<50> string1;
    TPtrC String ; 
    
    TInt Expected ;
    
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kclosedir , "In Closedir" );
	INFO_PRINTF1(Kclosedir);
   

    //Read the directory  name  from the ini file 
    _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
  
    if(!res) 
		{
       _LIT(Kerr , "Failed to read the input file name from ini file") ;
       INFO_PRINTF1(Kerr) ;	
		}  
    
    string1.Copy(String);
	char* directory = (char*) string1.Ptr();
	directory[string1.Length()]='\0';   
    
    _LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);

    if(!res) 
		{
    	_LIT(Kerr , "Failed to read expected return value from ini file ") ;
    	INFO_PRINTF1(Kerr) ;
    	return  KErrGeneral ;
		}
    
    
    if(Expected < 0 ) 
		{
	    closedir(NULL) ;
	    
        if(errno != EBADF) 
			{
	    	_LIT(Kerr , "Failed to set errno value") ;
	    	INFO_PRINTF1(Kerr) ;
	    	return KErrGeneral ;
			}
	    
		return KErrNone ;
		}
	DirHandle = opendir(directory)  ;
	
	if(DirHandle == (DIR *) NULL) 
		{
		_LIT (Kerr , "Failed to open the directory") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	if(closedir(DirHandle) < 0 )   
		{
		if(errno != EBADF) 
			{
			_LIT(Kerr , "Failed to set errno value ") ;
			INFO_PRINTF1(Kerr) ;
			return KErrGeneral ;
			}
		return KErrNone ;
		}
	return KErrNone ;
	}


/*
//-------------------------------------------------------------------------------

//  Test case : open  directory (negative test cases )
//            :       
//--------------------------------------------------------------------------------                  

TInt CTestSyscalls :: Opendir1()
{
 	DIR *DirHandle ;
 	char Buf[20] ;
	_LIT( KTsyscalls, "Tsyscalls" );
    _LIT( Kopendir, "In opendir1" );
    INFO_PRINTF1( Kopendir );
  	
  	if((DirHandle = opendir("Logs") ) != NULL)
    {
         _LIT( Kopendir, "Opendir for invalid argument failed" );
         INFO_PRINTF1(Kopendir) ;
         return	KErropendir  ;
    }
    
    if((DirHandle = opendir("C:\\testdir")) != NULL)   
    {
    	_LIT( Kopendir, "Opendir for invalid argument failed" );
         INFO_PRINTF1(Kopendir) ;
         return	KErropendir  ;
    }
    return KOpendir ;
}
*/

//-------------------------------------------------------------------------------

//  Test case : read  a directory
//            : returns 0 on success
//              5 on failure            
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Readdir()
	{
 	DIR *DirHandle ;
 	TBuf8<50> string1;
    TPtrC String ; 
    _LIT( Kreaddir, "In readdir" );
    INFO_PRINTF1( Kreaddir );
    
    //Read the directory  name  from the ini file 
    _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
     
    if(!res) 
		{
		_LIT(Kerr , "Failed to read the input file name from ini file") ;
		INFO_PRINTF1(Kerr) ;	
		}  
    
    string1.Copy(String);
	char* directory = (char*) string1.Ptr();
	directory[string1.Length()]='\0';   
    if((DirHandle = opendir(directory) )== NULL)
		{
        _LIT(Kerr , "Failed to open the given directory ") ;
        INFO_PRINTF1(Kerr) ;
        return KErrGeneral ;
        }
    if(readdir(DirHandle) == (dirent *)NULL)
		{
        _LIT(Kerr , " Falied to read the directory") ;
        ERR_PRINTF1(Kerr);
        return KErrGeneral  ;
		}
	closedir(DirHandle);
	if(readdir(DirHandle) == (dirent *)NULL  && errno == EBADF)
  		{
        return KErrNone;
  		}
  	else
  		{
  		ERR_PRINTF2(_L("readir failed for invalid DIR list,Expected errno = 9 and actual errno is = %d "),errno);
  		return KErrGeneral;	
  		}
	}

//-------------------------------------------------------------------------------

//  Test case : seek to specified no of bytes in a file
//            : returns 0 on success
//              6 on failure            
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Lseek()
	{
 	int fd ;
	TInt Seekbytes ; //read the seek 
	TInt ret ;
	TBuf8<50> string1;
    TPtrC String ; 
    
	
	_LIT( KTsyscalls, "Tsyscalls" );
    _LIT( Klseek, "In Lseek" );
    INFO_PRINTF1(KTsyscalls) ;
    INFO_PRINTF1( Klseek );
    
    _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
   
    if(!res) 
		{
		_LIT(Kerr , "Failed to read the input file name from ini file") ;
		INFO_PRINTF1(Kerr) ;	
		}  
    
    string1.Copy(String);
	char* file = (char*) string1.Ptr();
	file[string1.Length()]='\0';   
	
	
	_LIT( KSeekbytes, "Seekbytes" );
	res = GetIntFromConfig(ConfigSection(), KSeekbytes, Seekbytes);
    if(!res) 
		{
    	_LIT(Kerr , "Failed to read seekbytes value from ini file ") ;
    	INFO_PRINTF1(Kerr) ;
    	return  KErrGeneral ;
		}  
	unlink(file) ;
    //SEEK AN EMPTY FILE
    
    if((fd = open(file , O_CREAT | O_RDWR , 0666))  < 0) 
		{
		return KErrGeneral ;
		}
      
    ret =  lseek(fd , Seekbytes ,SEEK_SET) ;
    if(ret < 0 )   
		{
    	_LIT(Kerr , "Failed to SEEK_SET specified bytes ") ;
    	INFO_PRINTF1(Kerr) ;
        }
    ret = lseek(fd ,Seekbytes , SEEK_END ) ;
    
    if(ret < 0 )
		{
    	_LIT(Kerr , "Failed to SEEK_END specified bytes  seeking beyond end of file Not supported") ;
    	INFO_PRINTF1(Kerr) ;    
		}
    if(lseek(fd , Seekbytes , SEEK_CUR) < 0) 
		{ 
        _LIT(Kerr , "Seeking beyond end of the file not supported") ;
        INFO_PRINTF1(Kerr) ;
	    }
	close(fd);
	return KErrNone ;
	}


//-------------------------------------------------------------------------------

//  Test case : lseek test (negative tests )
//            : returns 0 on success
//              6 on failure            
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Lseek1()
	{
 	int fd ;
 	char Buf[20] ;
 	
	_LIT( KTsyscalls, "Tsyscalls" );
    _LIT( Klseek, "In Lseek" );
    INFO_PRINTF1(KTsyscalls) ;
    INFO_PRINTF1( Klseek );
    
    if((fd = open("C:\\Test\\Prog1234.txt" , O_CREAT | O_RDWR | O_TRUNC , 0666))  < 0) 
		{
      return KErrGeneral ;
		}
       
    if(write(fd , "HELLO HELLO hello hello" , 20) < 0) 
		{
    	_LIT(Kopen , "Failed to write to file ") ;
    	INFO_PRINTF1(Kopen) ;
		close(fd);
    	return KErrGeneral ;
		}
    
    if(lseek(fd , 2 ,SEEK_SET) < 0)   
		{
		close(fd);
    	return KErrGeneral ;
		}
    
    if(read(fd , Buf , 3)   > 0) 
		{
        if(strncmp(Buf , "LLO" , 3))
			{
			close(fd);
    		return KErrGeneral ;
			}
		}

    if(lseek(fd , 1 , SEEK_CUR) < 0) 
		{
		close(fd);
    	return KErrGeneral ;
		}
    
    if(read(fd , Buf , 5) > 0) 
		{
        if(strncmp(Buf , "HELLO" , 5))
			{
			close(fd);
    		return KErrGeneral;
			}
		}
        
    //Try to seek beyond the end of file
    
    if(lseek(fd , 10 , SEEK_END) < 0)     
		{
    	_LIT(Kseek , "seek beyond the end of file Not supported") ;
    	INFO_PRINTF1(Kseek) ;
        }
    //Try to seek -ve no of bytes from the start of the file 
    
    if(lseek(fd , -10 , SEEK_SET) < 0) 
		{
    	if(errno != EINVAL) 
    		{
    		_LIT(Kseek , "Failed to seek -ve no of bytes from starting Not supported\n") ;
    		INFO_PRINTF1(Kseek) ;
			close(fd);
    		return KErrNone ;
    		}
		}
	close(fd);
	return KErrNone ;
	}





//-------------------------------------------------------------------------------

//  Test case : access tests  R_OK for given file
//            : returns 0 on success
//            : 7 on failure  
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Access()
	{
	int fd ;
	TBuf8<50> string;
	TPtrC String ; 
   
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kaccess, "In Access" );
    INFO_PRINTF1( Kaccess );
    
    _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
    if(!res)  
		{
    	_LIT(Kerr , "Failed to read input file name from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    
    string.Copy(String) ;
    char *file = (char *) string.Ptr() ;
    file[string.Length()] = '\0' ;
    
    unlink(file) ;
   
    if((fd = open(file , O_CREAT | O_RDWR , 0666))  < 0) 
		{
        _LIT(Kerr, "Failed to open the given file ") ;
        ERR_PRINTF1(Kerr);
        return KErrGeneral ;
        }
    close(fd) ;
 	if(access(file ,R_OK) != 0 ) 
 		{
		_LIT(Kopen , "Failed to open file " ) ;
		INFO_PRINTF1(Kopen) ;
		return KErrGeneral  ;
		}
 	return KErrNone ;
    }


//-------------------------------------------------------------------------------

//  Test case : access tests  invalid args
//            : returns 0 on success
//            : 7 on failure  
//--------------------------------------------------------------------------------                  



TInt CTestSyscalls :: Access1()
	{
	int fd ;
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
    _LIT( Kaccess, "In Access" );
    INFO_PRINTF1( Kaccess );
    TPtrC String ;
    TBuf8<50> string;
    TInt ret   ; //To store all the return values 
   
    ret = KErrNone ;
    
    
    _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
    if(!res)  
		{
    	_LIT(Kerr , "Failed to read i/p file name from ini file ") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    string.Copy(String) ;
    char *file = (char *) string.Ptr() ;
    file[string.Length()] = '\0' ;
    
    unlink(file) ;
	if(access(file ,W_OK) < 0) 
		{
       if(errno == ENOENT) 
    		{
    		_LIT(Kaccess ,"Access test(W_OK) for a file created in Read only mode pass") ;
    		INFO_PRINTF1(Kaccess) ;
    	   
    		}
    	else 
    		{
			_LIT(Kaccess , "Access test failed to set errno value ") ;
			ERR_PRINTF1(Kaccess);
			ret = KErrGeneral ;
    		}
		}
    else 
		{
    	_LIT(Kaccess , "access R_OK for a non existing file failed");
    	INFO_PRINTF1(Kaccess) ;
    	ret = KErrGeneral ;
    	
		}
    
    
    if((fd = open(file , O_CREAT | O_WRONLY , 0444))  < 0) 
		{
		_LIT(Kopen , "Failed to open file " ) ;
		INFO_PRINTF1(Kopen) ;
		return KErrGeneral ; ;
		}
    close(fd) ;
    
    if(access(file , W_OK) < 0) 
		{
    	if(errno == EACCES) 
    		{
    		_LIT(Kaccess ,"Access test(W_OK) for a file created in Read only mode pass") ;
    		INFO_PRINTF1(Kaccess) ;
    	  
    		}
    	else  
    		{
    		ret = KErrGeneral ;
    		}
		}
    else 
		{
    	ret = KErrGeneral ;
		}
    chmod(file , S_IWUSR);
    return ret ;	
    
	}

//-------------------------------------------------------------------------------

//  Test case : dup tests for duplicating the file descriptor
//            : returns 0 on success
//            : 8 on failure  
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Dup()
	{
	int fd ;
	int newfd ;
	TPtrC String ;
	TBuf8<50> string;
	TInt Expected  ;
   
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kdup, "In Dup" );
	INFO_PRINTF1( Kdup);
    
    //Read the input file name from ini file 
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res)  
		{
    	_LIT(Kerr , "Failed to read i/p file name from ini file ") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
	string.Copy(String) ;
	char *file = (char *) string.Ptr() ;
	file[string.Length()] = '\0' ;
	unlink(file) ;
      
    //Now read expected values 
	_LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
    if(!res) 
		{
    	_LIT(Kerr, "Failed to read expected values from ini") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    
    if(Expected < 0) 
		{
    	newfd = dup(-1) ;
    	if(errno != EBADF) 
    		{
    		_LIT(Kerr , "Failed to set errno value") ;
    		INFO_PRINTF1(Kerr) ;
    		return KErrGeneral ;
    		}
    	if(newfd != Expected) 
    		{
    		_LIT(Kerr ,"Expected and the return value dont match") ;
    		INFO_PRINTF1(Kerr) ;
    		return KErrGeneral ;
    		}
    	return KErrNone ;
		}
  
	if((fd = open(file, O_CREAT | O_RDWR , 0666))  < 0) 
		{
		_LIT(Kopen , "Failed to open file " ) ;
		INFO_PRINTF1(Kopen) ;
        return KErrGeneral  ;
		}
    
    if((newfd = dup(fd)) < 0 )
		{
		close(fd) ;
		 unlink(file) ;
    	return KErrGeneral ;
		}
    
    close(fd) ;
    close(newfd) ;
    unlink(file) ;
    return KErrNone ;
    
	}





//-------------------------------------------------------------------------------

//  Test case : dup tests for duplicating the file descriptor in given slot
//            : returns 0 on success
//            : 8 on failure  
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Dup2()
	{
	int fd ;
	TInt newfd ;
	TInt Expected ;
	TInt retval ;
	TPtrC String ;
	TBuf8<50> string;
  
   
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kdup2, "In Dup2" );
	INFO_PRINTF1( Kdup2);
    
   //Read the input file name from ini file 
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res)  
		{
    	_LIT(Kerr , "Failed to read i/p file name from ini file ") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
	string.Copy(String) ;
	char *file = (char *) string.Ptr() ;
	file[string.Length()] = '\0' ;
	unlink(file) ;
   
   
	//Now read new fd's to be copied  values 
    _LIT( Knewfd, "newfd" );
	res = GetIntFromConfig(ConfigSection(), Knewfd, newfd);

    if(!res) 
		{
    	_LIT(Kerr, "Failed to read New fd  value from ini") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    if(newfd > 1000) 
    	{
    	_LIT(Kerr , "Upper bound for fd not supported \n") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrNone ;
    	}
    
    //Now read expected values 
    _LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
    if(!res) 
		{
    	_LIT(Kerr, "Failed to read expected values from ini") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
      
    
  
	if((fd = open(file , O_CREAT | O_RDWR | O_TRUNC , 0666))  < 0) 
		{
        _LIT(Kopen , "Failed  to open file ") ;
        INFO_PRINTF1(Kopen) ;
        return KErrGeneral ;
		}
    
	retval = dup2(fd , newfd) ;
    
    if(retval == Expected ) 
		{
    	if(Expected < 0 )  
    		{
    		if(errno != EBADF) 
    			{
    			_LIT(Kerr , "Failed to set errno value of dup2 call") ;
    			INFO_PRINTF1(Kerr) ;
    			return KErrGeneral ;
    			}
			close(fd) ;
			close(newfd) ;
			return KErrNone ;
    		}
    	else   
    		{
    	    TInt Nbytes ;
    		//Write some thing in the new fd and find size  using old fd 
    		if((Nbytes = write(fd , "Hello " , 6)) < 0 ) 
    			{
    		 	_LIT(Kerr , "Failed to write  through old fd ") ;
    		 	INFO_PRINTF1(Kerr) ;
    			}
    	    struct stat buf  ;
    	    if(fstat(newfd , &buf ) < 0 )  
    			{
    	     	_LIT(Kerr, "Failed to stat the file through new fd") ;
    	     	INFO_PRINTF1(Kerr) ;
    	     	return KErrGeneral ;
    			}
    	    if(Nbytes != buf.st_size) 
    			{
    	        _LIT(Kerr , "Failed to dupilcate the file des ") ;
    	        INFO_PRINTF1(Kerr) ;
    	        return KErrGeneral ;
    			}
    	    close(fd) ;
    	    close(newfd) ;
    	    return KErrNone ;
    		}
		}
    
    _LIT(Kerr , "Returned and the expected values are different ") ;
    INFO_PRINTF1(Kerr) ;
    close(fd) ;
    close(newfd) ;
    return KErrGeneral ;
    
	}



//-------------------------------------------------------------------------------

//  Test case : rename tests for renaming the existing file
//            : returns 0 on success
//            : 9 on failure  
//--------------------------------------------------------------------------------                  



TInt CTestSyscalls :: Rename()
	{
	int fd ;
	TPtrC String ;
	TBuf8 <50> string ;
	TInt Expected ;
	char File1[50] ; 
	char File2[50] ;
   
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Krename, "In Rename" );
	INFO_PRINTF1( Krename);
   
    
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res) 
		{
   		_LIT(Kerr , "failed to read input file name ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	string.Copy(String) ; 
	char *file = (char *) string.Ptr() ;
	file[string.Length()] = '\0' ;
   
	strcpy(File1 , file) ;
	TPtrC theString ;
	_LIT( KtheString, "theString" );
	res = GetStringFromConfig(ConfigSection(), KtheString, theString );
	if(!res) 
		{
   		_LIT(Kerr , "failed to read input file1 name ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	string.Copy(theString) ; 
	file = (char *) string.Ptr() ;
	file[string.Length()] = '\0' ;
   
	strcpy(File2 , file);
   
    //Now read expected value
    _LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
    if(!res) 
		{
    	_LIT(Kerr , "Failed to read expected value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    
	unlink(File1) ;unlink(File2);
    
    if(Expected < 0) 
		{
		TInt ret ; 
		ret =  rename(File1 , File2) ;
       	INFO_PRINTF2(_L("errno is %d"), errno);
		if((errno != EINVAL) && (errno != ENOENT) && (errno != EACCES))  
			{
			_LIT(Kerr , "Failed to set errno value") ;
       		INFO_PRINTF1(Kerr) ;
       		return KErrGeneral ;
			}
		if(ret == Expected)
			{
       		return KErrNone ;
			}
       
		_LIT(Kerr , "Expected and the returned values are not matching") ;
		ERR_PRINTF1(Kerr);
		return KErrGeneral ;
		}
   
    if((fd = open(File1 , O_CREAT | O_RDWR , 0666))  < 0) 
		{
		_LIT(Kopen , "Failed  to create file ") ;
        INFO_PRINTF1(Kopen) ;
        return KErrGeneral ;
		}
	close(fd) ;
    if(rename(File1 , File2) < 0)
		{
		_LIT(Krename , "Failed  to rename  file ") ;
        INFO_PRINTF1(Krename) ;

        return KErrGeneral ;
		}
    
  
    return KErrNone ;
    
	}


//-----------------------------------------------------------------------------
// This function test rename call for existing files and for read-only files

//------------------------------------------------------------------------------

#define READ_ONLY_FILE  0
#define NORMAL_FILE		1 
TInt CTestSyscalls :: Rename1() 
	{
	int fd ;
	TPtrC String ;
	TBuf8 <50> string ;
	TInt Command ;
	char File1[50] ; 
	char File2[50] ;
   
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Krename, "In Rename1" );
	INFO_PRINTF1( Krename);
   
    
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );

	if(!res) 
		{
   		_LIT(Kerr , "failed to read input file name ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	string.Copy(String) ; 
	char *file = (char *) string.Ptr() ;
	file[string.Length()] = '\0' ;
   
	strcpy(File1 , file) ;
	TPtrC theString;
	_LIT( KtheString, "theString" );
	res = GetStringFromConfig(ConfigSection(), KtheString, theString );
	if(!res) 
		{
   		_LIT(Kerr , "failed to read input file1 name ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	string.Copy(theString) ; 
	file = (char *) string.Ptr() ;
	file[string.Length()] = '\0' ;
   
	strcpy(File2 , file);
   
	_LIT( KCommand, "Command" );
	res = GetIntFromConfig(ConfigSection(), KCommand, Command);
 
	if(!res) 
		{
    	_LIT(Kerr , "Failed to read expected value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    
	fd = open(File1 , O_CREAT | O_RDWR , 0666) ;
   		   	
   	if(fd < 0 ) 
   		{
   		_LIT(Kerr , "Failed to open given file1\n") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   	close(fd) ;	   
    
	switch(Command) 
   		{
   		case NORMAL_FILE :
   		   	{
   		   	int fd = open(File2 , O_CREAT | O_RDWR , 0666) ;
   		   	
   		   	if(fd < 0 ) 
   		   		{
   		   		_LIT(Kerr , "Failed to open given file2\n") ;
   		   		INFO_PRINTF1(Kerr) ;
   		   		return KErrGeneral ;
   		   		}
   		   		
   		   	close(fd) ;
   		   	
   		   	if(rename(File1 , File2) < 0 ) 
   		   		{
   		   		_LIT(Kerr , "Rename of existing R/W file failed \n") ;
   		   		INFO_PRINTF1(Kerr) ;
   		   		return KErrGeneral ;
   		   		}
   		   	break ;
   		   	}
   		   	
   		case READ_ONLY_FILE :
   			{
   			int fd =  open(File2 , O_CREAT | O_RDWR , 0444) ;
   			
   			if(fd < 0) 
   				{
   				_LIT(Kerr , "Failed to open given file2 \n") ;
   				INFO_PRINTF1(Kerr) ;
   				return KErrGeneral ;
   				}
   			close(fd) ;
   			
   			if(rename(File1 , File2) < 0 ) 
   				{
   				_LIT(Kerr , "Failed to Rename an Existing Readonly file\n") ;
   				INFO_PRINTF1(Kerr) ;
   				return KErrGeneral ;
   				}
   				
   		    break ;	
   			}
		}	
   	return KErrNone ;
	}



//-------------------------------------------------------------------------------

//  Test case : chmod tests 
//            : 
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Chmod()
	{
	int fd ;
	TInt Expected ;
	TBuf8<50> string ;
	TPtrC  String ;
   
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kchmod, "In Chmod" );
	INFO_PRINTF1( Kchmod);
   
	//Read input file name 
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res)  
		{
   		_LIT(Kerr , "failed to read input file name ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	string.Copy(String) ;
   
	char *file = (char *) string.Ptr() ;
	file[string.Length()] = '\0' ;
   
	unlink(file) ;
   
	//Now read expected value 
	_LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
    if(!res) 
		{
    	_LIT(Kerr , "Failed to read expected value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    if(Expected < 0 ) 
		{
   
		TInt ret1 , ret2  ;
       
		ret1 = chmod(file , S_IWUSR) ;
     
		if(ret1 == Expected) 
			{
			if(errno != ENOENT) 
				{
      			_LIT(Kerr, "Failed to set errno to ENOENT") ;
      			INFO_PRINTF1(Kerr) ;
      			return KErrGeneral ;
				}
			}
    
        if((fd = open(file , O_CREAT | O_RDWR , 0444) ) < 0) 
			{
			_LIT(Kopen , "Failed  to create file ") ;
			INFO_PRINTF1(Kopen) ;
			return KErrGeneral ;
			}
        close(fd) ;
     
		//chmod and write some thing 
        if(chmod(file , S_IWUSR) < 0)
			{
			_LIT(Kerr , "Failed to change the modes of the file ") ;
			INFO_PRINTF1(Kerr) ;
			return KErrGeneral ;
			}
    
    
		//file  now is this file
		// in read write mode 
        ret2 = open(file ,O_RDWR) ;
    
        if(ret2 >= 0) 
			{
			_LIT(Kok , "Open successfull \n") ;
      		INFO_PRINTF1(Kok);
      		close(ret2) ;
      		ret2 = -1 ;
      	    }
        unlink(file) ;
        if((ret1 == Expected) &&(ret2 == Expected)) 
			{
			return KErrNone ;
			}
    
		_LIT(Kerr , "Returned and expected values are different") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		} //End of if(Expected < 0)
    
	if(( fd = open(file , O_CREAT | O_RDWR , 0444 ))< 0 )  
		{	
   		_LIT(Kerr , "Failed to open the file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	close(fd) ;   
   
	if(chmod(file  , S_IWUSR)  <0 )
		{
   		_LIT(Kerr , "Failed to change file mode") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	unlink(file) ;
	return KErrNone; 
	}


//-------------------------------------------------------------------------------

//  Test case : fchmod tests 
//            : 
//--------------------------------------------------------------------------------                  



TInt CTestSyscalls :: FChmod()
{
   
   int fd ;
   TInt Expected ;
   TBuf8<50> string ;
   TPtrC  String ;
   INFO_PRINTF1(_L("FChmod Test")) ;
   _LIT( KTsyscalls, "Tsyscalls" );
   INFO_PRINTF1(KTsyscalls) ;
   _LIT( Kchmod, "In FChmod" );
   INFO_PRINTF1( Kchmod);
   
   //Read input file name 
   _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
 
   if(!res)  
   {
   	 _LIT(Kerr , "failed to read input file name ") ;
   	 INFO_PRINTF1(Kerr) ;
   	 return KErrGeneral ;
   }
   string.Copy(String) ;
   
   char *file = (char *) string.Ptr() ;
   file[string.Length()] = '\0' ;
   
   unlink(file) ;
   
   //Now read expected value 
   _LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
 
    if(!res) 
    {
    	_LIT(Kerr , "Failed to read expected value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
    }
    
 
   if(Expected < 0 ) 
   {
     	fd = open(file , O_CREAT | O_RDWR  , 0666) ;
   
   		if(fd < 0 ) 
   		{
   		_LIT(Kerr , "Failed to open the given file name for fchmod \n") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   
       TInt ret1 , ret2  ;
       
       ret1 = fchmod(-1 , 0666) ;
     
       if(ret1 == Expected) 
       {
          if(errno != EBADF) 
          {
      	    _LIT(Kerr, "Failed to set errno to EBADF") ;
      	    INFO_PRINTF1(Kerr) ;
			close(fd);
      	    return KErrGeneral ;
          }
     
        }
    
          
     //chmod and write some thing 
        if(fchmod(fd , 0444 ) < 0)
        {
           _LIT(Kerr , "Failed to change the modes of the file ") ;
           INFO_PRINTF1(Kerr) ;
		   close(fd);
           return KErrGeneral ;
        }
        close(fd) ;
    
    //file  now is read only and hence it cannot be opened
    // in read write mode 
        ret2 = open(file ,O_RDWR) ;
    
        if(ret2 < 0) 
        {
           if(errno == EACCES) 
           	{
           	_LIT (Kerr , "Read-only file and hence cannot be opened in r/w mode\n") ;
            INFO_PRINTF1(Kerr);
           	}
           	else 
           	{
           	 _LIT(Kerr , "open failed to set error value ") ;
           	 INFO_PRINTF1(Kerr) ;
           	 return KErrGeneral ;	
           	}
      	    // return KErrNone ;
        }
        
        //Now try to do a chmod on closed fd 
        
        if(fchmod(fd , 0666) < 0 ) 
        	{
        	 if(errno != EBADF) 
        	 	{
        	 	_LIT(Kerr , " fchmod failed to set errno value , for closed fd") ;
        	 	INFO_PRINTF1(Kerr) ;
				close(ret2);
        	 	return KErrGeneral ;
        	 	}
        	}     
         unlink(file) ;
    
    
    	if((ret1 == Expected) &&(ret2 == Expected)) 
    	{
		close(ret2);
      	return KErrNone ;
    	}
    
    	_LIT(Kerr , "Returned and expected values are different") ;
		close(ret2);
   	 	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
   	} //End of if(Expected < 0)
    
    
   if(( fd = open(file , O_CREAT | O_RDWR , 0666 ))< 0 )  
   {
   	  _LIT(Kerr , "Failed to open the file ") ;
   	  INFO_PRINTF1(Kerr) ;
   	  return KErrGeneral ;
   }
   //close(fd) ;
 
   
   if(fchmod(fd  , 0444)  <0 )
   {
   	 _LIT(Kerr , "Failed to change file mode") ;
   	 INFO_PRINTF1(Kerr) ;
	 close(fd);
   	 return KErrGeneral ;
   }
   close(fd) ;
   unlink(file) ;
   return KErrNone; 
}


//-------------------------------------------------------------------------------

//  Test case : chmod of a directory
//            : returns 0 on success
//            :  
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Chmod_dir()
	{
	TBuf8<50> string ;
	TPtrC  String ;
   
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kchmod, "In Chmod_dir" );
	INFO_PRINTF1( Kchmod);
   
	//Read input file name 
	_LIT( KString, "String");
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
    if(!res)  
		{
   		_LIT(Kerr , "failed to read input directory  name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	string.Copy(String) ;
	char *dir = (char *) string.Ptr() ;
	dir[string.Length()] = '\0' ;
	rmdir(dir) ;                         //unlink that directory 
   
	//create the directory and then change the mode of that directory
     
	if(mkdir(dir , S_IWUSR) < 0 )   
		{
		_LIT(Kerr , "create the directory") ;
		ERR_PRINTF1(Kerr);
		return KErrGeneral ;
		}   
	if(chmod(dir , S_IWUSR) < 0)
		{
		_LIT(Kerr , "Failed to change the modes of the directory") ;
		INFO_PRINTF1(Kerr) ;
		rmdir(dir);
		return KErrGeneral ;
		}
    
    //now change the working directory to the above given directory
    
    if(chdir(dir) < 0 )   
		{
        if(errno != EACCES)
			{
			_LIT(Kerr , "Failed to set the ernno value for the chmod") ;
			INFO_PRINTF1(Kerr) ;
			rmdir(dir);
			return KErrGeneral ;
			}
        return KErrNone ;
		}
    
    //Control comes here only for failur case 
    _LIT(Kerr , "Chdir test fails ") ;
	rmdir(dir);
    INFO_PRINTF1(Kerr) ;
    return KErrNone ;
	}

//-------------------------------------------------------------------------------

//  Test case : fchmod of a directory
//            : returns 0 on success
//            :  
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: FChmod_dir()
{
   /*
   TBuf8<50> string ;
   TPtrC  String ;
   
   _LIT( KTsyscalls, "Tsyscalls" );
   INFO_PRINTF1(KTsyscalls) ;
   _LIT( Kchmod, "In Chmod_dir" );
   INFO_PRINTF1( Kchmod);
   
   //Read input file name 
  _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
  
   if(!res)  
   {
   	 _LIT(Kerr , "failed to read input directory  name from ini file ") ;
   	 INFO_PRINTF1(Kerr) ;
   	 return KErrGeneral ;
   }
   
   string.Copy(String) ;
   char *dir = (char *) string.Ptr() ;
   dir[string.Length()] = '\0' ;
   rmdir(dir) ;                         //unlink that directory 
   
   //create the directory and then change the mode of that directory
     
   if(mkdir(dir , 0666) < 0 )   
   {
      _LIT(Kerr , "create the directory") ;
      return KErrGeneral ;
   }   
   
   int fd = open(dir , O_RDONLY ) ;
   
        
    if(fd < 0 ) 
    {
     _LIT(Kerr , "Failed to a given directory ") ;
     INFO_PRINTF1(Kerr) ;
     return KErrGeneral ;	
    }
    
    if(fchmod(fd , 0444) < 0)
    {
     _LIT(Kerr , "Failed to change the modes of the directory") ;
     INFO_PRINTF1(Kerr) ;
     return KErrGeneral ;
      
     }
    
    close(fd) ;
    //now change the working directory to the above given directory
    
    if(chdir(dir) < 0 )   
    {
        if(errno != EACCES)
        {
           _LIT(Kerr , "Failed to set the ernno value for the chmod") ;
           INFO_PRINTF1(Kerr) ;
           return KErrGeneral ;
        }
        return KErrNone ;
    }
    
    //Control comes here only for failur case 
    _LIT(Kerr , "Chdir test fails  No support for permissions for directory ") ;
    INFO_PRINTF1(Kerr) ;*/
    return KErrNone ;
   

}

//-------------------------------------------------------------------------------

//  Test case : exit tests for renaming the existing file
//            : returns 0 on success
//            : 11 on failure  
//
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Exit()
	{
	return KErrNone ;
    }



//-------------------------------------------------------------------------------

//  Test case : close tests 
//            : returns 0 on success
//            : 11 on failure  
//
//--------------------------------------------------------------------------------                  

TInt CTestSyscalls :: Close()
	{
	int fd ;
	TInt Expected ;
	TPtrC String ;
	TBuf8<50> string ;
   
   
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kclose, "In close" );
	INFO_PRINTF1( Kclose);
   
   
	//read the file name from ini file 
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
 
    if(!res) 
		{
    	_LIT(Kerr, "Failed to read file name from ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    
    string.Copy(String) ;
    
    char *file = (char *) string.Ptr() ;
    file[string.Length()] = '\0' ;
    
    unlink(file) ;
    //Now read expected value 
    _LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);

    if(!res) 
		{
    	_LIT(Kerr , "Failed to read expected value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    
    if(Expected < 0)  
		{
		TInt ret1 , ret2 ;
		TInt Err1 , Err2 ; //store the errvalues
    	 //give an invalid fd 
		ret1 = close(-1) ;
		Err1 = errno ;
		ret2 = close(123) ;
		Err2 = errno ;
       
		if((ret1 == Expected) &&(ret2 == Expected))
			{
       		if((Err1 == EBADF) && (Err2 == EBADF) ) 
       			{
       	 		return KErrNone ;
       			}
       		_LIT(Kerr , "Failed to set errno") ;
       		INFO_PRINTF1(Kerr) ;
       		return KErrGeneral ;
			}
       
		else   
			{
       		_LIT(Kerr , "Expected and the return values are different ") ;
       		INFO_PRINTF1(Kerr) ;
       		return KErrGeneral ;
			}
       }
     
	if((fd = open(file , O_CREAT | O_RDWR , 0666))  < 0) 
		{
		_LIT(Kopen , "Failed  to create file ") ;
        INFO_PRINTF1(Kopen) ;
        return KErrGeneral ;
		}
     
    if(close(fd) < 0) 
		{
		_LIT(Kerr , "Failed to close the opened file") ;
		INFO_PRINTF1(Kerr) ;
		unlink(file) ;
		return KErrGeneral  ;	
		}
    
    unlink(file) ;
    return KErrNone ;
    
	}



//-------------------------------------------------------------------------------

//  Test case : mkdir  makes a directory with given name 
//            : returns 0 on success
//            : 13 on failure  
//
//--------------------------------------------------------------------------------                  

TInt CTestSyscalls :: Mkdir()
	{
    TBuf8<50> string ;
    TPtrC  String ;
    TInt Expected , ret ;     //To store expected and return values
    
    _LIT( KTsyscalls, "Tsyscalls" );
    INFO_PRINTF1(KTsyscalls) ;
    _LIT( Kmkdir, "In Mkdir" );
    INFO_PRINTF1( Kmkdir);
    
    
    //First read input directory name
    _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );

    if(!res) 
		{
		_LIT(Kerr , "Failed to read input dir name from ini file") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral;
		}
    string.Copy(String) ;
    
    char *DirName = (char *)string.Ptr() ;
    
    DirName[string.Length()] = '\0' ;
     
    //Read Expected value  
    _LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
 
    if(!res) 
		{
    	_LIT(Kerr , "Failed to read expected value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    
    rmdir(DirName) ;
     
    ret = mkdir(DirName , S_IWUSR) ;
    
    if(ret == Expected ) 
		{
        if(ret < 0 ) 
			{
    		if((errno != ENOENT) && (errno != EEXIST)) 
    			{
    			_LIT(Kerr , "Failed to set errno") ;
    			ERR_PRINTF1(Kerr);
         		ret=  KErrGeneral ;
    			}
    		else
				{
                ret = KErrNone ; 
				}
			}
		}
    else 
		{
    	 _LIT(Kerr , "Expected and the return values are different ") ;
    	 ERR_PRINTF1(Kerr) ;
    	 ret = KErrNone ; 
		}
    return ret;
    
	}
   


//-------------------------------------------------------------------------------

//  Test case : rmdir  removes the given direcorty 
//            : returns 0 on success
//            : -1 on failure  
//
//--------------------------------------------------------------------------------                  

TInt CTestSyscalls :: Rmdir()
	{
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Krmdir, "In Rmdir" );
	INFO_PRINTF1( Krmdir);
   
	TInt Expected  ;
	TPtrC String ;
	TBuf8<50>string ;
	TInt ret1 , ret2 ;

	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res) 
		{
   		_LIT(Kerr , "Failed to read input dir name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	string.Copy(String) ;
   
	char *DirName = (char *) string.Ptr() ;
	DirName[string.Length()] = '\0' ;
   
	_LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
	if(!res) 
		{
   		_LIT(Kerr , "Failed to read input expected value ini file") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	if(Expected < 0) 
		{
   		ret1 = rmdir(DirName) ;
   		if(ret1 == Expected ) 
   			{   
   	        if((errno != ENOENT)  && (errno != ENOTEMPTY))
   				{
   	        	_LIT(Kerr , "Failed to set errno value ") ;
   	        	INFO_PRINTF1(Kerr) ;
   	        	return KErrGeneral ;
   				}
			}
   	    else 
   			{
   			_LIT(Kerr , "Expected and actual values are different ") ;
   			INFO_PRINTF1(Kerr) ;
   	    	return KErrGeneral ;
   			}
   	    
   	    //Now create  a directory , create some files 
   	    // in the directory  and then try to delete it  
   	    
   	    if(mkdir(DirName ,S_IWUSR)  < 0) 
			{
			if(errno != EEXIST) 
				{
				_LIT(Kerr , "Failed to make dir entry") ;
    			INFO_PRINTF1(Kerr) ;
    			return KErrGeneral ;
				}
			}
		//Creaty an entry in the directory
		char FullPath[50] ;
		strcpy(FullPath , DirName) ;
		strcat(FullPath , "\\TEST_DIR.txt") ;
		TInt fd ;
       
		if((fd = open(FullPath , O_CREAT | O_RDWR , 0666 ))< 0) 
			{
   			_LIT(Kerr , "Failed to create entries in the newly created dir") ;
   			INFO_PRINTF1(Kerr) ;
   			return KErrGeneral ;
			}
       
		close (fd) ;
       
		ret2 = rmdir(DirName) ;
       
		if(ret2 == Expected) 
			{
			TInt Ret = KErrNone ;
			if(errno != ENOTEMPTY)   
				{	
         		_LIT(Kerr , "Failed to set errno value for rmdir") ;
         		INFO_PRINTF1(Kerr) ;
         		Ret = KErrGeneral ;
				}
			unlink(FullPath) ;
			rmdir(DirName) ;
			return Ret ;
			}
		_LIT(Kerr , "Expected and returned values are different") ;
		INFO_PRINTF1(Kerr) ;
		return  KErrGeneral ;
		}          //End of if(Expected < 0)
	if(mkdir(DirName ,S_IWUSR)  < 0) 
		{
        _LIT(Kerr , "Failed to make dir entry") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    
    ret2 = rmdir(DirName) ;
    
    if(ret2 != Expected) 
		{
    	_LIT(KErr , "Return and expected values are different") ;
    	INFO_PRINTF1(KErr) ;
    	return KErrGeneral;
    	}
    return KErrNone ;
        
	}


//For test of \.

TInt CTestSyscalls :: Rmdir1()
	{
	if(mkdir("C:\\Test\\TestDir" ,S_IWUSR)  < 0) 
		{
    	_LIT(Kerr , "Failed to make dir entry") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    
    if(rmdir("C:\\Test\\TestDir\\." )  < 0) 
		{
		TInt Err = errno ;
        if(errno != EINVAL) 
			{
			_LIT(Kerr , "Failed to make dir entry") ;
    		INFO_PRINTF1(Kerr) ;
    		return KErrGeneral ;
			}
		return KErrGeneral ;
		}
     return KErrNone ;
	}

//-------------------------------------------------------------------------------

//  Test case : Chdir_Rmdir  removes the given direcorty 
//            : returns 0 on success
//            : 
//
//--------------------------------------------------------------------------------                  

TInt CTestSyscalls :: Rmdir_Chdir()
	{
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Krmdir, "In Rmdir_Chdir" );
	INFO_PRINTF1( Krmdir);
   
	// TInt Expected  ;
	TPtrC String ;
	TBuf8<50>string ;
  
	//TInt exists = 0 ;
 
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res) 
		{
   		_LIT(Kerr , "Failed to read input dir name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	string.Copy(String) ;
   
	char *DirName = (char *) string.Ptr() ;
	DirName[string.Length()] = '\0' ;
   //Now create  a directory , create some files 
   // in the directory  and then try to delete it  
	rmdir(DirName) ;
   	    
	if(mkdir(DirName ,S_IWUSR)  < 0) 
		{
		if(errno != EEXIST) 
			{
			_LIT(Kerr , "Failed to make dir entry") ;
			INFO_PRINTF1(Kerr) ;
			return KErrGeneral ;
			}
		}
       
	//Now chage the current working directory to the new one 
	// and then try to  remove that directory
       
    if(chdir(DirName) < 0 )   
		{
		_LIT(Kerr , "Failed to change the current working directory to the given") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
      
    //Now try to delete the current working directory.
       
    if(rmdir(DirName) > 0 )   
		{
       	_LIT(Kerr , "Rmdir should fail as the some process is using this as currnet working directory") ;
       	INFO_PRINTF1(Kerr) ;
       	return KErrGeneral ;
		}
    return KErrNone ;
        
	}
//-------------------------------------------------------------------------------

//  Test case : fsync  tests 
//            : returns 0 on success
//            : -1 on failure  
//
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Fsync()
	{
       
	int fd ;
	TBuf8<50> string ;
	TPtrC String ;
	TInt Expected ;
	TInt ret ;
 
	_LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
    if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	string.Copy(String) ;
   
	char *file = (char *) string.Ptr() ;
	file[string.Length()] = '\0' ;
   
	//Now read expected value
 
	_LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
    if(!res) 
		{ 
   		_LIT(Kerr , "Failed to read Expected value from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;  
		}
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kfsync, "In Fsync" );
	INFO_PRINTF1(Kfsync);
   
	if(Expected < 0) 
		{    
   		ret = fsync(-1) ;
   		if(ret != 0)
   			{
	   		if(errno != EBADF)   
	   			{
	   	   		_LIT(Kerr , "Failed to set errno value ") ;
	   	   		INFO_PRINTF1(Kerr) ;
	   	   		return KErrGeneral ;
	   			}
			}
		else 
			{
	   		_LIT(Kerr , "fsync failed") ;
   	   		INFO_PRINTF1(Kerr) ;
   	   		return KErrGeneral;
   			}
   	   return KErrNone ;
	}
  
	if((fd = open(file , O_CREAT | O_RDWR , 0666))  < 0) 
		{
		_LIT(Kopen , "Failed  to create file ") ;
        INFO_PRINTF1(Kopen) ;
        return KErrGeneral ;
		}
     
	if(fsync(fd) < 0) 
		{
   		_LIT(Kerr , "Fsync failed ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	return KErrNone 	;
    }

//-------------------------------------------------------------------------------

//  Test case : UTIMES  tests 
//            : returns 0 on success
//            : 16 on failure  
//
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Utimes()
	{
    
    TBuf8<50> string ;
    TPtrC String ;
 
    TInt  timeVal  = 0;
   
    TInt fd ;
    _LIT( KTsyscalls, "Tsyscalls" );
    INFO_PRINTF1(KTsyscalls) ;
    _LIT( Kutimes, "In Utimes" );
    INFO_PRINTF1(Kutimes);
   
	struct stat  buf ;
	buf.st_atime=0;
	struct timeval tim[2] ;
   
    fd = open("C:\\Test\\123_123.txt" , O_CREAT | O_RDWR , 0666) ;
   
	if(fd < 0) 
   		{
   		_LIT(Kerr , "failed to open the file") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
	tim[0].tv_sec = 0 ; tim[0].tv_usec = 0;
	tim[1].tv_sec = 0 ; tim[1].tv_usec = 0;

	close(fd) ;
	//Read the  input file name
	_LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	string.Copy(String) ;
	
    //Read the time 
	_LIT( KtimeVal, "timeVal" );
	res = GetIntFromConfig(ConfigSection(), KtimeVal, timeVal);
 
	if(!res) 
		{ 
		_LIT(Kerr , "Failed to read time value from ini file ") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;  
		}
	buf.st_atime += timeVal  ;
	if(utimes("C:\\Test\\123_123.txt"  , tim) < 0 ) 
		{
     
		_LIT(Kerr, "Utime failed") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	return KErrNone ;
	}
    
 
 
  
  //-------------------------------------------------------------------------------

//  Test case : UTIMES  tests 
//            : returns 0 on success
//            : 16 on failure  
//
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Utime()
	{
    
    TInt Expected ;
    TInt ret ;
    TPtrC String ;
    TBuf8<50> string ;
    TInt timeVal ;
    
	struct utimbuf *tim = NULL ;
    char *rfile ;
    int fd = 0;
  
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kutime, "In Utime" );
    INFO_PRINTF1(Kutime);



	//Read the  input file name
	_LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
    if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	string.Copy(String) ;
	rfile = (char *) string.Ptr() ;
    //Read the time 

	_LIT( KtimeVal, "timeVal" );
	res = GetIntFromConfig(ConfigSection(), KtimeVal, timeVal);
	if(!res) 
		{ 
		_LIT(Kerr , "Failed to read time value from ini file ") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;  
		}
    
	rfile[string.Length()] = '\0' ;
   
	//Now read expected value 
	_LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
	if(!res) 
		{ 
   		_LIT(Kerr , "Failed to read Expected value from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;  
		}
	unlink(rfile) ;

 
    tim =(struct utimbuf *) calloc(1 , sizeof(struct timeval)) ; 
    
    //Negative tests 
    if(Expected < 0 ) 
		{
      	tim->actime = (time_t) timeVal ;
      	tim->modtime = (time_t) timeVal ;
      	
      	ret = utime(rfile , tim) ;          //Try to do utime on non existent file
      	
      	if(ret == Expected ) 
      		{
			if(errno != ENOENT)   
      			{
				_LIT(Kerr ,"Failed to set errno value  ") ;
      			INFO_PRINTF1(Kerr) ;
      			close(fd) ;
      			free(tim);
      			return KErrGeneral ;
      			}
			free(tim);      			
      	    return KErrNone ;
      		}
      	
		} //End of if(Expected < 0)
    
    struct stat buf1;
     
    if((fd = open(rfile , O_CREAT | O_RDWR , 0666))  < 0) 
		{
		_LIT(Kopen , "Failed  to create file ") ;
        INFO_PRINTF1(Kopen) ;
        free(tim);
        return KErrGeneral ;
		}
     
    
    if(fstat(fd , &buf1) <0 )  
		{
    	_LIT(Kerr , "Failed to stat the given file ") ;
    	INFO_PRINTF1(Kerr) ;
    	free(tim);
    	return KErrGeneral ;
		}
    
    buf1.st_atime += timeVal ;
    tim->actime = buf1.st_atime ; 
    tim->modtime = buf1.st_atime  ; 
	// ret = utime(rfile , tim) ;
    close(fd) ;
    
    if(utime(rfile , tim)  < 0) 
		{
		_LIT(Kerr , "Failed to set the access and modification time stamps of the given file ") ;
		INFO_PRINTF1(Kerr) ;
		free(tim);
		return KErrGeneral ; 
		}
	free(tim);
    return KErrNone ;
	}
    
  
//-------------------------------------------------------------------------------

//  Test case : CHDIR  tests 
//            : returns 0 on success
//            : 17(CHDIR_FAIL) on failure  
//
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Chdir()
	{

	TBuf8<50> string ;
	TPtrC String ;
	TInt Expected ;
	TInt ret ;
   
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kchdir, "In Chdir" );
	INFO_PRINTF1(Kchdir);
   
    //Read dir to be changed
    _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res) 
		{
   		_LIT(Kerr, "Failed to read input dir name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	string.Copy(String) ;
   
	char *dir = (char *) string.Ptr() ;
   
	dir[string.Length()] = '\0' ;
     
	//Now read expected value 
   	_LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
    if(!res) 
		{ 
   		_LIT(Kerr , "Failed to read Expected value from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;  
		}
	ret = chdir(dir) ;
  
	if(ret  == Expected)   
		{
		if(ret < 0 ) 
			{
			if(errno != ENOENT) 
   				{
   	  			_LIT(Kerr , "Failed to set errno value ") ;
   	  			INFO_PRINTF1(Kerr) ;
   	  			return KErrGeneral ;
   				}
			}
   		return KErrNone ;
		}
 	_LIT(Kerr, "Expected and return value are different") ;
 	INFO_PRINTF1(Kerr);
	return KErrGeneral ;
    }



//-------------------------------------------------------------------------------

//  Test case : FCNTL tests 
//            : returns 0 on success
//            : KErrGeneral on failure  
//
//--------------------------------------------------------------------------------                  

#define F_BADCMD  999

TInt CTestSyscalls :: Fcntl()
	{

	int fd ;
	TBuf8<50> string ;
	TPtrC String ;
	TInt command ;
  
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kfcntl, "In Fcntl" );
	INFO_PRINTF1(Kfcntl);
	//Read file name 
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
 
	if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	string.Copy(String) ;
   
	char *file = (char *) string.Ptr() ;
   
	file[string.Length()] = '\0' ;
   
    //Read input command 
    _LIT( Kcommand, "command" );
    res = GetIntFromConfig(ConfigSection(), Kcommand, command);
    if(!res)   
		{
   		_LIT(Kerr , "Failed to read the input command ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	remove(file);
	if((fd = open(file , O_CREAT | O_RDWR | O_TRUNC  , 0666))  < 0) 
		{
		_LIT(Kopen , "Failed  to create file ") ;
		INFO_PRINTF1(Kopen) ;
		return KErrGeneral  ;
		}
   
	switch(command)   
		{
   		case F_GETFL :
   			{
   			TInt flags ;
   			flags = fcntl(fd , F_GETFL ) ;
   			if(flags != O_RDWR )   
  				{
  				_LIT(Kerr , "Flag values returned is not proper ") ;
   				INFO_PRINTF1(Kerr) ;
   				return KErrGeneral ;
  				}
   			else  
   				{
   	    		_LIT(Kerr , "Flags returned are correct") ;
   	    		INFO_PRINTF1(Kerr) ;
   	    		return KErrNone ;
   				}
			}              //End of F_GETFL 
   	      
   		case  F_SETFL :
   			{
    		//First read the flag value to be set 
   	   		TInt flag ;
   	   		_LIT( Kflag, "flag" );
			res = GetIntFromConfig(ConfigSection(), Kflag, flag);
   	   		if(!res)  
   	   			{
   	   			_LIT(Kerr , "Failed to read input flag value to be set ") ;
   	   			INFO_PRINTF1(Kerr) ;
   	   			return KErrGeneral ;
   	   			}
   	   		if(fcntl(fd , F_SETFL , O_APPEND) < 0 ) 
   	   			{
   	   			_LIT(Kerr , "Failed to set the flags of the opened file \n") ;
   	   	  		INFO_PRINTF1(Kerr) ;
   	   	  		return KErrGeneral ;
   	   			}
   	   		if((flag = fcntl(fd , F_GETFL)) != (O_RDWR | O_APPEND)) 
   	   			{
   	   			_LIT(Kerr , "fcntl for F_SETFL Failed") ;
   	   			INFO_PRINTF1(Kerr) ;
   	   			return  KErrGeneral ;	
   	   			}
   	   	   }              //End of F_SETFL 
   	   
   	   
   		case   F_DUPFD :           //Duplicate file descriptor 
   			{
   	   		TInt Newfd ;
   	   		Newfd = fcntl(fd , F_DUPFD ,fd -1) ;
   	   		if(Newfd < (fd - 1 )) 
   	   			{
   	  			_LIT(Kerr , "failed to duplicate file descriptor") ;
   	   			INFO_PRINTF1(Kerr) ;
   	   			return KErrGeneral ;
   	   			}
   	   		close(Newfd); 
   	   		Newfd = fcntl(fd ,  F_DUPFD , fd )  ;
   	   		close (Newfd); 
   			if(Newfd < fd ) 
   	   			{
   	  			_LIT(Kerr , "failed to duplicate file descriptor") ;
   	   			INFO_PRINTF1(Kerr) ;
   	   			return KErrGeneral ;
   	   			}
   	   		close(Newfd);
   	   		Newfd = fcntl(fd ,  F_DUPFD , fd+1 )  ;
   	   		//Write something in using new file descriptor
   	   		if(Newfd < (fd +1 )) 
   	   			{
   	  			_LIT(Kerr , "failed to duplicate file descriptor") ;
   	   			INFO_PRINTF1(Kerr) ;
   	   			return KErrGeneral ;
   	   			}
   	   		int nbytes ;
   	   		if((nbytes  = write(Newfd , "Hello" , 6)) < 0 )
   	   			{
   	   	 		_LIT(Kerr , "dup value returned is improper " ) ;
   	   	 		INFO_PRINTF1(Kerr) ;
   	   	 		return KErrGeneral ;
   	   			}
			//Now find out the size of the file through old fd 
   	  	
   	  		struct stat buf ;
   	 
   	  		if(fstat(fd , &buf) < 0 ) 
   	  			{
   				_LIT(Kerr , "Failed to stat the file ") ;
   	  			INFO_PRINTF1(Kerr) ;
   	  			return KErrGeneral ;
   	  			}
   	  	
   	  		if(buf.st_size != nbytes) 
   	  			{
   	  			_LIT(Kerr , "File des are not duplicated properly") ;
   	  			INFO_PRINTF1(Kerr);
   	  			return KErrGeneral ;
   	  			}
   	  		close(fd) ; 
   	  		close(Newfd);
   	   		break ;
   			}             //End of F_DUPFL 

	case  F_GETFD :              //get close on exec flags 
   		{
   	  	TInt cflag ;
   	  	
   	  	cflag = fcntl(fd , F_GETFD ) ;
   	  	
   	  	if(cflag < 0 ) 
   	  		{
   	  		_LIT(Kerr , "failed to get close on exec flags") ;
   	  		INFO_PRINTF1(Kerr) ;
   	  		return KErrGeneral ;
   	  		}

   	  	break  ;
   		}             //End of F_GETFD
   	
	case F_SETFD :                 //Set close on exec flags 
		{
    	//First read the flag value to be set 
   	   	TInt flag ;
   	   	_LIT( Kflag, "flag" );
		res = GetIntFromConfig(ConfigSection(), Kflag, flag);
 
   	   	if(!res)  
   	   		{
   	   		_LIT(Kerr , "Failed to read input flag value to be set ") ;
   	   		INFO_PRINTF1(Kerr) ;
   	   		return KErrGeneral ;
   	   		}
   	   	if(fcntl(fd , F_SETFD , flag) < 0 ) 
   	   		{
   	   	    _LIT(Kerr , "Failed to set the flags of the opened file \n") ;
   	   	  	INFO_PRINTF1(Kerr) ;
   	   	  	return KErrGeneral ;
   	   		}
   	   	break ;
   		}	           //End of F_SETFD 
   	  
   	case F_BADCMD :
   	 	{
   	 	if(fcntl(fd , F_BADCMD , 1) < 0 ) 
   	 		{
   	 		if(errno != EINVAL) 
   	 			{
   	 			_LIT(Kerr , "Fcntl failed to set errno value for bad command") ;
   	 			INFO_PRINTF1(Kerr) ;
   	 			return KErrGeneral ;	
   	 			}
   	 		_LIT(Kok , "Fcntl test passes for bad command");
   	 		INFO_PRINTF1(Kok) ;
   	 		return KErrNone ;
   	 		}
   	 	else 
   	 		{
   	 		 _LIT(Kerr , "Fcntl should fail for bad command but passes") ;
   	 		INFO_PRINTF1(Kerr) ;
   	 		return KErrGeneral ;	
   	 		}
		}  //End of F_BADCMD :
   	  case  F_SETLK :
   	  	{
   	  	  TInt flag,start,len,whence;
    	//First read the flag value to be set 
   	   	 _LIT( Kflag, "flag" );
    	 res = GetIntFromConfig(ConfigSection(), Kflag, flag);
    	 if(!res)   
			{
   			_LIT(Kerr , "Failed to read the flag ") ;
   			INFO_PRINTF1(Kerr) ;
   			return KErrGeneral ;
			}
		_LIT( Kstart, "start" );
    	 res = GetIntFromConfig(ConfigSection(), Kstart, start);
    	 if(!res)   
			{
   			_LIT(Kerr , "Failed to read start ") ;
   			INFO_PRINTF1(Kerr) ;
   			return KErrGeneral ;
			}
		_LIT( Klen, "len" );
    	 res = GetIntFromConfig(ConfigSection(), Klen, len);
    	 if(!res)   
			{
   			_LIT(Kerr , "Failed to read the len ") ;
   			INFO_PRINTF1(Kerr) ;
   			return KErrGeneral ;
			}
   	   
		_LIT( Kwhence, "whence" );
    	 res = GetIntFromConfig(ConfigSection(), Kwhence, whence);
    	 if(!res)   
			{
   			_LIT(Kerr , "Failed to read the whence ") ;
   			INFO_PRINTF1(Kerr) ;
   			return KErrGeneral ;
			}
	     
	     struct flock fl;
   	   	 fl.l_type = F_WRLCK;
    	 fl.l_whence = whence;
    	 fl.l_start = start;
    	 fl.l_len = len;
         for(int i=0;i<=15;i++)
			{
			if(write(fd , "hello" ,6) < 0) 
				{
				_LIT(Kerr , "Failed to write to the file ") ;
				INFO_PRINTF1(Kerr) ;
				_LIT(Kerr1 , "errno%d") ;
				INFO_PRINTF2(Kerr1,errno) ;
				close(fd) ;
				return KErrGeneral ;
				}
			}
	     
	    
   	   	 if(flag==1||flag==2||flag==3||flag==4||flag==11)
   	   	 	{
   	   	 		if(fcntl(fd , F_SETLK , &fl)== -1 ) 
   	   	 		{	
   	   	  		_LIT(Kerr , "Failed to set the lock on the opened file \n") ;
   	   	  		INFO_PRINTF1(Kerr);
   	   	  		return KErrGeneral ;
  	   	 		}
   	   	 		if (errno == EACCES || errno == EAGAIN)
  	   	 		{
  	   	 		_LIT(Kerr , "Failed to set the lock on the opened file \n") ;
   	   	  		INFO_PRINTF1(Kerr);
   	   	  		return KErrGeneral ;
  	   	 		}
   	   	 
   	   		}
   	   	 if(flag==5||flag==9||flag==12)
   	   	 	{
   	   	 	if(fcntl(fd , F_SETLK , &fl)!= -1 ) 
   	   	 		{
   	   	   		_LIT(Kerr , " set the lock on the opened file \n") ;
   	   	  		INFO_PRINTF1(Kerr);
   	   	  		return KErrGeneral ;
  	   	 		}	
   	   	 	}
   	   	if(flag==6)
   	   		{
   	   		if(fcntl(-1 , F_SETLK , &fl)!= -1 ) 
   	   	 		{
   	   	   		_LIT(Kerr , " set the lock on the opened file \n") ;
   	   	  		INFO_PRINTF1(Kerr);
   	   	  		return KErrGeneral ;
  	   	 		}	
   	   		}
   	   if(flag==7)
   	   		{
   	   		if(fcntl(fd , F_SETLK , NULL)!= -1 ) 
   	   	 		{
   	   	   		_LIT(Kerr , " set the lock on the opened file \n") ;
   	   	  		INFO_PRINTF1(Kerr);
   	   	  		return KErrGeneral ;
  	   	 		}	
   	   		}
   	  
   	   if(flag==10)
   	   	{
   	   		fl.l_type = F_RDLCK;
    	    if(fcntl(fd , F_SETLK , &fl)!= -1 ) 
   	   	 		{
   	   	   		return KErrGeneral ;
  	   	 		}
   	   	}
   	   
  	   	 break ;
  	  }              //End of F_SETLK 
   	   
	case F_GETLK :
   	  {
   	    
   	      TInt flag,start,len,whence;
    	//First read the flag value to be set 
   	   	 _LIT( Kflag, "flag" );
    	 res = GetIntFromConfig(ConfigSection(), Kflag, flag);
    	 if(!res)   
			{
   			_LIT(Kerr , "Failed to read the flag ") ;
   			INFO_PRINTF1(Kerr) ;
   			return KErrGeneral ;
			}
		_LIT( Kstart, "start" );
    	 res = GetIntFromConfig(ConfigSection(), Kstart, start);
    	 if(!res)   
			{
   			_LIT(Kerr , "Failed to read the start ") ;
   			INFO_PRINTF1(Kerr) ;
   			return KErrGeneral ;
			}
		_LIT( Klen, "len" );
    	 res = GetIntFromConfig(ConfigSection(), Klen, len);
    	 if(!res)   
			{
   			_LIT(Kerr , "Failed to read the len ") ;
   			INFO_PRINTF1(Kerr) ;
   			return KErrGeneral ;
			}
   	   
		_LIT( Kwhence, "whence" );
    	 res = GetIntFromConfig(ConfigSection(), Kwhence, whence);
    	 if(!res)   
			{
   			_LIT(Kerr , "Failed to read the whence") ;
   			INFO_PRINTF1(Kerr) ;
   			return KErrGeneral ;
			}
	     
	     for(int i=0;i<=15;i++)
			{
			if(write(fd , "hello" ,6) < 0) 
				{
				_LIT(Kerr , "Failed to write to the file ") ;
				INFO_PRINTF1(Kerr) ;
				_LIT(Kerr1 , "errno%d") ;
				INFO_PRINTF2(Kerr1,errno) ;
				close(fd) ;
				return KErrGeneral ;
				}
			}
	     
   	  	 struct flock fl;
   	   	 fl.l_type = F_WRLCK;
    	 fl.l_whence = whence;
    	 fl.l_start = start;
    	 fl.l_len = len;
    	 if(flag==8)
    	 {
    	 if(fcntl(fd , F_GETLK , &fl)== -1 ) 
   	   	 		{
   	   	   		_LIT(Kerr , "Failed to get the lock on the opened file \n") ;
   	   	    	INFO_PRINTF1(Kerr);
   	   	  		return KErrGeneral ;
  	   	 		}
  	   	 fl.l_type = F_WRLCK;
    	 fl.l_whence = whence;
    	 fl.l_start = start;
    	 fl.l_len = len;
  	   	 if(fcntl(fd , F_GETLK , &fl)!= -1 ) 
   	   	 		{
   	   	   		_LIT(Kerr , "Failed to get the lock on the opened file \n") ;
   	   	    	INFO_PRINTF1(Kerr);
   	   	  		return KErrGeneral ;
  	   	 		}	
    	 }
  	   	 if(flag==1||flag==3||flag==4||flag==5)
   	   	 	{
  	   	 	if(fcntl(fd , F_GETLK , &fl)== -1 ) 
   	   	 		{
   	   	   		_LIT(Kerr , "Failed to get the lock on the opened file \n") ;
   	   	    	INFO_PRINTF1(Kerr);
   	   	  		return KErrGeneral ;
  	   	 		}
  	   	 	if (errno == EACCES || errno == EAGAIN)
  	   	 		{
  	   	 		_LIT(Kerr , "Failed to get the lock on the opened file \n") ;
   	   	    	INFO_PRINTF1(Kerr);
   	   	  		return KErrGeneral ;
  	   	 		}
  	   	 	if( fl.l_type != F_UNLCK)
  	   	 		{
  	   	 		return KErrGeneral;
  	   	 		}
  	   	 	}
  	   	 
  	   	if(flag==6||flag==2)
  	   		{
  	   		if(fcntl(fd , F_GETLK , &fl)!= -1 ) 
   	   	 		{
   	   	   		_LIT(Kerr , "Failed to get the lock on the opened file \n") ;
   	   	    	INFO_PRINTF1(Kerr);
   	   	  		return KErrGeneral ;
  	   	 		}
  	   		}
  	   	if(flag==7)
   	   	{
   	   		fl.l_type = F_RDLCK;
    	    if(fcntl(fd , F_GETLK , &fl)!= -1 ) 
   	   	 		{
   	   	   		return KErrGeneral ;
  	   	 		}
   	   	}
  	   	
   	    break ;
   	  }              //End of F_GETFL 
   	 default :
   		{
   	   	_LIT(Kerr , "Invalid command ") ;
   	   	INFO_PRINTF1(Kerr) ;
   	   	return KErrGeneral ;
		}            //End of default 
   	}  
	return KErrNone ;
	}


//-------------------------------------------------------------------------------

//  Test case : ioctl tests 
//            : returns 0 on success
//            : 18(KErrFcntl) on failure  
//
//--------------------------------------------------------------------------------                  

TInt CTestSyscalls :: Ioctl()
	{
	TBuf8<50> string ;
	TPtrC String ;
	TInt command ;
  
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kfcntl, "In Fcntl" );
	INFO_PRINTF1(Kfcntl);
   
   
	//Read file name
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	string.Copy(String) ;
    char *file = (char *) string.Ptr() ;
    file[string.Length()] = '\0' ;
   
    //Read input command 
    _LIT( Kcommand, "command" );
    res = GetIntFromConfig(ConfigSection(), Kcommand, command);
	if(!res)   
		{
   		_LIT(Kerr , "Failed to read the input command ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	if((open(file , O_CREAT | O_RDWR | O_TRUNC  , 0666))  < 0) 
		{
		_LIT(Kopen , "Failed  to create file ") ;
		INFO_PRINTF1(Kopen) ;
		return KErrGeneral  ;
		}
   
	switch(command)   
		{
   		default :
   			{
   	   		_LIT(Kerr , "Invalid command ") ;
   	   		INFO_PRINTF1(Kerr) ;
   	   		return KErrNone ;
   			}            //End of default 
   		}
	}

#define SIZE       0  
#define LINKS      1
#define ATIME      2
#define MTIME      3
#define CTIME      4


TInt CTestSyscalls :: Fstat() 
	{
	int fd ;
	TBuf8<50> string ;
	TPtrC String ;
	TInt Expected ;
	TInt command  ;
	TInt ret =KErrNone;
   
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kfstat, "In Fstat" );
	INFO_PRINTF1(Kfstat);
      //Read dir to be changed

	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	string.Copy(String) ;
   
	char *file = (char *) string.Ptr() ;
   
	file[string.Length()] = '\0' ;
   
     //Now read expected value 
    _LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
 
	if(!res) 
		{ 
   		_LIT(Kerr , "Failed to read Expected value from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;  
		}
	//Now read command
	_LIT( Kcommand, "command" );
	res = GetIntFromConfig(ConfigSection(), Kcommand, command);
	if(!res) 
		{ 
   		_LIT(Kerr , "Failed to read command value from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;  
		}
   
	if(Expected < 0 ) 
		{
		struct stat statbuf ;
		fd = -1 ;    
   		ret = fstat(fd , &statbuf) ;
   		if(ret == Expected) 
   			{
   	 		if(errno !=EBADF) 
   	 			{
   	 			_LIT(Kerr , "Failed to set errno value") ;
   	 			INFO_PRINTF1(Kerr) ;
   	 			return KErrGeneral ;
   	 			}
   	 		return KErrNone ;
   			}
   		else 
   			{
   	 		_LIT(Kerr , "Return and expected values are different") ;
   	 		INFO_PRINTF1(Kerr) ;
   	 		return KErrGeneral ;
   			}
   		}
   
	unlink(file) ;
   
	if((fd = open(file , O_CREAT | O_RDWR | O_TRUNC , 0666)) < 0) 
		{
   		_LIT(Kerr , "Failed to open specified file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
     //Write  some  bytes to the file 
   
	if(write(fd , "Hello Hello" , 12 ) < 12) 
		{
   		_LIT(Kerr , "failed to write to the newly opened file ") ;
   		ERR_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	struct stat Buf ;
	Buf.st_atime  = -1 ; //Need to do some modifications here 
	Buf.st_mtime  = -1 ;
	Buf.st_ctime  = -1 ;   
  
	if(fstat(fd , &Buf) < 0) 
		{
   		_LIT(Kerr , "Failed to fstat the given file") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	switch(command)   
		{
    	case SIZE :
    		if(12 == Buf.st_size)
    			{
    	    	ret = 0 ;
    			}
    	    else 
    			{
    	    	ret = -1 ;
    			}
    	     break ;
    	     
    	case ATIME :
    	     if(Buf.st_atime  > 0) 
    			{
    	     	ret = 0 ;
    			}
    	     else 
    			{
    	     	ret = -1 ;
    	     	}
    		break ;
    	     
    	case MTIME :
    	    if(Buf.st_mtime  > 0) 
    			{
    	     	ret = 0 ;
    			}
    	     else 
    			{
    	     	ret = -1 ;
    	     	}
    	     break ;
    	     
    	case CTIME :   //This will be same as atime. DEF124164 
			if(Buf.st_ctime  > 0)  
    			{
    	     	ret = 0 ;
    			}
    	     else 
    			{
    	     	ret = -1 ;
    	     	}
    	     break ;   
		}  
	close (fd) ;
    unlink(file);           
   
	if(ret != Expected ) 
		{
   		_LIT(Kerr , "Expected and return values dont match") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	return KErrNone ;    
}

#define NORMAL_FILE_NAME "c:\\Test\\stattestnormal.txt"
#define HIDDEN_SYS_FILE_NAME "c:\\Test\\stattesthidden.txt"
#define HIDDEN_FILE 0
#define SYSTEM_FILE 1

//---------------------------------------------------------------------------
//Stat Test cases 
//---------------------------------------------------------------------------
TInt CTestSyscalls :: Stat1()
{
    struct stat normalStat = {0};
    struct stat hiddenSysStat = {0};
	TInt   Expected ;
	TInt   command  ;
	TBool  res ;
    RFs    rfs;
    
    TInt   err = rfs.Connect();
        
    _LIT(KNormalFileName, "c:\\Test\\stattestnormal.txt");
    _LIT(KHiddenSysFileName, "c:\\Test\\stattesthidden.txt");
    
    // Create files for testing
    if (err == KErrNone)
    {
        RFile normalFile;
        RFile hiddenFile;
        err = normalFile.Create(rfs, KNormalFileName, EFileWrite);
        if(KErrNone != err)
        	{
            rfs.Delete(KNormalFileName);
            err = normalFile.Create(rfs, KNormalFileName, EFileWrite);             
        	}
        
        if (err == KErrNone)
        {
            normalFile.Close();
            err = hiddenFile.Create(rfs, KHiddenSysFileName, EFileWrite);
            if(KErrNone != err)
            	{
            	rfs.Delete(KHiddenSysFileName);
            	err = hiddenFile.Create(rfs, KHiddenSysFileName, EFileWrite);
            	}      
            if (err == KErrNone)
            {
                hiddenFile.Close();
            
                //Now read expected value
            	_LIT( KExpected, "Expected" );
            	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
            	if(!res) 
            	{	 
          			_LIT(Kerr , "Failed to read Expected value from ini file ") ;
          			INFO_PRINTF1(Kerr) ;
          			return KErrGeneral ;  
            	}
                
            	//Now read command
            	_LIT( Kcommand, "command" );
            	res = GetIntFromConfig(ConfigSection(), Kcommand, command);
            	if(!res) 
            	{ 
            		_LIT(Kerr , "Failed to read command value from ini file ") ;
            	   	INFO_PRINTF1(Kerr) ;
            	   	return KErrGeneral ;  
            	}
            	switch(command)
            		{
            		case HIDDEN_FILE:
            			rfs.SetAtt(KHiddenSysFileName, KEntryAttHidden, 0);
            			break;
            		case SYSTEM_FILE:
            			rfs.SetAtt(KHiddenSysFileName, KEntryAttSystem, 0);
            			break;
            		}      		
            }
        }
    }
    else
    {
    	_LIT(Kerr , "Failed to Connect to FileServer ") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
    }

    // Do the stat for files
    if (err == KErrNone)
    {
        stat(NORMAL_FILE_NAME, &normalStat);
        stat(HIDDEN_SYS_FILE_NAME, &hiddenSysStat);
       
        // Stat should be equivalent for both, since hidden attribute should not be relevant for statting,
        // and especially it shouldn't mean that the file is a symbolic link.
        if (normalStat.st_mode == hiddenSysStat.st_mode)
        {
            _LIT(Kerr , "st_modes are equivalent!") ;
            INFO_PRINTF1(Kerr) ;
        } 
        else 
        {
            _LIT(Kerr , "st_modes are different!") ;
        	INFO_PRINTF1(Kerr) ;
            err = KErrGeneral; 
        }
        
        if ((hiddenSysStat.st_mode & S_IFLNK) == S_IFLNK) 
        {
            _LIT(Kerr , "Hidden file considered a symbolic link!!") ;
            INFO_PRINTF1(Kerr) ;
            err = KErrGeneral;
        }
    }

    //rfs.Delete(KNormalFileName);
    //rfs.Delete(KHiddenSysFileName);
    rfs.Close();
   
    return err;
}


//---------------------------------------------------------------------------
//Stat Test cases 
//---------------------------------------------------------------------------


TInt CTestSyscalls :: Stat() 
	{
	int fd ;
	TBuf8<50> string ;
	TPtrC String ;
	TInt Expected ;
	TInt command  ;
	TInt ret=KErrNone ;
   
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kfstat, "In stat" );
	INFO_PRINTF1(Kfstat);
  
	//Read dir to be changed
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	string.Copy(String) ;
   
	char *file = (char *) string.Ptr() ;
   
	file[string.Length()] = '\0' ;
   
     //Now read expected value
	_LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
    if(!res) 
		{ 
   		_LIT(Kerr , "Failed to read Expected value from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;  
		}
   //Now read command
	_LIT( Kcommand, "command" );
	res = GetIntFromConfig(ConfigSection(), Kcommand, command);
	if(!res) 
		{ 
   		_LIT(Kerr , "Failed to read command value from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;  
		}
	if(Expected < 0 ) 
		{
		struct stat statbuf ;
		unlink(file) ;
   		ret = stat(file , &statbuf) ;
   	 
   		if(ret == Expected) 
   			{
   	 		if(errno !=ENOENT) 
   	 			{
   	 			_LIT(Kerr , "Failed to set errno value") ;
   	 			INFO_PRINTF1(Kerr) ;
   	 			return KErrGeneral ;
   	 			}
   	 		return KErrNone ;
   			}
   		else 
   			{
   	 		_LIT(Kerr , "Return and expected values are different") ;
   	 		INFO_PRINTF1(Kerr) ;
   	 		return KErrGeneral ;
   			}
   	
		}
   
	unlink(file) ;
   
	if((fd = open(file , O_CREAT | O_RDWR | O_TRUNC , 0666)) < 0) 
		{
   		_LIT(Kerr , "Failed to open specified file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
   //Write  some  bytes to the file 
   
	if(write(fd , "Hello Hello" , 12 ) < 12) 
		{
   		_LIT(Kerr , "failed to write to the newly opened file ") ;
   		ERR_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
	close(fd);
   
	struct stat Buf ;
	Buf.st_atime  = -1 ;
	Buf.st_atime  = -1 ; //Need to do some modifications here 
	Buf.st_mtime  = -1 ;
	Buf.st_ctime  = -1 ;   
  
	if(stat(file , &Buf) < 0) 
		{
   		_LIT(Kerr , "Failed to fstat the given file") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   
    switch(command)   
		{
    	case SIZE :
    	    if(12 == Buf.st_size)
    			{
    	    	ret = 0 ;
    			}
    	    else 
    			{
    	    	ret = -1 ;
    			}
    	    break ;
    	     
    	case ATIME :
    	     if(Buf.st_atime  > 0) 
    			{
    	     	ret = 0 ;
    			}
    	     else 
    			{
    	     	ret = -1 ;
				}
    	     break ;
    	     
    	case MTIME :
    	    if(Buf.st_mtime  > 0) 
    			{
    	     	ret = 0 ;
    			}
    	     else 
    			{
    	     	ret = -1 ;
    			}
    	     break ;
    	     
    	case CTIME :    //This will be same as atime. DEF124164
             if(Buf.st_ctime > 0) 
    			{
    	     	ret = 0 ;
    			}
    	     else 
    			{
    	     	ret = -1 ;
    	     	}
    	     break ;   
		}  
    close (fd) ;
    unlink(file);           
   
	if(ret != Expected ) 
		{
   		_LIT(Kerr , "Expected and return values dont match") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	return KErrNone ;    

	}


//---------------------------------------------------------------------------
//Stat Test cases 
//---------------------------------------------------------------------------
TInt CTestSyscalls :: Stat2() 
	{
	TInt ret = KErrNone;
    
	_LIT(Kerr , "stat should not succeed for nonexistent drives!") ;
	INFO_PRINTF1(Kerr) ;
    
    RFs   rfs;
    ret = rfs.Connect();
    if(KErrNone == ret)
    	{
        TDriveList aList;
	    TUint16 driveNum = 0;	    
	    char buffer[4];
	    char buffer2[20];
        struct stat st;

	    rfs.DriveList(aList);
	    
	    for(char c='A'; c <='Z'; c++, driveNum++, ret = KErrNone)
	    	{
	        sprintf(buffer, "%c:/", c);
	        sprintf(buffer2, "%c:/private/", c);
	        
	        ret = stat(buffer, &st);
	        if((0 != aList[driveNum]) && (KErrNone != ret))
	        	{
		    		ERR_PRINTF2(_L("stat Failed for an existing drive : %c "),c);
		    		return KErrGeneral ;	        		
	        	}
	        ret = stat(buffer2, &st);       
	    	}
    	}
    else
    	{
    	_LIT(Kerr , "Failed to Connect to FileServer ") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
    	}
	return KErrNone;
	}


//-------------------------------------------------------------------------
// getpid test : 
//-------------------------------------------------------------------------


TInt CTestSyscalls :: Getpid() 
	{
	pid_t pid ;
	
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kgetpid, "In Getpid" );
	INFO_PRINTF1(Kgetpid);
      
	pid = getpid() ;
	
	if(pid < 0 ) 
		{
		_LIT(Kerr , "getpid call failed \n") ;
		INFO_PRINTF1(Kerr);
		return KErrGeneral ;
		}
	return KErrNone ;
	}

//-------------------------------------------------------------------------
// Time  test : 
//-------------------------------------------------------------------------

TInt CTestSyscalls :: Time() 
	{
   
	time_t tim;
	char Buff[50] ;
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Ktime , "In Time" );
	INFO_PRINTF1(Ktime);
   
    if(time(&tim) < 0 ) 
		{
		_LIT(Kerr , "time  call failed \n") ;
		INFO_PRINTF1(Kerr);
		return KErrGeneral ;
		}
	char* ptr = ctime(&tim);
	strcpy(Buff , ctime(&tim)) ;  //For Debug testing 
	
	return KErrNone ;
	
	}


//-------------------------------------------------------------------------
// Clock  test : 
//-------------------------------------------------------------------------

TInt CTestSyscalls :: Clock() 
	{
   
	clock_t tim;
	char Buff[50] ;
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Ktime , "In Clock" );
	INFO_PRINTF1(Ktime);
    
    tim = clock() ;
    
    if(tim == (clock_t)-1 ) 
		{
		_LIT(Kerr , "clock  call failed \n") ;
		INFO_PRINTF1(Kerr);
		return KErrGeneral ;
		}
	strcpy(Buff , ctime((const int *)&tim)) ;  //For Debug testing 
	
	return KErrNone ;
	
	}


//-------------------------------------------------------------------------
// Waitpid  test : 
//-------------------------------------------------------------------------

TInt CTestSyscalls :: WaitPid() 
	{
	pid_t pid ;
	TInt status ;
    
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kwaitpid , "WaitPid" );
	INFO_PRINTF1(Kwaitpid);
    
    pid = getpid() ;
    
    if(pid < 0 )  
		{
    	_LIT(Kerr , "Failed to  get pid ") ;
    	INFO_PRINTF1(Kerr);
    	return KErrGeneral ;
		}
    
    if(waitpid(pid , &status , 0) < 0 )    
		{
    	if(errno != ECHILD)  
    		{
    		_LIT(Kerr , " failed to set errno value for waitpid") ;
            INFO_PRINTF1(Kerr) ;
            return KErrGeneral ;  
    		}
    	return KErrNone ;
		}
    
	return KErrNone ;
	}

TInt CTestSyscalls :: Sleep()
	{
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Ksleep , "Sleep" );
	INFO_PRINTF1(Ksleep);
	TInt Stime ;     //sleep time
	_LIT( KStime, "Stime" );
	TBool res = GetIntFromConfig(ConfigSection(), KStime, Stime);
 
	if(!res) 
		{ 
   		_LIT(Kerr , "Failed to read Sleep time from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;  
		}
	sleep(Stime);
	return KErrNone ;
	}

/*
//------------------------------------------------------------------
//Waitpid test : wait on a dead child 
//-------------------------------------------------------------------
TInt CTestSyscalls :: WaitPid1() 
{
   
   pid_t pid ;
   TInt status ;
    
   _LIT( KTsyscalls, "Tsyscalls" );
   INFO_PRINTF1(KTsyscalls) ;
   _LIT( Kwaitpid , "WaitPid" );
   INFO_PRINTF1(Kwaitpid);
    
    pid = getpid() ;
    
    if(pid < 0 )  
    {
    	_LIT(Kerr , "Failed to  get pid ") ;
    	INFO_PRINTF1(Kerr);
    	return KErrGeneral ;
    }
    
    if(waitpid(pid , &status , 0) < 0 )    
    {
    	if(errno != ECHILD)  
    	{
    		_LIT(Kerr , " failed to set errno value for waitpid") ;
             INFO_PRINTF1(Kerr) ;
             return KErrGeneral ;  
    	}
    	return KErrNone ;
    }
    
	return KErrNone ;
}

*/

/*
*This function test writev for directory file discrptor
*/

TInt CTestSyscalls :: Writev_Dir_Test(char *file)  
	{
	char Buf1[20] ;
	char Buf2[20] ;
	struct iovec Iovec[2] ;
   
	Iovec[0].iov_base = Buf1 ;
	Iovec[0].iov_len = 20 ;
	Iovec[1].iov_base = Buf2 ;
	Iovec[1].iov_len = 20 ;
   
	int fd =open(file , O_RDONLY) ;
   
	if(fd < 0 ) 
   		{
   		_LIT(Kerr , "Failed to open the given directory for readv test") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   	
	if(writev(fd , Iovec , 2) < 0 ) 
		{
   	   
   		if(errno != EBADF) 
   			{
   			_LIT(Kerr , "failed to set errno value for writev for directory file discriport") ;
   			INFO_PRINTF1(Kerr) ; 
   	  		return KErrGeneral ;
   	   		}
   		return KErrNone  ;
   		}
   	else 
   		{
   		_LIT(Kerr , "Write on a directory file discriptor should fail ");
   		INFO_PRINTF1(Kerr) ;
		}
   	return KErrGeneral  ;
	}

/**
*This function test writev on an invalid file discriptor
**/

TInt CTestSyscalls :: Writev_Invalid_Fd(void)
	{
	char Buf1[20] ;
	char Buf2[20] ;
	struct iovec Iovec[2] ;
   
	Iovec[0].iov_base = Buf1 ;
	Iovec[0].iov_len = 20 ;
	Iovec[1].iov_base = Buf2 ;
	Iovec[1].iov_len = 20 ;
   
	
	if(writev(-1 , Iovec , 2) < 0 ) 
		{
   		if(errno == EBADF) 
   			{
   	  		_LIT(Kok , "writev from invalid discrptor passed") ;
   	  		INFO_PRINTF1(Kok) ;
   	  		return KErrNone ;	 
   			}
   		_LIT(Kerr , "writev failed to set errno value to EBADF") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	_LIT(Kerr , "writev from an invalid discriptor should fail but found to pass"  ) ;
    INFO_PRINTF1(Kerr) ;
    return KErrGeneral ;	
	}

/**
* This function is called by Writev() and tests writev on an 
* read-only file discriptor 
**/

TInt CTestSyscalls :: Writev_Readfd_Test(char *file ) 
	{
	char Buf1[20] ;
	char Buf2[20] ;
	struct iovec Iovec[2] ;
	int fd = 0 ;
   
	Iovec[0].iov_base = Buf1 ;
	Iovec[0].iov_len = 20 ;
	Iovec[1].iov_base = Buf2 ;
	Iovec[1].iov_len = 20 ;
   
	unlink(file) ;
   
	fd = open(file , O_WRONLY | O_CREAT , 0666) ;
   
	if(fd < 0 ) 
		{
   		_LIT(Kerr , "Failed to open the given file readv test") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		} 
   	
	if(readv(fd , Iovec , 2) < 0 ) 
   		{
   		if(errno != EACCES )
   			{
   			_LIT(Kerr , "failed to read to different I/O Vectors for directory file descriptor") ;
   			INFO_PRINTF1(Kerr) ;
			return KErrGeneral ;
   			}
		}
   	return KErrNone ;
	}


//-------------------------------------------------------------------------------------------------
// This function test for writev  with iov vector length as zero
// and is called by 
//-------------------------------------------------------------------------------------------------
TInt CTestSyscalls :: Writev_Invalidcount_Test(char *file , TInt Firstarg)
	{
	char Buf1[20] ;
	char Buf2[20] ;
	struct iovec Iovec[2] ;
	int fd = 0 ;
   
	if(Firstarg)
		{
		Iovec[0].iov_len =  0 ;	
		Iovec[1].iov_len = 20 ;
		}
	else 
		{
		Iovec[0].iov_len =  20 ;	
		Iovec[1].iov_len =  0 ;
		}
   
	Iovec[0].iov_base = Buf1;
	Iovec[1].iov_base = Buf2 ;
   
	unlink(file) ;
	
	fd = open(file , O_RDWR | O_CREAT , 0666) ;
   
	if(fd < 0 ) 
		{
   		_LIT(Kerr , "Failed to open the given file writev test") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   	
	if(writev(fd , Iovec , 2) < 0 ) 
   		{
   		if(errno != EINVAL)
   			{
   			_LIT(Kerr , "failed to write from different I/O Vectors for directory file descriptor") ;
   			INFO_PRINTF1(Kerr) ;
			close(fd);
			return KErrGeneral ;
   			}
   	    }
   	close(fd);
   	return KErrNone ;
	}
  
/**
* This function is called by writev and is used to test writev for
* invalid io- vector addresses
**/

TInt CTestSyscalls :: Writev_AddInvalid_Test(char *file ,TInt Firstarg) 
	{
	char Buf1[20] = "HELLO";
	char Buf2[20] = "World" ;
	struct iovec Iovec[2] ;
	int fd = 0 ;
   
	if(Firstarg)
		{
		Iovec[0].iov_base =  NULL ;	
		Iovec[1].iov_base = Buf2 ;
		}
	else 
		{
		Iovec[0].iov_base =  Buf1 ;	
		Iovec[1].iov_base =  NULL ;
		}
   
	Iovec[0].iov_len = 20 ;
	Iovec[1].iov_len = 20 ;
   
	unlink(file) ;
   
	fd = open(file , O_RDWR | O_CREAT , 0666) ;
   
	if(fd < 0 ) 
		{
   		_LIT(Kerr , "Failed to open the given file readv test") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	if(writev(fd , Iovec , 2) < 0 ) 
   		{
   		if(errno != EINVAL)
   			{
   			_LIT(Kerr , "failed to write from different I/O Vectors for directory file descriptor") ;
   			INFO_PRINTF1(Kerr) ;
			return KErrGeneral ;
   			}
   	    }
	return KErrNone ;
	}

/**
* This function is called by WriteV() function and tests writev call
* for io-vectors = IOV_MAX.
**/

TInt CTestSyscalls :: Writev_MaxIov_Test(char *file)
	{
	char Buf1[20] ;
	char Buf2[20] ;
	struct iovec Iovec[2] ;
	TInt fd = 0 ;
   
	Iovec[0].iov_base = Buf1 ;
	Iovec[0].iov_len = 20 ;
	Iovec[1].iov_base = Buf2 ;
	Iovec[1].iov_len = 20 ;
   
	unlink(file) ;
   
	fd = open(file , O_RDWR | O_CREAT , 0666) ;
   
	if(fd < 0 ) 
		{
   		_LIT(Kerr , "Failed to open the given file readv test") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	
	if(writev(fd , Iovec , IOV_MAX + 1) < 0 ) 
		{
   		if(errno == EINVAL) 
   			{
   	  		_LIT(Kok , "INVALID ARGUMENT PASSED") ;
   	  		INFO_PRINTF1(Kok) ;
   	  		return KErrNone ;	 
   			}
   		_LIT(Kerr , "readv failed to set errno value to EBADF") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	_LIT(Kerr , "readv from an invalid discriptor should fail but found to pass"  ) ;
    INFO_PRINTF1(Kerr) ;
    return KErrGeneral ;
	}

/**
 *This function is called by WriteV() function and  this function test writev
 * for negative io-vector lengths
**/ 

TInt CTestSyscalls :: Writev_Negativecount_Test(char *file , TInt Firstarg)
	{
	char Buf1[20] ;
	char Buf2[20] ;
	struct iovec Iovec[2] ;
	int fd = 0 ;
   
	if(Firstarg)
		{
		Iovec[0].iov_len =  0 ;	
		Iovec[1].iov_len = size_t(-123 );
		}
	else 
		{
		Iovec[0].iov_len =  20 ;	
		Iovec[1].iov_len =  size_t(-12) ;
		}
   
	Iovec[0].iov_base = Buf1;
	Iovec[1].iov_base = Buf2 ;
   
	unlink(file) ;
   
	fd = open(file , O_RDWR | O_CREAT , 0666) ;
	
	if(fd < 0 ) 
		{
   		_LIT(Kerr , "Failed to open the given file readv test") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	if(writev(fd , Iovec , 2) < 0 ) 
   		{
   		if(errno != EINVAL)
   			{
   			_LIT(Kerr , "failed to read to different I/O Vectors for directory file descriptor") ;
   			INFO_PRINTF1(Kerr) ;
			close(fd);
			return KErrGeneral ;
   			}
   	    }
   	close(fd);
   	return KErrNone ;
   
	}

/**
* This function is called by WriteV and does functionality testing of writev 
* system call.
**/

TInt CTestSyscalls :: Writev_Normal(char *file)
	{
	char Buf1[] = "Hello World";
	char Buf2[] =  "By ritche" ; 
	struct iovec Iovec[2] ;
   
	Iovec[0].iov_base = Buf1 ;
	Iovec[0].iov_len = sizeof(Buf1)  ;
	Iovec[1].iov_base = Buf2 ;
	Iovec[1].iov_len = sizeof(Buf2) ;
   
	TInt fd = open(file , O_CREAT | O_RDWR | O_TRUNC , 0666) ;
   
	if(fd < 0) 
		{
		_LIT(Kerr , "failed to open the given file ") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;	
		}
	if(writev(fd , Iovec , 2) < (sizeof(Buf1) + sizeof(Buf2))  ) 
		{
		_LIT(Kerr , "Failed to write given no of bytes to the file specified") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;	
		}
	return KErrNone ;
   
	}

/**
* This function is called by ReadV() function and does functionality test
* of readv System call.
**/

TInt CTestSyscalls :: Readv_Normal(char *file)
	{
	char Buf1[20]  ;    //= "Hello World";
	char Buf2[20]  ;     //=  "By ritche" ; 
	struct iovec Iovec[2] ;
	TInt ret=KErrNone;
	Iovec[0].iov_base = Buf1 ;
	Iovec[0].iov_len = sizeof("Hello World")  ;
	Iovec[1].iov_base = Buf2 ;
	Iovec[1].iov_len = sizeof("By ritche") ;
   
	TInt fd = open(file , O_CREAT | O_RDWR  , 0666) ;
   
	if(fd < 0) 
		{
		_LIT(Kerr , "failed to open the given file ") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;	
		}
   
   
	if(readv(fd , Iovec , 2) < 0  ) 
		{
		_LIT(Kerr , "Failed to read from the file specified") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;	
		}
   
	if((!strcmp(Buf1 , "Hello World")) && (!strcmp(Buf2 , "By ritche"))) 
		{
		_LIT(Kok , "Readv normal test case pass") ;
		INFO_PRINTF1(Kok) ;
		ret = KErrNone ;	
		}
	else 
		{
   		_LIT(Kerr, "Failed to read from the file") ;
   		INFO_PRINTF1(Kerr) ;
   		ret = KErrGeneral ;
		}
	return ret;
   
	}

TInt CTestSyscalls :: Readv_Dir_Test(char *file)  
	{
	char Buf1[20] ;
	char Buf2[20] ;
	struct iovec Iovec[2] ;
   
	Iovec[0].iov_base = Buf1 ;
	Iovec[0].iov_len = 20 ;
	Iovec[1].iov_base = Buf2 ;
	Iovec[1].iov_len = 20 ;
   
	int fd =open(file , O_RDONLY) ;
   
	if(fd < 0 ) 
   		{
   		_LIT(Kerr , "Failed to open the given directory for readv test") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   	
	if(readv(fd , Iovec , 2) < 0 ) 
   		{
   		_LIT(Kerr , "failed to read to different I/O Vectors for directory file descriptor") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   	return KErrNone ;
	}

TInt CTestSyscalls :: Readv_Invalid_Fd(void)
	{
	char Buf1[20] ;
	char Buf2[20] ;
	struct iovec Iovec[2] ;
   
	Iovec[0].iov_base = Buf1 ;
	Iovec[0].iov_len = 20 ;
	Iovec[1].iov_base = Buf2 ;
	Iovec[1].iov_len = 20 ;
   
	
	if(readv(-1 , Iovec , 2) < 0 ) 
		{
   	
   		if(errno == EBADF) 
   			{
   	  		_LIT(Kok , "readv from invalid discrptor passed") ;
   	  		INFO_PRINTF1(Kok) ;
   	  		return KErrNone ;	 
   			}
   		_LIT(Kerr , "readv failed to set errno value to EBADF") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
    _LIT(Kerr , "readv from an invalid discriptor should fail but found to pass"  ) ;
    INFO_PRINTF1(Kerr) ;
    return KErrGeneral ;	
	}
   
TInt CTestSyscalls :: Readv_Writefd_Test(char *file ) 
	{
	char Buf1[20] ;
	char Buf2[20] ;
	struct iovec Iovec[2] ;
	int fd = 0 ;
   
	Iovec[0].iov_base = Buf1 ;
	Iovec[0].iov_len = 20 ;
	Iovec[1].iov_base = Buf2 ;
	Iovec[1].iov_len = 20 ;
   
	unlink(file) ;
   
	fd = open(file , O_WRONLY | O_CREAT , 0666) ;
   
	if(fd < 0 ) 
		{
   		_LIT(Kerr , "Failed to open the given file readv test") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	if(readv(fd , Iovec , 2) < 0 ) 
   		{
   		if(errno != EBADF )
   			{
   			_LIT(Kerr , "failed to read to different I/O Vectors for directory file descriptor") ;
   			INFO_PRINTF1(Kerr) ;
			return KErrGeneral ;
   			}
   	    }
   	return KErrNone ;
	}


TInt CTestSyscalls :: Readv_AddInvalid_Test(char *file ,TInt Firstarg) 
	{
	char Buf1[20] ;
	char Buf2[20] ;
	struct iovec Iovec[2] ;
	int fd = 0 ;
   
	if(Firstarg)
		{
		Iovec[0].iov_base =  NULL ;	
		Iovec[1].iov_base = Buf2 ;
		}
	else 
		{
		Iovec[0].iov_base =  Buf1 ;	
		Iovec[1].iov_base =  NULL ;
		}
   
	Iovec[0].iov_len = 20 ;
	Iovec[1].iov_len = 20 ;
   
	unlink(file) ;
   
	fd = open(file , O_RDWR | O_CREAT , 0666) ;
   
	if(fd < 0 ) 
		{
   		_LIT(Kerr , "Failed to open the given file readv test") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	if(readv(fd , Iovec , 2) < 0 ) 
   		{
   		if(errno != EINVAL)
   			{
   			_LIT(Kerr , "failed to read to different I/O Vectors for directory file descriptor") ;
   			INFO_PRINTF1(Kerr) ;
			return KErrGeneral ;
   			}
   	    }
   	
   	return KErrNone ;
   
	}





/**
* This function test for readv for vector length = IOV_MAX
**/


TInt CTestSyscalls :: Readv_MaxIov_Test(char *file)
	{
	char Buf1[20] ;
	char Buf2[20] ;
	struct iovec Iovec[2] ;
	TInt fd = 0 ;
   
	Iovec[0].iov_base = Buf1 ;
	Iovec[0].iov_len = 20 ;
	Iovec[1].iov_base = Buf2 ;
	Iovec[1].iov_len = 20 ;
   
	unlink(file) ;
   
	fd = open(file , O_RDWR | O_CREAT , 0666) ;
   
	if(fd < 0 ) 
		{
   		_LIT(Kerr , "Failed to open the given file readv test") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	
	if(readv(fd , Iovec , IOV_MAX + 1) < 0 ) 
		{
   		if(errno == EINVAL) 
   			{
   	  		_LIT(Kok , "INVALID ARGUMENT PASSED") ;
   	  		INFO_PRINTF1(Kok) ;
   	  		return KErrNone ;	 
   			}
   		_LIT(Kerr , "readv failed to set errno value to EBADF") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
    _LIT(Kerr , "readv from an invalid discriptor should fail but found to pass"  ) ;
    INFO_PRINTF1(Kerr) ;
    return KErrGeneral ;
  	
	}

/**
This function test for readv  with iov vector length as negative
**/
TInt CTestSyscalls :: Readv_Invalidcount_Test(char *file , TInt Firstarg)
	{
	char Buf1[20] ;
	char Buf2[20] ;
	struct iovec Iovec[2] ;
	int fd = 0 ;
   
	if(Firstarg)
		{
		Iovec[0].iov_len =  0 ;	
		Iovec[1].iov_len = 20 ;
		}
	else 
		{
		Iovec[0].iov_len =  20 ;	
		Iovec[1].iov_len =  0 ;
		}
   
	Iovec[0].iov_base = Buf1;
	Iovec[1].iov_base = Buf2 ;
   
	unlink(file) ;
   
	fd = open(file , O_RDWR | O_CREAT , 0666) ;
   
	if(fd < 0 ) 
		{
   		_LIT(Kerr , "Failed to open the given file readv test") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	if(readv(fd , Iovec , 2) < 0 ) 
   		{
   		if(errno != EINVAL)
   			{
   			_LIT(Kerr , "failed to read to different I/O Vectors for directory file descriptor") ;
   			INFO_PRINTF1(Kerr) ;
			return KErrGeneral ;
   			}
   	    }
   	
   	return KErrNone ;
	}


TInt CTestSyscalls :: Readv_Negativecount_Test(char *file , TInt Firstarg)
	{
	char Buf1[20] ;
	char Buf2[20] ;
	struct iovec Iovec[2] ;
	int fd = 0 ;
   
	if(Firstarg)
		{
		Iovec[0].iov_len =  0 ;	
		Iovec[1].iov_len = size_t(-123) ;
		}
	else 
		{
		Iovec[0].iov_len =  20 ;	
		Iovec[1].iov_len = size_t(-12) ;
		}
   
	Iovec[0].iov_base = Buf1;
	Iovec[1].iov_base = Buf2 ;
   
	unlink(file) ;
   
	fd = open(file , O_RDWR | O_CREAT , 0666) ;
   
	if(fd < 0 ) 
		{
   		_LIT(Kerr , "Failed to open the given file readv test") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	if(readv(fd , Iovec , 2) < 0 ) 
   		{
   		if(errno != EINVAL)
   			{
   			_LIT(Kerr , "failed to read to different I/O Vectors for directory file descriptor") ;
   			INFO_PRINTF1(Kerr) ;
			return KErrGeneral ;
   			}
   	    }
   	return KErrNone ;
   
	}


#define DIRECT_TEST         0         //test for directory
#define INVALID_ADD_TEST    1         //test for invalid address
#define INVALID_FD          2         //test for invalid file descriptor 
#define WRITE_ONLY_FD       3         // test of read on write only fd
#define INVALID_COUNT_TEST  4         //Test for invalid vector count
#define MAX_IOV_MAX         5         //TEST if the iovector is > IOV_MAX
#define COUNT_NEGATIVE      7         //negative count testing   
#define REGULAR_TEST        8         //functionality test
#define READ_ONLY_FD        9         //Test for read-only fd  for writev

TInt CTestSyscalls :: ReadV() 
	{
	TInt cmd = 0 ;
	TBuf8<50> string ;
	TPtrC String ;
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	string.Copy(String) ;
	char *file = (char *) string.Ptr() ;
   
	file[string.Length()] = '\0' ;
	_LIT( Kcmd, "cmd" );
	res = GetIntFromConfig(ConfigSection(), Kcmd, cmd);
	if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	TInt retVal = 0 ;
   
	switch(cmd) 
		{
   	
   		case  DIRECT_TEST :
   		   retVal = Readv_Dir_Test(file) ;
   		   break ;
   		   
   		case INVALID_FD :
   		    retVal = Readv_Invalid_Fd() ;
   		    break ;
   		    
   		case WRITE_ONLY_FD :
   		    retVal = Readv_Writefd_Test(file ) ;
   		    break ;
   		    
   	    case INVALID_COUNT_TEST :
   	    	{
   	    	TInt retVal1 = Readv_Invalidcount_Test(file , 1) ;
   	    	TInt retVal2 = Readv_Invalidcount_Test(file , 0) ;
   	    	
   	    	if((retVal1 != 0  ) && (retVal2 != 0)) 
   	    		{
   	    		retVal = KErrGeneral ;
   	    		}
   	    	else 
   				{
   	    		retVal = KErrNone ;
   	    		}
   	    	break ;	
   	    	}
   	    case  INVALID_ADD_TEST :
   			{
   			TInt retVal1 = Readv_AddInvalid_Test(file , 1) ;
   	    	TInt retVal2 = Readv_AddInvalid_Test(file , 0) ;
   	    	
   	    	if((retVal1 != 0  ) && (retVal2 != 0)) 
   	    		{
   	    		retVal = KErrGeneral ;
   	    		}
   	    	else 
   				{
   	    		retVal = KErrNone ;
   	    		}
   	    	break ;	
   	    	}
   	    	
   	    case MAX_IOV_MAX :
   	        {
   	        retVal = Readv_MaxIov_Test(file) ;
   	        break;	
   	        }	
   	     
   	    case COUNT_NEGATIVE:
   	    	{
   	    	TInt retVal1 = Readv_Negativecount_Test(file , 0) ;
   	    	TInt retVal2 = Readv_Negativecount_Test(file , 0) ;
   	    	
   	    	if((retVal1 != 0  ) && (retVal2 != 0)) 
   	    		{
   	    		retVal = KErrGeneral ;
   	    		}
   	    	else 
   				{
   	    		retVal = KErrNone ;
   	    		}
   	    	break ;	
   	    	}
   	        
   	    case REGULAR_TEST :
   	    	{
   	    	 unlink(file) ;
   	    	 Writev_Normal(file) ;
   	    	 retVal = Readv_Normal(file);
   	    	 unlink(file) ;
   	    	 break ;
   	    	}
   	   
      }                 //End of switch
	return retVal ;
   
	}


/**
This function tests the writev api
**/

TInt CTestSyscalls :: WriteV() 
	{
   
	TInt cmd = 0 ;
	TBuf8<50> string ;
	TPtrC String ;

	_LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	string.Copy(String) ;
	char *file = (char *) string.Ptr() ;
   
	file[string.Length()] = '\0' ;
   
	_LIT( Kcmd, "cmd" );
	res = GetIntFromConfig(ConfigSection(), Kcmd, cmd);
    if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	TInt retVal = 0 ;
   
	switch(cmd) 
		{
   	
   		case  DIRECT_TEST :
   			retVal = Writev_Dir_Test(file) ;
   			break ;
   		   
   		case INVALID_FD :
   		    retVal = Writev_Invalid_Fd() ;
   		    break ;
   		    
   		case READ_ONLY_FD :
   		    retVal = Writev_Readfd_Test(file ) ;
   		    break ;
   		    
   	    case INVALID_COUNT_TEST :
   	    	{
   	    	TInt retVal1 = Writev_Invalidcount_Test(file , 1) ;
   	    	TInt retVal2 = Writev_Invalidcount_Test(file , 0) ;
   	    	
   	    	if((retVal1 != 0  ) && (retVal2 != 0)) 
   	    		{
   	    		retVal = KErrGeneral ;
   	    		}
   	    	else 
   				{
   	    		retVal = KErrNone ;
   	    		}
   	    	break ;	
   	    	}
   	    case  INVALID_ADD_TEST :
   			{
   	    	TInt retVal1 = Writev_AddInvalid_Test(file , 1) ;
   	    	TInt retVal2 = Writev_AddInvalid_Test(file , 0) ;
   	    	
   	    	if((retVal1 != 0  ) && (retVal2 != 0)) 
   	    		{
   	    		retVal = KErrGeneral ;
   	    		}
   	    	else 
   				{
   	    		retVal = KErrNone ;
   	    		}
   	    	break ;	
   	    	}
   	    	
   	    case MAX_IOV_MAX :
   	        {
   	        retVal = Writev_MaxIov_Test(file) ;
   	        break;	
   	        }	
   	     
   	    case COUNT_NEGATIVE:
   	    	{
   	    	TInt retVal1 = Writev_Negativecount_Test(file , 0) ;
   	    	TInt retVal2 = Writev_Negativecount_Test(file , 0) ;
   	    	
   	    	if((retVal1 != 0  ) && (retVal2 != 0)) 
   	    		{
   	    		retVal = KErrGeneral ;
   	    		}
   	    	else 
   				{
   	    		retVal = KErrNone ;
   	    		}
   	    	break ;	
   	    	}
   	        
   	        case REGULAR_TEST :
   	    	{
   	    	Writev_Normal(file) ;
   	    	retVal = Readv_Normal(file);
   	    	unlink(file) ;
   	    	break ;
   	    	}
   	   
      }                 //End of switch
   return retVal ;
   
	}

TInt CTestSyscalls :: SeekDir() 
	{
	TInt Expected  = 0 ;
	TBuf8<50> string ;
	TPtrC String ;
 
	_LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
    if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	string.Copy(String) ;
	char *file = (char *) string.Ptr() ;
   
	file[string.Length()] = '\0' ;
   
   
	_LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
 
	if(!res) 
		{
   		_LIT(Kerr, "Failed to Expected value from ini file") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	char file1[100] =   	{ 0 } ;
	char file2[100] =   	{ 0 } ;
	char file3[100] =   	{ 0 } ;
   
	strcpy(file1 , file) ;
	strcat(file1 , "Test_seek1.txt") ;
	strcpy(file2 , file) ;
	strcat(file2 , "Test_seek2.txt") ;
	strcpy(file3 , file) ;
	strcat(file3 , "Test_seek3.txt") ;
   
	unlink(file1) ; 
	unlink(file2) ; 
	unlink(file3) ; 
	rmdir(file) ;
   
	if(mkdir(file , 0777) < 0 ) 
   		{
   	
   		_LIT(Kerr , "Failed to create the given directory %d") ;
   		int n = errno;
   		INFO_PRINTF2(Kerr,n) ;
   		return KErrGeneral ;
   		}
   	
   
	int fd = open(file1 , O_CREAT | O_RDWR , 0777) ;
   
	if(fd < 0 ) 
   		{
   		_LIT(Kerr , "failed to open create file Test_seek1.txt") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   	
   	close (fd) ;
   	
	fd = open(file2 , O_CREAT | O_RDWR , 0777) ;
   
	if(fd < 0 ) 
   		{
   		_LIT(Kerr , "failed to open create file Test_seek2.txt") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   	
   	close (fd) ; 
   	
   	
    fd = open(file3 , O_CREAT | O_RDWR , 0777) ;
   
	if(fd < 0 ) 
   		{	
   		_LIT(Kerr , "failed to open create file Test_seek3.txt") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   	
   	close (fd) ; 
   	
   	if(Expected < 0 ) 
		{
   	    DIR *Handle = opendir (file) ;
   		if(!Handle)	 
   			{
   			_LIT(Kerr , "Failed to open directory ") ;
   			INFO_PRINTF1(Kerr) ;
   			return KErrGeneral ;	
   			}
   		seekdir(Handle , telldir(Handle) -1 ) ;
   	   
   		struct dirent *Dir ;
   		Dir = readdir(Handle);
   		if(!Dir) 
   			{
   	   		_LIT(Kok , "invalid seek \n") ;
   	   		INFO_PRINTF1(Kok) ;
   	   		}
   	   	else 
   	   		{
   	   		_LIT(Kerr , "read from from invalid handle \n") ;
   	   		INFO_PRINTF1(Kerr) ;
   	   		return KErrGeneral ;
   	   		}
   	   	rewinddir(Handle ) ;
   	   	seekdir(Handle , telldir(Handle) + 2) ;
   	   	Dir = readdir(Handle);
   	   	if(!Dir) 
   	   		{
   	   		_LIT(Kerr , "Failed to read from the directory.")  ;
   	   		INFO_PRINTF1(Kerr) ;
   	   		return KErrGeneral ;
   	   		}
   		Dir = readdir(Handle);
        if(!Dir) 
   	   		{
   	   		_LIT(Kok , "illegal seek.")  ;
   	   		INFO_PRINTF1(Kok) ;
   	   		}
   	   	 
		rewinddir(Handle) ;
   	   	seekdir(Handle , telldir(Handle) + 1) ;
   	   	
   	   	Dir = readdir(Handle);
   	   	if(!Dir) 
   	   		{
   	   		_LIT(Kerr , "Failed to read from the directory..") ;
   	   		INFO_PRINTF1(Kerr) ;
   	   		return KErrGeneral ;
   	   		}
   	   	 
   	   	Dir = readdir(Handle);
        if(!Dir) 
   	   		{
   	   		_LIT(Kerr , "Failed to read from the directory... ") ;
   	   		INFO_PRINTF1(Kerr) ;
   	   		return KErrGeneral ;
   	   		}
   	   	Dir = readdir(Handle);
   	   	if(!Dir) 
   	   		{
   	   		_LIT(Kok , "Failed to read from the directory.... ") ;
   	   		INFO_PRINTF1(Kok) ;
   	   		}
   	   	 
   	   	rewinddir(Handle) ;
   	   	seekdir(Handle , telldir(Handle) + 0 ) ;
   	   	 
   	   	Dir = readdir(Handle);
   	   	if(!Dir) 
   	   		{
   	   		_LIT(Kerr , "Failed to read from the directory..... ") ;
   	   		INFO_PRINTF1(Kerr) ;
   	   		return KErrGeneral ;
   	   		}
   	   	Dir = readdir(Handle);
   	   	if(!(Dir)) 
   	   		{
   	   		_LIT(Kerr , "Failed to read from the directory...... ") ;
   	   		INFO_PRINTF1(Kerr) ;
   	   		return KErrGeneral ;
   	   		}
   	   	 
   	   	Dir = readdir(Handle);
   	   	if(!(Dir)) 
   	   		{
   	   		_LIT(Kerr , "Failed to read from the directory....... ") ;
   	   		INFO_PRINTF1(Kerr) ;
   	   		return KErrGeneral ;
   	   		}
   	   	Dir = readdir(Handle);
   	   	if(!(Dir)) 
   	   		{
   	   		_LIT(Kok , "illegal seek.")  ;
   	   		INFO_PRINTF1(Kok) ;
   	   		}
   		rewinddir(Handle)  ;
         
        seekdir(Handle  , telldir(Handle) + 4) ;
        Dir = readdir(Handle);
		if(!(Dir)) 
   	   		{
   	   		_LIT(Kok , "illegal seek.")  ;
   	   		INFO_PRINTF1(Kok) ;
   	   		}
   	   	closedir(Handle) ;
   	   	}
   	
	DIR *Handle ;
	//struct dirent *Dir ;
   
	if ((Handle = opendir(file)) == NULL) 
   		{
   		_LIT(Kerr , "Failed to open given directory %d\n") ;
   		int n = errno;
   		INFO_PRINTF2(Kerr, n) ;
   		return KErrGeneral ;
   		}
   	
	int count = 0 ;
   	
   	while(/*Dir =*/ readdir(Handle)) 
   		{
   		count++ ;
   		}
   	
   	if(count != 3) 
   		{
   		_LIT(Kerr , "Actual entries and read entries differ \n") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   	closedir(Handle);
   	return KErrNone ;
}

TInt CTestSyscalls :: RewindDir() 
	{
   
	TBuf8<50> string ;
	TPtrC String ;
   
	_LIT(Kinfo , "In RewindDir") ;
	INFO_PRINTF1(Kinfo) ;
 
	_LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );
    if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	string.Copy(String) ;
	char *file = (char *) string.Ptr() ;
   
	file[string.Length()] = '\0' ;
    
	char file1[100] =   	{ 0 } ;
	char file2[100] =   	{ 0 } ;
	char file3[100] =   	{ 0 } ;
   
	strcpy(file1 , file) ;
	strcat(file1 , "\\Test_seek1.txt") ;
	strcpy(file2 , file) ;
	strcat(file2 , "\\Test_seek2.txt") ;
	strcpy(file3 , file) ;
	strcat(file3 , "\\Test_seek3.txt") ;
   
	unlink(file1) ; unlink(file2) ; unlink(file3) ; rmdir(file) ;
   
	if(mkdir(file , 0777) < 0 ) 
   		{
   	
   		_LIT(Kerr , "Failed to create the given directory") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
	int fd = open(file1 , O_CREAT | O_RDWR , 0777) ;
   
	if(fd < 0 ) 
   		{
   		_LIT(Kerr , "failed to open create file Test_seek1.txt") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   	
	close (fd) ;
   	
	fd = open(file2 , O_CREAT | O_RDWR , 0777) ;
   
	if(fd < 0 ) 
   		{
   		_LIT(Kerr , "failed to open create file Test_seek2.txt") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   	
   	close (fd) ; 
   	
   	DIR *Handle = opendir(file) ;
   	
   	if(!Handle) 
   		{
   		_LIT(Kerr , "Failed to open the directory ")  ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   		
	int count = 0 ;
   
	while(/*Dir = */readdir(Handle)) 
   		{
   		count++ ;
   		}
   
	if(count != 2) 
    	{
    	_LIT(Kerr , "expected and returned no of directory entries are different.") ;
    	INFO_PRINTF1(Kerr) ;
    	closedir(Handle);
    	return KErrGeneral ;
    	}
    
    fd = open(file3 , O_CREAT | O_RDWR , 0666) ;
    
    if(fd < 0 )  
    	{
    	
    	_LIT(Kerr , "Failed to open final file ") ;
    	INFO_PRINTF1(Kerr) ;
    	closedir(Handle);
    	return KErrGeneral ;
    	}
	close(fd) ;
   
	rewinddir(Handle) ;  	
	count = 0 ;
   
	while(/*Dir =*/ readdir(Handle)) 
   		{
   		count++ ;
   		}
   
    if(count != 3) 
    	{
    	_LIT(Kerr , "expected and returned no of directory entries are different..") ;
    	INFO_PRINTF1(Kerr) ;
    	closedir(Handle);
    	return KErrGeneral ;
    	}
  	closedir(Handle);
	return KErrNone ;  	
   	}

TInt CTestSyscalls :: Telldir() 
	{
   
	TBuf8<50> string ;
	TPtrC String ;
   
	_LIT(Kinfo , "In RewindDir") ;
	INFO_PRINTF1(Kinfo) ;
   
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	string.Copy(String) ;
	char *file = (char *) string.Ptr() ;
   
	file[string.Length()] = '\0' ;
   
   
   
	char file1[100] =   	{ 0 } ;
	char file2[100] =   	{ 0 } ;
	char file3[100] =   	{ 0 } ;
   
	strcpy(file1 , file) ;
	strcat(file1 , "\\Test_seek1.txt") ;
	strcpy(file2 , file) ;
	strcat(file2 , "\\Test_seek2.txt") ;
	strcpy(file3 , file) ;
	strcat(file3 , "\\Test_seek3.txt") ;
   
	unlink(file1) ; 
	unlink(file2) ; 
	unlink(file3) ; 
	rmdir(file) ;
   
	if(mkdir(file , 0777) < 0 ) 
   		{
   	
   		_LIT(Kerr , "Failed to create the given directory") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   	
   
	int fd = open(file1 , O_CREAT | O_RDWR , 0777) ;
   
	if(fd < 0 ) 
   		{
   		_LIT(Kerr , "failed to open create file Test_seek1.txt") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   	
   	close (fd) ;
   	
	fd = open(file2 , O_CREAT | O_RDWR , 0777) ;
   
	if(fd < 0 ) 
   		{
   		_LIT(Kerr , "failed to open create file Test_seek2.txt") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
   		}
   	
   	close (fd) ; 
   	
   	DIR *Handle = opendir(file) ;
   	
   	if(!Handle) 
   		{
   		 _LIT(Kerr , "Failed to open the directory ")  ;
   		 INFO_PRINTF1(Kerr) ;
   		 return KErrGeneral ;
   		}
   		
	int count = 0 ;
   
	do  
		{
   		if(count != telldir(Handle)) 
   			{
   			_LIT(Kerr , "value returned from telldir is not same as expected.");
   			INFO_PRINTF1(Kerr) ;
   			closedir(Handle);
   			return KErrGeneral ;
   			}

   		count++ ;
   		} while(/*Dir = */readdir(Handle)) ;
   
    
    
    fd = open(file3 , O_CREAT | O_RDWR , 0666) ;
    
    if(fd < 0 )  
    	{
       	_LIT(Kerr , "Failed to open final file ") ;
    	INFO_PRINTF1(Kerr) ;
    	closedir(Handle);
    	return KErrGeneral ;
    	}
	close(fd) ;
   
	rewinddir(Handle) ; 
    count = 0 ;	
  
	while(/*Dir = */readdir(Handle)) 
   		{
   		count++ ;
   		if(count != telldir(Handle)) 
   			{
   			_LIT(Kerr , "value returned from telldir is not same as expected.");
   			INFO_PRINTF1(Kerr) ;
   			closedir(Handle);
   			return KErrGeneral ;
   			}
		}
	closedir(Handle);
   return KErrNone ;  	
   }

TInt CTestSyscalls ::TestClock()
	{
	TInt n;
	INFO_PRINTF1(_L("Starting countdown...\n"));
	for (n=10; n>0; n--)
	  {
	  //printf ("%d\n",n);
	  clock_t endwait;
  	  endwait = clock () + 1 * CLOCKS_PER_SEC ;
      while (clock() < endwait) {}
	  }
	INFO_PRINTF1(_L("1 sec delay!!\n"));
	return 0;
	}

TInt CTestSyscalls :: Creat2(  ) // negative test case so on errr conditions returning KErrNone
	{
    int fd ;
   	char mbcbuf[64] = {'\0'};
   	mbcbuf[0] = (unsigned char)0x3A;// colon :
    TPtrC8 outbufstring((TText8*)mbcbuf, strlen(mbcbuf));
       TBuf16<100> outbufname;
      	outbufname.Copy(outbufstring);
      	INFO_PRINTF2(_L("mbcbuf : %S"), &outbufname);
    if((fd = creat(mbcbuf,  0666))  < 0) 
		{
        _LIT(Kstmt , "creat success") ;
        INFO_PRINTF1(Kstmt) ;
    	close(fd) ;
    	return KErrNone;
		}
   

    _LIT(Kstmt , "creat failure") ;
    INFO_PRINTF1(Kstmt) ;
	INFO_PRINTF2(_L("The errno set is %d\n"),errno);
    return KErrGeneral;
    }
TInt CTestSyscalls :: open8() // negative test case so on errr conditions returning KErrNone
	{
	int fd ;
   	char mbcbuf[64]= {'\0'};
   	mbcbuf[0] = (unsigned char)0x3A;// colon :
   	INFO_PRINTF2(_L("The errno set is %s\n"),mbcbuf);
    if((fd = open(mbcbuf,  O_CREAT|O_RDWR , 0444))  < 0) 
		{
        _LIT(Kstmt , "open success") ;
        INFO_PRINTF1(Kstmt) ;
    	close(fd) ;
    	return KErrNone;
		}
    _LIT(Kstmt , "open failure") ;
	INFO_PRINTF2(_L("The errno set is %d\n"),errno);
    INFO_PRINTF1(Kstmt) ;
    return KErrGeneral;
    
	}

TInt CTestSyscalls :: TestStat()
	{
	int ret=KErrNone;
	struct stat buf;
	char *path = "C:\\private\\1028308C";
	
	ret = stat(path, &buf);
	
	if (!ret)
		{
		if(S_ISDIR(buf.st_mode))
			{
			_LIT(KSuccess, "Success");
			INFO_PRINTF1(KSuccess);	
			
			int count=0;
			DIR *handle = opendir(path);
			
			if (handle == NULL)
				{
				INFO_PRINTF1(_L("Failed to open directory"));
				}
			
			while(readdir(handle))
				{
				count++;
				}
			
			closedir(handle);
						
			}
		else if(S_ISREG(buf.st_mode))
			{
			INFO_PRINTF1(_L("Its a file"));	
			}
		
		}
	else
		{
		_LIT(KFailure, "Failure in STAT");
		INFO_PRINTF1(KFailure);
		}
	
	return ret;
	}
	
//-------------------------------------------------------------------------------

//  Test case : -ve test for lseek() on tty console      
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Lseekttytest1()
	{
	TInt res ;
	_LIT( KTsyscalls, "Tsyscalls" );
    _LIT( Klseek, "In Lseek for stderr" );
    INFO_PRINTF1(KTsyscalls) ;
    INFO_PRINTF1( Klseek );    
    res =  lseek(2, 0, SEEK_CUR) ;
    if(res < 0 )   
		{
    	_LIT(Kerr , "lseek always fails on tty") ;
    	INFO_PRINTF1(Kerr) ;
        }
   	return KErrNone ;
	}
	
//-------------------------------------------------------------------------------

//  Test case : -ve test for lseek() on tty console              
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Lseekttytest2()
	{
	TInt res ;
	_LIT( KTsyscalls, "Tsyscalls" );
    _LIT( Klseek, "In Lseek for stdout" );
    INFO_PRINTF1(KTsyscalls) ;
    INFO_PRINTF1( Klseek );    
    res =  lseek(1, 0, SEEK_CUR) ;
    if(res < 0 )   
		{
    	_LIT(Kerr , "lseek always fails on tty") ;
    	INFO_PRINTF1(Kerr) ;
        }
   	return KErrNone ;
	}
	
//-------------------------------------------------------------------------
// Test case : -ve test for waitpid()  
//-------------------------------------------------------------------------

TInt CTestSyscalls :: WaitPidtest() 
	{
	TInt status ;
    TInt ret;
	_LIT( KTsyscalls1, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls1) ;
	_LIT( Kwaitpid1 , "WaitPid" );
	INFO_PRINTF1(Kwaitpid1);    
    ret = waitpid(-1 , &status , WUNTRACED);
	if (ret < 0)
		{
		_LIT( Kwaitinfo , "negative test for waitpid()" );
		INFO_PRINTF1(Kwaitinfo); 	
		}	
	return KErrNone ;
	}

//-------------------------------------------------------------------------
// Test case : -ve test for wait()
//-------------------------------------------------------------------------

TInt CTestSyscalls :: Waittest() 
	{
	TInt status ;
    TInt ret;
	_LIT( KTsyscalls1, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls1) ;
	_LIT( Kwait1 , "Wait" );
	INFO_PRINTF1(Kwait1);    
    ret = wait(&status);
	if (ret < 0)
		{
		_LIT( Kwaitinfo , "negative test for wait()" );
		INFO_PRINTF1(Kwaitinfo); 	
		}	
	return KErrNone ;
	}
	
	
	/*-----------------------------------------------------------------
	 *Test Case:To Check 256 open file descriptor is supported
	 *-----------------------------------------------------------------
	*/
	
	TInt CTestSyscalls :: Open_FileDes_Test()
		{
		int fd[253];
		TPtrC String ;
		_LIT( KString, "String" );
		TBool res = GetStringFromConfig(ConfigSection(), KString, String );
		if(res)
			{
			TBuf8<100> string1;
			string1.Copy(String);
			char* file = (char*) string1.Ptr();
			file[string1.Length()]='\0';
	    	_LIT( KOpen3, "In open3" );
	  	    INFO_PRINTF1( KOpen3 );
	    	unlink(file);
  			for(TInt i=0;i<253;i++)
  				{
  				fd[i] = open(file ,O_CREAT | O_WRONLY , 0666);
   				if(fd[i]<0) 
   					{
   					_LIT( KFailedinfo, "Failed to create and open file in current working directory" );
      				INFO_PRINTF1(KFailedinfo);
      				return KErrGeneral;
   					}
  				}
  			for(int i=0;i<253;i++)
	 			{
	 			close(fd[i]);
	 			}
			}
		return KErrNone ;
		}

TInt CTestSyscalls :: openuid()
	{
	TInt openret,ret;
	
	ret = mkdir ("c:\\private\\1028308c",S_IRWXU| S_IRWXG); 
	_LIT( KTsyscalls2, "MKDIR passed %d " );
	INFO_PRINTF2(KTsyscalls2,ret);
	openret = open("c:\\Private\\1028308c\\name1_1028308c.db", O_RDWR|O_CREAT|O_BINARY|0, 0600);
	
	if(openret == -1)
		{
		_LIT( KTsyscalls1, "open failed" );
		INFO_PRINTF1(KTsyscalls1);
		return KErrGeneral;
		}
	_LIT( KTsyscalls1, "open passed" );
	INFO_PRINTF1(KTsyscalls1);
	close(openret);

	return KErrNone;
	}
//-------------------------------------------------------------------------------

//  Test case : MkDir1 - Tests wheather the mkdir sets the Errno EEXIST for the 
//              path c:\\private\\<UID3>
//            : returns 0 on success
//            : 
//
//--------------------------------------------------------------------------------                  

TInt CTestSyscalls :: Mkdir1()
	{
	TInt ret1,ret2;
	
	ret1 = mkdir ("c:\\private\\1028308c",S_IRWXU| S_IRWXG); 
	
	if(ret1 == 0)
		{
		ret2 = mkdir ("c:\\private\\1028308c",S_IRWXU| S_IRWXG); 
		_LIT( KTsyscalls2, "mkdir returned %d" );
		INFO_PRINTF2(KTsyscalls2,ret2);
		if(ret2==-1 && errno == EEXIST)
			{
			_LIT( KTsyscalls1, "mkdir passed" );
			INFO_PRINTF1(KTsyscalls1);
			return KErrNone;
			}
		}
	else if(ret1 ==-1 && errno == EEXIST)
		{
		_LIT( KTsyscalls3, "mkdir returned %d and set the proper error code" );
		INFO_PRINTF2(KTsyscalls3,ret1);
		return KErrNone;
		}
	_LIT( KTsyscalls1, "mkdir failed" );
	INFO_PRINTF1(KTsyscalls1);
	
	return KErrGeneral;
	}

//-------------------------------------------------------------------------------

//  Test case : MkDir2 - Tests wheather the mkdir sets the Errno EEXIST for the 
//              path c:\\private\\<UID3>\\directory
//            : returns 0 on success
//            : 
//
//--------------------------------------------------------------------------------                  


TInt CTestSyscalls :: Mkdir2()
	{
	TInt ret1,ret2;
	
	ret1 = mkdir ("c:\\private\\1028308c\\testcase",S_IRWXU| S_IRWXG); 
	
	if(ret1 == 0)
		{
		ret2 = mkdir ("c:\\private\\1028308c\\testcase",S_IRWXU| S_IRWXG); 
		_LIT( KTsyscalls2, "mkdir returned %d" );
		INFO_PRINTF2(KTsyscalls2,ret2);
		if(ret2==-1 && errno == EEXIST)
			{
			_LIT( KTsyscalls1, "mkdir passed" );
			INFO_PRINTF1(KTsyscalls1);
			return KErrNone;
			}
		}
	else if(ret1 ==-1 && errno == EEXIST)
		{
		_LIT( KTsyscalls3, "mkdir returned %d and set the proper error code" );
		INFO_PRINTF2(KTsyscalls3,ret1);
		return KErrNone;
		}
	_LIT( KTsyscalls1, "mkdir failed" );
	INFO_PRINTF1(KTsyscalls1);
	
	return KErrGeneral;
	}

//-------------------------------------------------------------------------------

//  Test case : Chmod for Others and group
//            : Negative Test case 
//            :  
//--------------------------------------------------------------------------------   

TInt CTestSyscalls :: Chmod1()
	{
	TInt Expected ,Mode ;
	TBuf8<50> string ;
	TPtrC  String  ;
   
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Kchmod, "In Chmod1" );
	INFO_PRINTF1( Kchmod);
   
	//Read input file name 
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res)  
		{
   		_LIT(Kerr , "failed to read input file name ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	string.Copy(String) ;
   
	char *file = (char *) string.Ptr() ;
	file[string.Length()] = '\0' ;
   
	unlink(file) ;
	
	//To read flag bit to be set
	_LIT( KMode, "Mode" );
	res = GetIntFromConfig(ConfigSection(), KMode, Mode );
    _LIT(Kerr,"mode value is %d\n");
	INFO_PRINTF2(Kerr,Mode) ;
	if(!res)  
		{
   		_LIT(Kerr , "failed to read the file mode bit from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	//Now read expected value 
	_LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
    if(!res) 
		{
    	_LIT(Kerr , "Failed to read expected value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    if(Expected < 0 ) 
		{
   		TInt ret1;
       	ret1 = chmod(file , Mode) ;
		if(ret1 == Expected) 
			{
			if(errno != EINVAL) 
				{
      			_LIT(Kerr, "Failed to set errno to EINVAL") ;
      			INFO_PRINTF1(Kerr) ;
      			return KErrGeneral ;
				}
			}
		}
	_LIT(KMsg,"Chmod success");
	INFO_PRINTF1(KMsg) ;
	return KErrNone;
	}
	
	//-----------------------------------------------------------------------------
// This function test rename call for directories

//------------------------------------------------------------------------------
 
TInt CTestSyscalls :: Rename2() 
	{
	int fd ;
	TPtrC String ;
	TBuf8 <50> string ;
	TInt HasFiles ;
	char File1[50] ; 
	char File2[50] ;
   
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Krename, "In Rename2" );
	INFO_PRINTF1( Krename);
   
    
	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );

	if(!res) 
		{
   		_LIT(Kerr , "failed to read input file name ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	string.Copy(String) ; 
	char *file = (char *) string.Ptr() ;
	file[string.Length()] = '\0' ;
   
	strcpy(File1 , file) ;
	TPtrC theString;
	_LIT( KtheString, "theString" );
	res = GetStringFromConfig(ConfigSection(), KtheString, theString );
	if(!res) 
		{
   		_LIT(Kerr , "failed to read input file1 name ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
   
	string.Copy(theString) ; 
	file = (char *) string.Ptr() ;
	file[string.Length()] = '\0' ;
   
	strcpy(File2 , file);
   
	_LIT( KHasFiles, "HasFiles" );
	res = GetIntFromConfig(ConfigSection(), KHasFiles, HasFiles);
 
	if(!res) 
		{
    	_LIT(Kerr , "Failed to read expected value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
	switch(HasFiles) 
   		{
   		case 0 :
   		   	{
   		   	/*Rename empty directory*/
   		   	res = mkdir(File1,0666);	   	
		   	if(res < 0 ) 
		   		{
		   		_LIT(Kerr , "Failed to create directory1\n") ;
		   		INFO_PRINTF1(Kerr) ;
		   		return KErrGeneral ;
		   		}
   		   	if(rename(File1 , File2) < 0 ) 
   		   		{
   		   		_LIT(Kerr , "Rename of directory failed \n") ;
   		   		INFO_PRINTF1(Kerr) ;
   		   		rmdir(File1);
   		   		return KErrGeneral ;
   		   		}
   		   	rmdir(File2);
   		   	break ;
   		   	}
   		   	
   		case 1:
   			{ 
   			/* Rename a non empty directory*/
   			res = mkdir(File1,0666);	   	
			   	if(res < 0 ) 
			   		{
			   		_LIT(Kerr , "Failed to create directory1\n") ;
			   		INFO_PRINTF1(Kerr) ;
			   		return KErrGeneral ;
			   		}
   			chdir(File1);
   			fd =  open("./abc.txt" , O_CREAT | O_RDWR , 0666) ;  			
   			if(fd < 0) 
   				{
   				_LIT(Kerr , "Failed to open given file2 \n") ;
   				INFO_PRINTF1(Kerr) ;
   				return KErrGeneral ;
   				}
   				close(fd);
   			chdir("..");
   			if(rename(File1 , File2) < 0 ) 
   				{
   				_LIT(Kerr , "Failed to Rename\n") ;
   				printf("%d",errno);
   				INFO_PRINTF1(Kerr) ;
   				chdir(File1);
   				if(unlink("./abc.txt")	< 0)
	   				{
	   				_LIT(Kerr , "Failed to unlinkfile2 \n") ;
	   				INFO_PRINTF1(Kerr) ;
	   				return KErrGeneral ;	
	   				}
   				rmdir(File2);
   				return KErrGeneral ;
   				}
   			chdir(File2);
   			unlink("./abc.txt")	;
   			chdir("..");
   			rmdir(File2);
   		    break ;	
   			}
   			case 2:
	   			{ 
	   			/* Try to rename to an nonempty destination*/
	   			res = mkdir(File1,0666);	   	
		   		if(res < 0 ) 
		   		{
		   		_LIT(Kerr , "Failed to create directory1\n") ;
		   		INFO_PRINTF1(Kerr) ;
		   		return KErrGeneral ;
		   		}
	   			res = mkdir(File2,0666);	
			   	if(res < 0 ) 
			   		{
			   		_LIT(Kerr , "Failed to create directory\n") ;
			   		INFO_PRINTF1(Kerr) ;
			   		rmdir(File1);
			   		return KErrGeneral ;
			   		}	
			   	chdir(File1);
		   		fd =  open("./abc.txt" , O_CREAT | O_RDWR , 0666) ;  			
		   		if(fd < 0) 
		   			{
		   			_LIT(Kerr , "Failed to open given file2 \n") ;
		   			INFO_PRINTF1(Kerr) ;
		   			return KErrGeneral ;
		   			}
		   			close(fd);
		   		chdir(File2);
		   		fd =  open("./abc.txt" , O_CREAT | O_RDWR , 0666) ;  			
		   		if(fd < 0) 
		   			{
		   			_LIT(Kerr , "Failed to open given file2 \n") ;
		   			INFO_PRINTF1(Kerr) ;
		   			return KErrGeneral ;
		   			}
		   			close(fd);
		   		if(rename(File1 , File2) == 0 ) 
		   			{
		   			_LIT(Kerr , "Test Case Failed\n") ;
		   			INFO_PRINTF1(Kerr) ;
		   			return KErrGeneral ;
		   			}
		   		chdir(File1);
		   		unlink("./abc.txt");
		   		chdir(File2);
		   		rmdir(File1);
		   		unlink("./abc.txt");
				chdir("..");
				rmdir(File2);
		   		break;
	   			}
   		case 3:
   			{
   				/* Try to rename to an nonempty destination*/
	   			res = mkdir(File1,0666);	   	
		   		if(res < 0 ) 
		   		{
		   		_LIT(Kerr , "Failed to create directory1\n") ;
		   		INFO_PRINTF1(Kerr) ;
		   		return KErrGeneral ;
		   		}
	   			res = mkdir(File2,0666);	
			   	if(res < 0 ) 
			   		{
			   		_LIT(Kerr , "Failed to create directory\n") ;
			   		INFO_PRINTF1(Kerr) ;
			   		rmdir(File1);
			   		return KErrGeneral ;
			   		}	
			   	chdir(File1);
		   		fd =  open("./abc.txt" , O_CREAT | O_RDWR , 0666) ;  			
		   		if(fd < 0) 
		   			{
		   			_LIT(Kerr , "Failed to open given file2 \n") ;
		   			INFO_PRINTF1(Kerr) ;
		   			return KErrGeneral ;
		   			}
		   			close(fd);
		   		if(rename(File1 , File2) < 0 ) 
		   			{
		   			_LIT(Kerr , "Test Case Failed\n") ;
		   			INFO_PRINTF1(Kerr) ;
		   			return KErrGeneral ;
		   			}
		   		chdir(File2);
		   		unlink("./abc.txt");
				chdir("..");
				rmdir(File2);
		   		break;	
   			}
		}	
   	return KErrNone ;
	}

//-------------------------------------------------------------------------------

//  Test case : mkdir  makes a directory and checks with the expected value 
//            : returns 0 on success
//           
//
//--------------------------------------------------------------------------------                  

TInt CTestSyscalls :: Mk_dir()
	{
    TBuf8<50> string ;
    TPtrC  String ;
    TInt Expected , ret , Expected_Errno;     //To store expected and return values
    
    _LIT( KTsyscalls, "Tsyscalls" );
    INFO_PRINTF1(KTsyscalls) ;
    _LIT( Kmkdir, "In Mk_dir" );
    INFO_PRINTF1( Kmkdir);
    
    
    //First read input directory name
    _LIT( KString, "String" );
    TBool res = GetStringFromConfig(ConfigSection(), KString, String );

    if(!res) 
		{
		_LIT(Kerr , "Failed to read input dir name from ini file") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral;
		}
    string.Copy(String) ;
    
    char *DirName = (char *)string.Ptr() ;
    
    DirName[string.Length()] = '\0' ;
     
    //Read Expected value  
    _LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
 
    if(!res) 
		{
    	_LIT(Kerr , "Failed to read expected value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
        
    //errno Expected value  
    _LIT( KExpected_Errno, "Expected_Errno" );
	res = GetIntFromConfig(ConfigSection(), KExpected_Errno, Expected_Errno);
 
    if(!res) 
		{
    	_LIT(Kerr , "Failed to read expected errno value from the ini file") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral ;
		}
    rmdir(DirName) ;
     
    ret = mkdir(DirName , S_IWUSR) ;
    
    if(ret == Expected ) 
		{
    	if(errno == Expected_Errno)
    		{
    		_LIT(KSuccess, "Success in setting the errno value");
    		INFO_PRINTF1(KSuccess);
    		return KErrNone;
    		}
    	else
    		{
    		_LIT(KErr,"Failure in setting the errno value");
    		INFO_PRINTF1(KErr);	
    		return KErrGeneral;
    		}
		}
	else
		{
		_LIT(KError,"Error in returning the value");
    	INFO_PRINTF1(KError);
    	return KErrGeneral;
		}
	}

//-------------------------------------------------------------------------------

//  Test case : rm_dir  removes the given direcorty and checks with the expected value 
//				Contains both positive and negative test cases	
//            : returns 0 on success
//            : -1 on failure  
//
//--------------------------------------------------------------------------------                  

TInt CTestSyscalls :: Rm_dir()
	{
	_LIT( KTsyscalls, "Tsyscalls" );
	INFO_PRINTF1(KTsyscalls) ;
	_LIT( Krm_dir, "In Rm_dir" );
	INFO_PRINTF1( Krm_dir);
   
	TInt Expected , Expected_Errno ;
	TPtrC String ;
	TBuf8<50>string ;
	TInt ret1 ;

	_LIT( KString, "String" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res) 
		{
   		_LIT(Kerr , "Failed to read input dir name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	string.Copy(String) ;
   
	char *DirName = (char *) string.Ptr() ;
	DirName[string.Length()] = '\0' ;
   
	_LIT( KExpected, "Expected" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
	if(!res) 
		{
   		_LIT(Kerr , "Failed to read input expected value ini file") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
		
	_LIT( KExpected_Errno, "Expected_Errno" );
	res = GetIntFromConfig(ConfigSection(), KExpected_Errno, Expected_Errno);
	if(!res) 
		{
   		_LIT(Kerr , "Failed to read input expected value ini file") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	if(Expected_Errno == 20)
	  	{
		int fd = open(DirName,O_WRONLY|O_CREAT);
		if(fd == -1)
			{
			_LIT(KErr,"failed to create a file and the errno is %d");
			INFO_PRINTF2(KErr,errno);
			return KErrGeneral;
			}
	  	}
  	if(Expected < 0)
	  	{
	  	//negative test case
	  	ret1 = rmdir(DirName) ;
	   	if(ret1 == Expected ) 
	   		{   
	   	    if(errno == Expected_Errno)
	   		 	{
	   	        _LIT(Kmsg , "rmdir successfully passed") ;
	   			INFO_PRINTF1(Kmsg);
	   			return KErrNone;
	   			}
	   		else
	   			{
	   			_LIT(Kerr , "Failed to set errno value") ;
	   	       	INFO_PRINTF1(Kerr) ;
	   	       	return KErrGeneral ;
	   			}
	   		}
	   	else 
	   		{
	  		_LIT(Kerr , "Expected and actual values are different ") ;
	   		INFO_PRINTF1(Kerr) ;
	       	return KErrGeneral ;
	  		}
	   	}
	else
		{
		//to check only pass scenerio of rmdir
		mkdir(DirName,S_IWUSR);
		ret1 = rmdir(DirName);
		if(ret1 == 0)
			{
			_LIT(Kmsg1,"positive case of rmdir is success");
			INFO_PRINTF1(Kmsg1);
			return KErrNone;
			}
		else
			{
			_LIT(Kerr1,"failed to delete the directory");
			INFO_PRINTF1(Kerr1);
			return KErrGeneral;
			}
		}
	}

/*
 * Test case to verify the thread safety of the lseek & fsync api when the file is buffered.
 */

int fd;

void* handler(void *)
	{
	int ret;
	ret = fsync(fd);
	ret = ret;
	return 0;
	}


void* handler1(void *)
	{
	int ret;
	lseek(fd, 2, SEEK_SET);
	ret = fsync(fd);   //only one fsync should be able to sync the data.
	ret = ret;
	return 0;
	}

TInt CTestSyscalls :: sync_safe()
	{
	int fd1,ret;
	char buf[5];
	pthread_t thread_a, thread_b;
	unlink("c:\\readfrom.txt");
	unlink("c:\\writeto.txt");
	fd1 = open("c:\\readfrom.txt", O_RDWR | O_CREAT, 0666);
	write(fd1,"hello",5);
	lseek(fd1,0,SEEK_SET);
	fd = open("c:\\writeto.txt",O_RDWR | O_CREAT | O_BUFFERED);
	ret = read(fd1, buf, sizeof(buf));
 	ret = write(fd,buf,sizeof(buf));
	pthread_create (&thread_a, 0 , handler, 0);
	pthread_create (&thread_b, 0 , handler1, 0);
	
	pthread_join(thread_a, NULL);
	pthread_join(thread_b, NULL);
	lseek(fd, 0, SEEK_SET);
	ret = read(fd, buf, sizeof(buf));
	if(ret == -1)
	{
		INFO_PRINTF2(_L("The errno set from read api is %d"),errno);
		unlink("c:\\readfrom.txt");
		unlink("c:\\writeto.txt");
		return KErrGeneral;
	}
	if(strncmp(buf,"hello",5)!=0)
	{
		INFO_PRINTF1(_L("The test has failed"));
		unlink("c:\\readfrom.txt");
		unlink("c:\\writeto.txt");
		return KErrGeneral;
	}
		
	close(fd1);
	close(fd);
	unlink("c:\\readfrom.txt");
	unlink("c:\\writeto.txt");
	return KErrNone;
	}


TInt CTestSyscalls :: testfsync() // negative test case so on errr conditions returning KErrNone
	{
	
	int x = 0,res=KErrNone;
	x=fsync(0);
	if(x<0)
		{
		INFO_PRINTF1(_L("fsync failed on STDIN"));
		res=KErrGeneral;
		}
	else 
		{
		INFO_PRINTF1(_L("fsync passed on STDIN"));
		}
	return res;
	}


	
/*********************************************/
/*negative test case for Rename()
To check whether the errno set in the case where we try to rename
C:\\orig to C:\\\orig\\dest is EINVAL or not*/
/*********************************************/

TInt CTestSyscalls :: testrename() 
	{	
	int err = 0;
	int res = mkdir("C:\\Orig\\", 0666)	;
	if (res < 0)
		{
		INFO_PRINTF1(_L("mkdir() for parent directory failed\n"));
		return res;
		}
	/*check for Rename() when old path present and new path absent
	  and new path is the child directory of old path*/
	res = rename("C:\\Orig\\", "C:\\Orig\\Dest"); 
	err = errno;
	if (err != 22)
		{
		INFO_PRINTF4(_L("%d, %d , %s\n"), res, err, strerror(err));
		}
	else
		{
		INFO_PRINTF1(_L("Rename() set the correct errno when C:\\Orig\\Dest\\ is not present\n"));
		}
	/*check for Rename() when old path present and new path present
	  and new path is the child directory of old path*/
	res = mkdir("C:\\Orig\\Dest\\", 0666)	;
	if (res < 0)
		{
		INFO_PRINTF1(_L("mkdir() for child directory failed\n"));
		return res;
		}
	res = rename("C:\\Orig\\", "C:\\Orig\\Dest"); 
	err = errno;
	if (err != 22)
		{
		INFO_PRINTF4(_L("%d, %d , %s\n"), res, err, strerror(err));
		}
	else
		{
		INFO_PRINTF1(_L("Rename() set the correct errno when C:\\Orig\\Dest\\ is present\n"));
		}
	res = rmdir("C:\\Orig\\Dest\\");
	if (res < 0)
		{
		INFO_PRINTF1(_L("rmdir() for child directory failed\n"));
		return res;
		}
	res = rmdir("C:\\Orig\\");
	if (res < 0)
		{
		INFO_PRINTF1(_L("rmdir() for parent directory failed\n"));
		return res;
		}
	return res;
	}

/**************************************************************
negative test case for Open()
To check whether the errno set in the case where 
we try to open a file in read only mode with O_TRUNC
errno = EACCES
we try to open a existing file with O_CREAT and O_EXCL
errno = EXIST
we try to open a existing file with O_CREAT, O_EXCL and O_APPEND
errno = EXIST
we try to open a existing file with O_CREAT, O_EXCL and O_TRUNC
errno = EXIST
****************************************************************/

TInt CTestSyscalls :: testopenvalidate() 
	{
	INFO_PRINTF1(_L("open a file in read-only mode\n"));
	int fd = open("c:\\opentest.txt",O_CREAT, S_IRUSR);
	if (fd < 0)
		{
		INFO_PRINTF1(_L("open() a file in read-only mode failed\n"));
		return fd;
		}	
	INFO_PRINTF1(_L("open the existing read-only file with O_TRUNC mode\n"));
	int fd1 = open("c:\\opentest.txt",O_TRUNC);
	if (fd1 < 0)
		{
		INFO_PRINTF1(_L("open the existing read-only file with O_TRUNC mode failed\n"));
		if (errno != EACCES)
			{
			INFO_PRINTF1(_L("open() doesn't set errno correctly for the existing read-only file opened with O_TRUNC mode\n"));
			return fd1;
			}
		INFO_PRINTF2(_L("open() set errno correctly for the existing read-only file opened with O_TRUNC mode to %d\n"), errno);
		}
	int ret = close(fd);
	if (ret < 0)
		{
		INFO_PRINTF1(_L("close() for the file fd failed\n"));
		return ret;
		}
	ret = unlink("c:\\opentest.txt");
	if (ret < 0)
		{
		INFO_PRINTF1(_L("unlink() for the file failed\n"));
		return ret;
		}
	INFO_PRINTF1(_L("open a file in read-write mode\n"));
	int fd2 = open("c:\\opentest.txt",O_CREAT, 0600);
	if (fd2 < 0)
		{
		INFO_PRINTF1(_L("open() for the file in read-write mode failed\n"));
		return fd2;
		}	
	INFO_PRINTF1(_L("open the existing file with O_CREAT and O_EXCL mode\n"));
	int fd3 = open("c:\\opentest.txt",O_CREAT|O_EXCL);
	if (fd3 < 0)
		{
		INFO_PRINTF1(_L("open the existing file with O_CREAT|O_EXCL mode failed\n"));
		if (errno != EEXIST)
			{
			INFO_PRINTF1(_L("open() doesn't set errno correctly for the existing file opened with O_CREAT|O_EXCL mode\n"));
			return fd3;
			}
		INFO_PRINTF2(_L("open() set errno correctly for the existing file opened with O_CREAT|O_EXCL mode to %d\n"), errno);
		}
	INFO_PRINTF1(_L("open the existing file with O_CREAT,O_EXCL and O_APPEND mode\n"));
	int fd4 = open("c:\\opentest.txt",O_CREAT|O_EXCL|O_APPEND);
	if (fd4 < 0)
		{
		INFO_PRINTF1(_L("open the existing file with O_CREAT|O_EXCL|O_APPEND mode failed\n"));
		if (errno != EEXIST)
			{
			INFO_PRINTF1(_L("open() doesn't set errno correctly for the existing file opened with O_CREAT|O_EXCL|O_APPEND mode\n"));
			return fd4;
			}
		INFO_PRINTF2(_L("open() set errno correctly for the existing file opened with O_CREAT|O_EXCL mode to %d\n"), errno);
		}
	INFO_PRINTF1(_L("open the existing file with O_CREAT,O_EXCL and O_TRUNC mode\n"));
	int fd5 = open("c:\\opentest.txt",O_CREAT|O_EXCL|O_TRUNC);
	if (fd5 < 0)
		{
		INFO_PRINTF1(_L("open the existing file with O_CREAT|O_EXCL|O_TRUNC mode failed\n"));
		if (errno != EEXIST)
			{
			INFO_PRINTF1(_L("open() doesn't set errno correctly for the existing file opened with O_CREAT|O_EXCL|O_TRUNC mode\n"));
			return fd5;
			}
		INFO_PRINTF2(_L("open() set errno correctly for the existing file opened with O_CREAT|O_EXCL|O_TRUNC mode to %d\n"), errno);
		}
	ret = close(fd2);
	if (ret < 0)
		{
		INFO_PRINTF1(_L("close() for the file fd failed\n"));
		return ret;
		}
	ret = unlink("c:\\opentest.txt");
	if (ret < 0)
		{
		INFO_PRINTF1(_L("unlink() for the file failed\n"));
		return ret;
		}
	return 0;
	}
TInt CTestSyscalls :: Fstat1() 
	{
	int fd ,ret;
	char* file = "C:\\Test\\Fstatt57.txt";
	if((fd = open(file , O_CREAT | O_RDWR | O_TRUNC , 0666)) < 0) 
		{
   		_LIT(Kerr , "Failed to open specified file ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	ret = fstat(fd,NULL);
	if(ret == -1)
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	}

TInt CTestSyscalls :: Utimes1()
	{
    
    TBuf8<50>string ;
    TPtrC String ;
 
    _LIT( KTsyscalls, "Tsyscalls" );
    INFO_PRINTF1(KTsyscalls) ;
    _LIT( Kutimes, "In Utimes" );
    INFO_PRINTF1(Kutimes);
   
	struct timeval tim[2] ;

	tim[0].tv_sec = 0 ; tim[0].tv_usec = 0;
	tim[1].tv_sec = 0 ; tim[1].tv_usec = 0;

	if(utimes(NULL,tim) < 0 ) 
		{
     
		_LIT(Kstmt, "Utime Successful") ;
		INFO_PRINTF1(Kstmt) ;
		return KErrNone ;
		}
	return KErrGeneral ;
	}
TInt CTestSyscalls :: Mkdir_test1()
	{
	
	TInt ret,ret1;
     
    ret1 = mkdir(NULL, S_IWUSR) ;
    
    if(ret1 == -1 ) 
    	{
    	_LIT(Kstmt , "mkdir success") ;
    	INFO_PRINTF1(Kstmt);
        ret=  KErrNone ;
    	}
	else 
		{
    	 _LIT(Kerr , "Expected and the return values are different ") ;
    	 ERR_PRINTF1(Kerr) ;
    	 ret = KErrGeneral; 
		}
    return ret;
    }

TInt CTestSyscalls :: Chmod_test()
	{
	TInt ret1;
	ret1 = chmod(NULL, S_IWUSR) ;
    
    if(ret1 == -1) 
		{
		_LIT(Kstmt, "chmod Success") ;
      	INFO_PRINTF1(Kstmt) ;
      	return KErrNone ;
		}
    
    else
    	{
    	_LIT(Kerr , "Failed to change the modes of the file ") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	}
TInt CTestSyscalls :: Chdir1()
	{
	TInt ret ;
   
	ret = chdir(NULL) ;
  
	if(ret  == -1)   
		{
		_LIT(Kerr , "Chdir Success") ;
   	  	INFO_PRINTF1(Kerr) ;
   	  	return KErrNone;
   		}
	else
		{
		_LIT(Kerr, "Expected and return value are different") ;
 		INFO_PRINTF1(Kerr);
		return KErrGeneral ;
		}
    }

TInt CTestSyscalls :: Rmdir2()
	{
	    
    if(rmdir(NULL)  < 0) 
		{
		_LIT(Kerr , "rmdir success") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrNone ;
		}
    return KErrGeneral ;
	}

TInt CTestSyscalls :: Rename_test()
	{
	int ret; 
	ret =  rename(NULL,"C:\\Test\\Fstatt57.txt") ;
       
	if(ret<1)
		{
		_LIT(Kerr , "rename success") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrNone ;
		}
    return KErrGeneral ;
	}
TInt CTestSyscalls :: Rename3()
	{
	int ret; 
	ret =  rename("C:\\Test\\Fstatt57.txt",NULL) ;
       
	if(ret<1)
		{
		_LIT(Kerr , "rename success") ;
    	INFO_PRINTF1(Kerr) ;
    	return KErrNone ;
		}
    return KErrGeneral ;
    
	}

TInt CTestSyscalls :: Creat1(  )
	{
    int fd ;
   
    if((fd = creat(NULL,  0666))  < 0) 
		{
        _LIT(Kstmt , "creat success") ;
        INFO_PRINTF1(Kstmt) ;
    	return KErrNone ;
		}
    close(fd) ;
    return KErrGeneral ;
    }

TInt CTestSyscalls :: ReadV1()
	{
	char Buf1[20]  ;    //= "Hello World";
	char Buf2[20]  ;     //=  "By ritche" ; 
	struct iovec Iovec[2] ;
	TInt ret=KErrNone;
	Iovec[0].iov_base = Buf1 ;
	Iovec[0].iov_len = sizeof("Hello World")  ;
	Iovec[1].iov_base = Buf2 ;
	Iovec[1].iov_len = sizeof("By ritche") ;
   
   	char *file = "c:\\logs\\temp.txt";
	TInt fd = open(file , O_CREAT | O_RDWR  , 0666) ;
   
	if(fd < 0) 
		{
		_LIT(Kerr , "failed to open the given file ") ;
		INFO_PRINTF1(Kerr) ;
		ret = KErrGeneral ;	
		}
   
   
	if(readv(fd , Iovec , -1) < 0 && errno == EINVAL ) 
		{
		_LIT(Kstmt , "readv success") ;
		INFO_PRINTF1(Kstmt) ;
		ret = KErrNone ;	
		}
   
	else
		{
   		_LIT(Kerr, "readv failure") ;
   		INFO_PRINTF1(Kerr) ;
   		ret = KErrGeneral ;
		}
  	return ret;

	}

 TInt CTestSyscalls :: Utimes2()
 	{
     
     char mbcbuf[64];
     mbcbuf[0] = (unsigned char)0x3A;// colon :
    	
     _LIT( KTsyscalls, "Tsyscalls" );
     INFO_PRINTF1(KTsyscalls) ;
     _LIT( Kutimes, "In Utimes" );
     INFO_PRINTF1(Kutimes);
    
 	struct timeval tim[2] ;
 
 	tim[0].tv_sec = 0 ; tim[0].tv_usec = 0;
 	tim[1].tv_sec = 0 ; tim[1].tv_usec = 0;
 
 	if(utimes(mbcbuf,tim) < 0 ) 
 		{
      
 		_LIT(Kstmt, "Utime Successful") ;
 		INFO_PRINTF1(Kstmt) ;
 		return KErrNone ;
 		}
 	return KErrGeneral ;
 	}
 	
 TInt CTestSyscalls :: Stat_test() 
 	{
 	
 	char mbcbuf[64];
    	int result,ret=KErrNone;
    	mbcbuf[0] = (unsigned char)0x3A;// colon :
    	
 	struct stat statbuf ;
    	result = stat(mbcbuf , &statbuf) ;
    	
    	if(result<0)
    		{
    		_LIT(Kstmt, "stat Successful") ;
 		INFO_PRINTF1(Kstmt) ;
 		}
    	else
    		{	
    		_LIT(Kstmt, "stat failure") ;
 		INFO_PRINTF1(Kstmt) ;
 		ret = KErrGeneral ;
    		}		   
 	return ret;
 	}
 
 TInt CTestSyscalls :: Mkdir_test2()
 	{
 	
 	TInt ret,ret1;
     char mbcbuf[64];
     mbcbuf[0] = (unsigned char)0x3A;// colon :
     ret1 = mkdir(mbcbuf, S_IWUSR) ;
     
     if(ret1 == -1 ) 
     	{
     	_LIT(Kstmt , "mkdir success") ;
     	INFO_PRINTF1(Kstmt);
         ret=  KErrNone ;
     	}
 	else 
 		{
     	 _LIT(Kerr , "Expected and the return values are different ") ;
     	 ERR_PRINTF1(Kerr) ;
     	 ret = KErrGeneral; 
 		}
     return ret;
     }
 
 TInt CTestSyscalls :: Chmod2()
 	{
 	TInt ret1;
 	char mbcbuf[64];
 	mbcbuf[0] = (unsigned char)0x3A;// colon :
 	ret1 = chmod(mbcbuf, S_IWUSR) ;
     
     if(ret1 == -1) 
 		{
 		_LIT(Kstmt, "chmod Success") ;
       	INFO_PRINTF1(Kstmt) ;
       	return KErrNone ;
 		}
     
     else
     	{
     	_LIT(Kerr , "Failed to change the modes of the file ") ;
 		INFO_PRINTF1(Kerr) ;
 		return KErrGeneral ;
 		}
 	}
 TInt CTestSyscalls :: Chdir2()
 	{
 	TInt ret ;
 	char mbcbuf[64];
 	mbcbuf[0] = (unsigned char)0x3A;// colon :
 	ret = chdir(mbcbuf) ;
   
 	if(ret  == -1)   
 		{
 		_LIT(Kerr , "Chdir Success") ;
    	  	INFO_PRINTF1(Kerr) ;
    	  	return KErrNone;
    		}
 	else
 		{
 		_LIT(Kerr, "Expected and return value are different") ;
  		INFO_PRINTF1(Kerr);
 		return KErrGeneral ;
 		}
     }
 
 TInt CTestSyscalls :: Rename4()
 	{
 	int ret; 
 	char mbcbuf[64];
 	mbcbuf[0] = (unsigned char)0x3A;// colon :
 	ret =  rename(mbcbuf,"C:\\Test\\rename1.txt") ;
        
 	if(ret<0)
 		{
 		_LIT(Kerr , "rename success") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrNone ;
 		}
     return KErrGeneral ;
     
 	}
 TInt CTestSyscalls :: Rename5()
 	{
 	int ret; 
 	char mbcbuf[64];
 	mbcbuf[0] = (unsigned char)0x3A;// colon :
 	ret =  rename("C:\\Test\\rename1.txt",mbcbuf) ;
        
 	if(ret<0)
 		{
 		_LIT(Kerr , "rename success") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrNone ;
 		}
     return KErrGeneral ;
     
 	}
 TInt CTestSyscalls :: Rmdir3()
 	{
 	char mbcbuf[64];
 	mbcbuf[0] = (unsigned char)0x3A;// colon :

     if(rmdir(mbcbuf)  < 0) 
 		{
 		_LIT(Kerr , "rmdir success") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrNone ;
 		}
     return KErrGeneral ;
 	}
 
 TInt CTestSyscalls :: read5()
 	{
  	
  	int fd; 
     int ret=KErrNone;
     char Buf[20] ;
     if((fd = open("C:\\Test\\rename1.txt" , O_CREAT | O_WRONLY , 0666))  < 0) 
 		{
 		_LIT(Kopen , "Failed to open file " ) ;
 		INFO_PRINTF1(Kopen) ;
 		return KErrGeneral  ;
 		}
     
     if(read(fd , Buf , 0)  == 0) 
 		{
     	if(errno != EBADF) 
     		{
     		_LIT(Kstmt , "read success \n") ;
     		INFO_PRINTF1(Kstmt) ;
     		ret = KErrNone ;
     		}
 		}
     
 	else   
 		{
 		_LIT(Kerr , "Read failed") ;
 		INFO_PRINTF1(Kerr) ;
 		ret = KErrGeneral ;
 		}
     
     return ret ; 
 
 	}
 	
 
//---------------------------------------------------------------------------
//Stat Test cases 
//---------------------------------------------------------------------------


TInt CTestSyscalls :: Stat3() 
	{

		struct stat buf;
       		if(stat("Y:\\"  , &buf) < 0 )	 //return -1,if memory card not mounted
       
       		printf("Failed to stat Y:\\n");          
          
       		printf("Stat system call succeeded \n"); 
       		getchar();
       		return 0; 
	}



