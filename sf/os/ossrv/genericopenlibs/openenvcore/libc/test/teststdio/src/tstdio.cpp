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

CTestStdio::~CTestStdio() 
	{ 
	}  

CTestStdio::CTestStdio(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestStdio::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EFail);
	return TestStepResult();
	}



TVerdict CTestStdio::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;
	return TestStepResult();
	}


TVerdict CTestStdio::doTestStepL()
	{
	int err;
	TVerdict err1 = EFail;
	if(TestStepName() == Kremove1)
		{
		INFO_PRINTF1(_L("remove1():"));
		err = remove1();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == Kremove2)
   			{
   	   		INFO_PRINTF1(_L("remove2():"));
   	   		err = remove2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Krename1)
   			{
   	   		INFO_PRINTF1(_L("rename1():"));
   	   		err = rename1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Ktempnam1)
   			{
   	   		INFO_PRINTF1(_L("tempnam1():"));
   	   		err = tempnam1 ();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ktempnam2)
   			{
   	   		INFO_PRINTF1(_L("tempnam2():"));
   	   		err = tempnam2 ();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfclose1)
   			{
   	   		INFO_PRINTF1(_L("fclose1():"));
   	   		err = fclose1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfflush1)
   			{
   	   		INFO_PRINTF1(_L("fflush1():"));
   	   		err = fflush1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfopen1)
   			{
   	   		INFO_PRINTF1(_L("fopen1():"));
   	   		err = fopen1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfopen2)
   			{
   	   		INFO_PRINTF1(_L("fopen2():"));
   	   		err = fopen2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfopen3)
   			{
   	   		INFO_PRINTF1(_L("fopen3():"));
   	   		err = fopen3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Ksetbuf1)
   			{
   	   		INFO_PRINTF1(_L("setbuf1():"));
   	   		err = setbuf1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Ksetbuf2)
   			{
   	   		INFO_PRINTF1(_L("setbuf2():"));
   	   		err = setbuf2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Ksetvbuf1)
   			{
   	   		INFO_PRINTF1(_L("setvbuf1():"));
   	   		err = setvbuf1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Ksetvbuf2)
   			{
   	   		INFO_PRINTF1(_L("setvbuf2():"));
   	   		err = setvbuf2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Ksetvbuf3)
   			{
   	   		INFO_PRINTF1(_L("setvbuf3():"));
   	   		err = setvbuf3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kclearerr1)
   			{
   	   		INFO_PRINTF1(_L("clearerr1():"));
   	   		err = clearerr1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfeof1)
   			{
   	   		INFO_PRINTF1(_L("feof1():"));
   	   		err = feof1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kferror1)
   			{
   	   		INFO_PRINTF1(_L("ferror1():"));
   	   		err = ferror1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfread1)
   			{
   	   		INFO_PRINTF1(_L("fread1():"));
   	   		err = fread1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfread2)
   			{
   	   		INFO_PRINTF1(_L("fread2():"));
   	   		err = fread2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfread3)
   			{
   	   		INFO_PRINTF1(_L("fread3():"));
   	   		err = fread3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfwrite1)
   			{
   	   		INFO_PRINTF1(_L("fwrite1():"));
   	   		err = fwrite1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfwrite2)
   			{
   	   		INFO_PRINTF1(_L("fwrite2():"));
   	   		err = fwrite2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfwrite3)
   			{
   	   		INFO_PRINTF1(_L("fwrite3():"));
   	   		err = fwrite3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfgetpos1)
   			{
   	   		INFO_PRINTF1(_L("fgetpos1():"));
   	   		err = fgetpos1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfseek1)
   			{
   	   		INFO_PRINTF1(_L("fseek1():"));
   	   		err = fseek1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfseek2)
	   		{
	   	   	INFO_PRINTF1(_L("fseek2():"));
	   	   	err = fseek2();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}
	else if(TestStepName() == Kfsetpos1)
   			{
   	   		INFO_PRINTF1(_L("fsetpos1():"));
   	   		err = fsetpos1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfsetpos2)
   			{
   	   		INFO_PRINTF1(_L("fsetpos2():"));
   	   		err = fsetpos2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kftell1)
   			{
   	   		INFO_PRINTF1(_L("ftell1():"));
   	   		err = ftell1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Krewind1)
   			{
   	   		INFO_PRINTF1(_L("rewind1():"));
   	   		err = rewind1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfgetc1)
   			{
   	   		INFO_PRINTF1(_L("fgetc1():"));
   	   		err = fgetc1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfputc1)
   			{
   	   		INFO_PRINTF1(_L("fputc1():"));
   	   		err = fputc1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfputc2)
   			{
   	   		INFO_PRINTF1(_L("fputc2():"));
   	   		err = fputc2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfputc3)
   			{
   	   		INFO_PRINTF1(_L("fputc3():"));
   	   		err = fputc3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfputc4)
   			{
   	   		INFO_PRINTF1(_L("fputc4():"));
   	   		err = fputc4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfputs1)
   			{
   	   		INFO_PRINTF1(_L("fputs1():"));
   	   		err = fputs1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfputs2)
   			{
   	   		INFO_PRINTF1(_L("fputs2():"));
   	   		err = fputs2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfputs3)
   			{
   	   		INFO_PRINTF1(_L("fputs3():"));
   	   		err = fputs3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfputs4)
   			{
   	   		INFO_PRINTF1(_L("fputs4():"));
   	   		err = fputs4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kgetc1)
   			{
   	   		INFO_PRINTF1(_L("getc1():"));
   	   		err = getc1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kputc1)
   			{
   	   		INFO_PRINTF1(_L("putc1():"));
   	   		err = putc1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kputc2)
   			{
   	   		INFO_PRINTF1(_L("putc2():"));
   	   		err = putc2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kputc3)
   			{
   	   		INFO_PRINTF1(_L("putc3():"));
   	   		err = putc3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kputc4)
   			{
   	   		INFO_PRINTF1(_L("putc4():"));
   	   		err = putc4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kungetc1)
   			{
   	   		INFO_PRINTF1(_L("ungetc1():"));
   	   		err = ungetc1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfileno1)
   			{
   	   		INFO_PRINTF1(_L("fileno1():"));
   	   		err = fileno1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfileno3)
   			{
   	   		INFO_PRINTF1(_L("fileno3():"));
   	   		err = fileno3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
	else if(TestStepName() == Kfileno2)
   			{
   	   		INFO_PRINTF1(_L("fileno2():"));
   	   		err = fileno2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kgetw1)
   			{
   	   		INFO_PRINTF1(_L("getw1():"));
   	   		err = getw1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kputw1)
   			{
   	   		INFO_PRINTF1(_L("putw1():"));
   	   		err = putw1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kgetdirentries1)
   			{
   	   		INFO_PRINTF1(_L("getdirentries1():"));
   	   		err = getdirentries1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kgetdirentries2)
   			{
   	   		INFO_PRINTF1(_L("getdirentries2():"));
   	   		err = getdirentries2();    	   	
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
    	   	}
	else if(TestStepName() == Kfreopen1)
	   		{
	   	   	INFO_PRINTF1(_L("freopen1():"));
	   	   	err = freopen1();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}
	else if(TestStepName() == KprintfTest1L)
   			{
   	   		INFO_PRINTF1(_L("printfTest1L():"));
   	   		err = printfTest1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest2L)
   			{
   	   		INFO_PRINTF1(_L("printfTest2L():"));
   	   		err = printfTest2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest3L)
   			{
   	   		INFO_PRINTF1(_L("printfTest3L():"));
   	   		err = printfTest3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest4L)
   			{
   	   		INFO_PRINTF1(_L("printfTest4L():"));
   	   		err = printfTest4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest5L)
   			{
   	   		INFO_PRINTF1(_L("printfTest5L():"));
   	   		err = printfTest5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest6L)
   			{
   	   		INFO_PRINTF1(_L("printfTest6L():"));
   	   		err = printfTest6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest7L)
   			{
   	   		INFO_PRINTF1(_L("printfTest7L():"));
   	   		err = printfTest7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest7L)
   			{
   	   		INFO_PRINTF1(_L("printfTest7L():"));
   	   		err = printfTest7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest8L)
   			{
   	   		INFO_PRINTF1(_L("printfTest8L():"));
   	   		err = printfTest8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest9L)
   			{
   	   		INFO_PRINTF1(_L("printfTest9L():"));
   	   		err = printfTest9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest10L)
   			{
   	   		INFO_PRINTF1(_L("printfTest10L():"));
   	   		err = printfTest10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest11L)
   			{
   	   		INFO_PRINTF1(_L("printfTest11L():"));
   	   		err = printfTest11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest12L)
   			{
   	   		INFO_PRINTF1(_L("printfTest12L():"));
   	   		err = printfTest12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest13L)
   			{
   	   		INFO_PRINTF1(_L("printfTest13L():"));
   	   		err = printfTest13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest14L)
   			{
   	   		INFO_PRINTF1(_L("printfTest14L():"));
   	   		err = printfTest14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest15L)
   			{
   	   		INFO_PRINTF1(_L("printfTest15L():"));
   	   		err = printfTest15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest16L)
   			{
   	   		INFO_PRINTF1(_L("printfTest16L():"));
   	   		err = printfTest16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest17L)
   			{
   	   		INFO_PRINTF1(_L("printfTest17L():"));
   	   		err = printfTest17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest18L)
   			{
   	   		INFO_PRINTF1(_L("printfTest18L():"));
   	   		err = printfTest18L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest19L)
   			{
   	   		INFO_PRINTF1(_L("printfTest19L():"));
   	   		err = printfTest19L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest20L)
   			{
   	   		INFO_PRINTF1(_L("printfTest20L():"));
   	   		err = printfTest20L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest21L)
   			{
   	   		INFO_PRINTF1(_L("printfTest21L():"));
   	   		err = printfTest21L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest22L)
   			{
   	   		INFO_PRINTF1(_L("printfTest22L():"));
   	   		err = printfTest22L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest23L)
   			{
   	   		INFO_PRINTF1(_L("printfTest23L():"));
   	   		err = printfTest23L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest24L)
   			{
   	   		INFO_PRINTF1(_L("printfTest24L():"));
   	   		err = printfTest24L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest25L)
   			{
   	   		INFO_PRINTF1(_L("printfTest25L():"));
   	   		err = printfTest25L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest26L)
   			{
   	   		INFO_PRINTF1(_L("printfTest26L():"));
   	   		err = printfTest26L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KprintfTest27L)
   			{
   	   		INFO_PRINTF1(_L("printfTest27L():"));
   	   		err = printfTest27L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest1L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest1L():"));
   	   		err = fprintfTest1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest2L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest2L():"));
   	   		err = fprintfTest2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest3L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest3L():"));
   	   		err = fprintfTest3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest4L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest4L():"));
   	   		err = fprintfTest4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest5L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest5L():"));
   	   		err = fprintfTest5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest6L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest6L():"));
   	   		err = fprintfTest6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest7L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest7L():"));
   	   		err = fprintfTest7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest8L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest8L():"));
   	   		err = fprintfTest8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest9L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest9L():"));
   	   		err = fprintfTest9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest10L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest10L():"));
   	   		err = fprintfTest10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest11L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest11L():"));
   	   		err = fprintfTest11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest12L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest12L():"));
   	   		err = fprintfTest12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest13L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest13L():"));
   	   		err = fprintfTest13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest14L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest14L():"));
   	   		err = fprintfTest14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest15L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest15L():"));
   	   		err = fprintfTest15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest16L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest16L():"));
   	   		err = fprintfTest16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest17L)
   	   		   			{
   	   		INFO_PRINTF1(_L("fprintfTest17L():"));
   	   		err = fprintfTest17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest18L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest18L():"));
   	   		err = fprintfTest18L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest19L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest19L():"));
   	   		err = fprintfTest19L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest20L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest20L():"));
   	   		err = fprintfTest20L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest21L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest21L():"));
   	   		err = fprintfTest21L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest22L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest22L():"));
   	   		err = fprintfTest22L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest23L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest23L():"));
   	   		err = fprintfTest23L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest24L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest24L():"));
   	   		err = fprintfTest24L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest25L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest25L():"));
   	   		err = fprintfTest25L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest26L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest26L():"));
   	   		err = fprintfTest26L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfprintfTest27L)
   			{
   	   		INFO_PRINTF1(_L("fprintfTest27L():"));
   	   		err = fprintfTest27L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest1L)
		   			{
   	   		INFO_PRINTF1(_L("sprintfTest1L():"));
   	   		err = sprintfTest1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest2L)	
   			{
   	   		INFO_PRINTF1(_L("sprintfTest2L():"));
   	   		err = sprintfTest2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest3L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest3L():"));
   	   		err = sprintfTest3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest4L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest4L():"));
   	   		err = sprintfTest4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest5L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest5L():"));
   	   		err = sprintfTest5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest6L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest6L():"));
   	   		err = sprintfTest6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest7L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest7L():"));
   	   		err = sprintfTest7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest8L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest8L():"));
   	   		err = sprintfTest8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest9L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest9L():"));
   	   		err = sprintfTest9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest10L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest10L():"));
   	   		err = sprintfTest10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest11L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest11L():"));
   	   		err = sprintfTest11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest12L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest12L():"));
   	   		err = sprintfTest12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest13L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest13L():"));
   	   		err = sprintfTest13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest14L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest14L():"));
   	   		err = sprintfTest14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest15L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest15L():"));
   	   		err = sprintfTest15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest16L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest16L():"));
   	   		err = sprintfTest16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest17L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest17L():"));
   	   		err = sprintfTest17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest18L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest18L():"));
   	   		err = sprintfTest18L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest19L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest19L():"));
   	   		err = sprintfTest19L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest20L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest20L():"));
   	   		err = sprintfTest20L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest21L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest21L():"));
   	   		err = sprintfTest21L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest22L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest22L():"));
   	   		err = sprintfTest22L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest23L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest23L():"));
   	   		err = sprintfTest23L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest24L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest24L():"));
   	   		err = sprintfTest24L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest25L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest25L():"));
   	   		err = sprintfTest25L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest26L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest26L():"));
   	   		err = sprintfTest26L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsprintfTest27L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest27L():"));
   	   		err = sprintfTest27L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KsprintfTest28L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest28L():"));
   	   		err = sprintfTest28L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KsprintfTest29L)
   			{
   	   		INFO_PRINTF1(_L("sprintfTest29L():"));
   	   		err = sprintfTest29L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KsprintfTestInfL)
   			{
   	   		INFO_PRINTF1(_L("sprintfTestInfL():"));
   	   		err = sprintfTestInfL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KsprintfTestNanL)
   			{
   	   		INFO_PRINTF1(_L("sprintfTestNanL():"));
   	   		err = sprintfTestNanL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest1L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest1L():"));
   	   		err = snprintfTest1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest2L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest2L():"));
   	   		err = snprintfTest2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest3L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest3L():"));
   	   		err = snprintfTest3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest4L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest4L():"));
   	   		err = snprintfTest4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest5L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest5L():"));
   	   		err = snprintfTest5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest6L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest6L():"));
   	   		err = snprintfTest6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest7L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest7L():"));
   	   		err = snprintfTest7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest8L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest8L():"));
   	   		err = snprintfTest8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest9L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest9L():"));
   	   		err = snprintfTest9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest10L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest10L():"));
   	   		err = snprintfTest10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest11L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest11L():"));
   	   		err = snprintfTest11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest12L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest12L():"));
   	   		err = snprintfTest12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest13L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest13L():"));
   	   		err = snprintfTest13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest14L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest14L():"));
   	   		err = snprintfTest14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest15L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest15L():"));
   	   		err = snprintfTest15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest16L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest16L():"));
   	   		err = snprintfTest16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest17L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest17L():"));
   	   		err = snprintfTest17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest18L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest18L():"));
   	   		err = snprintfTest18L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest19L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest19L():"));
   	   		err = snprintfTest19L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest20L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest20L():"));
   	   		err = snprintfTest20L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest21L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest21L():"));
   	   		err = snprintfTest21L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest22L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest22():"));
   	   		err = snprintfTest22L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest23L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest23L():"));
   	   		err = snprintfTest23L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest24L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest24L():"));
   	   		err = snprintfTest24L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest25L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest25L():"));
   	   		err = snprintfTest25L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest26L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest26L():"));
   	   		err = snprintfTest26L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsnprintfTest27L)
   			{
   	   		INFO_PRINTF1(_L("snprintfTest27L():"));
   	   		err = snprintfTest27L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest1L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest1L():"));
   	   		err = vprintfTest1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest2L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest2L():"));
   	   		err = vprintfTest2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest3L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest3L():"));
   	   		err = vprintfTest3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest4L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest4L():"));
   	   		err = vprintfTest4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest5L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest5L():"));
   	   		err = vprintfTest5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest6L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest6L():"));
   	   		err = vprintfTest6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest7L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest7L():"));
   	   		err = vprintfTest7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest8L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest8L():"));
   	   		err = vprintfTest8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest9L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest9L():"));
   	   		err = vprintfTest9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest10L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest10L():"));
   	   		err = vprintfTest10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest11L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest11L():"));
   	   		err = vprintfTest11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest12L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest12L():"));
   	   		err = vprintfTest12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest13L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest13L():"));
   	   		err = vprintfTest13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest14L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest14L():"));
   	   		err = vprintfTest14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest15L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest15L():"));
   	   		err = vprintfTest15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest16L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest16L():"));
   	   		err = vprintfTest16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest17L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest17L():"));
   	   		err = vprintfTest17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest18L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest18L():"));
   	   		err = vprintfTest18L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest19L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest19L():"));
   	   		err = vprintfTest19L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest20L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest20L():"));
   	   		err = vprintfTest20L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest21L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest21L():"));
   	   		err = vprintfTest21L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest22L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest22L():"));
   	   		err = vprintfTest22L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest23L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest23L():"));
   	   		err = vprintfTest23L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest24L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest24L():"));
   	   		err = vprintfTest24L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest25L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest25L():"));
   	   		err = vprintfTest25L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest26L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest26L():"));
   	   		err = vprintfTest26L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvprintfTest27L)
   			{
   	   		INFO_PRINTF1(_L("vprintfTest27L():"));
   	   		err = vprintfTest27L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfprintfbasic)
   			{
   	   		INFO_PRINTF1(_L("fprintfbasic():"));
   	   		err = fprintfbasic();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KscanfTest1L)
   			{
   	   		INFO_PRINTF1(_L("scanfTest1L():"));
   	   		err = scanfTest1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KscanfTest2L)
   			{
   	   		INFO_PRINTF1(_L("scanfTest2L():"));
   	   		err = scanfTest2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KscanfTest3L)
   			{
   	   		INFO_PRINTF1(_L("scanfTest3L():"));
   	   		err = scanfTest3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KscanfTest4L)
   			{
   	   		INFO_PRINTF1(_L("scanfTest4L():"));
   	   		err = scanfTest4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KscanfTest5L)
   			{
   	   		INFO_PRINTF1(_L("scanfTest5L():"));
   	   		err = scanfTest5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KscanfTest6L)
   			{
   	   		INFO_PRINTF1(_L("scanfTest6L():"));
   	   		err = scanfTest6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KscanfTest7L)
   			{
   	   		INFO_PRINTF1(_L("scanfTest7L():"));
   	   		err = scanfTest7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KscanfTest8L)
   			{
   	   		INFO_PRINTF1(_L("scanfTest8L():"));
   	   		err = scanfTest8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KscanfTest9L)
   			{
   	   		INFO_PRINTF1(_L("scanfTest9L():"));
   	   		err = scanfTest9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KscanfTest10L)
   			{
   	   		INFO_PRINTF1(_L("scanfTest10L():"));
   	   		err = scanfTest10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KscanfTest11L)
   			{
   	   		INFO_PRINTF1(_L("scanfTest11L():"));
   	   		err = scanfTest11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KscanfTest12L)
   			{
   	   		INFO_PRINTF1(_L("scanfTest12L():"));
   	   		err = scanfTest12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KscanfTest13L)
   			{
   	   		INFO_PRINTF1(_L("scanfTest13L():"));
   	   		err = scanfTest13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsscanfTest1L)
   			{
   	   		INFO_PRINTF1(_L("sscanfTest1L():"));
   	   		err = sscanfTest1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsscanfTest2L)
   			{
   	   		INFO_PRINTF1(_L("sscanfTest2L():"));
   	   		err = sscanfTest2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsscanfTest3L)
   			{
   	   		INFO_PRINTF1(_L("sscanfTest3L():"));
   	   		err = sscanfTest3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsscanfTest4L)
   			{
   	   		INFO_PRINTF1(_L("sscanfTest4L():"));
   	   		err = sscanfTest4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsscanfTest5L)
   			{
   	   		INFO_PRINTF1(_L("sscanfTest5L():"));
   	   		err = sscanfTest5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsscanfTest6L)
   			{
   	   		INFO_PRINTF1(_L("sscanfTest6L():"));
   	   		err = sscanfTest6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsscanfTest7L)
   			{
   	   		INFO_PRINTF1(_L("sscanfTest7L():"));
   	   		err = sscanfTest7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsscanfTest8L)
   			{
   	   		INFO_PRINTF1(_L("sscanfTest8L():"));
   	   		err = sscanfTest8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsscanfTest9L)
   			{
   	   		INFO_PRINTF1(_L("sscanfTest9L():"));
   	   		err = sscanfTest9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsscanfTest10L)
   			{
   	   		INFO_PRINTF1(_L("sscanfTest10L():"));
   	   		err = sscanfTest10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsscanfTest11L)
   			{
   	   		INFO_PRINTF1(_L("sscanfTest11L():"));
   	   		err = sscanfTest11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsscanfTest12L)
   			{
   	   		INFO_PRINTF1(_L("sscanfTest12L():"));
   	   		err = sscanfTest12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsscanfTest13L)
   			{
   	   		INFO_PRINTF1(_L("sscanfTest13L():"));
   	   		err = sscanfTest13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KsscanfTest14L)
   			{
   	   		INFO_PRINTF1(_L("sscanfTest14L():"));
   	   		err = sscanfTest14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfscanfTest1L)
   			{
   	   		INFO_PRINTF1(_L("fscanfTest1L():"));
   	   		err = fscanfTest1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfscanfTest2L)
   			{
   	   		INFO_PRINTF1(_L("fscanfTest2L():"));
   	   		err = fscanfTest2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfscanfTest3L)
   			{
   	   		INFO_PRINTF1(_L("fscanfTest3L():"));
   	   		err = fscanfTest3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfscanfTest4L)
   			{
   	   		INFO_PRINTF1(_L("fscanfTest4L():"));
   	   		err = fscanfTest4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfscanfTest5L)
   			{
   	   		INFO_PRINTF1(_L("fscanfTest5L():"));
   	   		err = fscanfTest5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfscanfTest6L)
   			{
   	   		INFO_PRINTF1(_L("fscanfTest6L():"));
   	   		err = fscanfTest6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfscanfTest7L)
   			{
   	   		INFO_PRINTF1(_L("fscanfTest7L():"));
   	   		err = fscanfTest7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfscanfTest8L)
   			{
   	   		INFO_PRINTF1(_L("fscanfTest8L():"));
   	   		err = fscanfTest8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfscanfTest9L)
   			{
   	   		INFO_PRINTF1(_L("fscanfTest9L():"));
   	   		err = fscanfTest9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfscanfTest10L)
   			{
   	   		INFO_PRINTF1(_L("fscanfTest10L():"));
   	   		err = fscanfTest10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfscanfTest11L)
   			{
   	   		INFO_PRINTF1(_L("fscanfTest11L():"));
   	   		err = fscanfTest11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfscanfTest12L)
   			{
   	   		INFO_PRINTF1(_L("fscanfTest12L():"));
   	   		err = fscanfTest12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfscanfTest13L)
   			{
   	   		INFO_PRINTF1(_L("fscanfTest13L():"));
   	   		err = fscanfTest13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KfscanfTest14L)
   			{
   	   		INFO_PRINTF1(_L("fscanfTest14L():"));
   	   		err = fscanfTest14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvscanfTest1L)
   			{
   	   		INFO_PRINTF1(_L("vscanfTest1L():"));
   	   		err = vscanfTest1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvscanfTest2L)
   			{
   	   		INFO_PRINTF1(_L("vscanfTest2L():"));
   	   		err = vscanfTest2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvscanfTest3L)
   			{
   	   		INFO_PRINTF1(_L("vscanfTest3L():"));
   	   		err = vscanfTest3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvscanfTest4L)
   			{
   	   		INFO_PRINTF1(_L("vscanfTest4L():"));
   	   		err = vscanfTest4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvscanfTest5L)
   			{
   	   		INFO_PRINTF1(_L("vscanfTest5L():"));
   	   		err = vscanfTest5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvscanfTest6L)
   			{
   	   		INFO_PRINTF1(_L("vscanfTest6L():"));
   	   		err = vscanfTest6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvscanfTest7L)
   			{
   	   		INFO_PRINTF1(_L("vscanfTest7L():"));
   	   		err = vscanfTest7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvscanfTest8L)
   			{
   	   		INFO_PRINTF1(_L("vscanfTest8L():"));
   	   		err = vscanfTest8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvscanfTest9L)
   			{
   	   		INFO_PRINTF1(_L("vscanfTest9L():"));
   	   		err = vscanfTest9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvscanfTest10L)
   			{
   	   		INFO_PRINTF1(_L("vscanfTest10L():"));
   	   		err = vscanfTest10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvscanfTest11L)
   			{
   	   		INFO_PRINTF1(_L("vscanfTest11L():"));
   	   		err = vscanfTest11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvscanfTest12L)
   			{
   	   		INFO_PRINTF1(_L("vscanfTest12L():"));
   	   		err = vscanfTest12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvscanfTest13L)
   			{
   	   		INFO_PRINTF1(_L("vscanfTest13L():"));
   	   		err = vscanfTest13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsscanfTest1L)
   			{
   	   		INFO_PRINTF1(_L("vsscanfTest1L():"));
   	   		err = vsscanfTest1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KvsscanfTest2L)
   			{
   	   		INFO_PRINTF1(_L("vsscanfTest2L():"));
   	   		err = vsscanfTest2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KvsscanfTest3L)
   			{
   	   		INFO_PRINTF1(_L("vsscanfTest3L():"));
   	   		err = vsscanfTest3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KvsscanfTest4L)
   			{
   	   		INFO_PRINTF1(_L("vsscanfTest4L():"));
   	   		err = vsscanfTest4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KvsscanfTest5L)
   			{
   	   		INFO_PRINTF1(_L("vsscanfTest5L():"));
   	   		err = vsscanfTest5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KvsscanfTest6L)
   			{
   	   		INFO_PRINTF1(_L("vsscanfTest6L():"));
   	   		err = vsscanfTest6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KvsscanfTest7L)
   			{
   	   		INFO_PRINTF1(_L("vsscanfTest7L():"));
   	   		err = vsscanfTest7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KvsscanfTest8L)
			{
   	   		INFO_PRINTF1(_L("vsscanfTest8L():"));
   	   		err = vsscanfTest8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KvsscanfTest9L)
   			{
   	   		INFO_PRINTF1(_L("vsscanfTest9L():"));
   	   		err = vsscanfTest9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KvsscanfTest10L)
   			{
   	   		INFO_PRINTF1(_L("vsscanfTest10L():"));
   	   		err = vsscanfTest10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsscanfTest11L)
   			{
   	   		INFO_PRINTF1(_L("vsscanfTest11L():"));
   	   		err = vsscanfTest11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsscanfTest12L)
   			{
   	   		INFO_PRINTF1(_L("vsscanfTest12L():"));
   	   		err = vsscanfTest12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsscanfTest13L)
   			{
   	   		INFO_PRINTF1(_L("vsscanfTest13L():"));
   	   		err = vsscanfTest13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsscanfTest14L)
   			{
   	   		INFO_PRINTF1(_L("vsscanfTest14L():"));
   	   		err = vsscanfTest14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfscanfTest1L)
   			{
   	   		INFO_PRINTF1(_L("vfscanfTest1L():"));
   	   		err = vfscanfTest1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfscanfTest2L)
   			{
   	   		INFO_PRINTF1(_L("vfscanfTest2L():"));
   	   		err = vfscanfTest2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfscanfTest3L)
   			{
   	   		INFO_PRINTF1(_L("vfscanfTest3L():"));
   	   		err = vfscanfTest3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfscanfTest4L)
   			{
   	   		INFO_PRINTF1(_L("vfscanfTest4L():"));
   	   		err = vfscanfTest4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfscanfTest5L)
   			{
   	   		INFO_PRINTF1(_L("vfscanfTest5L():"));
   	   		err = vfscanfTest5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfscanfTest6L)
   			{
   	   		INFO_PRINTF1(_L("vfscanfTest6L():"));
   	   		err = vfscanfTest6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfscanfTest7L)
   			{
   	   		INFO_PRINTF1(_L("vfscanfTest7L():"));
   	   		err = vfscanfTest7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfscanfTest8L)
   			{
   	   		INFO_PRINTF1(_L("vfscanfTest8L():"));
   	   		err = vfscanfTest8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfscanfTest9L)
   			{
   	   		INFO_PRINTF1(_L("vfscanfTest9L():"));
   	   		err = vfscanfTest9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfscanfTest10L)
   			{
   	   		INFO_PRINTF1(_L("vfscanfTest10L():"));
   	   		err = vfscanfTest10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfscanfTest11L)
   			{
   	   		INFO_PRINTF1(_L("vfscanfTest11L():"));
   	   		err = vfscanfTest11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfscanfTest12L)
   			{
   	   		INFO_PRINTF1(_L("vfscanfTest12L():"));
   	   		err = vfscanfTest12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfscanfTest13L)
   			{
   	   		INFO_PRINTF1(_L("vfscanfTest13L():"));
   	   		err = vfscanfTest13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfscanfTest14L)
   			{
   	   		INFO_PRINTF1(_L("vfscanfTest14L():"));
   	   		err = vfscanfTest14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest1L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest1L():"));
   	   		err = vsprintfTest1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest2L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest2L():"));
   	   		err = vsprintfTest2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest3L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest3L():"));
   	   		err = vsprintfTest3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest4L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest4L():"));
   	   		err = vsprintfTest4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest5L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest5L():"));
   	   		err = vsprintfTest5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest6L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest6L():"));
   	   		err = vsprintfTest6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest7L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest7L():"));
   	   		err = vsprintfTest7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest8L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest8L():"));
   	   		err = vsprintfTest8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest9L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest9L():"));
   	   		err = vsprintfTest9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest10L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest10L():"));
   	   		err = vsprintfTest10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest11L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest11L():"));
   	   		err = vsprintfTest11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest12L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest12L():"));
   	   		err = vsprintfTest12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest13L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest13L():"));
   	   		err = vsprintfTest13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest14L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest14L():"));
   	   		err = vsprintfTest14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest15L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest15L():"));
   	   		err = vsprintfTest15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest16L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest16L():"));
   	   		err = vsprintfTest16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest17L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest17L():"));
   	   		err = vsprintfTest17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest18L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest18L():"));
   	   		err = vsprintfTest18L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest19L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest19L():"));
   	   		err = vsprintfTest19L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest20L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest20L():"));
   	   		err = vsprintfTest20L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest21L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest21L():"));
   	   		err = vsprintfTest21L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest22L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest22L():"));
   	   		err = vsprintfTest22L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest23L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest23L():"));
   	   		err = vsprintfTest23L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest24L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest24L():"));
   	   		err = vsprintfTest24L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest25L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest25L():"));
   	   		err = vsprintfTest25L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest26L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest26L():"));
   	   		err = vsprintfTest26L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsprintfTest27L)
   			{
   	   		INFO_PRINTF1(_L("vsprintfTest27L():"));
   	   		err = vsprintfTest27L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest1L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest1L():"));
   	   		err = vsnprintfTest1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest2L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest2L():"));
   	   		err = vsnprintfTest2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest3L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest3L():"));
   	   		err = vsnprintfTest3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest4L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest4L():"));
   	   		err = vsnprintfTest4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest5L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest5L():"));
   	   		err = vsnprintfTest5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest6L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest6L():"));
   	   		err = vsnprintfTest6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest7L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest7L():"));
   	   		err = vsnprintfTest7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest8L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest8L():"));
   	   		err = vsnprintfTest8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest9L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest9L():"));
   	   		err = vsnprintfTest9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest10L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest10L():"));
   	   		err = vsnprintfTest10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest11L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest11L():"));
   	   		err = vsnprintfTest11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest12L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest12L():"));
   	   		err = vsnprintfTest12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest13L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest13L():"));
   	   		err = vsnprintfTest13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest14L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest14L():"));
   	   		err = vsnprintfTest14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest15L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest15L():"));
   	   		err = vsnprintfTest15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest16L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest16L():"));
   	   		err = vsnprintfTest16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest17L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest17L():"));
   	   		err = vsnprintfTest17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest18L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest18L():"));
   	   		err = vsnprintfTest18L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest19L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest19L():"));
   	   		err = vsnprintfTest19L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest20L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest20L():"));
   	   		err = vsnprintfTest20L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest21L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest21L():"));
   	   		err = vsnprintfTest21L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest22L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest22L():"));
   	   		err = vsnprintfTest22L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest23L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest23L():"));
   	   		err = vsnprintfTest23L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest24L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest24L():"));
   	   		err = vsnprintfTest24L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest25L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest25L():"));
   	   		err = vsnprintfTest25L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest26L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest26L():"));
   	   		err = vsnprintfTest26L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvsnprintfTest27L)
   			{
   	   		INFO_PRINTF1(_L("vsnprintfTest27L():"));
   	   		err = vsnprintfTest27L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}

	else if(TestStepName() == KvfprintfTest1L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest1L():"));
   	   		err = vfprintfTest1L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest2L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest2L():"));
   	   		err = vfprintfTest2L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest3L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest3L():"));
   	   		err = vfprintfTest3L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KvfprintfTest4L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest4L():"));
   	   		err = vfprintfTest4L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest5L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest5L():"));
   	   		err = vfprintfTest5L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest6L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest6L():"));
   	   		err = vfprintfTest6L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest7L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest7L():"));
   	   		err = vfprintfTest7L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest8L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest8L():"));
   	   		err = vfprintfTest8L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest9L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest9L():"));
   	   		err = vfprintfTest9L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest10L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest10L():"));
   	   		err = vfprintfTest10L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest11L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest11L():"));
   	   		err = vfprintfTest11L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest12L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest12L():"));
   	   		err = vfprintfTest12L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest13L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest13L():"));
   	   		err = vfprintfTest13L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest14L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest14L():"));
   	   		err = vfprintfTest14L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest15L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest15L():"));
   	   		err = vfprintfTest15L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
  	else if(TestStepName() == KvfprintfTest16L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest16L():"));
   	   		err = vfprintfTest16L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KvfprintfTest17L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest17L():"));
   	   		err = vfprintfTest17L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest18L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest18L():"));
   	   		err = vfprintfTest18L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest19L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest19L():"));
   	   		err = vfprintfTest19L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest20L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest20L():"));
   	   		err = vfprintfTest20L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest21L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest21L():"));
   	   		err = vfprintfTest21L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest22L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest22L():"));
   	   		err = vfprintfTest22L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest23L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest23L():"));
   	   		err = vfprintfTest23L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest24L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest24L():"));
   	   		err = vfprintfTest24L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest25L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest25L():"));
   	   		err = vfprintfTest25L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest26L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest26L():"));
   	   		err = vfprintfTest26L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KvfprintfTest27L)
   			{
   	   		INFO_PRINTF1(_L("vfprintfTest27L():"));
   	   		err = vfprintfTest27L();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kfseek_multithreaded)
   			{
   	   		INFO_PRINTF1(_L("fseek_multithreaded():"));
   	   		err = fseek_multithreaded();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kprintf_multithreaded)
   			{
   	   		INFO_PRINTF1(_L("printf_multithreaded():"));
   	   		err = printf_multithreaded();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfreopen_multithreaded)
   			{
   	   		INFO_PRINTF1(_L("freopen_multithreaded():"));
   	   		err = freopen_multithreaded();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kputc_multithreaded)
   			{
   	   		INFO_PRINTF1(_L("putc_multithreaded():"));
   	   		err = putc_multithreaded();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KTestRemove3)
	   		{
	   	   	INFO_PRINTF1(_L("TestRemove3():"));
	   	   	err = TestRemove3();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}
	else if(TestStepName() == KTestRemove4)
	   		{
	   	   	INFO_PRINTF1(_L("TestRemove4():"));
	   	   	err = TestRemove4();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}
	else if(TestStepName() == KTestRemove5)
	   		{
	   	   	INFO_PRINTF1(_L("TestRemove5():"));
	   	   	err = TestRemove5();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}
	else if(TestStepName() == KTestRemove6)
	   		{
	   	   	INFO_PRINTF1(_L("TestRemove6():"));
	   	   	err = TestRemove6();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}
	else if(TestStepName() == KTestRename2)
	   		{
	   	   	INFO_PRINTF1(_L("TestRename2():"));
	   	   	err = TestRename2();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}	
	else if(TestStepName() == KTestsprintf1)
	   		{
	   	   	INFO_PRINTF1(_L("Testsprintf1():"));
	   	   	err = Testsprintf1();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}
	else if(TestStepName() == KInvalidDriveTests29)
   			{
   	   		INFO_PRINTF1(_L("InvalidDriveTests29():"));
   	   		err = InvalidDriveTests29();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KputcTest)
	   		{
	   	   	INFO_PRINTF1(_L("putcTest():"));
	   	   	err = putcTest();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}
	else if(TestStepName() == KclrerrTest)
	   		{
	   	   	INFO_PRINTF1(_L("clrerrTest:"));
	   	   	err = clrerrTest();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}
	else if(TestStepName() == KgetcTest)
	   		{
	   	   	INFO_PRINTF1(_L("getcTest:"));
	   	   	err = getcTest();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}  
	 else if(TestStepName() == KfilenoTest)
	   		{
	   	   	INFO_PRINTF1(_L("filenoTest"));
	   	   	err = filenoTest();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}
	 else if(TestStepName() == KfeofTest)
	   		{
	   	   	INFO_PRINTF1(_L("feofTest"));
	   	   	err = feofTest();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	      	} 
	else if(TestStepName()==KvasprintfTest1)
	   		{
	   	   	INFO_PRINTF1(_L("vasprintfTest1"));
	   	   	err = vasprintfTest1();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	     	 	} 
	else if(TestStepName() == KasprintfTest1)
			{
	   		INFO_PRINTF1(_L("asprintfTest1"));
	   		err = asprintfTest1();
	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);		
			}
	else if(TestStepName()==Kposix_spawnattr_destroyTest)
	   		{
	   	   	INFO_PRINTF1(_L("posix_spawnattr_destroyTest"));
	   	   	err = posix_spawnattr_destroyTest();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	      	}
	else if(TestStepName()==KsetgroupsTest)
	   		{
	   	   	INFO_PRINTF1(_L("setgroupsTest"));
	   	   	err = setgroupsTest();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	      	}
	else if(TestStepName()==KgetpwentTest)
	   		{
	   	   	INFO_PRINTF1(_L("getpwentTest"));
	   	   	err = getpwentTest();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	      	}
	else if(TestStepName()==KbrkTest)
	   		{
	   	   	INFO_PRINTF1(_L("brkTest"));
	   	   	err = brkTest();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	      	}
	else if(TestStepName()==KsigactionTest)
	   		{
	   	   	INFO_PRINTF1(_L("sigactionTest"));
	   	   	err = sigactionTest();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	      	}
	else if(TestStepName()==KwfreopenTest)
	   		{
	   	   	INFO_PRINTF1(_L("wfreopenTest"));
	   	   	err = wfreopenTest();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	      		}
	else if(TestStepName()==KwopenTest)
	   		{
	   	   	INFO_PRINTF1(_L("wopenTest"));
	   	   	err = wopenTest();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	      		}
	else if(TestStepName()==Kfseek_negTest)
		   		{
		   	   	INFO_PRINTF1(_L("fseek_negTest"));
		   	   	err = fseek_negTest();
		   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		      	}
	else if(TestStepName()==Klseek_negTest)
			   	{
			   	INFO_PRINTF1(_L("lseek_negTest"));
			   	err = lseek_negTest();
			   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			    }
	else if(TestStepName()==Kfwrite4)
	   	{
	   	INFO_PRINTF1(_L("fwrite4"));
	   	err = fwrite4();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	    }
	else if(TestStepName()==KftellTest)
		{
		INFO_PRINTF1(_L("ftellTest"));
		err = ftellTest();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == Kfgets1)
	   	{
	   	INFO_PRINTF1(_L("fgets1():"));
	   	err = fgets1();	
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Kfgets2)
	   	{
	   	INFO_PRINTF1(_L("fgets2():"));
	   	err = fgets2();	
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Kfgets3)
	   	{
	   	INFO_PRINTF1(_L("fgets3():"));
	   	err = fgets3();	
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Kbinaryopen)
	   	{
	   	INFO_PRINTF1(_L("binaryopen():"));
	   	err = binaryopen();	
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Kbinaryfopen)
	   	{
	   	INFO_PRINTF1(_L("binaryfopen():"));
	   	err = binaryfopen();	
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Kfreopen_validmode)
	   		{
	   	   	INFO_PRINTF1(_L("freopen_validmode():"));
	   	   	err1 = freopen_validmode();
	   	   	return err1;
	   	   	}
	else if(TestStepName() == Kfreopen_invalidmode)
	   		{
	   	   	INFO_PRINTF1(_L("freopen_invalidmode():"));
	   	   	err1 = freopen_invalidmode();
	   	   	return err1;
	   	   	}
	else if(TestStepName() == KTestFileModeAccess1)
	   	{
	   	INFO_PRINTF1(_L("TestFileModeAccess1():"));
	   	err1 = TestFileModeAccess1();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileModeAccess2)
	   	{
	   	INFO_PRINTF1(_L("TestFileModeAccess2():"));
	   	err1 = TestFileModeAccess2();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileModeAccess3)
	   	{
	   	INFO_PRINTF1(_L("TestFileModeAccess3():"));
	   	err1 = TestFileModeAccess3();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileModeAccess4)
	   	{
	   	INFO_PRINTF1(_L("TestFileModeAccess4():"));
	   	err1 = TestFileModeAccess4();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileModeAccess5)
	   	{
	   	INFO_PRINTF1(_L("TestFileModeAccess5():"));
	   	err1 = TestFileModeAccess5();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileModeAccess6)
	   	{
	   	INFO_PRINTF1(_L("TestFileModeAccess6():"));
	   	err1 = TestFileModeAccess6();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileModeAccess7)
	   	{
	   	INFO_PRINTF1(_L("TestFileModeAccess7():"));
	   	err1 = TestFileModeAccess7();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileGetsPos)
	   	{
	   	INFO_PRINTF1(_L("TestFileGetsPos():"));
	   	err1 = TestFileGetsPos();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileTellReadWrite)
	   	{
	   	INFO_PRINTF1(_L("TestFileTellReadWrite():"));
	   	err1 = TestFileTellReadWrite();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileTellSeek)
	   	{
	   	INFO_PRINTF1(_L("TestFileTellSeek():"));
	   	err1 = TestFileTellSeek();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileLargeData)
	   	{
	   	INFO_PRINTF1(_L("TestFileLargeData():"));
	   	err1 = TestFileLargeData();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileReadTell)
	   	{
	   	INFO_PRINTF1(_L("TestFileReadTell():"));
	   	err1 = TestFileReadTell();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileReadSeekEnd)
	   	{
	   	INFO_PRINTF1(_L("TestFileReadSeekEnd():"));
	   	err1 = TestFileReadSeekEnd();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileWriteSeekEnd)
	   	{
	   	INFO_PRINTF1(_L("TestFileWriteSeekEnd():"));
	   	err1 = TestFileWriteSeekEnd();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileFlushWriteSeekEnd)
	   	{
	   	INFO_PRINTF1(_L("TestFileFlushWriteSeekEnd():"));
	   	err1 = TestFileFlushWriteSeekEnd();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileFlushWriteSeek)
	   	{
	   	INFO_PRINTF1(_L("TestFileFlushWriteSeek():"));
	   	err1 = TestFileFlushWriteSeek();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileFlushSeekCur)
	   	{
	   	INFO_PRINTF1(_L("TestFileFlushSeekCur():"));
	   	err1 = TestFileFlushSeekCur();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileTellSeekCornerCase1)
	   	{
	   	INFO_PRINTF1(_L("TestFileTellSeekCornerCase1():"));
	   	err1 = TestFileTellSeekCornerCase1();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileTellSeekCornerCase2)
	   	{
	   	INFO_PRINTF1(_L("TestFileTellSeekCornerCase2():"));
	   	err1 = TestFileTellSeekCornerCase2();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileSeekTellOnRW1)
	   	{
	   	INFO_PRINTF1(_L("TestFileSeekTellOnRW1():"));
	   	err1 = TestFileSeekTellOnRW1();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileSeekTellOnRW2)
	   	{
	   	INFO_PRINTF1(_L("TestFileSeekTellOnRW2():"));
	   	err1 = TestFileSeekTellOnRW2();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileSeekTellOnRW3)
	   	{
	   	INFO_PRINTF1(_L("TestFileSeekTellOnRW3():"));
	   	err1 = TestFileSeekTellOnRW3();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileSeekTellOnRW4)
	   	{
	   	INFO_PRINTF1(_L("TestFileSeekTellOnRW4():"));
	   	err1 = TestFileSeekTellOnRW4();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileWriteTextMode)
	   	{
	   	INFO_PRINTF1(_L("TestFileWriteTextMode():"));
	   	err1 = TestFileWriteTextMode();	
	   	return err1;
	   	}
	else if(TestStepName() == KTestFileWriteBinaryMode)
	   	{
	   	INFO_PRINTF1(_L("TestFileWriteBinaryMode():"));
	   	err1 = TestFileWriteBinaryMode();	
	   	return err1;
	   	}
	return TestStepResult(); 

	}
 
