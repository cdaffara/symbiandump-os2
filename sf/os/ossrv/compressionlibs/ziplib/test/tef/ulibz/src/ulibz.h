/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : ulibz.h
* 
*
*/




#ifndef __ULIBZ_H__
#define __ULIBZ_H__

#include <test/testexecutestepbase.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>


_LIT(KDIRBASE,"C:\\Libz");
_LIT(KDIRPATH, "C:\\Libz\\Test1\\");
_LIT(KGZFILE, "C:\\Libz\\Test1\\bye.gz");


//literals for libz operations
_LIT(KLibzcomp_decomp, "libzcomp_decomp");
_LIT(KLibzdefl_Infl, "libzdefl_Infl");
_LIT(KLibzgzio, "libzgzio");
_LIT(KLibzversion, "libzversion");

class CTestlibz : public CTestStep
	{
public:
	~CTestlibz(); 
	CTestlibz(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	//libz Test cases
	TInt libzcomp_decomp();
    TInt libzdefl_Infl();
	TInt libzdeflate(Byte*, uLong);
	TInt libzinflate(Byte*, uLong, Byte*, uLong);	
	TInt libzgzio();
	TInt libzversion();
	RFs iRfs;
	RFile iRfile;
	};
#endif // __ULIBZ_H__

