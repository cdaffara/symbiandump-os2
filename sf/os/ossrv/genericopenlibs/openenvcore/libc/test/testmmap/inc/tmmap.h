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



#ifndef __TESTPROGNAME_H__
#define __TESTPROGNAME_H__

#include <test/testexecutestepbase.h>
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>
#include <e32svr.h>
#include "tmmap.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/mman.h>
#include <e32base.h>

_LIT(Kmemmap, "memmap");
_LIT(Kmemmap1, "memmap1");
_LIT(Kmemmap2, "memmap2");
_LIT(Kmemmap3, "memmap3");
_LIT(Kmemmap4, "memmap4");
_LIT(Kmemmap5, "memmap5");
_LIT(Kmemmap6, "memmap6");

class CTestMmap : public CTestStep
	{
public:
	~CTestMmap(); 
	CTestMmap(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	TInt memmap();
	TInt memmap1();
	TInt memmap2();
	TInt memmap3();
	TInt memmap4();
	TInt memmap5();
	TInt memmap6();
	};
#endif 
