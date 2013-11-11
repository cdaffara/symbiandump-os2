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
// Name        : tlocaleblocks.cpp
//



#include <math.h> //round()
#include "tlocale.h"
#include <e32std_private.h>
#define MAXVAL 128
#define MAXVAL1 132
#define MAX  64


/* Ctype api's affeted by locale */
TInt CTestLocale::ReadStringAndSetlocale()
	{
	TPtrC langname;
	TInt category;
	TPtrC string;
	if(GetIntFromConfig(ConfigSection(), _L("category"), category) == 0)
		{
		ERR_PRINTF1(_L("Reading category from .ini fails"));
		return KErrGeneral;
		}
	if(GetStringFromConfig(ConfigSection(),_L("langname"), langname) == 0 )
		{
		ERR_PRINTF1(_L("Reading langname from .ini fails"));
		return KErrGeneral;
		}
	TBuf8<200> buf;
	buf.Copy(langname);
	char* inputstr = (char*) buf.Ptr();
	TInt len=buf.Length();
	inputstr[len]='\0';
	if(setlocale(category ,inputstr) == NULL)
		{
	    _LIT(KErr,"setlocale returns NULL %s");
	    ERR_PRINTF2(KErr,inputstr);
	    return KErrGeneral;
	    }
	return KErrNone;
	}

TBool CTestLocale::CompareString_WcharChar(wchar_t *aString1, char *aString2)
	{
	TInt i;
	TInt len = wcslen(aString1);
	for ( i = 0; ( (aString1[i]!=0) && (aString2[i]!=0) && (aString2[i] == aString1[i]) ); i++)	{} // Go through the strings
	if ( aString2[i] != aString1[i] )	// If the scanning ended because of a difference between the strings
		{
		return EFalse;
		}
	else	
		{
		return ETrue;
		}
	}

/* -----------------------------------------------------------------------------
 *Function Name :TestctypeApi
 *API Tested :isalpha(),isalnum(),isdigit(),ispunct(),isupper(),islower(),isxdigit(),toupper(),tolower(),isspace(),iscntrl(),isprint(),isgraph()
 *TestCase Description:Testing the behaviour of the above Api'ss for ascii values 0-127 
 *after setting locale category to LC_CTYPE.
 * -----------------------------------------------------------------------------
 */   
TVerdict CTestLocale::TestctypeApi(  )
    {
    SetTestStepResult(EFail);
    TInt result[MAXVAL];
    TInt expected[MAXVAL];
    if(ReadStringAndSetlocale() != 0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TInt count = 0;
    TInt Case;
    if(!GetIntFromConfig(ConfigSection(), _L("Case"),Case ))
    	{
    	ERR_PRINTF1(_L("Reading case from .ini fails"));
    	return TestStepResult();
    	}
    TBuf<16> inputBuf;
    TInt AsciiChar;
    _LIT( KAsciiChar, "AsciiChar%d" );
    inputBuf.Format(KAsciiChar,count);
    switch(Case)
    	{
    	case 0:
    	while (GetIntFromConfig(ConfigSection(), inputBuf, AsciiChar ))
    		{
    		result[count] = isalpha(AsciiChar);
    		inputBuf.Format(KAsciiChar,++count);
    		}
    	break;
    	case 1:
    	count = 0;
    	while (GetIntFromConfig(ConfigSection(), inputBuf, AsciiChar ))
    		{
    		result[count] = isalnum(AsciiChar);
    		inputBuf.Format(KAsciiChar,++count);
    		}
    	break;
    	case 2:
    	count = 0;
    	while (GetIntFromConfig(ConfigSection(), inputBuf, AsciiChar ))
    		{
    		result[count] = isdigit(AsciiChar);
    		inputBuf.Format(KAsciiChar,++count);
    		}
    	break;
    	case 3:
    	count = 0;
    	while (GetIntFromConfig(ConfigSection(), inputBuf, AsciiChar ))
    		{
    		result[count] = ispunct(AsciiChar);
    		inputBuf.Format(KAsciiChar,++count);
    		}
    	break;
    	case 4:
    	count = 0;
    	while (GetIntFromConfig(ConfigSection(), inputBuf, AsciiChar ))
    		{
    		result[count] = isupper(AsciiChar);
    		inputBuf.Format(KAsciiChar,++count);
    		}
    	break;
    	case 5:
    	count = 0;
    	while (GetIntFromConfig(ConfigSection(), inputBuf, AsciiChar ))
    		{
    		result[count] = islower(AsciiChar);
    		inputBuf.Format(KAsciiChar,++count);
    		}
    	break;
    	case 6:
    	count = 0;
    	while (GetIntFromConfig(ConfigSection(), inputBuf, AsciiChar ))
    		{
    		result[count] = isxdigit(AsciiChar);
    		inputBuf.Format(KAsciiChar,++count);
    		}
    	break;
    	case 7:
    	count = 0;
    	while (GetIntFromConfig(ConfigSection(), inputBuf, AsciiChar ))
    		{
    		result[count] = toupper(AsciiChar);
    		inputBuf.Format(KAsciiChar,++count);
    		}
    	break;
    	case 8:
    	count = 0;
    	while (GetIntFromConfig(ConfigSection(), inputBuf, AsciiChar ))
    		{
    		result[count] = tolower(AsciiChar);
    		inputBuf.Format(KAsciiChar,++count);
    		}
    	break;
    	case 9:
    	count = 0;
    	while (GetIntFromConfig(ConfigSection(), inputBuf, AsciiChar ))
    		{
    		result[count] = isspace(AsciiChar);
    		inputBuf.Format(KAsciiChar,++count);
    		}
    	break;
    	case 10:
    	count = 0;
    	while (GetIntFromConfig(ConfigSection(), inputBuf, AsciiChar ))
    		{
    		result[count] = iscntrl(AsciiChar);
    		inputBuf.Format(KAsciiChar,++count);
    		}
    	break;
    	case 11:
    	count = 0;
    	while (GetIntFromConfig(ConfigSection(), inputBuf, AsciiChar ))
    		{
    		result[count] = isprint(AsciiChar);
    		inputBuf.Format(KAsciiChar,++count);
    		}
    	break;
    	case 12:
    	count = 0;
    	while (GetIntFromConfig(ConfigSection(), inputBuf, AsciiChar ))
    		{
    		result[count] = isgraph(AsciiChar);
    		inputBuf.Format(KAsciiChar,++count);
    		}
    	default:
    	break;
    	}
    
    TInt ExpResult;
    TBuf<16> expBuf;
    _LIT(KExpResult, "ExpResult%d" );
    count = 0;
    expBuf.Format(KExpResult,count);
    while (GetIntFromConfig(ConfigSection(),expBuf,ExpResult))
    	{
        expected[count] = ExpResult;
        expBuf.Format(KExpResult,++count);
        }
     for (count = 0 ;count<MAXVAL;count++)
    	{
        if(result[count] != expected[count])
        	{
            ERR_PRINTF4(_L("Error at Position : %d ,ExpResult : %d ,Result : %d"),count,expected[count],result[count]);
            return TestStepResult();     
            }
        else
    	    {
            //INFO_PRINTF3(_L("ExpResult : %d ,Result : %d"),expected[count],result[count]);
            } 
        }
    SetTestStepResult(EPass);
    return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testisascii
 *API Tested :isascii()
 *TestCase Description:Testing the behaviour of isascii() for ascii values 0-127 and some bad parameter 
 *after setting locale category to LC_CTYPE.
 * -----------------------------------------------------------------------------
 */   

TVerdict CTestLocale::Testisascii( )
    {
    SetTestStepResult(EFail);
    TInt expected[MAXVAL1];
    TInt result[MAXVAL1];
    if(ReadStringAndSetlocale() !=0 )
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TInt AsciiChar,ExpResult;
    TBuf<16> expBuf;
    _LIT( KExpResult, "ExpResult%d" );
    TInt count = 0;
    expBuf.Format(KExpResult,count);
    while (GetIntFromConfig(ConfigSection(),expBuf,ExpResult))
       	{
        expected[count] = ExpResult;
        expBuf.Format(KExpResult,++count);
     	}
    count = 0;
    TBuf<16> inputBuf;
    _LIT( KAsciiChar, "AsciiChar%d" );
    inputBuf.Format(KAsciiChar,count);
    while (GetIntFromConfig(ConfigSection(), inputBuf, AsciiChar ))
       	{
        result[count] = isascii(AsciiChar);
        inputBuf.Format(KAsciiChar,++count);
        }
    for (count = 0 ;count<MAXVAL;count++)
    	{
        if(result[count] != expected[count])
        	{
            ERR_PRINTF4(_L("Error at Position : %d ,ExpResult : %d ,Result : %d"),count,expected[count],result[count]);
            return TestStepResult();	
            }
        else
        	{
            //INFO_PRINTF3(_L("ExpResult : %d ,Result : %d"),expected[count],result[count]);
            } 
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testtoascii
 *API Tested :toascii
 *TestCase Description:testing the behaviour of toascii() for ascii values 0-127 and some bad parameter .
 *after setting locale category to LC_CTYPE.
 * -----------------------------------------------------------------------------
 */   

TVerdict CTestLocale::Testtoascii( )
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() != 0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TInt AsciiChar;
    TBuf<16> InputBuf;
    TInt count = 0;
    _LIT(KAsciiChar, "AsciiChar%d");
    InputBuf.Format(KAsciiChar,count);
    while (GetIntFromConfig(ConfigSection(), InputBuf, AsciiChar ))
        {
        TInt retVal = toascii(AsciiChar);
        TInt ExpResult = AsciiChar & 0x7f;
      	if(retVal!= ExpResult)
           	{
            ERR_PRINTF4(_L("Error for AsciiChar: %d ,ExpResult : %d ,Result : %d"),AsciiChar,ExpResult,retVal);
            return TestStepResult();		
            }
        else
            {
            //INFO_PRINTF3(_L("ExpResult : %d ,Result : %d"),ExpResult,retVal);
            } 
      	InputBuf.Format(KAsciiChar,++count);
        }
    SetTestStepResult(EPass);
    return TestStepResult();
    }

/* Multibyte and widecharacter api's affeted by locale */


/* -----------------------------------------------------------------------------
 *Function Name :Testmblen0
 *API Tested :mblen()
 *TestCase Description:Multibyte char length -> 2 (with n = 2 and n = 0)
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testmblen0()
    {
    SetTestStepResult(EFail);
    char mbc[MB_CUR_MAX];
    if(ReadStringAndSetlocale() !=0 )
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    wchar_t wc = (unsigned short)0x0509;
    if(wctomb( mbc, wc ) == -1)
    	{
    	return TestStepResult();
    	}
    int retVal1 = mblen( mbc, MB_CUR_MAX );
	int retVal2 = mblen( mbc, 0 );
   	INFO_PRINTF3(_L("{Expected: 2, 0} %d %d"), retVal1, retVal2);
	if (retVal1 != 2 || retVal2 != 0)
		{
		ERR_PRINTF1(_L("Return value is not as expected"));
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	return TestStepResult();
    }


/* -----------------------------------------------------------------------------
 *Function Name :Testmblen1
 *API Tested :mblen()
 *TestCase Description:Multibyte char length -> 1 (with n = 2 and n = 0)
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testmblen1()
    {
    SetTestStepResult(EFail);
    wchar_t wc = L'a';
	char mbc[MB_CUR_MAX];
    if(ReadStringAndSetlocale() != 0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
	if(wctomb( mbc, wc ) == -1)
		{
		return TestStepResult();
		}
    int retVal1 = mblen( mbc, MB_CUR_MAX );
	int retVal2 = mblen( mbc, 0 );
	INFO_PRINTF3(_L("{Expected: 1, 0} %d %d"), retVal1, retVal2);
	if (retVal1 != 1 || retVal2 != 0)
		{
		ERR_PRINTF1(_L("Return value is not as expected"));
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	return TestStepResult();
    }
/* -----------------------------------------------------------------------------
 *Function Name :Testmblen2
 *API Tested :mblen
 *TestCase Description:mblen returns -> 0 when the input is a NULL char 
 *(with n = 2 and n = 0)
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testmblen2()
    {
    SetTestStepResult(EFail);
    wchar_t wc = L'\0';
	char mbc[MB_CUR_MAX];
    if(ReadStringAndSetlocale() != 0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
	if(wctomb( mbc, wc ) == -1)
		{
		return TestStepResult();
		}
    int retVal1 = mblen( mbc, MB_CUR_MAX );
	int retVal2 = mblen( mbc, 0 );
	INFO_PRINTF3(_L("{Expected: 0, 0} %d %d"), retVal1, retVal2);
	if (retVal1 != 0 || retVal2 != 0)
		{
		ERR_PRINTF1(_L("Return value is not as expected"));
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	return TestStepResult();
    }
/* -----------------------------------------------------------------------------
 *Function Name :Testmblen3
 *API Tested :mblen()
 *TestCase Description:mblen returns -> -1 (no complete multibyte character)
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testmblen3()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() != 0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
   	char mbc[MB_CUR_MAX];
	wchar_t wc = (unsigned short)0xff09; 
    if(wctomb( mbc, wc )== -1)
    	{
    	ERR_PRINTF1(_L("wctomb() fails"));
    	return TestStepResult();
    	}
    int retVal1 = mblen( mbc, 1 );
	int retVal2 = mblen( mbc, 0 );
	INFO_PRINTF3(_L("{Expected: -1, 0} %d %d"), retVal1, retVal2);
	if (retVal1 != -1 || retVal2 != 0 )
		{
		ERR_PRINTF1(_L("Return value is not as expected"));
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testmblen4
 *API Tested :mblen
 *TestCase Description:mblen returns -> 0 (NULL srting as input)
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testmblen4()
    {
    SetTestStepResult(EFail);
    if( ReadStringAndSetlocale() !=0 )
	    {
	    ERR_PRINTF1(_L("Set locale fails"));
	    return TestStepResult();
	    }
    char  *pmbc = NULL;
    int retVal1 = mblen( pmbc, MB_CUR_MAX );
	int retVal2 = mblen( pmbc, 0 );
	INFO_PRINTF3(_L("{Expected: 0, 0} %d %d"), retVal1, retVal2);
	if (retVal1 != 0 || retVal2 != 0)
		{
		ERR_PRINTF1(_L("Return value is not as expected"));
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	return TestStepResult();
    }
/* -----------------------------------------------------------------------------
 *Function Name :Testwctomb0
 *API Tested :wctomb()
 *TestCase Description:wctomb returns -> 0 (length of the multibyte character)
 *when the input wide character occupies 0 bytes after encoding it as a
 *multibyte character.
 *input --> wide char could be anything but the MB char buffer is NULL
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwctomb0()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() != 0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    int retVal = wctomb(NULL, L'A');              
    INFO_PRINTF2(_L("{Expected: 0} %d"), retVal);
	if (retVal != 0)
		{
		ERR_PRINTF1(_L("Return value is not as expected"));
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testwctomb1
 *API Tested :wctomb
 *TestCase Description: wctomb returns -> 1 (length of the multibyte character)
 *when the input wide character occupies 1 byte after encoding it as a
 *multibyte character.
 *input --> wide char '\0'
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwctomb1()
    {
    SetTestStepResult(EFail);
    char buf[MB_CUR_MAX];
	unsigned long lmask = 0x000000ff;
    if(ReadStringAndSetlocale() != 0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
	memset(buf, 0xcc, sizeof(buf));
	int retVal = wctomb(buf, L'\0');              
    INFO_PRINTF4(_L("{Expected: 1} %d %x %x"), retVal, buf[0], buf[1]);
	if (retVal != 1 || buf[0] != 0 || (buf[1] & lmask) != 0xcc)
		{
		ERR_PRINTF1(_L("Return value is not as expected"));
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testwctomb2
 *API Tested :wctomb()
 *TestCase Description:wctomb returns -> 3 (length of the multibyte character)
 *when the input MB char was an invalid MB sequence...in this case some 
 *static conversion is done.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwctomb2()
    {
    SetTestStepResult(EFail);
    char buf[MB_CUR_MAX];
	unsigned long lmask = 0x000000ff;
	if(ReadStringAndSetlocale() != 0)
		{
		ERR_PRINTF1(_L("Set locale fails"));
		return TestStepResult();
		}
	memset(buf, 0xcc, sizeof(buf));
	int retVal = wctomb(buf, 0xa3c1);              
    INFO_PRINTF5(_L("{Expected: 3, ea, 8f, 81} %d %x %x %x"), retVal, buf[0], buf[1], buf[2]);
	if (retVal != 3 || (buf[0] & lmask) != 0xea || (buf[1] & lmask) != 0x8f 
	             || (buf[2] & lmask) != 0x81)
		{
		ERR_PRINTF1(_L("Return value is not as expected"));
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	return TestStepResult();
    }
/* -----------------------------------------------------------------------------
 *Function Name :Testwctomb3
 *API Tested :wctomb()
 *TestCase Description:wctomb returns -> -1 when the input MB char was an invalid MB sequence.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwctomb3()
    {
    SetTestStepResult(EFail);
   	char mbc[MB_CUR_MAX];
    if(ReadStringAndSetlocale() != 0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
	wchar_t wc = (unsigned short)0xFFFF;
	int retVal = wctomb( mbc, wc ); 
	INFO_PRINTF2(_L("return value is %d") ,retVal);
	if(retVal != -1)
		{
		ERR_PRINTF1(_L("Return value is not as expected"));
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testmbtowc0
 *API Tested :mbtowc()
 *TestCase Description:mbtowc(wchar , mbc,n) is called 
 * with mbc as a null pointer 0r '\0' or invalid char sequence after changing the programs current locale using setlocale() with different possible option.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testmbtowc0()
    {
    SetTestStepResult(EFail);
    wchar_t wc;
    if(ReadStringAndSetlocale() != 0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
	int retVal1 = mbtowc(&wc, NULL, 3); 
	int retVal2 = mbtowc(&wc, '\0', 3);
	char buf[MB_CUR_MAX];
	memset(buf, 0xcc, sizeof(buf));
	wchar_t wc1 = L'z';
	buf[0] = (unsigned char)0xc2;
	buf[1] = (unsigned char)0x42;
	int retVal3 = mbtowc(&wc1, buf, 2); 
	INFO_PRINTF5(_L("Expected: 0 0 -1 and Result: %d %d %d , errno: %d"), retVal1,retVal2,retVal3,errno);
	if (retVal1 != 0 || retVal2 != 0 || retVal3 != -1 || errno != EILSEQ)
		{
		ERR_PRINTF1(_L("Return value is not as expected"));
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	return TestStepResult();
    }
/* -----------------------------------------------------------------------------
 *Function Name :Testmbtowc1
 *API Tested :mbtowc()
 *TestCase Description:Call mbtowc(wchar , mbc,n)  
 * where mbc = "a" and n = 1 after changing the programs current locale  
 * using setlocale() with different possible option.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testmbtowc1()
    {
    SetTestStepResult(EPass);
    if(ReadStringAndSetlocale() !=0 )
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
	TInt size;
	if(GetIntFromConfig(ConfigSection(), _L("size"), size) == 0)
		{
		ERR_PRINTF1(_L("Return value is not as expected"));
		return TestStepResult();
		}
	TInt expected;
	if(GetIntFromConfig(ConfigSection(), _L("expected"), expected) == 0)
		{
		return TestStepResult();
		}
	TPtrC src;
	if(GetStringFromConfig(ConfigSection(),_L("src"), src) == 0)
		{
		return TestStepResult();
		}
	TBuf8<200> buf;
	buf.Copy(src);
	char* inputStr = (char*) buf.Ptr();
	TInt length=buf.Length();
	inputStr[length]='\0';
	wchar_t wc;
	int retVal = mbtowc(&wc, inputStr, size); 
	if (retVal != expected)
		{
		ERR_PRINTF1(_L("Return value is not equal to expected result"));
		ERR_PRINTF3(_L("Expected: %d  and Result: %d" ),expected, retVal );
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testmbstowcs
 *API Tested :mbstowcs()
 *TestCase Description:To check the behaviour of mbstowcs() with different locale settings.
 * -----------------------------------------------------------------------------
 */ 

TVerdict CTestLocale::Testmbstowcs()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0 )
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}	
	TInt retVal;
	TPtrC mbcbuf;	
	if(GetStringFromConfig(ConfigSection(),_L("mbcbuf"), mbcbuf) == 0)
		{
		return TestStepResult();
		}
	TBuf8<200> cbuf;
	cbuf.Copy(mbcbuf);
	char* cinputStr = (char*) cbuf.Ptr();
	TInt clength=cbuf.Length();
	cinputStr[clength]='\0';
	
	TPtrC wcbuf;
	if(GetStringFromConfig(ConfigSection(),_L("wcbuf"), wcbuf) == 0)
		{
		ERR_PRINTF1(_L("Reading wcbuf value from ini fails"));
		return TestStepResult();
		}
	TBuf<200> wbuf;	
	wbuf.Copy(wcbuf);
	wchar_t * winputStr = (wchar_t*)wbuf.Ptr();
	TInt wlength=wbuf.Length();
	winputStr[wlength]='\0';
	
	TInt expected;
	if(GetIntFromConfig(ConfigSection(), _L("expected"), expected) == 0)
		{
		ERR_PRINTF1(_L("Reading expected value fails"));
		return TestStepResult();
		}
	TInt size;
	if(GetIntFromConfig(ConfigSection(), _L("size"), size) == 0)
		{
		ERR_PRINTF1(_L("Reading size value fails"));
		return TestStepResult();
		}	
	if(strcmp(cinputStr,"NULL") == 0)
		{
		retVal = mbstowcs(winputStr, NULL, size);
		}
	else
		{
		if(CompareString_WcharChar(winputStr,"NULL") != 0)
			{
			retVal = mbstowcs(NULL, cinputStr, size);
			}
		else
			{
			retVal = mbstowcs(winputStr, cinputStr, size);
			}
		}
	if (retVal != expected)
		{
		ERR_PRINTF1(_L("Return value is not equal to expected result"));
		ERR_PRINTF3(_L("Expected: %d and  Result: %d"),expected, retVal);
		return TestStepResult();
		}
   	SetTestStepResult(EPass);
   	return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testwcstombs
 *API Tested :wcstombs()
 *TestCase Description:To check the behaviour of wcstombs() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcstombs()
    {
    SetTestStepResult(EFail);
    if( ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}	
	TInt retVal;
	TPtrC mbcbuf;
	if(GetStringFromConfig(ConfigSection(),_L("mbcbuf"), mbcbuf)== 0)
		{
		return TestStepResult();
		}
	TBuf8<200> cbuf;
	cbuf.Copy(mbcbuf);
	char* cinputStr = (char*) cbuf.Ptr();
	TInt clength=cbuf.Length();
	cinputStr[clength]='\0';
	TPtrC wcbuf;
	if(GetStringFromConfig(ConfigSection(),_L("wcbuf"), wcbuf) == 0)
		{
		ERR_PRINTF1(_L("Reading wcbuf value fails"));
		return TestStepResult();
		}
	TBuf<200> wbuf;
	wbuf.Copy(wcbuf);
	wchar_t * winputStr = (wchar_t*)wbuf.Ptr();
	TInt wlength=wbuf.Length();
	winputStr[wlength]='\0';
	
	TInt expected;
	if(GetIntFromConfig(ConfigSection(), _L("expected"), expected) == 0)
		{
		ERR_PRINTF1(_L("Reading expected value fails"));
		return TestStepResult();
		}
	TInt size;
	if(GetIntFromConfig(ConfigSection(), _L("size"), size) == 0)
		{
		ERR_PRINTF1(_L("Reading size value fails"));
		return TestStepResult();
		}
	if(strcmp(cinputStr,"NULL") == 0)
		{
		retVal = wcstombs( NULL,winputStr, size);
		}
	else
		{
		if(CompareString_WcharChar(winputStr,"NULL") != 0)
			{
			retVal = wcstombs(cinputStr,NULL, size);
			}
		else
			{
			retVal = wcstombs(cinputStr,winputStr, size);
			}
		}
	INFO_PRINTF3(_L("Expected: %d and  Result: %d"),expected, retVal);
   	if (retVal != expected)
		{
		ERR_PRINTF1(_L("Return value is not equal to expected result"));
		return TestStepResult();
		}
   	SetTestStepResult(EPass);
   	return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testwcrtomb0
 *API Tested :wcrtomb()
 *TestCase Description:To check the behaviour of wcrtomb() when destination buffer is NULL with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcrtomb0()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale()!=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TInt wc;
  	if(GetIntFromConfig(ConfigSection(),_L("wc"), wc) == 0)
    	{
    	ERR_PRINTF1(_L("Reading wc value fails"));
    	return TestStepResult();
    	}
  	TInt expected;
    if(GetIntFromConfig(ConfigSection(), _L("expected"), expected) == 0)
    	{
    	ERR_PRINTF1(_L("Reading expected value fails"));
    	return TestStepResult();
    	}
    mbstate_t state;
    TInt retVal = wcrtomb(NULL, wc, &state);
    if(retVal != expected)
    	{
    	ERR_PRINTF1(_L("Return value is not equal to expected result"));
    	ERR_PRINTF3(_L("Expected: %d and  Result: %d"),expected, retVal);
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testwcrtomb1
 *API Tested :wcrtomb()
 *TestCase Description:To check the behaviour of wcrtomb() when destination buffer is not null with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcrtomb1()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale()!=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    mbstate_t state;
    wchar_t wc;
  	char mbBuff[MB_CUR_MAX+1];
 	for(short i =0; i< 0x80; i++)
		{
		if(wcrtomb(NULL, L'\0', &state) != 1)
			{
			ERR_PRINTF1(_L("wcrtomb() fails"));
			return TestStepResult();
			}
		wc = (wchar_t)i;
		size_t retVal = wcrtomb(mbBuff, wc, &state);
		if(retVal != 1 || mbBuff[0] != (char) i)
			{
			ERR_PRINTF1(_L("Return value is not equal to expected result"));
			return TestStepResult();
			}
		}
	SetTestStepResult(EPass);
    return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testwcrtomb_NULL
 *API Tested :wcrtomb()
 *TestCase Description:To check the behaviour of wcrtomb() with all parameter set to NULL with
 * different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcrtomb_NULL()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TInt expected;
    if(GetIntFromConfig(ConfigSection(), _L("expected"), expected) == 0)
    	{
    	ERR_PRINTF1(_L("Reading expected value fails"));
    	return TestStepResult();
    	}
    TInt retVal = wcrtomb(NULL,'\0', NULL);
    if(retVal != expected)
    	{
    	ERR_PRINTF3(_L("Expected: %d and  Result: %d"),expected, retVal);
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testwcrtomb_Neg1
 *API Tested :wcrtomb()
 *TestCase Description:To check the negative behaviour of wcrtomb() with all parameter set to NULL with
 * different locale settings when the state points to an object that contains an invalid conversion state.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcrtomb_Neg1()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    mbstate_t state;
    char mbBuff[MB_CUR_MAX+1];
    mbBuff[0] = '\0';
  	memset(&state,2, sizeof (state));
    wchar_t wc = 65;
    size_t retVal = wcrtomb(mbBuff,wc,&state);
    if(retVal != (size_t) -1 || errno != EINVAL || mbBuff[0] != '\0')
    	{
    	ERR_PRINTF1(_L("Return value is not equal to expected result"));
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }
/* -----------------------------------------------------------------------------
 *Function Name :Testwcrtomb_Neg1
 *API Tested :wcrtomb()
 *TestCase Description:To check the negative behaviour of wcrtomb() with all parameter set to NULL with
 * different locale settings Invalid wide-character code is detected.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcrtomb_Neg2()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    mbstate_t state;
    char mbBuff[MB_CUR_MAX+1];
    mbBuff[0] = '\0';
    if(wcrtomb(NULL,L'\0',&state) != 1)
    	{
    	return TestStepResult();
    	}
    wchar_t wc = 65;
    size_t retVal;
    retVal = wcrtomb(mbBuff,wc,&state);
    if(retVal != 1 || mbBuff[0] != 'A')
    	{
    	return TestStepResult();
    	}
    wc = 0xDC00 + 5;
    retVal = wcrtomb(mbBuff,wc,&state);
    if(retVal != (size_t) -1 || errno != EILSEQ || mbBuff[0] != 'A')
    	{
    	return TestStepResult();
    	}
    wc = 0xD800 + 5;
    retVal = wcrtomb(mbBuff,wc,&state);
    if(retVal != 0 || mbBuff[0] != 'A')
    	{
    	ERR_PRINTF1(_L("Return value is not equal to expected result"));
    	return TestStepResult();
    	}
    wc = 65;
    retVal = wcrtomb(mbBuff,wc,&state);
    if(retVal != (size_t) -1 || errno != EILSEQ || mbBuff[0] != 'A')
    	{
    	ERR_PRINTF1(_L("Return value is not equal to expected result"));
    	return TestStepResult();
    	}
    wc = 0xD800 + 5;
    retVal = wcrtomb(mbBuff,wc,&state);
    if(retVal != (size_t) -1 || errno != EILSEQ || mbBuff[0] != 'A')
    	{
    	ERR_PRINTF1(_L("Return value is not equal to expected result"));
    	return TestStepResult();
    	}
    wc = 0xDC00 + 5;
    retVal = wcrtomb(mbBuff,wc,&state);
    if(retVal != 4 || mbBuff[0] & 0xf0 != 0xf0)
    	{
    	ERR_PRINTF1(_L("Return value is not equal to expected result"));
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }


/* -----------------------------------------------------------------------------
 *Function Name :Testwcscoll
 *API Tested :wcscoll()
 *TestCase Description:To check the behaviour of wcscoll() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcscoll()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() != 0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TPtrC ws1;
    if(GetStringFromConfig(ConfigSection(),_L("ws1"), ws1)== 0)
    	{
    	ERR_PRINTF1(_L("Reading ws1 value fails"));
    	return TestStepResult();
    	}
    TBuf<200> buf1;
    buf1.Copy(ws1);
    wchar_t* inputStr1 = (wchar_t*) buf1.Ptr();
    TInt length1=buf1.Length();
    inputStr1[length1]='\0';
    
    TPtrC ws2;
    if(GetStringFromConfig(ConfigSection(),_L("ws2"), ws2) == 0)
    	{
    	ERR_PRINTF1(_L("Reading ws2 value fails"));
    	return TestStepResult();
    	}
    TBuf<200> buf2;
    buf2.Copy(ws2);
    wchar_t* inputStr2 = (wchar_t*) buf2.Ptr();
    TInt length2=buf2.Length();
    inputStr2[length2]='\0';
    TInt retVal = wcscoll(inputStr1,inputStr2);
    TInt ret ;
    if((ret = wcscmp(inputStr1,inputStr2)) == -1)
    	{
    	ERR_PRINTF1(_L("wcscmp() fails"));
    	return TestStepResult();
    	}
      if(retVal != ret && errno != 0)
    	{
    	ERR_PRINTF1(_L("Return value is not equal to expected result"));
    	ERR_PRINTF3(_L("Expected: %d and  Result: %d"),ret ,retVal);
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }
/* -----------------------------------------------------------------------------
 *Function Name :Testwcsicoll
 *API Tested :wcsicoll()
 *TestCase Description:To check the behaviour of wcsicoll() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcsicoll()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TPtrC ws1;
    if(GetStringFromConfig(ConfigSection(),_L("ws1"), ws1) == 0)
    	{
    	ERR_PRINTF1(_L("Reading ws1 value fails"));
    	return TestStepResult();
    	}
    TBuf<200> buf1;
    buf1.Copy(ws1);
    wchar_t* inputStr1 = (wchar_t*) buf1.Ptr();
    TInt length1=buf1.Length();
    inputStr1[length1]='\0';
    
    TPtrC ws2;
    if(GetStringFromConfig(ConfigSection(),_L("ws2"), ws2)== 0)
    	{
    	ERR_PRINTF1(_L("Reading ws1 value fails"));
    	return TestStepResult();
    	}
    TBuf<200> buf2;
    buf2.Copy(ws2);
    wchar_t* inputStr2 = (wchar_t*) buf2.Ptr();
    TInt length2=buf2.Length();
    inputStr2[length2]='\0';
    TInt retVal = wcsicoll(inputStr1,inputStr2);
    TInt ret ;
    if((ret = wcscmp(inputStr1,inputStr2))== -1)
    	{
    	ERR_PRINTF1(_L("wcscmp() fails"));
        return TestStepResult();
        }
    if(retVal != ret && errno != 0)
    	{
    	ERR_PRINTF3(_L("Expected: %d and  Result: %d"),ret ,retVal);
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testwcslwr
 *API Tested :wcslwr()
 *TestCase Description:To check the behaviour of wcslwr() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcslwr()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0 )
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TPtrC src ;
    if(GetStringFromConfig(ConfigSection(),_L("src"), src) == 0)
    	{
    	ERR_PRINTF1(_L("Reading src value fails"));
    	return TestStepResult();
    	}
    TBuf<200> srcBuf;
    srcBuf.Copy(src);
    wchar_t* srcStr = (wchar_t*) srcBuf.Ptr();
    TInt srcLen=srcBuf.Length();
    srcStr[srcLen]='\0';
    
    TPtrC dst;
    if(GetStringFromConfig(ConfigSection(),_L("dst"), dst)== 0)
    	{
    	ERR_PRINTF1(_L("Reading dst value fails"));
    	return TestStepResult();
    	}
    TBuf<200> dstBuf;
    dstBuf.Copy(dst);
    wchar_t* dstStr = (wchar_t*) dstBuf.Ptr();
    TInt dstLen=dstBuf.Length();
    dstStr[dstLen]='\0';
    
    wchar_t *retVal = wcslwr(srcStr);
    if(retVal == NULL || wcscmp(retVal,dstStr))
        {
        ERR_PRINTF3(_L("Expected: %s and  Result: %s"),dstStr ,retVal);
        return TestStepResult();
        }
    SetTestStepResult(EPass);
    return TestStepResult();
    }


/* -----------------------------------------------------------------------------
 *Function Name :Testwcsupr
 *API Tested :wcsupr()
 *TestCase Description:To check the  behaviour of wcsupr() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcsupr()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale()!=0 )
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TPtrC src;
    if(GetStringFromConfig(ConfigSection(),_L("src"), src) == 0)
    	{
    	ERR_PRINTF1(_L("Reading src value fails"));
        return TestStepResult();
        }
    TBuf<200> srcBuf;
    srcBuf.Copy(src);
    wchar_t* srcStr = (wchar_t*) srcBuf.Ptr();
    TInt srcLen=srcBuf.Length();
    srcStr[srcLen]='\0';
    
    TPtrC dst;
    if(GetStringFromConfig(ConfigSection(),_L("dst"), dst)== 0)
    	{
    	ERR_PRINTF1(_L("Reading dst value fails"));
    	return TestStepResult();
    	}
    TBuf<200> dstBuf;
    dstBuf.Copy(dst);
    wchar_t* dstStr = (wchar_t*) dstBuf.Ptr();
    TInt dstLen=dstBuf.Length();
    dstStr[dstLen]='\0';
    wchar_t *retVal = wcsupr(srcStr);
    if(retVal == NULL || wcscmp(retVal,dstStr))
    	{
    	ERR_PRINTF3(_L("Expected: %s and  Result: %s"),dstStr ,retVal);
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
	return TestStepResult();
	}
/* -----------------------------------------------------------------------------
 *Function Name :Testwctrans
 *API Tested :wctrans()
 *TestCase Description:To check the behaviour of wctrans() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwctrans()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale()!=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TInt err;
    TPtrC input1;
    if(GetStringFromConfig(ConfigSection(),_L("input1"),input1) == 0)
    	{
    	ERR_PRINTF1(_L("Reading input1 value fails"));
    	return TestStepResult();
    	}
    TBuf8<200> buf1;
    buf1.Copy(input1);
    char *inputStr1 = (char*) buf1.Ptr();
    TInt length1=buf1.Length();
    inputStr1[length1]='\0';
    wctrans_t type;
    if((type = wctrans(inputStr1)) ==(wctype_t)0)
       	{
       	ERR_PRINTF1(_L("wctrans() fails"));
    	SetTestStepResult(EFail);
    	}
    TInt input2;
    if(GetIntFromConfig(ConfigSection(),_L("input2"),input2) == 0)
    	{
    	ERR_PRINTF1(_L("Reading input2 value fails"));
    	return TestStepResult();
    	}
    wint_t twc ;
    twc = towctrans((wint_t)input2,type);
    TInt expected;
    if(GetIntFromConfig(ConfigSection(),_L("expected"), expected) == 0)
	    {
	    ERR_PRINTF1(_L("Reading expected  value fails"));
	    return TestStepResult();
	    }
    if(GetIntFromConfig(ConfigSection(),_L("err"), err)== 0)
    	{
    	ERR_PRINTF1(_L("Reading err value fails"));
    	return TestStepResult();
    	}
    
    if(twc != (wint_t)expected && errno != err)
    	{
    	INFO_PRINTF1(_L("Return value is not equal to expected"));
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testwctrans_Neg
 *API Tested :wctrans()
 *TestCase Description:To check the negative behaviour of wctrans() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwctrans_Neg()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TPtrC input1;
    if(GetStringFromConfig(ConfigSection(),_L("input1"),input1) == 0)
    	{
    	ERR_PRINTF1(_L("Reading input1 fails"));
    	return TestStepResult();
    	}
    TBuf8<200> buf1;
    buf1.Copy(input1);
    char *inputStr1 = (char*) buf1.Ptr();
    TInt length1=buf1.Length();
    inputStr1[length1]='\0';
    
    wctrans_t type;
    type = wctrans(inputStr1);
    if(type != (wctype_t)0)
    	{
    	ERR_PRINTF1(_L("wctrans() fails"));
    	return TestStepResult();
    	}
    TInt input2;
    if(GetIntFromConfig(ConfigSection(),_L("input2"),input2)==0)
	   {
	   ERR_PRINTF1(_L("Reading input2 fails"));
	   return TestStepResult();
	   }
    wint_t twc ;
    errno = 0;
    twc = towctrans((wint_t)input2,type);
    TInt expected;
    if(GetIntFromConfig(ConfigSection(),_L("expected"), expected) == 0)
    	{
    	ERR_PRINTF1(_L("Reading expected fails"));
    	return TestStepResult();
    	}
    TInt err;
    if(GetIntFromConfig(ConfigSection(),_L("err"), err) == 0)
    	{
    	ERR_PRINTF1(_L("Reading err fails"));
    	return TestStepResult();
    	}
    if(twc != (wint_t)expected && errno != err)
    	{
    	ERR_PRINTF1(_L("Return value not equal to expected"));
    	ERR_PRINTF4(_L("Expected: %c and  Result: %c errno: %d"),expected ,twc ,errno);
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }


/* -----------------------------------------------------------------------------
 *Function Name :Testwctype
 *API Tested :wctype()
 *TestCase Description:To check the behaviour of wctype() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwctype()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TPtrC property;
    if(GetStringFromConfig(ConfigSection(),_L("property"),property)== 0)
    	{
    	ERR_PRINTF1(_L("Reading property value fails"));
    	return TestStepResult();
    	}
    TBuf8<200> buf;
    buf.Copy(property);
    char *inputStr = (char*) buf.Ptr();
    TInt length=buf.Length();
    inputStr[length]='\0';
    wctype_t type = wctype(inputStr);
    if(type == 0)
    	{
    	ERR_PRINTF1(_L("Supplied character class name is not valid for the current locale"));
    	return TestStepResult();
    	}
    else
    	{
    	TInt wc;
    	TInt cnt = 1;
    	TBuf<16> pNameBuf;
    	_LIT(Kwc, "wc%d" );
    	pNameBuf.Format(Kwc,cnt);
    	while (GetIntFromConfig(ConfigSection(),pNameBuf,wc))
    		{
    		TInt expected;
    		TInt retVal = iswctype(wc,type);
    		TBuf<16> expBuf;
    		_LIT(Kexpected, "expected%d" );
    		 expBuf.Format(Kexpected,cnt);
    	    if(GetIntFromConfig(ConfigSection(),expBuf,expected) == 0)
    	    	{
    	    	ERR_PRINTF1(_L("Reading expected value fails"));
    	    	return TestStepResult();
    	    	}
    	    if(retVal != expected)
    	    	{
    	    	ERR_PRINTF1(_L("Return value not equal to expected"));
    	    	return TestStepResult();
    	    	}
    	    INFO_PRINTF3(_L("Expected: %d and  Result: %d"),expected ,retVal);
    	    pNameBuf.Format(Kwc,++cnt);
    	    }
    	
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }
/* -----------------------------------------------------------------------------
 *Function Name :Testwctype_Neg
 *API Tested :wctype()
 *TestCase Description:To check the negatiove behaviour of wctype() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwctype_Neg()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TPtrC property;
    if(GetStringFromConfig(ConfigSection(),_L("property"),property)== 0)
    	{
    	ERR_PRINTF1(_L("Reading property value fails"));
    	return TestStepResult();
    	}
    TBuf8<200> buf;
    buf.Copy(property);
    char *inputStr = (char*) buf.Ptr();
    TInt length=buf.Length();
    inputStr[length]='\0';
    wctype_t type = wctype(inputStr);
    if(type != 0)
    	{
    	ERR_PRINTF1(_L("Supplied character class name is a valid for the current locale"));
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }


/* -----------------------------------------------------------------------------
 *Function Name :Testwcstod
 *API Tested :wcstod()
 *TestCase Description:To check the behaviour of wcstod() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcstod()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TPtrC wcs;
    if(GetStringFromConfig(ConfigSection(),_L("wcs"),wcs) == 0)
    	{
    	ERR_PRINTF1(_L("Reading wcs value fails"));
    	return TestStepResult();
    	}
    TBuf16<200> wbuf;
    wbuf.Copy(wcs);
    wchar_t *wstr = (wchar_t*) wbuf.Ptr();
    TInt wlength=wbuf.Length();
    wstr[wlength]='\0';
    wchar_t *endPtr = L"";
    double type = wcstod(wstr,&endPtr);
    double retVal;
    if(type < 1)
    	{
    	retVal= round(type);
    	}
    else
    	{
    	retVal = type;
    	}
    TPtrC expected;
    if(GetStringFromConfig(ConfigSection(),_L("expected"),expected) == 0)
    	{
    	ERR_PRINTF1(_L("Reading expected  value fails"));
       	return TestStepResult();
       	}
    TBuf8<200> expBuf;
    expBuf.Copy(expected);
    char *expStr = (char*) expBuf.Ptr();
    TInt expLength=expBuf.Length();
    expStr[expLength]='\0';
    double ret = atof(expStr);
    TPtrC expEndPtr;
    if(GetStringFromConfig(ConfigSection(),_L("expEndPtr"),expEndPtr) == 0)
    	{
    	ERR_PRINTF1(_L("Reading expEndPtr value fails"));
       	return TestStepResult();
       	}
    TBuf16<200> expEndBuf;
    expEndBuf.Copy(expEndPtr);
    wchar_t *expEndStr = (wchar_t*) expEndBuf.Ptr();
    TInt expEndLength=expEndBuf.Length();
    expEndStr[expEndLength]='\0';
    TInt err;
    if(GetIntFromConfig(ConfigSection(),_L("err"),err) == 0)
        {
        ERR_PRINTF1(_L("Reading err value fails"));
        return TestStepResult();
        }
    if(err != errno)
    	{
    	return TestStepResult();
    	}
    INFO_PRINTF4(_L("Expected: %lf Result: %lf  errno: %d"),ret,retVal,errno);
    if((retVal != ret) ||(wcscmp(endPtr, expEndStr)))
        {
        ERR_PRINTF1(_L("Return value not equal to expected"));
        return TestStepResult();
        }
    SetTestStepResult(EPass);
    return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testwcstod_Neg
 *API Tested :wcstod_Neg()
 *TestCase Description:To check the negative behaviour of wcstod() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcstod_Neg()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TPtrC wcs;
    if(GetStringFromConfig(ConfigSection(),_L("wcs"),wcs)== 0)
    	{
    	ERR_PRINTF1(_L("Reading wcs value fails"));
    	return TestStepResult();
    	}
    TBuf16<200> wbuf;
    wbuf.Copy(wcs);
    wchar_t *wstr = (wchar_t*) wbuf.Ptr();
    TInt wlength=wbuf.Length();
    wstr[wlength]='\0';
    TPtrC endptr;
    if(GetStringFromConfig(ConfigSection(),_L("endptr"),endptr)== 0)
    	{
    	ERR_PRINTF1(_L("Reading endptr value fails"));
    	return TestStepResult();
    	}
    TBuf16<200> ebuf;
    ebuf.Copy(endptr);
    wchar_t *estr = (wchar_t*) ebuf.Ptr();
    TInt elength=ebuf.Length();
    estr[elength]='\0';
    double type = wcstod(wstr,&estr);
    if(errno != ERANGE) //ERANGE =34
    	{
    	ERR_PRINTF1(_L("errno not equal to ERANGE"));
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }

/* -----------------------------------------------------------------------------
 *Function Name :Testwctob
 *API Tested :wctob()
 *TestCase Description:To check the  behaviour of wctob() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwctob()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TInt input;
    if(GetIntFromConfig(ConfigSection(),_L("input"),input)== 0)
    	{
    	ERR_PRINTF1(_L("Reading input value fails"));
    	return TestStepResult();
    	}
    TInt expected;
    if(GetIntFromConfig(ConfigSection(),_L("expected"),expected)== 0)
    	{
    	ERR_PRINTF1(_L("Reading expected value fails"));
    	return TestStepResult();
    	}
    TInt ret = wctob(input);
    INFO_PRINTF2(_L("Result: %d"),ret);
    if(ret != expected)
    	{
    	ERR_PRINTF1(_L("Return value not equal to  expected"));
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }
/* -----------------------------------------------------------------------------
 *Function Name :Testwctob_Neg
 *API Tested :wctob()
 *TestCase Description:To check the  negative behaviour of wctob() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwctob_Neg()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TInt ret = wctob(WEOF);
    INFO_PRINTF2(_L("Result: %d"),ret);
    if(ret != EOF)
    	{
    	ERR_PRINTF1(_L("Return value not equal to  expected"));
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }
/* -----------------------------------------------------------------------------
 *Function Name :Testwcswidth
 *API Tested :wcswidth()
 *TestCase Description:To check the  behaviour of wcswidth() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcswidth()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TPtrC input;
    if(GetStringFromConfig(ConfigSection(),_L("input"),input) == 0)
    	{
       	return TestStepResult();
       	}
    TBuf16<200> buf;
    buf.Copy(input);
    wchar_t *inputstr = (wchar_t*)buf.Ptr();
    TInt length=buf.Length();
    inputstr[length]='\0';
    TInt size;
    if(GetIntFromConfig(ConfigSection(),_L("size"),size) == 0)
        {
        return TestStepResult();
        }
    TInt ret = wcswidth(inputstr,size);
    INFO_PRINTF2(_L("Result %d"),ret);
    TInt expected;
    if(GetIntFromConfig(ConfigSection(),_L("expected"),expected) == 0)
        {
        return TestStepResult();
        }
    if(ret != expected)
    	{
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }
/* -----------------------------------------------------------------------------
 *Function Name :Testwcswidth_Neg
 *API Tested :wcswidth()
 *TestCase Description:To check the negative behaviour of wcswidth() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcswidth_Neg()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TInt ret = wcswidth(L"\n\f\t",5);
    INFO_PRINTF2(_L("Result %d"),ret);
    if(ret != -1)
    	{
    	ERR_PRINTF1(_L("Return value is not as expected"));
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }
/* -----------------------------------------------------------------------------
 *Function Name :Testwcwidth
 *API Tested :wcwidth()
 *TestCase Description:To check the negative behaviour of wcwidth() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcwidth()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    TInt input;
    if(GetIntFromConfig(ConfigSection(),_L("input"),input) == 0)
    	{
    	ERR_PRINTF1(_L("Reading input fails"));
        return TestStepResult();
        }
    TInt ret = wcwidth(input);
    INFO_PRINTF2(_L("Result %d"),ret);
    TInt expected;
    if(GetIntFromConfig(ConfigSection(),_L("expected"),expected) == 0)
    	{
    	ERR_PRINTF1(_L("Reading expected  value fails"));
        return TestStepResult();
        }
    if(ret != expected)
    	{
    	ERR_PRINTF1(_L("Return value not as expected "));
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
    return TestStepResult();
    }
/* -----------------------------------------------------------------------------
 *Function Name :Testmbrlen1
 *API Tested :mbrlen()
 *TestCase Description:To check the  behaviour of mbrlen() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testmbrlen0()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
	wchar_t wc;
	char mbc[MB_CUR_MAX];
	mbstate_t *ps = NULL;
	wc = (unsigned short)0x0509;
	if(wctomb( mbc, wc ) == -1)
		{
		ERR_PRINTF1(_L("wctomb() fails"));
		return TestStepResult();
		}
    int retVal1 = mbrlen( mbc, MB_CUR_MAX, ps );
	int retVal2 = mbrlen( mbc, 0, ps );
   	if (retVal1 != 2 || retVal2 != 0)
		{
		ERR_PRINTF3(_L("{Expected: 2, 0} %d %d"), retVal1, retVal2);
		return TestStepResult();
		}
     SetTestStepResult(EPass);
    return TestStepResult();
    }
/* -----------------------------------------------------------------------------
 *Function Name :Testmbrlen1
 *API Tested :mbrlen()
 *TestCase Description:To check the negative behaviour of mbrlen() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testmbrlen1()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale() !=0)
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
	wchar_t wc;
	char mbc[MB_CUR_MAX];
	mbstate_t *ps = NULL;
	wc = L'\0';
	if(wctomb( mbc, wc ) == -1)
		{
		ERR_PRINTF1(_L("wctomb() fails"));
		return TestStepResult();
		}
    int retVal1 = mbrlen( mbc, MB_CUR_MAX, ps );
	int retVal2 = mbrlen( mbc, 0, ps );
   	if (retVal1 != 0 || retVal2 != 0)
		{
		ERR_PRINTF3(_L("{Expected: 0, 0} %d %d"), retVal1, retVal2);
		return TestStepResult();
		}
    SetTestStepResult(EPass);
    return TestStepResult();
    }
/* -----------------------------------------------------------------------------
 *Function Name :Testmbrtowc
 *API Tested :mbrtowc()
 *TestCase Description:To check the  behaviour of mbrtowc() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testmbrtowc()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale()!=0 )
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
	wchar_t *dst = NULL;
	TPtrC src;
	if(GetStringFromConfig(ConfigSection(),_L("src"),src) == 0)
		{
		ERR_PRINTF1(_L("Reading src value fails"));
		return TestStepResult();
		}
	TBuf8<200> buf;
	buf.Copy(src);
	char *srcptr = (char*)buf.Ptr();
	TInt length = buf.Length();
	srcptr[length] = '\0';
	TInt size;
	if(GetIntFromConfig(ConfigSection(),_L("size"),size) ==0)
		{
		ERR_PRINTF1(_L("Reading size value fails"));
		return TestStepResult();
		}
	size_t ret = mbrtowc(dst,srcptr,size,NULL);
	TInt expected;
	if(GetIntFromConfig(ConfigSection(),_L("expected"),expected) ==0)
		{
		ERR_PRINTF1(_L("Reading expected value fails"));
		return TestStepResult();
		}
	TInt err;
	if(GetIntFromConfig(ConfigSection(),_L("err"),err) == 0)
		{
		ERR_PRINTF1(_L("Reading err value fails"));
		return TestStepResult();
		}
	INFO_PRINTF2(_L("errno : %d"),errno);
	if( (ret != expected) || errno != err)
		{
		ERR_PRINTF3(_L("{Expected: %d ,Result: %d"), expected, ret);
		ERR_PRINTF3(_L("{Expected errno : %d ,Actual errno: %d"), err, errno);
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	return TestStepResult();
	
	}
/* -----------------------------------------------------------------------------
 *Function Name :Testmbrtowc_Neg
 *API Tested :mbrtowc()
 *TestCase Description:To check the negative behaviour of mbrtowc() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testmbrtowc_Neg()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale()!=0 )
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
	wchar_t dst[MAX];
	char src[MAX];
	TInt size;
	if(GetIntFromConfig(ConfigSection(),_L("size"),size) == 0)
		{
		return TestStepResult();
		}
	mbstate_t *ps = NULL;
	memset(src, 0xcc, sizeof(src));
	TInt input;
	TBuf<16> InputBuf;
	_LIT( Kinput, "input%d" );
	TInt count = 0;
	InputBuf.Format(Kinput,count);
	while (GetIntFromConfig(ConfigSection(),InputBuf,input))
	    {
	    src[count] = input;
	    InputBuf.Format(Kinput,++count);
	    }
	int ret = mbrtowc(dst,src, size,ps);              
	if (ret != -1 && errno != EILSEQ)
		{
		ERR_PRINTF2(_L("{Expected: -1 ,Result: %d}"),  ret);
		ERR_PRINTF2(_L("{Expected 86  ,Actual errno: %d}"), errno);
		return TestStepResult();
		}	
	SetTestStepResult(EPass);
	return TestStepResult();
	}
/* -----------------------------------------------------------------------------
 *Function Name :Testmbsrtowcs
 *API Tested :mbsrtowcs()
 *TestCase Description:To check the  behaviour of mbsrtowcs() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testmbsrtowcs()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale()!=0 )
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
	wchar_t dst[MAX] ;
   	TPtrC src;
	if(GetStringFromConfig(ConfigSection(),_L("src"),src) == 0)
		{
		return TestStepResult();
		}
	TBuf8<200> buf;
	buf.Copy(src);
	char *srcPtr = (char*)buf.Ptr();
	TInt length = buf.Length();
	srcPtr[length] = '\0';
	TInt size;
	if(GetIntFromConfig(ConfigSection(),_L("size"),size) == 0)
		{
		return TestStepResult();
		}
	int retVal = mbsrtowcs(dst,(const char**)&srcPtr,size,NULL);
	TInt expected;
	if(GetIntFromConfig(ConfigSection(),_L("expected"),expected) == 0)
		{
		return TestStepResult();
		}
	TInt err;
	if(GetIntFromConfig(ConfigSection(),_L("err"),err) == 0)
		{
		return TestStepResult();
		}
	INFO_PRINTF4(_L("Expected: %d Result: %d errno : %d"),expected,retVal,errno);
	if( (retVal != expected) || errno != err)
		{
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	return TestStepResult();
	}
/* -----------------------------------------------------------------------------
 *Function Name :Testmbsrtowcs1
 *API Tested :mbsrtowcs()
 *TestCase Description:To check the  behaviour of mbsrtowcs() when destination buffer is NULL
 * with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testmbsrtowcs1()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale()!=0 )
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
	const char *mbcbuf = "Hello";
	wchar_t *dst  = NULL;
	int retVal = mbsrtowcs(dst,&mbcbuf,0,NULL);
	TInt expected;
	if(GetIntFromConfig(ConfigSection(),_L("expected"),expected) == 0)
		{
		ERR_PRINTF1(_L("Reading from ini fails"));
		return TestStepResult();
		}
	TInt err;
	if(GetIntFromConfig(ConfigSection(),_L("err"),err) == 0)
		{
		ERR_PRINTF1(_L("Reading from ini fails"));
		return TestStepResult();
		}
	if( (retVal != expected) || errno != err)
		{
		ERR_PRINTF4(_L("Expected: %d Result: %d errno : %d"),expected,retVal,errno);
		return TestStepResult();
		}
	SetTestStepResult(EPass);
	return TestStepResult();
	
	}

/* -----------------------------------------------------------------------------
 *Function Name :Testmbsinit
 *API Tested :mbsinit()
 *TestCase Description:To check the  behaviour of mbsinit() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testmbsinit()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale()!=0 )
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    mbstate_t state1;
    memset(&state1,0,sizeof(mbstate_t)); 
    TInt retVal1 = mbsinit(&state1);
    TInt retVal2 = mbsinit(NULL);
    mbstate_t state2;
    TInt retVal3 = mbsinit(&state2);
    if(retVal1 == 0 || retVal2 == 0 || retVal3 != 0)
    	{
    	ERR_PRINTF4(_L("Expected return values: 0 0 0 ,Actual return values: %d %d %d "),retVal1,retVal2,retVal3);
    	return TestStepResult();
    	}
   	SetTestStepResult(EPass);
	return TestStepResult();
	
	}

/* -----------------------------------------------------------------------------
 *Function Name :Testwcsrtombs
 *API Tested :wcsrtombs()
 *TestCase Description:To check the  behaviour of wcsrtombs() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testwcsrtombs()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale()!=0 )
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
   	const wchar_t *tstArr = L"ABCD";
	char dstArr[(MB_CUR_MAX+1)*5];
	char* dst = (char*) dstArr;
	const wchar_t* src = tstArr;
	size_t retval;
	mbstate_t state;
	if(wcrtomb(NULL,L'\0',&state) != 1)
		{
		ERR_PRINTF1(_L("wcrtomb() fails"));
		return TestStepResult();
		}
	retval = wcsrtombs (dst, &src, sizeof dstArr, &state);
	if(retval != 4 	|| dst[0] != 'A'|| dst[1] != 'B'|| dst[4] != '\0')
		{
		ERR_PRINTF1(_L("Return value not as expected"));
		return TestStepResult();
		}
    SetTestStepResult(EPass);
	return TestStepResult();
	}
/* -----------------------------------------------------------------------------
 *Function Name :Testsetlocale
 *API Tested :setlocale()
 *TestCase Description:To check the  behaviour of setlocale() with different locale settings.
 * -----------------------------------------------------------------------------
 */
TVerdict CTestLocale::Testsetlocale()
    {
    SetTestStepResult(EFail);
    if(ReadStringAndSetlocale()!= KErrNone )
    	{
    	ERR_PRINTF1(_L("Set locale fails"));
    	return TestStepResult();
    	}
    SetTestStepResult(EPass);
	return TestStepResult();
	}

/* -----------------------------------------------------------------------------
 * Utility functions which is used to comapre the native locale with the PIPS locale
 * -----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------
 * Utility function which is used to convert the TChar to char*
 * -----------------------------------------------------------------------------
 */ 

void convertor(TChar ch,char *str)
	{
	const TText* point = (const TText*) &ch;
	if(point)
		{
		const TPtrC string(point);
		TBuf8<MAXLEN> buffer;
		buffer.Copy(string);
		strncpy(str, (char*) buffer.Ptr(), buffer.Length());
		str[buffer.Length()] = '\0';
		}	
	}

/* -----------------------------------------------------------------------------
 * Utility function which is used to convert the TText* to char*
 * -----------------------------------------------------------------------------
 */ 
 void convertorlang(const TText *text,char *str)
	{
	const TPtrC string(text);
	TBuf8<MAXLEN> buffer;
	buffer.Copy(string);
	strncpy(str, (char*) buffer.Ptr(), buffer.Length());
	str[buffer.Length()] = '\0';
	}	

/* -----------------------------------------------------------------------------
 * Utility function which is used to convert the TPtrC to char*
 * -----------------------------------------------------------------------------
 */ 
void convertorstrcoll(TPtrC string,char *str)
	{
    TBuf8<MAXLEN> buffer;
	buffer.Copy(string);
	strncpy(str, (char*) buffer.Ptr(), buffer.Length());
	str[buffer.Length()] = '\0';
	}	

/* -----------------------------------------------------------------------------
 * Utility function which is used to set the locale
 * -----------------------------------------------------------------------------
 */ 
TInt CTestLocale::SetCustomLocale(TDesC& aDllName)	
	{
	RLoader loader;
	TInt retVal = -1;
	TInt r = loader.Connect();
	if(KErrNone == r)
		{
		//Load the language variant DLL 
		TInt ret = KErrNotFound;
		TInt size = NumLocaleExports * sizeof(TLibraryFunction);
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
        if( aDllName.Find(_L("elocl_lan")) != KErrNotFound )
            {
            TPtr8 functionListBuf((TUint8*) data, size, size);
            ret = loader.SendReceive(ELoadLocale, TIpcArgs(0, (const TDesC*)&aDllName, &functionListBuf) );
                
            }
        else if( aDllName.Find(_L("elocl_reg")) != KErrNotFound )
            {
            TPtr8 functionListBuf((TUint8*) data_reg, size, size);
            ret = loader.SendReceive(ELoadLocale, TIpcArgs(0, (const TDesC*)&aDllName, &functionListBuf) );
            }
    
#else
		TPtr8 functionListBuf((TUint8*) data, size, size);
		ret = loader.SendReceive(ELoadLocale, TIpcArgs(0, (const TDesC*)&aDllName, &functionListBuf) );
#endif
		if(KErrNone == ret )
			{
			retVal = 0;
			}
			loader.Close();
		}
		return retVal;
	}


/* -----------------------------------------------------------------------------
 * Utility function which is used to get the system locale
 * -----------------------------------------------------------------------------
 */ 
TInt CTestLocale::getlocale(TLocaleAspect aspect)
	{   
	TBuf<MAXLEN> Kdname;
	TExtendedLocale locale;
    locale.LoadSystemSettings();
    locale.GetLocaleDllName(aspect,Kdname);
    return (SetCustomLocale(Kdname));
	}
	
/* -----------------------------------------------------------------------------
 * Utility function which is used by strcoll
 * -----------------------------------------------------------------------------
 */ 	
int test_strcoll(char* firststring,char* secondstring)
	{
       int result=strcoll(firststring,secondstring);	
       if(result<0)
       result=-1;
       if(result>0)
       result=1;
       return result;
	}

/* ---------------------------------------------------------------------------------------------
 * Utility function which is used to comapare the locale conv elements with the TLocale values
 * ---------------------------------------------------------------------------------------------
 */
	
TInt CTestLocale::compare()
	{
	//getting the TLocale values
	TInt retval=1;
	int ret,ret1,ret2;
	char decimalpoint[MAXLEN];
	char thousandsep[MAXLEN];
	char currencysymbol[MAXLEN];
	struct lconv* str = NULL;
	TLocale objlocale;
	objlocale.Refresh();
	TLocalePos aCurrencySymbolPosition = objlocale.CurrencySymbolPosition();
	TLocale::TNegativeCurrencyFormat aNegativeCurrencyFormat = objlocale.NegativeCurrencyFormat();
	TBool aCurrencySpaceBetween = objlocale.CurrencySpaceBetween();
	TBool aNegativeCurrencySymbolOpposite = objlocale.NegativeCurrencySymbolOpposite();
	TBool aNegativeLoseSpace = objlocale.NegativeLoseSpace();
	TBool aCurrencyTriadsAllowed = objlocale.CurrencyTriadsAllowed();
	
	TInt currencyDecimalPlaces = objlocale.CurrencyDecimalPlaces();
	TChar aThousandsSeparator= objlocale.ThousandsSeparator();
	TChar aDecimalSeparator = objlocale.DecimalSeparator();
   	TCurrencySymbol cur;
    str=localeconv();
   	
    char* ch=str->mon_decimal_point;
    char* ch1=str->mon_thousands_sep;
    char* ch2=str->currency_symbol;
    int fracdigits=str->frac_digits;
    int p_cs_precedes=str->p_cs_precedes;
    int n_cs_precedes=str->n_cs_precedes;
    int p_sep_by_space=str->p_sep_by_space;
    int n_sep_by_space=str->n_sep_by_space;
    
    convertor(aDecimalSeparator,decimalpoint);
	convertor(aThousandsSeparator,thousandsep);
	strncpy(currencysymbol,(char*)cur.Ptr(),cur.Length());
	currencysymbol[cur.Length()]='\0';
    
    if(p_cs_precedes != ((ELocaleBefore == aCurrencySymbolPosition)?1:0))
    retval=0;

    if(n_cs_precedes != aNegativeCurrencySymbolOpposite?0:1)
    retval=0;
    
    if(p_sep_by_space    !=(aCurrencySpaceBetween?1:0))
    retval=0;
    
    if(n_sep_by_space    != (aNegativeLoseSpace?1:0))
    retval=0;
    
	ret=strcmp(decimalpoint,ch);
	ret1=strcmp(thousandsep,ch1);
	ret2=strcmp(currencysymbol,ch2);
   
    
	if(ret!=0 || ret1!=0 || ret2!=0)
	retval=0;
	if(fracdigits != currencyDecimalPlaces)
	retval=0;
	
	return retval;
}



/* ---------------------------------------------------------------------------------------------
 * Utility function which is used to comapare the nl_langinfo elements with the TLocale values
 * ---------------------------------------------------------------------------------------------
 */

TInt CTestLocale::comparelang()
	{
	_LIT8(KAMPMSpace12hr, "B");
	_LIT8(KAMPMSpace, "A");
	_LIT8(KAMPMNoSpace12hr, "*B");
	_LIT8(KAMPMNoSpace, "*A");
	TLocale objlocale;
  	SLocaleTimeDateFormat iLocaleTimeDateFormat;
    char monthlang[MONTHS];
  	char abbmonthlang[MONTHS];
  	char daylang[WEEKDAYS];
  	char abbdaylang[WEEKDAYS];
  	char amlang[5];
  	char pmlang[5];
  	

  	if(getlocale(ELocaleLanguageSettings) ==  -1 )// to load language details from language dll
  	    {
            INFO_PRINTF1(_L("locale dll loading  failed"));
            return 0;
  	    }
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
    if (getlocale(ELocaleTimeDateSettings) == -1 ) // to load time info from region dll
        {
            INFO_PRINTF1(_L("locale dll loading  failed"));
            return 0;
        }

#endif
	char timeLocale[100];
 	char dateLocale[100];
 	char* str = NULL;
	//Set month and abbreviated month name information
	for(TInt aMonth = 0 ; aMonth < MONTHS; aMonth++)
		{
    #ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
		
		
		const TText16* month = ((reinterpret_cast<TText*const*>((const TText*) data[FnMonthTableV2]()))[aMonth]);
		
    #else
		const TText16* month = ((reinterpret_cast<TText*const*>((const TText*) data[FnMonthTable]()))[aMonth]);
    #endif
		if(month)
			{
			convertorlang(month,&monthlang[aMonth]);
			}
		str = nl_langinfo(aMonth+21);
		if(strcmp(str,&monthlang[aMonth])!=0)
		return 0;
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
		
		const TText16* abbMonth = ((reinterpret_cast<TText*const*>((const TText*) data[FnMonthAbbTableV2]()))[aMonth]);
#else
		const TText16* abbMonth = ((reinterpret_cast<TText*const*>((const TText*) data[FnMonthAbbTable]()))[aMonth]);
#endif
		if(abbMonth)
			{
			convertorlang(abbMonth,&abbmonthlang[aMonth]);			
			}
		str = nl_langinfo(aMonth+33);
		if(strcmp(str,&abbmonthlang[aMonth])!=0)
		return 0;
		}
		
	for(TInt aDay = 0 ; aDay < WEEKDAYS - 1; aDay++)
		{
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
		const TText16* day = ((reinterpret_cast<TText*const*>((const TText*) data[FnDayTableV2]()))[aDay]);
#else
		const TText16* day = ((reinterpret_cast<TText*const*>((const TText*) data[FnDayTable]()))[aDay]);
#endif
		if(day)
			{
			convertorlang(day,&daylang[aDay]);			
			}
			str = nl_langinfo(aDay+8);
			if(strcmp(str,&daylang[aDay])!=0)
	        return 0;
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
			const TText16* abbDay = ((reinterpret_cast<TText*const*>((const TText*) data[FnDayAbbTableV2]()))[aDay]);
#else
			const TText16* abbDay = ((reinterpret_cast<TText*const*>((const TText*) data[FnDayAbbTable]()))[aDay]);
#endif
		if(abbDay)
			{
			convertorlang(abbDay,&abbdaylang[aDay]);	
			}
			str = nl_langinfo(aDay+15);
			if(strcmp(str,&abbdaylang[aDay])!=0)
			return 0;
		}
			
			
		//Set AM string
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
	const TText16* am = ((reinterpret_cast<TText*const*>((const TText*) data[FnAmPmTableV2]()))[0]);
#else
	const TText16* am = ((reinterpret_cast<TText*const*>((const TText*) data[FnAmPmTable]()))[0]);
#endif
	if(am)
		{
		convertorlang(am,amlang);	
		}
	str = nl_langinfo(AM_STR);
	if(strcmp(str,amlang)!=0)
	return 0;
	//Set PM string
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
	const TText16* pm = ((reinterpret_cast<TText*const*>((const TText*) data[FnAmPmTableV2]()))[1]);
#else
	const TText16* pm = ((reinterpret_cast<TText*const*>((const TText*) data[FnAmPmTable]()))[1]);
#endif
	if(pm)
		{
		convertorlang(pm,pmlang);
		}
	str = nl_langinfo(PM_STR);
	if(strcmp(str,pmlang)!=0)
	return 0;
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
	    Mem::Copy(&iLocaleTimeDateFormat.iTimeFormatSpec[0], (const TAny*) data_reg[FnTimeFormatSpecV2](), sizeof(TText) * (KMaxTimeFormatSpec+1));
#else
	Mem::Copy(&iLocaleTimeDateFormat.iTimeFormatSpec[0], (const TAny*) data[FnTimeFormatSpec](), sizeof(TText) * (KMaxTimeFormatSpec+1));
#endif
	TText* timeForm = iLocaleTimeDateFormat.iTimeFormatSpec;
	//Set time format(T_FMT) and am/pm time format(T_FMT_AMPM) string
	if(timeForm)
		{
		const TPtrC time(timeForm);
		TBuf8<TIME_FORMAT_LENGTH> timeFormat;
		timeFormat.Copy(time);
		TChar timeSeparator = objlocale.TimeSeparator(1);
		TBuf8<DATE_TIME_SEPARATOR> timeSeparator1;
		timeSeparator1.Append(timeSeparator);
		timeSeparator = objlocale.TimeSeparator(2);
		TBuf8<DATE_TIME_SEPARATOR> timeSeparator2;
		timeSeparator2.Append(timeSeparator);
		TTimeFormat time12hror24hr = objlocale.TimeFormat();
		if(KErrNotFound != timeFormat.Locate('J'))
			{
			//Check for time format(12/24 hour format)
			if(time12hror24hr == ETime12)
				{
				strcpy( timeLocale,TWELVE_HOUR_STR);
				}
			else
				{
				strcpy( timeLocale,TWENTY_FOUR_HOUR_STR);
				}
			}
			
		else if(KErrNotFound != timeFormat.Locate('I'))
			{
			strcpy( timeLocale,TWELVE_HOUR_STR);
			}	
			
		else if(KErrNotFound != timeFormat.Locate('H'))
			{
			strcpy( timeLocale,TWENTY_FOUR_HOUR_STR);
			}	
		
		strncat( timeLocale, (char*) timeSeparator1.Ptr(), timeSeparator1.Length());
		strcat( timeLocale, MINUTE_STR);
		strncat( timeLocale, (char*) timeSeparator2.Ptr(), timeSeparator2.Length());
		strcat( timeLocale, SECOND_STR);
		if(time12hror24hr == ETime12)
			{
			if((KErrNotFound != timeFormat.Find(KAMPMSpace12hr)) || (KErrNotFound != timeFormat.Find(KAMPMSpace)) )
				{
				if((KErrNotFound != timeFormat.Find(KAMPMNoSpace12hr)) || (KErrNotFound != timeFormat.Find(KAMPMNoSpace)))
					{
					strcat( timeLocale, AMPM_NO_SPACE);
					}
					else
					{
					strcat( timeLocale, AMPM_SPACE);
					}
				}	
			}
						
	 	}
	
	str=nl_langinfo(T_FMT_AMPM);
	if(strcmp(str,timeLocale)!=0)
	return 0;
	TDateFormat aDateFormat =objlocale.DateFormat();
	//Set date fromat string(D_FMT)	
	TChar dateSeparator = objlocale.DateSeparator(1);
	TBuf8<DATE_TIME_SEPARATOR> dateSeparator1;
	dateSeparator1.Append(dateSeparator);
	TBuf8<DATE_TIME_SEPARATOR> dateSeparator2;
	dateSeparator = objlocale.DateSeparator(2);
	dateSeparator2.Append(dateSeparator);
	//Americal date formt(mm/dd/YYYY)
	if(EDateAmerican == aDateFormat)
		{
		strcpy( dateLocale, MONTH_STR);
		strncat( dateLocale, (char*) dateSeparator1.Ptr(), dateSeparator1.Length());
		strcat( dateLocale, DAY_STR);
		strncat( dateLocale, (char*) dateSeparator2.Ptr(), dateSeparator2.Length());
		strcat( dateLocale, YEAR_STR);
		}
		
		//European date format(dd/mm/YYYY)
	if(EDateEuropean == aDateFormat)
		{
		strcpy( dateLocale, DAY_STR);
		strncat( dateLocale, (char*) dateSeparator1.Ptr(), dateSeparator1.Length());
		strcat( dateLocale, MONTH_STR);
		strncat( dateLocale, (char*) dateSeparator2.Ptr(), dateSeparator2.Length());
		strcat( dateLocale, YEAR_STR);
		}
		
		//Japanese date format(YYYY/mm/dd)
	if(EDateJapanese == aDateFormat)
		{
		strcpy((char*) dateLocale, YEAR_STR);
		strncat((char*) dateLocale, (char*) dateSeparator1.Ptr(), dateSeparator1.Length());
		strcat((char*) dateLocale, MONTH_STR);
		strncat((char*) dateLocale, (char*) dateSeparator2.Ptr(), dateSeparator2.Length());
		strcat((char*) dateLocale, DAY_STR);
		}
		
    str=nl_langinfo(D_FMT);
	if(strcmp(str,dateLocale)!=0)
	return 0;
	
    return 1;
}
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestlocale::synclocale()
API Tested			: Checks whether localeconv,nl_langinfo api's sync withthe
                      smart phone settings 

-------------------------------------------------------------------------------
*/

TVerdict CTestLocale::synclocale()
	{
    SetTestStepResult(EFail);
	TLocale objlocale;
	objlocale.SetDecimalSeparator('%');
    objlocale.SetThousandsSeparator('&');
    objlocale.Set();
    if(!compare())
    return TestStepResult();	
    if(!comparelang())
    return TestStepResult();	
    objlocale.SetDecimalSeparator('g');
    objlocale.SetThousandsSeparator('h');
    objlocale.Set();
    if(!compare())
    {
    ERR_PRINTF1(_L("locale conv and TLocale values are not same"));
    return TestStepResult();	
    }
    objlocale.SetDateFormat(EDateAmerican);
    objlocale.Set();
    if(!comparelang())
    {
    ERR_PRINTF1(_L("locale conv and nl_langinfo values are not same"));
    return TestStepResult();
    }
    SetTestStepResult(EPass);
    return TestStepResult();	
    }

	


/*
 * new local model test
 * */


TVerdict CTestLocale:: testnewlocale()
    {
#ifdef  SYMBIAN_DISTINCT_LOCALE_MODEL
    SetTestStepResult(EFail);
    char locale[MAXLEN];
    struct lconv* str = NULL;
        
    strcpy(locale,"en_GB.ISO-8859-1");
    
    if(setlocale(LC_NUMERIC,locale)==NULL)
        {
        ERR_PRINTF1(_L("testnewlocale_Numeric LC_NUMERIC  fails with the value "));;
        return TestStepResult();
        }   
    str=localeconv();
    char* ch=str->mon_decimal_point;
    char* ch1=str->mon_thousands_sep;
    INFO_PRINTF2(_L("testnewlocale_Numeric : decimal_point  %s"),ch);
    INFO_PRINTF2(_L("testnewlocale_Numeric : thousands_sep  %s"),ch1);
    if(setlocale(LC_MONETARY,locale)==NULL)
           {
           ERR_PRINTF1(_L("testnewlocale_Numeric LC_MONETARY fails with the value"));
           return TestStepResult();
           }  
    struct lconv* str1 = NULL;
    str1=localeconv();
       
    char* ch2=str1->currency_symbol;
    INFO_PRINTF2(_L("testnewlocale_Numeric : currency_symbol  %s"),ch2);
#endif        
    SetTestStepResult(EPass);
    return TestStepResult();
    }


/*
 
-------------------------------------------------------------------------------
Function Name		: CTestlocale::synclocalehybrid()
API Tested			: check whether the PIPS api's and Native api's return 
                      the same values(like decimal separator..) for the same locale setting
-------------------------------------------------------------------------------
*/

TVerdict CTestLocale::synclocalehybrid()
	{
	char locale[MAXLEN];
	TBuf8<MAXLEN> localename;
	SetTestStepResult(EFail);
	TPtrC DllName,LocaleName,string1;
	if(!GetStringFromConfig(ConfigSection(),_L("DllName"), DllName))
	{
	ERR_PRINTF1(_L("unable to read the DllName from the ini file"));
	return TestStepResult();
	}
	if(!GetStringFromConfig(ConfigSection(),_L("LocaleName"), LocaleName))
	{
	ERR_PRINTF1(_L("unable to read the LocaleName from the ini file"));
	return TestStepResult();
	}
	localename.Copy(LocaleName);
    strncpy(locale,(char*)localename.Ptr(),localename.Length());
	locale[localename.Length()]='\0';
	TExtendedLocale exlocale;
    if(exlocale.LoadLocale(DllName)!=0)
    {
    ERR_PRINTF1(_L("unable to load the locale dll"));
	return TestStepResult();	
    }
    exlocale.SaveSystemSettings();
    if(setlocale(LC_ALL,locale)==NULL)
    {
    ERR_PRINTF1(_L("unable to set the locale"));
    return TestStepResult();
    }	
    if(!compare())
    return TestStepResult();
    if(!comparelang())
    return TestStepResult();
    SetTestStepResult(EPass);
	return TestStepResult();
    }
	
/*
----------------------------------------------------------------------------------------------
Function Name		: CTestlocale::strcoll_locale()
API Tested			: check whether the strcoll behaves properly for 
                      different locale settings
----------------------------------------------------------------------------------------------
*/

TVerdict CTestLocale::strcoll_locale()
	{
	SetTestStepResult(EFail);
	char locale[MAXLEN],strcollfirstarg[MAXLEN],strcollsecondarg[MAXLEN];
	TPtrC LocaleName,StrCollFirstString,StrCollSecondString;
	TInt ExpectedResult;
    if(!GetStringFromConfig(ConfigSection(),_L("LocaleName"), LocaleName))
    return TestStepResult();	
    convertorstrcoll(LocaleName,locale);
    if(!GetStringFromConfig(ConfigSection(),_L("StrCollFirstString"), StrCollFirstString))
    return TestStepResult();	
    convertorstrcoll(StrCollFirstString,strcollfirstarg);
    if(!GetStringFromConfig(ConfigSection(),_L("StrCollSecondString"), StrCollSecondString))	
    return TestStepResult();
    convertorstrcoll(StrCollSecondString,strcollsecondarg);
    if(!GetIntFromConfig(ConfigSection(),_L("ExpectedValue"), ExpectedResult))
    return TestStepResult();	
    if(setlocale(LC_COLLATE,locale)==NULL)
    {
    INFO_PRINTF1(_L("setlocale fails"));
    return TestStepResult();
    }
    int i;
    i=test_strcoll(strcollfirstarg,strcollsecondarg);
    if(ExpectedResult<0)
    {
    if(i>=0)
    {
    ERR_PRINTF2(_L("strcoll fails with the value %d"),i);
    return TestStepResult();	
    }
    }
	else if(ExpectedResult>0)
	{
	if(test_strcoll(strcollfirstarg,strcollsecondarg)<=0)
    {
    ERR_PRINTF2(_L("strcoll fails with the value %d"),i);
    return TestStepResult();	
	}
    }
	else
	{
	if(test_strcoll(strcollfirstarg,strcollsecondarg)!=0)
	{
	ERR_PRINTF2(_L("strcoll fails with the value %d"),i);
    return TestStepResult();	
	}
	}
	SetTestStepResult(EPass);
	return TestStepResult();
    }
	



/*
-------------------------------------------------------------------------------
Function Name		: CTestlocale::strcoll_synclocale()
API Tested			: check whether strcoll(PIPS api) and TDesC::CompareC
                      (Native api)return the same value for the same locale 
                      setting 
-------------------------------------------------------------------------------
*/

TVerdict CTestLocale::strcoll_synclocale()
	{
	SetTestStepResult(EFail);
	char strcollfirstarg[MAXLEN],strcollsecondarg[MAXLEN];
	TPtrC StrCollFirstString,StrCollSecondString;
    if(!GetStringFromConfig(ConfigSection(),_L("StrCollFirstString"), StrCollFirstString))
    return TestStepResult();	
    convertorstrcoll(StrCollFirstString,strcollfirstarg);
    if(!GetStringFromConfig(ConfigSection(),_L("StrCollSecondString"), StrCollSecondString))	
    return TestStepResult();
    convertorstrcoll(StrCollSecondString,strcollsecondarg);
    int i=strcoll(strcollfirstarg,strcollsecondarg);
    int j=StrCollFirstString.CompareC(StrCollSecondString,1,NULL);
    if((i<=0 && j>0)||(i>=0 && j<0) || (j<=0 && i >0) || (j>=0 && i<0))
    return TestStepResult();
    SetTestStepResult(EPass);
    return TestStepResult();
    }	
/*
-------------------------------------------------------------------------------
Function Name		: CTestlocale::clocale()
API Tested			: check whether localeconv and nl_langinfo return the C 
                      structures for the C locale

-------------------------------------------------------------------------------
*/

TVerdict CTestLocale::clocale()
	{
	SetTestStepResult(EFail);
	char locale[MAXLEN];
	TPtrC LocaleName;
	if(!GetStringFromConfig(ConfigSection(),_L("LocaleName"), LocaleName))
	{
	ERR_PRINTF1(_L("not able to read from the ini file"));
    return TestStepResult();
	}
	convertorstrcoll(LocaleName,locale);
    if(setlocale(LC_ALL,locale)==NULL)
    {
    ERR_PRINTF1(_L("setlocale fails"));
    return TestStepResult();
    }
    struct lconv* str=NULL;
    str=localeconv();
    if(str==NULL)
    {
    ERR_PRINTF1(_L("localeconv fails"));
	return TestStepResult();
    }
    if(nl_langinfo(D_FMT)==NULL)
    {
    ERR_PRINTF1(_L("nl_langinfo fails"));
    return TestStepResult();
    }
    if(nl_langinfo(T_FMT)==NULL)
    {
    ERR_PRINTF1(_L("nl_langinfo fails"));
    return TestStepResult();
    }
    SetTestStepResult(EPass);
    return TestStepResult();
	}
	

/* ========================== OTHER EXPORTED FUNCTIONS =========================*/


/*  End of File*/

