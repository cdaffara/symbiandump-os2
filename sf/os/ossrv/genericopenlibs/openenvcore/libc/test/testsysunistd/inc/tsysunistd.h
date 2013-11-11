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



#ifndef __TESTSYSUNISTD_H__
#define __TESTSYSUNISTD_H__

#include <test/TestExecuteStepBase.h>
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>



#include <e32svr.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <limits.h>


#define KMAXCHARS	100

_LIT(Kftruncate_file, "ftruncate_file");
_LIT(Kftruncate_dir, "ftruncate_dir");
_LIT(Kftruncate_num, "ftruncate_num");
_LIT(Kftruncate_siz, "ftruncate_siz");
_LIT(Kfchdir_dir, "fchdir_dir");
_LIT(Kfchdir_file, "fchdir_file");
_LIT(Kfchdir_num, "fchdir_num");
_LIT(Kfdatasync_file, "fdatasync_file");
_LIT(Kgetpagesize, "getpagesize");
_LIT(Kgetpriority, "getpriority");
_LIT(Kgetpriority_other, "getpriority_other");
_LIT(Ksetpriority_get, "setpriority_get");
_LIT(Ksetpriority_high, "setpriority_high");
_LIT(Ksetpriority_other_lower, "setpriority_other_lower");
_LIT(Ksetpriority_other_higher, "setpriority_other_higher");
_LIT(Ksetpriority_other, "setpriority_other");
_LIT(Knice_within_range, "nice_within_range");
_LIT(Knice_more_than_highest, "nice_more_than_highest");
_LIT(Knice_less_than_lowest, "nice_less_than_lowest");
_LIT(Kpathconf_valid, "pathconf_valid");
_LIT(Kpathconf_invalid, "pathconf_invalid");
_LIT(Kuname, "uname");
_LIT(Kuname_null, "uname_null");
_LIT(Kftruncate_negTest , "ftruncate_negTest");
_LIT(Ktruncate_negTest , "truncate_negTest");
_LIT(Kfcntl_largefileTest , "fcntl_largefileTest");

class CTestSysunistd : public CTestStep
	{
public:
	~CTestSysunistd(); 
	CTestSysunistd(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:

	TInt ftruncate_file();
	TInt ftruncate_dir();
	TInt ftruncate_num();
	TInt ftruncate_siz();
	TInt fchdir_dir();
	TInt fchdir_file();
	TInt fchdir_num();
	TInt fdatasync_file();
	TInt getpagesizeL();
	TInt getpriorityL();
	TInt getpriority_other();
	TInt setpriority_get();
	TInt setpriority_high();
	TInt setpriority_other_lower();
	TInt setpriority_other_higher();
	TInt setpriority_other();
	TInt nice_within_range();
	TInt nice_more_than_highest();
	TInt nice_less_than_lowest();
	TInt pathconf_valid();
	TInt pathconf_invalid();
	TInt unameL();
	TInt uname_null();
	TInt ftruncate_negTest();
	TInt truncate_negTest();
	TInt fcntl_largefileTest();
	};
#endif 

