/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : twcharapi.h
*
*/




#ifndef TWCHARAPI_H
#define TWCHARAPI_H

#include <test/testexecutestepbase.h>
#include <e32svr.h>

#include <u32std.h>
#include <unicode.h>
#include <collate.h>

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <_ansi.h>
#include <sys/stat.h> //S_ISWUSR
#include <sys/syslimits.h> //PATH_MAX
#include <unistd.h>
#include <wchar.h>
#include <locale.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <utime.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/uio.h>

_LIT(Ktwasctime, "twasctime");
_LIT(Ktwctime, "twctime");
_LIT(Ktwcslcat, "twcslcat");
_LIT(Ktwcslcpy, "twcslcpy");
_LIT(Ktwsetlocale, "twsetlocale");
_LIT(Ktwperror, "twperror");
_LIT(Ktwcserror, "twcserror");
_LIT(Ktwfindfirst, "twfindfirst");
_LIT(Ktwfindclose, "twfindclose");
_LIT(Ktwtelldir, "twtelldir");
_LIT(Ktwseekdir, "twseekdir");
_LIT(Ktwsetlocale1, "twsetlocale1");
_LIT(Ktwfindfirst1, "twfindfirst1");
_LIT(Ktwfindnext1, "twfindnext1");
_LIT(Kwcreat2, "wcreat2");
_LIT(Kwpopen3_1, "wpopen3_1");
_LIT(Ktwcslcpy1, "twcslcpy1");
_LIT(Kwopen1, "wopen1"); 
_LIT(Kwunlink1, "wunlink1"); 
_LIT(Kwrmdir1, "wrmdir1");
_LIT(Kwmkdir1, "wmkdir1");
_LIT(Kwchmod1, "wchmod1");
_LIT(Kwchdir1, "wchdir1");
_LIT(Kwstat1, "wstat1");
_LIT(Kwstat2, "wstat2");
_LIT(Kwrename1, "wrename1");
_LIT(Kwrename2, "wrename2");
_LIT(Kwpopen_2,"wpopen_2");
_LIT(Kwpopen_3,"wpopen_3");

class CTestWCharApi : public CTestStep
	{
public:
	~CTestWCharApi(); 
	CTestWCharApi(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	void LocateSupportedDLLs();	
	
protected:
    TInt twseekdir();
	TInt twtelldir();
	TInt twasctime();
	TInt twctime();
	TInt twcslcat();
	TInt twcslcpy();
	TInt twsetlocale();
	TInt twperror();
	TInt twcserror();
	TInt twfindfirst();
	TInt twfindclose();	
	TInt twsetlocale1();
	TInt twfindfirst1();
	TInt twfindnext1();	
	TInt wcreat2();
	TInt wpopen3_1();
	TInt twcslcpy1();
	TInt wopen1();
	TInt wunlink1();
	TInt wrmdir1();
	TInt wmkdir1();
	TInt wchmod1();
	TInt wchdir1();
	TInt wstat1();
	TInt wstat2();
	TInt wrename1();
	TInt wrename2();
	TInt wpopen_2();
	TInt wpopen_3();
private:    // Data
    #define SUPPORTED_LOCALES 104
    struct localeStrings
		{
		char localeName[50];
		TInt flag;		
		};

	struct localeStrings locales[SUPPORTED_LOCALES];	
	};
#endif 
