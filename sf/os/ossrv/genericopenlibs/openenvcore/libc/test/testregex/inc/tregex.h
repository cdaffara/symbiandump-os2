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



#ifndef __TESTREGEX_H__
#define __TESTREGEX_H__

#include <test/testexecutestepbase.h>
#include <e32svr.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>

_LIT(Kregcomp_escape, "regcomp_escape");
_LIT(Kregcomp_badrpt, "regcomp_badrpt");
_LIT(Kregcomp_basic, "regcomp_basic");
_LIT(Kregcomp_complex, "regcomp_complex");
_LIT(Kregcomp_eparen, "regcomp_eparen");
_LIT(Kregcomp_ebrack, "regcomp_ebrack");
_LIT(Kregexec_basic, "regexec_basic");
_LIT(Kregexec_icase, "regexec_icase");
_LIT(Kregexec_notbol, "regexec_notbol");
_LIT(Kregexec_noteol, "regexec_noteol");
_LIT(Kregexec_range, "regexec_range");
_LIT(Kregfree_basic, "regfree_basic");
_LIT(Kregfree_multiple, "regfree_multiple");
_LIT(Kregerror_basic, "regerror_basic");
_LIT(Kregcomp1, "regcomp1");
_LIT(Kregcomp2, "regcomp2");
_LIT(Kregcomp3, "regcomp3");
_LIT(Kregcomp_nospec, "regcomp_nospec");
_LIT(Kregcomp_icase2, "regcomp_icase2");
_LIT(Kregcomp_newline, "regcomp_newline");
_LIT(Kregfree_multiple1, "regfree_multiple1");
_LIT(Kregerror_basic1, "regerror_basic1");


class CTestRegex : public CTestStep
	{
public:
	~CTestRegex(); 
	CTestRegex(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	TInt regcomp_escape();
     TInt regcomp_badrpt();
     TInt regcomp_basic();
     TVerdict regcomp_complex();
     TInt regcomp_eparen();
     TInt regcomp_ebrack();
     TInt regexec_basic();
     TInt regexec_icase();
     TInt regexec_notbol();
     TInt regexec_noteol();
     TInt regexec_range();
     TInt regfree_basic();
     TInt regfree_multiple();
     TInt regerror_basic();
     TInt regcomp1();
     TInt regcomp2();     
     TInt regcomp3();     
     TInt regcomp_nospec();
     TInt regcomp_icase2();
     TInt regcomp_newline();
     TInt regerror_basic1();
     TInt regfree_multiple1();
 public:
 	TInt iParamCnt;
};
#endif 
