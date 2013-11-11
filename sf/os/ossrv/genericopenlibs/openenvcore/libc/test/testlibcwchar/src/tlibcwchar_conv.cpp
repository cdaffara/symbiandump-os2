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
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
#include <locale.h>
#endif

// -----------------------------------------------------------------------------
//Function Name :mblen_Test0
//API Tested :mblen
//TestCase Description:Multibyte char length -> 2 (with n = 2 and n = 0)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mblen_Test0L()
    {
    
    INFO_PRINTF1(_L("In mblen_Test0L"));

    //--------------------------
	wchar_t wc;
	char mbc[MB_CUR_MAX];

	wc = (unsigned short)0x0509;
  
    wctomb( mbc, wc );
   
    int i = mblen( mbc, MB_CUR_MAX );
	int j = mblen( mbc, 0 );
    
	INFO_PRINTF3(_L("{Expected: 2, 0} %d %d"), i, j);
	//--------------------------

	if (i != 2 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :mblen_Test1
//API Tested :mblen
//TestCase Description:Multibyte char length -> 1 (with n = 2 and n = 0)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mblen_Test1L()
    {
    
    INFO_PRINTF1(_L("In mblen_Test1L"));

    //--------------------------
	wchar_t wc = L'a';
	char mbc[MB_CUR_MAX];
		   
    wctomb( mbc, wc );
   
    int i = mblen( mbc, MB_CUR_MAX );
	int j = mblen( mbc, 0 );

    //--------------------------
	INFO_PRINTF3(_L("{Expected: 1, 0} %d %d"), i, j);

	if (i != 1 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mblen_Test2
//API Tested :mblen
//TestCase Description:mblen returns -> 0 when the input is a NULL char 
//(with n = 2 and n = 0)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mblen_Test2L()
    {
    
    INFO_PRINTF1(_L("In mblen_Test2L"));

    //--------------------------
	wchar_t wc = L'\0';
	char mbc[MB_CUR_MAX];
	   
    wctomb( mbc, wc );
   
    int i = mblen( mbc, MB_CUR_MAX );
	int j = mblen( mbc, 0 );

	//--------------------------
	INFO_PRINTF3(_L("{Expected: 0, 0} %d %d"), i, j);

	if (i != 0 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mblen_Test3
//API Tested :mblen
//TestCase Description:mblen returns -> 2 (MB char string with each char len 2)
//(with n = sizeof MB char string and n = 0)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mblen_Test3L()
    {
    
    INFO_PRINTF1(_L("In mblen_Test3L"));

    //--------------------------
	wchar_t ws[64];
	char mbs[64];

	ws[0] = (unsigned short)0x0509;
	ws[1] = (unsigned short)0x0609;
	ws[2] = (unsigned short)0x0709;
	ws[3] = (unsigned short)0x0809;
	ws[4] = (unsigned short)0x0909;

    wcstombs( mbs, ws, 5 );
   
    int i = mblen( mbs, MB_CUR_MAX );
	int j = mblen( mbs, 0 );

	//--------------------------
	INFO_PRINTF3(_L("{Expected: 2, 0} %d %d"), i, j);

	if (i != 2 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mblen_Test4
//API Tested :mblen
//TestCase Description:mblen returns -> 1 (MB char string with each char len 1)
//(with n = sizeof MB char string and n = 0)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mblen_Test4L()
    {
    
    INFO_PRINTF1(_L("In mblen_Test4L"));

    //--------------------------
	wchar_t ws[] = L"widearray";
	
	char    *pmbs = (char *)malloc( sizeof( ws ) );
    
    wcstombs( pmbs, ws, sizeof( ws ) );
   
    int i = mblen( pmbs, MB_CUR_MAX );
	int j = mblen( pmbs, 0 );

    free (pmbs);
	//--------------------------
	INFO_PRINTF3(_L("{Expected: 1, 0} %d %d"), i, j);

	if (i != 1 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mblen_Test5
//API Tested :mblen
//TestCase Description:mblen returns -> 0 (NULL MB char string)
//(with n = sizeof MB char string and n = 0)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mblen_Test5L()
    {
    
    INFO_PRINTF1(_L("In mblen_Test5L"));

    //--------------------------
	wchar_t ws[] = L"\0";
	
	char    *pmbs = (char *)malloc( sizeof( ws ) );
    
    wcstombs( pmbs, ws, sizeof( ws ) );
   
    int i = mblen( pmbs, sizeof( wchar_t ) );
	int j = mblen( pmbs, 0 );

    free (pmbs);
	//--------------------------
	INFO_PRINTF3(_L("{Expected: 0, 0} %d %d"), i, j);

	if (i != 0 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mblen_Test6
//API Tested :mblen
//TestCase Description:mblen returns -> -1 (no complete multibyte character)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mblen_Test6L()
    {
    
    INFO_PRINTF1(_L("In mblen_Test6L"));

    //--------------------------
    wchar_t wc;
	char mbc[MB_CUR_MAX];

	wc = (unsigned short)0xff09; 
   
    wctomb( mbc, wc );
          
    int i = mblen( mbc, 1 );
	int j = mblen( mbc, 0 );

	//--------------------------
	INFO_PRINTF3(_L("{Expected: -1, 0} %d %d"), i, j);

	if (i != -1 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mblen_Test7
//API Tested :mblen
//TestCase Description:mblen returns -> -1 (incomplete MB sequence)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mblen_Test7L()
    {
    
    INFO_PRINTF1(_L("In mblen_Test7L"));

    //--------------------------
    wchar_t wc;
	char mbc[MB_CUR_MAX];

	wc = (unsigned short)0xe109;
    
    wctomb( mbc, wc );
          
    int i = mblen( mbc, 2 );
	int j = mblen( mbc, 0 );

	//--------------------------
	INFO_PRINTF3(_L("{Expected: -1, 0} %d %d"), i, j);

	if (i != -1 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mblen_Test8
//API Tested :mblen
//TestCase Description:mblen returns -> 0 (NULL srting as input)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mblen_Test8L()
    {
    
    INFO_PRINTF1(_L("In mblen_Test8L"));

    //--------------------------
    char  *pmbc = NULL;
                
    int i = mblen( pmbc, MB_CUR_MAX );
	int j = mblen( pmbc, 0 );
	//--------------------------
	INFO_PRINTF3(_L("{Expected: 0, 0} %d %d"), i, j);

	if (i != 0 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mblen_Test9
//API Tested :mblen
//TestCase Description:mblen returns -> 2 (invalid sequence replaced 
//with replacement char case) mblen returns -> -1 (truncated MB sequence)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mblen_Test9L()
    {
    
    INFO_PRINTF1(_L("In mblen_Test9L"));

    //--------------------------
	char mbc[MB_CUR_MAX];
    
	mbc[0] = (unsigned char)0xcc;
	mbc[1] = (unsigned char)'\0';
  
    int i = mblen( mbc, 2 );
	int j = mblen( mbc, 1 );

    //--------------------------
	INFO_PRINTF3(_L("{Expected: 2, -1} %d %d"), i, j);

	if (i != -1 || j != -1)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wctomb_Test0
//API Tested :wctomb
//TestCase Description:wctomb returns -> 2 (length of the multibyte character) 
//when the input wide character occupies 2 bytes after encoding it as a 
//multibyte character.
//input --> wide char between 0x0080 and 0x07ff
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::wctomb_Test0L()
    {
    
    INFO_PRINTF1(_L("In wctomb_Test0L"));

	char *p = new char;	//if we don't have this variable, wctomb fails
						//as the parameter we pass here (L'A') is not going
						//to be the same as the argument wctomb receives
						//in that case.
						
    //--------------------------
	char buf[MB_CUR_MAX];
	unsigned long lmask = 0x000000ff;

	memset(buf, 0xcc, sizeof(buf));

	wchar_t wc = (unsigned short)0x00a9;
	
	int len = wctomb(buf, wc);              
    
	INFO_PRINTF5(_L("{Expected: 2  c2  a9 cc} %d %x %x"), len, buf[0], buf[1], buf[2]);

	delete p;
	
	if (len != 2 || (buf[0] & lmask) != 0xc2 || (buf[1] & lmask) != 0xa9
	             || (buf[2] & lmask) != 0xcc)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wctomb_Test1
//API Tested :wctomb
//TestCase Description:wctomb returns -> 3 (length of the multibyte character)
//when the input wide character occupies 3 bytes after encoding it as a
//multibyte character.
//input --> wide char between 0x0800 and 0xffff
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::wctomb_Test1L()
    {
    
    INFO_PRINTF1(_L("In wctomb_Test1L"));

    //--------------------------
	char buf[MB_CUR_MAX];
	unsigned long lmask = 0x000000ff;

	memset(buf, 0xcc, sizeof(buf));

	int len = wctomb(buf, 0x2260);              

	INFO_PRINTF5(_L("{Expected: 3  e2  89  a0} %d %x %x %x"), len, buf[0], buf[1], buf[2]);
    
	if (len != 3 || (buf[0] & lmask) != 0xe2 || (buf[1] & lmask) != 0x89
	             || (buf[2] & lmask) != 0xa0)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wctomb_Test2
//API Tested :wctomb
//TestCase Description: wctomb returns -> 1 (length of the multibyte character)
//when the input wide character occupies 1 byte after encoding it as a
//multibyte character.
//input --> wide char between 0x0000 and 0x007f
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::wctomb_Test2L()
    {
    
    INFO_PRINTF1(_L("In wctomb_Test2L"));
	
	char *p = new char;	//if we don't have this variable, wctomb fails
						//as the parameter we pass here (L'A') is not going
						//to be the same as the argument wctomb receives
						//in that case.
	
    //--------------------------
	char buf[MB_CUR_MAX];
	unsigned long lmask = 0x000000ff;

	memset(buf, 0xcc, sizeof(buf));

	int len = wctomb(buf, L'A');              
    
	INFO_PRINTF4(_L("{Expected: 1  41  cc} %d %x %x"), len, buf[0], buf[1]);	
	
	delete p;
	
	if (len != 1 || (buf[0] & lmask) != 0x41 || (buf[1] & lmask) != 0xcc)
		{
		return KErrGeneral;
		}
	
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wctomb_Test3
//API Tested :wctomb
//TestCase Description:wctomb returns -> 0 (length of the multibyte character)
//when the input wide character occupies 0 bytes after encoding it as a
//multibyte character.
//input --> wide char could be anything but the MB char buffer is NULL
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::wctomb_Test3L()
    {
           
    INFO_PRINTF1(_L("In wctomb_Test3L"));

    //--------------------------
	int len = wctomb(NULL, L'A');              
    
	INFO_PRINTF2(_L("{Expected: 0} %d"), len);

	if (len != 0)
		return KErrGeneral;
	
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wctomb_Test4
//API Tested :wctomb
//TestCase Description: wctomb returns -> 1 (length of the multibyte character)
//when the input wide character occupies 1 byte after encoding it as a
//multibyte character.
//input --> wide char '\0'
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::wctomb_Test4L()
    {
    
    INFO_PRINTF1(_L("In wctomb_Test4L"));

    //--------------------------
	char buf[MB_CUR_MAX];
	unsigned long lmask = 0x000000ff;

	memset(buf, 0xcc, sizeof(buf));
	
	int len = wctomb(buf, L'\0');              
    
	INFO_PRINTF4(_L("{Expected: 1} %d %x %x"), len, buf[0], buf[1]);

	if (len != 1 || buf[0] != 0 || (buf[1] & lmask) != 0xcc)
		{
		return KErrGeneral;
		}
	
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wctomb_Test5
//API Tested :wctomb
//TestCase Description:wctomb returns -> 3 (length of the multibyte character)
//when the input MB char was an invalid MB sequence...in this case some 
//static conversion is done.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::wctomb_Test5L()
    {
    
    INFO_PRINTF1(_L("In wctomb_Test5L"));

    //--------------------------
	char buf[MB_CUR_MAX];
	unsigned long lmask = 0x000000ff;

	memset(buf, 0xcc, sizeof(buf));
	
	int len = wctomb(buf, 0xa3c1);              
    
	INFO_PRINTF5(_L("{Expected: 3, ea, 8f, 81} %d %x %x %x"), len, buf[0], buf[1], buf[2]);

	if (len != 3 || (buf[0] & lmask) != 0xea || (buf[1] & lmask) != 0x8f 
	             || (buf[2] & lmask) != 0x81)
		{
		return KErrGeneral;
		}
	
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcstombs_Test0
//API Tested :wcstombs
//TestCase Description:wcstombs returns -> 5 (number of bytes written to the 
//multibyte char string) when the input wide char string occupies 5 bytes after 
//encoding it as a multibyte char string.
//input --> wide char string with chars occupying 1 or 2 or 3 bytes in a 
//multibyte char.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::wcstombs_Test0L()
    {
    
    INFO_PRINTF1(_L("In wcstombs_Test0L"));

    //--------------------------
	wchar_t wcbuf[64];
	char mbcbuf[64];
	unsigned long lmask = 0x000000ff;

	wmemset(wcbuf, 0xcc, sizeof(wcbuf)/sizeof(*wcbuf));
	memset(mbcbuf, 0xcc, sizeof(mbcbuf));

	wcbuf[0] = (unsigned short)0x00a9;
	wcbuf[1] = (unsigned short)0x2260;
	wcbuf[2] = 0;
    	
	int len = wcstombs(mbcbuf, wcbuf, sizeof(mbcbuf));              
    
	//INFO_PRINTF2(_L("{Expected: 5, c2, a9, e2, 89, a0, 0, cc} %d %x %x %x %x %x %x %x"), 
	//len, mbcbuf[0], mbcbuf[1], mbcbuf[2], mbcbuf[3], mbcbuf[4], mbcbuf[5], mbcbuf[6]);
 
	if (len != 5 || (mbcbuf[0] & lmask) != 0xc2 || (mbcbuf[1] & lmask) != 0xa9
		         || (mbcbuf[2] & lmask) != 0xe2 || (mbcbuf[3] & lmask) != 0x89 
				 || (mbcbuf[4] & lmask) != 0xa0 || (mbcbuf[5] & lmask) != 0
				 || (mbcbuf[6] & lmask) != 0xcc)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcstombs_Test1
//API Tested :wcstombs
//TestCase Description:wcstombs returns -> 4 (number of bytes written to 
//the multibyte char string) when the input wide char string occupies 4 bytes
//after encoding it as a multibyte char string.
//input --> wide char string L"some"
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::wcstombs_Test1L()
    {
    
    INFO_PRINTF1(_L("In wcstombs_Test1L"));

    //--------------------------
	wchar_t wcbuf[64];
	char mbcbuf[64];
	unsigned long lmask = 0x000000ff;

	wmemset(wcbuf, 0xcc, sizeof(wcbuf)/sizeof(*wcbuf));
	memset(mbcbuf, 0xcc, sizeof(mbcbuf));

	wcscpy(wcbuf, L"some");
	
	int len = wcstombs(mbcbuf, wcbuf, sizeof(mbcbuf));              
    
	INFO_PRINTF3(_L("{Expected: 4, cc, \"some\"} %d %x"), len, mbcbuf[5]);

	int i = 0;
	for(i = 0; mbcbuf[i] != NULL; i++)
		{
		INFO_PRINTF2(_L(" %c"), mbcbuf[i]);
		}
	
	if (len != 4 || (mbcbuf[5] & lmask) != 0xcc || strcmp(mbcbuf, "some") != 0)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcstombs_Test2
//API Tested :wcstombs
//TestCase Description:wcstombs returns -> 3 (number of bytes written to the 
//multibyte char string) when there is no enough space in the dest buffer.
//input --> wide char string L"some" and n --> 3
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::wcstombs_Test2L()
    {
    
    INFO_PRINTF1(_L("In wcstombs_Test2L"));

    //--------------------------
	wchar_t wcbuf[64];
	char mbcbuf[64];
	unsigned long lmask = 0x000000ff;

	wmemset(wcbuf, 0xcc, sizeof(wcbuf)/sizeof(*wcbuf));
	memset(mbcbuf, 0xcc, sizeof(mbcbuf));

	wcscpy(wcbuf, L"some");
	
	int len = wcstombs(mbcbuf, wcbuf, 3);              
    
	INFO_PRINTF3(_L("{Expected: 3, cc, \"som\"} %d %x"), len, mbcbuf[3]);

	int i = 0;
	for(i = 0; i < 4; i++)
		{
		INFO_PRINTF2(_L(" %c"), mbcbuf[i]);
		}

	if (len != 3 || (mbcbuf[3] & lmask) != 0xcc || memcmp(mbcbuf, "som", 3) != 0)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcstombs_Test3
//API Tested :wcstombs
//TestCase Description:wcstombs returns -> 0 (number of bytes written to the
//multibyte char string) when the source buffer (wcbuf) is empty.
//input --> wide char string L'\0'
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::wcstombs_Test3L()
    {
    
    INFO_PRINTF1(_L("In wcstombs_Test3L"));

    //--------------------------
	wchar_t wcbuf[64];
	char mbcbuf[64];

	wmemset(wcbuf, 0xcc, sizeof(wcbuf)/sizeof(*wcbuf));
	memset(mbcbuf, 0xcc, sizeof(mbcbuf));

	wcbuf[0] = L'\0';
	
	int len = wcstombs(mbcbuf, wcbuf, sizeof(mbcbuf));              
    
	INFO_PRINTF3(_L("{Expected: 0 0} %d %x"), len, mbcbuf[0]);
   
	if (len != 0 || mbcbuf[0] != 0)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcstombs_Test4
//API Tested :wcstombs
//TestCase Description:wcstombs returns -> 16 (wcslen(wcbuf) * MB_CUR_MAX + 1)
//when the dest buffer is NULL.
//input --> wide char string L"hello" (5 * 3 + 1 = 16)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::wcstombs_Test4L()
    {
    
	
    INFO_PRINTF1(_L("In wcstombs_Test4L"));

	wchar_t wcbuf[64];

    //--------------------------	
	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
	
	wcscpy(wcbuf, L"hello");
	
	int len = wcstombs(NULL, wcbuf, sizeof(wcbuf)/sizeof(*wcbuf));              
    
	if (len != 5)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :wcstombs_Test5
//API Tested :wcstombs
//TestCase Description:wcstombs returns -> 0 (number of bytes written to the 
//multibyte char string) when the dest buffer is of zero length.
//input --> wide char string L"hello" and n --> 0
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::wcstombs_Test5L()
    {
    
    INFO_PRINTF1(_L("In wcstombs_Test5L"));

    //--------------------------
	wchar_t wcbuf[64];
	char mbcbuf[64];
	unsigned long lmask = 0x000000ff;

	wmemset(wcbuf, 0xcc, sizeof(wcbuf)/sizeof(*wcbuf));
	memset(mbcbuf, 0xcc, sizeof(mbcbuf));

	wcscpy(wcbuf, L"hello");
	
	int len = wcstombs(mbcbuf, wcbuf, 0);              
    
	INFO_PRINTF3(_L("{Expected: 0, cc} %d %x"), len, mbcbuf[0]);

	if (len != 0 || (mbcbuf[0] & lmask) != 0xcc)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mbtowc_Test0
//API Tested :mbtowc
//TestCase Description:mbtowc returns -> 3 (length of the multibyte character)
//when the input MB char occupies 3 bytes in UTF-8 encoding.
//input --> MB char 0xe2 0x89 0xa0
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mbtowc_Test0L()
    {
    
    INFO_PRINTF1(_L("In mbtowc_Test0L"));

    //--------------------------
	char buf[MB_CUR_MAX];
	wchar_t wc;
	memset(buf, 0xcc, sizeof(buf));

	buf[0] = (unsigned char)0xe2;
	buf[1] = (unsigned char)0x89;
	buf[2] = (unsigned char)0xa0;

	int len = mbtowc(&wc, buf, 3);              
    
	INFO_PRINTF3(_L("{Expected: 3 2260} %d %x"), len, wc);

	if (len != 3 || wc != 0x2260)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mbtowc_Test1
//API Tested :mbtowc
//TestCase Description:mbtowc returns -> 0 (length of the multibyte character)
//when the input MB char was converted to a NULL wide char.
//input --> MB char 0
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mbtowc_Test1L()
    {
    
    INFO_PRINTF1(_L("In mbtowc_Test1L"));

    //--------------------------
	char buf[MB_CUR_MAX];
	wchar_t wc = 0xcccc;
	memset(buf, 0xcc, sizeof(buf));

	buf[0] = 0;
	
	int len = mbtowc(&wc, buf, 3);              
    
	INFO_PRINTF3(_L("{Expected: 0 0} %d %x"), len, wc);

	if (len != 0 || wc != 0)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mbtowc_Test2
//API Tested :mbtowc
//TestCase Description:mbtowc returns -> 2 (length of the multibyte character)
//when the input MB char was converted to a wide char but when the dest is NULL.
//input --> wide char NULL
//-----------------------------------------------------------------------------
TInt CTestLibcwchar::mbtowc_Test2L()
    {
    
    INFO_PRINTF1(_L("In mbtowc_Test2L"));

    //--------------------------
	char buf[MB_CUR_MAX];
	memset(buf, 0xcc, sizeof(buf));

	buf[0] = (unsigned char)0xc2;
	buf[1] = (unsigned char)0xa9;
	
	int len = mbtowc(NULL, buf, 3);              
    
	INFO_PRINTF2(_L("{Expected: 2} %d"), len);

	if (len != 2)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mbtowc_Test3
//API Tested :mbtowc
//TestCase Description:mbtowc returns -> 2 (incomplete char sequence) when the
//input MB char was a truncated double byte.
//input --> MB char truncated double byte
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mbtowc_Test3L()
    {
    
    INFO_PRINTF1(_L("In mbtowc_Test3L"));

    //--------------------------
	char buf[MB_CUR_MAX];
	memset(buf, 0xcc, sizeof(buf));

	wchar_t wc = L'z';

	buf[0] = (unsigned char)0xc2;
	buf[1] = (unsigned char)0x42;
	
	int len = mbtowc(&wc, buf, 1);              
    
	INFO_PRINTF3(_L("{Expected: -1 'z'} %d %c"), len, wc);

	if (len != -1 || wc != L'z')
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mbtowc_Test4
//API Tested :mbtowc
//TestCase Description:mbtowc returns -> 2 (length of the multibyte character) 
//when the input MB char was an invalid MB sequence...in this case it is using
//a replacement char (0xfffd) and hence not returning an error(-1).
//input --> MB char (invalid sequence)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mbtowc_Test4L()
    {
    
    INFO_PRINTF1(_L("In mbtowc_Test4L"));

    //--------------------------
	char buf[MB_CUR_MAX];
	memset(buf, 0xcc, sizeof(buf));

	wchar_t wc = L'z';

	buf[0] = (unsigned char)0xc2;
	buf[1] = (unsigned char)0x42;
	
	int len = mbtowc(&wc, buf, 2);              
    
	if(len != (size_t) -1)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;		
		}
    }

// -----------------------------------------------------------------------------
//Function Name :mbtowc_Test5
//API Tested :mbtowc
//TestCase Description:mbtowc returns -> 1 (length of the multibyte character)
//when the input MB char occupies 3 bytes in UTF-8 encoding.
//input --> MB char 'A'
//Also another call with NULL NULL and 0 which returns 0
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mbtowc_Test5L()
    {
    
    INFO_PRINTF1(_L("In mbtowc_Test5L"));

    //--------------------------
	wchar_t wc;
	char buf[MB_CUR_MAX];
	memset(buf, 0xcc, sizeof(buf));

	buf[0] = 'A';
		
	int len1 = mbtowc(&wc, buf, 1);   
	
	int len2 = mbtowc(NULL, NULL, 0);   
    
	INFO_PRINTF4(_L("{Expected: 1 'A' 0} %d %c %d"), len1, wc, len2);

	if (len1 != 1 || wc != L'A' || len2 != 0)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mbstowcs_Test0
//API Tested :mbstowcs
//TestCase Description:mbstowcs returns -> 2 (number of wide chars that make
//up the wide char string) when the input MB char string occupies 5 bytes.
//input --> MB char string with chars "0xc2 0xa9 0xe2 0x89 0xa0 0x00"
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mbstowcs_Test0L()
    {
    
    INFO_PRINTF1(_L("In mbstowcs_Test0L"));

    //--------------------------
	wchar_t wcbuf[64];
	char mbcbuf[64];

	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
	memset(mbcbuf, 0xcc, sizeof(mbcbuf));

    mbcbuf[0] = (unsigned char)0xc2;
    mbcbuf[1] = (unsigned char)0xa9;
	mbcbuf[2] = (unsigned char)0xe2;
	mbcbuf[3] = (unsigned char)0x89;
	mbcbuf[4] = (unsigned char)0xa0;
	mbcbuf[5] = 0;

	int len = mbstowcs(wcbuf, mbcbuf, sizeof(wcbuf)/sizeof(*wcbuf));              
    
	INFO_PRINTF5(_L("{Expected: 2 a9 2260 0} %d %x %x %x"), len, wcbuf[0], wcbuf[1], wcbuf[2]);

	if (len != 2 || wcbuf[0] != 0xa9 || wcbuf[1] != 0x2260 || wcbuf[2] != 0)		         
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mbstowcs_Test1
//API Tested :mbstowcs
//TestCase Description:mbstowcs returns -> 4 (number of wide chars that make
//up the wide char string) when the input MB char string is of length 4.
//input --> MB char string "some".
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mbstowcs_Test1L()
    {
    
    INFO_PRINTF1(_L("In mbstowcs_Test1L"));

    //--------------------------
	wchar_t wcbuf[64];
	char mbcbuf[64];

	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
	memset(mbcbuf, 0xcc, sizeof(mbcbuf));

	strcpy(mbcbuf, "some");
	
	int len = mbstowcs(wcbuf, mbcbuf, sizeof(wcbuf)/sizeof(*wcbuf));              
    
	INFO_PRINTF3(_L("{Expected: 4 cccc \"some\"} %d %x"), len, wcbuf[5]);

	int i = 0;
	for(i = 0; wcbuf[i] != NULL; i++)
		{
		INFO_PRINTF2(_L(" %c"), wcbuf[i]);
		}

	if (len != 4 || wcbuf[5] != 0xcccc || wcscmp(wcbuf, L"some") != 0)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mbstowcs_Test2
//API Tested :mbstowcs
//TestCase Description:mbstowcs returns -> 3 (number of wide chars that make
//up the wide char string) when the input MB char string is of length 4 but 
//not enough space in dest buffer.
//input --> MB char string "some" and n --> 3
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mbstowcs_Test2L()
    {
    
    INFO_PRINTF1(_L("In mbstowcs_Test2L"));

    //--------------------------
	wchar_t wcbuf[64];
	char mbcbuf[64];

	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
	memset(mbcbuf, 0xcc, sizeof(mbcbuf));

	strcpy(mbcbuf, "some");
	
	int len = mbstowcs(wcbuf, mbcbuf, 3);              
    
	INFO_PRINTF3(_L("{Expected: 3 cccc \"som\"} %d %x"), len, wcbuf[3]);

	int i = 0;
	for(i = 0; i < 4; i++)
		{
		INFO_PRINTF2(_L(" %c"), wcbuf[i]);
		}

	if (len != 3 || wcbuf[3] != 0xcccc || wmemcmp(wcbuf, L"som", 3) != 0)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mbstowcs_Test3
//API Tested :mbstowcs
//TestCase Description:mbstowcs returns -> 0 (number of wide chars that make
//up the wide char string) when the source buffer (mbcbuf) is empty.
//input --> MB char string '\0'
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mbstowcs_Test3L()
    {
    
    INFO_PRINTF1(_L("In mbstowcs_Test3L"));

    //--------------------------
	wchar_t wcbuf[64];
	char mbcbuf[64];

	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
	memset(mbcbuf, 0xcc, sizeof(mbcbuf));

	mbcbuf[0] = '\0';
	
	int len = mbstowcs(wcbuf, mbcbuf, sizeof(wcbuf)/sizeof(*wcbuf));              
    
	INFO_PRINTF3(_L("{Expected: 0 0} %d %x"), len, wcbuf[0]);
    
	if (len != 0 || wcbuf[0] != 0)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mbstowcs_Test4
//API Tested :mbstowcs
//TestCase Description:mbstowcs returns -> 6 (strlen(mbcbuf) + 1)
//when the dest buffer is NULL.
//input --> MB char string "hello" (5 + 1 = 6)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mbstowcs_Test4L()
    {
    
    INFO_PRINTF1(_L("In mbstowcs_Test4L"));

    //--------------------------
	wchar_t wcbuf[64];
	char mbcbuf[64];

	wmemset(wcbuf, 0xccc, sizeof(wcbuf)/sizeof(*wcbuf));
	
	strcpy(mbcbuf, "hello");
	
	int len = mbstowcs(NULL, mbcbuf, sizeof(wcbuf)/sizeof(*wcbuf));              
    
	INFO_PRINTF2(_L("{Expected: 5} %d"), len);
   
	if (len != 5)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :mbstowcs_Test5
//API Tested :mbstowcs
//TestCase Description:mbstowcs returns -> 0 (number of wide chars that make
//up the wide char string) when the dest buffer is of zero length.
//input --> MB char string "hello" and n --> 0
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::mbstowcs_Test5L()
    {
    
    INFO_PRINTF1(_L("In mbstowcs_Test5L"));

    //--------------------------
	wchar_t wcbuf[64];
	char mbcbuf[64];

	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
	memset(mbcbuf, 0xcc, sizeof(mbcbuf));

	strcpy(mbcbuf, "hello");
	
	int len = mbstowcs(wcbuf, mbcbuf, 0);              
    
	INFO_PRINTF3(_L("{Expected: 0 cccc} %d %x"), len, wcbuf[0]);

	if (len != 0 || wcbuf[0] != 0xcccc)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }


//1_15_2_2400 wcrtomb verify that unicode characters in the range U+0  to U+7F are encoded properly    
TInt CTestLibcwchar::wcrtomb2400(  )
	{
	//verify that mbchar is updated correctly with the UTF8 version of 
	//the input character. Return value is 1
	char mbBuff[MB_CUR_MAX+1];
	wchar_t wc;
	for(short i =0; i< 0x80; i++)
		{
		mbstate_t state;
		wcrtomb(NULL,L'\0',&state);
		wc = (wchar_t) i;
		size_t retval = wcrtomb (mbBuff, wc, &state);
		if(retval != 1 || mbBuff[0] != (char) i)
			{	
			return i;
			}
		}
	
	return KErrNone;
	}

//1_15_2_2401 wcrtomb verify that unicode characters in the range U+80 to  U+7FF are encoded properly
TInt CTestLibcwchar::wcrtomb2401(  )
	{
	//verify that mbchar is updated correctly with the UTF8 version of 
	//the input character. Return value is 2.
	char mbBuff[MB_CUR_MAX+1];
	wchar_t wc;
	int divident, remainder;
	for(short i = 0x80; i< 0x800; i++)
		{
		mbstate_t state;
		wcrtomb(NULL,L'\0',&state);
		wc = (wchar_t) i;
		divident = i/64;
		remainder = i % 64;
		assert(divident*64 +remainder == i);
		size_t retval = wcrtomb (mbBuff, wc, &state);
		if(retval != 2 
		|| mbBuff[0] != (char) 192 + divident
		|| mbBuff[1] != (char) 128 + remainder)
			{	
			return i;
			}
		}
	//
	return KErrNone;
	}

//1_15_2_2402 wcrtomb verify that unicode characters in the range U+800 to  U+D800 and from U+ E000 to U+FFFF are encoded properly
TInt CTestLibcwchar::wcrtomb2402(  )
	{
	//verify that mbchar is updated correctly with the UTF8 version of 
	//the input character. Return value is 3.
	char mbBuff[MB_CUR_MAX+1];
	wchar_t wc;
	char byte1, byte2, byte3;
	for(unsigned short i = 0x800; i< 0xd800; i++)
		{
		mbstate_t state;
		wcrtomb(NULL,L'\0',&state);
		wc = (wchar_t) i;
		byte1 =  224 + i/4096;
		int t = i/64;
		byte2 = 128 +(t % 64);
		byte3 = 128 + (i % 64);
		size_t retval = wcrtomb (mbBuff, wc, &state);
		if(retval != 3 
		|| mbBuff[0] != byte1
		|| mbBuff[1] != byte2
		|| mbBuff[2] != byte3)
			{	
			return i;
			}
		}
	
	for(unsigned short i = 0xe000; i< 0xfdd0; i++)
		{
		mbstate_t state;
		wcrtomb(NULL,L'\0',&state);
		wc = (wchar_t) i;
		byte1 =  224 + i/4096;
		int t = i/64;
		byte2 = 128 +(t % 64);
		byte3 = 128 + (i % 64);
		size_t retval = wcrtomb (mbBuff, wc, &state);
		if(retval != 3 
		|| mbBuff[0] != byte1
		|| mbBuff[1] != byte2
		|| mbBuff[2] != byte3)
			{	
			return i;
			}
		}
	
	return KErrNone;
	}

//1_15_2_2403 wcrtomb passing NULL as value to mbchar parameter
TInt CTestLibcwchar::wcrtomb2403(  )
	{
	//verify that mbstate_t pointer parameter ps is updated to 
	//initial state. 
	//input parameter wc is ignored. 
	//return value depends on the state of the passed mbstate_t pointer parameter ps
	
	mbstate_t state;
	size_t retval;
		
	wchar_t wc1 = 65;
	memset(&state,2, sizeof (state));
	retval = wcrtomb(NULL,wc1,&state);
	if(mbsinit(&state) == 0 || retval != 1)
		{
		return KErrGeneral;
		}
	
	wchar_t wc2 = 0x81;
	memset(&state,2, sizeof (state));
	retval = wcrtomb(NULL,wc2,&state);
	if(mbsinit(&state) == 0 || retval != 1)
		{
		return KErrGeneral;
		}
	
	wchar_t wc3 = 0x880;
	memset(&state,2, sizeof (state));
	retval = wcrtomb(NULL,wc3,&state);
	if(mbsinit(&state) == 0 || retval != 1)
		{
		return KErrGeneral;
		}
	
	return KErrNone;
	}

//1_15_2_2404 wcrtomb passing NULL as value to wc input parameter
TInt CTestLibcwchar::wcrtomb2404(  )
	{
	//verify that ps in initial state, 
	//mbchar updated with the shift sequence required 
	//to go to initial state and a terminating NULL written out. 
	//Return value is number of bytes written.
	mbstate_t state;
	size_t retval;
	memset(&state,2, sizeof (state));
	
	char mbBuff[MB_CUR_MAX+1];
	wchar_t wc = L'\0';
	retval = wcrtomb(mbBuff,wc,&state);
	if(mbsinit(&state) == 0 || retval != 1 || mbBuff[0] != '\0')
		{
		return KErrGeneral;
		}
	
	return KErrNone;
	}

//1_15_2_2405 wcrtomb passing NULL as value to ps input parameter
TInt CTestLibcwchar::wcrtomb2405(  )
	{
	//Verify that internal static anonymous mbstate_t 
	//variable gets affected as per the input. 
	//mbchar and return value are as expected for 
	//the input value of the wc parameter.
	char mbBuff[MB_CUR_MAX+1];
	size_t retval;
	
	wchar_t wc1 = 65;
	retval = wcrtomb(mbBuff,wc1,NULL);
	if(retval != 1 || mbBuff[0] != 65)
		{
		return KErrGeneral;
		}
	
	wchar_t wc2 = 0x81;
	retval = wcrtomb(mbBuff,wc2,NULL);
	if(retval != 2
	|| mbBuff[0] != (char) 0xC2
	|| mbBuff[1] != (char) 0x81)
		{
		return KErrGeneral;
		}
	
	wchar_t wc3 = 0x880;
	retval = wcrtomb(mbBuff,wc3,NULL);
	if(retval != 3
	|| mbBuff[0] != (char) 0xE0
	|| mbBuff[1] != (char) 0xA2
	|| mbBuff[2] != (char) 0x80)
		{
		return KErrGeneral;
		}
	
	return KErrNone;
	}

//1_15_2_2406 wcrtomb passing  ps with invalid state information
TInt CTestLibcwchar::wcrtomb2406(  )
	{
	//Verify that return value is -1, errno set to EINVAL, mbchar not written 
	mbstate_t state;
	char mbBuff[MB_CUR_MAX+1];
	size_t retval;
	mbBuff[0] = '\0';
	
	memset(&state,2, sizeof (state));
	
	wchar_t wc1 = 65;
	retval = wcrtomb(mbBuff,wc1,&state);
	if(retval != (size_t) -1 || errno != EINVAL || mbBuff[0] != '\0')
		{
		return KErrGeneral;
		}
	
	return KErrNone;
	
	}

//1_15_2_2407 wcrtomb passing  ps which is in different shift states
TInt CTestLibcwchar::wcrtomb2407(  )
	{
	//Verify that the output written to mbchar is different for the same input
	mbstate_t state;
	char mbBuff[MB_CUR_MAX+1];
	size_t retval;
	mbBuff[0] = '\0';
	
	wcrtomb(NULL,L'\0',&state);
	
	wchar_t wc1 = 65;
	retval = wcrtomb(mbBuff,wc1,&state);
	if(retval != 1 || mbBuff[0] != 'A')
		{
		return KErrGeneral;
		}
	
	wc1 = 0xDC00 + 5;
	retval = wcrtomb(mbBuff,wc1,&state);
	if(retval != (size_t) -1 || errno != EILSEQ || mbBuff[0] != 'A')
		{
		return KErrGeneral;
		}
	
	wc1 = 0xD800 + 5;
	retval = wcrtomb(mbBuff,wc1,&state);
	if(retval != 0 || mbBuff[0] != 'A')
		{
		return KErrGeneral;
		}
	
	wc1 = 65;
	retval = wcrtomb(mbBuff,wc1,&state);
	if(retval != (size_t) -1 || errno != EILSEQ || mbBuff[0] != 'A')
		{
		return KErrGeneral;
		}
	
	wc1 = 0xD800 + 5;
	retval = wcrtomb(mbBuff,wc1,&state);
	if(retval != (size_t) -1 || errno != EILSEQ || mbBuff[0] != 'A')
		{
		return KErrGeneral;
		}
	wc1 = 0xDC00 + 5;
	retval = wcrtomb(mbBuff,wc1,&state);
	if(retval != 4 || mbBuff[0] & 0xf0 != 0xf0)
		{
		return KErrGeneral;
		}
	
	return KErrNone;
	}

//1_15_2_2672 wcrtomb verify that unicode characters in the surrogate range U+D800 to          
TInt CTestLibcwchar::wcrtomb2672(  )
	{
	//verify that mbchar is updated correctly with the UTF8 version of 
	//the input character. Return value is 4.
	char mbBuff[MB_CUR_MAX+1];
	wchar_t wc;
	mbstate_t state;
	wcrtomb(NULL,L'\0',&state);
	wc = (wchar_t) 0xd800 + 5;;
	size_t retval = wcrtomb (mbBuff, wc, &state);
	if(retval != 0)
		{	
		return KErrGeneral;
		}
	
	wc = (wchar_t) 0xdc00 + 5;;
	retval = wcrtomb (mbBuff, wc, &state);
	if(retval != 4)
		{		
		return KErrGeneral;
		}
	return KErrNone;
	}

bool VerifyStream(char* /*aUtf8Stream*/, wchar_t* /*aUnicodeStream*/, int /*aLen*/)
	{
	return true;
	}

//1_15_2_2409 wcsrtombs input src string has characters in the range U+0 to U+7F
TInt CTestLibcwchar::wcsrtombs2409(  )
	{
	//verify that dest  is updated correctly with the UTF8 
	//version of the input stream. 
	//Return value is number of bytes written
	const wchar_t *tstArr = L"ABCD";
	char dstArr[(MB_CUR_MAX+1)*5];
	char* dst = (char*) dstArr;
	const wchar_t* src = tstArr;
	size_t retval;
	mbstate_t state;
	wcrtomb(NULL,L'\0',&state);
	retval = wcsrtombs (dst, &src, sizeof dstArr, &state);
	if(retval != 4 
	|| dst[0] != 'A'
	|| dst[1] != 'B'
	|| dst[4] != '\0'
	)
		{
		return KErrGeneral;
		}
	
	if(mbsinit(&state) == 0
	|| src != NULL)
		{
		return KErrGeneral;
		}
	
	return KErrNone;
	}

//1_15_2_2410 wcsrtombs input src string has characters in the range U+80 to U+7FF
TInt CTestLibcwchar::wcsrtombs2410(  )
	{	
	//verify that dest  is updated correctly with the UTF8 
	//version of the input stream. Return value is number of 
	//bytes written
	wchar_t* tstArr = new wchar_t [10];
	int i;
	for(i = 0; i < 5; i++)
		{
		tstArr[i] = 0x80 + 20 +i;
		}
	tstArr[5] = L'\0';
	
	char dstArr[(MB_CUR_MAX+1)*10];
	char* dst = (char*) dstArr;
	const wchar_t* src = tstArr;
	size_t retval;
	mbstate_t state;
	wcrtomb(NULL,L'\0',&state);
	retval = wcsrtombs (dst, &src, sizeof dstArr, &state);
	if(retval != 10)
		{
		delete tstArr;
		
		return KErrGeneral;
		}
	
	if(!VerifyStream(dst,tstArr,5))
		{
		delete tstArr;	
		
		return KErrGeneral;
		}
	
	if(mbsinit(&state) == 0
	|| src != NULL)
		{
		delete tstArr;
		
		return KErrGeneral;
		}
	delete tstArr;
	
	return KErrNone;
	}

//1_15_2_2411 wcsrtombs input src string has characters in the range U+800 to U+D800 and from U+E000 to U+FFFF
TInt CTestLibcwchar::wcsrtombs2411(  )
	{
	//verify that dest  is updated correctly with the 
	//UTF8 version of the input stream. Return value is 
	//number of bytes written
	
	wchar_t* tstArr = new wchar_t [10];
	int i;
	for(i = 0; i < 5; i++)
		{
		tstArr[i] = 0x800 + 20 +i;
		}
	tstArr[5] = L'\0';
	
	char dstArr[(MB_CUR_MAX+1)*10];
	char* dst = (char*) dstArr;
	const wchar_t* src = tstArr;
	size_t retval;
	mbstate_t state;
	wcrtomb(NULL,L'\0',&state);
	retval = wcsrtombs (dst, &src, sizeof dstArr, &state);
	if(retval != 15)
		{
		delete [] tstArr;
		
		return KErrGeneral;
		}
	
	if(!VerifyStream(dst,tstArr,5))
		{
		delete [] tstArr;
		
		return KErrGeneral;
		}
	
	if(mbsinit(&state) == 0
	|| src != NULL)
		{
		delete [] tstArr;
		
		return KErrGeneral;
		}
	delete [] tstArr;
	
	return KErrNone;
	}

//1_15_2_2412 wcsrtombs input src string with characters in the complete range U+0 to U+FFFF
TInt CTestLibcwchar::wcsrtombs2412(  )
	{
	//verify that dest  is updated correctly 
	//with the UTF8 version of the input stream. 
	//Return value is number of bytes written
	return KErrNone;
	}

//1_15_2_2413 wcsrtombs input dst parameter is NULL
TInt CTestLibcwchar::wcsrtombs2413(  )
	{
	//verify that src conversion behaviour is 
	//normal, passed len value is ignored. 
	//Errors in input are trapped. src and ps are updated as per input.
	const wchar_t *tstArr = L"ABCD";
	const wchar_t* src = tstArr;
	size_t retval;
	mbstate_t state;
	wcrtomb(NULL,L'\0',&state);
	retval = wcsrtombs (NULL, &src, 0, &state);
	if(retval != 4)
		{
		
		return KErrGeneral;
		}
	
	if(mbsinit(&state) == 0
	|| src != NULL)
		{
		
		return KErrGeneral;
		}
		
	return KErrNone;
	}

//1_15_2_2414 wcsrtombs input src parameter is NULL
TInt CTestLibcwchar::wcsrtombs2414(  )
	{
	//verify that return value is 0. dst and ps are not updated.
	char* dst = NULL;
	size_t retval;
	mbstate_t state;
	wcrtomb(NULL,L'\0',&state);
	retval = wcsrtombs(dst,NULL, 10,&state);
	if(retval != 0)
		{
		return KErrGeneral;
		}
	
	return KErrNone;
	}

//1_15_2_2415 wcsrtombs input len parameter is 0
TInt CTestLibcwchar::wcsrtombs2415(  )
	{
	//verify that return value is 0. src, dst and ps not updated.
	const wchar_t *tstArr = L"ABCD";
	char dstArr[(MB_CUR_MAX+1)*10];
	char* dst = (char*) dstArr;
	const wchar_t* src = tstArr;
	size_t retval;
	mbstate_t state;
	wcrtomb(NULL,L'\0',&state);
	retval = wcsrtombs (dst, &src, 0, &state);
	if(retval != 0 
	|| src != tstArr
	)
		{
		return KErrGeneral;
		}
	
	return KErrNone;
	}

//1_15_2_2423 wcsrtombs input ps parameter is NULL
TInt CTestLibcwchar::wcsrtombs2423(  )
	{
	//To verify that passing NULL as value to ps input 
	//parameter uses internal static anonymous mbstate_t 
	//variable which gets affected as per the input. 
	//src and return value are updated as expected for the other input values
	const wchar_t *tstArr = L"ABCD";
	char dstArr[(MB_CUR_MAX+1)*5];
	char* dst = (char*) dstArr;
	const wchar_t* src = tstArr;
	size_t retval;
	retval = wcsrtombs (dst, &src, sizeof dstArr, NULL);
	if(retval != 4 
	|| dst[0] != 'A'
	|| dst[1] != 'B'
	|| dst[4] != '\0'
	|| src != NULL
	)
		{
		return KErrGeneral;
		}
	return KErrNone;
	}

//1_15_2_2436 wcsrtombs input ps parameter is in different states
TInt CTestLibcwchar::wcsrtombs2436(  )
	{
	//Verify that  the bytes written to the 
	//dst parameter and  the return value for the 
	//same src input are different for the different shift states.
	wchar_t* tstArr = new wchar_t [10];
	tstArr[0]  = L'A';
	tstArr[1]  = 0x80 + 11;
	tstArr[2]  = 0x80 + 12;
	tstArr[3]  = L'\0';
	
	char dstArr[(MB_CUR_MAX+1)*5];
	char* dst = (char*) dstArr;
	const wchar_t* src = tstArr;
	size_t retval;
	mbstate_t state;
	wcrtomb(NULL,L'\0',&state);
	retval = wcsrtombs (dst, &src, 2, &state);
	if(retval !=1 || dst[0] != 'A')
		{
		delete tstArr;
		
		return KErrGeneral;
		}
			
	tstArr[1]  = 0x800 + 11;
	retval = wcsrtombs (dst, &src, 2, &state);
	if(retval != 0 || dst[0] != 'A')
		{
		delete tstArr;
		
		return KErrGeneral;
		}
	retval = wcsrtombs (dst, &src, 3, &state);
	if(retval != 3 )
		{
		delete tstArr;
		
		return KErrGeneral;
		}
	
	delete tstArr;
	
	return KErrNone;
	}

//1_15_2_2437 wcsrtombs input len parameter is 1, next widechar needs 1 byte for translation
TInt CTestLibcwchar::wcsrtombs2437(  )
	{
	//Verify that the src is updated to point to next char 
	//to be converted, dest is updated with wide char translated, 
	//return value is same as the number of bytes written.
	const wchar_t *tstArr = L"ABCD";
	char dstArr[(MB_CUR_MAX+1)*5];
	char* dst = (char*) dstArr;
	const wchar_t* src = tstArr;
	size_t retval;
	mbstate_t state;
	wcrtomb(NULL,L'\0',&state);
	retval = wcsrtombs (dst, &src, 1, &state);
	if(retval != 1	|| dst[0] != 'A')
		{
		return KErrGeneral;
		}
	
	if(mbsinit(&state) == 0
	|| src != tstArr +1)
		{
		return KErrGeneral;
		}
	
	return KErrNone;

	}

//1_15_2_2438 wcsrtombs input len 1, next wide char needs 2 bytes for translation.
TInt CTestLibcwchar::wcsrtombs2438(  )
	{
	//Examine return value, 
	//mbstate_t ps parameter, src and dst parameter
	wchar_t* tstArr = new wchar_t [10];
	tstArr[0]  = L'A';
	tstArr[1]  = 0x80 + 11;
	tstArr[2]  = 0x80 + 12;
	tstArr[3]  = L'\0';
	
	char dstArr[(MB_CUR_MAX+1)*5];
	char* dst = (char*) dstArr;
	const wchar_t* src = tstArr;
	size_t retval;
	mbstate_t state;
	wcrtomb(NULL,L'\0',&state);
	retval = wcsrtombs (dst, &src, 2, &state);
	if(retval != 1 || dst[0] != 'A')
		{
		delete tstArr;
		
		return KErrGeneral;
		}
	
	if(src != (tstArr +1 ) )
		{
		delete tstArr;
		
		return KErrGeneral;
		}
	delete tstArr;
	
	return KErrNone;

	}

//1_15_2_2439 wcsrtombs input len 1, next wide char needs 3 bytes for translation.
TInt CTestLibcwchar::wcsrtombs2439(  )
	{
	//Examine return value, 
	//mbstate_t ps parameter, src and dst parameter
	wchar_t* tstArr = new wchar_t [10];
	tstArr[0]  = L'A';
	tstArr[1]  = 0x800 + 11;
	tstArr[2]  = 0x800 + 12;
	tstArr[3]  = L'\0';
	
	char dstArr[(MB_CUR_MAX+1)*5];
	char* dst = (char*) dstArr;
	const wchar_t* src = tstArr;
	size_t retval;
	mbstate_t state;
	wcrtomb(NULL,L'\0',&state);
	retval = wcsrtombs (dst, &src, 2, &state);
	if(retval != 1 || dst[0] != 'A')
		{
		delete tstArr;
		
		return KErrGeneral;
		}
	
	if(src != (tstArr+1) )
		{
		delete tstArr;
		
		return KErrGeneral;
		}
	delete tstArr;
	
	return KErrNone;
	}

//1_15_2_2440 wcsrtombs input len 2, next wide char needs 3 bytes for translation.
TInt CTestLibcwchar::wcsrtombs2440(  )
	{
	//Examine return value, mbstate_t ps parameter, src and dst parameter
	wchar_t* tstArr = new wchar_t [10];
	tstArr[0]  = L'A';
	tstArr[1]  = 0x800 + 11;
	tstArr[2]  = 0x800 + 12;
	tstArr[3]  = L'\0';
	
	char dstArr[(MB_CUR_MAX+1)*5];
	char* dst = (char*) dstArr;
	const wchar_t* src = tstArr;
	size_t retval;
	mbstate_t state;
	wcrtomb(NULL,L'\0',&state);
	retval = wcsrtombs (dst, &src, 3, &state);
	if(retval != 1 || dst[0] != 'A')
		{
		delete tstArr;
		
		return KErrGeneral;
		}
	
	if(src != tstArr +1)
		{
		delete tstArr;
		
		return KErrGeneral;
		}
	delete tstArr;
	
	return KErrNone;
	}

//1_15_2_2442 wcsrtombs input len parameter is 2, next widechar needs 2 bytes for translation
TInt CTestLibcwchar::wcsrtombs2442(  )
	{
	//Verify that the src is updated to point to 
	//next char to be converted, dest is updated 
	//with wide char translated, return value is 
	//same as the number of bytes written.
	wchar_t* tstArr = new wchar_t [10];
	tstArr[0]  = 0x80 + 10;
	tstArr[1]  = 0x80 + 11;
	tstArr[2]  = 0x80 + 12;
	tstArr[3]  = L'\0';
	
	char dstArr[(MB_CUR_MAX+1)*5];
	char* dst = (char*) dstArr;
	const wchar_t* src = tstArr;
	size_t retval;
	mbstate_t state;
	wcrtomb(NULL,L'\0',&state);
	retval = wcsrtombs (dst, &src, 2, &state);
	if(retval != 2 || !VerifyStream(dst,tstArr,1))
		{
		delete tstArr;
		
		return KErrGeneral;
		}
	
	if(src != tstArr+1)
		{
		delete tstArr;
		
		return KErrGeneral;
		}
	delete tstArr;
	
	return KErrNone;
	}

//1_15_2_2444 wcsrtombs input len parameter is 3, next widechar needs 3 bytes for translation
TInt CTestLibcwchar::wcsrtombs2444(  )
	{
	//Verify that the src is updated to point to 
	//next char to be converted, dest is updated
	// with wide char translated, return value is same as the 
	//number of bytes written.
	wchar_t* tstArr = new wchar_t [10];
	tstArr[0]  = 0x800 + 10;
	tstArr[1]  = 0x800 + 11;
	tstArr[2]  = 0x800 + 12;
	tstArr[3]  = L'\0';
	
	char dstArr[(MB_CUR_MAX+1)*5];
	char* dst = (char*) dstArr;
	const wchar_t* src = tstArr;
	size_t retval;
	mbstate_t state;
	wcrtomb(NULL,L'\0',&state);
	retval = wcsrtombs (dst, &src, 3, &state);
	if(retval != 3 || !VerifyStream(dst,tstArr,1))
		{
		delete tstArr;
		
		return KErrGeneral;
		}
	
	if(src != tstArr +1)
		{
		delete tstArr;
		
		return KErrGeneral;
		}
	delete tstArr;
	
	return KErrNone;
	}

//1_15_2_2460 wcsrtombs terminating NULL char encounter in src input before len bytes written out
TInt CTestLibcwchar::wcsrtombs2460(  )
	{
	//Verify that the ps parameter is reset to the initial state, 
	//*src is NULL, dest has a terminating NULL wide char, 
	//return value is the number of bytes written.
	const wchar_t *tstArr = L"ABCD";
	char dstArr[(MB_CUR_MAX+1)*5];
	char* dst = (char*) dstArr;
	const wchar_t* src = tstArr;
	size_t retval;
	mbstate_t state;
	wcrtomb(NULL,L'\0',&state);
	retval = wcsrtombs (dst, &src, 10, &state);
	if(retval != 4 
	|| dst[0] != 'A'
	|| dst[1] != 'B'
	|| dst[4] != '\0'
	)
		{
		return KErrGeneral;
		}
	
	if(mbsinit(&state) == 0
	|| src != NULL)
		{
		return KErrGeneral;
		}
	
	return KErrNone;

	}


//1_15_2_2673 wcsrtombs input src string has characters in the surrogate range U+D800 to U+DFFF
TInt CTestLibcwchar::wcsrtombs2673(  )
	{
	return KErrNone;
	}
	
	
	
	


//Test cases for the API towlower  :: Start

/* ----------------------------------------------------------------------------
	Function Name			:	testtowlower1L
	API Tested				:	towlower()
	TestCase Description	:	convert a upper case wide char to a lower char.
   ---------------------------------------------------------------------------*/


TInt CTestLibcwchar::testtowlower1L(  )
	{	 
 	wint_t ucwc = L'M';
 	wint_t lcwc;
 	
 	lcwc = towlower(ucwc);
 	
 	if(lcwc == L'm')
		{
 		return KErrNone;
		}
 	else
		{
 		return KErrGeneral;
		}
	}


/* ----------------------------------------------------------------------------
	Function Name			:	testtowlower2L
	API Tested				:	towlower()
	TestCase Description	:	this should not do any conversion as the argument
	passed is in lower case only.
   ---------------------------------------------------------------------------*/

TInt CTestLibcwchar::testtowlower2L(  )
	{ 
 	wint_t ucwc = L'm';
 	wint_t lcwc;
 	
 	lcwc = towlower(ucwc);
 	
 	if(lcwc == L'm')
 		{
 		return KErrNone;
		}
 	else
		{
 		return KErrGeneral;
		}	
	}

/* ----------------------------------------------------------------------------
	Function Name			:	testtowlower3L
	API Tested				:	towlower()
	TestCase Description	:	this should return WEOF only as its arg is WEOF.
   ---------------------------------------------------------------------------*/

TInt CTestLibcwchar::testtowlower3L(  )
	{	 
 	wint_t ucwc = WEOF;
 	wint_t lcwc;
 	
 	lcwc = towlower(ucwc);
 	
 	if(lcwc == WEOF)
 		{
 		return KErrNone;
		}
 	else
		{
 		return KErrGeneral;
		}
	}

//Test cases for the API towlower  :: End



//Test cases for the API towupper  :: Start

/* ----------------------------------------------------------------------------
	Function Name			:	testtowupper1L
	API Tested				:	towupper()
	TestCase Description	:convert a lower case wide char to a upper case char.
   ---------------------------------------------------------------------------*/


TInt CTestLibcwchar::testtowupper1L(  )
	{	 
 	wint_t lwc = L'm';
 	wint_t uwc;
 	
 	uwc = towupper(lwc);
 	
 	if(uwc == L'M')
		{
 		return KErrNone;
		}
 	else
		{
 		return KErrGeneral;
		}
	}


/* ----------------------------------------------------------------------------
	Function Name			:	testtowupper2L
	API Tested				:	towupper()
	TestCase Description	:	this should not do any conversion as the argument
	passed is in upper case only.
   ---------------------------------------------------------------------------*/

TInt CTestLibcwchar::testtowupper2L(  )
	{	 
 	wint_t wc = L'M';
 	wint_t uwc;
 	
 	uwc = towupper(wc);
 	
 	if(uwc == L'M')
		{
 		return KErrNone;
		}
 	else
		{
 		return KErrGeneral;
		}
	}

/* ----------------------------------------------------------------------------
	Function Name			:	testtowupper3L
	API Tested				:	towupper()
	TestCase Description	:	this should return WEOF only as its arg is WEOF.
   ---------------------------------------------------------------------------*/

TInt CTestLibcwchar::testtowupper3L(  )
	{	 
 	wint_t wc = WEOF;
 	wint_t uwc;
 	
 	uwc = towlower(wc);
 	
 	if(uwc == WEOF)
 		{
 		return KErrNone;
		}
 	else
		{
 		return KErrGeneral;
		}
	}

//Test cases for the API towlower  :: End



//mbsrtowcs API test cases :: Start


// -----------------------------------------------------------------------------
//Function Name :testmbsrtowcs1
//API Tested :mbsrtowcs
//TestCase Description:mbsrtowcs returns -> 4 (number of wide chars that make
//up the wide char string) when the input MB char string is of length 4.
//input --> MB char string "some".
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbsrtowcs1L()
    {
	wchar_t wcbuf[64];
	const char *mbcbuf = "some";
	unsigned int len1;
	mbstate_t *ps = NULL;

	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
		
	len1 = sizeof(wcbuf)/sizeof(*wcbuf);
	
	int len = mbsrtowcs(wcbuf,&mbcbuf,len1,ps);              
	    
	INFO_PRINTF3(_L("{Expected: 4 cccc \"some\"} %d %x"), len, wcbuf[5]);

	int i = 0;
	for(i = 0; wcbuf[i] != NULL; i++)
		{
		INFO_PRINTF2(_L(" %c"), wcbuf[i]);
		}

	if (len != 4 || wcbuf[5] != 0xcccc || wcscmp(wcbuf, L"some") != 0)
		{
		return KErrGeneral;
		}

	return KErrNone;
	
    }


// -----------------------------------------------------------------------------
//Function Name :testmbsrtowcs2
//API Tested :mbsrtowcs
//TestCase Description:mbsrtowcs returns -> 3 (number of wide chars that make
//up the wide char string) when the input MB char string is of length 4 but 
//not enough space in dest buffer.
//input --> MB char string "some" and n --> 3
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbsrtowcs2L()
    {
	wchar_t wcbuf[64];
	const char *mbcbuf = "some";
	unsigned int len1;
	mbstate_t *ps = NULL;

	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
		
	len1 = 3;
		
	int len = mbsrtowcs(wcbuf, &mbcbuf, len1,ps);              
	    
	INFO_PRINTF3(_L("{Expected: 3 cccc \"som\"} %d %x"), len, wcbuf[3]);

	int i = 0;
	for(i = 0; i < 4; i++)
		{
		INFO_PRINTF2(_L(" %c"), wcbuf[i]);
	    }

	if (len != 3 || wcbuf[3] != 0xcccc || wmemcmp(wcbuf, L"som", 3) != 0)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testmbsrtowcs3
//API Tested :mbsrtowcs
//TestCase Description:mbsrtowcs returns -> 0 (number of wide chars that make
//up the wide char string) when the source buffer (mbcbuf) is empty.
//input --> MB char string '\0'
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbsrtowcs3L()
    {
	wchar_t wcbuf[64];
	const char *mbcbuf = "";
	unsigned int len1;
	mbstate_t *ps = NULL;

	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
		
	len1 = 4;
		
	int len = mbsrtowcs(wcbuf, &mbcbuf,len1,ps);              
	    
	INFO_PRINTF3(_L("{Expected: 0 0} %d %x"), len, wcbuf[0]);
	    
	if (len != 0 || wcbuf[0] != 0)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testmbsrtowcs4
//API Tested :mbsrtowcs
//TestCase Description:mbsrtowcs returns -> 6 (strlen(mbcbuf) + 1)
//when the dest buffer is NULL.
//input --> MB char string "hello" (5)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbsrtowcs4L()
    {
	const char *mbcbuf = "hello";
	unsigned int len1;
	mbstate_t *ps = NULL;

	len1 = 5;
		
	int len = mbsrtowcs(NULL, &mbcbuf, len1,ps);
	    
	INFO_PRINTF2(_L("{Expected: 5} %d"), len);
	   
	if (len != 5)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testmbsrtowcs5
//API Tested :mbsrtowcs
//TestCase Description:mbsrtowcs returns -> 0 (number of wide chars that make
//up the wide char string) when the dest buffer is of zero length.
//input --> MB char string "hello" and n --> 0
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbsrtowcs5L()
    {
	wchar_t wcbuf[64];
	const char *mbcbuf = "hello";
	unsigned int len1;
	mbstate_t *ps = NULL;

	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
		
	len1 = 0;
		
	int len = mbsrtowcs(wcbuf, &mbcbuf, len1,ps);              
	    
	INFO_PRINTF3(_L("{Expected: 0 cccc} %d %x"), len, wcbuf[0]);

	if (len != 0 || wcbuf[0] != 0xcccc)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

//mbsrtowcs API test cases :: End


//mbrtowc API test cases :: Start



// -----------------------------------------------------------------------------
//Function Name :testmbrtowc1
//API Tested :mbrtowc
//TestCase Description:mbrtowc returns -> 1 as it parses first multibyte i.e.
// 's' and converts it to wide character.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrtowc1L()
    {
	wchar_t wcbuf[64];
	const char *mbcbuf = "some";
	unsigned int len1;
	mbstate_t *ps = NULL;

	len1 = 1;
		
	int len = mbrtowc(wcbuf,mbcbuf,len1,ps); 
		
	if (len != 1 || wcbuf[0] != L's' && ps != NULL)
		{
		return KErrGeneral;
		}
	return KErrNone;
	
    }


// -----------------------------------------------------------------------------
//Function Name :testmbrtowc2
//API Tested :mbrtowc
//TestCase Description:mbrtowc returns -> -1 and sets errno to EILSEQ as the 
// multibyte sequence passed is invalid one.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrtowc2L()
    {
	wchar_t wcbuf[64];
	char mbcbuf[64];
	unsigned int len1 = 3;
	mbstate_t *ps = NULL;
	memset(mbcbuf, 0xcc, sizeof(mbcbuf));
		
	mbcbuf[0] = 0xea ;
	mbcbuf[1] = 0x8f ;
	mbcbuf[2] = 0xf1 ;
				
	int len = mbrtowc(wcbuf,mbcbuf, len1,ps);              
	   
	if (len != -1 && errno != EILSEQ)
		{
		return KErrGeneral;
		}	
	return KErrNone;
		
    }

// -----------------------------------------------------------------------------
//Function Name :testmbrtowc3
//API Tested :mbrtowc
//TestCase Description:mbrtowc returns -> 0 (number of wide chars that make
//up the wide char string) when the source buffer (mbcbuf) is empty.
//input --> MB char string '\0'
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrtowc3L()
    {
	wchar_t wcbuf[64];
	const char *mbcbuf = "";
	unsigned int len1;
	mbstate_t *ps = NULL;

	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
		
	len1 = 4;
		
	int len = mbrtowc(wcbuf, mbcbuf,len1,ps);              
	    
	INFO_PRINTF3(_L("{Expected: 0 0} %d %x"), len, wcbuf[0]);
	    
	if (len != 0 || wcbuf[0] != 0)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testmbrtowc4
//API Tested :mbrtowc
//TestCase Description:mbrtowc returns -> 1 (number of wide chars that make
//up the wide char string) when the source buffer (mbcbuf) contains 0x41.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrtowc4L()
    {
	wchar_t wcbuf[64];
	char mbcbuf[64];
	unsigned int len1;
	mbstate_t *ps = NULL;

	memset(mbcbuf, 0xcc, sizeof(mbcbuf)/sizeof(*mbcbuf));
		
	len1 = 1;
	mbcbuf[0] = 0x41;
		
	int len = mbrtowc(wcbuf, mbcbuf,len1,ps);              
	    
	INFO_PRINTF3(_L("{Expected: 0 0} %d %x"), len, wcbuf[0]);
	    
	if (len != 1 || wcbuf[0] != L'A')
		{
		return KErrGeneral;
		}

	return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :testmbrtowc5
//API Tested :mbrtowc
//TestCase Description:mbrtowc returns -> 0 (number of wide chars that make
//up the wide char string) when the dest buffer is of zero length.
//input --> MB char string "hello" and n --> 0
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrtowc5L()
    {
	wchar_t wcbuf[64];
	const char *mbcbuf = "hello";
	unsigned int len1;
	mbstate_t *ps = NULL;

	wmemset(wcbuf, 0xcccc, sizeof(wcbuf)/sizeof(*wcbuf));
		
	len1 = 0;
		
	int len = mbrtowc(wcbuf, mbcbuf, len1,ps);  
		
	INFO_PRINTF3(_L("{Expected: 0 cccc} %d %x"), len, wcbuf[0]);

	if (len != 0)
		{
		return KErrGeneral;
		}

	return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :testmbrtowc6
//API Tested :mbrtowc
//TestCase Description:mbrtowc returns -> 3 as the multibyte sequence passed for
// conversion is valid one.
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrtowc6L()
    {
	wchar_t wcbuf[64];
	char mbcbuf[64];
	unsigned int len1 = 3;
	mbstate_t *ps = NULL;

	memset(mbcbuf, 0xcc, sizeof(mbcbuf));
		
	mbcbuf[0] = 0xea ;
	mbcbuf[1] = 0x8f ;
	mbcbuf[2] = 0x81 ;
				
	int len = mbrtowc(wcbuf,mbcbuf, len1,ps);              
	    
	if(len != 3 || wcbuf[0] != 0xa3c1)
		{
		return KErrGeneral;
		}
		
	return KErrNone;
		
    }




//mbrtowc API test cases :: End


//Test cases for the API btowc  :: Start

/* ----------------------------------------------------------------------------
	Function Name			:	testbtowc1L
	API Tested				:	btowc()
	TestCase Description	:	convert a single byte to a wide character
   ---------------------------------------------------------------------------*/

TInt CTestLibcwchar::testbtowc1L(  )
	{	 
 	int c = 'a';
 	wint_t wc;
 	
 	wc = btowc(c);
 	
 	if(wc == L'a')
		{
 		return KErrNone;
		}
 	else
		{
 		return KErrGeneral;
		}
	}

/* ----------------------------------------------------------------------------
	Function Name			:	testbtowc2L
	API Tested				:	btowc()
	TestCase Description	:	convert a single byte to a wide character.
	when single byte char is EOF then it should return WEOF.
   ---------------------------------------------------------------------------*/

TInt CTestLibcwchar::testbtowc2L(  )
	{	 
 	int c = EOF;
 	wint_t wc;
 	
 	wc = btowc(c);
 	
 	if(wc == WEOF)
 		{
 		return KErrNone;
		}
 	else
		{
 		return KErrGeneral;
		}
	}

/* ----------------------------------------------------------------------------
	Function Name			:	testbtowc3L
	API Tested				:	btowc()
	TestCase Description	:	convert a single byte to a wide character.
	if c is not a valid multibyte sequence of length 1 then it should return WEOF.
   ---------------------------------------------------------------------------*/

TInt CTestLibcwchar::testbtowc3L(  )
	{	 
 	int c = 1234;
 	wint_t wc;
 	
 	wc = btowc(c);
 	
 	if(wc == WEOF)
		{
 		return KErrNone;
		}
 	else
		{
 		return KErrGeneral;
		}
	}

//Test cases for the API btowc  :: End


//Test cases for the API wctob  :: Start

/* ----------------------------------------------------------------------------
	Function Name			:	testwctob1L
	API Tested				:	wctob()
	TestCase Description	:	convert a wide character to single byte. 
   ---------------------------------------------------------------------------*/

TInt CTestLibcwchar::testwctob1L(  )
	{	 
 	wint_t wc = L'a';
 	int c;
 	
 	c = wctob(wc);
 	
 	if(c == 'a')
 		{
 		return KErrNone;
		}
 	else
		{
 		return KErrGeneral;
		}
	}

/* ----------------------------------------------------------------------------
	Function Name			:	testwctob2L
	API Tested				:	wctob()
	TestCase Description	:	convert a wide character a single byte. 
	when wide char is WEOF then it should return EOF.
   ---------------------------------------------------------------------------*/

TInt CTestLibcwchar::testwctob2L(  )
	{	 
 	wint_t wc = WEOF;
 	int c;
 	
 	c = wctob(wc);
 
 	if(c == EOF)
 		{
 		return KErrNone;
		}
 	else
		{
 		return KErrGeneral;
		}
	}

/* ----------------------------------------------------------------------------
	Function Name			:	testwctob3L
	API Tested				:	wctob()
	TestCase Description	:	convert a wide character to a single byte.
	if we cannot convert wide char to single byte then it should return EOF.
   ---------------------------------------------------------------------------*/

TInt CTestLibcwchar::testwctob3L(  )
	{	 
 	wint_t wc = 1234;
 	int c;
 	
 	c = wctob(wc);
 	
 	if(c == EOF)
 		{
 		return KErrNone;
		}
 	else
		{
 		return KErrGeneral;
		}
	}

//Test cases for the API wctob  :: End



//Test cases for the API mbrlen  :: Start

// -----------------------------------------------------------------------------
//Function Name :testmbrlen0
//API Tested :mbrlen
//TestCase Description:Multibyte char length -> 2 (with n = 2 and n = 0)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrlen0L()
    {
    
	wchar_t wc;
	char mbc[MB_CUR_MAX];
	mbstate_t *ps = NULL;

	wc = (unsigned short)0x0509;
  
    wctomb( mbc, wc );
   
    int i = mbrlen( mbc, MB_CUR_MAX, ps );
	int j = mbrlen( mbc, 0, ps );
    
	INFO_PRINTF3(_L("{Expected: 2, 0} %d %d"), i, j);
	
	if (i != 2 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :testmbrlen1
//API Tested :mbrlen
//TestCase Description:Multibyte char length -> 1 (with n = 2 and n = 0)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrlen1L()
    {
    mbstate_t *ps = NULL;
	wchar_t wc = L'a';
	char mbc[MB_CUR_MAX];
		   
    wctomb( mbc, wc );
   
    int i = mbrlen( mbc, MB_CUR_MAX,ps );
	int j = mbrlen( mbc, 0, ps );

    INFO_PRINTF3(_L("{Expected: 1, 0} %d %d"), i, j);

	if (i != 1 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testmbrlen2
//API Tested :mbrlen
//TestCase Description:mbrlen returns -> 0 when the input is a NULL char 
//(with n = 2 and n = 0)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrlen2L()
    {
    
    mbstate_t *ps = NULL;
	wchar_t wc = L'\0';
	char mbc[MB_CUR_MAX];
	   
    wctomb( mbc, wc );
   
    int i = mbrlen( mbc, MB_CUR_MAX, ps );
	int j = mbrlen( mbc, 0, ps );

	INFO_PRINTF3(_L("{Expected: 0, 0} %d %d"), i, j);

	if (i != 0 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testmbrlen3
//API Tested :mbrlen
//TestCase Description:mbrlen returns -> 2 (MB char string with each char len 2)
//(with n = sizeof MB char string and n = 0)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrlen3L()
    {
    mbstate_t *ps = NULL;
	wchar_t ws[64];
	char mbs[64];

	ws[0] = (unsigned short)0x0509;
	ws[1] = (unsigned short)0x0609;
	ws[2] = (unsigned short)0x0709;
	ws[3] = (unsigned short)0x0809;
	ws[4] = (unsigned short)0x0909;

    wcstombs( mbs, ws, 5 );
   
    int i = mbrlen( mbs, MB_CUR_MAX, ps );
	int j = mbrlen( mbs, 0, ps );

	INFO_PRINTF3(_L("{Expected: 2, 0} %d %d"), i, j);

	if (i != 2 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testmbrlen4
//API Tested :mbrlen
//TestCase Description:mbrlen returns -> 1 (MB char string with each char len 1)
//(with n = sizeof MB char string and n = 0)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrlen4L()
    {
    mbstate_t *ps = NULL;
	wchar_t ws[] = L"widearray";
	
	char    *pmbs = (char *)malloc( sizeof( ws ) );
    
    wcstombs( pmbs, ws, sizeof( ws ) );
   
    int i = mbrlen( pmbs, MB_CUR_MAX, ps );
	int j = mbrlen( pmbs, 0, ps );

    free (pmbs);
	
	INFO_PRINTF3(_L("{Expected: 1, 0} %d %d"), i, j);

	if (i != 1 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testmbrlen5
//API Tested :mbrlen
//TestCase Description:mbrlen returns -> 0 (NULL MB char string)
//(with n = sizeof MB char string and n = 0)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrlen5L()
    {
    mbstate_t *ps = NULL;
	wchar_t ws[] = L"\0";
	
	char    *pmbs = (char *)malloc( sizeof( ws ) );
    
    wcstombs( pmbs, ws, sizeof( ws ) );
   
    int i = mbrlen( pmbs, sizeof( wchar_t ), ps );
	int j = mbrlen( pmbs, 0, ps );

    free (pmbs);
	
	INFO_PRINTF3(_L("{Expected: 0, 0} %d %d"), i, j);

	if (i != 0 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testmbrlen6
//API Tested :mbrlen
//TestCase Description:mbrlen returns -> -2 (no complete multibyte character)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrlen6L()
    {
    mbstate_t *ps = NULL;
    wchar_t wc;
	char mbc[MB_CUR_MAX];

	wc = (unsigned short)0xff09; 
   
    wctomb( mbc, wc );
          
    int i = mbrlen( mbc, 1 ,ps );
	int j = mbrlen( mbc, 0 ,ps );

	INFO_PRINTF3(_L("{Expected: -1, 0} %d %d"), i, j);

	if (i != -2 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testmbrlen7
//API Tested :mbrlen
//TestCase Description:mbrlen returns -> -1 (incomplete MB sequence)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrlen7L()
    {
    mbstate_t *ps = NULL;
    wchar_t wc;
	char mbc[MB_CUR_MAX];

	wc = (unsigned short)0xe109;
    
    wctomb( mbc, wc );
          
    int i = mbrlen( mbc, 2 ,ps );
	int j = mbrlen( mbc, 0 ,ps );

	INFO_PRINTF3(_L("{Expected: -1, 0} %d %d"), i, j);

	if (i != -2 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testmbrlen8
//API Tested :mbrlen
//TestCase Description:mbrlen returns -> 0 (NULL srting as input)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrlen8L()
    {
    mbstate_t *ps = NULL;
    char  *pmbc = NULL;
                
    int i = mbrlen( pmbc, MB_CUR_MAX ,ps);
	int j = mbrlen( pmbc, 0, ps );
	
	INFO_PRINTF3(_L("{Expected: 0, 0} %d %d"), i, j);

	if (i != 0 || j != 0)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :testmbrlen9
//API Tested :mbrlen
//TestCase Description:mbrlen returns -> -2 (invalid sequence replaced 
//with replacement char case) mbrlen returns -> -1 (truncated MB sequence)
// -----------------------------------------------------------------------------
TInt CTestLibcwchar::testmbrlen9L()
    {
    mbstate_t *ps = NULL;
	char mbc[MB_CUR_MAX];
    
	mbc[0] = (unsigned char)0xcc;
	mbc[1] = (unsigned char)'\0';
  
    int i = mbrlen( mbc, 2,ps );
	int j = mbrlen( mbc, 1,ps );

    INFO_PRINTF3(_L("{Expected: 2, -1} %d %d"), i, j);

	if (i != -1 || j != -2)
		{
		return KErrGeneral;
		}
    
	return KErrNone;
    }


//Test cases for the API mbrlen  :: End


//Test cases for the API wcstof  :: Start

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstof1L
	API Tested				:	Wcstof()
	TestCase Description	:	Test where input wide code character cannot be completed converted to float.
	
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::wcstof1L(  )
	{	 
    INFO_PRINTF1(_L("started Wcstof  1"));
	wchar_t wcs1[21] = L"  1.23abcd";
	wchar_t wcs2[5]=L"abcd";
	wchar_t *eptr;
	float f;

   
    f = wcstof(wcs1, &eptr);
 
    INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a float number: %f and end pointer is set to the Wchar string: %ls"),wcs1,f,eptr);
     
     	 
    if((f== 1.23F) && !(wcscmp (eptr, wcs2)))
		{
   		return KErrNone;
		}
    else
		{
		return KErrGeneral;
		}

 
	}
/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstof2L
	API Tested				:	Wcstof()
	TestCase Description	:	Test where endptr is null
	
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::wcstof2L(  )
	{
    	 
	INFO_PRINTF1(_L("started Wcstof  2"));
	wchar_t wcs1[21] = L"1.23abcd";
	wchar_t **eptr;
	float f;

	eptr = (wchar_t **)NULL;
  
    f = wcstof(wcs1, eptr);
    
    INFO_PRINTF3(_L("Input Wchar string: %ls is converted to float number: %f and end pointer is set to null"),wcs1,f);
   
    if((f== 1.23F) && (eptr==NULL))
   		{
   		return KErrNone;
		}
    else
		{
		return KErrGeneral;
		}
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstof3L
	API Tested				:	Wcstof()
	TestCase Description	:	test where input wide char code can be completely converted to float
	
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::wcstof3L(  )
	{
    	 
	INFO_PRINTF1(_L("started Wcstof  3"));
	wchar_t wcs1[21] = L"123.456";
	wchar_t *eptr;
	float f;
   
    f = wcstof(wcs1, &eptr);

    INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a float number: %f and end pointer is set to the Wchar string: %ls"),wcs1,f,eptr);
     
     	 
    if((f== 123.456F) && (eptr))
   		{
   		return KErrNone;
		}
    else
		{
		return KErrGeneral;
		}
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstof4L
	API Tested				:	Wcstof()
	TestCase Description	:	test where the input wide char code cannot be converted to float
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::wcstof4L(  )
	{
	   	 
	INFO_PRINTF1(_L("started Wcstof  4"));
	wchar_t wcs1[21] = L"abcd";
	wchar_t wcs2[5] = L"abcd";
	wchar_t *eptr;
	float f;
   
    f = wcstof(wcs1, &eptr);

    INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a float number: %f and end pointer is set to the Wchar string: %ls"),wcs1,f,eptr);
     
     	 
    if(!(wcscmp (eptr, wcs2)))
   	 	{
   		return KErrNone;
		}
    else
		{
		return KErrGeneral;
		}
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstof5L
	API Tested				:	Wcstof()
	TestCase Description	:	test where the value is out of range
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::wcstof5L(  )
	{
    	 
	INFO_PRINTF1(_L("started Wcstof 5"));
	wchar_t wcs1[1000] = L"9999999999999999999999999999999999999999999999999999999999.999";
	wchar_t *eptr;
	float f;
	
    f = wcstof(wcs1, &eptr);
 
    INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a float number: %f and end pointer is set to the Wchar string: %ls"),wcs1,f,eptr);
     
    
    if(errno == ERANGE)
   		{
   		return KErrNone;
		}
    else
		{
		return KErrGeneral;
		}
	}

// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime1_aL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %a
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::wcsftime1_aL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,50,L"%a",tm);

	INFO_PRINTF2(_L("wcsftime1_a result is %s"),datestring);
	
	

	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
   
    }




// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime2_AL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %A
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::wcsftime2_AL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%A",tm);
	
	INFO_PRINTF2(_L("wcsftime2_A result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
	    
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime3_bL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %b
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::wcsftime3_bL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	


	retval = wcsftime(datestring,15,L"%b",tm);
	
	INFO_PRINTF2(_L("wcsftime3_b result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime4_BL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %B
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::wcsftime4_BL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%B",tm);

	INFO_PRINTF2(_L("wcsftime4_B result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime5_cL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %c
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::wcsftime5_cL(  )
    {
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
	setlocale(LC_ALL,"C");
#endif
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	
	retval = wcsftime(datestring,30,L"%c",tm);
	
	INFO_PRINTF2(_L("wcsftime5_c result is %s"),datestring);
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}	
    
    }

// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime6_CL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %C
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime6_CL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%C",tm);
	
	INFO_PRINTF2(_L("wcsftime6_C result is %s"),datestring);
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime7_dL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %d
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime7_dL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%d",tm);
	
	INFO_PRINTF2(_L("wcsftime7_d result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    
    }

// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime8_DL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %D
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime8_DL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%D",tm);
	
	INFO_PRINTF2(_L("wcsftime8_D result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime9_eL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %e
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime9_eL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%e",tm);
	

	INFO_PRINTF2(_L("wcsftime9_e result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }

// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime10_FL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %F
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime10_FL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%F",tm);
	
	INFO_PRINTF2(_L("wcsftime10_F result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime11_hL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %h
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime11_hL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%h",tm);
	
	INFO_PRINTF2(_L("wcsftime11_h result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime12_IL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %I
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime12_IL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	
	retval = wcsftime(datestring,15,L"%I",tm);
	
	INFO_PRINTF2(_L("wcsftime12_I result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime13_jL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %j
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime13_jL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%j",tm);
	
	INFO_PRINTF2(_L("wcsftime13_j result is %s"),datestring);
	
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime14_mL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %m
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime14_mL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%m",tm);
	
	INFO_PRINTF2(_L("wcsftime14_m result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime15_ML
// wcsftime test method function 
// <src file name : Mrt_wchartime.c>
// TD ID: 1178
// Test case no: 1_15_2_1178
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %M
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime15_ML(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%M",tm);
	
	INFO_PRINTF2(_L("wcsftime15_M result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }



// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime16_nL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %n
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime16_nL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);


	retval = wcsftime(datestring,15,L"%n",tm);
	
	INFO_PRINTF2(_L("wcsftime16_n result is %x"),datestring[0]);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime17_pL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %p
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime17_pL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%p",tm);
	
	INFO_PRINTF2(_L("wcsftime17_p result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime18_rL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %r
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime18_rL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%r",tm);
	
	INFO_PRINTF2(_L("wcsftime18_r result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime19_RL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %R
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime19_RL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%R",tm);
	
	INFO_PRINTF2(_L("wcsftime19_R result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }



// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime20_SL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %S
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime20_SL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%S",tm);
	
	INFO_PRINTF2(_L("wcsftime20_S result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }



// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime21_tL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %t
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime21_tL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%t",tm);
	
	INFO_PRINTF2(_L("wcsftime21_t result is %x"),datestring[0]);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }



// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime22_TL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %T
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime22_TL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,20,L"%T",tm);
	
	INFO_PRINTF2(_L("wcsftime22_T result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }



// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime23_uL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %u
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime23_uL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%u",tm);
	
	INFO_PRINTF2(_L("wcsftime23_u result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }



// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime25_wL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %w
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime25_wL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%w",tm);
	
	INFO_PRINTF2(_L("wcsftime25_w result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime26_xL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %x
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime26_xL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,20,L"%x",tm);
	
	INFO_PRINTF2(_L("wcsftime26_x result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }



// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime27_XL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %X
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime27_XL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%X",tm);
	
	INFO_PRINTF2(_L("wcsftime27_X result is %s"),datestring);
	
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }



// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime28_yL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %y
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime28_yL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%y",tm);
	
	INFO_PRINTF2(_L("wcsftime28_y result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }



// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime29_YL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %Y
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime29_YL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%Y",tm);
	
	
	INFO_PRINTF2(_L("wcsftime29_Y result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }



// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime30L
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is %%
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime30L(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,15,L"%%",tm);

	INFO_PRINTF2(_L("wcsftime30 result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime_mixL
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is sufficient and format string is a combination of more than one format string
// -----------------------------------------------------------------------------
//

TInt CTestLibcwchar::wcsftime_mixL(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,50,L"%x %a %A %y --- %a",tm);

	INFO_PRINTF2(_L("wcsftime_mix result is %s"),datestring);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }


// -----------------------------------------------------------------------------
// CTestLibcwchar::wcsftime2L
// wcsftime test method function 
// This test function checks if the API returns the proper date/time string when maxsize
// is less than sufficient. The API should return 0.
// -----------------------------------------------------------------------------
//
TInt CTestLibcwchar::wcsftime2L(  )
    {
	
	wchar_t datestring[50];
	int retval;
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);
	
	retval = wcsftime(datestring,25,L"%x %a %A %y --- %a",tm);

	INFO_PRINTF2(_L("wcsftime2 result is %d"),retval);
	
	if(!retval)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;	
		}
    }


/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstod1L
	API Tested				:	Wcstod()
	TestCase Description	:	Test where input wide code character cannot be completed 
								converted to double.
	
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::wcstod1L(  )
	{	 
    INFO_PRINTF1(_L("started Wcstod  1"));
	wchar_t wcs1[21] = L"  1.23abcd";
	wchar_t wcs2[5]=L"abcd";
	wchar_t *eptr;
	double d;

   
    d = wcstod(wcs1, &eptr);
 
    INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a double number: %f and end pointer is set to the Wchar string: %ls"),wcs1,d,eptr);
     
     	 
    if((d == 1.23) && !(wcscmp (eptr, wcs2)))
		{
   	 	return KErrNone;
		}
     else
		{
     	return KErrGeneral;
		}

 
	}
/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstod2L
	API Tested				:	Wcstod()
	TestCase Description	:	Test where endptr is null
	
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::wcstod2L(  )
	{
    	 
	INFO_PRINTF1(_L("started Wcstod  2"));
	wchar_t wcs1[21] = L"1.23abcd";
	wchar_t **eptr;
	double d;

   	eptr = (wchar_t **)NULL;
  
    d = wcstod(wcs1, eptr);
    
    INFO_PRINTF3(_L("Input Wchar string: %ls is converted to double number: %f and end pointer is set to null"),wcs1,d);
   
    if((d == 1.23) && (eptr==NULL))
   	 	{
   	 	return KErrNone;
		}
     else
		{
     	return KErrGeneral;
		}
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstod3L
	API Tested				:	Wcstod()
	TestCase Description	:	test where input wide char code can be completely converted to double
	
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::wcstod3L(  )
	{
    	 
	INFO_PRINTF1(_L("started Wcstod  3"));
	wchar_t wcs1[21] = L"123.456";
	wchar_t *eptr;
	double d;
   
    d = wcstod(wcs1, &eptr);

    INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a double number: %f and end pointer is set to the Wchar string: %ls"),wcs1,d,eptr);
     
     	 
    if((d == 123.456) && (eptr))
   	 	{
   	 	return KErrNone;
		}
     else
		{
     	return KErrGeneral;
		}
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstod4L
	API Tested				:	Wcstod()
	TestCase Description	:	test where the input wide char code cannot be converted to double
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::wcstod4L(  )
	{
    	 
	INFO_PRINTF1(_L("started Wcstod  4"));
	wchar_t wcs1[21] = L"abcd";
	wchar_t wcs2[5] = L"abcd";
	wchar_t *eptr;
	double d;
   
    d = wcstod(wcs1, &eptr);

    INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a double number: %f and end pointer is set to the Wchar string: %ls"),wcs1,d,eptr);
     
     	 
    if((d == 0.0F) && !(wcscmp (eptr, wcs2)))
   	 	{
   	 	return KErrNone;
		}
     else
		{
     	return KErrGeneral;
		}
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstod5L
	API Tested				:	Wcstod()
	TestCase Description	:	test where the value is out of range
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::wcstod5L(  )
	{
    	 
	INFO_PRINTF1(_L("started Wcstod 5"));
	wchar_t wcs1[50] = L"4752364536550478678e-674635jkjk";
	wchar_t *eptr;
	double d;
	d = wcstod(wcs1, &eptr);
 
    INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a double number: %f and end pointer is set to the Wchar string: %ls"),wcs1,d,eptr);
     
    if(errno == ERANGE)
   	 	{
   	 	return KErrNone;
		}
     else
		{
     	return KErrGeneral;
		}
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstold1L
	API Tested				:	Wcstold()
	TestCase Description	:	Test where input wide code character cannot be completed 
								converted to long double.
	
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::wcstold1L(  )
	{	 
    INFO_PRINTF1(_L("started Wcstold  1"));
	wchar_t wcs1[21] = L"  1.23abcd";
	wchar_t wcs2[5]=L"abcd";
	wchar_t *eptr;
	long double d;
   
    d = wcstold(wcs1, &eptr);
 
    INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a double number: %f and end pointer is set to the Wchar string: %ls"),wcs1,d,eptr);
     
     	 
    if((d == 1.23) && !(wcscmp (eptr, wcs2)))
   	 	{
   	 	return KErrNone;
		}
     else
		{
     	return KErrGeneral;
		}
	}
/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstold2L
	API Tested				:	Wcstold()
	TestCase Description	:	Test where endptr is null
	
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::wcstold2L(  )
	{
    	 
	INFO_PRINTF1(_L("started Wcstold  2"));
	wchar_t wcs1[21] = L"1.23abcd";
	wchar_t **eptr;
	long double d;

   	eptr = (wchar_t **)NULL;
  
    d = wcstold(wcs1, eptr);
    
    INFO_PRINTF3(_L("Input Wchar string: %ls is converted to double number: %f and end pointer is set to null"),wcs1,d);
   
    if((d == 1.23) && (eptr==NULL))
   	 	{
   	 	return KErrNone;
		}
     else
		{
     	return KErrGeneral;
		}
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstold3L
	API Tested				:	Wcstold()
	TestCase Description	:	test where input wide char code can be completely converted to double
	
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::wcstold3L(  )
	{
    	 
	INFO_PRINTF1(_L("started Wcstold  3"));
	wchar_t wcs1[21] = L"123.456";
	wchar_t *eptr;
	long double d;
   
    d = wcstold(wcs1, &eptr);

    INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a long double number: %f and end pointer is set to the Wchar string: %ls"),wcs1,d,eptr);
     
    if((d == 123.456) && (eptr))
   		{
   	 	return KErrNone;
		}
     else
		{
     	return KErrGeneral;
		}
	}

/* --------------------------------------------------------------------------------------------
	Function Name			:	Wcstold4L
	API Tested				:	Wcstold()
	TestCase Description	:	test where the input wide char code cannot be converted to double
  --------------------------------------------------------------------------------------------- */
TInt CTestLibcwchar::wcstold4L(  )
	{
    	 
	INFO_PRINTF1(_L("started Wcstold  4"));
	wchar_t wcs1[21] = L"abcd";
	wchar_t wcs2[5] = L"abcd";
	wchar_t *eptr;
	long double d;
   
    d = wcstold(wcs1, &eptr);

    INFO_PRINTF4(_L("Input Wchar string: %ls is converted to a long double number: %f and end pointer is set to the Wchar string: %ls"),wcs1,d,eptr);
     
     	 
    if((d == 0.0F) && !(wcscmp (eptr, wcs2)))
   		{
   	 	return KErrNone;
		}
     else
		{
     	return KErrGeneral;
		}
	}

//Test cases for the API wcstold  :: End
