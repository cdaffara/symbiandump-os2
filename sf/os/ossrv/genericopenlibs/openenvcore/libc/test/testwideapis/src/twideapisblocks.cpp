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
// Name        : twideapisBlocks.cpp
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



// INCLUDE FILES
#include "twideapis.h"

// -----------------------------------------------------------------------------
//Function Name : wfdopen_ivalm
//API Tested : wfdopen
//TestCase Description: testing the behaviour of wfdopen for invalid modes 
// initialized to NULL.
// -----------------------------------------------------------------------------  

TInt CTestWideApi::wfdopen_ivalm()
	{
	TPtrC nameRead;
	_LIT( KString, "Parameter1" );	    	
	TBool res = GetStringFromConfig(ConfigSection(), KString, nameRead);
	if(!res)
		{
		_LIT(Kerr , "Failed to read string") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}	
	int ret_wmkdir = wmkdir(L"C:\\ggg" , S_IWUSR);
	if((errno == EEXIST) || (!ret_wmkdir))
		{
		TBuf8<100> buf;
	
		int wfd;
		buf.Copy(nameRead);
		char* filemode = (char*) buf.Ptr();
		filemode[buf.Length()]='\0';
		wchar_t *dmode = (wchar_t *)malloc(30*sizeof(wchar_t));
		if(dmode==NULL)
			{
			wrmdir(L"C:\\ggg");        								
			return KErrNoMemory;							
			}						
		size_t siz = mbstowcs(dmode, filemode, 30);
		wunlink(L"C:\\ggg\\lll.txt");
		if((wfd=wopen(L"C:\\ggg\\lll.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777))<0)
	        {
        	_LIT(Kopen,"Failed to open file");
			INFO_PRINTF1(Kopen);
			wrmdir(L"C:\\ggg");      
			free(dmode) ;
			return KErrGeneral;
	        }
        else
	        {
	         FILE *fp =wfdopen(wfd ,dmode);
        	 if(NULL == fp)    	
        	 	{
	    	 	_LIT(Kopen,"wfdopen failed");
				INFO_PRINTF1(Kopen);
				close(wfd);
				wunlink(L"C:\\ggg\\lll.txt");
				wrmdir(L"C:\\ggg"); 
				free(dmode) ;     		
				return KErrNone;
        	 	}
        	 else
        	    { 
    	    	wunlink(L"C:\\ggg\\lll.txt");
   				wrmdir(L"C:\\ggg");
   				free(dmode) ;
    	    	return KErrGeneral;
        	    }        	 	
    	    }      
      	}
	else
		{
	  	_LIT(Kerr , "Failed to make dir") ;
		INFO_PRINTF1(Kerr) ;
		wunlink(L"C:\\ggg\\lll.txt");
   		wrmdir(L"C:\\ggg");
		return KErrGeneral ;
		}
	}
	
	
// -----------------------------------------------------------------------------
//Function Name : wfdopen_val
//API Tested : wfdopen
//TestCase Description: testing the behaviour of wfdopen for valid arguments
// initialized to NULL.
// -----------------------------------------------------------------------------  
TInt CTestWideApi::wfdopen_val()
	{
	TPtrC nameRead;
	wchar_t *pathName = NULL;
	_LIT( KString, "Parameter1" );	    	
	TBool res = GetStringFromConfig(ConfigSection(), KString, nameRead);
	if(!res)
		{
		_LIT(Kerr , "Failed to read string") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}		
	int ret_wmkdir = wmkdir(L"C:\\ggg" , S_IWUSR);
	if((errno == EEXIST) || (!ret_wmkdir))
		{
		TBuf8<100> buf;	
		int wfd;
		FILE *fp;
		buf.Copy(nameRead);
		char* filename = (char*) buf.Ptr();
		filename[buf.Length()]='\0';
		pathName = (wchar_t *)malloc(30*sizeof(wchar_t));
		if(pathName==NULL)
			{
			wrmdir(L"C:\\ggg");        								
			return KErrNoMemory;							
			}						
		size_t siz = mbstowcs(pathName, filename, 30);
		wunlink(pathName) ;
        if((wfd=wopen(pathName, O_WRONLY | O_CREAT | O_TRUNC, 0666))<0)
	        {
        	_LIT(Kopen,"Failed to open file");
			INFO_PRINTF1(Kopen);
			wrmdir(L"C:\\ggg"); 
			free(pathName);       		
			return KErrGeneral;
	        }
        else
	        {
        	 if(NULL == (fp =wfdopen(wfd ,L"w")))    	
        	 	{
        	 	_LIT(Kopen,"wfdopen failed");
				INFO_PRINTF1(Kopen);
				wrmdir(L"C:\\ggg"); 
				free(pathName);       		
				return KErrGeneral;
        	 	}
        	 char *buffer="Writing to the file";
        	 size_t size=strlen(buffer);        	 
        	 if(size!=(fwrite(buffer,1,size,fp)))
        	 	{
        	 	_LIT(Kopen,"fwrite failed");
				INFO_PRINTF1(Kopen);
				wrmdir(L"C:\\ggg");    
				free(pathName);    		
				return KErrGeneral;
        	 	}        	 	
    	    }
       fclose(fp);
       wunlink(pathName);
       wrmdir(L"C:\\ggg");
       free(pathName);        		
	   return KErrNone;
	    }
	else
		{
	  	_LIT(Kerr , "Failed to make dir") ;
		INFO_PRINTF1(Kerr) ;
		wrmdir(L"C:\\ggg");
		return KErrGeneral ;
		}
	}
	
TInt CTestWideApi::wfdopen_ivalm1()
	{
	int wfd;
	int ret_wmkdir = wmkdir(L"C:\\ggg" , S_IWUSR);
	if(ret_wmkdir)
		{
	  	_LIT(Kerr , "Failed to make dir") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;		
		}				
	if((wfd=wopen(L"C:\\ggg\\lll.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777))<0)
		{
	   	_LIT(Kopen,"Failed to open file");
		INFO_PRINTF1(Kopen);
		wrmdir(L"C:\\ggg");        		
		return KErrGeneral;
	    }
    FILE *fp =wfdopen(wfd ,NULL);
    if(NULL != fp)    	   
	    {
	 	wunlink(L"C:\\ggg\\lll.txt");
	   	wrmdir(L"C:\\ggg");
	   	return KErrGeneral;    	
	    }
	close(wfd);
    wunlink(L"C:\\ggg\\lll.txt");
	wrmdir(L"C:\\ggg");
    return KErrNone;  
	}


// -----------------------------------------------------------------------------
//Function Name : wfreopen_valm
//API Tested : wfreopen
//TestCase Description: testing the behaviour of wfreopen for different modes
// initialized to NULL.
// -----------------------------------------------------------------------------  
TInt CTestWideApi::wfreopen_valm() 
	{
	TPtrC nameRead;
	_LIT( KString, "Parameter1" );	    	
	TBool res = GetStringFromConfig(ConfigSection(), KString, nameRead);
	if(!res)
		{
		_LIT(Kerr , "Failed to read string") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
   int ret_wmkdir = wmkdir(L"C:\\ggg" , S_IWUSR);
   if((errno == EEXIST) || (!ret_wmkdir))
		{
		TBuf8<100> buf;
	
		FILE *fp;
		buf.Copy(nameRead);
		char* filename = (char*) buf.Ptr();
		filename[buf.Length()]='\0';
		wchar_t *pathName = (wchar_t *)malloc(30*sizeof(wchar_t));
		if(pathName==NULL)
			{
			wrmdir(L"C:\\ggg");        								
			return KErrNoMemory;							
			}						
		size_t siz = mbstowcs(pathName, filename, 30);
		wunlink(pathName) ;       
        if((fp=wfreopen(pathName ,L"w+",stdout))==NULL) 
			{
			_LIT(Kropen , " Failed to reopen file " ) ;
			INFO_PRINTF1(Kropen) ;
			wrmdir(L"C:\\ggg");
			free(pathName);     		
			return KErrGeneral;
			}
        else
        	{
           int j=printf("Output Redirected to file now");
           long i=ftell(fp);
           if(i!=j)
        	  	{
        	 	fclose(fp);	 			        	 
        	 	wunlink(pathName);
        	 	wrmdir(L"C:\\ggg");
        	 	free(pathName);     	 			        	 	 			        	 	
    	     	return KErrGeneral;	 			        	 
        	 	}
        	 fclose(fp);	 			        	 
        	 wunlink(pathName);
        	 wrmdir(L"C:\\ggg"); 
        	 free(pathName);    	 			        	 	 			        	 	       		
        	 return KErrNone;
        	}
		}
	else
		{
	  	_LIT(Kerr , "Failed to make dir") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	}

// -----------------------------------------------------------------------------
//Function Name : wfreopen_val
//API Tested : wfreopen
//TestCase Description: testing the behaviour of wfreopen for valid arguments
// initialized to NULL.
// -----------------------------------------------------------------------------    
    
TInt CTestWideApi::wfreopen_val()
	{
	TPtrC nameRead;
	_LIT( KString, "Parameter1" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, nameRead);
	if(!res)
		{
		_LIT(Kerr , "Failed to read string") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
    int ret_wmkdir = wmkdir(L"C:\\ggg" , S_IWUSR);
	if((errno == EEXIST) || (!ret_wmkdir))
		{
		TBuf8<100> buf;	
		int wfd;
		FILE *fp;
		buf.Copy(nameRead);
		char c = 'z';
		char* filename = (char*) buf.Ptr();
		filename[buf.Length()]='\0';
		wchar_t *pathName = (wchar_t *)malloc(30*sizeof(wchar_t));
		if(pathName==NULL)
			{
			wrmdir(L"C:\\ggg");        								
			return KErrNoMemory;							
			}							
		size_t siz = mbstowcs(pathName, filename, 30);
		wunlink(pathName) ;
        if((wfd=wopen(pathName, O_WRONLY | O_CREAT | O_TRUNC, 0666))<0)
        	{
        	_LIT(Kopen,"Failed to open file");
			INFO_PRINTF1(Kopen);
			wrmdir(L"C:\\ggg");  
			free(pathName);      		
			return KErrGeneral;
        	}
        else
        	{
             write(wfd,&c,1); 	
        	}
        close(wfd);
        if((fp=wfreopen(pathName ,L"r",stdin))==NULL) 
			{
			_LIT(Kropen , " Failed to reopen file " ) ;
			INFO_PRINTF1(Kropen) ;
			wrmdir(L"C:\\ggg");  
			free(pathName);       		
			return KErrGeneral;
			}
     	char * filen="C:\\tem.txt";
     	unlink(filen);
     	int fd=open(filen, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if(fd<0)
			{
			_LIT(Kopen,"Failed to open file");
			INFO_PRINTF1(Kopen);
			wrmdir(L"C:\\ggg");        		
			free(pathName); 
			return KErrGeneral  ;
			}
		else
			{
		   	write(fd,&c,1);
			}
		close(fd); 
	    fclose(fp); 
	    unlink(filen);
		wunlink(pathName);	
		wrmdir(L"C:\\ggg");  
		free(pathName);       		
        return KErrNone ;
	    }
	else
		{
	  	_LIT(Kerr , "Failed to make dir") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	}
	


// -----------------------------------------------------------------------------
//Function Name : wfreopen_valinv
//API Tested : wfreopen
//TestCase Description: testing the behaviour of wfreopen for invalid filename
// initialized to NULL.
// -----------------------------------------------------------------------------  


TInt CTestWideApi::wfreopen_valinv()
	{
	TPtrC nameRead;	
	_LIT( KString, "Parameter1" );	    	
	TBool res = GetStringFromConfig(ConfigSection(), KString, nameRead);
	if(!res)
		{
		_LIT(Kerr , "Failed to read string") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}	
	int ret_wmkdir = wmkdir(L"C:\\ggg" , S_IWUSR);
	if((errno == EEXIST) || (!ret_wmkdir))
		{
		TBuf8<100> buf;	   				
		buf.Copy(nameRead);
		char* filename = (char*) buf.Ptr();
		filename[buf.Length()]='\0';
		wchar_t *pathName = (wchar_t *)malloc(30*sizeof(wchar_t));
		if(pathName==NULL)
			{
			wrmdir(L"C:\\ggg");        								
			return KErrNoMemory;							
			}						
		size_t siz = mbstowcs(pathName, filename, 30);
		wunlink(pathName) ;
        FILE *fp = wfreopen(pathName ,L"w",stdout);
        if(fp == NULL) 
			{
			_LIT(Kropen , " Failed to reopen file " ) ;
			INFO_PRINTF1(Kropen) ;
			wrmdir(L"C:\\ggg");        		
			free(pathName);
			return KErrNone;
			}
        else
        	{
    		wrmdir(L"C:\\ggg");  
    		free(pathName);
    		return KErrGeneral;
        	}
		}
	else
		{
	  	_LIT(Kerr , "Failed to make dir") ;
		INFO_PRINTF1(Kerr) ;
		return KErrGeneral ;
		}
	}

//-----------------------------------------------------------------------------
//Function Name :wremoveTest1
//API Tested :wremove
//TestCase Description: creates a directory and 2 files inside the dir and tries
// to delete the files using wremove API, it should return 0 on successful 
// deletion of the files and then the directory and -1 on failure.
//-----------------------------------------------------------------------------

TInt CTestWideApi::wremove_val1()
	{
	wchar_t *dirName = L"C:\\wremoveDir"; 
	wchar_t *fileName1 = L"C:\\wremoveDir\\file1.txt"; 
	wchar_t *fileName2 = L"C:\\wremoveDir\\file2.txt"; 	
    TInt ret = -1;    
    errno = 0;
    if(wmkdir(dirName ,S_IWUSR)  < 0) 
	    {
    	INFO_PRINTF1(_L("Failed to create the parent directory"));
    	return KErrGeneral ;
	    }    
    FILE *fp = wfopen(fileName1, (wchar_t *)"w");    
    if(fp == NULL) 
	    {
    	INFO_PRINTF1(_L("Failed to create the child directory"));
    	return KErrGeneral ;
	    }
    fclose(fp);        
    
    fp = wfopen(fileName2, (wchar_t *)"w");    
    if(fp == NULL) 
	    {
    	INFO_PRINTF1(_L("Failed to create the child directory"));
    	return KErrGeneral ;
	    }
    fclose(fp);    
    ret = wremove(fileName1);
    if(ret != 0) 
	    {
    	INFO_PRINTF1(_L("wremove failed"));
    	wunlink(fileName1);
    	return KErrGeneral;
	    }    
    ret = wremove(fileName2);
    if(ret != 0) 
	    {
    	INFO_PRINTF1(_L("wremove failed"));
    	wunlink(fileName2);    	
    	return KErrGeneral;
	    }    
    ret = wremove(dirName);
    if(ret != 0)
		{
		INFO_PRINTF1(_L("wremove failed"));
		return KErrGeneral;
		}    
    return KErrNone ;        
	}


//-----------------------------------------------------------------------------
//Function Name :wremoveTest2
//API Tested :wremove
//TestCase Description: creates a directory and tries to delete the file that 
// doesnot exist in the dir using wremove API, it should return -1 and set the
// error number correspondingly.
//-----------------------------------------------------------------------------

TInt CTestWideApi :: wremove_val2()
	{
	wchar_t *dirName = L"C:\\wremoveDir1"; 
	wchar_t *fileName1 = L"C:\\wremoveDir1\\file1.txt"; //file doesnot exist
	TInt ret = -1;    
    errno = 0;
    if(wmkdir(dirName ,S_IWUSR)  < 0) 
    	{
    	INFO_PRINTF1(_L("Failed to create the parent directory"));
    	return KErrGeneral ;
    	}    
    ret = wremove(fileName1);
    if(ret != -1 && errno == 0) 
    	{
    	INFO_PRINTF1(_L("wremove failed"));
    	return KErrGeneral;
    	}       
    ret = wremove(dirName);
    if(ret != 0)
    	{
    	INFO_PRINTF1(_L("wremove failed"));
    	return KErrGeneral;
    	}    
    return KErrNone ;
	}


//-----------------------------------------------------------------------------
//Function Name :wremoveTest3
//API Tested :wremove
//TestCase Description: creates a directory and tries to delete the dir using 
// wremove API, it should return -1 and set the error number correspondingly.
//-----------------------------------------------------------------------------

TInt CTestWideApi::wremove_val3() 
	{
	TInt nameRead;
	TInt retnVal;
	int retn = KErrGeneral;		
   _LIT( Klen, "Parameter1" );
   
   TBool res = GetIntFromConfig(ConfigSection(), Klen, nameRead);
   if(!res)
		{
	 	_LIT(Kerr , "Failed to read the value") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
   _LIT( Klen2, "Parameter2" );   
   res = GetIntFromConfig(ConfigSection(), Klen2, retnVal);
   if(!res)
		{
	 	_LIT(Kerr , "Failed to read the value") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}	
	wchar_t* dirName = L"C:\\wremoveDir3";
	int ret1 = wmkdir(dirName, S_IWUSR);
	if(!ret1)
		{
		int ret = wremove(dirName);		
		if(!ret && !retnVal)
			{
			INFO_PRINTF1(  _L("Passed..."));
			retn = KErrNone;
			}
		else if(ret && retnVal)
			{
			INFO_PRINTF1( _L("Passed..."));
			retn = KErrNone;
			}			
		else
			{
			INFO_PRINTF1( _L("Failed..."));	
			}
		}
	else
		{
		if(retnVal)
			{
			INFO_PRINTF1(  _L("Passed..."));
			retn = KErrNone;
			}
		else
			{
			INFO_PRINTF1( _L("Failed..."));	
			}
		}
    return retn;
	}


//-----------------------------------------------------------------------------
//Function Name :wremove_null
//API Tested :wremove
//TestCase Description: tests the behaviour of wremove with NULL as the argument.
//-----------------------------------------------------------------------------

TInt CTestWideApi :: wremove_null()
	{
	int ret = wremove(NULL);
	if(ret < 0)
		{
		return KErrNone ;
		}
	return KErrGeneral;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestWideApi::getws_val
API Tested			: getws
TestCase Description: The test case takes a wide character string from standard 
					  input.
-------------------------------------------------------------------------------
*/
TInt CTestWideApi::getws_val()
	{
	FILE* stdop = freopen("c:\\stdop","w+",stdout);
	FILE* stdip = freopen("c:\\stdip","w+",stdin);
	wchar_t* buf = (wchar_t*)malloc(sizeof(wchar_t)*50);
	if(buf==NULL)
		{
    	fclose(stdin);
    	fclose(stdop);
		remove("c:\\stdop");
		remove("c:\\stdip");	
		return KErrNoMemory;	
		}		
	wchar_t s[100]; 
	TInt ret = KErrNone;
	size_t size = fwrite("sushma\n", 1, 6, stdip);
	fclose(stdip);
	stdip = freopen("c:\\stdip","r", stdin);
	getws(s);  // read a line (from stdin)
	putws(s);
    fclose(stdop);
    stdop = freopen("c:\\stdop","r", stdout);
    //size = fread(buf, 1, 6, stdop);
    fgetws(buf,7,stdop);
    if(wcsncmp(s, buf,6))
    	{
    	ret = KErrGeneral;
    	}    
    fclose(stdin);
    fclose(stdop);
	remove("c:\\stdop");
	remove("c:\\stdip");
	free(buf);
	return ret;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestWideApi::getws_null
API Tested			: getws
TestCase Description: The test case to test getws for NULL argument
-------------------------------------------------------------------------------
*/
TInt CTestWideApi::getws_null()
	{
	INFO_PRINTF1(_L("getws_nullL"));	
	wchar_t* ret = getws(NULL);  // read a line (from stdin)	
	if(!ret)
		{		
		return KErrNone;
		}	
	else 
		{
		return KErrGeneral;
		}
	}
    
/*
-------------------------------------------------------------------------------
Function Name		: CTestWideApi::putws_val1
API Tested			: putws
TestCase Description: The testcase funtion will write a fixed lenth wide char string 
					  to the stdout.
-------------------------------------------------------------------------------
*/
TInt CTestWideApi::putws_val1()
	{
	INFO_PRINTF1(_L("putws"));
	wchar_t buf[12];	
	FILE* stdop = freopen("c:\\stdop","w+",stdout);
	FILE* op;
	putws(L"Hello World");
	fclose(stdop);	
	op = freopen("c:\\stdop","r",stdout);	
	fgetws(buf, 12, op);	
	fclose(stdop);	
	remove("c:\\stdop");
    if(!(wcsncmp(L"Hello World", buf, 11)))
		{			
		return KErrNone;
		}	
	else 
		{
		return KErrGeneral;
		}
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestWideApi::putws_val2L
API Tested			: putws
TestCase Description: The testcase function will write a wide string to the screen
					  which has a null terminator in the middle of the string.
-------------------------------------------------------------------------------
*/
TInt CTestWideApi::putws_val2()
	{
	INFO_PRINTF1(_L("putws2"));
	wchar_t buf[10];
	FILE* stdop = freopen("c:\\stdop","w+",stdout);	
	putws(L"Hel\0lo World");
	fclose(stdop);
	stdop = freopen("c:\\stdop","r",stdout);	
	fgetws(buf, 4, stdop);
	fclose(stdop);
	remove("c:\\stdop");	
    if(!wcsncmp(L"Hel", buf, 4))
		{		
		return KErrNone;
		}	
	else 
		{
		return KErrGeneral;
		}	
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestWideApi::putws_nullL
API Tested			: putws
TestCase Description: The testcase funtion tests the behaviour of putws for NULL
					  argument.
-------------------------------------------------------------------------------
*/
TInt CTestWideApi::putws_null()
	{
	INFO_PRINTF1(_L("putws1"));
	int ret = putws(NULL);
    if(ret>0)
		{			
		return KErrGeneral;
		}	
	else 
		{
		return KErrNone;
		}
	}


//-----------------------------------------------------------------------------
//Function Name :wpopen_1L
//API Tested :wpopen
//TestCase Description: tests the behaviour of wpopen with NULL as the command.
//-----------------------------------------------------------------------------

TInt CTestWideApi :: wpopen_1()
	{
    FILE *fp;
    fp=wpopen(NULL,L"r");
	if  (fp == NULL && errno == ENOENT) 
		{        
		INFO_PRINTF1(_L("wpopen success"));
        return KErrNone;
		}
	return KErrGeneral;
	}



TInt CTestWideApi::wfreopen1()
	{
	char* filename = "c:\\ggg1.txt";
	wchar_t *pathName = (wchar_t *)malloc(30*sizeof(wchar_t));
	if(pathName==NULL)
		{
		return KErrNoMemory;							
		}						
	size_t siz = mbstowcs(pathName, filename, 30);
	FILE *fp = wfreopen(pathName ,NULL,stdout);
	free(pathName) ;
	if(fp == NULL)
		{        
		INFO_PRINTF1(_L("wfreopen success"));
        return KErrNone;
		}
		
	return KErrGeneral;
	}
TInt CTestWideApi::wfreopen2()
	{
	char* filename = "c:\\ggg.txt";
	wchar_t *pathName = (wchar_t *)malloc(30*sizeof(wchar_t));
	if(pathName==NULL)
		{
		return KErrNoMemory;							
		}						
	size_t siz = mbstowcs(pathName, filename, 30);
	FILE *fp = wfreopen(pathName ,L"x",stdout);
	free(pathName) ;
	if(fp == NULL)
		{        
		INFO_PRINTF1(_L("wfreopen success"));
        return KErrNone;
		}
		
	return KErrGeneral;
	}
TInt CTestWideApi::wfreopen3()
	{
	
	FILE *fp1 = wfreopen(NULL ,L"w",stdout);
	if(fp1 == NULL)
		{        
		INFO_PRINTF1(_L("wfreopen success"));
        return KErrNone;
		}
		
	return KErrGeneral;
	}
TInt CTestWideApi::wfreopen4()
	{
	
	wchar_t *fileName1 = L"C:\\myfile1.txt"; 
	FILE *fp = wfopen(fileName1, (wchar_t *)"w");    
    if(fp == NULL) 
	    {
    	INFO_PRINTF1(_L("Failed to create the child directory"));
    	return KErrGeneral ;
	    }

	FILE *fp1 = wfreopen(NULL ,L"w",fp);
	if(fp1 != NULL)
		{        
		INFO_PRINTF1(_L("wfreopen success"));
        return KErrNone;
		}
		
	return KErrGeneral;
	}
//  End of File
