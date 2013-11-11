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



#ifndef __TESTLIBCWCHAR_H__
#define __TESTLIBCWCHAR_H__
#define TO_BE_TESTED 
#include <test/testexecutestepbase.h>
#include <limits.h>
#include <e32def.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <wctype.h>
#include <inttypes.h>
#include <c32comm.h>

#define BUF_SIZE 100

//fgetwc
_LIT(KTestfgetwcL, "TestfgetwcL");
//fgetws
_LIT(KTestOnefgetwsL, "TestOnefgetwsL");
_LIT(KTestTwofgetwsL, "TestTwofgetwsL");
_LIT(KTestThreefgetwsL, "TestThreefgetwsL");
//getwc
_LIT(KTestgetwcL, "TestgetwcL");
//fwide
_LIT(KTestOnefwideL, "TestOnefwideL");
_LIT(KTestTwofwideL, "TestTwofwideL");
_LIT(KTestThreefwideL, "TestThreefwideL");
_LIT(KTestFourfwideL, "TestFourfwideL");
//fputwc
_LIT(KTestOnefputwcL, "TestOnefputwcL"); 
_LIT(KTestTwofputwcL, "TestTwofputwcL");
_LIT(KTestThreefputwcL, "TestThreefputwcL");
_LIT(KTestFourfputwcL, "TestFourfputwcL"); 
//putwc
_LIT(KTestOneputwcL, "TestOneputwcL"); 
_LIT(KTestTwoputwcL, "TestTwoputwcL");
_LIT(KTestThreeputwcL, "TestThreeputwcL");
_LIT(KTestFourputwcL, "TestFourputwcL");
//fputws
_LIT(KTestOnefputwsL, "TestOnefputwsL");
_LIT(KTestTwofputwsL, "TestTwofputwsL");
_LIT(KTestThreefputwsL, "TestThreefputwsL");
_LIT(KTestFourfputwsL, "TestFourfputwsL");
//ungetwc
_LIT(KTestOneungetwcL, "TestOneungetwcL");
//putwchar
_LIT(KTestOneputwcharL, "TestOneputwcharL");
//getwchar
_LIT(KTestOnegetwcharL, "TestOnegetwcharL");
//test methods for wcslen
_LIT(Kwcslen1L,"wcslen1L");
_LIT(Kwcslen2L,"wcslen2L");
//test methods for wmemcmp
_LIT(Kwmemcmp1L,"wmemcmp1L");
_LIT(Kwmemcmp2L,"wmemcmp2L");
//test methods for wcswidth
_LIT(Kwcswidth1L,"wcswidth1L");
_LIT(Kwcswidth2L,"wcswidth2L");
_LIT(Kwcswidth3L,"wcswidth3L");
_LIT(Kwcswidth4L,"wcswidth4L");
//test methods for wcwidth
_LIT(Kwcwidth1L,"wcwidth1L");
_LIT(Kwcwidth2L,"wcwidth2L");
//test methods for wmemchr
_LIT(Kwmemchr1L,"wmemchr1L");
_LIT(Kwmemchr2L,"wmemchr2L");
//test methods for wmemchr
_LIT(Kwmemcpy1L,"wmemcpy1L");
_LIT(Kwmemcpy2L,"wmemcpy2L");
_LIT(Kwmemcpy3L,"wmemcpy3L");
//test methods for wmemset
_LIT(Kwmemset1L,"wmemset1L");
_LIT(Kwmemset2L,"wmemset2L");
_LIT(Kwmemset3L,"wmemset3L");
//test methods for wmemset
_LIT(Kwmemmove1L,"wmemmove1L");
_LIT(Kwmemmove2L,"wmemmove2L");
_LIT(Kwmemmove3L,"wmemmove3L");
_LIT(Kwmemmove4L,"wmemmove4L");
//wcsncmp Tests
_LIT(KWcsncmp_Test_1L,"Wcsncmp_Test_1L");
_LIT(KWcsncmp_Test_2L,"Wcsncmp_Test_2L");
_LIT(KWcsncmp_Test_3L,"Wcsncmp_Test_3L");
_LIT(KWcsncmp_Test_4L,"Wcsncmp_Test_4L");
_LIT(KWcsncmp_Test_5L,"Wcsncmp_Test_5L");
//wcscmp Tests
_LIT(KWcscmp_Test_1L,"Wcscmp_Test_1L");
_LIT(KWcscmp_Test_2L,"Wcscmp_Test_2L");
_LIT(KWcscmp_Test_3L,"Wcscmp_Test_3L");
_LIT(KWcscmp_Test_4L,"Wcscmp_Test_4L");
//wcscat Tests
_LIT(KWcscat_Test_1L,"Wcscat_Test_1L");
_LIT(KWcscat_Test_2L,"Wcscat_Test_2L");
_LIT(KWcscat_Test_3L,"Wcscat_Test_3L");
_LIT(KWcscat_Test_4L,"Wcscat_Test_4L");
//wcsncat Tests
_LIT(KWcsncat_Test_1L,"Wcsncat_Test_1L");
_LIT(KWcsncat_Test_2L,"Wcsncat_Test_2L");
_LIT(KWcsncat_Test_3L,"Wcsncat_Test_3L");
_LIT(KWcsncat_Test_4L,"Wcsncat_Test_4L");
_LIT(KWcsncat_Test_5L,"Wcsncat_Test_5L");
//wcscpy Tests
_LIT(KWcscpy_Test_1L,"Wcscpy_Test_1L");
_LIT(KWcscpy_Test_2L,"Wcscpy_Test_2L");
_LIT(KWcscpy_Test_3L,"Wcscpy_Test_3L");
_LIT(KWcscpy_Test_4L,"Wcscpy_Test_4L");
//wcschr Tests
_LIT(KWcschr_Test_1L,"Wcschr_Test_1L") ;
_LIT(KWcschr_Test_2L,"Wcschr_Test_2L") ;
_LIT(KWcschr_Test_3L,"Wcschr_Test_3L") ;
_LIT(KWcschr_Test_4L,"Wcschr_Test_4L") ;
_LIT(KWcschr_Test_5L,"Wcschr_Test_5L") ;
//wcsrchr Tests
_LIT(KWcsrchr_Test_1L,"Wcsrchr_Test_1L") ;
_LIT(KWcsrchr_Test_2L,"Wcsrchr_Test_2L") ;
_LIT(KWcsrchr_Test_3L,"Wcsrchr_Test_3L") ;
_LIT(KWcsrchr_Test_4L,"Wcsrchr_Test_4L") ;
_LIT(KWcsrchr_Test_5L,"Wcsrchr_Test_5L") ;
//wcsspn  Tests..
_LIT(KWcsspn_Test_1L,"Wcsspn_Test_1L") ;
_LIT(KWcsspn_Test_2L,"Wcsspn_Test_2L") ;
_LIT(KWcsspn_Test_3L,"Wcsspn_Test_3L");
_LIT(KWcsspn_Test_4L,"Wcsspn_Test_4L");
//wcscspn Tests ..
_LIT(KWcscspn_Test_1L,"Wcscspn_Test_1L") ;
_LIT(KWcscspn_Test_2L,"Wcscspn_Test_2L") ;
_LIT(KWcscspn_Test_3L,"Wcscspn_Test_3L");
_LIT(KWcscspn_Test_4L,"Wcscspn_Test_4L");
//wcspbrk Tests  ..
_LIT(KWcspbrk_Test_1L,"Wcspbrk_Test_1L");
_LIT(KWcspbrk_Test_2L,"Wcspbrk_Test_2L");
_LIT(KWcspbrk_Test_3L,"Wcspbrk_Test_3L");
_LIT(KWcspbrk_Test_4L,"Wcspbrk_Test_4L");
//wcscoll Tests ..
_LIT(KWcscoll_Test_1L,"Wcscoll_Test_1L");
_LIT(KWcscoll_Test_2L,"Wcscoll_Test_2L");
_LIT(KWcscoll_Test_3L,"Wcscoll_Test_3L");
_LIT(KWcscoll_Test_4L,"Wcscoll_Test_4L");
//wcsncpy Tests
_LIT(KWcsncpy_Test_1L,"Wcsncpy_Test_1L");
_LIT(KWcsncpy_Test_2L,"Wcsncpy_Test_2L");
_LIT(KWcsncpy_Test_3L,"Wcsncpy_Test_3L");
_LIT(KWcsncpy_Test_4L,"Wcsncpy_Test_4L");
_LIT(KWcsncpy_Test_5L,"Wcsncpy_Test_5L");
//wcsstr Tests
_LIT(KWcsstr_Test_1L,"Wcsstr_Test_1L");
_LIT(KWcsstr_Test_2L,"Wcsstr_Test_2L");
_LIT(KWcsstr_Test_3L,"Wcsstr_Test_3L");
_LIT(KWcsstr_Test_4L,"Wcsstr_Test_4L");
//swprintf tests
_LIT(Kswprintf_Test0L,"swprintf_Test0L");
_LIT(Kswprintf_Test1L,"swprintf_Test1L");
_LIT(Kswprintf_Test2L,"swprintf_Test2L");
_LIT(Kswprintf_Test3L,"swprintf_Test3L");
_LIT(Kswprintf_Test4L,"swprintf_Test4L");
_LIT(Kswprintf_Test5L,"swprintf_Test5L");
_LIT(Kswprintf_Test6L,"swprintf_Test6L");
_LIT(Kswprintf_Test7L,"swprintf_Test7L");
_LIT(Kswprintf_Test8L,"swprintf_Test8L");
_LIT(Kswprintf_Test9L,"swprintf_Test9L");
_LIT(Kswprintf_Test10L,"swprintf_Test10L");
_LIT(Kswprintf_Test11L,"swprintf_Test11L");
_LIT(Kswprintf_Test12L,"swprintf_Test12L");
_LIT(Kswprintf_Test13L,"swprintf_Test13L");
_LIT(Kswprintf_Test14L,"swprintf_Test14L");
_LIT(Kswprintf_Test15L,"swprintf_Test15L");
_LIT(Kswprintf_Test16L,"swprintf_Test16L");
_LIT(Kswprintf_Test17L,"swprintf_Test17L");
_LIT(Kswprintf_Test18L,"swprintf_Test18L");
_LIT(Kswprintf_Test19L,"swprintf_Test19L");
_LIT(Kswprintf_Test20L,"swprintf_Test20L");
_LIT(Kswprintf_Test21L,"swprintf_Test21L");
_LIT(Kswprintf_Test22L,"swprintf_Test22L");
//swscanf tests
_LIT(Kswscanf_Test1L,"swscanf_Test1L");
_LIT(Kswscanf_Test2L,"swscanf_Test2L");
_LIT(Kswscanf_Test3L,"swscanf_Test3L");
_LIT(Kswscanf_Test4L,"swscanf_Test4L");
_LIT(Kswscanf_Test5L,"swscanf_Test5L");
_LIT(Kswscanf_Test6L,"swscanf_Test6L");
_LIT(Kswscanf_Test7L,"swscanf_Test7L");
_LIT(Kswscanf_Test8L,"swscanf_Test8L");
_LIT(Kswscanf_Test9L,"swscanf_Test9L");
_LIT(Kswscanf_Test10L,"swscanf_Test10L");
_LIT(Kswscanf_Test11L,"swscanf_Test11L");
_LIT(Kswscanf_Test12L,"swscanf_Test12L");
_LIT(Kswscanf_Test13L,"swscanf_Test13L");
_LIT(Kswscanf_Test14L,"swscanf_Test14L");
_LIT(Kswscanf_Test15L,"swscanf_Test15L");
_LIT(Kswscanf_Test16L,"swscanf_Test16L");
_LIT(Kswscanf_Test17L,"swscanf_Test17L");
//mblen
_LIT(Kmblen_Test0L,"mblen_Test0L");
_LIT(Kmblen_Test1L,"mblen_Test1L");
_LIT(Kmblen_Test2L,"mblen_Test2L");
_LIT(Kmblen_Test3L,"mblen_Test3L");
_LIT(Kmblen_Test4L,"mblen_Test4L");
_LIT(Kmblen_Test5L,"mblen_Test5L");
_LIT(Kmblen_Test6L,"mblen_Test6L");
_LIT(Kmblen_Test7L,"mblen_Test7L");
_LIT(Kmblen_Test8L,"mblen_Test8L");
_LIT(Kmblen_Test9L,"mblen_Test9L");
//wctomb
_LIT(Kwctomb_Test0L,"wctomb_Test0L");
_LIT(Kwctomb_Test1L,"wctomb_Test1L");
_LIT(Kwctomb_Test2L,"wctomb_Test2L");
_LIT(Kwctomb_Test3L,"wctomb_Test3L");
_LIT(Kwctomb_Test4L,"wctomb_Test4L");
_LIT(Kwctomb_Test5L,"wctomb_Test5L");
//wcstombs
_LIT(Kwcstombs_Test0L,"wcstombs_Test0L");
_LIT(Kwcstombs_Test1L,"wcstombs_Test1L");
_LIT(Kwcstombs_Test2L,"wcstombs_Test2L");
_LIT(Kwcstombs_Test3L,"wcstombs_Test3L");
_LIT(Kwcstombs_Test4L,"wcstombs_Test4L");
_LIT(Kwcstombs_Test5L,"wcstombs_Test5L");
//mbtowc
_LIT(Kmbtowc_Test0L,"mbtowc_Test0L");
_LIT(Kmbtowc_Test1L,"mbtowc_Test1L");
_LIT(Kmbtowc_Test2L,"mbtowc_Test2L");
_LIT(Kmbtowc_Test3L,"mbtowc_Test3L");
_LIT(Kmbtowc_Test4L,"mbtowc_Test4L");
_LIT(Kmbtowc_Test5L,"mbtowc_Test5L");
//mbstowcs
_LIT(Kmbstowcs_Test0L,"mbstowcs_Test0L");
_LIT(Kmbstowcs_Test1L,"mbstowcs_Test1L");
_LIT(Kmbstowcs_Test2L,"mbstowcs_Test2L");
_LIT(Kmbstowcs_Test3L,"mbstowcs_Test3L");
_LIT(Kmbstowcs_Test4L,"mbstowcs_Test4L");
_LIT(Kmbstowcs_Test5L,"mbstowcs_Test5L");
// Wcstol
_LIT(KWcstol1L,"Wcstol1L");
_LIT(KWcstol2L,"Wcstol2L");
_LIT(KWcstol3L,"Wcstol3L");
_LIT(KWcstol4L,"Wcstol4L");
_LIT(KWcstol5L,"Wcstol5L");
_LIT(KWcstol6L,"Wcstol6L");
_LIT(KWcstol7L,"Wcstol7L");
_LIT(KWcstol8L,"Wcstol8L");
_LIT(KWcstol9L,"Wcstol9L");
// Wcstok
_LIT(KWcstok1L,"Wcstok1L");
_LIT(KWcstok2L,"Wcstok2L");
_LIT(KWcstok3L,"Wcstok3L");
_LIT(KWcstok4L,"Wcstok4L");
//wcrtomb
_LIT(Kwcrtomb2400,"wcrtomb2400");
_LIT(Kwcrtomb2401,"wcrtomb2401");
_LIT(Kwcrtomb2402,"wcrtomb2402");
_LIT(Kwcrtomb2403,"wcrtomb2403");
_LIT(Kwcrtomb2404,"wcrtomb2404");
_LIT(Kwcrtomb2405,"wcrtomb2405");
_LIT(Kwcrtomb2406,"wcrtomb2406");
_LIT(Kwcrtomb2407,"wcrtomb2407");
_LIT(Kwcrtomb2672,"wcrtomb2672");	
//wcsrtombs
_LIT(Kwcsrtombs2409,"wcsrtombs2409");
_LIT(Kwcsrtombs2410,"wcsrtombs2410");        
_LIT(Kwcsrtombs2411,"wcsrtombs2411");
_LIT(Kwcsrtombs2412,"wcsrtombs2412");
_LIT(Kwcsrtombs2413,"wcsrtombs2413");
_LIT(Kwcsrtombs2414,"wcsrtombs2414");
_LIT(Kwcsrtombs2415,"wcsrtombs2415");
_LIT(Kwcsrtombs2423,"wcsrtombs2423");
_LIT(Kwcsrtombs2436,"wcsrtombs2436");
_LIT(Kwcsrtombs2437,"wcsrtombs2437");
_LIT(Kwcsrtombs2438,"wcsrtombs2438");
_LIT(Kwcsrtombs2439,"wcsrtombs2439");
_LIT(Kwcsrtombs2440,"wcsrtombs2440");
_LIT(Kwcsrtombs2442,"wcsrtombs2442");
_LIT(Kwcsrtombs2444,"wcsrtombs2444");
_LIT(Kwcsrtombs2460,"wcsrtombs2460");
_LIT(Kwcsrtombs2673,"wcsrtombs2673");
#ifdef TO_BE_TESTED        
//wprintf  
_LIT(Kwprintf_Test0L,"wprintf_Test0L");	
_LIT(Kwprintf_Test1L,"wprintf_Test1L");
_LIT(Kwprintf_Test2L,"wprintf_Test2L");
_LIT(Kwprintf_Test3L,"wprintf_Test3L");
_LIT(Kwprintf_Test4L,"wprintf_Test4L");
_LIT(Kwprintf_Test5L,"wprintf_Test5L");
_LIT(Kwprintf_Test6L,"wprintf_Test6L");
_LIT(Kwprintf_Test7L,"wprintf_Test7L");
_LIT(Kwprintf_Test8L,"wprintf_Test8L");
_LIT(Kwprintf_Test9L,"wprintf_Test9L");
_LIT(Kwprintf_Test10L,"wprintf_Test10L");
_LIT(Kwprintf_Test11L,"wprintf_Test11L");
_LIT(Kwprintf_Test12L,"wprintf_Test12L");
_LIT(Kwprintf_Test13L,"wprintf_Test13L");
_LIT(Kwprintf_Test14L,"wprintf_Test14L");
_LIT(Kwprintf_Test15L,"wprintf_Test15L");
_LIT(Kwprintf_Test16L,"wprintf_Test16L");
_LIT(Kwprintf_Test17L,"wprintf_Test17L");
_LIT(Kwprintf_Test18L,"wprintf_Test18L");
_LIT(Kwprintf_Test19L,"wprintf_Test19L");
_LIT(Kwprintf_Test20L,"wprintf_Test20L");
_LIT(Kwprintf_Test21L,"wprintf_Test21L");
//vwprintf
_LIT(Kvwprintf_Test0L,"vwprintf_Test0L");
_LIT(Kvwprintf_Test1L,"vwprintf_Test1L");
_LIT(Kvwprintf_Test2L,"vwprintf_Test2L");
_LIT(Kvwprintf_Test3L,"vwprintf_Test3L");
_LIT(Kvwprintf_Test4L,"vwprintf_Test4L");
_LIT(Kvwprintf_Test5L,"vwprintf_Test5L");
_LIT(Kvwprintf_Test6L,"vwprintf_Test6L");
_LIT(Kvwprintf_Test7L,"vwprintf_Test7L");
_LIT(Kvwprintf_Test8L,"vwprintf_Test8L");
_LIT(Kvwprintf_Test9L,"vwprintf_Test9L");
_LIT(Kvwprintf_Test10L,"vwprintf_Test10L");
_LIT(Kvwprintf_Test11L,"vwprintf_Test11L");
_LIT(Kvwprintf_Test12L,"vwprintf_Test12L");
_LIT(Kvwprintf_Test13L,"vwprintf_Test13L");
_LIT(Kvwprintf_Test14L,"vwprintf_Test14L");
_LIT(Kvwprintf_Test15L,"vwprintf_Test15L");
_LIT(Kvwprintf_Test16L,"vwprintf_Test16L");
_LIT(Kvwprintf_Test17L,"vwprintf_Test17L");
_LIT(Kvwprintf_Test18L,"vwprintf_Test18L");
_LIT(Kvwprintf_Test19L,"vwprintf_Test19L");
_LIT(Kvwprintf_Test20L,"vwprintf_Test20L");
_LIT(Kvwprintf_Test21L,"vwprintf_Test21L");		
#endif	//TO_BE_TESTED

//fwprintf 
_LIT(Kfwprintf_Test1L,"fwprintf_Test1L");
_LIT(Kfwprintf_Test2L,"fwprintf_Test2L");
_LIT(Kfwprintf_Test3L,"fwprintf_Test3L");
_LIT(Kfwprintf_Test4L,"fwprintf_Test4L");
_LIT(Kfwprintf_Test5L,"fwprintf_Test5L");
_LIT(Kfwprintf_Test6L,"fwprintf_Test6L");
_LIT(Kfwprintf_Test7L,"fwprintf_Test7L");
_LIT(Kfwprintf_Test8L,"fwprintf_Test8L");
_LIT(Kfwprintf_Test9L,"fwprintf_Test9L");
_LIT(Kfwprintf_Test10L,"fwprintf_Test10L");
_LIT(Kfwprintf_Test11L,"fwprintf_Test11L");
_LIT(Kfwprintf_Test12L,"fwprintf_Test12L");
_LIT(Kfwprintf_Test13L,"fwprintf_Test13L");
_LIT(Kfwprintf_Test14L,"fwprintf_Test14L");
_LIT(Kfwprintf_Test15L,"fwprintf_Test15L");
_LIT(Kfwprintf_Test16L,"fwprintf_Test16L");
_LIT(Kfwprintf_Test17L,"fwprintf_Test17L");
_LIT(Kfwprintf_Test18L,"fwprintf_Test18L");
_LIT(Kfwprintf_Test19L,"fwprintf_Test19L");
_LIT(Kfwprintf_Test20L,"fwprintf_Test20L");
_LIT(Kfwprintf_Test21L,"fwprintf_Test21L");		
// vfwprintf functions
_LIT(Kvfwprintf_Test0L,"vfwprintf_Test0L");
_LIT(Kvfwprintf_Test1L,"vfwprintf_Test1L");
_LIT(Kvfwprintf_Test2L,"vfwprintf_Test2L");
_LIT(Kvfwprintf_Test3L,"vfwprintf_Test3L");
_LIT(Kvfwprintf_Test4L,"vfwprintf_Test4L");
_LIT(Kvfwprintf_Test5L,"vfwprintf_Test5L");
_LIT(Kvfwprintf_Test6L,"vfwprintf_Test6L");
_LIT(Kvfwprintf_Test7L,"vfwprintf_Test7L");
_LIT(Kvfwprintf_Test8L,"vfwprintf_Test8L");
_LIT(Kvfwprintf_Test9L,"vfwprintf_Test9L");
_LIT(Kvfwprintf_Test10L,"vfwprintf_Test10L");
_LIT(Kvfwprintf_Test11L,"vfwprintf_Test11L");
_LIT(Kvfwprintf_Test12L,"vfwprintf_Test12L");
_LIT(Kvfwprintf_Test13L,"vfwprintf_Test13L");
_LIT(Kvfwprintf_Test14L,"vfwprintf_Test14L");
_LIT(Kvfwprintf_Test15L,"vfwprintf_Test15L");
_LIT(Kvfwprintf_Test16L,"vfwprintf_Test16L");
_LIT(Kvfwprintf_Test17L,"vfwprintf_Test17L");
_LIT(Kvfwprintf_Test18L,"vfwprintf_Test18L");
_LIT(Kvfwprintf_Test19L,"vfwprintf_Test19L");
_LIT(Kvfwprintf_Test20L,"vfwprintf_Test20L");
_LIT(Kvfwprintf_Test21L,"vfwprintf_Test21L");
//vswprintf
_LIT(Kvswprintf_Test0L,"vswprintf_Test0L");
_LIT(Kvswprintf_Test1L,"vswprintf_Test1L");
_LIT(Kvswprintf_Test2L,"vswprintf_Test2L");
_LIT(Kvswprintf_Test3L,"vswprintf_Test3L");
_LIT(Kvswprintf_Test4L,"vswprintf_Test4L");
_LIT(Kvswprintf_Test5L,"vswprintf_Test5L");
_LIT(Kvswprintf_Test6L,"vswprintf_Test6L");
_LIT(Kvswprintf_Test7L,"vswprintf_Test7L");
_LIT(Kvswprintf_Test8L,"vswprintf_Test8L");
_LIT(Kvswprintf_Test9L,"vswprintf_Test9L");
_LIT(Kvswprintf_Test10L,"vswprintf_Test10L");
_LIT(Kvswprintf_Test11L,"vswprintf_Test11L");
_LIT(Kvswprintf_Test12L,"vswprintf_Test12L");
_LIT(Kvswprintf_Test13L,"vswprintf_Test13L");
_LIT(Kvswprintf_Test14L,"vswprintf_Test14L");
_LIT(Kvswprintf_Test15L,"vswprintf_Test15L");
_LIT(Kvswprintf_Test16L,"vswprintf_Test16L");
_LIT(Kvswprintf_Test17L,"vswprintf_Test17L");
_LIT(Kvswprintf_Test18L,"vswprintf_Test18L");
_LIT(Kvswprintf_Test19L,"vswprintf_Test19L");
_LIT(Kvswprintf_Test20L,"vswprintf_Test20L");
_LIT(Kvswprintf_Test21L,"vswprintf_Test21L");	
//wscanf
_LIT(Kwscanf_Test1L,"wscanf_Test1L");
_LIT(Kwscanf_Test2L,"wscanf_Test2L");
_LIT(Kwscanf_Test3L,"wscanf_Test3L");
_LIT(Kwscanf_Test4L,"wscanf_Test4L");
_LIT(Kwscanf_Test5L,"wscanf_Test5L");
_LIT(Kwscanf_Test6L,"wscanf_Test6L");
_LIT(Kwscanf_Test7L,"wscanf_Test7L");
_LIT(Kwscanf_Test8L,"wscanf_Test8L");
_LIT(Kwscanf_Test9L,"wscanf_Test9L");
_LIT(Kwscanf_Test10L,"wscanf_Test10L");
_LIT(Kwscanf_Test11L,"wscanf_Test11L");
_LIT(Kwscanf_Test12L,"wscanf_Test12L");
_LIT(Kwscanf_Test13L,"wscanf_Test13L");
_LIT(Kwscanf_Test14L,"wscanf_Test14L");
_LIT(Kwscanf_Test15L,"wscanf_Test15L");
_LIT(Kwscanf_Test16L,"wscanf_Test16L");
_LIT(Kwscanf_Test17L,"wscanf_Test17L");
#ifdef TO_BE_TESTED        		
//vwscanf
_LIT(Kvwscanf_Test1L,"vwscanf_Test1L");
_LIT(Kvwscanf_Test2L,"vwscanf_Test2L");
_LIT(Kvwscanf_Test3L,"vwscanf_Test3L");
_LIT(Kvwscanf_Test4L,"vwscanf_Test4L");
_LIT(Kvwscanf_Test5L,"vwscanf_Test5L");
_LIT(Kvwscanf_Test6L,"vwscanf_Test6L");
_LIT(Kvwscanf_Test7L,"vwscanf_Test7L");
_LIT(Kvwscanf_Test8L,"vwscanf_Test8L");
_LIT(Kvwscanf_Test9L,"vwscanf_Test9L");
_LIT(Kvwscanf_Test10L,"vwscanf_Test10L");
_LIT(Kvwscanf_Test11L,"vwscanf_Test11L");
_LIT(Kvwscanf_Test12L,"vwscanf_Test12L");
_LIT(Kvwscanf_Test13L,"vwscanf_Test13L");
_LIT(Kvwscanf_Test14L,"vwscanf_Test14L");
_LIT(Kvwscanf_Test15L,"vwscanf_Test15L");
_LIT(Kvwscanf_Test16L,"vwscanf_Test16L");
_LIT(Kvwscanf_Test17L,"vwscanf_Test17L");
#endif //TO_BE_TESTED		
//fwscanf
_LIT(Kfwscanf_Test1L,"fwscanf_Test1L");
_LIT(Kfwscanf_Test2L,"fwscanf_Test2L");
_LIT(Kfwscanf_Test3L,"fwscanf_Test3L");
_LIT(Kfwscanf_Test4L,"fwscanf_Test4L");
_LIT(Kfwscanf_Test5L,"fwscanf_Test5L");
_LIT(Kfwscanf_Test6L,"fwscanf_Test6L");
_LIT(Kfwscanf_Test7L,"fwscanf_Test7L");
_LIT(Kfwscanf_Test8L,"fwscanf_Test8L");
_LIT(Kfwscanf_Test9L,"fwscanf_Test9L");
_LIT(Kfwscanf_Test10L,"fwscanf_Test10L");
_LIT(Kfwscanf_Test11L,"fwscanf_Test11L");
_LIT(Kfwscanf_Test12L,"fwscanf_Test12L");
_LIT(Kfwscanf_Test13L,"fwscanf_Test13L");
_LIT(Kfwscanf_Test14L,"fwscanf_Test14L");
_LIT(Kfwscanf_Test15L,"fwscanf_Test15L");
_LIT(Kfwscanf_Test16L,"fwscanf_Test16L");
//vfwscanf
_LIT(Kvfwscanf_Test1L,"vfwscanf_Test1L");
_LIT(Kvfwscanf_Test2L,"vfwscanf_Test2L");
_LIT(Kvfwscanf_Test3L,"vfwscanf_Test3L");
_LIT(Kvfwscanf_Test4L,"vfwscanf_Test4L");
_LIT(Kvfwscanf_Test5L,"vfwscanf_Test5L");
_LIT(Kvfwscanf_Test6L,"vfwscanf_Test6L");
_LIT(Kvfwscanf_Test7L,"vfwscanf_Test7L");
_LIT(Kvfwscanf_Test8L,"vfwscanf_Test8L");
_LIT(Kvfwscanf_Test9L,"vfwscanf_Test9L");
_LIT(Kvfwscanf_Test10L,"vfwscanf_Test10L");
_LIT(Kvfwscanf_Test11L,"vfwscanf_Test11L");
_LIT(Kvfwscanf_Test12L,"vfwscanf_Test12L");
_LIT(Kvfwscanf_Test13L,"vfwscanf_Test13L");
_LIT(Kvfwscanf_Test14L,"vfwscanf_Test14L");
_LIT(Kvfwscanf_Test15L,"vfwscanf_Test15L");
_LIT(Kvfwscanf_Test16L,"vfwscanf_Test16L");
// vswscanf
_LIT(Kvswscanf_Test1L,"vswscanf_Test1L");
_LIT(Kvswscanf_Test2L,"vswscanf_Test2L");
_LIT(Kvswscanf_Test3L,"vswscanf_Test3L");
_LIT(Kvswscanf_Test4L,"vswscanf_Test4L");
_LIT(Kvswscanf_Test5L,"vswscanf_Test5L");
_LIT(Kvswscanf_Test6L,"vswscanf_Test6L");
_LIT(Kvswscanf_Test7L,"vswscanf_Test7L");
_LIT(Kvswscanf_Test8L,"vswscanf_Test8L");
_LIT(Kvswscanf_Test9L,"vswscanf_Test9L");
_LIT(Kvswscanf_Test10L,"vswscanf_Test10L");
_LIT(Kvswscanf_Test11L,"vswscanf_Test11L");
_LIT(Kvswscanf_Test12L,"vswscanf_Test12L");
_LIT(Kvswscanf_Test13L,"vswscanf_Test13L");
_LIT(Kvswscanf_Test14L,"vswscanf_Test14L");
_LIT(Kvswscanf_Test15L,"vswscanf_Test15L");
_LIT(Kvswscanf_Test16L,"vswscanf_Test16L");
_LIT(Kvswscanf_Test17L,"vswscanf_Test17L");

//btowc
_LIT(Ktestbtowc1L,"testbtowc1L");
_LIT(Ktestbtowc2L,"testbtowc2L");
_LIT(Ktestbtowc3L,"testbtowc3L");

//btowc
_LIT(Ktestwctob1L,"testwctob1L");
_LIT(Ktestwctob2L,"testwctob2L");
_LIT(Ktestwctob3L,"testwctob3L");

//towlower
_LIT(Ktesttowlower1L,"testtowlower1L");
_LIT(Ktesttowlower2L,"testtowlower2L");
_LIT(Ktesttowlower3L,"testtowlower3L");

//towupper
_LIT(Ktesttowupper1L,"testtowupper1L");
_LIT(Ktesttowupper2L,"testtowupper2L");
_LIT(Ktesttowupper3L,"testtowupper3L");

//mbsrtowcs
_LIT(Ktestmbsrtowcs1L,"testmbsrtowcs1L");
_LIT(Ktestmbsrtowcs2L,"testmbsrtowcs2L");
_LIT(Ktestmbsrtowcs3L,"testmbsrtowcs3L");
_LIT(Ktestmbsrtowcs4L,"testmbsrtowcs4L");
_LIT(Ktestmbsrtowcs5L,"testmbsrtowcs5L");

//mbrtowc
_LIT(Ktestmbrtowc1L,"testmbrtowc1L");
_LIT(Ktestmbrtowc2L,"testmbrtowc2L");
_LIT(Ktestmbrtowc3L,"testmbrtowc3L");
_LIT(Ktestmbrtowc4L,"testmbrtowc4L");
_LIT(Ktestmbrtowc5L,"testmbrtowc5L");
_LIT(Ktestmbrtowc6L,"testmbrtowc6L");

//mbrlen
_LIT(Ktestmbrlen0L,"testmbrlen0L");
_LIT(Ktestmbrlen1L,"testmbrlen1L");
_LIT(Ktestmbrlen2L,"testmbrlen2L");
_LIT(Ktestmbrlen3L,"testmbrlen3L");
_LIT(Ktestmbrlen4L,"testmbrlen4L");
_LIT(Ktestmbrlen5L,"testmbrlen5L");
_LIT(Ktestmbrlen6L,"testmbrlen6L");
_LIT(Ktestmbrlen7L,"testmbrlen7L");
_LIT(Ktestmbrlen8L,"testmbrlen8L");
_LIT(Ktestmbrlen9L,"testmbrlen9L");

//mbsinit
_LIT(Ktestmbsinit1L,"testmbsinit1L");
_LIT(Ktestmbsinit2L,"testmbsinit2L");
_LIT(Ktestmbsinit3L,"testmbsinit3L");


//wctype
_LIT(Ktestwctype1L,"testwctype1L");
_LIT(Ktestwctype2L,"testwctype2L");
_LIT(Ktestwctype3L,"testwctype3L");
_LIT(Ktestwctype4L,"testwctype4L");
_LIT(Ktestwctype5L,"testwctype5L");
_LIT(Ktestwctype6L,"testwctype6L");
_LIT(Ktestwctype7L,"testwctype7L");
_LIT(Ktestwctype8L,"testwctype8L");
_LIT(Ktestwctype9L,"testwctype9L");
_LIT(Ktestwctype10L,"testwctype10L");
_LIT(Ktestwctype11L,"testwctype11L");
_LIT(Ktestwctype12L,"testwctype12L");
_LIT(Ktestwctype13L,"testwctype13L");
_LIT(Ktestwctype14L,"testwctype14L");
_LIT(Ktestwctype15L,"testwctype15L");
_LIT(Ktestwctype16L,"testwctype16L");
_LIT(Ktestwctype17L,"testwctype17L");

//wctrans and towctrans
_LIT(Ktestwctrans1L,"testwctrans1L");
_LIT(Ktestwctrans2L,"testwctrans2L");
_LIT(Ktestwctrans3L,"testwctrans3L");
_LIT(Ktestwctrans4L,"testwctrans4L");
_LIT(Ktestwctrans5L,"testwctrans5L");


//1 wcsnlen API test cases
_LIT(Ktestwcsnlen1L,"testwcsnlen1L");
_LIT(Ktestwcsnlen2L,"testwcsnlen2L");
_LIT(Ktestwcsnlen3L,"testwcsnlen3L");
_LIT(Ktestwcsnlen4L,"testwcsnlen4L");
_LIT(Ktestwcsnlen5L,"testwcsnlen5L");

//2 wcsdup API test cases
_LIT(Ktestwcsdup1L,"testwcsdup1L");
_LIT(Ktestwcsdup2L,"testwcsdup2L");

//3  wcscasecmp API test cases
_LIT(Ktestwcscasecmp1L,"testwcscasecmp1L");
_LIT(Ktestwcscasecmp2L,"testwcscasecmp2L");
_LIT(Ktestwcscasecmp3L,"testwcscasecmp3L");
_LIT(Ktestwcscasecmp4L,"testwcscasecmp4L");
_LIT(Ktestwcscasecmp5L,"testwcscasecmp5L");
_LIT(Ktestwcscasecmp6L,"testwcscasecmp6L");

//4  wcsncasecmp API test cases
_LIT(Ktestwcsncasecmp1L,"testwcsncasecmp1L");
_LIT(Ktestwcsncasecmp2L,"testwcsncasecmp2L");
_LIT(Ktestwcsncasecmp3L,"testwcsncasecmp3L");
_LIT(Ktestwcsncasecmp4L,"testwcsncasecmp4L");
_LIT(Ktestwcsncasecmp5L,"testwcsncasecmp5L");
_LIT(Ktestwcsncasecmp6L,"testwcsncasecmp6L");
_LIT(Ktestwcsncasecmp7L,"testwcsncasecmp7L");

//5  wcpcpy API test cases
_LIT(Ktestwcpcpy1L,"testwcpcpy1L");
_LIT(Ktestwcpcpy2L,"testwcpcpy2L");

//6  wcpncpy API test cases
_LIT(Ktestwcpncpy1L,"testwcpncpy1L");
_LIT(Ktestwcpncpy2L,"testwcpncpy2L");
_LIT(Ktestwcpncpy3L,"testwcpncpy3L");
_LIT(Ktestwcpncpy4L,"testwcpncpy4L");

//7  wcsxfrm API test cases
_LIT(Ktestwcsxfrm1L,"testwcsxfrm1L");
_LIT(Ktestwcsxfrm2L,"testwcsxfrm2L");

//8  wcstoul API test cases
_LIT(Ktestwcstoul1L,"testwcstoul1L");
_LIT(Ktestwcstoul2L,"testwcstoul2L");
_LIT(Ktestwcstoul3L,"testwcstoul3L");
_LIT(Ktestwcstoul4L,"testwcstoul4L");
_LIT(Ktestwcstoul5L,"testwcstoul5L");
_LIT(Ktestwcstoul6L,"testwcstoul6L");
_LIT(Ktestwcstoul7L,"testwcstoul7L");
_LIT(Ktestwcstoul8L,"testwcstoul8L");
_LIT(Ktestwcstoul9L,"testwcstoul9L");
_LIT(Ktestwcstoul10L,"testwcstoul10L");
_LIT(Ktestwcstoul11L,"testwcstoul11L");
_LIT(Ktestwcstoul12L,"testwcstoul12L");
_LIT(Ktestwcstoul13L,"testwcstoul13L");
_LIT(Ktestwcstoul14L,"testwcstoul14L");
_LIT(Ktestwcstoul15L,"testwcstoul15L");
_LIT(Ktestwcstoul16L,"testwcstoul16L");
_LIT(Ktestwcstoul17L,"testwcstoul17L");

//9  wcstoll API test cases
_LIT(Ktestwcstoll1L,"testwcstoll1L");
_LIT(Ktestwcstoll2L,"testwcstoll2L");
_LIT(Ktestwcstoll3L,"testwcstoll3L");
_LIT(Ktestwcstoll4L,"testwcstoll4L");
_LIT(Ktestwcstoll5L,"testwcstoll5L");
_LIT(Ktestwcstoll6L,"testwcstoll6L");
_LIT(Ktestwcstoll7L,"testwcstoll7L");
_LIT(Ktestwcstoll8L,"testwcstoll8L");
_LIT(Ktestwcstoll9L,"testwcstoll9L");
_LIT(Ktestwcstoll10L,"testwcstoll10L");
_LIT(Ktestwcstoll11L,"testwcstoll11L");
_LIT(Ktestwcstoll12L,"testwcstoll12L");
_LIT(Ktestwcstoll13L,"testwcstoll13L");
_LIT(Ktestwcstoll14L,"testwcstoll14L");
_LIT(Ktestwcstoll15L,"testwcstoll15L");
_LIT(Ktestwcstoll16L,"testwcstoll16L");
_LIT(Ktestwcstoll17L,"testwcstoll17L");

//10  wcstoull API test cases
_LIT(Ktestwcstoull1L,"testwcstoull1L");
_LIT(Ktestwcstoull2L,"testwcstoull2L");
_LIT(Ktestwcstoull3L,"testwcstoull3L");
_LIT(Ktestwcstoull4L,"testwcstoull4L");
_LIT(Ktestwcstoull5L,"testwcstoull5L");
_LIT(Ktestwcstoull6L,"testwcstoull6L");
_LIT(Ktestwcstoull7L,"testwcstoull7L");
_LIT(Ktestwcstoull8L,"testwcstoull8L");
_LIT(Ktestwcstoull9L,"testwcstoull9L");
_LIT(Ktestwcstoull10L,"testwcstoull10L");
_LIT(Ktestwcstoull11L,"testwcstoull11L");
_LIT(Ktestwcstoull12L,"testwcstoull12L");
_LIT(Ktestwcstoull13L,"testwcstoull13L");
_LIT(Ktestwcstoull14L,"testwcstoull14L");
_LIT(Ktestwcstoull15L,"testwcstoull15L");
_LIT(Ktestwcstoull16L,"testwcstoull16L");
_LIT(Ktestwcstoull17L,"testwcstoull17L");

//11  wcstoq API test cases
_LIT(Ktestwcstoq1L,"testwcstoq1L");
_LIT(Ktestwcstoq2L,"testwcstoq2L");
_LIT(Ktestwcstoq3L,"testwcstoq3L");
_LIT(Ktestwcstoq4L,"testwcstoq4L");
_LIT(Ktestwcstoq5L,"testwcstoq5L");
_LIT(Ktestwcstoq6L,"testwcstoq6L");
_LIT(Ktestwcstoq7L,"testwcstoq7L");
_LIT(Ktestwcstoq8L,"testwcstoq8L");
_LIT(Ktestwcstoq9L,"testwcstoq9L");
_LIT(Ktestwcstoq10L,"testwcstoq10L");
_LIT(Ktestwcstoq11L,"testwcstoq11L");
_LIT(Ktestwcstoq12L,"testwcstoq12L");
_LIT(Ktestwcstoq13L,"testwcstoq13L");
_LIT(Ktestwcstoq14L,"testwcstoq14L");
_LIT(Ktestwcstoq15L,"testwcstoq15L");
_LIT(Ktestwcstoq16L,"testwcstoq16L");
_LIT(Ktestwcstoq17L,"testwcstoq17L");

//12  wcstouq API test cases
_LIT(Ktestwcstouq1L,"testwcstouq1L");
_LIT(Ktestwcstouq2L,"testwcstouq2L");
_LIT(Ktestwcstouq3L,"testwcstouq3L");
_LIT(Ktestwcstouq4L,"testwcstouq4L");
_LIT(Ktestwcstouq5L,"testwcstouq5L");
_LIT(Ktestwcstouq6L,"testwcstouq6L");
_LIT(Ktestwcstouq7L,"testwcstouq7L");
_LIT(Ktestwcstouq8L,"testwcstouq8L");
_LIT(Ktestwcstouq9L,"testwcstouq9L");
_LIT(Ktestwcstouq10L,"testwcstouq10L");
_LIT(Ktestwcstouq11L,"testwcstouq11L");
_LIT(Ktestwcstouq12L,"testwcstouq12L");
_LIT(Ktestwcstouq13L,"testwcstouq13L");
_LIT(Ktestwcstouq14L,"testwcstouq14L");
_LIT(Ktestwcstouq15L,"testwcstouq15L");
_LIT(Ktestwcstouq16L,"testwcstouq16L");
_LIT(Ktestwcstouq17L,"testwcstouq17L");

//13  wcstoumax API test cases
_LIT(Ktestwcstoumax1L,"testwcstoumax1L");
_LIT(Ktestwcstoumax2L,"testwcstoumax2L");
_LIT(Ktestwcstoumax3L,"testwcstoumax3L");
_LIT(Ktestwcstoumax4L,"testwcstoumax4L");
_LIT(Ktestwcstoumax5L,"testwcstoumax5L");
_LIT(Ktestwcstoumax6L,"testwcstoumax6L");
_LIT(Ktestwcstoumax7L,"testwcstoumax7L");
_LIT(Ktestwcstoumax8L,"testwcstoumax8L");
_LIT(Ktestwcstoumax9L,"testwcstoumax9L");
_LIT(Ktestwcstoumax10L,"testwcstoumax10L");
_LIT(Ktestwcstoumax11L,"testwcstoumax11L");
_LIT(Ktestwcstoumax12L,"testwcstoumax12L");
_LIT(Ktestwcstoumax13L,"testwcstoumax13L");
_LIT(Ktestwcstoumax14L,"testwcstoumax14L");
_LIT(Ktestwcstoumax15L,"testwcstoumax15L");
_LIT(Ktestwcstoumax16L,"testwcstoumax16L");
_LIT(Ktestwcstoumax17L,"testwcstoumax17L");

//14  wcstoimax API test cases
_LIT(Ktestwcstoimax1L,"testwcstoimax1L");
_LIT(Ktestwcstoimax2L,"testwcstoimax2L");
_LIT(Ktestwcstoimax3L,"testwcstoimax3L");
_LIT(Ktestwcstoimax4L,"testwcstoimax4L");
_LIT(Ktestwcstoimax5L,"testwcstoimax5L");
_LIT(Ktestwcstoimax6L,"testwcstoimax6L");
_LIT(Ktestwcstoimax7L,"testwcstoimax7L");
_LIT(Ktestwcstoimax8L,"testwcstoimax8L");
_LIT(Ktestwcstoimax9L,"testwcstoimax9L");
_LIT(Ktestwcstoimax10L,"testwcstoimax10L");
_LIT(Ktestwcstoimax11L,"testwcstoimax11L");
_LIT(Ktestwcstoimax12L,"testwcstoimax12L");
_LIT(Ktestwcstoimax13L,"testwcstoimax13L");
_LIT(Ktestwcstoimax14L,"testwcstoimax14L");
_LIT(Ktestwcstoimax15L,"testwcstoimax15L");
_LIT(Ktestwcstoimax16L,"testwcstoimax16L");
_LIT(Ktestwcstoimax17L,"testwcstoimax17L");

//15  mbsnrtowcs API test cases
_LIT(Ktestmbsnrtowcs1L,"testmbsnrtowcs1L");
_LIT(Ktestmbsnrtowcs2L,"testmbsnrtowcs2L");
_LIT(Ktestmbsnrtowcs3L,"testmbsnrtowcs3L");
_LIT(Ktestmbsnrtowcs4L,"testmbsnrtowcs4L");
_LIT(Ktestmbsnrtowcs5L,"testmbsnrtowcs5L");

//16  mbsnrtowcs API test cases
_LIT(Ktestwcsnrtombs1L,"testwcsnrtombs1L");
_LIT(Ktestwcsnrtombs2L,"testwcsnrtombs2L");
_LIT(Ktestwcsnrtombs3L,"testwcsnrtombs3L");
_LIT(Ktestwcsnrtombs4L,"testwcsnrtombs4L");
_LIT(Ktestwcsnrtombs5L,"testwcsnrtombs5L");

//wcstof API test cases
_LIT(Kwcstof1L,"wcstof1L");
_LIT(Kwcstof2L,"wcstof2L");
_LIT(Kwcstof3L,"wcstof3L");
_LIT(Kwcstof4L,"wcstof4L");
_LIT(Kwcstof5L,"wcstof5L");

//wcsftime.
_LIT(Kwcsftime1_aL,"wcsftime1_aL");
_LIT(Kwcsftime2_AL,"wcsftime2_AL");
_LIT(Kwcsftime3_bL,"wcsftime3_bL");
_LIT(Kwcsftime4_BL,"wcsftime4_BL");
_LIT(Kwcsftime5_cL,"wcsftime5_cL");
_LIT(Kwcsftime6_CL,"wcsftime6_CL");
_LIT(Kwcsftime7_dL,"wcsftime7_dL");
_LIT(Kwcsftime8_DL,"wcsftime8_DL");
_LIT(Kwcsftime9_eL,"wcsftime9_eL");
_LIT(Kwcsftime10_FL,"wcsftime10_FL");
_LIT(Kwcsftime11_hL,"wcsftime11_hL");
_LIT(Kwcsftime12_IL,"wcsftime12_IL");
_LIT(Kwcsftime13_jL,"wcsftime13_jL");
_LIT(Kwcsftime14_mL,"wcsftime14_mL");
_LIT(Kwcsftime15_ML,"wcsftime15_ML");
_LIT(Kwcsftime16_nL,"wcsftime16_nL");
_LIT(Kwcsftime17_pL,"wcsftime17_pL");
_LIT(Kwcsftime18_rL,"wcsftime18_rL");
_LIT(Kwcsftime19_RL,"wcsftime19_RL");
_LIT(Kwcsftime20_SL,"wcsftime20_SL");
_LIT(Kwcsftime21_tL,"wcsftime21_tL");
_LIT(Kwcsftime22_TL,"wcsftime22_TL");
_LIT(Kwcsftime23_uL,"wcsftime23_uL");
_LIT(Kwcsftime25_wL,"wcsftime25_wL");
_LIT(Kwcsftime26_xL,"wcsftime26_xL");
_LIT(Kwcsftime27_XL,"wcsftime27_XL");
_LIT(Kwcsftime28_yL,"wcsftime28_yL");
_LIT(Kwcsftime29_YL,"wcsftime29_YL");
_LIT(Kwcsftime30L,"wcsftime30L");
_LIT(Kwcsftime_mixL,"wcsftime_mixL");
_LIT(Kwcsftime2L,"wcsftime2L");

//wcstod API test cases
_LIT(Kwcstod1L,"wcstod1L");
_LIT(Kwcstod2L,"wcstod2L");
_LIT(Kwcstod3L,"wcstod3L");
_LIT(Kwcstod4L,"wcstod4L");
_LIT(Kwcstod5L,"wcstod5L");

//wcstold API test cases
_LIT(Kwcstold1L,"wcstold1L");
_LIT(Kwcstold2L,"wcstold2L");
_LIT(Kwcstold3L,"wcstold3L");
_LIT(Kwcstold4L,"wcstold4L");
//wcswcs Tests
_LIT(KWcswcs_Test_1L,"Wcswcs_Test_1L");
_LIT(KWcswcs_Test_2L,"Wcswcs_Test_2L");
_LIT(KWcswcs_Test_3L,"Wcswcs_Test_3L");
_LIT(KWcswcs_Test_4L,"Wcswcs_Test_4L");


class CTestLibcwchar : public CTestStep
{
public:
~CTestLibcwchar(); 
CTestLibcwchar(const TDesC& aStepName);
TVerdict doTestStepL();
TVerdict doTestStepPreambleL();
TVerdict doTestStepPostambleL();
private:

TInt CreateTestDataFile(char *filename,unsigned int *arr, int count);
#ifdef TO_BE_TESTED  	  			
TInt vwprintfTests(const wchar_t *fmt,...);		
#endif //TO_BE_TESTED    	
TInt vfwprintfTests( char *filename,const wchar_t *fmt,...);
TInt vswprintfTests( const wchar_t *fmt,...);

#ifdef TO_BE_TESTED  	  	
TInt vwscanfTests (const wchar_t *fmt, ...);

#endif //TO_BE_TESTED  	  	
TInt vfwscanfTests (char *filename,const wchar_t *fmt, ...);
TInt vswscanfTests (const wchar_t *str,const wchar_t *fmt, ...); 

//fgetwc
TInt TestfgetwcL( );
//fgetws
TInt TestOnefgetwsL( );
TInt TestTwofgetwsL( );
TInt TestThreefgetwsL( );
//getwc
TInt TestgetwcL( );
//fwide
TInt TestOnefwideL ( );
TInt TestTwofwideL ( );
TInt TestThreefwideL( );
TInt TestFourfwideL( );
//fputwc
TInt TestOnefputwcL( );
TInt TestTwofputwcL( );
TInt TestThreefputwcL( );
TInt TestFourfputwcL( );
//putwc
TInt TestOneputwcL( );
TInt TestTwoputwcL( );
TInt TestThreeputwcL( );
TInt TestFourputwcL( );
//fputws
TInt TestOnefputwsL( );
TInt TestTwofputwsL( );
TInt TestThreefputwsL( );
TInt TestFourfputwsL( );
//putwchar
TInt TestOneputwcharL( );
//getwchar
TInt TestOnegetwcharL( );
//ungetwc
TInt TestOneungetwcL( );

//test methods for wcslen
TInt wcslen1L(  );
TInt wcslen2L(  );
//test methods for wmemcmp
TInt wmemcmp1L(  );
TInt wmemcmp2L(  );
//test methods for wcswidth
TInt wcswidth1L(  );
TInt wcswidth2L(  );
TInt wcswidth3L(  );
TInt wcswidth4L(  );
//test methods for wcwidth
TInt wcwidth1L(  );
TInt wcwidth2L(  );
//test methods for wmemchr
TInt wmemchr1L(  );
TInt wmemchr2L(  );
//test methods for wmemchr
TInt wmemcpy1L(  );
TInt wmemcpy2L(  );
TInt wmemcpy3L(  );
//test methods for wmemset
TInt wmemset1L(  );
TInt wmemset2L(  );
TInt wmemset3L(  );
//test methods for wmemset
TInt wmemmove1L(  );
TInt wmemmove2L(  );
TInt wmemmove3L(  );
TInt wmemmove4L(  );
//wcsncmp Tests
TInt Wcsncmp_Test_1L( );
TInt Wcsncmp_Test_2L( );
TInt Wcsncmp_Test_3L( );
TInt Wcsncmp_Test_4L( );
TInt Wcsncmp_Test_5L( );
//wcscmp Tests
TInt Wcscmp_Test_1L( );
TInt Wcscmp_Test_2L( );
TInt Wcscmp_Test_3L( );
TInt Wcscmp_Test_4L( );
//wcscat Tests
TInt Wcscat_Test_1L( );
TInt Wcscat_Test_2L( );
TInt Wcscat_Test_3L( );
TInt Wcscat_Test_4L( );
//wcsncat Tests
TInt Wcsncat_Test_1L( );
TInt Wcsncat_Test_2L( );
TInt Wcsncat_Test_3L( );
TInt Wcsncat_Test_4L( );
TInt Wcsncat_Test_5L( );
//wcscpy Tests
TInt Wcscpy_Test_1L(  );
TInt Wcscpy_Test_2L(  );
TInt Wcscpy_Test_3L( );
TInt Wcscpy_Test_4L( );
//wcschr Tests
TInt Wcschr_Test_1L( ) ;
TInt Wcschr_Test_2L( ) ;
TInt Wcschr_Test_3L( ) ;
TInt Wcschr_Test_4L( ) ;
TInt Wcschr_Test_5L( ) ;
//wcsrchr Tests
TInt Wcsrchr_Test_1L( ) ;
TInt Wcsrchr_Test_2L( ) ;
TInt Wcsrchr_Test_3L( ) ;
TInt Wcsrchr_Test_4L( ) ;
TInt Wcsrchr_Test_5L( ) ;
//wcsspn  Tests..
TInt Wcsspn_Test_1L( ) ;
TInt Wcsspn_Test_2L( ) ;
TInt Wcsspn_Test_3L( );
TInt Wcsspn_Test_4L( );
//wcscspn Tests ..
TInt Wcscspn_Test_1L( ) ;
TInt Wcscspn_Test_2L( ) ;
TInt Wcscspn_Test_3L( );
TInt Wcscspn_Test_4L( );
//wcspbrk Tests  ..
TInt Wcspbrk_Test_1L( );
TInt Wcspbrk_Test_2L( );
TInt Wcspbrk_Test_3L( );
TInt Wcspbrk_Test_4L( );
//wcscoll Tests ..
TInt Wcscoll_Test_1L( );
TInt Wcscoll_Test_2L( );
TInt Wcscoll_Test_3L( );
TInt Wcscoll_Test_4L( );
//wcsncpy Tests
TInt Wcsncpy_Test_1L(  );
TInt Wcsncpy_Test_2L(  );
TInt Wcsncpy_Test_3L( );
TInt Wcsncpy_Test_4L( );
TInt Wcsncpy_Test_5L( );
//wcsstr Tests
TInt Wcsstr_Test_1L( );
TInt Wcsstr_Test_2L( );
TInt Wcsstr_Test_3L( );
TInt Wcsstr_Test_4L( );
//swprintf tests
TInt swprintf_Test0L();
TInt swprintf_Test1L(  );
TInt swprintf_Test2L();
TInt swprintf_Test3L();
TInt swprintf_Test4L();
TInt swprintf_Test5L();
TInt swprintf_Test6L();
TInt swprintf_Test7L();
TInt swprintf_Test8L();
TInt swprintf_Test9L();
TInt swprintf_Test10L();
TInt swprintf_Test11L();
TInt swprintf_Test12L();
TInt swprintf_Test13L();
TInt swprintf_Test14L();
TInt swprintf_Test15L();
TInt swprintf_Test16L();
TInt swprintf_Test17L();
TInt swprintf_Test18L();
TInt swprintf_Test19L();
TInt swprintf_Test20L();
TInt swprintf_Test21L();
TInt swprintf_Test22L();
//swscanf tests
TInt swscanf_Test1L();
TInt swscanf_Test2L();
TInt swscanf_Test3L();
TInt swscanf_Test4L();
TInt swscanf_Test5L();
TInt swscanf_Test6L();
TInt swscanf_Test7L();
TInt swscanf_Test8L();
TInt swscanf_Test9L();
TInt swscanf_Test10L();
TInt swscanf_Test11L();
TInt swscanf_Test12L();
TInt swscanf_Test13L();
TInt swscanf_Test14L();
TInt swscanf_Test15L();
TInt swscanf_Test16L();
TInt swscanf_Test17L();
//mblen
TInt mblen_Test0L(  );
TInt mblen_Test1L(  );
TInt mblen_Test2L(  );
TInt mblen_Test3L(  );
TInt mblen_Test4L(  );
TInt mblen_Test5L(  );
TInt mblen_Test6L(  );
TInt mblen_Test7L(  );
TInt mblen_Test8L(  );
TInt mblen_Test9L(  );
//wctomb
TInt wctomb_Test0L(  );
TInt wctomb_Test1L(  );
TInt wctomb_Test2L(  );
TInt wctomb_Test3L(  );
TInt wctomb_Test4L(  );
TInt wctomb_Test5L(  );
//wcstombs
TInt wcstombs_Test0L(  );
TInt wcstombs_Test1L(  );
TInt wcstombs_Test2L(  );
TInt wcstombs_Test3L(  );
TInt wcstombs_Test4L(  );
TInt wcstombs_Test5L(  );
//mbtowc
TInt mbtowc_Test0L(  );
TInt mbtowc_Test1L(  );
TInt mbtowc_Test2L(  );
TInt mbtowc_Test3L(  );
TInt mbtowc_Test4L(  );
TInt mbtowc_Test5L(  );
//mbstowcs
TInt mbstowcs_Test0L(  );
TInt mbstowcs_Test1L(  );
TInt mbstowcs_Test2L(  );
TInt mbstowcs_Test3L(  );
TInt mbstowcs_Test4L(  );
TInt mbstowcs_Test5L(  );
// Wcstol
TInt Wcstol1L(  );
TInt Wcstol2L(  );
TInt Wcstol3L(  );
TInt Wcstol4L(  );
TInt Wcstol5L(  );
TInt Wcstol6L(  );
TInt Wcstol7L(  );
TInt Wcstol8L(  );
TInt Wcstol9L(  );
// Wcstok
TInt Wcstok1L(  );
TInt Wcstok2L(  );
TInt Wcstok3L(  );
TInt Wcstok4L(  );
//wcrtomb
TInt wcrtomb2400(  );
TInt wcrtomb2401(  );
TInt wcrtomb2402(  );
TInt wcrtomb2403(  );
TInt wcrtomb2404(  );
TInt wcrtomb2405(  );
TInt wcrtomb2406(  );
TInt wcrtomb2407(  );
TInt wcrtomb2672(  );	
//wcsrtombs
TInt wcsrtombs2409(  );
TInt wcsrtombs2410(  );        
TInt wcsrtombs2411(  );
TInt wcsrtombs2412(  );
TInt wcsrtombs2413(  );
TInt wcsrtombs2414(  );
TInt wcsrtombs2415(  );
TInt wcsrtombs2423(  );
TInt wcsrtombs2436(  );
TInt wcsrtombs2437(  );
TInt wcsrtombs2438(  );
TInt wcsrtombs2439(  );
TInt wcsrtombs2440(  );
TInt wcsrtombs2442(  );
TInt wcsrtombs2444(  );
TInt wcsrtombs2460(  );
TInt wcsrtombs2673(  );
#ifdef TO_BE_TESTED        
//wprintf  
TInt wprintf_Test0L(  );	
TInt wprintf_Test1L(  );
TInt wprintf_Test2L( );
TInt wprintf_Test3L( );
TInt wprintf_Test4L( );
TInt wprintf_Test5L( );
TInt wprintf_Test6L( );
TInt wprintf_Test7L( );
TInt wprintf_Test8L( );
TInt wprintf_Test9L( );
TInt wprintf_Test10L( );
TInt wprintf_Test11L( );
TInt wprintf_Test12L( );
TInt wprintf_Test13L( );
TInt wprintf_Test14L( );
TInt wprintf_Test15L( );
TInt wprintf_Test16L( );
TInt wprintf_Test17L( );
TInt wprintf_Test18L( );
TInt wprintf_Test19L( );
TInt wprintf_Test20L( );
TInt wprintf_Test21L( );
//vwprintf
TInt vwprintf_Test0L( );
TInt vwprintf_Test1L( );
TInt vwprintf_Test2L( );
TInt vwprintf_Test3L( );
TInt vwprintf_Test4L( );
TInt vwprintf_Test5L( );
TInt vwprintf_Test6L( );
TInt vwprintf_Test7L( );
TInt vwprintf_Test8L( );
TInt vwprintf_Test9L( );
TInt vwprintf_Test10L( );
TInt vwprintf_Test11L( );
TInt vwprintf_Test12L( );
TInt vwprintf_Test13L( );
TInt vwprintf_Test14L( );
TInt vwprintf_Test15L( );
TInt vwprintf_Test16L( );
TInt vwprintf_Test17L( );
TInt vwprintf_Test18L( );
TInt vwprintf_Test19L( );
TInt vwprintf_Test20L( );
TInt vwprintf_Test21L( );		
#endif	//TO_BE_TESTED

//fwprintf 
TInt fwprintf_Test1L( );
TInt fwprintf_Test2L( );
TInt fwprintf_Test3L( );
TInt fwprintf_Test4L( );
TInt fwprintf_Test5L( );
TInt fwprintf_Test6L( );
TInt fwprintf_Test7L( );
TInt fwprintf_Test8L( );
TInt fwprintf_Test9L( );
TInt fwprintf_Test10L( );
TInt fwprintf_Test11L( );
TInt fwprintf_Test12L( );
TInt fwprintf_Test13L( );
TInt fwprintf_Test14L( );
TInt fwprintf_Test15L( );
TInt fwprintf_Test16L( );
TInt fwprintf_Test17L( );
TInt fwprintf_Test18L( );
TInt fwprintf_Test19L( );
TInt fwprintf_Test20L( );
TInt fwprintf_Test21L( );		
// vfwprintf functions
TInt vfwprintf_Test0L( );
TInt vfwprintf_Test1L( );
TInt vfwprintf_Test2L( );
TInt vfwprintf_Test3L( );
TInt vfwprintf_Test4L( );
TInt vfwprintf_Test5L( );
TInt vfwprintf_Test6L( );
TInt vfwprintf_Test7L( );
TInt vfwprintf_Test8L( );
TInt vfwprintf_Test9L( );
TInt vfwprintf_Test10L( );
TInt vfwprintf_Test11L( );
TInt vfwprintf_Test12L( );
TInt vfwprintf_Test13L( );
TInt vfwprintf_Test14L( );
TInt vfwprintf_Test15L( );
TInt vfwprintf_Test16L( );
TInt vfwprintf_Test17L( );
TInt vfwprintf_Test18L( );
TInt vfwprintf_Test19L( );
TInt vfwprintf_Test20L( );
TInt vfwprintf_Test21L( );
//vswprintf
TInt vswprintf_Test0L();
TInt vswprintf_Test1L( );
TInt vswprintf_Test2L( );
TInt vswprintf_Test3L( );
TInt vswprintf_Test4L( );
TInt vswprintf_Test5L( );
TInt vswprintf_Test6L( );
TInt vswprintf_Test7L( );
TInt vswprintf_Test8L( );
TInt vswprintf_Test9L( );
TInt vswprintf_Test10L( );
TInt vswprintf_Test11L( );
TInt vswprintf_Test12L( );
TInt vswprintf_Test13L( );
TInt vswprintf_Test14L( );
TInt vswprintf_Test15L( );
TInt vswprintf_Test16L( );
TInt vswprintf_Test17L( );
TInt vswprintf_Test18L( );
TInt vswprintf_Test19L( );
TInt vswprintf_Test20L( );
TInt vswprintf_Test21L( );	
//wscanf
TInt wscanf_Test1L( );
TInt wscanf_Test2L( );
TInt wscanf_Test3L( );
TInt wscanf_Test4L( );
TInt wscanf_Test5L( );
TInt wscanf_Test6L( );
TInt wscanf_Test7L( );
TInt wscanf_Test8L( );
TInt wscanf_Test9L( );
TInt wscanf_Test10L( );
TInt wscanf_Test11L( );
TInt wscanf_Test12L( );
TInt wscanf_Test13L( );
TInt wscanf_Test14L( );
TInt wscanf_Test15L( );
TInt wscanf_Test16L( );
TInt wscanf_Test17L( );
#ifdef TO_BE_TESTED        		
//vwscanf
TInt vwscanf_Test1L( );
TInt vwscanf_Test2L( );
TInt vwscanf_Test3L( );
TInt vwscanf_Test4L( );
TInt vwscanf_Test5L( );
TInt vwscanf_Test6L( );
TInt vwscanf_Test7L( );
TInt vwscanf_Test8L( );
TInt vwscanf_Test9L( );
TInt vwscanf_Test10L( );
TInt vwscanf_Test11L( );
TInt vwscanf_Test12L( );
TInt vwscanf_Test13L( );
TInt vwscanf_Test14L( );
TInt vwscanf_Test15L( );
TInt vwscanf_Test16L( );
TInt vwscanf_Test17L( );
#endif //TO_BE_TESTED		
//fwscanf
TInt fwscanf_Test1L( );
TInt fwscanf_Test2L( );
TInt fwscanf_Test3L( );
TInt fwscanf_Test4L( );
TInt fwscanf_Test5L( );
TInt fwscanf_Test6L( );
TInt fwscanf_Test7L( );
TInt fwscanf_Test8L( );
TInt fwscanf_Test9L( );
TInt fwscanf_Test10L( );
TInt fwscanf_Test11L( );
TInt fwscanf_Test12L( );
TInt fwscanf_Test13L( );
TInt fwscanf_Test14L( );
TInt fwscanf_Test15L( );
TInt fwscanf_Test16L( );
//vfwscanf
TInt vfwscanf_Test1L( );
TInt vfwscanf_Test2L( );
TInt vfwscanf_Test3L( );
TInt vfwscanf_Test4L( );
TInt vfwscanf_Test5L( );
TInt vfwscanf_Test6L( );
TInt vfwscanf_Test7L( );
TInt vfwscanf_Test8L( );
TInt vfwscanf_Test9L( );
TInt vfwscanf_Test10L( );
TInt vfwscanf_Test11L( );
TInt vfwscanf_Test12L( );
TInt vfwscanf_Test13L( );
TInt vfwscanf_Test14L( );
TInt vfwscanf_Test15L( );
TInt vfwscanf_Test16L( );
// vswscanf
TInt vswscanf_Test1L( );
TInt vswscanf_Test2L( );
TInt vswscanf_Test3L( );
TInt vswscanf_Test4L( );
TInt vswscanf_Test5L( );
TInt vswscanf_Test6L( );
TInt vswscanf_Test7L( );
TInt vswscanf_Test8L( );
TInt vswscanf_Test9L( );
TInt vswscanf_Test10L( );
TInt vswscanf_Test11L( );
TInt vswscanf_Test12L( );
TInt vswscanf_Test13L( );
TInt vswscanf_Test14L( );
TInt vswscanf_Test15L( );
TInt vswscanf_Test16L( );
TInt vswscanf_Test17L( );

//btowc
TInt testbtowc1L( );
TInt testbtowc2L( );
TInt testbtowc3L( );

//btowc
TInt testwctob1L( );
TInt testwctob2L( );
TInt testwctob3L( );

//towlower
TInt testtowlower1L( );
TInt testtowlower2L( );
TInt testtowlower3L( );

//towupper
TInt testtowupper1L( );
TInt testtowupper2L( );
TInt testtowupper3L( );

//mbsrtowcs
TInt testmbsrtowcs1L( );
TInt testmbsrtowcs2L( );
TInt testmbsrtowcs3L( );
TInt testmbsrtowcs4L( );
TInt testmbsrtowcs5L( );

//mbrtowc
TInt testmbrtowc1L( );
TInt testmbrtowc2L( );
TInt testmbrtowc3L( );
TInt testmbrtowc4L( );
TInt testmbrtowc5L( );
TInt testmbrtowc6L( );

//mbrlen
TInt testmbrlen0L(  );
TInt testmbrlen1L(  );
TInt testmbrlen2L(  );
TInt testmbrlen3L(  );
TInt testmbrlen4L(  );
TInt testmbrlen5L(  );
TInt testmbrlen6L(  );
TInt testmbrlen7L(  );
TInt testmbrlen8L(  );
TInt testmbrlen9L(  );


//mbsinit
TInt testmbsinit1L(  );
TInt testmbsinit2L(  );
TInt testmbsinit3L(  );

//wctype
TInt testwctype1L(  );
TInt testwctype2L(  );
TInt testwctype3L(  );
TInt testwctype4L(  );
TInt testwctype5L(  );
TInt testwctype6L(  );
TInt testwctype7L(  );
TInt testwctype8L(  );
TInt testwctype9L(  );
TInt testwctype10L(  );
TInt testwctype11L(  );
TInt testwctype12L(  );
TInt testwctype13L(  );
TInt testwctype14L(  );
TInt testwctype15L(  );
TInt testwctype16L(  );
TInt testwctype17L(  );

//wctrans and towctrans
TInt testwctrans1L(  );
TInt testwctrans2L(  );
TInt testwctrans3L(  );
TInt testwctrans4L(  );
TInt testwctrans5L(  );

///*
//1 wcsnlen API test cases
TInt testwcsnlen1L(  );
TInt testwcsnlen2L(  );
TInt testwcsnlen3L(  );
TInt testwcsnlen4L(  );
TInt testwcsnlen5L(  );

//2 wcsdup API test cases
TInt testwcsdup1L(  );
TInt testwcsdup2L(  );

//3  wcscasecmp API test cases
TInt testwcscasecmp1L(  );
TInt testwcscasecmp2L(  );
TInt testwcscasecmp3L(  );
TInt testwcscasecmp4L(  );
TInt testwcscasecmp5L(  );
TInt testwcscasecmp6L(  );

//4  wcsncasecmp API test cases
TInt testwcsncasecmp1L(  );
TInt testwcsncasecmp2L(  );
TInt testwcsncasecmp3L(  );
TInt testwcsncasecmp4L(  );
TInt testwcsncasecmp5L(  );
TInt testwcsncasecmp6L(  );
TInt testwcsncasecmp7L(  );

//5  wcpcpy API test cases
TInt testwcpcpy1L(  );
TInt testwcpcpy2L(  );

//6  wcpncpy API test cases
TInt testwcpncpy1L(  );
TInt testwcpncpy2L(  );
TInt testwcpncpy3L(  );
TInt testwcpncpy4L(  );

//7  wcsxfrm API test cases
TInt testwcsxfrm1L(  );
TInt testwcsxfrm2L(  );

//8  wcstoul API test cases
TInt testwcstoul1L(  );
TInt testwcstoul2L(  );
TInt testwcstoul3L(  );
TInt testwcstoul4L(  );
TInt testwcstoul5L(  );
TInt testwcstoul6L(  );
TInt testwcstoul7L(  );
TInt testwcstoul8L(  );
TInt testwcstoul9L(  );
TInt testwcstoul10L(  );
TInt testwcstoul11L(  );
TInt testwcstoul12L(  );
TInt testwcstoul13L(  );
TInt testwcstoul14L(  );
TInt testwcstoul15L(  );
TInt testwcstoul16L(  );
TInt testwcstoul17L(  );

//9  wcstoll API test cases
TInt testwcstoll1L(  );
TInt testwcstoll2L(  );
TInt testwcstoll3L(  );
TInt testwcstoll4L(  );
TInt testwcstoll5L(  );
TInt testwcstoll6L(  );
TInt testwcstoll7L(  );
TInt testwcstoll8L(  );
TInt testwcstoll9L(  );
TInt testwcstoll10L(  );
TInt testwcstoll11L(  );
TInt testwcstoll12L(  );
TInt testwcstoll13L(  );
TInt testwcstoll14L(  );
TInt testwcstoll15L(  );
TInt testwcstoll16L(  );
TInt testwcstoll17L(  );

//10  wcstoull API test cases
TInt testwcstoull1L(  );
TInt testwcstoull2L(  );
TInt testwcstoull3L(  );
TInt testwcstoull4L(  );
TInt testwcstoull5L(  );
TInt testwcstoull6L(  );
TInt testwcstoull7L(  );
TInt testwcstoull8L(  );
TInt testwcstoull9L(  );
TInt testwcstoull10L(  );
TInt testwcstoull11L(  );
TInt testwcstoull12L(  );
TInt testwcstoull13L(  );
TInt testwcstoull14L(  );
TInt testwcstoull15L(  );
TInt testwcstoull16L(  );
TInt testwcstoull17L(  );

//11  wcstoq API test cases
TInt testwcstoq1L(  );
TInt testwcstoq2L(  );
TInt testwcstoq3L(  );
TInt testwcstoq4L(  );
TInt testwcstoq5L(  );
TInt testwcstoq6L(  );
TInt testwcstoq7L(  );
TInt testwcstoq8L(  );
TInt testwcstoq9L(  );
TInt testwcstoq10L(  );
TInt testwcstoq11L(  );
TInt testwcstoq12L(  );
TInt testwcstoq13L(  );
TInt testwcstoq14L(  );
TInt testwcstoq15L(  );
TInt testwcstoq16L(  );
TInt testwcstoq17L(  );

//12  wcstouq API test cases
TInt testwcstouq1L(  );
TInt testwcstouq2L(  );
TInt testwcstouq3L(  );
TInt testwcstouq4L(  );
TInt testwcstouq5L(  );
TInt testwcstouq6L(  );
TInt testwcstouq7L(  );
TInt testwcstouq8L(  );
TInt testwcstouq9L(  );
TInt testwcstouq10L(  );
TInt testwcstouq11L(  );
TInt testwcstouq12L(  );
TInt testwcstouq13L(  );
TInt testwcstouq14L(  );
TInt testwcstouq15L(  );
TInt testwcstouq16L(  );
TInt testwcstouq17L(  );

//13  wcstoumax API test cases
TInt testwcstoumax1L(  );
TInt testwcstoumax2L(  );
TInt testwcstoumax3L(  );
TInt testwcstoumax4L(  );
TInt testwcstoumax5L(  );
TInt testwcstoumax6L(  );
TInt testwcstoumax7L(  );
TInt testwcstoumax8L(  );
TInt testwcstoumax9L(  );
TInt testwcstoumax10L(  );
TInt testwcstoumax11L(  );
TInt testwcstoumax12L(  );
TInt testwcstoumax13L(  );
TInt testwcstoumax14L(  );
TInt testwcstoumax15L(  );
TInt testwcstoumax16L(  );
TInt testwcstoumax17L(  );

//14  wcstoimax API test cases
TInt testwcstoimax1L(  );
TInt testwcstoimax2L(  );
TInt testwcstoimax3L(  );
TInt testwcstoimax4L(  );
TInt testwcstoimax5L(  );
TInt testwcstoimax6L(  );
TInt testwcstoimax7L(  );
TInt testwcstoimax8L(  );
TInt testwcstoimax9L(  );
TInt testwcstoimax10L(  );
TInt testwcstoimax11L(  );
TInt testwcstoimax12L(  );
TInt testwcstoimax13L(  );
TInt testwcstoimax14L(  );
TInt testwcstoimax15L(  );
TInt testwcstoimax16L(  );
TInt testwcstoimax17L(  );

//15  mbsnrtowcs API test cases
TInt testmbsnrtowcs1L(  );
TInt testmbsnrtowcs2L(  );
TInt testmbsnrtowcs3L(  );
TInt testmbsnrtowcs4L(  );
TInt testmbsnrtowcs5L(  );

//16  mbsnrtowcs API test cases
TInt testwcsnrtombs1L(  );
TInt testwcsnrtombs2L(  );
TInt testwcsnrtombs3L(  );
TInt testwcsnrtombs4L(  );
TInt testwcsnrtombs5L(  );

//wcstof API test cases
TInt wcstof1L(  );
TInt wcstof2L(  );
TInt wcstof3L(  );
TInt wcstof4L(  );
TInt wcstof5L(  );

//wcsftime.
TInt wcsftime1_aL(  );
TInt wcsftime2_AL(  );
TInt wcsftime3_bL(  );
TInt wcsftime4_BL(  );
TInt wcsftime5_cL(  );
TInt wcsftime6_CL(  );
TInt wcsftime7_dL(  );
TInt wcsftime8_DL(  );
TInt wcsftime9_eL(  );
TInt wcsftime10_FL(  );
TInt wcsftime11_hL(  );
TInt wcsftime12_IL(  );
TInt wcsftime13_jL(  );
TInt wcsftime14_mL(  );
TInt wcsftime15_ML(  );
TInt wcsftime16_nL(  );
TInt wcsftime17_pL(  );
TInt wcsftime18_rL(  );
TInt wcsftime19_RL(  );
TInt wcsftime20_SL(  );
TInt wcsftime21_tL(  );
TInt wcsftime22_TL(  );
TInt wcsftime23_uL(  );
TInt wcsftime25_wL(  );
TInt wcsftime26_xL(  );
TInt wcsftime27_XL(  );
TInt wcsftime28_yL(  );
TInt wcsftime29_YL(  );
TInt wcsftime30L(  );
TInt wcsftime_mixL(  );
TInt wcsftime2L(  );

//wcstod API test cases
TInt wcstod1L(  );
TInt wcstod2L(  );
TInt wcstod3L(  );
TInt wcstod4L(  );
TInt wcstod5L(  );

//wcstold API test cases
TInt wcstold1L(  );
TInt wcstold2L(  );
TInt wcstold3L(  );
TInt wcstold4L(  );
//*/
//wcswcs Tests
TInt Wcswcs_Test_1L( );
TInt Wcswcs_Test_2L( );
TInt Wcswcs_Test_3L( );
TInt Wcswcs_Test_4L( );

	};
#endif 
