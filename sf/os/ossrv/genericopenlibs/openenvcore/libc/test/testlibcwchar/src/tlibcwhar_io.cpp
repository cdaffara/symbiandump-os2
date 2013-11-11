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



#include "tlibcwchar.h"
 


TInt CTestLibcwchar::TestfgetwcL()
	{
	INFO_PRINTF1(_L("Libcwchar Test Getting (test migrated from STIFF)"));	
		
	int retval=0;		
	FILE *fp;
	
	int iFCreateRetVal;
	unsigned int wchar_letters[20] = {0x0905,0x0906,0x0907,0x0000,0x0909,0x090a,0x090f,0x090d,0x090e,0x090b};	
	int iTestResult;
 	int  counter=0;
	char *filename = "c:\\FgetwcInputFile.txt";

	iTestResult = KErrNone;

	iFCreateRetVal = CreateTestDataFile(filename,wchar_letters,10);
	if(iFCreateRetVal != 0)
		{
	    INFO_PRINTF2(_L("Error : Input File creation... fgetwc Failed !! %d"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... fgetwc Failed !!"), errno);
		
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		
		}
	counter = 1;
	while((int)(retval = fgetwc(fp) )!= WEOF)
		{
		if(((int)retval == WEOF) && (counter <= 10))
			{	
			iTestResult = KErrGeneral;
			break;
			}
		counter++;
		}
	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("fgetwc Failed !!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("fgetwc Passed!!"), errno);
		}

	fclose(fp);
	
	
    return iTestResult;


	}
		
//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestOnefgetwsL
//API Tested			: fgetws
//TestCase Description	: This test case will open a unicode file and will read
//fixed number of the characters from the file and displays it on the console.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestOnefgetwsL()
	{
    FILE *fp;
	wchar_t buf[20];
	unsigned int wchar_letters[20] = {0x0905,0x0906,0x0907,0x0000,0x0909,0x090a,0x090f,0x090d,0x090e,0x090b};	
	int iFCreateRetVal;
	int iTestResult;
	char *filename = "c:\\FgetwsInputFile.txt";
		
	iTestResult = KErrNone;
	iFCreateRetVal = CreateTestDataFile(filename,wchar_letters,10);
	if(iFCreateRetVal != 0)
		{
	    INFO_PRINTF2(_L("Error : Input File creation... fgetws:1 Failed!!"), errno);
		iTestResult = KErrGeneral;
		return iTestResult;
		}

    fp = fopen(filename,"r");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... fgetws:1 Failed!!"), errno);
		iTestResult = KErrGeneral;
		return iTestResult;
		}
	if(fgetws(buf,18,fp) == NULL)
		{
		iTestResult = KErrGeneral;
		}

	buf[0] = L'\0';
	if(fgetws(buf,1,fp) != NULL)
		{
		iTestResult = KErrGeneral;
		}
	
	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("fgetws:1 Failed!!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("fgetws:1 Passed!!"), errno);
		}
	fclose(fp);
	
	return iTestResult;
	}


//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestTwofgetwsL
//API Tested			: fgetws
//TestCase Description	: This test case will open a unicode file and will read
//strings where the length is less than, equal to and greater than the number 
//of characters in the file.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestTwofgetwsL()
	{
    FILE *fp;
	wchar_t buf[30];
	unsigned int wchar_letters[20] = {0x0905,0x0906,0x0907,0x0000,0x0909,0x090a,0x090f,0x090d,0x090e,0x090b};	
	int iFCreateRetVal;
	int iTestResult;
	char *filename = "c:\\FgetwsInputFile.txt";

	
	iTestResult = KErrNone;
	iFCreateRetVal = CreateTestDataFile(filename,wchar_letters,10);
	if(iFCreateRetVal != 0)
		{
		INFO_PRINTF2(_L("Error : Input File creation... fgetws:2  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF2(_L("Error : Error : File open... fgetws:2  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
		
	    return iTestResult;
		}

	if(fgetws(buf,5,fp) == NULL)
		{
		iTestResult = KErrGeneral;
		}
	
	rewind(fp);
	if(fgetws(buf,10,fp) == NULL)
		{
		iTestResult = KErrGeneral;
		}
	
	rewind(fp);
	if(fgetws(buf,15,fp) == NULL)
		{
		iTestResult = KErrGeneral;
		}

	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("fgetws:2 Failed!!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("fgetws:2 Passed!!"), errno);
		}
	fclose(fp);
	
    return iTestResult;
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestThreefgetwsL
//API Tested			: fgetws
//TestCase Description	: This test case will open a unicode file and will read
//fixed number of the characters from the file with a newline character in the
//middle. It displays the characters read from the file on the console.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestThreefgetwsL()
	{
    FILE *fp;
	wchar_t buf[20];
	unsigned int wchar_letters[20] = {0x0905,0x0906,0x0907,0x0000,0x0909,0x000a,0x090f,0x090d,0x090e,0x090b};	
	int iFCreateRetVal;
	int iTestResult;
	char *filename = "c:\\FgetwsInputFile.txt";
	
	iTestResult = KErrNone;
	iFCreateRetVal = CreateTestDataFile(filename ,wchar_letters,10);
	if(iFCreateRetVal != 0)
		{
		INFO_PRINTF2(_L("Error : Input File creation... fgetws:3  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF2(_L("Error : File open... fgetws:3  Failed!!"), errno);
		iTestResult = KErrGeneral;
	    return iTestResult;
		}

	if(fgetws(buf,10,fp) == NULL)
		{
		iTestResult = KErrGeneral;
		}

	if(iTestResult == KErrGeneral)
		{
	    INFO_PRINTF2(_L("fgetws:3 Failed!!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("fgetws:3 Passed!!"), errno);
		}

	fclose(fp);
	
	return iTestResult;
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestgetwcL
//API Tested			: getwc
//TestCase Description	: This test case will open a unicode file and will read
//	the characters from the file till EOF.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestgetwcL()
	{
    FILE *fp;
	wint_t retval;
	int iFCreateRetVal;
	unsigned int wchar_letters[20] = {0x0905,0x0906,0x0907,0x0000,0x0909,0x090a,0x090f,0x090d,0x090e,0x090b};	
	int iTestResult;
	int counter;
	char *filename = "c:\\GetwcInputFile.txt";
	
	iTestResult = KErrNone;
	iFCreateRetVal = CreateTestDataFile(filename,wchar_letters,10);
	if(iFCreateRetVal != 0)
		{
		INFO_PRINTF2(_L("Error : Input File creation... getwc  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}
	
	fp = fopen(filename,"r");
	if(fp == NULL)
		{	
		INFO_PRINTF2(_L("Error : File open... getwsc  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
		return iTestResult;
		}

	counter = 1;

	while((int)(retval = getwc(fp) )!= WEOF)
		{
		if(((int)retval == WEOF) && (counter <= 10))
			{
			iTestResult = KErrGeneral;
			break;
			}
		counter++;
		}

	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("getwc Failed!!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("getwc Passed!!"), errno);
		}

	fclose(fp);
	
	
    return iTestResult;
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestOnefwideL
//API Tested			: fwide
//TestCase Description	: This test case will first read the orientation of the
//and then set the orientation of the stream to wide char and once again read 
//back the orientation that was set.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestOnefwideL()
	{
    FILE *fp;
	int retval;
	int iFCreateRetVal;
	unsigned int wchar_letters[20] = {0x0905,0x0906,0x0907};	
	int iTestResult;
	char *filename = "c:\\FwideTestInputFile.txt";
	
	iTestResult = KErrNone;
	iFCreateRetVal = CreateTestDataFile(filename,wchar_letters,3);
	if(iFCreateRetVal != 0)
		{
		INFO_PRINTF2(_L("Error : Input File creation... fwide   Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF2(_L("Error : File open... fwide:1  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
		
	    return iTestResult;
		}

	retval = fwide(fp,0);
	if(retval == 0)
		{
		INFO_PRINTF1(_L("No Orientation is set\n"));
		}
	if(retval < 0)
		{
		INFO_PRINTF1(_L("Stream is set to Byte Orientation\n"));
		}
	if(retval > 0)
		{
		INFO_PRINTF1(_L("Stream is set to Wide Orientation\n"));
		}

	retval = fwide(fp,1);

	if(retval == 0)
		{
		INFO_PRINTF1(_L("No Orientation is set\n"));
		}
	if(retval < 0)
		{
		INFO_PRINTF1(_L("Stream is set to Byte Orientation\n"));
		}
	if(retval > 0)
		{
		INFO_PRINTF1(_L("Stream is set to Wide Orientation\n"));
		}

	retval = fwide(fp,0);

	if(retval == 0)
		{
		INFO_PRINTF1(_L("No Orientation is set\n"));
		}
	if(retval < 0)
		{
		INFO_PRINTF1(_L("Stream is set to Byte Orientation\n"));
		}
	if(retval > 0)
		{
		INFO_PRINTF1(_L("Stream is set to Wide Orientation\n"));
		}
		
	INFO_PRINTF1(_L("fwide:1 Passed !!"));
	fclose(fp);
	
	
	return iTestResult;
    
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestTwofwideL
//API Tested			: fwide
//TestCase Description	: This test case will first read the orientation of the
//and then set the orientation of the stream to byte and once again read 
//back the orientation that was set.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestTwofwideL()
	{
    FILE *fp;
	int retval;
	int iFCreateRetVal;
	unsigned int wchar_letters[20] = {0x0905,0x0906,0x0907};	
	int iTestResult;
	char *filename = "c:\\FwideTestInputFile.txt";
	
	iTestResult = KErrNone;
	iFCreateRetVal = CreateTestDataFile(filename,wchar_letters,3);
	if(iFCreateRetVal != 0)
		{
	    INFO_PRINTF2(_L("Error : Input File creation... fwide:2  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF2(_L("Error : File open... fwide:2  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
		return iTestResult;
		}

	retval = fwide(fp,0);
	if(retval == 0)
		{
		INFO_PRINTF1(_L("No Orientation is set\n"));
		}
	if(retval < 0)
		{
		INFO_PRINTF1(_L("Stream is set to Byte Orientation\n"));
		}
	if(retval > 0)
		{
		INFO_PRINTF1(_L("Stream is set to Wide Orientation\n"));
		}

	retval = fwide(fp,-1);
	if(retval == 0)
		{
		INFO_PRINTF1(_L("No Orientation is set\n"));
		}
	if(retval < 0)
		{
		INFO_PRINTF1(_L("Stream is set to Byte Orientation\n"));
		}
	if(retval > 0)
		{
		INFO_PRINTF1(_L("Stream is set to Wide Orientation\n"));
		}

	retval = fwide(fp,0);
	if(retval == 0)
		{
		INFO_PRINTF1(_L("No Orientation is set\n"));
		}
	if(retval < 0)
		{
		INFO_PRINTF1(_L("Stream is set to Byte Orientation\n"));
		}
	if(retval > 0)
		{
		INFO_PRINTF1(_L("Stream is set to Wide Orientation\n"));
		}
	
	INFO_PRINTF1(_L("fwide:2 Passed !!"));
	fclose(fp);
	
	
	return iTestResult;
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestThreefwideL
//API Tested			: fwide
//TestCase Description	: This test case will first read the orientation of the
//and then set the orientation of the stream to byte and then tries to change
//the orientation to wide.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestThreefwideL()
	{
    FILE *fp;
	int retval;
	int iFCreateRetVal;
	unsigned int wchar_letters[20] = {0x0905,0x0906,0x0907};	
	int iTestResult;
	char *filename = "c:\\FwideTestInputFile.txt";
	
	iTestResult = KErrNone;
	iFCreateRetVal = CreateTestDataFile(filename,wchar_letters,3);
	if(iFCreateRetVal != 0)
		{
		INFO_PRINTF2(_L("Error : Input File creation... fwide:3  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF2(_L("Error : File open... fwide:3  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
		return iTestResult;
		}

	retval = fwide(fp,0);
	if(retval == 0)
		{
		INFO_PRINTF1(_L("No Orientation is set\n"));
		}
	if(retval < 0)
		{
		INFO_PRINTF1(_L("Stream is set to Byte Orientation\n"));
		}
	if(retval > 0)
		{
		INFO_PRINTF1(_L("Stream is set to Wide Orientation\n"));
		}

	retval = fwide(fp,-1);

	if(retval == 0)
		{
		INFO_PRINTF1(_L("No Orientation is set\n"));
		}
	if(retval < 0)
		{
		INFO_PRINTF1(_L("Stream is set to Byte Orientation\n"));
		}
	if(retval > 0)
		{
		INFO_PRINTF1(_L("Stream is set to Wide Orientation\n"));
		}

	retval = fwide(fp,1);
	if(retval == 0)
		{
		INFO_PRINTF1(_L("No Orientation is set\n"));
		}
	if(retval < 0)
		{
		INFO_PRINTF1(_L("Stream is set to Byte Orientation\n"));
		}
	if(retval > 0)
		{
		INFO_PRINTF1(_L("Stream is set to Wide Orientation\n"));
		}
	
	INFO_PRINTF1(_L("fwide:3 Passed !!"));
	fclose(fp);
	
	
	return iTestResult;
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestFourfwideL
//API Tested			: fwide
//TestCase Description	: This test case will first read the orientation of the
//and then set the orientation of the stream to wide and then tries to change
//the orientation to byte.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestFourfwideL()
    {
    FILE *fp;
	int retval;
	int iFCreateRetVal;
	unsigned int wchar_letters[20] = {0x0905,0x0906,0x0907};	
	int iTestResult;
	char *filename = "c:\\FwideTestInputFile.txt";

	iTestResult = KErrNone;
	iFCreateRetVal = CreateTestDataFile(filename,wchar_letters,3);
	if(iFCreateRetVal != 0)
		{
		INFO_PRINTF2(_L("Error : Input File creation... fwide:4  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF2(_L("Error : File open... fwide:4  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
		return iTestResult;
		}

	retval = fwide(fp,0);
	if(retval == 0)
		{
		INFO_PRINTF1(_L("No Orientation is set\n"));
		}
	if(retval < 0)
		{
		INFO_PRINTF1(_L("Stream is set to Byte Orientation\n"));
		}
	if(retval > 0)
		{
		INFO_PRINTF1(_L("Stream is set to Wide Orientation\n"));
		}

	retval = fwide(fp,1);
	if(retval == 0)
		{
		INFO_PRINTF1(_L("No Orientation is set\n"));
		}
	if(retval < 0)
		{
		INFO_PRINTF1(_L("Stream is set to Byte Orientation\n"));
		}
	if(retval > 0)
		{
		INFO_PRINTF1(_L("Stream is set to Wide Orientation\n"));
		}

	retval = fwide(fp,-1);

	if(retval == 0)
		{
		INFO_PRINTF1(_L("No Orientation is set\n"));
		}
	if(retval < 0)
		{
		INFO_PRINTF1(_L("Stream is set to Byte Orientation\n"));
		}
	if(retval > 0)
		{
		INFO_PRINTF1(_L("Stream is set to Wide Orientation\n"));
		}
	
	INFO_PRINTF1(_L("fwide:4 Passed !!"));
	fclose(fp);
	
	
	return iTestResult;
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestOnefputwcL
//API Tested			: fputwc
//TestCase Description	: This test case writes a wchar to the stream and 
//closes the file. The file is again opened and the character that was written
//by fputwc is read back and compared.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestOnefputwcL()
	{
    FILE *fp;
	wint_t rretval,wretval;
	int iTestResult;
	char *filename = "c:\\FputwcTestInputFile.txt";

	iTestResult = KErrNone;
	fp = fopen(filename,"w");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... fputwc:1  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	wretval = fputwc(wchar_t(0x0905),fp);
	if((signed int)wretval == WEOF)
		{
	    INFO_PRINTF2(_L("fputwc:1 Failed!!"), errno);
		iTestResult = KErrGeneral;
		fclose(fp);
		
		return iTestResult;    
		}

	fclose(fp);

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... fputwc:1  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
		return iTestResult;
		}

	rretval = fgetwc(fp);
	if(wretval != rretval)
		{
		iTestResult = KErrGeneral;
		}
	else
		{
		iTestResult = KErrNone;
		}

	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("fputwc:1 Failed!!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("fputwc:1 Passed !!"), errno);
		}

	fclose(fp);
	
	
	return iTestResult;    
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestTwofputwcL
//API Tested			: fputwc
//TestCase Description	: This test case writes a set of wide characters to 
//the stream and closes the file. The file is again opened and the characters
//that were written is read back and compared.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestTwofputwcL()
	{
    FILE *fp;
	wint_t rretval,wretval;
	wchar_t wchar_letters[20] = {0x0905,0x0906,0x0907,0x0909,0x090a,0x090f,0x090d,0x090e,0x090b,0x090c};	
	int iTestResult;
	int i;
	char *filename = "c:\\FputwcTestInputFile.txt";

	iTestResult = KErrNone;
	fp = fopen(filename,"w");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... fputwc:2  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	for(i=0;i<10;i++)
		{
		wretval = fputwc(wchar_letters[i],fp);
		if((signed int)wretval == WEOF)
			{
			INFO_PRINTF2(_L("fputwc:2 Failed!!"), errno);
			iTestResult = KErrGeneral;
			fclose(fp);
					
			return iTestResult;
			}
		}
	fclose(fp);
	
	fp = fopen(filename,"r");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... fputwc:2 Failed!!"), errno);
		iTestResult = KErrGeneral;
		
		return iTestResult;
		}

	for(i=0;i<10;i++)
		{
		rretval = fgetwc(fp);
		if(wchar_letters[i] != rretval)
			{
			iTestResult = KErrGeneral;
			break;
			}
		}
	
	if(iTestResult == KErrGeneral)
		{	
		INFO_PRINTF2(_L("fputwc:2 Failed !!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("fputwc:2 Passed !!"), errno);
		}

	fclose(fp);
	
	return iTestResult;    
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestThreefputwcL
//API Tested			: fputwc
//TestCase Description	: This test case writes a new line character then 
//closes the file. The file is again opened and the character that was written
// is read back and compared.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestThreefputwcL()
	{
    FILE *fp;
	wint_t rretval,wretval;
	int iTestResult;
	char *filename = "c:\\FputwcTestInputFile.txt";

	iTestResult = KErrNone;
	fp = fopen(filename,"w");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... fputwc:3 Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	wretval = fputwc(L'\n',fp);
	if((signed int)wretval == WEOF)
		{
	    INFO_PRINTF2(_L("fputwc:3 Failed!!"), errno);
		iTestResult = KErrGeneral;
		fclose(fp);
		
		return iTestResult;
		}

	fclose(fp);

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... fputwc:3  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
		return iTestResult;
		}

	rretval = fgetwc(fp);
	if(wretval != rretval)
		{
		iTestResult = KErrGeneral;
		}
	
	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("fputwc:3 Failed !!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("fputwc:3 Passed !!"), errno);
		}

	fclose(fp);
		
	return iTestResult;    
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestFourfputwcL
//API Tested			: fputwc
//TestCase Description	: This test case tries to write of a file that is 
//as readonly.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestFourfputwcL()
	{
    FILE *fp;
	wint_t wretval;
	unsigned int wchar_letters[20] = {0x0905,0x0906,0x0907};	
	int iTestResult,iFCreateRetVal;
	char *filename = "c:\\FputwcTestInputFile.txt";

	iTestResult = KErrNone;
	iFCreateRetVal = CreateTestDataFile(filename,wchar_letters,3);
	if(iFCreateRetVal != 0)
		{
		INFO_PRINTF2(_L("Error : Input File creation... fputwc:4 Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... fputwc:4 Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	wretval = fputwc(L'\n',fp);
	if((signed int)wretval != WEOF)
		{
	    iTestResult = KErrGeneral;
		}


	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("fputwc:4 Failed !!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("fputwc:4 Passed !!"), errno);
		}

	fclose(fp);
	
	return iTestResult;    
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestOneputwcL
//API Tested			: putwc
//TestCase Description	: This test case writes a wchar to the stream and 
//closes the file. The file is again opened and the character that was written
//by fputwc is read back and compared.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestOneputwcL(  )
	{
    FILE *fp;
	wint_t rretval,wretval;
	int iTestResult;
	char *filename = "c:\\PutwcTestInputFile.txt";

	iTestResult = KErrNone;
	fp = fopen(filename,"w");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... putwc:1 Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	wretval = putwc(wchar_t(0x0905),fp);
	if((signed int)wretval == WEOF)
		{
	    INFO_PRINTF2(_L("putwc:1 Failed!!"), errno);
		iTestResult = KErrGeneral;
		fclose(fp);
		
		return iTestResult;    
		}
	
	fclose(fp);

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... putwc:1 Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	rretval = fgetwc(fp);
	if(wretval != rretval)
		{
		iTestResult = KErrGeneral;
		}
	else
		{
		iTestResult = KErrNone;
		}

	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("putwc:1 Failed!!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("putwc:1 Passed !!"), errno);
		}

	fclose(fp);
	
	return iTestResult;    
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestTwoputwcL
//API Tested			: putwc
//TestCase Description	: This test case writes a set of wide characters to 
//the stream and closes the file. The file is again opened and the characters
//that were written is read back and compared.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestTwoputwcL(  )
	{
    FILE *fp;
	wint_t rretval,wretval;
	wchar_t wchar_letters[20] = {0x0905,0x0906,0x0907,0x0909,0x090a,0x090f,0x090d,0x090e,0x090b,0x090c};	
	int iTestResult;
	int i;
	char *filename = "c:\\PutwcTestInputFile.txt";
	
	iTestResult = KErrNone;
	fp = fopen(filename,"w");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... putwc:2 Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	for(i=0;i<10;i++)
		{
		wretval = putwc(wchar_letters[i],fp);
		if((signed int)wretval == WEOF)
			{
			INFO_PRINTF2(_L("putwc:2 Failed!!"), errno);
			iTestResult = KErrGeneral;
			fclose(fp);
			
			return iTestResult;
			}
		}
	fclose(fp);
	
	fp = fopen(filename,"r");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... putwc:2 Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	for(i=0;i<10;i++)
		{	
		rretval = fgetwc(fp);
		if(wchar_letters[i] != rretval)
			{
			iTestResult = KErrGeneral;
			break;
			}
		}
	
	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("putwc:2 Failed !!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("putwc:2 Passed !!"), errno);
		}

	fclose(fp);
	
	return iTestResult;    
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestThreeputwcL
//API Tested			: putwc
//TestCase Description	: This test case writes a new line character then 
//closes the file. The file is again opened and the character that was written
// is read back and compared.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestThreeputwcL(  )
	{
    FILE *fp;
	wint_t rretval,wretval;
	int iTestResult;
	char *filename = "c:\\PutwcTestInputFile.txt";
	
	iTestResult = KErrNone;
	fp = fopen(filename,"w");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... putwc:3 Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	wretval = putwc(L'\n',fp);
	if((signed int)wretval == WEOF)
		{
	    INFO_PRINTF2(_L("putwc:3 Failed!!"), errno);
		iTestResult = KErrGeneral;
		fclose(fp);
		
		return iTestResult;
		}

	fclose(fp);

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... putwc:3 Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	rretval = fgetwc(fp);
	if(wretval != rretval)
		{
		iTestResult = KErrGeneral;
		}
	
	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("putwc:3 Failed !!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("putwc:3 Passed !!"), errno);
		}

	fclose(fp);
	
	
	return iTestResult;    
}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestFourputwcL
//API Tested			: putwc
//TestCase Description	: This test case tries to write of a file that is 
//as readonly.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestFourputwcL(  )
	{
    FILE *fp;
	wint_t wretval;
	unsigned int wchar_letters[20] = {0x0905,0x0906,0x0907};	
	int iTestResult,iFCreateRetVal;
	char *filename = "c:\\PutwcTestInputFile.txt";

	iTestResult = KErrNone;
	iFCreateRetVal = CreateTestDataFile(filename,wchar_letters,3);
	if(iFCreateRetVal != 0)
		{
		INFO_PRINTF2(_L("Error : Input File creation... putwc:4 Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
	    INFO_PRINTF2(_L("Error : File open... putwc:4 Failed!!"), errno);
		iTestResult = KErrGeneral;
		
		return iTestResult;
		}

	wretval = putwc(L'\n',fp);
	if((signed int)wretval != WEOF)
		{
	    iTestResult = KErrGeneral;
		}


	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("putwc:4 Failed !!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("putwc:4 Passed !!"), errno);
		}

	fclose(fp);
		
	return iTestResult; 
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestOnefputwsL
//API Tested			: fputws
//TestCase Description	: This test case reads a string from a file and writes
//it into another file and then closes the file. The file is again is opened 
//the string that was written is read back. The string that was written and 
//read back from the file are compared.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestOnefputwsL(  )
	{    
    FILE *fp;
	wint_t wretval;
	wchar_t rws1[10],rws2[10];
	wchar_t *rptr;
	unsigned int wchar_letters[20] = {0x0905,0x0906,0x0907,0x0909,0x090a,0x090f,0x090d,0x090e,0x090b,0x090c};	
	int iTestResult,iFCreateRetVal,retval;
	char *filename = "c:\\FputwsTestInputFile.txt";
	char *fname = "c:\\FputwsTestFile.txt";

	iTestResult = KErrNone;
	iFCreateRetVal = CreateTestDataFile(filename,wchar_letters,10);
	if(iFCreateRetVal != 0)
		{
		INFO_PRINTF2(_L("Error : Input File creation... fputws:1  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}
	
	fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF2(_L("Error : File open... fputws:1 Failed, errno is %d\n!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	rptr = fgetws(rws1,10,fp);
	if(rptr == NULL)
		{
		INFO_PRINTF2(_L("Error : Reading from the input file... fputws:1 Failed!!"), errno);
		iTestResult = KErrGeneral;
		fclose(fp);
		
		return iTestResult;
		}

	fclose(fp);

	fp = fopen(fname,"w");
	if(fp == NULL)
		{
		INFO_PRINTF2(_L("Error : File open... fputws:1  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
		return iTestResult;
		}

	wretval = fputws(rws1,fp);
	if((signed int)wretval == WEOF)
		{
		INFO_PRINTF2(_L("fputws:1 Failed!!"), errno);
		iTestResult = KErrGeneral;
		fclose(fp);
		
	    return iTestResult;
		}

	fclose(fp);

	fp = fopen(fname,"r");
	if(fp == NULL)
		{
		INFO_PRINTF2(_L("Error : File open... fputws:1  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	rptr = fgetws(rws2,10,fp);
	if(rptr == NULL)
		{
		INFO_PRINTF2(_L("Error : Reading back from the file ... fputws:1  Failed!!"), errno);
		iTestResult = KErrGeneral;
		fclose(fp);
		
	    return iTestResult;
		}

	retval = wcscmp(rws1,rws2);
	if(retval != 0)
		{
		iTestResult = KErrGeneral;
		}

	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("fputws:1 Failed !!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("fputws:1 Passed !!"), errno);
		}

	fclose(fp);

	
    return iTestResult;
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestTwofputwsL
//API Tested			: fputws
//TestCase Description	: This test case writes a wide string which has a
//newline character in the middle of the string and the file is closed. Then
//the file is again opened and the string that was written is read back and
//the strings are compared.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestTwofputwsL(  )
	{
    FILE *fp;
	wint_t wretval;
	wchar_t rws1[10] = L"abcd\r\nefg";
	wchar_t *rptr,rws2[10];
	int retval,iTestResult;
	char *filename = "c:\\FputwsTestInputFile.txt";

	iTestResult = KErrNone;
	fp = fopen(filename,"w");
	if(fp == NULL)
		{
		INFO_PRINTF2(_L("Error : File open... fputws:2  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	wretval = fputws(rws1,fp);
	if((signed int)wretval == WEOF)
		{
		INFO_PRINTF2(_L("fputws:2 Failed!!"), errno);
		iTestResult = KErrGeneral;
		fclose(fp);
		
		return iTestResult;
		}

	fclose(fp);

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF2(_L("Error : File open... fputws:2  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
		return iTestResult;
		}

	rptr = fgetws(rws2,10,fp);
	if(rptr == NULL)
		{
		INFO_PRINTF2(_L("Error : Reading back from the file... fputws:2  Failed!!"), errno);
		iTestResult = KErrGeneral;
		fclose(fp);
		
		return iTestResult;
		}

	retval = wcscmp(L"abcd\r\n",rws2);
	if(retval != 0)
		{
		iTestResult = KErrGeneral;	
		}
	
	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("fputws:2 Failed !!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("fputws:2 Passed !!"), errno);
		}

	fclose(fp);
	
	return iTestResult;
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestThreefputwsL
//API Tested			: fputws
//TestCase Description	: This test case tries to write of a file that is 
//as readonly.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestThreefputwsL(  )
	{
    FILE *fp;
	wint_t wretval;
	unsigned int wchar_letters[5] = {0x0905,0x0906,0x0907};	
	int iTestResult,iFCreateRetVal;
	char *filename = "c:\\FputwsTestInputFile.txt";

	iTestResult = KErrNone;
	iFCreateRetVal = CreateTestDataFile(filename,wchar_letters,3);
	if(iFCreateRetVal != 0)
		{
		INFO_PRINTF2(_L("Error : Input File creation... fputws:3 Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}
	
	fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF2(_L("Error : File open... fputws:3  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
		
	    return iTestResult;
		}

	wretval = fputws(L"a",fp);
	if((signed int)wretval != WEOF)
		{
		iTestResult = KErrGeneral;
		}
	
	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("fputws:3 Failed !!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("fputws:3 Passed !!"), errno);
		}

	fclose(fp);
	
	
	return iTestResult;
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestFourfputwsL
//API Tested			: fputws
//TestCase Description	: This test case writes a wide string which has a
//string terminator character in the middle of the string and the file is 
//closed. Then the file is again opened and the string that was written is 
//read back and the strings are compared.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestFourfputwsL(  )
	{	
    FILE *fp;
	wint_t wretval;
	wchar_t rws1[10] = L"abcd\0efg";
	wchar_t *rptr,rws2[10];
	int iTestResult,retval;
	char *filename = "c:\\FputwsTestInputFile.txt";

	iTestResult = KErrNone;
	fp = fopen(filename,"w");
	if(fp == NULL)
		{
		INFO_PRINTF2(_L("Error : File open... fputws:4  Failed!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	wretval = fputws(rws1,fp);
	if((signed int)wretval == WEOF)
		{
		INFO_PRINTF2(_L("fputws:4  Failed!!"), errno);
		iTestResult = KErrGeneral;
		fclose(fp);
		
		
	    return iTestResult;
		}

	fclose(fp);

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF2(_L("Error : File open... fputws:4 Failed!!"), errno);
		iTestResult = KErrGeneral;
		
		
	    return iTestResult;
		}

	rptr = fgetws(rws2,10,fp);
	if(rptr == NULL)
		{
	    INFO_PRINTF2(_L("Error : Reading back from the file... fputws:4 Failed!!"), errno);
		iTestResult = KErrGeneral;
		fclose(fp);
		
		
	    return iTestResult;
		}

	retval = wcscmp(L"abcd",rws2);
	if(retval != 0)
		{
		iTestResult = KErrGeneral;
		}

	if(iTestResult == KErrGeneral)
		INFO_PRINTF2(_L("fputws:4 Failed !!"), errno);
	else
		INFO_PRINTF2(_L("fputws:4 Passed !!"), errno);

	fclose(fp);
	
	return iTestResult;
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestOneungetwcL
//API Tested			: ungetwc
//TestCase Description	: The testcase function will test ungetwc randomly by
//calling getwc and ungetwc is some random order. The values returned by both
//ungetwc and getwc are compared.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestOneungetwcL(  )
	{
	FILE *fp;
	wint_t c;
	int iTestResult;
	int iFCreateRetVal;
	unsigned int wchar_letters[5] = {0x0062,0x006c,0x0061};	
	char *filename = "c:\\UngetwcInputFile.txt";

	iTestResult = KErrNone;
	iFCreateRetVal = CreateTestDataFile(filename,wchar_letters,3);
	if(iFCreateRetVal != 0)
		{
	    INFO_PRINTF2(_L("Error : Input File creation... ungetwc Failed, errno is %d\n!!"), errno);
		iTestResult = KErrGeneral;
		
	    return iTestResult;
		}

	fp = fopen(filename,"r");
	if(fp == NULL)
		{
		INFO_PRINTF2(_L("Error : File open... ungetwc Failed!!"), errno);
		iTestResult = KErrGeneral;
		
		return iTestResult;
	
		}

	if(ungetwc (L'z', fp) != L'z')
		{
		iTestResult = KErrGeneral;
		}

	if(getwc (fp) != L'z')
		{
		iTestResult = KErrGeneral;
		}

	if(getwc (fp) != L'b')
		{
		iTestResult = KErrGeneral;
		}

	if(getwc (fp) != L'l')
		{
		iTestResult = KErrGeneral;
		}

	if(ungetwc (L'm', fp) != L'm')
		{
		iTestResult = KErrGeneral;
		}

	if(getwc (fp) != L'm')
		{
		iTestResult = KErrGeneral;
		}
	
	if((c = getwc (fp)) != L'a')
		{
		iTestResult = KErrGeneral;
		}
	
	if((signed int)getwc (fp) != WEOF)
		{
		iTestResult = KErrGeneral;
		}

	if(ungetwc (c, fp) != c)
		{
		iTestResult = KErrGeneral;
		}

	if(getwc (fp) != c)
		{
		iTestResult = KErrGeneral;
		}

	if((signed int)getwc (fp) != WEOF)
		{
		iTestResult = KErrGeneral;
		}
    
	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("ungetwc Failed !!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("ungetwc Passed !!"), errno);
		}

	fclose(fp);
	
	return iTestResult;
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestOneputwcharL
//API Tested			: putwchar
//TestCase Description	: This test case writes a wide character and checks for
//the return value of putwchar.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestOneputwcharL(  )
	{
	wint_t wc2;
	wchar_t wc1;
	int iTestResult;

	iTestResult = KErrNone;

	wprintf(L"A wide character is printed : ");
	wc1 = 0x0906;
	wc2 = putwchar(wc1);
	if(wc1 != (wchar_t)wc2)
		{
		iTestResult = KErrGeneral;
		}
	
	INFO_PRINTF2(_L("%x : This testcase printed a some wide character !"), wc1);
	wprintf(L"\n\nPress any key to continue....\n");
	getwchar();
	
	wprintf(L"Null termination character is printed : ");	
	wc1 = L'\0';
	wc2 = putwchar(wc1);
	if(wc1 != (wchar_t)wc2)
		{
		iTestResult = KErrGeneral;
		}

	INFO_PRINTF2(_L("%x : This testcase printed a null character !"),wc2);
	wprintf(L"\n\nPress any key to continue....\n");	
	getwchar();

	wprintf(L"A new line character is printed : ");
	wc1 = L'\n';
	wc2 = putwchar(wc1);
	if(wc1 != (wchar_t)wc2)
		{
		iTestResult = KErrGeneral;
		}

	INFO_PRINTF2(_L("%x : This testcase printed a new line character !"),wc1);
	wprintf(L"\n\nPress any key to continue....\n");	
	getwchar();
	
	wprintf(L"Few random characters are printed: ");

	wc1 = L'a';
	wc2 = putwchar(wc1);

	if(wc1 != (wchar_t)wc2)
		{
		iTestResult = KErrGeneral;
		}
	
	wc1 = L' ';
	wc2 = putwchar(wc1);

	if(wc1 != (wchar_t)wc2)
		{
		iTestResult = KErrGeneral;
		}

	wc1 = L'x';
	wc2 = putwchar(wc1);

	if(wc1 != (wchar_t)wc2)
		{
		iTestResult = KErrGeneral;
		}

	wc1 = L'\n';
	wc2 = putwchar(wc1);

	if(wc1 != (wchar_t)wc2)
		{
		iTestResult = KErrGeneral;
		}
	
	if(iTestResult == KErrGeneral)
		{
		wprintf(L"\n\nputwchar Failed !!");
		}
	else
		{
		wprintf(L"\n\nputwchar Passed !!");
		}
	
	wprintf(L"\n\nPress any key to exit the testcase....\n");
	getwchar();

	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("putwchar Failed !!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("putwchar Passed !!"), errno);
		}
	
	return iTestResult;
	}

//----------------------------------------------------------------------------
//Function Name		: CTestLibcwchar::TestOnegetwcharL
//API Tested			: getwchar
//TestCase Description	: getwchar can be tested using this testcase function 
//by entering different characters. The character that was entered will be
//displayed on the console and the return value of getwchar is also checked.
//----------------------------------------------------------------------------
TInt CTestLibcwchar::TestOnegetwcharL(  )
	{
	wint_t wc;
	int iTestResult;
	
	iTestResult = KErrNone;
	wprintf(L"Enter any character : ");
	wc = getwchar();	

	if((signed int)wc == WEOF)
		{
		iTestResult = KErrGeneral;
		}
	
	wprintf(L"\nCharacter that was entered : %lc\n",wc);
	

	if(iTestResult == KErrGeneral)
		{
		wprintf(L"\n\ngetwchar Failed !!");
		}
	else
		{
		wprintf(L"\n\ngetwchar Passed !!");
		}
	
	wprintf(L"\n\nPress any key to exit the testcase....\n");
	getwchar();

	if(iTestResult == KErrGeneral)
		{
		INFO_PRINTF2(_L("getwchar Failed !!"), errno);
		}
	else
		{
		INFO_PRINTF2(_L("getwchar Passed !!"), errno);
		}

	return iTestResult;
	}

TInt CTestLibcwchar::CreateTestDataFile(char *filename,unsigned int *arr, int count)
	{
	FILE *iFp;
	int i;
	int iRval;
	unsigned int *iStartaddr;
 
	iFp = NULL;
	iFp = fopen(filename,"w");
	if(iFp == NULL)
		{
		return -1;
		}
 
	iStartaddr = arr;
	for(i=0;i<count;i++)
		{
		iRval = fputwc((wchar_t) (*iStartaddr),iFp);
		if((signed int)iRval == WEOF)
			{
			fclose(iFp);
		
			return -2;
			}
		iStartaddr++;
		}
	fclose(iFp);
	return 0;
	}

