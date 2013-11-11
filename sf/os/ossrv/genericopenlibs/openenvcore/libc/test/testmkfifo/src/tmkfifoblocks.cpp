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


#include "tmkfifo.h"
#include <errno.h>
    


// -----------------------------------------------------------------------------
// GetFullPath
// -----------------------------------------------------------------------------
//
TInt GetFullPath(TParse& aParse, const TText16* upath, RFs& aSession, TDes* aFileName)
//
// Parse a path of the form "[C:][\]AAA\..\.\BBB\xxx" where:
// .  indicates the current directory
// .. indicates move to the parent directory
// An optional "\" at the start of the path indicates the path is not relative to the current path,
// and is implied if the drive specifier is present
// If aFileName is non-NULL then the final component is a filename and should be copied into 
// the aFileName descriptor.
//
	{
	TInt r;
	TBuf<3> drive;
	TFileName nextBit;
	TText16 c=*upath;

	if (c && upath[1]==KDriveDelimiter) 
		{
		// drive name specified
		if (c==L'?')
			drive.Zero();			// use "?:" to mean scan across drives
		else
			{
			drive.Copy(TPtrC16(upath, 2));
			drive.UpperCase();
			}
		upath+=2;
		drive.Append(TChar(KPathDelimiter));	// enforce absoluteness
		}
	else
		{
		// no leading drive specifier
		drive.Zero();
		if (c==KPathDelimiter||c==L'/')
			{
			upath+=1;
			drive.Append(TChar(KPathDelimiter));
			}
		}
	r = aSession.Parse(drive, aParse);

	// upath now looks like a relative pathname, to be added onto
	// aParse a directory at a time. Note that '/' is not allowed in
	// EPOC32 file or directory names, so treat it as an alternative separator

	c=*upath;
	while (c && (r==KErrNone))
		{
		const TText16* ustart=upath;
		do 
		{
			c=*upath++;
		}
		while (c && c!=KPathDelimiter && c!=L'/');

		TInt len=(upath-ustart)-1;		// excludes delimiter
		if (len==0)
			{
			continue;
			}
		if (ustart[0]==L'.')
			{
			if (len==1)
				{
				continue;	// directory . ignored
				}
			if (len==2 && ustart[1]==L'.')
				{
				// directory ..
				(void) aParse.PopDir();	// just stick at root dir, no errors
				continue;
				}
			}
		if (len>=KMaxFileName)
			{
			return ENAMETOOLONG;
			}
		if (c==L'\0' && aFileName!=NULL)
			{
			// it's the trailing filename
			aFileName->Copy(TPtrC16(ustart, len));
			break;
			}
		else	
			{
			// it's a component of the accumulating path
			nextBit.Copy(TPtrC16(ustart, len));
			r = aParse.AddDir(nextBit);
			}
		}
	return(r);
	}
	
// -----------------------------------------------------------------------------
// GetFullFile
// Use GetFullPath to establish the pathname, then add the filename onto the end
// -----------------------------------------------------------------------------
//
TInt GetFullFile(TFileName& aName, const TText16* upath, RFs& aSession)
	{	
	TParse path;
	TInt err = GetFullPath(path,upath,aSession,&aName);
	if (err!=KErrNone)
		{
		return err;
		}
	// Wildcard drive letter for searching across drives
	if (upath[0]==L'?' && upath[1]==L':')
		{
		TFindFile search(aSession);
		err=search.FindByDir(aName,path.Path());
		if (!err)
			{
			aName=search.File();
			return KErrNone;
			}
		}
	err = path.SetNoWild(path.DriveAndPath(),NULL,&aName);
	if (!err)
		{
		aName = path.FullName();
		}
	return err;	
	}


// -----------------------------------------------------------------------------
// CTestMkfifo::DeleteFifo
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::DeleteFifo(  )
{

    _LIT(KFunc, "DeleteFifo");
	const char* path = "C:\\mkfifo.test";
	wchar_t _widename[KMaxFileName+1];
	RFs iSession;
    TInt err;
    TFullName fullName;
    
    INFO_PRINTF1( KFunc);
    


    err = iSession.Connect(); // connect to server
    if (!err)
    {
        if((size_t)-1 != mbstowcs(_widename, path, KMaxFileName))
        {
    	    err = GetFullFile(fullName,(const TText16*)_widename,iSession);
    	    if(!err)
    	    {
    	        err = iSession.Delete(fullName);
    	        if(err < 0)
    	        {
    	    	    INFO_PRINTF2( KFunc, err);
    	    	    err  = KErrNone; // we do not care if this file was deleted
    	        }
    	    }
        }
    	
    }
    iSession.Close();
    //DebugLogPrintL ( KFunc(), ERROR, err);
    ////DebugLogPrintL( KFunc(), OUT);
    return err;    
}

// -----------------------------------------------------------------------------
// CTestMkfifo::FifoCreate
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::FifoCreate(  )
{
	_LIT(KFunc, "FifoCreate");
	const char* path = "C:\\mkfifo.test";
    INFO_PRINTF1( KFunc);
    TInt errVal;
    _LIT( KerrVal, "Param%d" );
     TBuf<8> pNameBuf;
     pNameBuf.Format(KerrVal,++iParamCnt);
	 TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, errVal);
	 if(!res)
	 {
	 	_LIT(Kerr , "Unable to retrieve errVal") ;
	 	INFO_PRINTF1(Kerr) ;
		return KErrGeneral; 	
	 }
    
    TInt err = mkfifo (path, 0666);
    
    //struct _reent *r = _REENT;
	  //TInt err_no = r->_errno;
	  
    if ( err < 0)
        {      
          if ( errVal && (errno == EEXIST))
            {
            err = KErrNone;
            }
          else
            {
            ERR_PRINTF2( _L("mkfifo error: %d"), err);
            }
        }
    ////DebugLogPrintL( KFunc(), OUT);
    return err;
}


// -----------------------------------------------------------------------------
// CTestMkfifo::FifoCreateNULL
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::FifoCreateNULL(  )
{
	_LIT(KFunc, "FifoCreateNULL");
    INFO_PRINTF1( KFunc);
    TInt errVal;
    _LIT( KerrVal, "Param%d" );
     TBuf<8> pNameBuf;
     pNameBuf.Format(KerrVal,++iParamCnt);
	 TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, errVal);
	 if(!res)
	 {
	 	_LIT(Kerr , "Unable to retrieve errVal") ;
	 	INFO_PRINTF1(Kerr) ;
		return KErrGeneral; 	
	 }
    TInt err = mkfifo (NULL, 0666);
    if ( err )
        {
        	if ( errVal && (errno == ENOENT))
            {
            err = KErrNone;
            }
          else
            {
            ERR_PRINTF2( _L("mkfifo error: %d"), err);
            }
        }
    ////DebugLogPrintL( KFunc(), OUT);
    return err;
}
// -----------------------------------------------------------------------------
// CTestMkfifo::FifoCreateInSystemDir
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::FifoCreateInSystemDir (  )
{
	_LIT(KFunc, "FifoCreateInSystemDir");
	const char* path = "Z:\\mkfifo.test";
    INFO_PRINTF1( KFunc);
    
    TInt err = mkfifo (path, 0666);
    if ( err < 0 && errno == EACCES)
    {
         err = KErrNone;
    }
    else
    {
         ERR_PRINTF2( _L("mkfifo error: %d"), err);
         err = KErrGeneral;
    }
    ////DebugLogPrintL( KFunc(), OUT);
    return err;
}

// -----------------------------------------------------------------------------
// CTestMkfifo::FifoCreateNameTooLong
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::FifoCreateNameTooLong (  )
{
	_LIT(KFunc, "FifoCreateInSystemDir");
	char path[258];
	TInt i;
    INFO_PRINTF1( KFunc);
    
    for (i=0; i<258; i++)
    {
    	path[i] = 'a';
    }
    path[i] = '\0';
    
    TInt err = mkfifo (path, 0666);
    if ( err < 0 && errno == ENAMETOOLONG)
    {
         err = KErrNone;
    }
    else
    {
         ERR_PRINTF2( _L("mkfifo error:%d"), err);
         err = KErrGeneral;
    }
	////DebugLogPrintL( KFunc(), OUT);
    return err;
}

// -----------------------------------------------------------------------------
// CTestMkfifo::FifoCreateInNonDir
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::FifoCreateInNonDir (  )
{
	_LIT(KFunc, "FifoCreateInNonDir");
	const char* path = "C:\\mkfifo.test\\mkfifo.test";
    INFO_PRINTF1( KFunc);
    
    TInt err = mkfifo (path, 0666);
    if ( err < 0 && errno == ENOTDIR)
    {
         err = KErrNone;
    }
    else
    {
         ERR_PRINTF3( _L("mkfifo error :%d %d"), err,errno);
         err = KErrGeneral;
    }
	unlink("C:\\mkfifo.test\\mkfifo.test");
    ////DebugLogPrintL( KFunc(), OUT);
    return err;
}


// -----------------------------------------------------------------------------
// CTestMkfifo::FifoCreateNonExistantDir
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::FifoCreateNonExistantDir (  )
{
	_LIT(KFunc, "FifoCreateNonExistantDir");
	const char* path = "C:\\XXX\\mkfifo.test";
    INFO_PRINTF1( KFunc);
    
    unlink("C:\\XXX");
    
    TInt err = mkfifo (path, 0666);
    if ( err < 0 && errno == ENOENT)
    {
         err = KErrNone;
    }
    else
    {
         ERR_PRINTF2( _L("mkfifo error :%d"), err);
         err = KErrGeneral;
    }
    ////DebugLogPrintL( KFunc(), OUT);
    return err;
}

// -----------------------------------------------------------------------------
// CTestMkfifo::FifoOpen
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::FifoOpen(  )
{
	  _LIT(KFunc, "FifoOpen");
	  const char* path = "C:\\mkfifo.test";
    INFO_PRINTF1( KFunc);
    TInt err;
    //if ( err )
      //  {
       // DebugLogPrintL ( _L("aItem.GetNextInt"), ERROR, err);
       // return err;
       // }
    err = open (path, O_RDONLY);
    if ( err < 0)
        {
            ERR_PRINTF2 ( _L("open error :%d"), err);
        }
    //DebugLogPrintL ( KFunc(), OUT);
    return KErrNone;
}


// -----------------------------------------------------------------------------
// CTestMkfifo::FifoOpenWNonBlock
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::FifoOpenWNonBlock(  )
{
	  _LIT(KFunc, "FifoOpenWNonBlock");
	  const char* path = "C:\\mkfifo.test";
    INFO_PRINTF1( KFunc);
    TInt fd;
    fd = open (path, O_NONBLOCK|O_WRONLY);
	TInt err_no =  errno;
    if ( fd < 0 && err_no == ENXIO)
        {
            ERR_PRINTF2 ( _L("open error :%d"), fd);
            return KErrNone;
        }
    if ( fd > 0 )    
        {
        close(fd);
        return KErrNone;
        }
    return fd;
}

// -----------------------------------------------------------------------------
// CTestMkfifo::FifoOpenRNonBlock
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::FifoOpenRNonBlock(  )
{
	  _LIT(KFunc, "FifoOpenRNonBlock");
	  const char* path = "C:\\mkfifo.test";
    INFO_PRINTF1( KFunc);
    TInt fd;
    fd = open (path, O_NONBLOCK|O_RDONLY);
    if ( fd > 0 )
        {
            close(fd);
            return KErrNone;
        }
    return fd;
}
// -----------------------------------------------------------------------------
// CTestMkfifo::FifoClose
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::FifoClose(  )
{
	  _LIT(KFunc, "FifoClose");
	  const char* path = "C:\\mkfifo.test";
    INFO_PRINTF1( KFunc);
    TInt err;
    TInt fd = open (path, O_NONBLOCK|O_RDONLY);
    if ( fd < 0)
        {
            ERR_PRINTF3 ( _L("Fifo open returned: %d [%d]"), fd, errno);
            return -1;
        }
    err = close(fd);  
    if ( err )
       {
           ERR_PRINTF2 ( _L("File close error: %d"), err);
           return -1;
       }  
    return err;
}


// -----------------------------------------------------------------------------
// CTestMkfifo::FifoRead
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::FifoRead(  )
{
	_LIT(KFunc, "FifoRead");
	const char* path = "C:\\mkfifo.tst";
    INFO_PRINTF1( KFunc);
    TInt err;
    char buf[128];

    TInt fd = open (path, O_RDONLY|O_CREAT);
    if ( fd < 0 )
        {
            ERR_PRINTF2 ( _L("Fifo open %d"),  fd);
            return KErrGeneral;
        }
    User::After(500000);    
    err = read (fd, buf, 128);  
    if ( (err < 0) || (strcmp(buf, "somejunk") != 0))
       {
           ERR_PRINTF2 ( _L("Read error: %d"), err);
		   close(fd);
           return KErrGeneral;
       }  
    User::After(100000);
    close(fd);   
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CTestMkfifo::FifoWrite
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::FifoWrite(  )
{
	_LIT(KFunc, "FifoWrite");
	const char* path = "C:\\mkfifo.test";
    INFO_PRINTF1( KFunc);
    TInt err;
    char buf[128];
    TInt fd = open (path, O_WRONLY);
    if ( fd < 0 )
        {
            ERR_PRINTF2 ( _L("Fifo open error: Returned %d"),  fd);
            return -1;
        }
    err = write (fd, buf, 128);  
    if ( err < 0 )
       {
           ERR_PRINTF2 ( _L("Write error : %d"), err);
           return -1;
       }  
    return err;
}

// -----------------------------------------------------------------------------
// CTestMkfifo::FifoWriteRead
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::FifoWriteRead(  )
{
	  _LIT(KFunc, "FifoWriteRead");
	  const char* path = "C:\\mkfifo.test";
    INFO_PRINTF1( KFunc);
    TInt err;
    const char* wbuf = "testfifowrite";
    char rbuf[128];
    TInt fd = open (path, O_WRONLY);
    if ( fd < 0 )
        {
            ERR_PRINTF2( _L("Fifo open"),  fd);
            return -1;
        }
    err = write (fd, wbuf, (strlen(wbuf)+1));  
    if ( err < 0 )
       {
           ERR_PRINTF2( _L("write error :%d"), err);
           return -1;
       }  
    err = read (fd, rbuf, 128);  
    if ( err < 0 )
       {
           ERR_PRINTF2( _L("read error :%d"), err);
           return -1;
       }     
    return err;
}

// -----------------------------------------------------------------------------
// CTestMkfifo::CreateThreadL
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::CreateThreadL(  )
{
    _LIT(KFunc, "CreateThreadL");
    INFO_PRINTF1( KFunc);
    TInt threadType;
    TInt err = KErrNone;
    _LIT( KthreadType, "Param%d" );
     TBuf<8> pNameBuf;
     pNameBuf.Format(KthreadType,++iParamCnt);
	 TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, threadType);
	 if(!res)
	 {
	 	_LIT(Kerr , "Unable to retrieve threadType") ;
	 	INFO_PRINTF1(Kerr) ;
		return KErrGeneral; 	
	 }
    switch(threadType)
        {
        case 1: 
            err = pthread_create((unsigned int *)&iThreadId,(pthread_attr_t *)NULL,ThreadEntryFunctionW,(void*)this);
            User::After(500000);
            
            err = OpenFifo(TREAD);
           
            break;
        case 2:
            err = pthread_create((unsigned int *)&iThreadId,(pthread_attr_t *)NULL,ThreadEntryFunctionR,(void*)this);
            User::After(100000);
            err = OpenFifo(TWRITE);
            break;
        case 3:
        	err = pthread_create((unsigned int *)&iThreadId,(pthread_attr_t *)NULL,ThreadEntryFunctionWrite,(void*)this);
        	pthread_join(iThreadId, NULL);
			INFO_PRINTF1( _L("case3 called***********"));
			err = FifoRead();
		    break;
        default:
            err = KErrNone;
            break;            
        }
    if ( err )
        {
        ERR_PRINTF3 ( _L("pthread_create: %d %d"), err, errno);
        }
    return err;
}

// -----------------------------------------------------------------------------
// CTestMkfifo::ThreadEntryFunctionW
// -----------------------------------------------------------------------------
//    
void* CTestMkfifo::ThreadEntryFunctionW(void* arg)
{
    CTestMkfifo *self = static_cast<CTestMkfifo*> (arg);
    const char* path = "C:\\mkfifo.test";
      self->iBlocked = 1;
    TInt fd = open (path, O_WRONLY);
    if(fd > 0 )
    {
       self->iBlocked = 0;
       close(fd);
    }
    return NULL;
}    

// -----------------------------------------------------------------------------
// CTestMkfifo::ThreadEntryFunctionR
// -----------------------------------------------------------------------------
//
void* CTestMkfifo::ThreadEntryFunctionR(void* arg)
	{
    CTestMkfifo *self = static_cast<CTestMkfifo*> (arg);
    const char* path = "C:\\mkfifo.test";
    self->iBlocked = 1;
    TInt fd = open (path, O_RDONLY);
    
    if(fd > 0 )
    	{
       	self->iBlocked = 0;
       	close(fd);
    	}
    return NULL;
	}

// -----------------------------------------------------------------------------
// CTestMkfifo::ThreadEntryFunctionWrite
// -----------------------------------------------------------------------------
//
void* CTestMkfifo::ThreadEntryFunctionWrite(void* arg)
	{
	CTestMkfifo *self = static_cast<CTestMkfifo*> (arg);
    const char* path = "C:\\mkfifo.tst";
    const char* buf = "somejunk";
    arg = NULL;
    self->iBlocked = 1;
    TInt fd = open (path, O_WRONLY|O_CREAT);
    if(fd > 0 )
    	{
    	self->iBlocked = 0;
       	write(fd, buf, 9);
       	close(fd);
    	}
     return NULL;
	}    

// -----------------------------------------------------------------------------
// CTestMkfifo::OpenFifo
// -----------------------------------------------------------------------------
//
TInt CTestMkfifo::OpenFifo(TMode mode)
{
    _LIT(KFunc, "OpenFifo");
    INFO_PRINTF1(KFunc);
    CTestMkfifo *self = static_cast<CTestMkfifo*>(this);
    TInt err = KErrNone;
    TInt fd;
    const char* path = "C:\\mkfifo.test";
    
    
    if (mode == TREAD)
    {
    	fd = open(path, O_RDONLY|O_CREAT);
    }
    else // TWRITE
    {
    	fd = open(path, O_WRONLY);
    
    }
    User::After(500000);
    if(!self->iBlocked)
    {
        if(fd < 0)
    	   {
    	      self->ERR_PRINTF2 ( _L("open error :%d"), fd);
    	   }
    	   else
    	   {
    	    close(fd);
    		err = KErrNone;
    	   }
    }
    else
    {
    	close(fd);
   	    err = KErrGeneral;
    }
   
    return err;
}
    

TInt CTestMkfifo::IntgTest1()
{
	TInt err = DeleteFifo();
	if(err)
		{
		return err;
		}
	err = FifoCreate();
	return err;
}

TInt CTestMkfifo::IntgTest2()
{
	TInt err = DeleteFifo();
	if(err)
		{
		return err;
		}
	err = FifoCreate();
	if(err)
		{
		return err;
		}
	err = FifoCreate();
	return err;
}

TInt CTestMkfifo::IntgTest3()
{
	TInt err = DeleteFifo();
	if(err)
		{
		return err;
		}
	err = FifoCreate();
	if(err)
		{
		return err;
		}
	err = FifoCreateInNonDir();
	return err;
}

TInt CTestMkfifo::IntgTest4()
{
	TInt err = DeleteFifo();
	if(err)
		{
		return err;
		}
	err = FifoCreate();
	if(err)
		{
		return err;
		}
	err = CreateThreadL();
	return err;
}

TInt CTestMkfifo::IntgTest5()
{
	TInt err = DeleteFifo();
	if(err)
		{
		return err;
		}
	err = FifoCreate();
	if(err)
		{
		return err;
		}
	err = FifoOpenWNonBlock();
	return err;
}

TInt CTestMkfifo::IntgTest6()
{
	TInt err = DeleteFifo();
	if(err)
		{
		return err;
		}
	err = FifoCreate();
	if(err)
		{
		return err;
		}
	err = FifoOpenRNonBlock();
	return err;
}

TInt CTestMkfifo::IntgTest7()
{
	TInt err = DeleteFifo();
	if(err)
		{
		return err;
		}
	err = FifoCreate();
	if(err)
		{
		return err;
		}
	err = FifoClose();
	if(err)
		{
		return err;
		}
	err = DeleteFifo();
	return err;
}

//********************************************************************//
//******CTestMkfifo::TMkfifoReaderWriter******************************//
//**mkfifo test using pthreads - 2 threads one reader and one writer**//
//********************************************************************//
static char path[MAXPATHLEN+1];
const mode_t mode = 0666;
int bufsize;
static void *reader(void* arg) 
{
	arg = arg;
	int fd;
	int n;
	char *buf;
	TInt ret = KErrNone;
	if ((fd = open(path, O_RDONLY, mode)) < 0) 
		{
		char *ptr = strerror(errno);
		unlink(path);
		ret = -1;
		}	
	if ((buf = (char *)malloc(bufsize)) == NULL)
			{
			char *ptr = strerror(errno);
			}
	do
	{
		if ((n = read(fd, buf, bufsize)) < 0)
			{
			ret = -1;
			}
		bufsize -= n;
	}while(bufsize != 0);
	close(fd);
	free(buf);	
	return (void *)ret;
}

static void *writer(void *arg) 
{
	arg = arg;
	char *buf;
	int fd;
	int n = 0;
	char t = 'A';
	TInt ret = KErrNone;
	if ((fd = open(path, O_WRONLY, mode)) < 0)
		{
		char *ptr = strerror(errno);
		unlink(path);
		ret = -1;
		}
	if ((buf = (char *)malloc(bufsize)) == NULL)
			{
			char *ptr = strerror(errno);
			}			
	memset(buf, t, bufsize);
	n = write(fd, buf, bufsize);
	if (n < 0) 
		{
		ret = -1;
		}
	close(fd);
	free(buf);
	return (void*)ret;
}

TInt CTestMkfifo::TMkfifoReaderWriter()
{
	pthread_t new_th[2];
	bufsize = 1024;
	TInt ret = KErrNone;
	sprintf(path, "c:\\fifo.%d.%d", getpid(), 1);	
	
	if (mkfifo(path, mode) < 0)
		{
		INFO_PRINTF2(_L("errno %d\n"), errno);
		char *ptr = strerror(errno);
		INFO_PRINTF3(_L("mkfifo(%s) %s\n"), path, ptr);
		ret = -1;
		}
	 // Create a write thread. 
	 if(pthread_create(&new_th[0], NULL, writer, NULL) != 0)
	 	{
	  	perror("Error creating write thread \n");
	  	return -1;
	 	}
	 // Create a read thread. 
	 if(pthread_create(&new_th[1], NULL, reader, NULL) != 0)
	 	{
	  	perror("Error creating read thread \n");
	  	return -1;
	 	}
	 // Wait for threads to return 	 
	 if(pthread_join(new_th[0], NULL) != 0)//write thread returning
	 	{
	  	perror("Error in pthread_join() \n");
	 	}
	 if(pthread_join(new_th[1], NULL) != 0)//read thread returning
		 {
		 perror("Error in pthread_join() \n");
		 }
	unlink(path);	
	return ret;
}


//********************************************************************//
//******CTestMkfifo::TMkfifoReaderWriter1*****************************//
//**mkfifo test using RThreads - 2 threads one reader and one writer**//
//********************************************************************//
int nread = 0;
int nwrite = 0;

static int reader1(void *arg) 
{
	arg = arg;
	int fd;
	char *buf;
	int n = 0;
	TInt ret = KErrNone;
	
	if ((fd = open(path, O_RDONLY, mode)) < 0) 
		{
		char *ptr = strerror(errno);
		unlink(path);
		ret = -1;
		}
	if ((buf = (char *)malloc(bufsize)) == NULL)
			{
			char *ptr = strerror(errno);
			}
	do
	{
		if ((n = read(fd, buf, bufsize)) < 0)
			{
			ret = -1;
			}
		bufsize -= n;
		nread += n;
	}while(bufsize != 0);
	close(fd);
	free(buf);	
	return ret;
}

static int writer1(void *arg) 
{
	arg = arg;
	int n = 0;
	char *buf;
	int fd;
	char t = 'A';
	TInt ret = KErrNone;
	if ((fd = open(path, O_WRONLY, mode)) < 0)
		{
		char *ptr = strerror(errno);
		unlink(path);
		ret = -1;
		}
	if ((buf = (char *)malloc(bufsize)) == NULL)
			{
			char *ptr = strerror(errno);
			}			
	memset(buf, t, bufsize);
	if ((n = write(fd, buf, bufsize)) < 0) 
		{
		ret = -1;
		}
	else
		{
		nwrite = n;
		}	
	close(fd);
	free(buf);
	return ret;
}



TInt CTestMkfifo::TMkfifoReaderWriter1()
{
	bufsize = 1024;
	TInt ret = KErrNone;
	sprintf(path, "c:\\fifo.%d.%d", getpid(), 1);	
	
	if (mkfifo(path, mode) < 0)
		{
		INFO_PRINTF2(_L("errno %d\n"), errno);
		char *ptr = strerror(errno);
		INFO_PRINTF3(_L("mkfifo(%s) %s\n"), path, ptr);
		ret = -1;
		}
	
	 // Create a write thread. 
	 RThread read, write;
	 write.Create(_L("write"), writer1, 2000, 2000, 4000, NULL);
	 write.Resume();
	 // Create a read thread. 
	 read.Create(_L("read"), reader1, 2000, 2000, 4000, NULL);
	 read.Resume();
 	 // Wait for threads to return 
	 TRequestStatus swrite, sread;
	 write.Logon(swrite);
	read.Logon(sread);
	User::WaitForAnyRequest();
	User::WaitForAnyRequest();
	INFO_PRINTF3(_L("\nAll done\n%d %d\n"), nread, nwrite);	
	unlink(path);	
	return ret;
}
TInt CTestMkfifo::FifoCreateNULL1(  )
{
	_LIT(KFunc, "FifoCreateNULL1");
    INFO_PRINTF1( KFunc);
    
    TInt err = mkfifo ("", 0666);
    if ( err )
        {
        	if ( errno == ENOENT )
            {
            err = KErrNone;
            }
          else
            {
            ERR_PRINTF2( _L("mkfifo error: %d"), err);
            }
        }
    return err;
}

TInt CTestMkfifo::FifoCreate_invalid(  )
{
	_LIT(KFunc, "FifoCreate_invalid");
    INFO_PRINTF1( KFunc);
    
	char mbcbuf[64];
   	mbcbuf[0] = (unsigned char)0x08;
    
    TInt err = mkfifo (mbcbuf, 0666);
    if ( err )
        {
        	if ( errno == EILSEQ )
            {
            err = KErrNone;
            }
          else
            {
            ERR_PRINTF2( _L("mkfifo error: %d"), err);
            }
        }
    return err;
}
//  End of File

