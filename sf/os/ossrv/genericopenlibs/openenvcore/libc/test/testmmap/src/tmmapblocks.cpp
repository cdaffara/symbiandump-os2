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



#include "tmmap.h"
#include <errno.h>
TInt CTestMmap::memmap()
	{
	int len;
	int prot;
	int flags;
	TPtrC String ;
	int offset;
	char* mapaddr;
	int fperm;

    _LIT( Klen, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), Klen, len);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read length") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	_LIT( Kprot, "Parameter2" );
	res = GetIntFromConfig(ConfigSection(), Kprot, prot);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read prot field") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    _LIT( Kflags, "Parameter3" );
	res = GetIntFromConfig(ConfigSection(), Kflags, flags);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read flag field") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 
	_LIT( KString, "Parameter4" );
	res = GetStringFromConfig(ConfigSection(), KString, String);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read file name") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}	 
	TBuf8<100> string1;
	string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0'; 
	 
	_LIT( Koffset, "Parameter5" );
	res = GetIntFromConfig(ConfigSection(), Koffset, offset);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read offset field") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	_LIT( Kfperm, "Parameter6" );
	res = GetIntFromConfig(ConfigSection(), Kfperm, fperm);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read file open perm") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
	 
	int fd1;
	switch(fperm)
		{
		case O_RDONLY:
			fperm = fperm | O_CREAT;
			break;
		case O_WRONLY:
			fperm = fperm | O_CREAT;
			break;
		case O_RDWR:
			fperm = fperm | O_CREAT;
			break;
		default:
			break;		
		}
		
    if((fd1 = open(file1, fperm, 0666) ) < 0)
		{
    	_LIT(Kerr , "Failed to open file") ;
      	INFO_PRINTF1(Kerr) ;
		INFO_PRINTF2(_L("%d"),errno);
     	return KErrGeneral ;
		}
    
    _LIT(Kerr1 , "File open succeeded on input file") ;
    INFO_PRINTF1(Kerr1) ;
    mapaddr = (char*)mmap((void*)0, len, prot, flags, fd1, offset);
    if(mapaddr == MAP_FAILED)
		{
    	_LIT(Kerr ,"mmap file to memory failed");
     	INFO_PRINTF1(Kerr) ;
    	return KErrGeneral;
		}        
    
    _LIT(Kerr2 , "mmap on input file succeeded") ;
	INFO_PRINTF1(Kerr2) ;    
    
	if(mapaddr != MAP_FAILED)
		{
	 	strcpy(mapaddr, "This is a write through mapped memory");
		}
	_LIT(Kerr3 , "write into mapped memory succeeded") ;
	INFO_PRINTF1(Kerr3) ;    
    
	if(-1 == msync(mapaddr, len, MS_SYNC))
		{
		_LIT(Kerr ,"msync from memory to file failed") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
		}
	_LIT(Kerr4 , "msync on input file succeeded") ;
	INFO_PRINTF1(Kerr4) ;    
	if(-1 == munmap(mapaddr, len))
		{
		_LIT(Kerr ,"munmap from file failed") ;
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
		}
	  
	_LIT(Kerr5 , "munmap on input file succeeded") ;
	INFO_PRINTF1(Kerr5) ;
    
		/*close file*/
	if(0 > close(fd1))
		{
    	_LIT(Kerr , "Failed to close file");
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
		}
    
    _LIT(Kerr6 , "close on input file succeeded") ;
    INFO_PRINTF1(Kerr6) ;   
	return KErrNone;
	}




TInt CTestMmap::memmap1(  )
	{
	int len = 4096; // a page length
	int prot = 2;   // Read and write
	int flags = 1;  // shared across process
	TPtrC String ;
	int offset[5] = { 0, 4096, 8192, 12288, 16384};			
	char* mapaddr[5], *retval;
	int fperm = 0;
	int fd1;
		
	fperm = fperm | O_RDWR | O_CREAT;
	
	/*read the parameters*/	
	_LIT( KString, "Parameter1" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String);
	if(!res)
		{
		_LIT(Kerr , "Failed to read file name") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	
	TBuf8<100> string1;
	string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0'; 
	
	/*open file and create memmap*/
	if((fd1 = open(file1, fperm, 0666) ) < 0)
		{
    	_LIT(Kerr , "Failed to open file");
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
		}
    //create mmap sequentially
    for(int i=0; i<5; i++)
		{
    	retval = (char*)mmap((void*)0, len, prot, flags, fd1, offset[i]);
	    if(retval == MAP_FAILED)
			{
	    	_LIT(Kerr ,"mmap file to memory failed");
	    	close(fd1);
	     	INFO_PRINTF1(Kerr) ;
	     	for(int j =0; j<=i; j++)
				{
	     		if(-1 == munmap(mapaddr[j], len))
					{
					_LIT(Kerr ,"munmap from file failed") ;
		     		INFO_PRINTF1(Kerr) ;
		     		return KErrGeneral ;
					}
				}
	    	return KErrGeneral;
	    	}
	    else
			{
	    	mapaddr[i] = retval;
			}	      	    	
		}
    
    if(mapaddr != MAP_FAILED)
		{	 
		//write and sync all nodes	    
				for(int i=0;i<5;i++)
					{
		    		strcpy(mapaddr[i], "This is a write to all mapped memory nodes");	
		    		if(-1 == msync(mapaddr[i], len, MS_SYNC))
						{
						_LIT(Kerr ,"msync from memory to file failed") ;
			     		INFO_PRINTF1(Kerr) ;
			     		return KErrGeneral ;
						}					
					}
				
			}
		//all mapped locations are unmapped sequentially				
				for(int i=0;i<5;i++)
					{		    	
		    		if(-1 == munmap(mapaddr[i], len))
						{
						_LIT(Kerr ,"munmap from file failed") ;
			     		INFO_PRINTF1(Kerr) ;
			     		return KErrGeneral ;
						}					
					}
	
	/*close file*/
	if(0 > close(fd1))
		{
    	_LIT(Kerr , "Failed to close file");
     	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
		}
    return KErrNone;
	}

TInt CTestMmap::memmap2(  )
	{
	int len;
	int prot;
	int flags;
	TPtrC String ;
	int offset;
	char* mapaddr;
	int fperm;
	TInt ret=KErrNone;
	_LIT( Klen, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), Klen, len);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read length") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	_LIT( Kprot, "Parameter2" );
	res = GetIntFromConfig(ConfigSection(), Kprot, prot);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read prot field") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
    _LIT( Kflags, "Parameter3" );
	res = GetIntFromConfig(ConfigSection(), Kflags, flags);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read flag field") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 
	_LIT( KString, "Parameter4" );
	res = GetStringFromConfig(ConfigSection(), KString, String);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read file name") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	TBuf8<100> string1;
	string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0'; 
	_LIT( Koffset, "Parameter5" );
	res = GetIntFromConfig(ConfigSection(), Koffset, offset);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read offset field") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	_LIT( Kfperm, "Parameter6" );
	res = GetIntFromConfig(ConfigSection(), Kfperm, fperm);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read file open perm") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 
	int fd1;
	switch(fperm)
		{
		case O_RDONLY:
			fperm = fperm | O_CREAT;
			break;
		case O_WRONLY:
			fperm = fperm | O_CREAT;
			break;
		case O_RDWR:
			fperm = fperm | O_CREAT;
			break;
		default:
			break;		
		}
    if((fd1 = open(file1, fperm, 0666) ) < 0)
		{
    	_LIT(Kerr , "Failed to open file") ;
      	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
		}
    
    _LIT(Kerr1 , "File open succeeded on input file") ;
    INFO_PRINTF1(Kerr1) ;    
    
    mapaddr = (char*)mmap((void*)0, len, prot, flags, fd1, offset);
    if(mapaddr == MAP_FAILED)
		{    
     	if(EINVAL == errno)
     		{
     		_LIT(Kerr , "Passed: Returned - EINVAL") ;
      		INFO_PRINTF1(Kerr) ;
     		}
    	ret = KErrNone;
		}
    return ret;                		 	
	}

TInt CTestMmap::memmap3(  )
	{
	int len;
	int prot;
	int flags;
	TPtrC String ;
	int offset;
	char* mapaddr;
	int fperm;
	TInt ret=KErrNone;
	_LIT( Klen, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), Klen, len);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read length") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	_LIT( Kprot, "Parameter2" );
	res = GetIntFromConfig(ConfigSection(), Kprot, prot);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read prot field") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 _LIT( Kflags, "Parameter3" );
	 res = GetIntFromConfig(ConfigSection(), Kflags, flags);
	 if(!res)
		{
	 	_LIT(Kerr , "Failed to read flag field") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 
	 _LIT( KString, "Parameter4" );
	 res = GetStringFromConfig(ConfigSection(), KString, String);
	 if(!res)
		{
	 	_LIT(Kerr , "Failed to read file name") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	TBuf8<100> string1;
	string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0'; 
	_LIT( Koffset, "Parameter5" );
	res = GetIntFromConfig(ConfigSection(), Koffset, offset);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read offset field") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	_LIT( Kfperm, "Parameter6" );
	res = GetIntFromConfig(ConfigSection(), Kfperm, fperm);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read file open perm") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 
	int fd1;
	switch(fperm)
		{
		case O_RDONLY:
			fperm = fperm | O_CREAT;
			break;
		case O_WRONLY:
			fperm = fperm | O_CREAT;
			break;
		case O_RDWR:
			fperm = fperm | O_CREAT;
			break;
		default:
			break;		
		}
    if((fd1 = open(file1, fperm, 0666) ) < 0)
		{
    	_LIT(Kerr , "Failed to open file") ;
      	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
		}
    
    _LIT(Kerr1 , "File open succeeded on input file") ;
    INFO_PRINTF1(Kerr1) ;    
    
    mapaddr = (char*)mmap((void*)0, len, prot, flags, fd1, offset);
    if(mapaddr == MAP_FAILED)
		{	    	
     	if(ENOTSUP == errno)
     		{
     		_LIT(Kerr , "Passed: Returned - ENOTSUP") ;
      		INFO_PRINTF1(Kerr) ;
     		}
    	ret = KErrNone;
		}
    return ret;                		 	
	}

TInt CTestMmap::memmap4(  )
	{
	int len;
	int prot;
	int flags;
	TPtrC String ;
	int offset;
	char* mapaddr;
	int fperm;
	TInt ret=KErrNone;
	_LIT( Klen, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), Klen, len);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read length") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	_LIT( Kprot, "Parameter2" );
	res = GetIntFromConfig(ConfigSection(), Kprot, prot);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read prot field") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 _LIT( Kflags, "Parameter3" );
	 res = GetIntFromConfig(ConfigSection(), Kflags, flags);
	 if(!res)
		{
	 	_LIT(Kerr , "Failed to read flag field") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 
	 _LIT( KString, "Parameter4" );
	 res = GetStringFromConfig(ConfigSection(), KString, String);
	 if(!res)
		{
	 	_LIT(Kerr , "Failed to read file name") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}

	TBuf8<100> string1;
	string1.Copy(String);
	char* file1 = (char*) string1.Ptr();
	file1[string1.Length()]='\0'; 
	_LIT( Koffset, "Parameter5" );
	res = GetIntFromConfig(ConfigSection(), Koffset, offset);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read offset field") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	_LIT( Kfperm, "Parameter6" );
	res = GetIntFromConfig(ConfigSection(), Kfperm, fperm);
	if(!res)
		{
		_LIT(Kerr , "Failed to read file open perm") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
		}
	 
	int fd1;
	switch(fperm)
		{
		case O_RDONLY:
			fperm = fperm | O_CREAT;
			break;
		case O_WRONLY:
			fperm = fperm | O_CREAT;
			break;
		case O_RDWR:
			fperm = fperm | O_CREAT;
			break;
		default:
			break;		
		}
    if((fd1 = open(file1, fperm, 0666) ) < 0)
		{
    	_LIT(Kerr , "Failed to open file") ;
      	INFO_PRINTF1(Kerr) ;
     	return KErrGeneral ;
		}
    
    _LIT(Kerr1 , "File open succeeded on input file") ;
    INFO_PRINTF1(Kerr1) ;    
    
    mapaddr = (char*)mmap((void*)0, len, prot, flags, fd1, offset);
    if(mapaddr == MAP_FAILED)
		{    	
     	if(EACCES == errno)
     		{
     		_LIT(Kerr , "Passed: Returned - EACCES") ;
      		INFO_PRINTF1(Kerr) ;
     		}
    	ret = KErrNone;
		}
    return ret;                		 	
	}	


TInt CTestMmap::memmap5(  )
	{
	#define INPUTFILE "C:\\abc.xml"
	#define OUTPUTFILE "C:\\xyz.xml"
	int fdin, fdout, i;
	char *src, *dst=NULL;
	struct stat statbuf;
	TInt ret = KErrNone;
	
	for(i=1; i<3; i++)
	{
		INFO_PRINTF2(_L("Iteration number (%d) starts\n"), i);
			
		if ((fdin = open(INPUTFILE, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0)
			{
			INFO_PRINTF2(_L("OPEN error for fdin file%s\n"), INPUTFILE);
			ret = -1;
			}
		if (write(fdin, "Hellowolrd", 6) == -1)
			{
			INFO_PRINTF1(_L("write error\n"));
			ret = -1;			
			}
		if ((fdout = open(OUTPUTFILE, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0)
			{
			INFO_PRINTF2(_L("Open error for fdout file%s\n"),OUTPUTFILE);
			ret = -1;
			}
		if (write(fdout, "symbian world", 6) == -1)
			{
			INFO_PRINTF1(_L("write error\n"));
			ret = -1;			
			}		
	
		if (fstat(fdin, &statbuf) < 0)
			{
			INFO_PRINTF1(_L("fstat error\n"));
			ret = -1;
			}
	
		if (lseek(fdout, statbuf.st_size - 1, SEEK_SET) == -1)
			{
			INFO_PRINTF1(_L("lseek error\n"));
			ret = -1;
			}	
		/* write a dummy byte at the last location */
		if (write(fdout, "", 1) != 1)
			{
			INFO_PRINTF1(_L("write error\n"));
			ret = -1;			
			}
	
		if ((src = (char*)mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == (caddr_t) - 1)
			{
			char *ptr = strerror(errno);
			INFO_PRINTF3(_L("mmap error for input, error(%d), %s\n"), errno, ptr);
			ret = -1;
			}
		
    	if ((dst = (char*)mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0)) == (caddr_t) - 1)
			{
			char *ptr = strerror(errno);
			INFO_PRINTF3(_L("mmap error for output, error(%d), %s\n"), errno, ptr);
			ret = -1;
			}
	
		memcpy(dst, src, statbuf.st_size);
	
		if (munmap(src, statbuf.st_size) == -1)
			{
			INFO_PRINTF1(_L("Error munmap src\n"));
			ret = -1;
			}		
		if(close(fdin) == -1)
			{
			INFO_PRINTF1(_L("Error closing fdin"));
			ret = -1;			
			}	
		if (munmap(dst, statbuf.st_size) == -1)
			{
			INFO_PRINTF1(_L("Error munmap dst\n"));
			ret = -1;
			}		
		if(close(fdout) == -1)
			{
			INFO_PRINTF1(_L("Error closing fdin"));
			ret = -1;
			}	
		if (unlink(INPUTFILE) == -1)
			{
			INFO_PRINTF2(_L("unlink(%s)\n"), INPUTFILE);
			ret = -1;
			}
		if (unlink(OUTPUTFILE) == -1)
			{
			INFO_PRINTF2(_L("unlink(%s)\n"), OUTPUTFILE);
			ret = -1;
			}		
		INFO_PRINTF2(_L("Iteration number (%d) ends\n"), i);
		} 
	return ret;            		 	
	}


TInt CTestMmap::memmap6(  )
	{
     int fdin, fdout;
     char *src, *dst;
     struct stat statbuf;
     TInt ret=KErrGeneral;

     char input_file[] = "c:\\data\\input";
     char output_file[] = "c:\\data\\output";

     /* open the input file */
     if ((fdin = open (input_file, O_RDONLY)) < 0)
	{
       		ERR_PRINTF1 (_L("Can't open input file for reading"));
       		return errno ;	
	}

     /* open/create the output file */
     if ((fdout = open (output_file, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0)
	{
      		ERR_PRINTF1 (_L("can't create output for writing"));
		close(fdin);
		remove("c:\\data\\input");
		return errno;
	}

     /* find size of input file */
     if (fstat (fdin,&statbuf) < 0)
	{
       		ERR_PRINTF1 (_L("fstat error"));
		ret=errno;
		goto memmap6_close;
	}

     /* go to the location corresponding to the last byte */
     if (lseek (fdout, statbuf.st_size - 1, SEEK_SET) == -1)
	{
       		ERR_PRINTF1 (_L("lseek error"));
		ret=errno;
		goto memmap6_close;	
	}
     
     /* write a dummy byte at the last location */
     if (write (fdout, "", 1) != 1)
	{
       		ERR_PRINTF1 (_L("write error"));
		ret=errno;
		goto memmap6_close;	
	}
     /* mmap the input file */
     if ((src = (char*)mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == (caddr_t) -1)
	{
       		ERR_PRINTF1 (_L("mmap error for input"));
		ret=errno;
		goto memmap6_close;	
	}
     /* mmap the output file */
     if ((dst = (char*)mmap (0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0)) == (caddr_t) -1)
	{
       		ERR_PRINTF1 (_L("mmap error for output"));
		ret=errno;
		goto memmap6_close;	
	}
     /* this copies the input file to the output file */
     memcpy (dst, src, statbuf.st_size);
     
     if(munmap(src, statbuf.st_size) != 0)
	{
         	ERR_PRINTF1 (_L("munmap error for input"));
		ret=errno;
		goto memmap6_close;	
	}
     
     if(munmap(dst, statbuf.st_size) != 0)
	{
        	ERR_PRINTF1 (_L("munmap error for output"));
		ret=errno;
		goto memmap6_close;	
	}
     else
	{
	 	 INFO_PRINTF1(_L("Test case passed"));
	  	 ret = KErrNone;
	}

memmap6_close:
    close(fdout);
    close(fdin);
    remove("c:\\data\\output");
    remove("c:\\data\\input");
    return ret; 
}


// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
