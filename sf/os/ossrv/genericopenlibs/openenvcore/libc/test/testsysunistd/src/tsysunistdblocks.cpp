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



#include "tsysunistd.h"

// -----------------------------------------------------------------------------
//Function Name :ftruncate_fileL
//API Tested :ftruncate
//API Description:Truncates the given file (using file descriptor) to the 
//length specified.
// -----------------------------------------------------------------------------
    
TInt CTestSysunistd::ftruncate_file( )
    {
    // Print to UI
	_LIT( KFailed, "Failed" );
    _LIT( KFtruncate, "In ftruncate" );
    // Print to log file
    INFO_PRINTF1( KFtruncate );
	char array[KMAXCHARS];
	
	char ch = 'h';
    TInt i = 0;
    TInt retVal=KErrGeneral;
    TInt retVal2;
    off_t retSize;
    TInt retrn = KErrGeneral;
    
    struct stat buf;
    
	for(i = 0; i < KMAXCHARS - 1; i++)
		{
		array[i] = ch;
		}
	
	array[i] = '\0';
	
	int fp = open("C:\\test_ftruncate.txt", O_RDWR|O_CREAT);
	int fd;
	if(fp != -1)
		{
		INFO_PRINTF2(_L("fp: %d"),fp );
		size_t size = write( fp, array, KMAXCHARS );
		INFO_PRINTF2(_L("Size of file after writing : %d"),size );
		INFO_PRINTF2(_L("fp: %d"),fp);
		if((fp != -1) && (size  == KMAXCHARS))
			{
			int sizebytwo = size/2;
			INFO_PRINTF2(_L("sizebytwo before ftruncate %d"),sizebytwo );
			retVal = ftruncate( fp, sizebytwo );
			INFO_PRINTF2(_L("retVal from ftruncate %d"),retVal);
			close(fp);
			}
		else
			{
			INFO_PRINTF1( KFailed );
			INFO_PRINTF1(_L("else for retVal couldn open file"));
			retrn = KErrGeneral;
			}
		fd = open("C:\\test_ftruncate.txt", O_RDONLY);
		if((fd != -1) && (!retVal))
			{
			INFO_PRINTF2(_L("fd: %d"),fd );
			retVal2 = fstat( fd, &buf );
			if ( !retVal2 )
				{
				retSize = buf.st_size;
				if( retSize == (size/2 ) )
					{
					INFO_PRINTF2(_L("Passed -> retSize = %d"),retSize);
					INFO_PRINTF2(_L("The errno value is %d\n"),errno);
					retrn = KErrNone;
					}
				else
					{
					INFO_PRINTF2(_L("Failed -> retSize = %d"),retSize);
					INFO_PRINTF2(_L("The errno value is %d\n"),errno);
					retrn = KErrGeneral;
					}
				}
			else
				{
				INFO_PRINTF1( KFailed );
				INFO_PRINTF1(_L("fstat failed"));
				retrn = KErrGeneral;
				}
			}
			else
				{
				INFO_PRINTF1( KFailed );
				INFO_PRINTF1(_L("Couldn open file after truncate"));
				INFO_PRINTF2(_L("The errno value is %d\n"),errno);
				retrn = KErrGeneral;
				}	
			close(fd);
			int ret = remove("C:\\test_ftruncate.txt");			
			}
	else
		{
		INFO_PRINTF1(_L("Couldn create/open"));
		INFO_PRINTF2(_L("The errno value is %d\n"),errno);
		INFO_PRINTF1( KFailed );
		retrn = KErrGeneral;
		}
	
    return retrn ;
    }


// -----------------------------------------------------------------------------
//Function Name :ftruncate_dirL
//API Tested :ftruncate
//API Description:Truncates the given file (using file descriptor) to the 
//length specified.
// -----------------------------------------------------------------------------
  
TInt CTestSysunistd::ftruncate_dir(  )
    {

    // Print to UI
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KFtruncate, "In ftruncate" );
    INFO_PRINTF1( KFtruncate );
	TInt retrn = KErrGeneral;

	int mkdr = mkdir("C:\\test1", S_IWUSR);
	if( !mkdr )
		{
		int fp = open("C:\\test1", O_RDONLY);
		if( fp != -1 )
			{
			TInt retVal;
			size_t size = 10;

			retVal = ftruncate( fp, size );

			int cls = close(fp);
			if( !cls )
				{
				int rmdr = rmdir("C:\\test1");	
				}
			if( !retVal )
				{
				INFO_PRINTF1( KFailed );
				retrn = KErrGeneral;
				}
			else
				{
				INFO_PRINTF1( KPassed);
				retrn = KErrNone;
				}
			}
		else
			{
			INFO_PRINTF1( KFailed );
			INFO_PRINTF2(_L("The errno value is %d\n"),errno);
			retrn = KErrGeneral;
			}
		}
	else
		{
		INFO_PRINTF1( KFailed );
		INFO_PRINTF2(_L("The errno value is %d\n"),errno);
		retrn = KErrGeneral;
		}
    return retrn ;
    }

// -----------------------------------------------------------------------------
//Function Name :ftruncate_numL
//API Tested :ftruncate
//API Description:Truncates the given file (using file descriptor) to the 
//length specified.
// -----------------------------------------------------------------------------
  

TInt CTestSysunistd::ftruncate_num(  )
    {

    // Print to UI
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KFtruncate, "In ftruncate" );
    INFO_PRINTF1( KFtruncate );
	TInt retrn = KErrGeneral;

	TInt retVal;
	size_t size = 10;

	retVal = ftruncate( 3, size );
	
	if( !retVal )
		{
		INFO_PRINTF1( KFailed );
		retrn = KErrGeneral;
		}
	else
		{
		INFO_PRINTF1( KPassed );
		retrn = KErrNone;
		}
    return retrn ;
    }

// -----------------------------------------------------------------------------
//Function Name :ftruncate_sizL
//API Tested :ftruncate
//API Description:Truncates the given file (using file descriptor) to the 
//length specified.
// -----------------------------------------------------------------------------
  

TInt CTestSysunistd::ftruncate_siz(  )
    {

    // Print to UI
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KFtruncate, "In ftruncate" );
    INFO_PRINTF1( KFtruncate );
	
	char array[KMAXCHARS];
	
	char ch = 'h';
    TInt i = 0;
    TInt retVal;
    TInt retrn = KErrGeneral;
    
    for(i = 0; i < KMAXCHARS - 1; i++)
		{
		array[i] = ch;
		}
	
	array[i] = '\0';
	
	int fp = open("C:\\test_siz.txt", O_RDWR|O_CREAT);
	if(fp != -1)
		{
		size_t size = write( fp, array, KMAXCHARS );
		retVal = ftruncate( fp, size*2 );
		if(retVal == -1 )
			{
			INFO_PRINTF1( KFailed );
			retrn = KErrGeneral;
			}
		else
			{
			INFO_PRINTF1( KPassed );
			retrn = KErrNone;
			}		
		close(fp);
		int ret = remove("C:\\test_siz.txt");
		}
	else
		{
		INFO_PRINTF1( KFailed );
		INFO_PRINTF2(_L("The errno value is %d\n"),errno);
		retrn = KErrGeneral;
		}
	return retrn ;
	}
// -----------------------------------------------------------------------------
//Function Name :fchdir_dirL
//API Tested :fchdir
//API Description:The fchdir() function shall be equivalent to chdir() except that
//the directory that is to be the new current working directory is
//specified by the file descriptor fildes.
// -----------------------------------------------------------------------------
      
TInt CTestSysunistd::fchdir_dir(  )
    {

    // Print to UI
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KFchdir, "In fchdir" );
    INFO_PRINTF1( KFchdir );
	TInt retrn = KErrGeneral;

	TInt retVal;

	int mkdr = mkdir("C:\\test_dir", S_IWUSR);
	if( !mkdr )
		{
		int opn = open("C:\\test_dir", O_RDONLY);
		if( opn != -1 )
			{
			retVal = fchdir( opn );
			
			if( !retVal )
				{
				INFO_PRINTF1( KPassed );
				retrn = KErrNone ;
				}
			else
				{
				INFO_PRINTF1( KFailed );
				retrn = KErrGeneral;
				}
			int cls = close(opn);
			}
		else
			{
			INFO_PRINTF1( KFailed );
			INFO_PRINTF2(_L("The errno value is %d\n"),errno);
			retrn = KErrGeneral;
			}
		rmdir("C:\\test_dir");	
		
		}
	else
		{
		INFO_PRINTF1( KFailed );
		INFO_PRINTF2(_L("The errno value is %d\n"),errno);
		retrn = KErrGeneral;
		}
    return retrn ;
    }
// -----------------------------------------------------------------------------
//Function Name :fchdir_fileL
//API Tested :fchdir
//API Description:The fchdir() function shall be equivalent to chdir() except that
//the directory that is to be the new current working directory is
//specified by the file descriptor fildes.
// -----------------------------------------------------------------------------

TInt CTestSysunistd::fchdir_file(  )
    {

    // Print to UI
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KFchdir, "In fchdir" );
    INFO_PRINTF1( KFchdir );
	TInt retrn = KErrGeneral;

	TInt retVal;

	int opn = open("C:\\test_fchdir.txt", O_RDWR|O_CREAT);
	if( opn != -1 )
		{

		retVal = fchdir( opn );
		
		if( !retVal )
			{
			INFO_PRINTF1( KFailed );
			retrn = KErrGeneral;
			}
		else
			{
			INFO_PRINTF1( KPassed );
			retrn = KErrNone;
			}
		close( opn );
		int ret = remove("C:\\test_fchdir.txt");
		}
	else
		{
		INFO_PRINTF1( KFailed );
		retrn = KErrGeneral;
		}
	return retrn ;
    }
// -----------------------------------------------------------------------------
//Function Name :fchdir_numL
//API Tested :fchdir
//API Description:The fchdir() function shall be equivalent to chdir() except that
//the directory that is to be the new current working directory is
//specified by the file descriptor fildes.
// -----------------------------------------------------------------------------
    
TInt CTestSysunistd::fchdir_num(  )
    {

    // Print to UI
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

   _LIT( KFchdir, "In fchdir" );
    // Print to log file
    INFO_PRINTF1( KFchdir );
	TInt retrn = KErrGeneral;

	TInt retVal;
	
	retVal = fchdir( 2 );
	
	if( !retVal )
		{
		INFO_PRINTF1( KFailed );
		retrn = KErrGeneral;
		}
	else
		{
		INFO_PRINTF1( KPassed );
		retrn = KErrNone;
		}
	return retrn ;
    }
// -----------------------------------------------------------------------------
//Function Name :fdatasync_fileL
//API Tested :fdatasync
//API Description:The fdatasync() function shall force all currently queued I/O 
//operations associated with the file indicated by file descriptor fildes to the 
//synchronized I/O completion state.
// -----------------------------------------------------------------------------
 

TInt CTestSysunistd::fdatasync_file( )
	{
	     
	int fd ;
	TBuf8<50> string ;
	TPtrC String ;
	TInt Expected ;
	_LIT( KString, "Parameter1" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res)
		{
	    _LIT(Kerr , " Failed to  read input file name from ini file ") ;
	    INFO_PRINTF1(Kerr) ;
	    return KErrGeneral ;
		}

	string.Copy(String) ;
   
	char *file = (char *) string.Ptr() ;
	file[string.Length()] = '\0' ;
   
   
   	_LIT( KExpected, "Parameter2" );
	res = GetIntFromConfig(ConfigSection(), KExpected, Expected);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read expected value from ini file.") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
   
	_LIT( KFdatasync, "In fdatasync" );
    INFO_PRINTF1( KFdatasync );
   
	if(Expected < 0) 
		{    
   		fdatasync(-1) ;
   	  
   		if(errno != EBADF)   
   			{
   	   		_LIT(Kerr , "Failed to set errno value ") ;
   	   		INFO_PRINTF1(Kerr) ;
   	   		return KErrGeneral ;
   			}
   		return KErrNone ;
		}
  
	if((fd = open(file , O_CREAT | O_RDWR , 0666))  < 0) 
		{
		_LIT(Kopen , "Failed  to create file ") ;
        INFO_PRINTF1(Kopen) ;
        return KErrGeneral ;
		}
     
	if(fdatasync(fd) < 0) 
		{
   		_LIT(Kerr , "Fdatasync failed ") ;
   		INFO_PRINTF1(Kerr) ;
   		return KErrGeneral ;
		}
	close(fd);
	remove(file);
	return KErrNone 	;
	}
// -----------------------------------------------------------------------------
//Function Name :getpagesizeL
//API Tested :getpagesize
//API Description:The function getpagesize() returns the number of bytes in a 
//page, where "page" defines the unit of data that is brought into memory at once
//(ex: files are mapped in page-sized units).
// -----------------------------------------------------------------------------
 
TInt CTestSysunistd::getpagesizeL(  )
    {

    // Print to UI
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KGetpagesize, "In getpagesize" );
    INFO_PRINTF1( KGetpagesize );
	
	int retVal = 0;
	TInt retrn;
	
	retVal = getpagesize();
	
	if( retVal >= 0)
		{
	    INFO_PRINTF1( KPassed );
		retrn = KErrNone;
		}
	else
		{
		INFO_PRINTF1( KFailed );
		retrn = KErrGeneral;
		}	
	return retrn ;
    }

// -----------------------------------------------------------------------------
//Function Name :getpriorityL
//API Tested :getpriority
//API Description:This function checks if the which and who parameters sent 
//are 'the process priority' and 'the current process' respectively,i.e. PROC_PROCESS 
//and 0 respectively. The function getpriority() shall ignore the values PRIO_PGRP,
//PRIO_USER as the argument 'which' and any other value other than 0 for 'who' shall
//not be considered.
// -----------------------------------------------------------------------------
 
TInt CTestSysunistd::getpriorityL(  )
    {

    // Print to UI
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KGetpriority, "In getpriority" );
    INFO_PRINTF1( KGetpriority );
	
	int retVal = 0;
	TInt retrn;
	errno = 0;
	retVal = getpriority(PRIO_PROCESS, 0);
		
	if((retVal == -1) && (errno))
		{
		INFO_PRINTF1( KFailed );
		retrn = KErrGeneral;
	    }
	else
		{
		INFO_PRINTF1( KPassed );
		retrn = KErrNone;
		}	

    return retrn ;

    }
// -----------------------------------------------------------------------------
//Function Name :getpriority_otherL
//API Tested :getpriority
//API Description:This function checks if the which and who parameters sent 
//are 'the process priority' and 'the current process' respectively,i.e. PROC_PROCESS 
//and 0 respectively. The function getpriority() shall ignore the values PRIO_PGRP,
//PRIO_USER as the argument 'which' and any other value other than 0 for 'who' shall
//not be considered.
// -----------------------------------------------------------------------------
 
    
TInt CTestSysunistd::getpriority_other( )
    {

    // Print to UI
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KGetpriority, "In getpriority" );
    INFO_PRINTF1( KGetpriority );
	
	int retVal = 0;
	TInt retrn;
	errno = 0;
	TInt which;
	TInt who;
	_LIT( Kwhich, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), Kwhich, which);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read first parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	_LIT( Kwho, "Parameter2" );
	res = GetIntFromConfig(ConfigSection(), Kwho, who);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read second parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}

	retVal = getpriority(which, who);	
	if((retVal == -1) && (errno))
		{
		INFO_PRINTF1( KPassed );
		retrn = KErrNone;
		}
	else
		{
		INFO_PRINTF1( KFailed );
		retrn = KErrGeneral;
		}	
    return retrn ;

    }
// -----------------------------------------------------------------------------
//Function Name :setpriority_getL
//API Tested :setpriority
//API Description:This function setpriority() checks if the which and who 
//parameters sent are 'the process priority' and 'the current process' respectively,
//i.e. PROC_PROCESS and 0 respectively.The function setpriority() shall ignore the 
//values PRIO_PGRP, PRIO_USER as the argument 'which' and any other value other than 
//0 for 'who' shall not be considered. 
// -----------------------------------------------------------------------------


TInt CTestSysunistd::setpriority_get( )
    {

    // Print to UI
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KSetpriority, "In setpriority" );
    INFO_PRINTF1( KSetpriority );
    TInt prio_high, prio_low;
	int retVal, retSet;
	int retVal2 = -1, i;
	TInt retrn=KErrNone;
	errno = 0;
	
	retVal = getpriority(PRIO_PROCESS, 0);
	
	_LIT( Kprio_high, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), Kprio_high, prio_high);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read first parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	_LIT( Kprio_low, "Parameter2" );
	res = GetIntFromConfig(ConfigSection(), Kprio_low, prio_low);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read second parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	for(i = prio_low;i> prio_high;i--)
		{
		if((retVal == -1) && (errno))
			{
			INFO_PRINTF1( KFailed );
			retrn = KErrGeneral;
		    }
		else
			{
			retSet = setpriority(PRIO_PROCESS, 0, retVal+i);
			if(!retSet)
				{
				retVal2 = getpriority(PRIO_PROCESS, 0);
				if(retVal2 == retVal+i)
					{
					INFO_PRINTF1( KPassed );
					}
				else
					{
					INFO_PRINTF1( KFailed );
					retrn = KErrGeneral;			
					}	
				}
			else
				{
				INFO_PRINTF1( KFailed );
				retrn = KErrGeneral;
				}
			}	
		}
	retrn = KErrNone;	
    return retrn ;
    }
    
// -----------------------------------------------------------------------------
//Function Name :setpriority_highL
//API Tested :setpriority
//API Description:This function setpriority() checks if the which and who 
//parameters sent are 'the process priority' and 'the current process' respectively,
//i.e. PROC_PROCESS and 0 respectively.The function setpriority() shall ignore the 
//values PRIO_PGRP, PRIO_USER as the argument 'which' and any other value other than 
//0 for 'who' shall not be considered. 
// -----------------------------------------------------------------------------

TInt CTestSysunistd::setpriority_high( )
    {
	_LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KSetpriority, "In setpriority" );
    INFO_PRINTF1( KSetpriority );
	int retVal = 0, i;
	TInt retrn;
	errno = 0;
	TInt prio_high_end;
	TInt prio_high_start;

	_LIT( Kprio_high_start, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), Kprio_high_start, prio_high_start);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read first parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	_LIT( Kprio_high_end, "Parameter2" );
	res = GetIntFromConfig(ConfigSection(), Kprio_high_end, prio_high_end);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read second parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	for (i = prio_high_start; i >= prio_high_end; i--)
		{
		retVal = setpriority(PRIO_PROCESS, 0, i);
		if((retVal == -1) && (errno == ENOSYS))
			{
		    INFO_PRINTF1( KPassed );
			}
		else
			{
			INFO_PRINTF1( KFailed );
			retrn = KErrGeneral;
			}	
		}
	retrn = KErrNone;
    return retrn ;
    }
    
// -----------------------------------------------------------------------------
//Function Name :setpriority_other_lowerL
//API Tested :setpriority
//API Description:This function setpriority() checks if the which and who 
//parameters sent are 'the process priority' and 'the current process' respectively,
//i.e. PROC_PROCESS and 0 respectively.The function setpriority() shall ignore the 
//values PRIO_PGRP, PRIO_USER as the argument 'which' and any other value other than 
//0 for 'who' shall not be considered. 
// -----------------------------------------------------------------------------

TInt CTestSysunistd::setpriority_other_lower( )
    {
	_LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KSetpriority, "In setpriority" );
    
    INFO_PRINTF1( KSetpriority );
	int retVal = 0, i;
	TInt retrn;
	errno = 0;
	TInt prio_low_end;
	TInt prio_low_start;
	
	_LIT( Kprio_low_start, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), Kprio_low_start, prio_low_start);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read first parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	_LIT( Kprio_low_end, "Parameter2" );
	res = GetIntFromConfig(ConfigSection(), Kprio_low_end, prio_low_end);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read second parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	for (i = prio_low_start; i <= prio_low_end; i++)
		{
		retVal = setpriority(PRIO_PROCESS, 0, i);
		if(retVal == 0)
			{
		    int retGet = getpriority(PRIO_PROCESS, 0);
		    if (retGet == LOWER_LIMIT_PRIORITY_LOWEST )
		    	{
		    	INFO_PRINTF1( KPassed );
		    	}
			else
				{
				INFO_PRINTF1( KFailed );
				retrn = KErrGeneral;
				}	
		    }
		else
			{
			INFO_PRINTF1( KFailed );
			retrn = KErrGeneral;
			}	
		}
	retrn = KErrNone;
	return retrn ;
    }
    
// -----------------------------------------------------------------------------
//Function Name :setpriority_other_higherL
//API Tested :setpriority
//API Description:This function setpriority() checks if the which and who 
//parameters sent are 'the process priority' and 'the current process' respectively,
//i.e. PROC_PROCESS and 0 respectively.The function setpriority() shall ignore the 
//values PRIO_PGRP, PRIO_USER as the argument 'which' and any other value other than 
//0 for 'who' shall not be considered. 
// -----------------------------------------------------------------------------

TInt CTestSysunistd::setpriority_other_higher( )
    {
	_LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KSetpriority, "In setpriority" );

    INFO_PRINTF1( KSetpriority );
	int retVal = 0, i;
	TInt retrn;
	errno = 0;
	TInt prio_high_end;
	TInt prio_high_start;
	
	_LIT( Kprio_high_start, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), Kprio_high_start, prio_high_start);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read first parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 	_LIT( Kprio_high_end, "Parameter2" );
	res = GetIntFromConfig(ConfigSection(), Kprio_high_end, prio_high_end);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read second parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	for (i = prio_high_start; i >= prio_high_end; i--)
		{
		retVal = setpriority(PRIO_PROCESS, 0, i);
		if(retVal == 0)
			{
		    int retGet = getpriority(PRIO_PROCESS, 0);
		    if (retGet == UPPER_LIMIT_PRIORITY_ABOVE_NORMAL )
		    	{
		    	INFO_PRINTF1( KPassed );
		    	}
			else
				{
				INFO_PRINTF1( KFailed );
				retrn = KErrGeneral;
				}	
		    }
		else
			{
			INFO_PRINTF1( KFailed );
			retrn = KErrGeneral;
			}	
		}
	retrn = KErrNone;
    return retrn ;
    }

// -----------------------------------------------------------------------------
//Function Name :setpriority_other
//API Tested :setpriority
//API Description:This function setpriority() checks if the which and who 
//parameters sent are 'the process priority' and 'the current process' respectively,
//i.e. PROC_PROCESS and 0 respectively.The function setpriority() shall ignore the 
//values PRIO_PGRP, PRIO_USER as the argument 'which' and any other value other than 
//0 for 'who' shall not be considered. 
// -----------------------------------------------------------------------------
    
TInt CTestSysunistd::setpriority_other( )
    {

    // Print to UI
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KSetpriority, "In setpriority" );

    INFO_PRINTF1( KSetpriority );
	
	int retVal = 0, value;
	TInt retrn;
	errno = 0;
	TInt which;
	TInt who;
	
	_LIT( Kwhich, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), Kwhich, which);
	if(!res)
		{
		_LIT(Kerr , "Failed to read first parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	_LIT( Kwho, "Parameter2" );
	res = GetIntFromConfig(ConfigSection(), Kwho, who);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read second parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 _LIT( Kvalue, "Parameter3" );
	 res = GetIntFromConfig(ConfigSection(), Kvalue, value);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read third parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	retVal = setpriority(which, who, value);	
	if((retVal == -1) && (errno))
		{
	    INFO_PRINTF1( KPassed );
		retrn = KErrNone;
		}
	else
		{
		INFO_PRINTF1( KFailed );
		retrn = KErrGeneral;
		}	
    return retrn ;
    }
// -----------------------------------------------------------------------------
//Function Name :nice_less_than_lowestL
//API Tested :nice
//API Description:The function nice() increases or decreases the priority value 
//by adding the value 'incr' to the current priority.It internally uses getpriority and 
//setpriority to set the priority to new value.
// -----------------------------------------------------------------------------
    
TInt CTestSysunistd::nice_less_than_lowest( )
    {
	_LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KSetpriority, "In setpriority" );

    INFO_PRINTF1( KSetpriority );
	int retVal = 0, i;
	TInt retrn;
	errno = 0;
	TInt prio_low_end;
	TInt prio_low_start;
	
	_LIT( Kprio_low_start, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), Kprio_low_start, prio_low_start);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read first parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 _LIT( Kprio_low_end, "Parameter2" );
	 res = GetIntFromConfig(ConfigSection(), Kprio_low_end, prio_low_end);
	 if(!res)
		{
	 	_LIT(Kerr , "Failed to read second parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	errno = 0;
	for (i = prio_low_start; i >= prio_low_end; i--)
		{
		retVal = nice(i);
		if((retVal == -1)&&(errno))
			{
			INFO_PRINTF1( KFailed );
			retrn = KErrGeneral;
			}
		else
			{
	    	INFO_PRINTF1( KPassed );
		    }
		}
	retrn = KErrNone;
	return retrn ;
    }

// -----------------------------------------------------------------------------
//Function Name :nice_within_rangeL
//API Tested :nice
//API Description:The function nice() increases or decreases the priority value 
//by adding the value 'incr' to the current priority.It internally uses getpriority and 
//setpriority to set the priority to new value.
// -----------------------------------------------------------------------------
      
TInt CTestSysunistd::nice_within_range( )
    {
	_LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KSetpriority, "In nice" );
    
    INFO_PRINTF1( KSetpriority );
	int retVal = 0, i;
	TInt retrn;
	errno = 0;
	TInt prio_end;
	TInt prio_start;
	
	_LIT( Kprio_start, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), Kprio_start, prio_start);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read first parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 _LIT( Kprio_end, "Parameter2" );
	 res = GetIntFromConfig(ConfigSection(), Kprio_end, prio_end);
	 if(!res)
		{
	 	_LIT(Kerr , "Failed to read second parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	errno = 0;
	for (i = prio_start; i >= prio_end; i--)
		{
		retVal = nice(i);
		if((retVal == -1)&&(errno))
			{
			INFO_PRINTF1( KFailed );
			retrn = KErrGeneral;
			}
		else
			{
	    	INFO_PRINTF1( KPassed );
		    }
		}
	retrn = KErrNone;
    return retrn ;
    }    
// -----------------------------------------------------------------------------
//Function Name :nice_more_than_highestL
//API Tested :nice
//API Description:The function nice() increases or decreases the priority value 
//by adding the value 'incr' to the current priority.It internally uses getpriority and 
//setpriority to set the priority to new value.
// -----------------------------------------------------------------------------

TInt CTestSysunistd::nice_more_than_highest( )
    {
	_LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KSetpriority, "In nice" );
    INFO_PRINTF1( KSetpriority );
	int retVal = 0, i;
	TInt retrn;
	errno = 0;
	TInt prio_high_end;
	TInt prio_high_start;
	
	_LIT( Kprio_high_start, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), Kprio_high_start, prio_high_start);
	if(!res)
		{
		_LIT(Kerr , "Failed to read first parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 _LIT( Kprio_high_end, "Parameter2" );
	 res = GetIntFromConfig(ConfigSection(), Kprio_high_end, prio_high_end);
	 if(!res)
		{
	 	_LIT(Kerr , "Failed to read second parameter from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	errno = 0;

	for (i = prio_high_start; i >= prio_high_end; i--)
		{
		retVal = nice(i);
		if((retVal == -1)&&(errno))
			{
			INFO_PRINTF1( KFailed );
			retrn = KErrGeneral;
			}
		else
			{
	    	INFO_PRINTF1( KPassed );
		    }
		}
	retrn = KErrNone;
    return retrn ;
    }    

// -----------------------------------------------------------------------------
//Function Name :pathconf_validL
//API Tested :pathconf
//API Description:pathconf returns configuration option name
//input --> _PC_XXXX_XXXX constants
// -----------------------------------------------------------------------------

TInt CTestSysunistd::pathconf_valid()
	{
	_LIT( KFailed, "Failed" );	
	TInt ret = KErrNone;
    
    INFO_PRINTF1(_L("In pathconf_Test0L"));
    
 	int fp = open("C:\\test_pathconf1.txt", O_RDWR|O_CREAT);
	if(fp != -1)
 		{
		int n = pathconf("C:\\test_pathconf1.txt", _PC_LINK_MAX);

		INFO_PRINTF2(_L("{Expected: (> _POSIX_LINK_MAX) } %d"), n);

		if( n < _POSIX_LINK_MAX )
			{
			ret = KErrGeneral;
			}
				         
		n = pathconf("C:\\test_pathconf1.txt", _PC_NAME_MAX);

		INFO_PRINTF2(_L("{Expected: (> _POSIX_NAME_MAX) } %d"), n);

		if( n < _POSIX_NAME_MAX )
			{
			ret = KErrGeneral;
			}
		
		n = pathconf("C:\\test_pathconf1.txt", _PC_PATH_MAX);

		INFO_PRINTF2(_L("{Expected: (> _POSIX_PATH_MAX) } %d"), n);

		if( n < _POSIX_PATH_MAX )
			{
			ret = KErrGeneral;
			}

		n = pathconf("C:\\test_pathconf1.txt", _PC_PIPE_BUF);

		INFO_PRINTF2(_L("{Expected: (> _POSIX_PIPE_BUF) } %d"), n);

		if( n < _POSIX_PIPE_BUF )
			{
			
			ret = KErrGeneral;
			}
		
		close(fp);
		int retVal = remove("C:\\test_pathconf1.txt");
 		}
 	else
 		{
 		INFO_PRINTF1( KFailed );
		ret = KErrGeneral;
 		}
	return ret;
    }
    
    
// -----------------------------------------------------------------------------
//Function Name :pathconf_invalidL
//API Tested :pathconf
//API Description:pathconf sets EINVAL when invalid constant is the input
//input --> _PC_XXXX_XXXX constants
// -----------------------------------------------------------------------------

TInt CTestSysunistd::pathconf_invalid()
    {
	TInt ret = KErrNone;
    _LIT( KFailed, "Failed" );
    INFO_PRINTF1(_L("In pathconf_Test1L"));
    
        
    #define _PC_INV_CONST 100
    
    errno = 0;
    int fp = open("C:\\test_pathconf.txt", O_RDWR|O_CREAT);
 	if(fp != -1)
 		{
	    int n = pathconf("C:\\test_pathconf.txt", _PC_INV_CONST);
		    
	  	INFO_PRINTF2(_L("{Expected: -1 } %d"), n);
		       
	    if( n != -1 || errno != EINVAL)
		    {
		    INFO_PRINTF2(_L("errno was set to - %d"), errno);
		    ret = KErrGeneral;
		    }
		close(fp);
		int retVal = remove("C:\\test_pathconf.txt");
 		}
	else
 		{
 		INFO_PRINTF1( KFailed );
		INFO_PRINTF2(_L("The errno value is %d\n"),errno);
		ret = KErrGeneral;
 		}
	
	return ret;
    }
    
// -----------------------------------------------------------------------------
//Function Name :unameL
//API Tested :uname
//API Description:The  uname function stores  NUL -terminated strings of 
//information identifying the current system into the structure referenced by 
//utsname structure( that consists sysname, nodename, release, version, machine).
// -----------------------------------------------------------------------------
    
TInt CTestSysunistd::unameL( )
    {

    // Print to UI
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

	_LIT( KFtruncate, "In uname" );
    // Print to log file
    INFO_PRINTF1( KFtruncate );
	TInt retrn = KErrGeneral;

	TInt retVal;
	struct utsname name;
	
	
	retVal = uname( &name );
	
	if( !retVal )
		{
		INFO_PRINTF1( KPassed );
		retrn = KErrNone;
		}
	else
		{
		INFO_PRINTF1( KFailed );
		retrn = KErrGeneral;
		}

    return retrn ;

    }

// -----------------------------------------------------------------------------
//Function Name :uname_nullL
//API Tested :uname
//API Description:The  uname function stores  NUL -terminated strings of 
//information identifying the current system into the structure referenced by 
//utsname structure( that consists sysname, nodename, release, version, machine).
// -----------------------------------------------------------------------------
TInt CTestSysunistd::uname_null( )
    {

    // Print to UI
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );

    _LIT( KFtruncate, "In uname_null" );
    // Print to log file
    INFO_PRINTF1( KFtruncate );
	TInt retrn = KErrGeneral;

	TInt retVal;
	struct utsname* name = NULL;
	errno = 0;	
	
	retVal = uname( name );
	
	if( !retVal )
		{
		INFO_PRINTF1( KFailed );
		retrn = KErrGeneral;
		}
	else
		{
		INFO_PRINTF1( KPassed );
		retrn = KErrNone;
		}

    return retrn ;
    }
        
/*Test Case Name 		:ftruncate_fileL
 *API Tested 			:ftruncate()
 *Test case Description :To Check the error condtion  by using a large value (off_t) which are greater than TInt(32 bit).
 */

TInt CTestSysunistd::ftruncate_negTest( )
    {
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );
	TInt retrn = KErrGeneral;
	TInt retval = 0;
#if defined(SYMBIAN_OE_LARGE_FILE_SUPPORT) && !defined(SYMBIAN_OE_NO_LFS)
	off_t offval =  ULLONG_MAX;
#else
	off_t offval =  UINT_MAX;
#endif /* SYMBIAN_OE_LARGE_FILE_SUPPORT && !SYMBIAN_OE_NO_LFS */
	int fp = open("C:\\truncate.txt", O_RDWR|O_CREAT);
	if(fp != -1)
		{
		retval = ftruncate( fp,offval);
		INFO_PRINTF2(_L("Value returned by truncate in this case is : %d"), retval);
		INFO_PRINTF2(_L("errno was set to : %d"), errno);
		if(retval == -1 && errno == 22)
			{
			INFO_PRINTF1( KPassed);
			retrn = KErrNone;
			}
		else
			{
			INFO_PRINTF1(KFailed);
			}
		}
	close(fp);
	unlink("C:\\truncate.txt");
	return retrn ;
    }

/*Test Case Name			:truncate_fileL
 *API Tested 			:truncate()
 * Test case Description:To Check the error condtion  by using a large value (off_t) which are greater than TInt(32 bit).
 */

TInt CTestSysunistd::truncate_negTest( )
    {
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );
	TInt retrn = KErrGeneral;
	TInt retval = 0;
#if defined(SYMBIAN_OE_LARGE_FILE_SUPPORT) && !defined(SYMBIAN_OE_NO_LFS)
	off_t offval =  ULLONG_MAX;
#else
	off_t offval =  UINT_MAX;
#endif /* SYMBIAN_OE_LARGE_FILE_SUPPORT && !SYMBIAN_OE_NO_LFS */
	char file[15] = "C:\\input.txt";
	int fp = open("c:\\input.txt", O_RDWR|O_CREAT);
	if(fp ==-1)
	{
		INFO_PRINTF2(_L("File opening failed with errno =%d\n"),errno);
		return KErrGeneral;
	}
	retval = truncate(file, offval);
	INFO_PRINTF2(_L("Value returned by truncate in this case is : %d"), retval);
	INFO_PRINTF2(_L("errno was set to : %d"), errno);
	if(retval == -1 && errno == 22)
			{
			INFO_PRINTF1( KPassed);
			retrn = KErrNone;
			}
		else
			{
			INFO_PRINTF1(KFailed);
			}
	close(fp);
	unlink("C:\\input.txt");
	return retrn ;
    }


/*Test Case Name		:fcntl_largefileTest
 *API Tested 			:fcntl()
 *Test case Description:To Check whether fcntl() call returns the correct fd when F_DUPFD is passed.
 */

TInt CTestSysunistd::fcntl_largefileTest( )
    {
	int retVal = KErrNone;
	char* filedir = "c:\\largefile.txt";
	off_t length = 0xFF;//255
	int fd = open(filedir, O_CREAT|O_RDWR, 0666);
	int val = ftruncate(fd, length);
	int thirdarg = 7;
	
	int ret;
	int fd2 = fcntl(fd, F_DUPFD, thirdarg);
	INFO_PRINTF2(_L("first fcntl call returns = %d"), fd2);
	if(fd2 >= thirdarg)
		{
		INFO_PRINTF2(_L("Passed, the file descriptor is >=  third argument i.e %d"), thirdarg); 
		}
	else
		{
		ERR_PRINTF2(_L("Failed, the file descriptor is < third argument i.e %d"), thirdarg);
		retVal = -1;
		close(fd);
        close(fd2);
        unlink(filedir);
        return retVal;
		}
	thirdarg = 10;
	int fd3 = fcntl(fd, F_DUPFD, thirdarg);
	INFO_PRINTF2(_L("second fcntl call returns = %d"), fd3);	
	if(fd3 >= thirdarg)
		{
		INFO_PRINTF2(_L("Passed, the file descriptor is >=  third argument i.e %d"), thirdarg); 
		}
	else
		{
		ERR_PRINTF2(_L("Failed, the file descriptor is < third argument i.e %d"), thirdarg);
		retVal = -1;
		goto cleanup;
		}
	//making use of the duplicate fd3 obtained by the fcntl call
	ret = write(fd3, "merlyn", 6);
	if (6 == ret)
		{
		INFO_PRINTF2(_L("write returns = %d"), ret);
		}
	else
		{
		ERR_PRINTF3(_L("write returns = %d and errno = %d\n"), ret, errno);
		retVal = -1;
		goto cleanup;
		}
	ret = lseek(fd3, -255, SEEK_END);//seek to begining of file
	if (0 == ret)
		{
		INFO_PRINTF2(_L("lseek returns = %d"), ret);
		}
	else
		{
		ERR_PRINTF3(_L("lseek returns = %d and errno = %d"), ret, errno);
		retVal = -1;
		goto cleanup;
		}
	char buf[20];
	ret = read(fd3, buf, 6);
	if ((6 == ret) && (strncmp(buf, "merlyn", 6) == 0))
		{
		INFO_PRINTF2(_L("read returns = %d"), ret);
		}
	else
		{
		ERR_PRINTF3(_L("read returns = %d and errno = %d"), ret, errno);
		retVal = -1;
		goto cleanup;
		}
cleanup:
	close(fd);
	close(fd2);
	close(fd3);
	unlink(filedir);
	return retVal;
    }
    
// -----------------------------------------------------------------------------
// CTestSysunistd::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
