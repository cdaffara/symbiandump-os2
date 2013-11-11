// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tfilemodeblocks.cpp
// Test cases for signal api's
//


#include "tstdio.h"

// -----------------------------------------------------------------------------
// CTestStdio::TestFileModeAccess1
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileModeAccess1()
	{
    char writeBuf[5][30] = { "writetest", "writetest\nhi", "writetest\nhi\nhello", "writetest\r\nhi\nhello", "\n\nwritetest\r\nhi\nhello\n" };
    char expected[5][30] = { "writetest", "writetest\nhi", "writetest\nhi\nhello", "writetest\r\nhi\nhello", "\n\nwritetest\r\nhi\nhello\n" };
    char file[5][20] = { "c:\\mode1.txt", "c:\\mode2.txt", "c:\\mode3.txt", "c:\\mode4.txt", "c:\\mode5.txt"};
    set_fmode('t');
    for (int i = 0; i <=4; i++)
        {
        FILE *fp1 = fopen(file[i], "w");
        if (!fp1)
            {
            ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
            return TestStepResult();
            }
        size_t written = 0, read = 0;
        written = fwrite(writeBuf[i], sizeof(char), strlen(writeBuf[i]), fp1);
        if (written != strlen(writeBuf[i])) 
            {
            ERR_PRINTF2(_L("Failed to write to a file and errno is %d"),errno);
            fclose(fp1);
            return TestStepResult();
            } 
        fclose(fp1);
        FILE *fp2 = fopen(file[i], "r");
        if (!fp2)
            {
            ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
            return TestStepResult();
            }
        char readBuf[50];
        read = fread(readBuf, sizeof(char), strlen(expected[i]), fp2);
        if (read != strlen(expected[i]))
            {
            ERR_PRINTF1(_L("Error in reading from a file"));
            fclose(fp2);
            return TestStepResult();
            }
        fclose(fp2);
        if( (strncmp(readBuf,expected[i],strlen(expected[i])) ) != 0 )       
            {
            ERR_PRINTF2(_L("error in strcmp when i is %d"),i);
            return TestStepResult();
            }        
        }
    SetTestStepResult(EPass);
	return TestStepResult();
	}	


// -----------------------------------------------------------------------------
// CTestStdio::TestFileModeAccess2
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileModeAccess2()
	{
    char writeBuf[5][30] = { "writetest", "writetest\nhi", "writetest\nhi\nhello", "writetest\r\nhi\nhello", "\n\nwritetest\r\nhi\nhello\n" };
    char expected[5][30] = { "writetest", "writetest\nhi", "writetest\nhi\nhello", "writetest\r\nhi\nhello", "\n\nwritetest\r\nhi\nhello\n" };
    char file[5][20] = { "c:\\mode1.txt", "c:\\mode2.txt", "c:\\mode3.txt", "c:\\mode4.txt", "c:\\mode5.txt"};
    set_fmode('t');
    for (int i = 0; i <=4; i++)
        {
        FILE *fp1 = fopen(file[i], "wt");
        if (!fp1)
            {
            ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
            return TestStepResult();
            }
        size_t written = 0, read = 0;
        written = fwrite(writeBuf[i], sizeof(char), strlen(writeBuf[i]), fp1);
        if (written != strlen(writeBuf[i])) 
            {
            ERR_PRINTF2(_L("Failed to write to a file and errno is %d"),errno);
            fclose(fp1);
            return TestStepResult();
            } 
        fclose(fp1);
        FILE *fp2 = fopen(file[i], "r");
        if (!fp2)
            {
            ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
            return TestStepResult();
            }
        char readBuf[50];
        read = fread(readBuf, sizeof(char), strlen(expected[i]), fp2);
        if (read != strlen(expected[i]))
            {
            ERR_PRINTF1(_L("Error in reading from a file"));
            fclose(fp2);
            return TestStepResult();
            }
        fclose(fp2);
        if( (strncmp(readBuf,expected[i],strlen(expected[i])) ) != 0 )       
            {
            ERR_PRINTF2(_L("error in strcmp when i is %d"),i);
            return TestStepResult();
            }        
        }
    SetTestStepResult(EPass);
	return TestStepResult();
	}	

// -----------------------------------------------------------------------------
// CTestStdio::TestFileModeAccess3
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileModeAccess3(  )
	{
    char writeBuf[5][30] = { "writetest", "writetest\nhi", "writetest\nhi\nhello", "writetest\r\nhi\nhello", "\n\nwritetest\r\nhi\nhello\n" };
    char expected[5][30] = { "writetest", "writetest\nhi", "writetest\nhi\nhello", "writetest\nhi\nhello", "\n\nwritetest\nhi\nhello\n" };
    char file[5][20] = { "c:\\mode1.txt", "c:\\mode2.txt", "c:\\mode3.txt", "c:\\mode4.txt", "c:\\mode5.txt"};
    set_fmode('t');
    for (int i = 0; i <=4; i++)
        {
        FILE *fp1 = fopen(file[i], "wb");
        if (!fp1)
            {
            ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
            return TestStepResult();
            }
        size_t written = 0, read = 0;
        written = fwrite(writeBuf[i], sizeof(char), strlen(writeBuf[i]), fp1);
        if (written != strlen(writeBuf[i])) 
            {
            ERR_PRINTF2(_L("Failed to write to a file and errno is %d"),errno);
            fclose(fp1);
            return TestStepResult();
            } 
        fclose(fp1);
        FILE *fp2 = fopen(file[i], "r");
        if (!fp2)
            {
            ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
            return TestStepResult();
            }
        char readBuf[50];
        read = fread(readBuf, sizeof(char), strlen(expected[i]), fp2);
        if (read != strlen(expected[i]))
            {
            ERR_PRINTF1(_L("Error in reading from a file"));
            fclose(fp2);
            return TestStepResult();
            }
        fclose(fp2);
        if( (strncmp(readBuf,expected[i],strlen(expected[i])) ) != 0 )       
            {
            ERR_PRINTF2(_L("error in strcmp when i is %d"),i);
            return TestStepResult();
            }        
        }
    SetTestStepResult(EPass);
	return TestStepResult();
	}	


// -----------------------------------------------------------------------------
// CTestStdio::TestFileModeAccess4
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileModeAccess4()
	{
    char writeBuf[5][30] = { "writetest", "writetest\nhi", "writetest\nhi\nhello", "writetest\r\nhi\nhello", "\n\nwritetest\r\nhi\nhello\n" };
    char expected[5][30] = { "writetest", "writetest\nhi", "writetest\nhi\nhello", "writetest\r\nhi\nhello", "\n\nwritetest\r\nhi\nhello\n" };
    char file[5][20] = { "c:\\mode1.txt", "c:\\mode2.txt", "c:\\mode3.txt", "c:\\mode4.txt", "c:\\mode5.txt"};
    set_fmode('b');
    for (int i = 0; i <=4; i++)
        {
        FILE *fp1 = fopen(file[i], "w");
        if (!fp1)
            {
            ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
            return TestStepResult();
            }
        size_t written = 0, read = 0;
        written = fwrite(writeBuf[i], sizeof(char), strlen(writeBuf[i]), fp1);
        if (written != strlen(writeBuf[i])) 
            {
            ERR_PRINTF2(_L("Failed to write to a file and errno is %d"),errno);
            fclose(fp1);
            return TestStepResult();
            } 
        fclose(fp1);
        FILE *fp2 = fopen(file[i], "r");
        if (!fp2)
            {
            ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
            return TestStepResult();
            }
        char readBuf[50];
        read = fread(readBuf, sizeof(char), strlen(expected[i]), fp2);
        if (read != strlen(expected[i]))
            {
            ERR_PRINTF1(_L("Error in reading from a file"));
            fclose(fp2);
            return TestStepResult();
            }
        fclose(fp2);
        if( (strncmp(readBuf,expected[i],strlen(expected[i])) ) != 0 )       
            {
            ERR_PRINTF2(_L("error in strcmp when i is %d"),i);
            return TestStepResult();
            }        
        }
    SetTestStepResult(EPass);
	return TestStepResult();
	}	


// -----------------------------------------------------------------------------
// CTestStdio::TestFileModeAccess5
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileModeAccess5(  )
	{
    char writeBuf[5][30] = { "writetest", "writetest\nhi", "writetest\nhi\nhello", "writetest\r\nhi\nhello", "\n\nwritetest\r\nhi\nhello\n" };
    char expected[5][30] = { "writetest", "writetest\r\nhi", "writetest\r\nhi\r\nhello", "writetest\r\r\nhi\r\nhello", "\r\n\r\nwritetest\r\r\nhi\r\nhello\r\n" };
    char file[5][20] = { "c:\\mode1.txt", "c:\\mode2.txt", "c:\\mode3.txt", "c:\\mode4.txt", "c:\\mode5.txt"};
    set_fmode('b');
    for (int i = 0; i <=4; i++)
        {
        FILE *fp1 = fopen(file[i], "wt");
        if (!fp1)
            {
            ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
            return TestStepResult();
            }
        size_t written = 0, read = 0;
        written = fwrite(writeBuf[i], sizeof(char), strlen(writeBuf[i]), fp1);
        if (written != strlen(writeBuf[i])) 
            {
            ERR_PRINTF2(_L("Failed to write to a file and errno is %d"),errno);
            fclose(fp1);
            return TestStepResult();
            } 
        fclose(fp1);
        FILE *fp2 = fopen(file[i], "r");
        if (!fp2)
            {
            ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
            return TestStepResult();
            }
        char readBuf[50];
        read = fread(readBuf, sizeof(char), strlen(expected[i]), fp2);
        if (read != strlen(expected[i]))
            {
            ERR_PRINTF1(_L("Error in reading from a file"));
            fclose(fp2);
            return TestStepResult();
            }
        fclose(fp2);
        if( (strncmp(readBuf,expected[i],strlen(expected[i])) ) != 0 )       
            {
            ERR_PRINTF2(_L("error in strcmp when i is %d"),i);
            return TestStepResult();
            }        
        }
    SetTestStepResult(EPass);
	return TestStepResult();
	}	


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CTestStdio::TestFileModeAccess6
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileModeAccess6(  )
	{
    char writeBuf[5][30] = { "writetest", "writetest\nhi", "writetest\nhi\nhello", "writetest\r\nhi\nhello", "\n\nwritetest\r\nhi\nhello\n" };
    char expected[5][30] = { "writetest", "writetest\nhi", "writetest\nhi\nhello", "writetest\r\nhi\nhello", "\n\nwritetest\r\nhi\nhello\n" };
    char file[5][20] = { "c:\\mode1.txt", "c:\\mode2.txt", "c:\\mode3.txt", "c:\\mode4.txt", "c:\\mode5.txt"};
    set_fmode('b');
    for (int i = 0; i <=4; i++)
        {
        FILE *fp1 = fopen(file[i], "wb");
        if (!fp1)
            {
            ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
            return TestStepResult();
            }
        size_t written = 0, read = 0;
        written = fwrite(writeBuf[i], sizeof(char), strlen(writeBuf[i]), fp1);
        if (written != strlen(writeBuf[i])) 
            {
            ERR_PRINTF2(_L("Failed to write to a file and errno is %d"),errno);
            fclose(fp1);
            return TestStepResult();
            } 
        fclose(fp1);
        FILE *fp2 = fopen(file[i], "r");
        if (!fp2)
            {
            ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
            return TestStepResult();
            }
        char readBuf[50];
        read = fread(readBuf, sizeof(char), strlen(expected[i]), fp2);
        if (read != strlen(expected[i]))
            {
            ERR_PRINTF1(_L("Error in reading from a file"));
            fclose(fp2);
            return TestStepResult();
            }
        fclose(fp2);
        if( (strncmp(readBuf,expected[i],strlen(expected[i])) ) != 0 )       
            {
            ERR_PRINTF2(_L("error in strcmp when i is %d"),i);
            return TestStepResult();
            }        
        }
    SetTestStepResult(EPass);
	return TestStepResult();
	}	


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CTestStdio::TestFileModeAccess7
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileModeAccess7()
	{
    char writeBuf[5][30] = { "writetest", "writetest\nhi", "writetest\nhi\nhello", "writetest\r\nhi\nhello", "\n\nwritetest\r\nhi\nhello\n" };
    char expected[5][30] = { "writetest", "writetest\r\nhi", "writetest\r\nhi\r\nhello", "writetest\r\r\nhi\r\nhello", "\r\n\r\nwritetest\r\r\nhi\r\nhello\r\n" };
    char file[5][20] = { "c:\\mode1.txt", "c:\\mode2.txt", "c:\\mode3.txt", "c:\\mode4.txt", "c:\\mode5.txt"};
    set_fmode('t');
    for (int i = 0; i <=4; i++)
        {
        FILE *fp1 = fopen(file[i], "w");
        if (!fp1)
            {
            ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
            return TestStepResult();
            }
        size_t written = 0, read = 0;
        written = fwrite(writeBuf[i], sizeof(char), strlen(writeBuf[i]), fp1);
        if (written != strlen(writeBuf[i])) 
            {
            ERR_PRINTF2(_L("Failed to write to a file and errno is %d"),errno);
            fclose(fp1);
            return TestStepResult();
            } 
        fclose(fp1);
        FILE *fp2 = fopen(file[i], "rb");
        if (!fp2)
            {
            ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
            return TestStepResult();
            }
        char readBuf[50];
        read = fread(readBuf, sizeof(char), strlen(expected[i]), fp2);
        if (read != strlen(expected[i]))
            {
            ERR_PRINTF1(_L("Error in reading from a file"));
            fclose(fp2);
            return TestStepResult();
            }
        fclose(fp2);
        if( (strncmp(readBuf,expected[i],strlen(expected[i])) ) != 0 )       
            {
            ERR_PRINTF2(_L("error in strcmp when i is %d"),i);
            return TestStepResult();
            }        
        }
    SetTestStepResult(EPass);
	return TestStepResult();
	}	

void CleanupOnfiles(FILE *fw, FILE *fr)
	{
	fclose(fw);
	fclose(fr);
	unlink("c:\\temp_tc.txt");
	unlink("c:\\temp_tc_out.txt");
	}

// -----------------------------------------------------------------------------
// CTestStdio::TestFileGetsPos
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileGetsPos()
	{
    char *data = "helloworld\nfine";
    //char *p = NULL;
    int ret = 0;
    FILE *fw, *fr;
    
    set_fmode('t');
    fw = fopen("c:\\temp_tc.txt", "w");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    int count = fwrite(data, 1, strlen(data), fw);
    fclose(fw);
    fw = fopen("c:\\temp_tc_out.txt", "w");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        fclose(fw);
        return TestStepResult();
        }
    fr = fopen("c:\\temp_tc.txt", "r");
    if (fr == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    char p[50];
    char *retn = fgets(p, count, fr);
    if(strlen(p) != 11)
        {
        ERR_PRINTF1(_L("1st read failed\n"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();      
        }
    int pos = ftell(fr);                                                // 12
    if (pos != 12)
    	{
        ERR_PRINTF1(_L(" Error in getting the POS after 1st read\n"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();      
    	}
    INFO_PRINTF2(_L("POS After 1st read: %d"), pos);
    ret = fseek(fr,pos,SEEK_SET);
    if (ret != 0)
    	{
        ERR_PRINTF1(_L("Failed in fseek()"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();     	
    	}
    ret = fread(p+11,1,4,fr);
    if( ret != 4)
        {
        ERR_PRINTF1(_L("Failed to read using fread()"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();      
        }
    p[count] = '\0';
    pos = ftell(fr);                                                    // 16      
    if (pos != 16)
    	{
        ERR_PRINTF1(_L(" Error in getting the POS after 2nd read"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();      
    	}
    INFO_PRINTF2(_L("POS After 2nd read: %d\n"), pos);
    count = fwrite(p, 1, strlen(p), fw);
    if(count != strlen(p))
        {
        ERR_PRINTF1(_L(" Failed to write onto another file"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();      
        }
    INFO_PRINTF1(_L("Passed to write onto another file"));
    CleanupOnfiles(fw,fr);
    SetTestStepResult(EPass);
	return TestStepResult();                                
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileTellReadWrite
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileTellReadWrite()
    {
    char *data = "helloworld\nfine";
    int ret = 0;
    FILE *fw, *fr;
    
    set_fmode('t');
    fw = fopen("c:\\temp_tc.txt", "w");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    int count = fwrite(data, 1, strlen(data), fw);
    fclose(fw);
    
    fw = fopen("c:\\temp_tc_out.txt", "w");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    fr = fopen("c:\\temp_tc.txt", "r");
    if (fr == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    char p[50];    
    // Reading 11-bytes gives "helloworld\n" and offset will be 12, ftell() gives 12, resetting to 0th position, 
    // again reading 11 bytes should give "helloworld\n" and should return offset = 12....
    ret = fread(p,1,11,fr);                                     // reading first 11 bytes
    if(ret != 11)
        {
        ERR_PRINTF1(_L("fread() failed 1st time "));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();      
        }
    int pos = ftell(fr);                                        // 12/12                                          //  12/12
    if (pos != 12)
    	{
        ERR_PRINTF1(_L("ftell() failed"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();      	
    	}
    INFO_PRINTF2(_L(" Position after 1st read is: %d"), pos);
    fseek(fr, 0, SEEK_SET);                                     // resetting to 0th position
    ret = fread(p,1,11,fr);                                     // reading again first 11 bytes
    if(ret != 11)
        {
        ERR_PRINTF1(_L("fread() failed 2nd time"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();      
        }
    pos = ftell(fr);                                            //  12/12
    if (pos != 12)
    	{
        ERR_PRINTF1(_L("ftell() failed"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();      	
    	}
    INFO_PRINTF2(_L(" Position after 2nd read is: %d"), pos);
    ret = fread(p+11,1,4,fr);                                   // reading next 4 bytes
    if(ret != 4)
        {
        ERR_PRINTF1(_L("fread() failed 1st time"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();      
        }
    pos = ftell(fr);                                            // 16/16  
    if (pos != 16)
    	{
        ERR_PRINTF1(_L("ftell() failed"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();      	
    	}
    INFO_PRINTF2(_L(" Position after 3rd read is: %d"), pos);
    p[count] = '\0';
    count = fwrite(p, 1, strlen(p), fw);
    if(count != strlen(p))
        {
        ERR_PRINTF1(_L("Writing back onto other file failed"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();      
        }
    INFO_PRINTF1(_L("Writing back onto other file passed"));
    CleanupOnfiles(fw,fr);

    SetTestStepResult(EPass);
    return TestStepResult();        
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileTellSeek
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileTellSeek()
    {
    char *data = "helloworld\nfine";
    int ret = 0;
    FILE *fw, *fr;
    
    set_fmode('t');
    fw = fopen("c:\\temp_tc.txt", "w");
    int count = fwrite(data, 1, strlen(data), fw);
    fclose(fw);
    fw = fopen("c:\\temp_tc_out.txt", "w");
    fr = fopen("c:\\temp_tc.txt", "r");
    char p[50];       

    ret = fread(p,1,11,fr);
    int pos = ftell(fr);                                // 12/12
    INFO_PRINTF2(_L(" Position after 1st read is: %d"), pos);
    ret = fread(p+11,1,4,fr);
    
    int pos1 = ftell(fr);                               // 16/16
    INFO_PRINTF2(_L(" Position after 2nd read is: %d"), pos1);
    fseek(fr, pos, SEEK_SET);                           // resetting to pos
    pos = ftell(fr);                                    // 12/12 
    INFO_PRINTF2(_L(" Position after fseek is: %d"), pos);

    ret = fread(p+11, 1, 4, fr);
    pos = ftell(fr);
    INFO_PRINTF2(_L(" Position after 3rd read is: %d"), pos);   // 16/16
    if( ret != 4)
        {
        ERR_PRINTF1(_L("Failed during 3rd read"));
        CleanupOnfiles(fw,fr);
        return TestStepResult();        
        }
    p[count] = '\0';
    count = fwrite(p, 1, strlen(p), fw);
    if(count != strlen(p))
        {
        ERR_PRINTF1(_L(" Failed to write onto another file"));
        CleanupOnfiles(fw,fr);
        return TestStepResult();        
        }
    INFO_PRINTF1(_L("Passed to write onto another file\n"));
    CleanupOnfiles(fw,fr);

    SetTestStepResult(EPass);
    return TestStepResult();        
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileLargeData
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileLargeData()
    {
    FILE *fp = NULL, *fpout = NULL;
    size_t count = 0;
    size_t ret = 0;
    char *ptr1 = "sanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\nsanjay\n";

    int length = strlen(ptr1);
    set_fmode('t');
    if (get_fmode() != 't')
    	{
        ERR_PRINTF1(_L("Failed in getting the file mode"));
        return TestStepResult();  	
    	}
    fp = fopen("c:\\sample.txt", "w"); // rt,rt+,r+t,wt,wt+,w+t,at,at+,a+t // r,w,a,r+,w+,a+
    if (fp == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    count = fwrite(ptr1, 1, strlen(ptr1),fp);
    fclose(fp);
    fp = NULL;
    
    // reading from a file
    fp = fopen("c:\\sample.txt", "r");
    if (fp == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }               
    
    // Open another file for writing
    fpout = fopen("c:\\output.txt", "w"); // w+
    if (fpout == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }  
    char *p = NULL;
    p = (char *)malloc(count+1); // extra one is for holding '\0'
    ret = fread(p, 1, 1050, fp);
    if(ret != 1050)
        {
        ERR_PRINTF1(_L("fread() failed "));
        return TestStepResult();
        }
    int offset1 = ftell(fp);                                // 1050 + 150 = 1200
    INFO_PRINTF2(_L(" offset1 after 1st read: %d"), offset1);
    ret = fread(p+1050,1,450,fp);
    if(ret!=450)
        {
        ERR_PRINTF1(_L("2nd read failed"));
        return TestStepResult();
        }
    int offset2 = ftell(fp);                            // 1500 + 214 = 1714
    INFO_PRINTF2(_L(" offset2 after 2nd read: %d"), offset2);
    fseek(fp, offset1, SEEK_SET);       // 1200 -->> failing bcoz this falls into fseek optimization part of code
    offset2 = ftell(fp);                  // 1650  ---->>>> 512 - 450 = 62 in fp->_r
    INFO_PRINTF2(_L(" offset2 after fseek(offset1 i.e. 1200 ) read: %d"), offset2);             // 1200
    ret = fread(p+1050,1,450,fp);
    if(ret!=450)
    	{
    	ERR_PRINTF1(_L("3rd read failed"));
        return TestStepResult();
    	}
    offset2 = ftell(fp);                                // 1614
    INFO_PRINTF2(_L(" offset2 after 3rd read: %d"), offset2);
    ret = fread(p+1500,1,180,fp);
    if(ret!=180)
    	{
    	ERR_PRINTF1(_L("4th read failed"));
        return TestStepResult();
    	}
    offset2 = ftell(fp);                            // 1680 + 240 = 1920
    INFO_PRINTF2(_L(" offset2 after 4th read: %d"), offset2);
    INFO_PRINTF1(_L(" fread() also passed "));
    p[count] = '\0';
    // write back the read contents into the output.txt file
    count = fwrite(p, 1, strlen(p),fpout);
    length = count;
    if(count != strlen(p))
    	{
    	ERR_PRINTF1(_L("Writing onto another file failed"));
        return TestStepResult();
    	}
    INFO_PRINTF1(_L("Writing onto another file passed"));
    free((void*)p);
    fclose(fp);
    fclose(fpout);
    unlink("c:\\output.txt");
    SetTestStepResult(EPass);
    return TestStepResult();        
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileReadTell
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileReadTell()
    {
    char *data = "helloworld\nfine";
    int retn = 0;
    FILE *fw, *fr;
    
    set_fmode('t');
    fw = fopen("c:\\temp_tc.txt", "w");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }  
    fwrite(data, 1, strlen(data), fw);
    fclose(fw);

    fr = fopen("c:\\temp_tc.txt", "r");    
    if (fr == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }  
    char p[50];
    retn = fread(p, 1, strlen(data), fr);
    if(retn != (strlen(data)))
        {
        ERR_PRINTF1(_L("fread() failed "));
        fclose(fr);
        unlink("c:\\temp_tc.txt");
        return TestStepResult();     
        }
    INFO_PRINTF2(_L(" Return value from fread is: %d"), retn);

    int pos = ftell(fr);
    if (pos != 16)
    	{
        ERR_PRINTF1(_L("fread() failed "));
        fclose(fr);
        unlink("c:\\temp_tc.txt");
        return TestStepResult();       	
    	}
    INFO_PRINTF2(_L("ftell - after reading: %d"), pos);

    fclose(fr);
    unlink("c:\\temp_tc.txt");
    SetTestStepResult(EPass);
    return TestStepResult();       
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileReadSeekEnd
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileReadSeekEnd()
    {
    char *data = "helloworld\nfine\n\n"; // "helloworld\nfine" -->> "helloworld\r\nfine" , "helloworld\nab\n\n" -->> "helloworld\r\nab\r\n\r\n" , 
    int ret = 0;
    FILE *fw, *fr;
    
    set_fmode('t');
    fw = fopen("c:\\temp_tc.txt", "w");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    int count = fwrite(data, 1, strlen(data), fw);
    fclose(fw);
    fw = fopen("c:\\temp_tc_out.txt", "w");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    fr = fopen("c:\\temp_tc.txt", "r");
    if (fr == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    char p[50];
    char *retn = fgets(p, count, fr);
    if(strlen(p) != 11)
        {
        ERR_PRINTF1(_L("1st read failed"));
        }
    int pos = ftell(fr);
    INFO_PRINTF2(_L("ftell - after reading: %d"), pos);
    fseek(fr,-pos,SEEK_END);         // Now, working fine with new implementation 
    ret = fread(p+8,1,9,fr);
    if( ret != 9)
        {
        ERR_PRINTF2(_L("Return value from fread() is: %d"), ret);
        ERR_PRINTF1(_L("Failed to read using fread()"));
        }
    p[count] = '\0';
    pos = ftell(fr);
    INFO_PRINTF2(_L(" ftell - after 2nd read is: %d"), pos);
    count = fwrite(p, 1, strlen(p), fw);
    if(count != strlen(p))
        {
        ERR_PRINTF1(_L(" Failed to write onto another file"));
        }
    INFO_PRINTF1(_L("Passed to write onto another file"));

    fclose(fr);
    fclose(fw);
    SetTestStepResult(EPass);
    return TestStepResult();       
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileWriteSeekEnd
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileWriteSeekEnd()
    {
    char *data = "helloworld\nfine\n\n"; // -->> "helloworld\r\nfine\r\n\r\n", 
    int ret = 0;
    FILE *fw, *fr;
    
    set_fmode('t');
    fw = fopen("c:\\temp_tc.txt", "w");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    int count = fwrite(data, 1, 11, fw);
    if(count != 11)
        {
        ERR_PRINTF1(_L("1st fwrite() failed"));
        fclose(fw);
        unlink("c:\\temp_tc.txt");
        return TestStepResult();        
        }
    int pos = ftell(fw);                                   // 12 
    INFO_PRINTF2(_L(" ftell - After 1st write is: %d"), pos);
    count += fwrite(data+11, 1, 6, fw);
    if(count != strlen(data))
        {
        ERR_PRINTF1(_L(" 2nd fwrite() failed"));
        fclose(fw);
        unlink("c:\\temp_tc.txt");
        return TestStepResult();        
        }
    pos = ftell(fw);                                        // 20 
    INFO_PRINTF2(_L(" ftell - After 1st write is: %d"), pos);
    fclose(fw);
    fw = fopen("c:\\temp_tc_out.txt", "w");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    fr = fopen("c:\\temp_tc.txt", "r");
    if (fr == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        fclose(fw);
        return TestStepResult();
        }
    char p[50];
    char *retn = fgets(p, count, fr);
    if(strlen(p) != 11)
        {
        ERR_PRINTF1(_L("1st read failed"));
        CleanupOnfiles(fw,fr);
        return TestStepResult();        
        }
    pos = ftell(fr);
    INFO_PRINTF2(_L("ftell - after reading: %d"), pos);
    fseek(fr,-pos,SEEK_END);         
    ret = fread(p+8,1, 9,fr);
    if( ret != 9)
        {
        ERR_PRINTF2(_L("Return value from fread() is: %d"), ret);
        ERR_PRINTF1(_L("Failed to read using fread()"));
        CleanupOnfiles(fw,fr);
        return TestStepResult();        
        }
    p[count] = '\0';
    pos = ftell(fr);
    INFO_PRINTF2(_L(" ftell - after 2nd read is: %d"), pos);
    count = fwrite(p, 1, strlen(p), fw);
    if(count != strlen(p))
        {
        ERR_PRINTF1(_L(" Failed to write onto another file"));
        CleanupOnfiles(fw,fr);
        return TestStepResult();        
        }
    INFO_PRINTF1(_L("Passed to write onto another file"));

    CleanupOnfiles(fw,fr);   
    SetTestStepResult(EPass);
    return TestStepResult();     
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileFlushWriteSeekEnd
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileFlushWriteSeekEnd()
    {
    char *data = "helloworld\nfine\n\n"; // -->> "helloworld\r\nfine\r\n\r\n", 
    int ret = 0;
    FILE *fw = NULL, *fr = NULL;
    
    set_fmode('t');
    unlink("c:\\temp_tc.txt");
    fw = fopen("c:\\temp_tc.txt", "w");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    int count = fwrite(data, 1, 11, fw);
    if(count != 11)
        {
        ERR_PRINTF1(_L("1st fwrite() failed"));
        fclose(fw);
        return TestStepResult();        
        }
    int pos = ftell(fw);                                   // 12 
    INFO_PRINTF2(_L(" ftell - After 1st write is: %d"), pos);
    count += fwrite(data+11, 1, 6, fw);
    if(count != strlen(data))
        {
        ERR_PRINTF1(_L(" 2nd fwrite() failed"));
        fclose(fw);
        return TestStepResult();        
        }
    pos = ftell(fw);                                        // 20   
    INFO_PRINTF2(_L(" ftell - After 1st write is: %d"), pos);
    
    // Flush and call ftell() to check the current offset.
    fflush(fw);
    pos = ftell(fw);                                        // 20 
    INFO_PRINTF2(_L(" ftell - After flushing the buffer is: %d"), pos);

    fclose(fw);
    fw = fopen("c:\\temp_tc_out.txt", "w");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    fr = fopen("c:\\temp_tc.txt", "r");
    if (fr == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        fclose(fw);
        return TestStepResult();
        }
    char p[50];
    char *retn = fgets(p, count, fr);
    if(strlen(p) != 11)
        {
        ERR_PRINTF1(_L("1st read failed"));
        CleanupOnfiles(fw,fr);
        return TestStepResult();        
        }
    pos = ftell(fr);
    INFO_PRINTF2(_L("ftell - after reading: %d"), pos);
    
    fseek(fr,-pos,SEEK_END);
             
    ret = fread(p+8,1, 9,fr);
    if( ret != 9)
        {
        ERR_PRINTF2(_L("Return value from fread() is: %d"), ret);
        ERR_PRINTF1(_L("Failed to read using fread()"));
        CleanupOnfiles(fw,fr);
        return TestStepResult();        
        }
    p[count] = '\0';
    pos = ftell(fr);
    INFO_PRINTF2(_L(" ftell - after 2nd read is: %d\n"), pos);
    count = fwrite(p, 1, strlen(p), fw);
    if(count != strlen(p))
        {
        ERR_PRINTF1(_L(" Failed to write onto another file"));
        CleanupOnfiles(fw,fr);
        return TestStepResult();        
        }
    INFO_PRINTF1(_L("Passed to write onto another file"));

    CleanupOnfiles(fw,fr);
    SetTestStepResult(EPass);
    return TestStepResult();     
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileFlushWriteSeek
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileFlushWriteSeek()
    {
    char *data = "helloworld\nfine\n\n"; // -->> "helloworld\r\nfine\r\n\r"), 
    int ret = 0;
    FILE *fw = NULL, *fr = NULL;
    
    set_fmode('t');
    fw = fopen("c:\\temp_tc.txt", "w");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    int count = fwrite(data, 1, 11, fw);
    if(count != 11)
        {
        ERR_PRINTF1(_L("1st fwrite() failed"));
        }
    int pos = ftell(fw);                                   // 12 
    INFO_PRINTF2(_L(" ftell - After 1st write is: %d"), pos);
    count += fwrite(data+11, 1, 6, fw);
    if(count != strlen(data))
        {
        ERR_PRINTF1(_L(" 2nd fwrite() failed"));
        }
    pos = ftell(fw);                                        // 20  
    INFO_PRINTF2(_L(" ftell - After 1st write is: %d"), pos);
    
    // Flush and call ftell() to check the current offset.
    fflush(fw);
    pos = ftell(fw);                                        // 20   
    INFO_PRINTF2(_L(" ftell - After flushing the buffer is: %d"), pos);
    
    fseek(fw, 0, SEEK_SET);
    pos = ftell(fw);                                        // 0
    INFO_PRINTF2(_L(" ftell - After seeking to beginning of file: %d"), pos);
    int count2 = fwrite(data, 1, 11, fw);
    if(count2 != 11)
        {
        ERR_PRINTF1(_L("Failed to write after fseek() "));
        fclose(fw);
        return TestStepResult();        
        }
    pos = ftell(fw);                                        // 12
    INFO_PRINTF2(_L(" ftell - After seeking to beginning of file and writing 11- bytes: %d"), pos);
  
    fclose(fw);
    
    fw = fopen("c:\\temp_tc_out.txt", "w");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    pos = ftell(fw);                                        // 0
    INFO_PRINTF2(_L(" ftell - After opening file in write-mode: %d"), pos);

    fr = fopen("c:\\temp_tc.txt", "r");
    if (fr == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        fclose(fw);
        return TestStepResult();
        }
    pos = ftell(fr);                                        // 0
    INFO_PRINTF2(_L(" ftell - After opening file in read-mode: %d"), pos);

    char p[50];
    char *retn = fgets(p, count, fr);
    if(strlen(p) != 11)
        {
        ERR_PRINTF1(_L("1st read failed"));
        CleanupOnfiles(fw,fr);
        return TestStepResult();        
        }
    pos = ftell(fr);                                        // 12
    INFO_PRINTF2(_L("ftell - after reading: %d"), pos);
    
    fseek(fr,-pos,SEEK_END);   
          
    ret = fread(p+8,1, 9,fr);
    if( ret != 9)
        {
        ERR_PRINTF2(_L("Return value from fread() is: %d"), ret);
        ERR_PRINTF1(_L("Failed to read using fread()"));
        CleanupOnfiles(fw,fr);
        return TestStepResult();        
        }
    p[count] = '\0';
    pos = ftell(fr);                                        // 20 
    INFO_PRINTF2(_L(" ftell - after 2nd read is: %d"), pos);
    count = fwrite(p, 1, strlen(p), fw);
    if(count != strlen(p))
        {
        ERR_PRINTF1(_L(" Failed to write onto another file"));
        CleanupOnfiles(fw,fr);
        return TestStepResult();        
        }   
    INFO_PRINTF1(_L("Passed to write onto another file"));

    CleanupOnfiles(fw,fr); 
    SetTestStepResult(EPass);
    return TestStepResult();     
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileFlushSeekCur
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileFlushSeekCur()
    {
    char *data = "helloworld\nfine\n\n"; // -->> "helloworld\r\nfine\r\n\r\n", 
    int ret = 0;
    FILE *fw = NULL, *fr = NULL;
    
    set_fmode('t');
    fw = fopen("c:\\temp_tc.txt", "w");  
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }    
    int count = fwrite(data, 1, 11, fw);
    if(count != 11)
        {
        ERR_PRINTF1(_L("1st fwrite() failed"));
        }
    int pos = ftell(fw);                                   // 12 
    INFO_PRINTF2(_L(" ftell - After 1st write is: %d"), pos);
    count += fwrite(data+11, 1, 6, fw);
    if(count != strlen(data))
        {
        ERR_PRINTF1(_L(" 2nd fwrite() failed"));
        }
    int pos1 = ftell(fw);                                        // 20   
    INFO_PRINTF2(_L(" ftell - After 2nd write is: %d"), pos1);
    
    // SEEK_CUR
    fseek(fw, -pos, SEEK_CUR);
    pos = ftell(fw);                                        // ( 20 - 12 ) = 8
    INFO_PRINTF2(_L(" ftell - After seeking using SEEK_CUR to pos and Writing 6-bytes is: %d"), pos);

    int count2 = fwrite(data+8, 1, 6, fw);
    if( count2 != 6)
        {
        ERR_PRINTF1(_L(" Failed to write after SEEK_CUR "));
        CleanupOnfiles(fw,fr);
        }
    pos = ftell(fw);                                        // 15
    INFO_PRINTF2(_L(" ftell - After seeking using SEEK_CUR to pos and Writing 6-bytes is: %d"), pos);

    // Flush and call ftell() to check the current offset.
    fflush(fw);
    pos = ftell(fw);                                        // 20  
    INFO_PRINTF2(_L(" ftell - After flushing the buffer is: %d"), pos);
    
    fseek(fw, 0, SEEK_SET);
    pos = ftell(fw);                                        // 0
    INFO_PRINTF2(_L(" ftell - After seeking to beginning of file: %d"), pos);
    
    fclose(fw);
    
    fw = fopen("c:\\temp_tc_out.txt", "w");
    pos = ftell(fw);                                        // 0
    INFO_PRINTF2(_L(" ftell - After opening file in write-mode: %d"), pos);

    fr = fopen("c:\\temp_tc.txt", "r");  
    if (fr == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        CleanupOnfiles(fw,fr);
        return TestStepResult();
        }    
    pos = ftell(fr);                                        // 0
    INFO_PRINTF2(_L(" ftell - After opening file in read-mode: %d"), pos);
    char p[50];

    char *retn = fgets(p, count, fr);
    if(strlen(p) != 11)
        {
        ERR_PRINTF1(_L("1st read failed"));
        CleanupOnfiles(fw,fr);
        return TestStepResult();    
        }
    pos = ftell(fr);                                        // 12
    INFO_PRINTF2(_L("ftell - after reading: %d"), pos);
    
    fseek(fr,-pos,SEEK_END);   
          
    ret = fread(p+8,1, 9,fr);
    if( ret != 9)
        {
        ERR_PRINTF2(_L("Return value from fread() is: %d"), ret);
        ERR_PRINTF1(_L("Failed to read using fread()"));
        CleanupOnfiles(fw,fr);
        return TestStepResult();    
        }
    p[count] = '\0';
    pos = ftell(fr);                                        // 20 
    INFO_PRINTF2(_L(" ftell - after 2nd read is: %d"), pos);
    count = fwrite(p, 1, strlen(p), fw);
    if(count != strlen(p))
        {
        ERR_PRINTF1(_L(" Failed to write onto another file"));
        CleanupOnfiles(fw,fr);
        return TestStepResult();    
        }
    INFO_PRINTF1(_L("Passed to write onto another file"));
        
    fclose(fr);
    fclose(fw);
    SetTestStepResult(EPass);
    return TestStepResult();    
    }


// -----------------------------------------------------------------------------
// CTestStdio::TestFileTellSeekCornerCase1
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileTellSeekCornerCase1()
    {
    FILE *fp = NULL, *fpout = NULL;
    size_t count = 0;
    size_t ret = 0;
    char *ptr_511_1 = "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss\ns\nsanjay\n\n";

    char *p = NULL;
    int length = strlen(ptr_511_1); // ptr_511_2
    set_fmode('t');
    if (get_fmode() != 't')
    	{
        ERR_PRINTF1(_L("Failed in getting the file mode"));
        return TestStepResult();  	
    	}
    fp = fopen("c:\\sample.txt", "w"); // rt,rt+,r+t,wt,wt+,w+t,at,at+,a+t // r,w,a,r+,w+,a+
    if (fp == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    //(void) setvbuf(fp, NULL, _IOLBF, 0); // _IOFBF, _IOLBF, _IONBF
    // 1st write
    count = fwrite(ptr_511_1, 1, 511,fp); // ptr_511_2
    if(count != 511)
        {
        ERR_PRINTF1(_L("Failed during 1st write "));
        }
    int pos = ftell(fp);                                // 512
    INFO_PRINTF2(_L("ftell- After 1st write: %d"), pos);
    
    // 2nd write
    count += fwrite(ptr_511_1+511, 1, 8,fp); // ptr_511_2
    if(count != strlen(ptr_511_1))           // ptr_511_2
        {
        ERR_PRINTF1(_L("Failed during 1st write "));           // 522
        }
    pos = ftell(fp);
    INFO_PRINTF2(_L("ftell- After 2nd write: %d"), pos);

    //INFO_PRINTF1(_L("Wrote %lu bytes"), (unsigned long)count);
    fclose(fp);
    fp = NULL;
    
    // reading from a file
    fp = fopen("c:\\sample.txt", "r");
    if (fp == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }     
    
    // Open another file for writing
    fpout = fopen("c:\\output.txt", "w"); // w+
    if (fpout == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    //p = (char *)malloc(count+ total_count+1); // extra one is for holding '\0'
    p = (char *)malloc(count+1); // extra one is for holding '\0'
    
    //ret = fread(p, 1, count + total_count, fp);
    // 1st read
    ret = fread(p, 1, 511, fp);
    if(ret != 511)
        {
        ERR_PRINTF1(_L(" 1st fread() failed "));
        }
    pos = ftell(fp);                                // 512
    INFO_PRINTF2(_L("ftell- After 1st read: %d"), pos);
    
    // 2nd read
    ret += fread(p+511, 1, 8, fp);
    if(ret != strlen(ptr_511_1))                    // ptr_511_2
        {
        ERR_PRINTF1(_L(" 2nd fread() failed "));
        }
    else
    {
    pos = ftell(fp);                                // 522
    INFO_PRINTF2(_L("ftell- After 2nd read: %d"), pos);

    INFO_PRINTF1(_L(" fread() also passed "));
    p[count] = '\0';
    // write back the read contents into the output.txt file
    count = fwrite(p, 1, strlen(p),fpout);
    //ERR_PRINTF1(_L("Length of p: %d"), length);
    if(count != strlen(p))
        {
        ERR_PRINTF1(_L("Writing onto another file failed"));
        }
    else
        {
        INFO_PRINTF1(_L("Writing onto another file passed"));
        }
    }
    free((void*)p);
    fclose(fpout);
    fclose(fp);
    SetTestStepResult(EPass);
    return TestStepResult();    
    
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileTellSeekCornerCase2
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileTellSeekCornerCase2()
    {
    FILE *fp = NULL, *fpout = NULL;
    size_t count = 0;
    size_t ret = 0;
   char *ptr_511_1 = "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss\ns\n";
    
    char *p = NULL;
    int length = strlen(ptr_511_1);
    set_fmode('t');
    if (get_fmode() != 't')
    	{
        ERR_PRINTF1(_L("Failed in getting the file mode"));
        return TestStepResult();  	
    	}
    fp = fopen("c:\\sample.txt", "w"); // rt,rt+,r+t,wt,wt+,w+t,at,at+,a+t // r,w,a,r+,w+,a+
    if (fp == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    //(void) setvbuf(fp, NULL, _IOLBF, 0); // _IOFBF, _IOLBF, _IONBF
    count = fwrite(ptr_511_1, 1, strlen(ptr_511_1),fp);
    if(count != strlen(ptr_511_1))
        {
        ERR_PRINTF1(_L("Failed during 1st write "));
        }
    int pos = ftell(fp);                                // 513
    INFO_PRINTF2(_L("ftell- After 1st write: %d"), pos);

    //ERR_PRINTF1(_L("Wrote %lu bytes"), (unsigned long)count);
    fclose(fp);
    fp = NULL;
    
    // reading from a file
    fp = fopen("c:\\sample.txt", "r");
    if (fp == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }         
    
    // Open another file for writing
    fpout = fopen("c:\\output.txt", "w"); // w+
    if (fpout == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }
    
    //p = (char *)malloc(count+ total_count+1); // extra one is for holding '\0'
    p = (char *)malloc(count+1); // extra one is for holding '\0'
    
    //ret = fread(p, 1, count + total_count, fp);
    ret = fread(p, 1, count, fp);
    if(ret != count)
        {
        ERR_PRINTF1(_L(" 1st fread() failed "));
        }
    else
    {
    pos = ftell(fp);                                // 513
    INFO_PRINTF2(_L("ftell- After 1st read: %d"), pos);

    INFO_PRINTF1(_L(" fread() also passed "));
    p[count] = '\0';
    // write back the read contents into the output.txt file
    count = fwrite(p, 1, strlen(p),fpout);
    //ERR_PRINTF1(_L("Length of p: %d"), length);
    if(count != strlen(p))
        {
        ERR_PRINTF1(_L("Writing onto another file failed"));
        }
    else
        {
        INFO_PRINTF1(_L("Writing onto another file passed"));
        }
    }
    free((void*)p);
    fclose(fpout);
    fclose(fp);
    SetTestStepResult(EPass);
    return TestStepResult();  
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileSeekTellOnRW1
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileSeekTellOnRW1()
    {
    char *data = "helloworld\nfine\n\n"; // -->> "helloworld\r\nfine\r\n\r\n"
    int ret = 0;
    FILE *fw = NULL, *fr = NULL;
    
    set_fmode('t');
    
    // 1st part 
    fw = fopen("c:\\temp_tc.txt", "w+");   
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }    
    int count = fwrite(data, 1, 11, fw);
    if(count != 11)
        {
        ERR_PRINTF1(_L("1st fwrite() failed"));
        fclose(fw);
        return TestStepResult(); 
        }
    int pos = ftell(fw);                                   // 12 
    INFO_PRINTF2(_L(" ftell - After 1st write is: %d"), pos);
    count += fwrite(data+11, 1, 6, fw);
    if(count != strlen(data))
        {
        ERR_PRINTF1(_L(" 2nd fwrite() failed"));
        fclose(fw);
        return TestStepResult(); 
        }
    pos = ftell(fw);                                        // 20  
    INFO_PRINTF2(_L(" ftell - After 1st write is: %d"), pos);
    fclose(fw);
    
    // 2nd part
    fw = fopen("c:\\temp_tc_out.txt", "w+");   
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }    
    fr = fopen("c:\\temp_tc.txt", "r+");   
    if (fr == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        fclose(fw);
        return TestStepResult();
        }    

    char p[50];
    char *retn = fgets(p, count, fr);
    if(strlen(p) != 11)
        {
        ERR_PRINTF1(_L("1st read failed"));
        CleanupOnfiles(fw,fr);
        return TestStepResult(); 
        }
    pos = ftell(fr);
    INFO_PRINTF2(_L("ftell - after reading: %d"), pos);
    fseek(fr,-pos,SEEK_END);         
    
    ret = fread(p+8,1, 9,fr);
    if( ret != 9)
        {
        ERR_PRINTF2(_L("Return value from fread() is: %d"), ret);
        ERR_PRINTF1(_L("Failed to read using fread()"));
        CleanupOnfiles(fw,fr);
        return TestStepResult(); 
        }
    p[count] = '\0';
    pos = ftell(fr);
    INFO_PRINTF2(_L(" ftell - after 2nd read is: %d"), pos);
    
    int count2 = fwrite(data, 1, 11, fr);
    if( count2 != 11)
        {
        ERR_PRINTF3(_L("value of count2 and errno are: %d and %d "), count2, errno);
        ERR_PRINTF1(_L("Failed to write when opened in r+ mode"));
        CleanupOnfiles(fw,fr);
        return TestStepResult(); 
        }
    pos = ftell(fr);
    INFO_PRINTF2(_L(" ftell - after 2nd read is: %d"), pos);
    
    // WRITING AS PART OF "r+"
    count = fwrite(p, 1, strlen(p), fw);
    if(count != strlen(p))
        {
        ERR_PRINTF1(_L(" Failed to write onto another file"));
        CleanupOnfiles(fw,fr);
        return TestStepResult(); 
        }
    INFO_PRINTF1(_L("Passed to write onto another file"));

    CleanupOnfiles(fw,fr);
    SetTestStepResult(EPass);
    return TestStepResult();  
    
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileSeekTellOnRW2
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileSeekTellOnRW2()
    {

    char *data = "helloworld\nfine\n\n"; // -->> "helloworld\r\nfine\r\n\r"),
    int ret = 0;
    FILE *fw = NULL, *fr = NULL;
    set_fmode('t');
    // 1st part
    fw = fopen("c:\\temp_tc.txt", "w+");    
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }    
    int count = fwrite(data, 1, 11, fw);
    if(count != 11)
        {
        ERR_PRINTF1(_L("1st fwrite() failed"));
        CleanupOnfiles(fw,fr);
        return TestStepResult(); 
        }
    int pos = ftell(fw);                                   // 12
    INFO_PRINTF2(_L(" ftell - After 1st write is: %d"), pos);
    count += fwrite(data+11, 1, 6, fw);
    if(count != strlen(data))
        {
        ERR_PRINTF1(_L(" 2nd fwrite() failed"));
        CleanupOnfiles(fw,fr);
        return TestStepResult(); 
        }
    pos = ftell(fw);                                        // 20
    INFO_PRINTF2(_L(" ftell - After 1st write is: %d"), pos);
    fclose(fw);
    
    // 2nd part
    fw = fopen("c:\\temp_tc_out.txt", "w+");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }    
    fr = fopen("c:\\temp_tc.txt", "r+");
    if (fr == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        fclose(fw);
        return TestStepResult();
        }    
    char p[50];
    ret = fread(p, 1, count, fr);
    if(ret != count)
        {
        ERR_PRINTF1(_L("Read failed"));
        CleanupOnfiles(fw,fr);
        return TestStepResult(); 
        }
    pos = ftell(fr);                                       // 20
    INFO_PRINTF2(_L("ftell - After reading: %d"), pos);
    p[count] = '\0';

    // WRITING AS PART OF "r+"
    int count2 = fwrite(data, 1, 11, fr);
    if( count2 != 11)
        {
        ERR_PRINTF3(_L("value of count2 and errno are: %d and %d "), count2, errno);
        ERR_PRINTF1(_L("Failed to write when opened in r+ mode"));
        CleanupOnfiles(fw,fr);
        return TestStepResult(); 
        }
    pos = ftell(fr);
    INFO_PRINTF2(_L(" ftell - after 2nd read is: %d"), pos);

    count = fwrite(p, 1, strlen(p), fw);
    if(count != strlen(p))
        {
        ERR_PRINTF1(_L(" Failed to write onto another file"));
        CleanupOnfiles(fw,fr);
        return TestStepResult(); 
        }
    INFO_PRINTF1(_L("Passed to write onto another file"));

    CleanupOnfiles(fw,fr);
    SetTestStepResult(EPass);
    return TestStepResult(); 
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileSeekTellOnRW3
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileSeekTellOnRW3()
    {
    char *data = "helloworld\nfine\n\n"; // -->> "helloworld\r\nfine\r\n\r"),
    int ret = 0;
    FILE *fw = NULL, *fr = NULL;

    set_fmode('t');

    fw = fopen("c:\\temp_tc.txt", "w+");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }  
    fclose(fw);
    fr = fopen("c:\\temp_tc.txt", "r+"); // "rb+", Note: Given File should be already existed,bcoz // fopen will not create new file when mode is "r+"
    if (fr == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        fclose(fw);
        return TestStepResult();
        }      
    fw = fopen("c:\\temp_tc_out.txt", "w+");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        fclose(fr);
        return TestStepResult();
        }    
    
    int count = fwrite(data, 1, 11, fr);
    if(count != 11)
        {
        ERR_PRINTF3(_L(" Value returned from 1st fwrite() and errno in r+ mode are: %d\n %d"),count, errno);
        ERR_PRINTF1(_L("1st fwrite() failed"));
        CleanupOnfiles(fw,fr);
        return TestStepResult(); 
        }
    int pos = ftell(fr);                                   // 12
    INFO_PRINTF2(_L(" ftell - After 1st write is: %d"), pos);
    count += fwrite(data+11, 1, 6, fr);
    if(count != strlen(data))
        {
        ERR_PRINTF3(_L(" Value returned from 2nd fwrite() and errno in r+ mode are: %d\n %d"),count, errno);
        ERR_PRINTF1(_L(" 2nd fwrite() failed"));
        CleanupOnfiles(fw,fr);
        return TestStepResult(); 
        }
    pos = ftell(fr);                                        // 20 
    INFO_PRINTF2(_L(" ftell - After 2nd fwrite is: %d"), pos);

    fseek(fr,0,SEEK_SET);
    pos = ftell(fr);                                        // 0
    INFO_PRINTF2(_L(" ftell - After fseek() to beginning is: %d"), pos);

    char p[50];
    ret = fread(p,1, count, fr);
    if(ret != count)
        {
        ERR_PRINTF1(_L(" Chance given to Fread after fwrite has failed as part of r+ "));
        ERR_PRINTF3(_L(" Return value and errno are: %d and %d"), ret, errno);
        CleanupOnfiles(fw,fr);
        return TestStepResult(); 
        }

    pos = ftell(fr);                                        // 20
    INFO_PRINTF2(_L(" ftell - After fread() is: %d"), pos);
    p[count] = '\0';
    
    // Writing onto another file
    ret = fwrite(p, 1, strlen(p),fw);
    if(ret != count)
        {
        ERR_PRINTF1(_L("Failed to write onto another file"));
        CleanupOnfiles(fw,fr);
        return TestStepResult(); 
        }
    INFO_PRINTF1(_L("Passed to write onto another file"));

    CleanupOnfiles(fw,fr);
    SetTestStepResult(EPass);
    return TestStepResult();      
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileSeekTellOnRW4
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileSeekTellOnRW4()
    {
    char *data = "helloworld\nfine\n\n"; // -->> "helloworld\r\nfine\r\n\r\n"
    int ret = 0;
    FILE *fw = NULL, *fr = NULL;

    set_fmode('t');

    // 1st part
    fw = fopen("c:\\temp_tc.txt", "w+");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }    
    int count = fwrite(data, 1, strlen(data), fw);
    if(count != strlen(data))
        {
        ERR_PRINTF1(_L("1st fwrite() failed"));
        return TestStepResult(); 
        }
    int pos = ftell(fw);                                   // 20
    INFO_PRINTF2(_L(" ftell - After 1st write is: %d"), pos);
    
    fclose(fw);

    // 2nd part
    fw = fopen("c:\\temp_tc_out.txt", "w+");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }    
    fr = fopen("c:\\temp_tc.txt", "w+");
    if (fr == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        fclose(fw);
        return TestStepResult();
        }    
    char p[50];
    
    ret = fread(p, 1, count, fr);
    if(ret != 0)
        {
        ERR_PRINTF3(_L("return value from fread() and errno are: %d and %d"), ret,errno);
        ERR_PRINTF1(_L("Read failed"));
        CleanupOnfiles(fw,fr);
        return TestStepResult(); 
        }
    pos = ftell(fr);
    INFO_PRINTF2(_L("ftell - after Read using fread(): %d"), pos);
    
    p[ret] = '\0';


    // WRITING AS PART OF "w+"
    int count2 = fwrite(data, 1, 11, fr);
    if( count2 != 11)
        {
        ERR_PRINTF3(_L("value of FWRITE and errno in w+ are: %d and %d "), count2, errno);
        ERR_PRINTF1(_L("Failed to write when opened in r+ mode"));
        CleanupOnfiles(fw,fr);
        return TestStepResult(); 
        }
    pos = ftell(fr);
    INFO_PRINTF2(_L(" ftell - after writing as part of w+ is: %d"), pos);

    ret = fwrite(p, 1, strlen(p), fw);
    if(ret != 0)
        {
        ERR_PRINTF1(_L(" Failed to write onto another file"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();     	                 
        }
    INFO_PRINTF1(_L("Passed to write onto another file"));

    CleanupOnfiles(fw,fr);
    SetTestStepResult(EPass);
    return TestStepResult();     
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileWriteTextMode
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileWriteTextMode()
    {
    char *data = "helloworld\nfine\n\n"; // -->> "helloworld\r\nfine\r\n\r"),
    char *replace = "sanj\n\n";
    int ret = 0, readCount = 0;
    FILE *fw, *fr;

    set_fmode('t');
    // 1st part
    fw = fopen("c:\\temp_tc.txt", "w+");
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }    
    fr = fopen("c:\\temp_tc_out.txt", "w+");
    if (fr == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        fclose(fw);
        return TestStepResult();
        }

    // 1st write
    int count = fwrite(data, 1, strlen(data), fw);
    if(count != strlen(data))
        {
        ERR_PRINTF1(_L("1st fwrite() failed"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();     	                
        }
    int pos = ftell(fw);                                            // 20
    INFO_PRINTF2(_L(" ftell - After 1st write is: %d"), pos);
    char p[50];
    
    // seek to the beginning of the file and Read 11-bytes
    fseek(fw, 0, SEEK_SET);
    pos = ftell(fw);                                                // 0
    INFO_PRINTF2(_L("ftell - after Seeking to the beginning of the file: %d"), pos);
    

    // 1st read
    readCount = fread(p, 1, 11, fw);
    if(readCount != 11)
        {
        ERR_PRINTF3(_L("return value from 1st fread() and errno are: %d and %d"), ret,errno);
        ERR_PRINTF1(_L("1st Read failed"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();     	                        
        }
    int posRetain = ftell(fw);                                                // 12
    INFO_PRINTF2(_L("ftell - after 1st Read using fread(): %d"), posRetain);
    
    
    // 2nd write
    count = fwrite(replace, 1, strlen(replace), fw);
    if(count != strlen(replace))
        {
        ERR_PRINTF1(_L("2nd fwrite() failed"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();     	                        
        }
    pos = ftell(fw);                                            // 20 
    INFO_PRINTF2(_L(" ftell - After 2nd write is: %d"), pos);
    
    
    // Seek back to posRetain from beginning so as to read from there.
    fseek(fw,posRetain, SEEK_SET);
    pos = ftell(fw);                                                // 12 
    INFO_PRINTF2(_L("ftell - after seeking back to posRetain from beginning of file: %d"), pos);
    

    // 2nd read
    readCount += fread(p+11, 1, 6, fw);
    if(readCount != (strlen(data)))
        {
        ERR_PRINTF3(_L("return value from 1st fread() and errno are: %d and %d"), ret,errno);
        ERR_PRINTF1(_L("Read failed"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();     	                        
        }
    pos = ftell(fw);                                                // 20 
    INFO_PRINTF2(_L("ftell - after 1st Read using fread(): %d"), pos);
    p[readCount] = '\0';

    // Finally writing onto another file
    ret = fwrite(p, 1, strlen(p), fr);
    if(ret != strlen(p))
        {
        ERR_PRINTF1(_L(" Failed to write onto another file"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();     	                
        }
    INFO_PRINTF1(_L("Passed to write onto another file"));
    
    CleanupOnfiles(fw,fr);   	        
    SetTestStepResult(EPass);
    return TestStepResult();     
    }

// -----------------------------------------------------------------------------
// CTestStdio::TestFileWriteBinaryMode
// 
// API tested: set_fmode(),fwrite(), fread()
// Description: Test case to check the file opening mode
// -----------------------------------------------------------------------------

TVerdict CTestStdio::TestFileWriteBinaryMode()
    {
    char *data = "helloworld\nfine\n\n"; // -->> "helloworld\r\nfine\r\n\r"),
    char *replace = "sanj\n\n";
    int ret = 0, readCount = 0;
    FILE *fw, *fr;

    set_fmode('t');

    // 1st part
    fw = fopen("c:\\temp_tc.txt", "wb+");     // binary-mode
    if (fw == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        return TestStepResult();
        }    
    fr = fopen("c:\\temp_tc_out.txt", "wb+"); // binary-mode
    if (fr == NULL)
        {
        ERR_PRINTF2(_L("Failed to open file and errno is %d"),errno);
        fclose(fw);
        return TestStepResult();
        }

    // 1st write
    int count = fwrite(data, 1, strlen(data), fw);
    if(count != strlen(data))
        {
        ERR_PRINTF1(_L("1st fwrite() failed"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();     	        
        }
    int pos = ftell(fw);                                            // 17
    INFO_PRINTF2(_L(" ftell - After 1st write is: %d"), pos);
    char p[50];
    // seek to the beginning of the file and Read 11-bytes
    fseek(fw, 0, SEEK_SET);
    pos = ftell(fw);                                                // 0
    INFO_PRINTF2(_L("ftell - after Seeking to the beginning of the file: %d"), pos);
    

    // 1st read
    readCount = fread(p, 1, 11, fw);
    if(readCount != 11)
        {
        ERR_PRINTF3(_L("return value from 1st fread() and errno are: %d and %d"), ret,errno);
        ERR_PRINTF1(_L("1st Read failed"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();     	        
        }
    int posRetain = ftell(fw);                                                // 11
    INFO_PRINTF2(_L("ftell - after 1st Read using fread(): %d"), posRetain);
    
    
    // 2nd write
    count = fwrite(replace, 1, strlen(replace), fw);
    if(count != strlen(replace))
        {
        ERR_PRINTF1(_L("2nd fwrite() failed"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();     	        
        }
    pos = ftell(fw);                                            // 17 
    INFO_PRINTF2(_L(" ftell - After 2nd write is: %d"), pos);
    
    
    // Seek back to posRetain from beginning so as to read from there.
    fseek(fw,posRetain, SEEK_SET);
    pos = ftell(fw);                                                // 11
    INFO_PRINTF2(_L("ftell - after seeking back to posRetain from beginning of file: %d"), pos);
    
    // 2nd read
    readCount += fread(p+11, 1, 6, fw);
    if(readCount != (strlen(data)))
        {
        ERR_PRINTF3(_L("return value from 1st fread() and errno are: %d and %d"), ret,errno);
        ERR_PRINTF1(_L("Read failed"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();     	        
        }
    pos = ftell(fw);                                                // 17
    INFO_PRINTF2(_L("ftell - after 1st Read using fread(): %d"), pos);
    p[readCount] = '\0';

    // Finally writing onto another file
    ret = fwrite(p, 1, strlen(p), fr);
    if(ret != strlen(p))
        {
        ERR_PRINTF1(_L(" Failed to write onto another file"));
        CleanupOnfiles(fw,fr);
    	return TestStepResult();     	        
        }
    INFO_PRINTF1(_L("Passed to write onto another file"));

    CleanupOnfiles(fw,fr);
    SetTestStepResult(EPass);
    return TestStepResult();     
    }

//End of file
