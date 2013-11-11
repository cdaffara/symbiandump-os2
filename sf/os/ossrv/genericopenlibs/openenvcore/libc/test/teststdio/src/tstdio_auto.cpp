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



#include "tstdio.h"

#define MAX_LEN  20
// ============================ MEMBER FUNCTIONS ===============================


/*--------------------- FILE-RELATED OPERATIONS -----------------------------*/
/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::remove1
API Tested			: remove
TestCase Description: The test case tries to remove an existing file which is 
					  not opened by any process
-------------------------------------------------------------------------------
*/
TInt CTestStdio::remove1()
	{
	INFO_PRINTF1(_L("fscanf returned:\n%d"));

	//Same as for unlink or rmdir. See stdlib test cases.
	char name[50] = "C:\\Logs\\remove1.txt";
	FILE *fp = NULL;
	int retval = 0;
	fp = fopen(name, "w");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}

	fclose(fp);

	retval  = remove(name);
	if (retval<0)
		{
		INFO_PRINTF2(_L("remove failed - error %d"), errno);
		}

	return retval;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::remove2
API Tested			: remove
TestCase Description: Create one file, try to remove the opened file and then 
					  close it. It should be removed after the close.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::remove2()
	{

	INFO_PRINTF1(_L("remove2"));
	
	char name[16] = "c:\\remove2.txt";
	FILE *fp = NULL;
	int retval = 0;	
	
	fp = fopen (name, "w");
	if (fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));	
		return -1;
		}
	
	retval  = remove(name);
	if (retval<0)
		{
		INFO_PRINTF2(_L("remove failed - error %d"), errno);
		fclose(fp);
		remove(name);
		retval = 0;
		return retval;
		}

	fclose (fp);
	return KErrNone;
	}



/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::rename1
API Tested			: rename
TestCase Description: create one file and move it to new file using rename()
-------------------------------------------------------------------------------
*/
TInt CTestStdio::rename1()
	{
	INFO_PRINTF1(_L("rename1"));
	char oldpath[14] = "c:\\path1.txt";
	char newpath[14] = "c:\\path2.txt";
	FILE *fp = NULL;
	int retval = 0;	
	
	fp = fopen (oldpath, "w");
	if (fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));	
		return -1;
		}	
	fclose (fp);

	retval  = rename(oldpath, newpath);

	if (retval<0)
		{
		INFO_PRINTF1(_L("rename failed - error "));
		unlink(oldpath);
		}

	unlink(newpath);
	return retval;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::tempnam1
API Tested			: tempnam
TestCase Description: This function creates a unique temporary file name in user 
					  specified directory.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::tempnam1()
	{
	INFO_PRINTF1(_L("tempnam1"));
	TInt res = KErrNone;
	FILE *stream = NULL;
	char* name = "c:\\tmp";
	char* file;
	if (mkdir(name, 0666) == -1)
		{
		if (errno != EEXIST)
			{
			res = KErrGeneral;
			return res;
			}
		}
   	for (int i = 1; i <= 10; i++) 
		{
		if ((file = tempnam(name,"wow.")) == NULL)
    		{
			INFO_PRINTF1(_L("tempnam() failed\n"));
    		res = KErrGeneral;	
    		}
		else 
    		{
			INFO_PRINTF2(_L("Creating tempfile %d"),i);
       		if ((stream = fopen(file,"wb+")) == NULL)
				{
				INFO_PRINTF1(_L("Could not open temporary file\n"));
    			res = KErrGeneral;	
				}
       		else
				{
				int bytes = fprintf(stream, "tempnam1");
				fflush(stream);
				if(bytes != 8)  // 8 bytes is string length of 'tempnam1'
					{
					INFO_PRINTF1(_L("Error in fprintf"));
					res = KErrGeneral;
					}
				else
					{
					char buf[10];
					fseek(stream, -8, SEEK_CUR); //reset file position indicator
					bytes = fscanf(stream, "%s", buf);
					
					if(strlen(buf) != 8)
						{
						INFO_PRINTF1(_L("Error in fscanf, Test case failed"));
						fclose(stream);
						unlink(file);
						free(file);
						return KErrGeneral;
						}
						
					if(strcmp(buf, "tempnam1"))
						{
						INFO_PRINTF1(_L("String read from file does not match the string - tempnam1"));
						res = KErrGeneral;
						}
					}
				fclose(stream);
				}
       		unlink(file);
			}
		free(file);
		}
	return res;    
	}
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::tempnam2
API Tested			: tempnam
TestCase Description: This function creates a unique temporary file name in 
			          implementation specified directory. If dir = NULL P_tmpdir
			          (C:\\Private\\UID3)which is implementation defined shall 
			          be used and if pfx = NULL file name generated shall be 
			          prefixed as tmp.xxxxxx
-------------------------------------------------------------------------------
*/
TInt CTestStdio::tempnam2()
	{
	INFO_PRINTF1(_L("tempnam2"));
	TInt res = KErrNone;
	FILE *stream = NULL;
	char* file;
   	for (int i = 1; i <= 10; i++) 
		{
		if ((file = tempnam(NULL,NULL)) == NULL)
    		{
			INFO_PRINTF1(_L("tempnam() failed\n"));
    		res = KErrGeneral;	
    		}
		else 
    		{
			INFO_PRINTF2(_L("Creating tempfile %d"),i);
       		if ((stream = fopen(file,"wb+")) == NULL)
				{
				INFO_PRINTF1(_L("Could not open temporary file\n"));
    			res = KErrGeneral;	
				}
       		else
				{
				int bytes = fprintf(stream, "tempnam2");
				fflush(stream);
				if(bytes != 8)  // 8 bytes is string length of 'tempnam1'
					{
					INFO_PRINTF1(_L("Error in fprintf"));
					res = KErrGeneral;
					}
				else
					{
					char buf[10];
					fseek(stream, -8, SEEK_CUR); //reset file position indicator
					bytes = fscanf(stream, "%s", buf);
					
					if(strlen(buf) != 8)
						{
						INFO_PRINTF1(_L("Error in fscanf, Test case failed"));
						fclose(stream);
						unlink(file);
						free(file);
						return KErrGeneral;
						}
						
					if(strcmp(buf, "tempnam2"))
						{
						INFO_PRINTF1(_L("String read from file does not match the string - tempnam1"));
						res = KErrGeneral;
						}
					}
				fclose(stream);
				}
       		unlink(file);
			}
		free(file);
		}
	return res;    
	}
/*--------------- END OF FILE-RELATED OPERATIONS -----------------------------*/

	

/*--------------------- STREAM-RELATED OPERATIONS ---------------------------*/

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fclose1
API Tested			: fclose
TestCase Description: Just open a file and close it
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fclose1()
	{
	INFO_PRINTF1(_L("fclose1"));
	FILE *fp;

	fp = fopen("c:\\input.txt", "w");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}

	fprintf(fp, "This is the first line\n"); 
	INFO_PRINTF1(_L("Wrote to file\n")); 
	//do something here
	fclose(fp);	
	return KErrNone;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fflush1
API Tested			: fflush
TestCase Description: Create a file, put the mode to full buffering then call 
					  fflush
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fflush1()
	{
	INFO_PRINTF1(_L("fflush1"));
	FILE *fp;
	int retval = 0;
	char name[20] = "c:\\fflush1.txt";

	fp = fopen(name, "w");	
	if(fp == NULL)
		{		
		INFO_PRINTF1(_L("Error : File open"));
		return -1;
		}

	setvbuf(fp, NULL, _IOFBF, 100);  // set to full buffering with NULL buffer	
	fprintf(fp, "we are trying to buffer 100 characters at once with NULL buffer. ");
	
	retval = fflush(fp);
	if (retval)
		{		
		INFO_PRINTF1(_L("fflush failed"));
		fclose(fp);
		unlink(name);
		return -1;
		}

	fprintf(fp, "Again.. we are trying to buffer 100 characters at once with NULL buffer. ");
	retval = fflush(fp);
	if (retval)
		{		
		INFO_PRINTF1(_L("fflush failed"));
		fclose(fp);
		unlink(name);
		return -1;
		}

	fprintf(fp, "Once Again.. we are trying to buffer 100 characters at once with NULL buffer\n");
	retval = fflush(fp);
	if (retval)
		{		
		INFO_PRINTF1(_L("fflush failed"));
		fclose(fp);
		unlink(name);
		return -1;
		}

	fclose(fp);
	unlink(name);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fopen1
API Tested			: fopen
TestCase Description: Create a file using fopen() using absolute path and write
					  something in to it. Then close it. Open the same file 
					  again in append mode and write something in to it
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fopen1()
	{
	INFO_PRINTF1(_L("fopen1"));
	FILE *fp;	
	char name[20] = "c:\\fopen1.txt";

	if ((fp = fopen (name, "w")) == NULL) 
		{
		INFO_PRINTF1(_L("Error creating file"));
		return -1;
		}
		INFO_PRINTF1(_L("Opened file c:\\fopen1.txt\n"));

	fprintf(fp, "This is the first line\n"); 
	INFO_PRINTF1(_L("Wrote to file\n")); 
	fclose (fp);
	INFO_PRINTF1(_L("Closed file\n")); 
	if ((fp = fopen (name, "a")) == NULL) 
		{
		INFO_PRINTF1(_L("Error creating file"));
		return -1;
		} 
	INFO_PRINTF1(_L("Opened file c:\\fopen1.txt for appending\n")); 
	fprintf(fp, "This is the second line\n");
	fclose (fp);
	unlink(name);
	return KErrNone;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fopen2
API Tested			: fopen
TestCase Description: Create a file using fopen() using relative path and write
					  something in to it. Then close it. Open the same file 
					  again in append mode and write something in to it
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fopen2()
	{
	INFO_PRINTF1(_L("fopen2"));

	FILE *fp;	
	chdir("C:\\System\\");
	if ((fp = fopen ("..\\input.txt", "w")) == NULL) //relative path
	{ 
	INFO_PRINTF1(_L("Error creating file"));
	INFO_PRINTF2(_L("The err no is set to %d\n"),errno);
	return -1;
	} 
	INFO_PRINTF1(_L("Opened file c:\\input.txt\n"));
	fprintf(fp, "This is the first line\n");
	INFO_PRINTF1(_L("Wrote to file\n")); 
	
	fclose (fp);
	INFO_PRINTF1(_L("Closed file\n")); 
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fopen3
API Tested			: fopen
TestCase Description: Open a file in read-only mode and try to write something 
					  into it
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fopen3()
	{
	INFO_PRINTF1(_L("fopen3"));
	FILE *fp;
	int retval = 0;

	if ((fp = fopen ("C:\\input.txt", "w")) == NULL) 
		{		
		INFO_PRINTF1(_L("Error opening file"));
		return -1;
		}	
	
	fclose(fp);	
	if ((fp = fopen ("C:\\input.txt", "r")) == NULL) 
		{		
		INFO_PRINTF1(_L("Error opening file"));
		return -1;
		}	
	retval = fputc('c', fp); 
	
	
	if (retval == EOF)
		{
		retval = KErrNone; //Negative test case, So Success
		}
	else
		{
		retval = -1;
		}

	fclose(fp);
	return retval;
	}
/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::setbuf1
API Tested			: setbuf
TestCase Description: Test setbuf in full buffering mode
-------------------------------------------------------------------------------
*/
TInt CTestStdio::setbuf1()
	{
	INFO_PRINTF1(_L("setbuf1"));
	FILE *fp;
	char FullBuf[bufSize];
	char name[20] = "c:\\setbuf1.txt";
	fp = fopen(name, "w");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L("fopen failed\n")); 
		return -1;
		} 
	setbuf(fp, FullBuf);  // Fully buffered
	if (ferror(fp))
		{ 
		INFO_PRINTF1(_L("setbuf failed\n"));  
		
		fclose(fp);
		unlink(name);
		return -1;
		}
	fprintf(fp, "we are trying to buffer 20 characters at once. ");
	fprintf(fp, "Again.. we are trying to buffer 20 characters at once. ");
	fprintf(fp, "Once Again.. we are trying to buffer 20 characters at once\n");	

	fclose(fp);
	unlink(name);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::setbuf2
API Tested			: setbuf
TestCase Description: Test setbuf in unbuffered mode
-------------------------------------------------------------------------------
*/
TInt CTestStdio::setbuf2()
	{
	
	INFO_PRINTF1(_L("setbuf2"));
	FILE *fp;	
	char name[20] = "c:\\setbuf2.txt";

	fp = fopen(name, "w");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L("fopen failed\n")); 
		return -1;
		}
	setbuf(fp, NULL);  // unbuffered
	if (ferror(fp))
		{ 
		INFO_PRINTF1(_L("setbuf failed\n"));  
		fclose(fp);
		unlink(name);
		return -1;
		}
	fprintf(fp, "write to a file with no buffering");
	fprintf(fp, "Again.. write to a file with no buffering\n");

	fclose(fp);
	unlink(name);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::setvbuf1
API Tested			: setvbuf
TestCase Description: Test setvbuf in line buffering mode with and without
					  buffer (NULL buffer)
-------------------------------------------------------------------------------
*/
TInt CTestStdio::setvbuf1()
	{
	INFO_PRINTF1(_L("setvbuf1"));

	FILE *fp;
	char lineBuf[bufSize];
	int retval = 0;
	char name[20] = "c:\\setvbuf1.txt";

	fp = fopen(name, "w");
	if (fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n")); 
		return -1;
		}
	retval = setvbuf(fp, lineBuf, _IOLBF, 1024);  // set to line buffering
	if (retval)
		{ 
		INFO_PRINTF1(_L("setvbuf failed\n")); 
		fclose(fp);
		unlink(name);
		return -1;
		}
	fprintf(fp, "1st line without eol\t");
	fprintf(fp, "2nd line with eol\n");

	retval = setvbuf(fp, NULL, _IOLBF, 1024);  // set to line buffering with NULL buffer
	if (retval)
		{ 
		INFO_PRINTF1(_L("setvbuf failed\n")); 
		fclose(fp);
		unlink(name);
		return -1;
		}
	fprintf(fp, "3rd line without eol\t");
	fprintf(fp, "4th line with eol\n");

	fclose(fp);
	unlink(name);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::setvbuf2
API Tested			: setvbuf
TestCase Description: Test setvbuf in full buffering mode with and without
					  buffer (NULL buffer)
-------------------------------------------------------------------------------
*/
TInt CTestStdio::setvbuf2()
	{
	INFO_PRINTF1(_L("setvbuf2"));

	FILE *fp;
	char FullBuf[bufSize];
	int retval = 0;
	char name[20] = "c:\\setvbuf2.txt";

	fp = fopen(name, "w");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}
	retval = setvbuf(fp, FullBuf, _IOFBF, 20);  // set to full buffering
	if (retval)
		{ 
		INFO_PRINTF1(_L("setvbuf failed\n")); 
		fclose(fp);
		unlink(name);
		return -1;
		}
	fprintf(fp, "we are trying to buffer 20 characters at once. ");
	fprintf(fp, "Again.. we are trying to buffer 20 characters at once. ");
	fprintf(fp, "Once Again.. we are trying to buffer 20 characters at once\n");

	retval = setvbuf(fp, NULL, _IOFBF, 20);  // set to full buffering with NULL buffer
	if (retval)
		{ 
		INFO_PRINTF1(_L("setvbuf failed\n")); 
		fclose(fp);
		unlink(name);
		return -1;
		}
	fprintf(fp, "we are trying to buffer 20 characters at once with NULL buffer. ");
	fprintf(fp, "Again.. we are trying to buffer 20 characters at once with NULL buffer. ");
	fprintf(fp, "Once Again.. we are trying to buffer 20 characters at once with NULL buffer\n");

	fclose(fp);
	unlink(name);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::setvbuf3
API Tested			: setvbuf
TestCase Description: Test setvbuf in unbuffered mode with and without
					  buffer (NULL buffer)
-------------------------------------------------------------------------------
*/
TInt CTestStdio::setvbuf3()
	{
	INFO_PRINTF1(_L("setvbuf3"));

	FILE *fp;
	char NoBuf[bufSize];
	int retval = 0;
	char name[20] = "c:\\setvbuf3.txt";

	fp = fopen(name, "w");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}
	retval = setvbuf(fp, NoBuf, _IONBF, 10);  // set to No buffering
	if (retval)
		{ 
		INFO_PRINTF1(_L("setvbuf failed\n")); 

		fclose(fp);
		unlink(name);
		return -1;
		}
	fprintf(fp, "write to a file with no buffering");
	fprintf(fp, "Again.. write to a file with no buffering\n");

	retval = setvbuf(fp, NULL, _IONBF, 10);  // set to No buffering with NULL buffer. Same as above
	if (retval)
		{ 
		INFO_PRINTF1(_L("setvbuf failed\n")); 

		fclose(fp);
		unlink(name);
		return -1;
		}
	fprintf(fp, "write to a file with no buffering and NULL buffer. ");
	fprintf(fp, "Again.. write to a file with no buffering and NULL buffer\n");

	fclose(fp);
	unlink(name);
	return KErrNone;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::clearerr1
API Tested			: clearerr
TestCase Description: Read single character at a time, stopping on EOF or 
					  error. Then call clearerr(). It should clear the EOF and
					  error flags.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::clearerr1()
	{
	INFO_PRINTF1(_L("clearerr1"));
	char a;
    FILE *fp;

    fp = fopen("c:\\input.txt", "r");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L("fopen failed\n")); 
		
		return -1;
		}
    // read single chars at a time, stopping on EOF or error:

    while(fread(&a, sizeof(char), 1, fp), !feof(fp) && !ferror(fp)) 
		{
		INFO_PRINTF2(_L("I read %c\t"), a);
		}

    if (feof(fp))
		{
		INFO_PRINTF1(_L("End of file was reached.\n"));
		}
    if (ferror(fp))
		{
		INFO_PRINTF1(_L("An error occurred.\n"));
		}

	clearerr(fp);

	if (!feof(fp) && !ferror(fp))
		{
		INFO_PRINTF1(_L("clearerr sucessful\n"));
		}
	else
		{
		fclose(fp);
		return -1;
		}

    fclose(fp);
	return KErrNone;
	}


TInt CTestStdio::feof1()
	{
	INFO_PRINTF1(_L("feof1"));
	char a;
    FILE *fp;

    fp = fopen("c:\\input.txt", "r");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}
    // read single chars at a time, stopping on EOF or error:

    while(fread(&a, sizeof(char), 1, fp), !feof(fp) && !ferror(fp))
		{
        INFO_PRINTF2(_L("I read %c\t"), a);
		}

    if (feof(fp))
		{
        INFO_PRINTF1(_L("End of file was reached.\n"));
		}

    if (ferror(fp))
		{
        INFO_PRINTF1(_L("An error occurred.\n"));
		}

    fclose(fp);
	return KErrNone;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::ferror1
API Tested			: ferror
TestCase Description: This test case tries to write something in to a file in
					  read-only mode so the ferror() will give some error.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::ferror1()
	{
	INFO_PRINTF1(_L("ferror1"));
	FILE * fp;
	char a;
	fp=fopen("c:\\input.txt","r");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}
	else 
		{
		fwrite(&a, sizeof(char), 1, fp);
		if (ferror (fp))
			{
			INFO_PRINTF1(_L("ferror successful\n"));
			}
		else
			{
			fclose(fp);
			return -1;
			}
		clearerr(fp);
		if (!ferror(fp))
			{
			INFO_PRINTF1(_L("ferror successful after clearerr\n"));
			}
		fclose (fp);
		}
	return KErrNone;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fread1
API Tested			: fread
TestCase Description: read single char at a time, stopping on EOF or error
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fread1()
	{
	INFO_PRINTF1(_L("fread1"));
	char a;
    FILE *fp;

    fp = fopen("c:\\input.txt", "r");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}
    // read single chars at a time, stopping on EOF or error:

    while (fread(&a, sizeof(char), 1, fp), !feof(fp) && !ferror(fp)) 
		{
        INFO_PRINTF2(_L("I read %c\t"), a);
		}
	if (ferror(fp)) //Some error occured
		{
		fclose(fp);
		return -1;
		}
	fclose(fp);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fread2
API Tested			: fread
TestCase Description: Testing for the return value of the fread() API for the
					  different input values
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fread2()
	{
	INFO_PRINTF1(_L("fread2"));
	char a[20] ;//= "\0";
	
    FILE *fp;
  	
    fp = fopen("c:\\input.txt", "r");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}
    
   	if ((fread(&a, sizeof(a), 1, fp)) != 1 ) 
		{
        INFO_PRINTF1(_L("Fread()::Error"));		
		fclose(fp);
		return -1;
		}

	int retval = fseek(fp, 0, SEEK_SET); // seek to the beginning of the file
	if((fread(&a, sizeof(char), 10, fp)) != 10 ) 
		{
        fclose(fp);
		INFO_PRINTF1(_L("Fread()::Error"));
		return -1;
		}
	else
		{
		a[10] = '\0';
		TBuf<MAX_LEN> buf;
    	TInt len = strlen(a);
       	for (TInt j =0; j<len;j++)
			{
	    	buf.Append(a[j]);
			}	
		INFO_PRINTF2(_L("Read string - %S\n"), &buf);
		}
	fclose(fp);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fread3
API Tested			: fread
TestCase Description: Testing the fread() API for the different values of input
					  "nitem"
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fread3()
	{
	INFO_PRINTF1(_L("fread3"));
	char a[20] = "\0";
    FILE *fp;	

    fp = fopen("c:\\input.txt", "r");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}

    if(fread(&a, sizeof(a), 0, fp) != 0  ) 
		{
		fclose(fp);
        INFO_PRINTF1(_L("fread not returning Zero when nitem = 0"));
		
		return -1;
		}
	
	if(fread(&a, 0, 0, fp) != 0  ) 
		{
		fclose(fp);
        INFO_PRINTF1(_L("fread not returning Zero when nitem = 0 and size = 0"));
		
		return -1;
		}

	INFO_PRINTF2(_L("original NULL string - %s\n"),a);	
		
	fclose(fp);
	return KErrNone;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fwrite1
API Tested			: fwrite
TestCase Description: Write some random characters in to a file
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fwrite1()
	{
	INFO_PRINTF1(_L("fwrite1"));
	int i;
    int r[10];
    FILE *fp;

    // populate the array with random numbers:
    for(i = 0; i < 10; i++) 
		{
        r[i] = rand();
		}

    // save the random numbers (10 ints) to the file
    fp = fopen("c:\\fwrite1.txt", "wb");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}
    fwrite(r, sizeof(int), 10, fp); // write 10 ints
	if (ferror(fp))
		{
		fclose(fp);
		return -1;
		}
    fclose(fp);
	unlink("c:\\fwrite1.txt");
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fwrite2
API Tested			: fwrite
TestCase Description: Testing for the return value of the fwrite() API for the
					  different input values
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fwrite2()
	{
	INFO_PRINTF1(_L("fwrite2"));
	char a[20] = "f;dl`24|N0";
    FILE *fp;
	
	
    fp = fopen("c:\\fwrite2.txt", "wb");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}
    
   	if(fwrite(&a, sizeof(a), 1, fp) != 1  ) 
		{
        INFO_PRINTF1(_L("fwrite failed\n"));
		return -1;
		}

	if(fwrite(&a, sizeof(char), 10, fp) != 10 ) 
		{
        INFO_PRINTF1(_L("fwrite failed\n"));
		return -1;
		}
		

	fclose(fp);
	unlink("c:\\fwrite2.txt");
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fwrite3
API Tested			: fwrite
TestCase Description: Testing the fwrite() API for the different values of
					  input "nitem". Also test whether fwrite returns 0 incase
					  of nitem or size i/p is zero.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fwrite3()
	{
	INFO_PRINTF1(_L("fwrite3"));
	char a[20] = "\f;dl`24|N0";
    FILE *fp;
	
	
    fp = fopen("c:\\fwrite3.txt", "wb");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}
   
    if(fwrite(&a, sizeof(a), 0, fp) != 0  ) 
		{
        INFO_PRINTF1(_L("fwrite not returning Zero when nitem = 0"));
		
		return -1;
		}
	
	if(fwrite(&a, 0, 0, fp) != 0  ) 
		{
        INFO_PRINTF1(_L("fwrite not returning Zero when nitem = 0 and size = 0"));
		
		return -1;
		}		
	
	fclose(fp);
	unlink("c:\\fwrite3.txt");
	return KErrNone;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fgetpos1
API Tested			: fgetpos
TestCase Description: This test case reads a line from the file, saves the
					  position, reads another line from the file. Then restores
					  the position to where we saved and again reads one more
					  line from that position.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fgetpos1()
	{
	INFO_PRINTF1(_L("fgetpos1"));
	char s[20];
	fpos_t pos;
	char *buf = "This is the test.\n";
	
	FILE *fp;
	int retval;

    fp = fopen("C:\\input.txt","w");
    fputs(buf , fp);
    fputs(buf , fp);
	fclose(fp);
	fp = fopen("C:\\input.txt","r");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}
	fgets(s, sizeof(s), fp); // read a line from the file

	retval = fgetpos(fp, &pos);   // save the position
	if (retval)
		{ 
		INFO_PRINTF1(_L ("fgetpos failed\n")); 
		return -1;
		}

	fgets(s, sizeof(s), fp); // read another line from the file

	retval = fsetpos(fp, &pos);   // now restore the position to where we saved
	if (retval)
		{ 
		INFO_PRINTF1(_L ("fgetpos failed\n")); 
		return -1;
		}

	fgets(s, sizeof(s), fp); // read the second line from the file again
	fclose(fp);
	return KErrNone;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fseek1
API Tested			: fseek
TestCase Description: Test fseek() to set the file pointer to different
					  positions in a file.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fseek1()
	{
	INFO_PRINTF1(_L("fseek1"));
	FILE *fp;
	int retval;

    fp = fopen("c:\\input.txt", "a");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}

	fprintf(fp, "This is the first line\n"); 
	fprintf(fp, "This is the first line\n");

	INFO_PRINTF1(_L("Wrote to file\n")); 
	retval = fseek(fp, 30, SEEK_SET); // seek to the 20th byte of the file
	if (retval)
		{ 
		INFO_PRINTF1(_L ("fseek failed\n")); 
		return -1;
		}
	retval = fseek(fp, -10, SEEK_CUR); // seek backward 30 bytes from the current pos
	if (retval)
		{ 
		INFO_PRINTF1(_L ("fseek failed\n")); 
		return -1;
		}
	retval = fseek(fp, -10, SEEK_END); // seek to the 10th byte before the end of file
	if (retval)
		{ 
		INFO_PRINTF1(_L ("fseek failed\n")); 
		return -1;
		}

	retval = fseek(fp, 0, SEEK_SET);   // seek to the beginning of the file
	if (retval)
		{ 
		INFO_PRINTF1(_L ("fseek failed\n")); 
		return -1;
		}

	long pos = ftell(fp);
	if (pos ==0)
		{	
		INFO_PRINTF1(_L("fseek successful\n"));
		retval = KErrNone;
		}

	fclose(fp);
	return retval;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fseek2
API Tested			: fseek
TestCase Description: 	1. Create an empty file 
						2. fseek beyond the EOF
						3. Verify the size of the file
Return value		:	Fstat fills up stat file size with zero
 						Testcase returns KerrNone
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fseek2()
	{
	INFO_PRINTF1(_L("fseek2"));
	int retval;
	FILE *fp;
	struct stat st;
	fp = fopen("c:\\fseek.txt", "a+");	

	retval = fseek(fp, 100, SEEK_SET);		// seeking to the 100th byte in the empty file

	if(retval != 0)
		INFO_PRINTF1(_L("fseek failed"));
	else
		{
	    INFO_PRINTF2(_L("ftell returns %d\n"), ftell(fp));
	    fstat(fileno(fp), &st);
	    if(st.st_size != 0)
	    	{
	    	INFO_PRINTF2(_L("size of file = %d"),st.st_size);
	    	INFO_PRINTF1(_L("Size of the file should have been 0 bytes"));
	    	return KErrGeneral;
	    	}
	    }
	fclose(fp);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fsetpos1
API Tested			: fsetpos
TestCase Description: This test case reads a line from the file, saves the
					  position, reads another line from the file. Then restores
					  the position to where we saved and again reads one more
					  line from that position.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fsetpos1()
	{
	INFO_PRINTF1(_L("fsetpos1"));
	char s[20];
	char s1[20];
	char s2[20];
	fpos_t pos;
	char *buf = "This is the test";
	FILE *fp;
	int retval;

    fp = fopen("C:\\input.txt","w");
    fputs(buf , fp);
    fputs(buf, fp);
	fclose(fp);
	fp = fopen("C:\\input.txt","r");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}

	fgets(s, sizeof(s), fp); // read a line from the file

	retval = fgetpos(fp, &pos);   // save the position
	if (retval)
		{ 
		INFO_PRINTF1(_L ("fgetpos failed\n")); 
		return -1;
		}

	fgets(s1, sizeof(s1), fp); // read another line from the file

	fsetpos(fp, &pos);   // now restore the position to where we saved

	fgets(s2, sizeof(s2), fp); // read the second line from the file again

	if(strcmp(s1,s2))
		{
		INFO_PRINTF1(_L("Fsetpos():: failed .."));
		fclose(fp);
		return -1;
		}
	fclose(fp);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fsetpos2
API Tested			: fsetpos
TestCase Description: Test that, call to ungetc() will affect the functionality
					  of fsetpos() API
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fsetpos2()
	{
	INFO_PRINTF1(_L("fsetpos2"));
	char s[20];
	char s1[20];
	char s2[20];
	fpos_t pos;
	char *buf = "This is the test";
	FILE *fp;

    fp = fopen("C:\\input.txt","w");
    fputs(buf, fp);
    fputs(buf, fp);
	fclose(fp);
	fp = fopen("C:\\input.txt","r");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}

	fgets(s, sizeof(s), fp); // read a line from the file
	// As we tested fsetpos() working accordingly...

	fgetpos(fp, &pos);   // save the position

	fgets(s1, sizeof(s1), fp); // read another line from the file
	
	ungetc('f',fp); //Must undo any effects of ungetc() on the same stream

	fsetpos(fp, &pos);   // now restore the position to where we saved

	fgets(s2, sizeof(s2), fp); // read the second line from the file again

	if(strcmp(s1,s2))
		{
		INFO_PRINTF1(_L("Fsetpos():: failed .."));
		fclose(fp);
		return -1;
		}
	fclose(fp);		
	return KErrNone;
	}



/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::ftell1
API Tested			: ftell
TestCase Description: Test basic functionality of ftell()
-------------------------------------------------------------------------------
*/
TInt CTestStdio::ftell1()
	{
	INFO_PRINTF1(_L("ftell1"));
	long pos;
	FILE *fp;

    fp = fopen("c:\\ftell1.txt", "w");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}

	fwrite("Initially, Write something in to ftell1.txt", sizeof(char), 32, fp);

	// store the current position in variable "pos":
	pos = ftell(fp);
	if (pos < 0)
		{ 
		INFO_PRINTF1(_L ("ftell failed\n")); 
		return -1;
		}
	// seek ahead 10 bytes:
	fseek(fp, 10, SEEK_CUR);

	// do some mysterious writes to the file
	fwrite("Again, Write something in to ftell1.txt", sizeof(char), 28, fp);

	// and return to the starting position, stored in "pos":
	fseek(fp, pos, SEEK_SET);

	fwrite("Overwriting", sizeof(char), 11, fp);

	fclose(fp);
	unlink("c:\\ftell1.txt");
	return KErrNone;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::rewind1
API Tested			: rewind
TestCase Description: Test the basic functionality of rewind
-------------------------------------------------------------------------------
*/
TInt CTestStdio::rewind1()
	{
	INFO_PRINTF1(_L("rewind1"));
	FILE *fp;

    fp = fopen("c:\\input.txt", "r");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return -1;
		}
	fseek(fp, 100, SEEK_SET); // seek to the 100th byte of the file
	fseek(fp, -30, SEEK_CUR); // seek backward 30 bytes from the current pos
	fseek(fp, -10, SEEK_END); // seek to the 10th byte before the end of file

	rewind(fp);               // seek to the beginning of the file
	if (ferror(fp))
		{
		return -1;
		}
	long pos = ftell(fp);
	if (pos ==0)
		{
		INFO_PRINTF1(_L("rewind successful\n"));
		}

	fclose(fp);
	return KErrNone;
	}
/*--------------- END OF STREAM-RELATED OPERATIONS --------------------------*/


/*--------------------- INPUT/OUTPUT OPERATIONS -----------------------------*/

/*
-------------------------------------------------------------------------------
Function Name		: CTStdio::fgetc1
API Tested			: fgetc
TestCase Description: The test function opens a file which has characters.
					  It reads all the characters in the file one by one till
					  EOF and writes the characters in the log file.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fgetc1()
	{
	INFO_PRINTF1(_L("fgetc1"));
	
	FILE *fp;

		
	fp = fopen("C:\\input.txt","w");
	fclose(fp);
	fp = fopen("C:\\input.txt","r");
	if(fp == NULL)
		{
	    INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	int n = fgetc(fp);
	INFO_PRINTF1(_L("fgetc done"));
	while (n != -1)
		{
		n = fgetc(fp);
		}
	if (n == EOF)
		{
		INFO_PRINTF1(_L("End of file reached!"));
		}
	fclose(fp);	
	return KErrNone;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fgets1
API Tested			: fgets
TestCase Description: The test case function will open a file with characters.
					  It reads all the characters from the file as one string.
					  It writes the read string into log file. Then it does
					  another string read from the stream where the EOF is
					  already reached. This read will return NULL.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fgets1()
	{
	INFO_PRINTF1(_L("fgets1"));
	FILE *fp;
	char buf[20];
		
	fp = fopen("C:\\input.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	if(fgets(buf,18,fp) != NULL)
		{
		INFO_PRINTF2(_L("%s\n"), buf);
		}
	else
		{
		INFO_PRINTF1(_L("Buffer is empty\n"));
		}
	
	buf[0] = '\0';
	if(fgets(buf,2,fp) != NULL)
		{
		INFO_PRINTF2(_L("%s\n"), buf);
		}
	else
		{
		INFO_PRINTF1(_L("Buffer is empty\n"));
		}

	fclose(fp);	
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fgets2
API Tested			: fgets
TestCase Description: The test case will open a file which has characters. The
					  function reads 3 strings where the size of the string to
					  be read is less than, equal to and greater than the number
					  of characters in the file. The file position indicator is
					  rewinded after each time fgets function is called.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fgets2()
	{
	INFO_PRINTF1(_L("fgets2"));
	FILE *fp;
	char buf[30];
		
    fp = fopen("C:\\input.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	if(fgets(buf,5,fp) != NULL)
		{
		INFO_PRINTF2(_L("%s\n"), buf);
		}
	else
		{
		INFO_PRINTF1(_L("Buffer is empty\n"));
		}
	
	rewind(fp);
	
	if(fgets(buf,18,fp) != NULL)
		{
		INFO_PRINTF2(_L("%s\n"), buf);
		}
	else
		{
		INFO_PRINTF1(_L("Buffer is empty\n"));
		}
	
	rewind(fp);
	
	if(fgets(buf,20,fp) != NULL)
		{
		INFO_PRINTF2(_L("%s\n"), buf);
		}
	else
		{
		INFO_PRINTF1(_L("Buffer is empty\n"));
		}	
	
	fclose(fp);	
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fgets3
API Tested			: fgets
TestCase Description: This test case open a file with characters. The function
					  will read a string which has a new line charater within
					  the specified length of the string to be read.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fgets3()
	{
	INFO_PRINTF1(_L("fgets3"));
	FILE *fp;
	char buf[20];
		
	fp = fopen("C:\\input.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	if(fgets(buf,10,fp) != NULL)
		{
		INFO_PRINTF2(_L("%s\n"), buf);
		}
	else
		{
		INFO_PRINTF1(_L("Buffer is empty\n"));
		}
	
	fclose(fp);
	return KErrNone;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fputc1
API Tested			: fputc
TestCase Description: The testcase function writes one character on to a file
					  and closes the file. The same file is again opened and a
					  character is read from the file.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fputc1()
	{
	INFO_PRINTF1(_L("fputc1"));
	FILE *fp;
	int rretval,wretval;
	char name[20] = "c:\\fputc1.txt";
	
	fp = fopen(name,"w");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	wretval = fputc('a',fp); //fputc(char(0x0905),fp);
	if(wretval == EOF)
		{
		INFO_PRINTF1(_L("fputc failed\n"));
		fclose(fp);
		unlink(name);
		return -1;
		}

	fclose(fp);

	fp = fopen("C:\\fputc1.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		fclose(fp);
		unlink(name);		
		return -1;
		}

	rretval = fgetc(fp);
	if(wretval != rretval)
		{
		INFO_PRINTF1(_L("fputc failed\n"));
		}
	else
		{
		INFO_PRINTF1(_L("fputc passed\n"));
		}

	fclose(fp);
	unlink(name);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fputc2
API Tested			: fputc
TestCase Description: The testcase function writes a set characters on to a
					  file and closes the file. The same file is again opened
					  and characters areread from the file.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fputc2()
	{
	INFO_PRINTF1(_L("fputc2"));
	FILE *fp;
	int wretval[10],rretval;

	int alphabets[10] = {0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a};
	int i;
	char name[20] = "c:\\fputc2.txt";
	
	fp = fopen(name,"w");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));		
		return -1;
		}

	for(i=0;i<10;i++)
		{
		wretval[i] = fputc((char) alphabets[i],fp);
		if(wretval[i] == EOF)
			{
			INFO_PRINTF1(_L("fputc failed\n"));
			fclose(fp);
			unlink(name);
			return -1;
			}
		}

	fclose(fp);
	
	fp = fopen("C:\\fputc2.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	for(i=0;i<10;i++)
		{
		rretval = fgetc(fp);
		if(rretval != wretval[i])
			{
			INFO_PRINTF1(_L("fputc failed\n"));
			fclose(fp);
			unlink(name);
			return -1;
			}
		}
	INFO_PRINTF1(_L("fputc passed\n"));

	fclose(fp);
	unlink(name);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fputc3
API Tested			: fputc
TestCase Description: The testcase function writes a new line character on to a
					  file and closes the file. The same file is again opened
					  and the content is read from the file.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fputc3()
	{
	INFO_PRINTF1(_L("fputc3"));
	FILE *fp;
	int rretval, wretval;	
	char name[20] = "c:\\fputc3.txt";
	
	fp = fopen(name, "w");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	wretval = fputc('\n',fp);
	if(wretval == EOF)
		{
		INFO_PRINTF1(_L("fputc failed\n"));
		fclose(fp);
		unlink(name);
		return -1;
		}

	fclose(fp);

	fp = fopen("C:\\fputc3.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	rretval = fgetc(fp);
	if(wretval != rretval)
		{
		INFO_PRINTF1(_L("fputc failed\n"));
		}
	else
		{
		INFO_PRINTF1(_L("fputc passed\n"));
		}

	fclose(fp);
	unlink(name);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fputc4
API Tested			: fputc
TestCase Description: The testcase function tries writing a character on to a
					  file which is opened for reading only.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fputc4()
	{
	INFO_PRINTF1(_L("fputc4"));
	FILE *fp;
	int wretval = 0;
	char name[20] = "c:\\input.txt";

	fp = fopen(name,"w");
	fclose(fp);
	fp = fopen(name,"r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));	
		return -1;
		}

	wretval = fputc('c',fp);
	if(wretval == EOF)
		{
		fclose(fp);
		return KErrNone; //Negative test case, So success
		}		

	fclose(fp);
	return KErrNone;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fputs1
API Tested			: fputs
TestCase Description: The testcase funtion will write a fixed lenth string to a
					  file, then the file is closed. The same file again opened
					  and the string that was written using fputws is read back
					  using fgets and the 2 strings are compared to determine
					  the result of the test case.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fputs1()
	{
	INFO_PRINTF1(_L("fputs1"));
	FILE *fp;
	int wretval;	
	char rs1[10],rs2[10];
	char *buf = "This is a test";
	char *rptr;
	int retval;

	fp = fopen("C:\\input.txt","w");
	fputs(buf,fp);
	fclose(fp);
	fp = fopen("C:\\input.txt","r");
	
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	rptr = fgets(rs1,10,fp);
	if(rptr == NULL)
		{
		INFO_PRINTF1(_L("fgets failed\n"));
		fclose(fp);
		return -1;
		}

	fclose(fp);

	fp = fopen("c:\\fputs1.txt","w");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	wretval = fputs(rs1,fp);
	if(wretval == EOF)
		{
		INFO_PRINTF1(_L("fputs failed\n"));
		fclose(fp);
		return -1;
		}

	fclose(fp);

	fp = fopen("C:\\fputs1.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	rptr = fgets(rs2,10,fp);
	if(rptr == NULL)
		{
		INFO_PRINTF1(_L("fgets failed\n"));
		fclose(fp);
		return -1;
		}

	retval = strcmp(rs1,rs2);

	if(retval == 0)
		{
		INFO_PRINTF1(_L("fputs passed\n"));
		}
	else
		{
		INFO_PRINTF1(_L("fputs failed\n"));
		}

	fclose(fp);
	unlink("C:\\fputs1.txt");
	
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fputs2
API Tested			: fputs
TestCase Description: The testcase function will write a string which has a new
					  line character in the middle and the file is closed. The
					  same file is opened and the string is read back by fgets API.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fputs2()
	{
	INFO_PRINTF1(_L("fputs2"));
	FILE *fp;
	int wretval;
	char rs1[10] = "abcd\nefg";
	char *rptr,rs2[10];
	int retval;
	
	fp = fopen("c:\\fputs2.txt","w");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	wretval = fputs(rs1,fp);
	if(wretval == EOF)
		{
		INFO_PRINTF1(_L("fputs failed\n"));
		fclose(fp);
		return -1;
		}

	fclose(fp);

	fp = fopen("C:\\fputs2.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	rptr = fgets(rs2,10,fp);
	if(rptr == NULL)
		{
		INFO_PRINTF1(_L("fgets failed\n"));
		fclose(fp);
		return -1;
		}

	retval = strcmp(rs1,rs2);

	if(retval == 0)
		{
		INFO_PRINTF1(_L("fputs failed\n"));
		}
	else
		{
		INFO_PRINTF1(_L("fputs passed\n"));
		}

	fclose(fp);
	unlink("C:\\fputs2.txt");
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fputs3
API Tested			: fputs
TestCase Description: The testcase will open a file in read-only mode and try
					  to write a string.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fputs3()
	{
	INFO_PRINTF1(_L("fputs3"));
	FILE *fp;
	int wretval;
	char rs1[10] = "abcd\nefg";

	fp = fopen("C:\\input.txt","w");
	fclose(fp);
	fp = fopen("C:\\input.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	wretval = fputs(rs1,fp);
	if(wretval == EOF)
		{
		INFO_PRINTF1(_L("fputs passed\n"));
		fclose(fp);
		return KErrNone;
		}

	fclose(fp);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fputs4
API Tested			: fputs
TestCase Description: The testcase function will write a string to a file which
					  has a null terminator in the middle of the string, the
					  file is closed after writing. Then the same file is
					  opened and the content is read out and compared.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::fputs4()
	{
	INFO_PRINTF1(_L("fputs4"));
	FILE *fp;
	int wretval;
	char rs1[10] = "abcd\0efg";
	char *rptr, rs2[10];
	int retval;

	fp = fopen("c:\\fputs4.txt","w");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	wretval = fputs(rs1,fp);
	if(wretval == EOF)
		{
		INFO_PRINTF1(_L("fputs failed\n"));
		fclose(fp);
		return -1;
		}

	fclose(fp);

	fp = fopen("C:\\fputs4.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	rptr = fgets(rs2,10,fp);
	if(rptr == NULL)
		{
		INFO_PRINTF1(_L("fputs failed\n"));
		}

	retval = strcmp(rs1,rs2);

	if(retval == 0)
		{
		INFO_PRINTF1(_L("fputs passed\n"));
		}
	else
		{
		INFO_PRINTF1(_L("fputs failed\n"));
		}

	fclose(fp);
	unlink("C:\\fputs4.txt");
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::getc1
API Tested			: getc
TestCase Description: The test function opens a file which has characters. It
					  reads all the characters in the file one by one till EOF
					  and writes the characters into log file.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::getc1()
	{
	INFO_PRINTF1(_L("getc1"));
	FILE *fp;
	int retval;

	fp = fopen("C:\\input.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	retval = getc(fp);
	while((int)(retval = getc(fp) )!= EOF)
		{
		INFO_PRINTF3(_L("%c\t%x\n"), retval, retval);
		}

	fclose(fp);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::putc1
API Tested			: putc
TestCase Description: The testcase function writes one character on to a file
					  and closes the file. The same file is again opened and
					  a character is read from the file.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::putc1()
	{
	INFO_PRINTF1(_L("putc1"));
	FILE *fp;
	int rretval, wretval;
		
	fp = fopen("c:\\putc1.txt","w");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	wretval = putc('a',fp); //putc(char(0x0905), fp);
	if(wretval == EOF)
		{
		INFO_PRINTF1(_L("putc failed\n"));
		fclose(fp);
		return -1;
		}

	fclose(fp);

	fp = fopen("C:\\putc1.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	rretval = fgetc(fp);
	if(wretval != rretval)
		{
		INFO_PRINTF1(_L("putc failed\n"));
		}
	else
		{
		INFO_PRINTF1(_L("putc passed\n"));
		}

	fclose(fp);
	unlink("c:\\putc1.txt");
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::putc2
API Tested			: putc
TestCase Description: The testcase function writes a set of characters on to a
					  file and closes the file. The same file is again opened
					  and characters are read from the file.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::putc2()
	{
	INFO_PRINTF1(_L("putc2"));
	FILE *fp;
	int wretval[10], rretval;
	int alphabets[10] = {0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a};
	int i;
	
	fp = fopen("c:\\putc2.txt","w");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}
	for(i=0;i<10;i++)
		{
		wretval[i] = putc((char) alphabets[i],fp);
		if(wretval[i] == EOF)
			{
			INFO_PRINTF1(_L("putc failed\n"));
			fclose(fp);
			return -1;
			}
		}

	fclose(fp);

	fp = fopen("C:\\putc2.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	for(i=0;i<10;i++)
		{
		rretval = fgetc(fp);
		if(rretval != wretval[i])
			{
			INFO_PRINTF1(_L("putc failed\n"));
			fclose(fp);
			return -1;
			}
		}
	INFO_PRINTF1(_L("putc passed\n"));

	fclose(fp);
	unlink("c:\\putc2.txt");
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::putc3
API Tested			: putc
TestCase Description: The testcase function writes a new line character on to a
					  file and closes the file. The same file is again opened
					  and the content is read from the file.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::putc3()
	{
	INFO_PRINTF1(_L("putc3"));
	FILE *fp;
	int rretval,wretval;
	
	fp = fopen("c:\\putc3.txt","w");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	wretval = putc('\n',fp);
	if(wretval == EOF)
		{
		INFO_PRINTF1(_L("putc failed\n"));
		fclose(fp);
		return -1;
		}

	fclose(fp);

	fp = fopen("C:\\putc3.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	rretval = fgetc(fp);
	if(wretval != rretval)
		{
		INFO_PRINTF1(_L("putc failed\n"));
		}
	else
		{
		INFO_PRINTF1(_L("putc passed\n"));
		}

	fclose(fp);
	unlink("c:\\putc3.txt");
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::putc4
API Tested			: putc
TestCase Description: The testcase function tries writing a character on to a
					  file which is opened for reading only.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::putc4()
	{
	INFO_PRINTF1(_L("putc4"));
	FILE *fp;
	int wretval;

	fp = fopen("C:\\input.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	wretval = putc('\n', fp);
	if(wretval == EOF)
		{
		INFO_PRINTF1(_L("putc passed\n"));
		fclose(fp);
		return KErrNone;
		}

	INFO_PRINTF1(_L("putc failed\n"));
	fclose(fp);
	unlink("input.txt");
	return KErrNone;
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::ungetc1
API Tested			: ungetc
TestCase Description: The testcase function opens a file and writes few
					  characters into the file. Then the file is closed. Once
					  again the same file is opened and ungetc and getc API's
					  are called. The test case is verified by comparing the
					  characters written by ungetc and read by getc.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::ungetc1()
	{
	INFO_PRINTF1(_L("ungetc1"));
	FILE *fp;
	int err = -1;
	int c;

	fp = fopen("c:\\ungetc1.txt","w");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}	
	fputs ("bla", fp);
	INFO_PRINTF1(_L("fputs done\n"));
	fclose (fp);

	fp = fopen("c:\\ungetc1.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	if(ungetc ('z', fp) != 'z')
		{
		err = 1;
		}

    INFO_PRINTF2(_L("%d"), err);

	INFO_PRINTF1(_L("ungetc done"));

	if(getc (fp) != 'z')
		{
		err = 1;
		}

	INFO_PRINTF2(_L("%d"),err);

	if(getc (fp) != 'b')
		err = 1;

    INFO_PRINTF2(_L("%d"),err);

	if(getc (fp) != 'l')
		{
		err = 1;
		}

	INFO_PRINTF2(_L("%d"),err);

	if(ungetc ('m', fp) != 'm')
		{
		err = 1;
		}

    INFO_PRINTF2(_L("%d"),err);
	
	INFO_PRINTF1(_L("ungetc done"));

	if(getc (fp) != 'm')
		{
		err = 1;
		}

    INFO_PRINTF2(_L("%d"),err);
	
	if((c = getc (fp)) != 'a')
		{
		err = 1;
		}

    INFO_PRINTF2(_L("%d"),err);
	
	if(getc (fp) != EOF)
		{
		err = 1;
		}
    INFO_PRINTF2(_L("%d"),err);

	if(ungetc (c, fp) != c)
		{
		err = 1;
		}
	INFO_PRINTF2(_L("%d"),err);

	INFO_PRINTF1(_L("ungetc done"));

	if(feof (fp) != 0)
		{
		err = 1;
		}
    INFO_PRINTF2(_L("%d"),err);
	
	if(getc (fp) != c)
		{
		err = 1;
		}
    INFO_PRINTF2(_L("%d"),err);	

	if(getc (fp) != EOF)
		{
		err = 1;
		}
    INFO_PRINTF2(_L("%d"),err);
		
	fclose (fp);
	unlink("c:\\ungetc1.txt");    

	if(err == 1)
		{
		INFO_PRINTF1(_L("ungetc failed\n"));
		}
	else
		{
		INFO_PRINTF1(_L("ungetc passed\n"));
		}
		
	return KErrNone;
	}
	
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fileno1
API Tested			: fileno
TestCase Description: The function fileno examines the argument stream and returns its  
					  integer descriptor.

-------------------------------------------------------------------------------
*/
TInt CTestStdio::fileno1()	
	{
	INFO_PRINTF1(_L("fileno1"));
	FILE *fp;
	char *buf="C:\\fileno.txt";
	int i;
	fp = fopen(buf,"w");
	i = fileno(fp);
	if( i == -1)
		{
		INFO_PRINTF1(_L("fileno1 failed\n"));
		}
	else
		{
		INFO_PRINTF1(_L("fileno1 passed\n"));
		}
	fclose(fp);
	unlink(buf);
	return KErrNone;	
	}
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fileno2
API Tested			: fileno
TestCase Description: Negative testcase. This testcase tries to call fileno on a
 					 file stream which has been closed using a call to fclose()

-------------------------------------------------------------------------------
*/
TInt CTestStdio::fileno2()	
	{
	INFO_PRINTF1(_L("fileno2"));
	FILE *fp;
	char *buf="C:\\fileno.txt";
	int i;
	fp = fopen(buf,"w");
	fclose(fp);
	i = fileno(fp);
	if( i == -1 && errno == EBADF)
		{
		INFO_PRINTF1(_L("fileno2:closed stream Passed\n"));
		}
	else
		{
		INFO_PRINTF1(_L("fileno2:closed stream failed\n"));
		}
	fp = NULL;
	i = fileno(fp);
	if( i == -1 && errno == EBADF)
		{
		INFO_PRINTF1(_L("fileno2: Null stream Passed\n"));
		}
	else
		{
		INFO_PRINTF1(_L("fileno2: Null stream failed\n"));
		}
	unlink(buf);
	return KErrNone;	
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::fileno3
API Tested			: fileno
Testcase for DEF118788
-------------------------------------------------------------------------------
*/	
TInt CTestStdio::fileno3()	
	{
	FILE *stream;
	char *file;
	TInt ret = KErrNone;
	TInt arg1 = 0;
	FILE *var1 = 0;

	file = "c:\\testfile";
	if((stream = fopen(file, "w")) == (FILE *) NULL)
		{
		INFO_PRINTF2(_L("Cannot open %s to write\n"), file);
		return KErrGeneral;
		}

	(void) fileno((var1 = (arg1++, stream))) ;                                                                   

	if ( arg1 != 1 )
		{
		INFO_PRINTF2(_L("Arg 1 of fileno evaluated %d times\n"),arg1);
		fclose(stream);
		unlink(file);
		return KErrGeneral;
		}

	if ( arg1 > 0 && var1 != stream)
		{
		INFO_PRINTF1(_L("Arg 1 of fileno not protected by parentheses\n"));
		fclose(stream);
		unlink(file);
		return KErrGeneral;
		}
	fclose(stream);
	(void) unlink(file);	
	return ret;
	}
 
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::getw1
API Tested			: getw
TestCase Description: The test function opens a file write word and read it back
					  and compare it with written value.
-------------------------------------------------------------------------------
*/
	TInt CTestStdio::getw1()
	{
	INFO_PRINTF1(_L("getw"));
	
	INFO_PRINTF1(_L("putw"));
	FILE *fp;
	int rretval;
	int word = 94;
	char name[20] = "c:\\fgetw1.txt";
	
	fp = fopen(name,"wb");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));		
		return -1;
		}

	putw(word,fp);
	
	if (ferror(fp))
		{
        INFO_PRINTF1(_L("Error writing to file\n"));
		}
    else
		{
        INFO_PRINTF1(_L("Successful write\n"));
		}
    fclose(fp);

    /* reopen the file */
    fp = fopen(name, "rb");
    if (fp == NULL)
		{
       INFO_PRINTF2(_L("Error opening file %s\n"), name);
       return -1;
		}

    /* extract the word */
    rretval = getw(fp);
    if (ferror(fp))
		{
        INFO_PRINTF1(_L("Error reading file\n"));
		}
    else
		{
        INFO_PRINTF2(_L("Successful read: word = %d\n"), word);
		}
    
    /* Clean up */
    fclose(fp);
    unlink(name);
    
    if(rretval != word)
		{
		return -1;
		}
	else
		{
		return KErrNone;
		}	
	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::putw1
API Tested			: fgetc
TestCase Description: The test function opens a file write word and read it back
					  and compare it with written value.
-------------------------------------------------------------------------------
*/
TInt CTestStdio::putw1()
	{
	INFO_PRINTF1(_L("putw"));
	FILE *fp;
	int rretval;
	int word = 94;
	char name[20] = "c:\\fputw1.txt";
	
	fp = fopen(name,"wb");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));		
		return -1;
		}

	putw(word,fp);
	
	if (ferror(fp))
		{
        INFO_PRINTF1(_L("Error writing to file\n"));
		}
    else
		{
        INFO_PRINTF1(_L("Successful write\n"));
		}
    fclose(fp);

    /* reopen the file */
    fp = fopen(name, "rb");
    if (fp == NULL)
		{
       INFO_PRINTF2(_L("Error opening file %s\n"), name);
      return -1;
		}

    /* extract the word */
    rretval = getw(fp);
    if (ferror(fp))
		{
        INFO_PRINTF1(_L("Error reading file\n"));
		}
    else
		{
        INFO_PRINTF2(_L("Successful read: word = %d\n"), word);
		}
    
    /* Clean up */
    fclose(fp);
    unlink(name);
    if(rretval != word)
		{
		return -1;
		}
	else
		{
		return KErrNone;
		}
	}
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::getdirentries1
API Tested			: getdirentries
TestCase Description: The function fileno examines the argument stream and returns its  
					  integer descriptor.

-------------------------------------------------------------------------------
*/


TInt CTestStdio::getdirentries1()	
	{

    INFO_PRINTF1(_L("getdirentries1"));
 	char buf[4096];	 	
    
    long dbase = (off_t) 0;        
    struct dirent *d1;
    struct dirent *d2;         
    char * name1 = "dummy.txt";   
    char * name2 = "junk.txt";   
    int fd;
    int len_getdir;
    
    TInt len=0;
	TPtrC string;
	GetStringFromConfig(ConfigSection(),_L("parameter1"), string);	
    TBuf8<100> buf1;
    buf1.Copy(string);
    char* path1 = (char*) buf1.Ptr();
	len=buf1.Length();
    path1[len]='\0';
	
               
    TFileName dirName(string);
	RFs iTestSession;
	iTestSession.Connect();
	if(iTestSession.MkDir(dirName) != KErrNone)
		{
		INFO_PRINTF1(_L("create dir failed, directory may be already present\n"));			
		return -1;
		}
	INFO_PRINTF1(_L("create dir successfully\n"));
	iTestSession.Close();	
			
	strcat(path1,"dummy.txt"); 

	TInt fd1,fd2;
	fd1=open(path1,O_WRONLY|O_CREAT|O_TRUNC);				
	if( fd1 == - 1)
		{
		INFO_PRINTF1(_L("open file 1 failed\n"));
		return -1;
		}
	else 
		{
		INFO_PRINTF1(_L("opened file 1\n"));
		path1[len]='\0';		
		strcat(path1,"junk.txt");
		fd2=open(path1,O_WRONLY|O_CREAT|O_TRUNC);
	
		if( fd2 == -1 )		
			{
			INFO_PRINTF1(_L("open file 2 failed\n"));
			close(fd1);
			return -1;
			}
		close(fd1);
		close(fd2);	
		path1[len-1]='\0';
		INFO_PRINTF1(_L("both files opened successfully\n"));	
		}		
			

    fd = open(path1,O_RDONLY);
    if(fd == -1)
		{
      		INFO_PRINTF1(_L("open dir failed\n"));
            return -1;
		}
    
    INFO_PRINTF1(_L("open dir successfully\n"));

    len_getdir = getdirentries (fd, buf,(unsigned int)sizeof (buf),&dbase);
    if(len_getdir == -1)
		{
    	 INFO_PRINTF1(_L("getdirentries call failed\n"));
		 close(fd);
    	return -1;
		}        	
    
    d1=(struct dirent *)buf;                
    d2 = (struct dirent *)(buf + d1->d_reclen);
            
    if(strcmp(d1->d_name,name1) || strcmp(d2->d_name,name2))
		{
    	close(fd);
        path1[len -1] = '\\';
        path1[len] = '\0'; 
        strcat(path1,"junk.txt"); 
        remove(path1);
        path1[len] = '\0';         
        strcat(path1,"dummy.txt"); 
        remove(path1);  
        path1[len] = '\0';         
        remove(path1);                  
        INFO_PRINTF1(_L("Remove dir successful\n"));        
   		INFO_PRINTF1(_L("strcmp failed on files\n"));
    	close(fd);
    	return -1;
		}     
              
    close(fd);
    path1[len -1] = '\\';
    path1[len] = '\0'; 
    strcat(path1,"junk.txt"); 
    remove(path1);
    path1[len] = '\0';         
    strcat(path1,"dummy.txt"); 
    remove(path1);  
    path1[len] = '\0';         
    remove(path1);                  
    INFO_PRINTF1(_L("Remove dir successful\n")); 
	INFO_PRINTF1(_L("getdirentries call successful\n"));                
    return KErrNone;
	}
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestStdio::getdirentries2
API Tested			: getdirentries
TestCase Description: The function examines whether the file name(d_name) returned is null terminated or not

-------------------------------------------------------------------------------
*/

TInt CTestStdio::getdirentries2()
	{
	int retval;
    long basep=(off_t) 0;
    char buf[1024];
    struct dirent * d;
    char * dname="C:\\emp";
    char * fname1="C:\\emp\\abc1.txt";
    char * fname="C:\\emp\\def.txt";
    int fd,fd1, fd2;
    retval = mkdir(dname,0666);
    if(retval)
    	{
    	INFO_PRINTF2(_L("failed to create directory and errno returned is %d"),errno);
    	return KErrGeneral;
    	}
    fd2=open(fname1,O_WRONLY|O_CREAT);
    fd1=open(fname,O_WRONLY|O_CREAT);
    if(fd1==-1)
 	   {
       INFO_PRINTF2(_L("file creation failed and errno is %d\n"),errno);
	   close(fd2);
       return KErrGeneral;         
       }
    fd=open(dname,O_RDONLY);
    if(fd==-1)
		{
        INFO_PRINTF2(_L("directory opening failed and errno returned is %d\n"),errno);
		close(fd2), close(fd1);
        return KErrGeneral;    
        }
        retval = getdirentries (fd, buf,(unsigned int)sizeof (buf),&basep);
    if(retval == -1)
        {
        INFO_PRINTF2(_L("getdirentries call failed and errno returned is %d\n"),errno);
		close(fd2), close(fd1), close(fd);
        return KErrGeneral; 
        }               
    INFO_PRINTF1(_L("getdirentries is success\n"));   
    d=(struct dirent *)buf;
    TBuf<MAX_LEN> buffer;
    TInt len = strlen(d->d_name);
    for (TInt j =0; j<len;j++)
		{
	    buffer.Append(d->d_name[j]);
		}	
    INFO_PRINTF2(_L("first file name - %S\n"),&buffer);    
    retval = strcmp(d->d_name, "abc1.txt");
    INFO_PRINTF2(_L("the length of the first record is %d\n"),d->d_reclen);
    if(d->d_reclen!=20)
		{
		INFO_PRINTF1(_L("The length of the record is misleading\n"));	
		close(fd2), close(fd1), close(fd);
		return KErrGeneral;
		}
    if(retval)
        {
        INFO_PRINTF1(_L("file name is not matching might be not null terminated\n"));	
		close(fd2), close(fd1), close(fd);
        return KErrGeneral; 
        }
      
    struct dirent* d2 = (struct dirent *)(buf + d->d_reclen);
	INFO_PRINTF2(_L("the length of the second record is %d\n"),d2->d_reclen);
	if(d2->d_reclen!=16)
		{
		INFO_PRINTF1(_L("The length of the record is misleading\n"));	
		close(fd2), close(fd1), close(fd);
		return KErrGeneral;
		}
	buffer.Zero();
    len = strlen(d2->d_name);
    for (TInt j =0; j<len;j++)
		{
	    buffer.Append(d2->d_name[j]);
		}
	INFO_PRINTF2(_L("second file name - %S\n"),&buffer);
    retval = strcmp(d2->d_name, "def.txt");
    if(retval)
        {
        INFO_PRINTF1(_L("file name is not matching might be not null terminated\n"));	
		close(fd2), close(fd1), close(fd);
        return KErrGeneral; 
        }
    INFO_PRINTF1(_L("file name is null terminated\n"));	   
    close(fd2); 
    close(fd1);
    close(fd);
    retval = remove(fname1);
    retval =remove(fname);
    retval = rmdir(dname);
    if(retval)
        {
        INFO_PRINTF2(_L("File or directory removal failed %d\n"), retval);	
        return KErrGeneral; 
        }
    return KErrNone;
    } 

//-------------------------------------------------------------------------------
//Function Name		: CTestStdio::freopen1
//API Tested		: freopen
//Description		: This api checks if stdout contents are redirected to the file
//-------------------------------------------------------------------------------

TInt CTestStdio::freopen1()
	{
	char filename[20] = "c:\\myfile.txt";
	char string[] = "This redirects to a file\n";
	char string1[30];
	FILE *fp = freopen (filename,"w",stdout);
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("freopen failed\n"));
		return -1;
		}
  	int retval = printf ("%s", string);// sentence is redirected to a file.\n"); 
  	fclose (stdout);  	
	FILE *fp1 = fopen(filename,"r");
	if(fp1 == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}
	else
		{
		fgets(string1, retval+1, fp1);//fgets reads bytes from stream upto n-1 bytes or \n 
		}
    fclose (fp1);
    if(strcmp(string1, string)==0)
    {
    	INFO_PRINTF1(_L("freopen successful\n"));
    	return KErrNone;
    }
    else
    {
    	INFO_PRINTF1(_L("freopen failed\n"));
    	return KErrGeneral;
    }
 	}


//-------------------------------------------------------------------------------
//Function Name		: Testbinopen
//API Tested		: open()
//Description		: Given a path for a file, returns a file descriptor. Testing
//					  against defect number Team Track id : 111519
//--------------------------------------------------------------------------------

TInt CTestStdio::binaryopen()
	{
	int fd = -1;
	int ret = 0;
	fd = open("c:\\binaryfile.bd", O_RDWR|O_CREAT|O_BINARY);
	if(fd == -1)
		{
		INFO_PRINTF1(_L("open() failed\n"));
		return KErrGeneral;
		}
	ret = close(fd);
	if(ret == -1)
		{
		INFO_PRINTF1(_L("close() failed\n"));
		remove("c:\\binaryfile.bd");
		return KErrGeneral;
		}
	//cleanup
	remove("c:\\binaryfile.bd");
	return KErrNone;
	}

//---------------------------------------------------------------------------------
//Function Name		: Testbinfopen
//API Tested		: fopen(), fclose()
//Description		: Given a path for a file, returns a file stream. Testing 
//					  against defect number Team Track Id:111519
//---------------------------------------------------------------------------------

TInt CTestStdio::binaryfopen()
	{
	FILE *fs = NULL;
	int ret = 0;
	fs = fopen("c:\\binaryfile1.bd","wb+");
	if(NULL == fs)
		{
		INFO_PRINTF1(_L("fopen() failed\n"));
		return KErrGeneral;
		}
	ret = fclose(fs);
	if(0 != ret)
		{
		INFO_PRINTF1(_L("fopen() failed\n"));
		remove("c:\\binaryfile1.bd");
		return KErrGeneral;
		}
		//Clean up
	remove("c:\\binaryfile1.bd");
	return KErrNone;
	}

//---------------------------------------------------------------------------------
//Function Name		: Testsprintf1
//API Tested		: sprintf1
//Description		: Its a manual test.Follow the steps given below to change the epoc environment 
//					  1) Goto control panel -> International 
//					  2) Select Tab " No " and change the decimal seperator to ','
//					  and run the test by uncommenting the TestStep in script file, 
//					  against defect number Team Track Id:DEF111624
//---------------------------------------------------------------------------------
 
TInt CTestStdio::Testsprintf1()
	{
	
	float value = 0.100000f;
	float value1 = 1234567.123000;
	float value2 = 123.123000;
	float value3 = 4.0;
	
	char result[32];
	char result1[32];
	char result2[32];
	char result3[32];
	
	sprintf (result, "%f",value);
	sprintf (result1, "%10.1f",value1);
	sprintf (result2, "%f",value2);
	sprintf (result3, "%10.1f",value3);
	
	if(!(strcmp(result,"0.10000")))
		{
		INFO_PRINTF1(_L("sprintf failed\n"));
		return KErrGeneral;
		}
		
	if(!(strcmp(result,"1234567.1")))
		{
		INFO_PRINTF1(_L("sprintf failed\n"));
		return KErrGeneral;
		}
	
	if(!(strcmp(result,"123.123000")))
		{
		INFO_PRINTF1(_L("sprintf failed\n"));
		return KErrGeneral;
		}
		
	if(!(strcmp(result,"        4.0")))
		{
		INFO_PRINTF1(_L("sprintf failed\n"));
		return KErrGeneral;
		}
	
	return KErrNone;
	}
	
int getFirstMissingDrive(char& aChar)	
	{
	RFs fs;
	fs.Connect();

	//obtain list of available drives
	char ch = 'A';	
	TDriveList driveList8;
	fs.DriveList(driveList8);
	fs.Close();
	
	for (TInt i = 0; i < KMaxDrives; ++i)
		{
		if (driveList8[i]==0)
			{
			aChar=ch;
			return 0;
			}
		++ch;	
		}	
	return -1;		
	}

TInt CTestStdio::InvalidDriveTests29()
	{
	char file[12+1]; //rqd max is 12
	char drive;
	int ret;
	SetTestStepResult(EPass);    	    	
	
	if(getFirstMissingDrive(drive)==0)
		{
		struct stat sbuf;
		struct utimbuf times;
		int fd;
		FILE* fptr;
		
		sprintf(file, "%c:\\file.txt", drive);	
		fptr=fopen(file, "a");
		if(fptr!=NULL || errno!=ENOENT)
			{
			INFO_PRINTF2(_L("fopen() error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}
		
		memset(file, 0, 13);	
		sprintf(file, "%c:\\file\\", drive);	
		ret=mkdir(file, 0x777);
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("mkdir() error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}
			
		memset(file, 0, 13);	
		sprintf(file, "%c:\\file\\", drive);	
		ret=rmdir(file);
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("rmdir() error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}
			
		memset(file, 0, 13);	
		sprintf(file, "%c:\\file.txt", drive);	
		ret=stat(file, &sbuf);
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("stat() error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}
			
		memset(file, 0, 13);	
		sprintf(file, "%c:\\file\\", drive);	
		ret=chdir(file);
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("chdir() error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}
			
		memset(file, 0, 13);	
		sprintf(file, "%c:\\file.txt", drive);	
		ret=chmod(file, S_IRUSR|S_IRGRP|S_IROTH);
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("chmod() error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}
			
		memset(file, 0, 13);	
		sprintf(file, "%c:\\file.txt", drive);	
		ret=utime(file, &times);
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("utime() error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}
			
		fd=open("c:\\invdr.txt", O_CREAT|O_WRONLY);	
		close(fd);
		memset(file, 0, 13);	
		sprintf(file, "%c:\\file.txt", drive);	
		ret=rename(file, "c:\\invdr.txt");
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("rename() -1 error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}			
		ret=rename("c:\\invdr.txt", file);
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("rename() -2 error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}			
		ret=rename(file, file);
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("rename() -3 error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}			

		memset(file, 0, 13);	
		sprintf(file, "%c:\\file", drive);	
		ret=mkfifo(file, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("mkfifo() error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}
						
		memset(file, 0, 13);	
		sprintf(file, "%c:\\file.txt", drive);	
		ret=link("c:\\invdr.txt", file);
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("link() error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}		
			
		memset(file, 0, 13);	
		sprintf(file, "%c:\\file.txt", drive);	
		ret=unlink(file);
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("unlink() error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}
		
		memset(file, 0, 13);	
		sprintf(file, "%c:\\file.txt", drive);	
		ret=creat(file, O_CREAT|O_WRONLY);
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("creat() error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}
			
		memset(file, 0, 13);	
		sprintf(file, "%c:\\file.txt", drive);	
		fptr=freopen(file, "a+", stdout);
		if(fptr!=NULL || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("freopen() error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}
			
		memset(file, 0, 13);	
		sprintf(file, "%c:\\file.txt", drive);	
		ret=access(file, F_OK);
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("access() error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}
			
		memset(file, 0, 13);	
		sprintf(file, "%c:\\file.txt", drive);	
		ret=symlink("c:\\invdr.txt", file);
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("symlink() error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}

		memset(file, 0, 13);	
		sprintf(file, "%c:\\file.txt", drive);	
		ret=truncate(file, 128);
		if(ret!=-1 || errno!=ENOENT)	
			{
			INFO_PRINTF2(_L("truncate() error- Error no. %d..."), errno);
			SetTestStepResult(EFail);    	    
			}

		unlink("c:\\invdr.txt");								
		}
		
		return TestStepResult();						
	}

//-------------------------------------------------------------------------------
//Function Name		: Testgetchar
//API Tested		: seteof
//Description		: This api checks if End Of File has been reached
//-------------------------------------------------------------------------------

/*TInt CTestStdio::Testgetchar()
	{
	FILE *fp;
	int retval;

	fp = fopen("C:\\input.txt","r");
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		return -1;
		}

	retval = getchar();
	while((int)(retval = getc(fp) )!= EOF)
		{
		INFO_PRINTF3(_L("%c\t%x\n"), retval, retval);
		}

	fclose(fp);
  return 0;

	}*/
	
//-------------------------------------------------------------------------------
//Function Name	: Testferror
//API Tested	: ferror
//Description: Tests if a error has occurred in the last file operation like 
// 				reading and writing with the given stream
//-------------------------------------------------------------------------------


/*--------------------- END OF INPUT/OUTPUT OPERATIONS ----------------------*/


 
/**
*Testcase Name:wopenTest
*API Name	  :wopen()
*Description  :It possibly create a file or device 
* successful, wopen returns a non-negative integer, termed a file descriptor.
* It returns -1 on failure
*/

 
 TInt CTestStdio::wopenTest()
	 {
	 int fd = wopen(L"c:\\wopen.txt" ,O_CREAT, 0666) ;
     if(fd == -1 ) 
	     {
	     INFO_PRINTF1(_L("Failed to create and open file in current working directory \n") );
	     return KErrGeneral;
	     }
   	INFO_PRINTF1(_L("File created and opened in current working directory \n"  ) );
	close(fd);
	unlink("c:\\wopen.txt");
   	return KErrNone;
	}


 /**
 *TestCase Name	:posix_spawnattr_destroyTest()
 *API			:posix_spawnattr_destroy
 *Description	:It destroy's a spawn attributes object.
 *which  can be reinitialized using posix_spawnattr_init(); 
 *Upon successful completion it  returns zero
 */
 
 
 TInt CTestStdio::posix_spawnattr_destroyTest()
 	{
   	posix_spawnattr_t* attrp;
   	TInt err=posix_spawnattr_init(attrp=0);
	if(err!=0)
		{
		INFO_PRINTF1(_L("posix_spawnattr_init failed..."));
		}
		
	err =posix_spawnattr_destroy(attrp);
 	if(err!= 0)
	 	{
	 	return KErrGeneral;
	 	}
	return KErrNone;
  	}
  
/**
*Testcase Name	:setgroupsTest
*API			:setgroups()
*Description	:Sets the supplementary group IDs for the process.
*On success, zero is returned. On error, -1 is returned, and errno is set appropriately. 
*/


TInt CTestStdio::setgroupsTest()
	{
	TInt ret=setgroups(1,0);
	if(ret<0)
		{
		return KErrGeneral;
		}
	return KErrNone;
	}
	
	
/**
*Testcase Name	:sigactionTest
*API			:sigaction()
*Description	: The sigaction system call is used to change the action for
* a  signal.It returns return 0 on success and -1 on error.
*/


TInt CTestStdio::sigactionTest()
	{
	TInt ret = sigaction(SIGINT,NULL,NULL);
	if(ret == -1)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
	
	}

	
	
/**
*Testcase Name:getpwentTest
*API		  :getpwent()
*Description  :It return a valid pointer to a passwd structure on success or NULL 
if the entry is not found or if an error occurs
*/
	
	
TInt CTestStdio::getpwentTest()
	{
	struct passwd* pw_ent=NULL;
	setpwent();
	pw_ent = getpwent();
	if(pw_ent == NULL)      
	    {
	    return KErrGeneral;	
	    }
	free(pw_ent->pw_dir);	    
	free(pw_ent);    
	endpwent();     
	return KErrNone;
    }
    
 /*
  *Test case Name	:brkTest()
  *API			    :brk()
  *Description		:The brk function is used to change the amount of memory allocated in a process’s data segment
  *Upon successful completion, brk() returns 0. Otherwise, it returns -1 and sets errno to indicate the error.
  */
 
 
TInt CTestStdio::brkTest()
  {
  TInt ret=brk(0);
  if(ret == -1)
	  {
	  return KErrGeneral;
	  }
  else
	  {
	  return KErrNone;	
	  }
  
  }



/*
 * -------------------------------------------------------------------------------
 *Function Name		  : fseek_negTest()
 *API Tested		  : fseek
 *TestCase Description: To Check the error condtion  by using a large value (off_t) which are greater than TInt(32 bit).
 *-------------------------------------------------------------------------------
 */


TInt CTestStdio::fseek_negTest()
	{
	INFO_PRINTF1(_L("fseek_negTest()"));
	_LIT(KPassed,"Passed");
	_LIT(KFailed,"Failed");
	FILE *fp;
	TInt retval;
	TInt retrn = KErrGeneral;
	off_t  offval = UINT_MAX;
	INFO_PRINTF2(_L("offset value is  : %d"), offval);
    fp = fopen("c:\\input.txt", "a");
	if (fp == NULL)
		{ 
		INFO_PRINTF1(_L ("fopen failed\n")); 
		return KErrGeneral;
		}
	retval = fseek(fp, offval, SEEK_SET);
	INFO_PRINTF2(_L("Value returned by fseek in this case is : %d"), retval);
	INFO_PRINTF2(_L("errno was set to : %d"), errno);
	if (retval == -1 && errno == 22)
		{ 
		INFO_PRINTF1( KPassed);
		retrn = KErrNone;
		}
	else
		{
		INFO_PRINTF1( KFailed);
		retrn = KErrGeneral;
		}
	fclose(fp);
	unlink("c:\\input.txt");
	return retrn;
	}

/*-------------------------------------------------------------------------------
 *Function Name		  :  lseek_negTest()
 *API Tested		  : lseek
 *TestCase Description: To Check the error condtion  by using a large value (off_t) which are greater than TInt(32 bit).
 *-------------------------------------------------------------------------------
 */
TInt CTestStdio::lseek_negTest()
	{
	INFO_PRINTF1(_L("lseek_negTest()"));
	_LIT(KPassed,"Passed");
	_LIT(KFailed,"Failed");
	TInt retrn = KErrGeneral;
#if defined(SYMBIAN_OE_LARGE_FILE_SUPPORT) && !defined(SYMBIAN_OE_NO_LFS)
	off_t retval;
	off_t offval =  ULLONG_MAX;
#else
	TInt retval;
	off_t offval =  UINT_MAX;
#endif /* SYMBIAN_OE_LARGE_FILE_SUPPORT && !SYMBIAN_OE_NO_LFS */
	TInt fp;
	if((fp = open("C:\\lseek.txt", O_RDWR|O_CREAT)) < 0 )
		{
		INFO_PRINTF1(_L ("open failed\n")); 
		return KErrGeneral;
		}
	retval = lseek(fp,offval,SEEK_CUR);
	INFO_PRINTF2(_L("Value returned by lseek in this case is : %d"), retval);
	INFO_PRINTF2(_L("errno was set to : %d"), errno);
	if (retval == -1 && errno == 22)
		{ 
		INFO_PRINTF1( KPassed);
		retrn = KErrNone;
		}
	else
		{
		INFO_PRINTF1( KFailed);
		retrn = KErrGeneral;
		}
	unlink("C:\\lseek.txt");
	close(fp);
	return retrn;
	}
TInt CTestStdio::fwrite4()
	{
	INFO_PRINTF1(_L("fwrite4"));
	int r[10];
	int retval = KErrGeneral;
	int ret = 0,i;
	for(i = 0; i < 10; i++) 
		{
	    r[i] = i;
		}
	ret = fwrite(r, sizeof(int), 10, stderr);
	if(ret == 10)
		{
		INFO_PRINTF1(_L ("fwrite passed\n")); 
		retval = KErrNone;
		}
	return retval;
	}
/*
 * Test Case Name		:ftellTest()
 * API Tested	 		:ftell()
 * Test case Description:To check ftell() gives the correct position if a file is opened
 * before and after fseek().
 */


TInt CTestStdio::ftellTest()
	{
	int ret = KErrNone;
	FILE* fpa = NULL;
	int fdb = 0;
	FILE* fpc = NULL;
	FILE* fpd = NULL;
	char* file = "c:\\hello.txt";
	unlink("c:\\hello.txt");
	do
		{
		fpa = fopen(file, "w");
		fprintf (fpa, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
		if (fpa == NULL)
		     {
		     ret = KErrGeneral;
		     break;
		     
			 }
		fclose(fpa);
		fpa = fopen(file, "r");
		if (fpa == NULL)
			{
			ret = KErrGeneral;
			break;
			
			}
		fdb = fileno(fpa);
	    if (fdb < 0)
	    	{
	    	ret = KErrGeneral;
			break;
			
			} 
		fpc = fdopen(fdb, "r");
		if (fpc == NULL)
			{
			ret = KErrGeneral;
			break;
			
			} 
		int posc1 = ftell(fpc);
		int r  = fseek(fpa,10, SEEK_SET)  ;
		if (r < 0)
			{
			ret = KErrGeneral;
			break;
			
			}
		fpd = fdopen(fdb, "r");
		if (fpd == NULL)
			{
			ret = KErrGeneral;
			break;
			
			}
		int posa = ftell(fpa);
		int posc2 = ftell(fpc);
		int posd = ftell(fpd);
		if (posa == 10 && posc1 == 0 && posc2 == 0 && posd == 10)
			{
			INFO_PRINTF1(_L("Test Passed"));
			ret = KErrNone;
			}
		else
			{
			INFO_PRINTF1(_L("Test Failed"));
			ret = KErrGeneral;
			}
		_LIT(KPostion,"Position are: posa = %d , posc1 = %d ,posc2 = %d , posd = %d\n");
		INFO_PRINTF5(KPostion, posa, posc1,posc2,posd);
		} while (0);
		
	fclose(fpc);
	fclose(fpd);
	fclose(fpa);
	unlink("c:\\hello.txt");
	return ret;
	}
TVerdict CTestStdio::freopen_validmode()
	{
	SetTestStepResult(EPass);
	TBuf8<50> string ;
	TPtrC String ;
	_LIT( KString, "FileName" );
	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		SetTestStepResult(EFail);
		}
	string.Copy(String) ;
	char *filename1 = (char *) string.Ptr() ;
	filename1[string.Length()] = '\0' ;

	FILE *fp1 = fopen(filename1,"w");
	if(fp1 == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		SetTestStepResult(EFail);
		}
	FILE *fp = freopen (NULL,"w",fp1);
	if(fp == NULL)
		{
		INFO_PRINTF1(_L("freopen failed\n"));
		unlink(filename1);
		SetTestStepResult(EFail);
		}
	fclose(fp);
	unlink(filename1);
    return TestStepResult();  	
	}
TVerdict CTestStdio::freopen_invalidmode()
	{
	TBuf8<50> string ;
	TPtrC String ;
	_LIT( KString, "FileName" );
	SetTestStepResult(EPass);

	TBool res = GetStringFromConfig(ConfigSection(), KString, String );
	if(!res) 
		{
   		_LIT(Kerr, "Failed to read input file name from ini file ") ;
   		INFO_PRINTF1(Kerr) ;
   		SetTestStepResult(EFail);
		}
	string.Copy(String) ;
	char *filename1 = (char *) string.Ptr() ;
	filename1[string.Length()] = '\0' ;

	FILE *fp1 = fopen(filename1,"w");
	if(fp1 == NULL)
		{
		INFO_PRINTF1(_L("fopen failed\n"));
		SetTestStepResult(EFail);
		}
	FILE *fp = freopen (NULL,"x",fp1);
	if(fp != NULL)
		{
		INFO_PRINTF1(_L("freopen failed\n"));
		unlink(filename1);
		SetTestStepResult(EFail);
		}
	fclose(fp);
	unlink(filename1);
    return TestStepResult();  	
	}
//  End of File
