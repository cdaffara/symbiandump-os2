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



#ifndef __TESTWCTYPE_H__
#define __TESTWCTYPE_H__

#include <test/testexecutestepbase.h>
#include <e32math.h>
#include <wctype.h>
#include <stdlib.h>
#include <string.h>

_LIT(Ktest_iswalpha, "test_iswalpha");
_LIT(Ktest_iswalnum, "test_iswalnum");
_LIT(Ktest_iswdigit, "test_iswdigit");
_LIT(Ktest_iswcntrl, "test_iswcntrl");
_LIT(Ktest_iswxdigit, "test_iswxdigit");
_LIT(Ktest_iswspace, "test_iswspace");
_LIT(Ktest_iswblank, "test_iswblank");
_LIT(Ktest_iswpunct, "test_iswpunct");
_LIT(Ktest_iswlower, "test_iswlower");
_LIT(Ktest_iswupper, "test_iswupper");
_LIT(Ktest_iswprint, "test_iswprint");
_LIT(Ktest_iswgraph, "test_iswgraph");
_LIT(Ktest_iswctype, "test_iswctype");

class CTestWctype : public CTestStep
	{
public:
	~CTestWctype(); 
	CTestWctype(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	TInt test_iswalpha();
	TInt test_iswalnum();
	TInt test_iswdigit();
	TInt test_iswcntrl();
	TInt test_iswxdigit();
	TInt test_iswspace();
	TInt test_iswblank();
	TInt test_iswpunct();
	TInt test_iswlower();
	TInt test_iswupper();
	TInt test_iswprint();
	TInt test_iswgraph();
	TInt test_iswctype();
	
	
public:  //data
	TInt iParamCnt;
	};
#endif 
