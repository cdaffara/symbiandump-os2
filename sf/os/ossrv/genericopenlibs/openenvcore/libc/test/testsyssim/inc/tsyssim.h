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



#ifndef __TESTSYSSIM_H__
#define __TESTSYSSIM_H__

#include <test/testexecutestepbase.h>
#include <e32svr.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <sys/stat.h>

_LIT(Kgetgrpid, "getgrpid");
_LIT(Ksetgrpid, "setgrpid");
_LIT(Kgeteffgrpid, "geteffgrpid");
_LIT(Kseteffgrpid, "seteffgrpid");
_LIT(Ksetregrpid, "setregrpid");
_LIT(Kgetusrid, "getusrid");
_LIT(Ksetusrid, "setusrid");
_LIT(Kgeteffusrid, "geteffusrid");
_LIT(Kseteffusrid, "seteffusrid");
_LIT(Ksetreusrid, "setreusrid");
_LIT(Kgetpgrpid, "getpgrpid");
_LIT(Ksetpgrpid, "setpgrpid");
_LIT(Kgetprocgrp, "getprocgrp");
_LIT(Ksetprocgrp, "setprocgrp");
_LIT(Kgetgrps, "getgrps");
_LIT(Ksetsessid, "setsessid");
_LIT(Kgetpprocid, "getpprocid");
_LIT(Kfilemask, "filemask");
_LIT(Kchowner, "chowner");
_LIT(Klchowner, "lchowner");
_LIT(Kinitgrps, "initgrps");
_LIT(Kschedyield, "schedyield");
_LIT(Kgetproioritymax, "getproioritymax");
_LIT(Kgetproioritymin, "getproioritymin");


class CTestSyssim : public CTestStep
	{
public:
	~CTestSyssim(); 
	CTestSyssim(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
    TInt getgrpid();
    TInt setgrpid();
    TInt geteffgrpid();
    TInt seteffgrpid();
    TInt setregrpid();
    TInt getusrid();
    TInt setusrid();
    TInt geteffusrid();
    TInt seteffusrid();
    TInt setreusrid();
    TInt getpgrpid();
    TInt setpgrpid();
    TInt getprocgrp();
    TInt setprocgrp();
    TInt getgrps();
    TInt setsessid();
    TInt getpprocid();
    TInt filemask();
    TInt chowner();
    TInt lchowner();
    TInt initgrps();
    TInt schedyield();
    TInt getproioritymax();
    TInt getproioritymin();
	};
#endif 
