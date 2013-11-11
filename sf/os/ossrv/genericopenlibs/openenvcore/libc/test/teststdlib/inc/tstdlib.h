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



#ifndef __TESTSTDLIB_H__
#define __TESTSTDLIB_H__

#include <test/testexecutestepbase.h>

#define LEAST_ATEXIT 32

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
	//class CTstdlib;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
typedef void (*vfuncp) (void);

_LIT(Kmalloc_Test0, "malloc_Test0");
_LIT(Kmalloc_Test1, "malloc_Test1");
_LIT(Kmalloc_Test2, "malloc_Test2");
//calloc
_LIT(Kcalloc_Test0, "calloc_Test0");
_LIT(Kcalloc_Test1, "calloc_Test1");
_LIT(Kcalloc_Test2, "calloc_Test2");
//realloc
_LIT(Krealloc_Test0, "realloc_Test0");
_LIT(Krealloc_Test1, "realloc_Test1");
_LIT(Krealloc_Test2, "realloc_Test2");
_LIT(Krealloc_Test3, "realloc_Test3");
_LIT(Krealloc_Test4, "realloc_Test4");
//free
_LIT(Kfree_Test0, "free_Test0");

//getenv
_LIT(Kgetenv_Test0, "getenv_Test0");
_LIT(Kgetenv_Test1, "getenv_Test1");
//setenv
_LIT(Ksetenv_Test0, "setenv_Test0");
_LIT(Ksetenv_Test1, "setenv_Test1");
_LIT(Ksetenv_Test2, "setenv_Test2");
_LIT(Ksetenv_Test3, "setenv_Test3");
_LIT(Ksetenv_Test4, "setenv_Test4");
//putenv
_LIT(Kputenv_Test0, "putenv_Test0");
_LIT(Kputenv_Test1, "putenv_Test1");
_LIT(Kputenv_Test2, "putenv_Test2");
_LIT(Kputenv_Test3, "putenv_Test3");
//unsetenv
_LIT(Kunsetenv_Test0, "unsetenv_Test0");
//system
_LIT(Ksystem_Test0, "system_Test0");
_LIT(Ksystem_Test1, "system_Test1");
_LIT(Ksystem_Test2, "system_Test2");
_LIT(Ksystem_Test3, "system_Test3");
_LIT(Ksystem_Test4, "system_Test4");
_LIT(Ksystem_Test5, "system_Test5");
_LIT(Ksystem_Test6, "system_Test6");
//abort
_LIT(Kabort_Test0, "abort_Test0");

_LIT(Kfopen_Test, "fopen_Test");
//exit
_LIT(Kexit_Test0, "exit_Test0");
_LIT(Kexit_Test1, "exit_Test1");
//_Exit
_LIT(K_exitE_Test0, "_exitE_Test0");
//_exit
_LIT(K_exit_Test0, "_exit_Test0");
//atexit
_LIT(Katexit_Test0, "atexit_Test0");
_LIT(Katexit_Test1, "atexit_Test1");

//abs	
_LIT(Kabs_good_param, "abs_good_param");
_LIT(Kabs_pos_max, "abs_pos_max");
_LIT(Kabs_neg_max, "abs_neg_max");
//labs
_LIT(Klabs_good_param, "labs_good_param");
_LIT(Klabs_pos_max, "labs_pos_max");
_LIT(Klabs_neg_max, "labs_neg_max");
//llabs
_LIT(Kllabs_good_param, "llabs_good_param");
_LIT(Kllabs_pos_max, "llabs_pos_max");
_LIT(Kllabs_neg_max, "llabs_neg_max");

//atoi
_LIT(Katoi_null_string, "atoi_null_string");
_LIT(Katoi_good_param, "atoi_good_param");
//atof
_LIT(Katof_null_string, "atof_null_string");
_LIT(Katof_pos_floatnum, "atof_pos_floatnum");
_LIT(Katof_neg_floatnum, "atof_neg_floatnum");
_LIT(Katof_alpha, "atof_alpha");
_LIT(Katof_alpha_floatnum, "atof_alpha_floatnum");
_LIT(Katof_pos_floatnum_alpha, "atof_pos_floatnum_alpha");
_LIT(Katof_neg_floatnum_alpha, "atof_neg_floatnum_alpha");
_LIT(Katof_leading_zero, "atof_leading_zero");
_LIT(Katof_neg_leading_zero, "atof_neg_leading_zero");
_LIT(Katof_pos_floatnum_pos_expo, "atof_pos_floatnum_pos_expo");
_LIT(Katof_neg_floatnum_pos_expo, "atof_neg_floatnum_pos_expo");
_LIT(Katof_pos_floatnum_neg_expo, "atof_pos_floatnum_neg_expo");
_LIT(Katof_neg_floatnum_neg_expo, "atof_neg_floatnum_neg_expo");
_LIT(Katof_locale, "atof_locale");
//atol    
_LIT(Katol_null_string, "atol_null_string");
_LIT(Katol_good_param, "atol_good_param");
//atoll
_LIT(Katollbasic, "atollbasic");
_LIT(Katollspacecheck, "atollspacecheck");
_LIT(Katollsignedsrc, "atollsignedsrc");
_LIT(Katolloctalcheck, "atolloctalcheck");
_LIT(Katollhexcheck, "atollhexcheck");
_LIT(Katolldigitfirstcheck, "atolldigitfirstcheck");
_LIT(Katollalphafirstcheck, "atollalphafirstcheck");
_LIT(Katollalphacheck, "atollalphacheck");
_LIT(Katollleadingzeroscheck, "atollleadingzeroscheck");

//div
_LIT(Kdiv_good_param, "div_good_param");
//ldiv
_LIT(Kldiv_good_param, "ldiv_good_param");
//lldiv
_LIT(Klldiv_good_param0, "lldiv_good_param0");
_LIT(Klldiv_good_param1, "lldiv_good_param1");        
_LIT(Klldiv_good_param2, "lldiv_good_param2");        
_LIT(Klldiv_good_param3, "lldiv_good_param3");        
_LIT(Klldiv_good_param4, "lldiv_good_param4");

//rand
_LIT(Krand, "rand");
//srand
_LIT(Ksrand, "srand");

//strtol
_LIT(Kstrtol_good_param, "strtol_good_param");
_LIT(Kstrtol_null_string, "strtol_null_string");
//strtod
_LIT(Kstrtod_null_string, "strtod_null_string");
_LIT(Kstrtod_good_param, "strtod_good_param");
_LIT(Kstrtod_nan, "strtod_nan");
_LIT(Kstrtod_inf, "strtod_inf");
_LIT(Kstrtod_neg_cases, "strtod_neg_cases");
_LIT(Kstrtod_misc_cases, "strtod_misc_cases");
_LIT(Kstrtod_locale, "strtod_locale");
//strtoul
_LIT(Kstrtoul_null_string, "strtoul_null_string");
_LIT(Kstrtoul_good_param, "strtoul_good_param");
_LIT(Kstrtoul_neg_num_alpha, "strtoul_neg_num_alpha");
_LIT(Kstrtoul_neg_leading_zeroes, "strtoul_neg_leading_zeroes");
//strmon
_LIT(KTeststrmon, "strmon_cases");
//qsort
_LIT(Kqsort_integers, "qsort_integers");
_LIT(Kqsort_strings, "qsort_strings");
//bsearch
_LIT(Kbinsearch_integers, "binsearch_integers");
_LIT(Kbinsearch_strings, "binsearch_strings");

//isatty
_LIT(Kisatty_Test0, "isatty_Test0");
_LIT(Kisatty_Test1, "isatty_Test1");
//usleep
_LIT(Kusleep_Test0, "usleep_Test0");
_LIT(Kusleep_Test1, "usleep_Test1");
_LIT(Kusleep_Test2, "usleep_Test2");
//getcwd
_LIT(Kgetcwd_Test0, "getcwd_Test0");
_LIT(Kgetcwd_Test1, "getcwd_Test1");
_LIT(Kgetcwd_Test2, "getcwd_Test2");
_LIT(Kgetcwd_Test3, "getcwd_Test3");
//tmpfile
_LIT(Ktmpfile_Test0, "tmpfile_Test0");
_LIT(Ktmpfile_Test1, "tmpfile_Test1");
//tmpnam
_LIT(Ktmpnam_Test0, "tmpnam_Test0");
_LIT(Ktmpnam_Test1, "tmpnam_Test1");
//lseek
_LIT(KTestlseek, "Testlseek");
//realpath
_LIT(Krealpath_Test0, "realpath_Test0");
_LIT(Krealpath_Test1, "realpath_Test1");
_LIT(Krealpath_Test2, "realpath_Test2");
_LIT(Krealpath_Test3, "realpath_Test3");
_LIT(Krealpath_Test4, "realpath_Test4");

//perror
_LIT(Kperror_Test0, "perror_Test0");
//mkstemp
_LIT(Kmkstemp_Test0, "mkstemp_Test0");
_LIT(Kmkstemp_Test1, "mkstemp_Test1");
//confstr
_LIT(Kconfstr_Test0, "confstr_Test0");
_LIT(Kconfstr_Test1, "confstr_Test1");

//fpathconf
_LIT(Kfpathconf_Test0, "fpathconf_Test0");
_LIT(Kfpathconf_Test1, "fpathconf_Test1");
//file locks -flockfile, funlockfile
_LIT(Kfilelock_Test0, "filelock_Test0");
_LIT(Kfilelock_Test1, "filelock_Test1");
//file locks -ftrylockfile, flockfile, funlockfile
_LIT(Kfilelock_Test2, "filelock_Test2");
_LIT(Kfilelock_Test3, "filelock_Test3");
//truncate
_LIT(Ktruncate_test, "truncate_test");
//getopt
_LIT(KgetoptTest1, "getoptTest1");
_LIT(KgetoptTest2, "getoptTest2");
_LIT(KgetoptTest3, "getoptTest3");
_LIT(KgetoptTest4, "getoptTest4");
_LIT(KgetoptTest5, "getoptTest5");
_LIT(KgetoptTest6, "getoptTest6");
//getopt_long
_LIT(KgetoptTest_long1, "getoptTest_long1");
_LIT(KgetoptTest_long2, "getoptTest_long2");
_LIT(KgetoptTest_long3, "getoptTest_long3");
_LIT(KgetoptTest_long4, "getoptTest_long4");
_LIT(KTestgetc_unlocked, "Testgetc_unlocked");
_LIT(KTestgetchar_unlocked, "Testgetchar_unlocked");
_LIT(KTestRealPath5,"TestRealPath5");
_LIT(KTeststats, "Teststats");
_LIT(KTestsigemptyset, "Testsigemptyset");
//strfmon
_LIT(Kstrfmon1, "strfmon1");
_LIT(KTestlseek1, "Testlseek1");
_LIT(KTestlseek2, "Testlseek2");
_LIT(KTestlseek3, "Testlseek3");
_LIT(KgetoptTest_long5, "getoptTest_long5");


_LIT(KsetjmpTest,"setjmpTest");
_LIT(KabortTest, "abortTest");
_LIT(KchownTest,"chownTest");
_LIT(KiconvTest,"iconvTest");
_LIT(KsetgrentTest,"setgrentTest");
_LIT(KwcreatTest,"wcreatTest");
_LIT(Kbinsearch_integers1, "binsearch_integers1");
_LIT(Kcalloc_Test3, "calloc_Test3");
_LIT(Kcalloc_Test4, "calloc_Test4");
_LIT(Kgetenv_Test2, "getenv_Test2");
_LIT(Kreallocf_Test0, "reallocf_Test0");
_LIT(Katexit_Test2, "atexit_Test2");
_LIT(Kstrtol_space_string, "strtol_space_string");
_LIT(Kstrtol_extreme_string, "strtol_extreme_string");
_LIT(Kstrtol_extreme_string1, "strtol_extreme_string1");
_LIT(Kstrtoll_extreme_string, "strtoll_extreme_string");
_LIT(Kstrtoll_extreme_string1, "strtoll_extreme_string1");
_LIT(Kstrtoll_good_param, "strtoll_good_param");
_LIT(Kstrtoul_extreme_string, "strtoul_extreme_string");
_LIT(Kstrtoul_extreme_string1, "strtoul_extreme_string1");
_LIT(Kqsort_integers1, "qsort_integers1");
_LIT(Kqsort_integers2, "qsort_integers2");
_LIT(Kqsort_integers3, "qsort_integers3");
_LIT(Kstrtoullbasic, "strtoullbasic");
_LIT(Kstrtoullneg, "strtoullneg");
_LIT(Kstrtoullpos, "strtoullpos");
_LIT(Kstrtoullbase, "strtoullbase");
_LIT(Kstrtoulpos, "strtoulpos");
_LIT(Kstrtoimaxbasic, "strtoimaxbasic");
_LIT(Kstrtoimaxbasic1, "strtoimaxbasic1");
_LIT(Kstrtoimaxbasic2, "strtoimaxbasic2");
_LIT(Kstrtoumaxbasic, "strtoumaxbasic");
_LIT(Kstrtoumaxbasic1, "strtoumaxbasic1");
_LIT(Kstrtoumaxbasic2, "strtoumaxbasic2");
_LIT(Kconfstr_Test2, "confstr_Test2");
_LIT(Ktmpfile_fseek, "tmpfile_fseek");
_LIT(KtestSymLink, "testSymLink");

class CTestStdlib : public CTestStep
	{
public:
	~CTestStdlib(); 
	CTestStdlib(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	TInt malloc_Test0();
	TInt malloc_Test1(  );
	TInt malloc_Test2(  );
	
	TInt calloc_Test0(  );
	TInt calloc_Test1(  );
	TInt calloc_Test2(  );

	TInt realloc_Test0(  );
	TInt realloc_Test1(  );
	TInt realloc_Test2(  );
	TInt realloc_Test3(  );
	TInt realloc_Test4(  );

	TInt free_Test0(  );
	
	TInt getenv_Test0(  );
	TInt getenv_Test1(  );

	TInt setenv_Test0(  );
	TInt setenv_Test1(  );
	TInt setenv_Test2(  );
	TInt setenv_Test3(  );
	TInt setenv_Test4(  );

	TInt putenv_Test0(  );
	TInt putenv_Test1(  );
	TInt putenv_Test2(  );
	TInt putenv_Test3(  );

	TInt unsetenv_Test0(  );

	TInt system_Test0(  );
	TInt system_Test1(  );
	TInt system_Test2(  );
	TInt system_Test3(  );
	TInt system_Test4(  );
	TInt system_Test5(  );
	TInt system_Test6(  );
	
	TInt abort_Test0(  );
	TInt fopen_Test(  );
	
	TInt exit_Test0(  );
	TInt exit_Test1(  );

	TInt _exitE_Test0(  );

	TInt _exit_Test0(  );

	TInt atexit_Test0(  );
	TInt atexit_Test1(  );
	
	TInt abs_good_param(  );
	TInt abs_pos_max(  );
	TInt abs_neg_max(  );

	TInt labs_good_param(  );
	TInt labs_pos_max(  );
	TInt labs_neg_max(  );

	TInt llabs_good_param(  );
	TInt llabs_pos_max(  );
	TInt llabs_neg_max(  );
	
	TInt atoi_null_string(  );
	TInt atoi_good_param(  );

	TInt atof_null_string(  );
	TInt atof_pos_floatnum(  );
	TInt atof_neg_floatnum(  );
	TInt atof_alpha(  );
	TInt atof_alpha_floatnum(  );
	TInt atof_pos_floatnum_alpha(  );
	TInt atof_neg_floatnum_alpha (  );
	TInt atof_leading_zero (  );
	TInt atof_neg_leading_zero (  );
	TInt atof_pos_floatnum_pos_expo(  );
	TInt atof_neg_floatnum_pos_expo(  );
	TInt atof_pos_floatnum_neg_expo (  );
	TInt atof_neg_floatnum_neg_expo(  );
	TInt atof_locale( );

	TInt atol_null_string(  );
	TInt atol_good_param(  );

	TInt atollbasic(  );
	TInt atollspacecheck(  );
	TInt atollsignedsrc(  );
	TInt atolloctalcheck(  );
	TInt atollhexcheck(  );
	TInt atolldigitfirstcheck(  );
	TInt atollalphafirstcheck(  );
	TInt atollalphacheck(  );
	TInt atollleadingzeroscheck(  );
	
	TInt div_good_param(  );
	TInt ldiv_good_param(  );

	TInt lldiv_good_param0(  );
	TInt lldiv_good_param1(  );
	TInt lldiv_good_param2(  );
	TInt lldiv_good_param3(  );
	TInt lldiv_good_param4(  );
	
	TInt randL(  );
	TInt srandL(  );

	TInt strtol_good_param(  );
	TInt strtol_null_string(  );

	TInt strtod_null_string(  );
	TInt strtod_good_param(  );
	TInt strtod_nan(  );
	TInt strtod_inf(  );
	TInt strtod_neg_cases(  );
	TInt strtod_misc_cases(  );
	TInt strtod_locale( );

	TInt strtoul_null_string(  );
	TInt strtoul_good_param(  );
	TInt strtoul_neg_leading_zeroes(  );
	TInt strtoul_neg_num_alpha(  );
	TInt strmon_cases();
	
	TInt qsort_integers(  );
	TInt qsort_strings(  );

	TInt binsearch_integers(  );
	TInt binsearch_strings(  );
	
		TInt isatty_Test0(  );
	TInt isatty_Test1(  );

	TInt usleep_Test0(  );
	TInt usleep_Test1(  );
	TInt usleep_Test2(  );
	
	TInt getcwd_Test0(  );
	TInt getcwd_Test1(  );
	TInt getcwd_Test2(  );
	TInt getcwd_Test3(  );

	TInt tmpfile_Test0(  );
	TInt tmpfile_Test1(  );

	TInt tmpnam_Test0(  );
	TInt tmpnam_Test1(  );
	TInt realpath_Test0(  );
	TInt realpath_Test1(  );
	TInt realpath_Test2(  );
	TInt realpath_Test3(  );
	TInt realpath_Test4(  );

	TInt perror_Test0(  );

	TInt mkstemp_Test0(  );
	TInt mkstemp_Test1(  );

	TInt confstr_Test0(  );
	TInt confstr_Test1(  );
	
	TInt fpathconf_Test0(  );
	TInt fpathconf_Test1(  );

	TInt filelock_Test0(  );
	TInt filelock_Test1(  );
	TInt filelock_Test2(  );
	TInt filelock_Test3(  );

	TInt truncate_test();
	TInt getoptTest1(   );
	TInt getoptTest2(   );
	TInt getoptTest3(   );
	TInt getoptTest4(   );
	TInt getoptTest5(   );    	
	TInt getoptTest6(   );
		
	TInt getoptTest_long1();
	TInt getoptTest_long2();
	TInt getoptTest_long3();
	TInt getoptTest_long4();  
	
	TInt Testlseek();
	TInt Testgetc_unlocked();
	TInt Testgetchar_unlocked();
	TInt Teststats();
	void ReadStringParam(char* );
	void ReadIntParam(TInt&);
	TInt TestRealPath5();
	TInt Testsigemptyset();
	TInt strfmon1();
	TInt Testlseek1();
	TInt Testlseek2();
	TInt Testlseek3();
	TInt getoptTest_long5();

	
	TInt setjmpTest();
	TInt abortTest();
	TInt chownTest();
	TInt iconvTest();
	TInt setgrentTest();
	TInt wcreatTest();
	TInt binsearch_integers1(  );
	TInt calloc_Test3(  );
	TInt calloc_Test4(  );
	TInt getenv_Test2(  );
	TInt reallocf_Test0(  );
	TInt atexit_Test2(  );
	TInt strtol_space_string(  );
	TInt strtol_extreme_string(  );
	TInt strtol_extreme_string1(  );
	TInt strtoll_extreme_string(  );
	TInt strtoll_extreme_string1(  );
	TInt strtoul_extreme_string(  );
	TInt strtoul_extreme_string1(  );
	TInt strtoll_good_param(  );
	TInt qsort_integers1(  );
	TInt qsort_integers2(  );
	TInt qsort_integers3(  );
	TInt strtoullbasic();
	TInt strtoullneg();
	TInt strtoullpos();
	TInt strtoullbase();
	TInt strtoulpos();
	TInt strtoimaxbasic();
	TInt strtoimaxbasic1();
	TInt strtoimaxbasic2();
	TInt strtoumaxbasic();
	TInt strtoumaxbasic1();
	TInt strtoumaxbasic2();
	TInt confstr_Test2(  );
	TInt tmpfile_fseek();
	TInt testSymLink();
	
public:
	TInt iParamCnt;
	};
#endif 
