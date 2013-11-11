/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : tlocale.h
*
*/




#ifndef __TESTLOCALE_H__
#define __TESTLOCALE_H__

#include <test/TestExecuteStepBase.h>
#include <e32svr.h>
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <errno.h>
#include <wctype.h>
#include <stdlib.h>
#include <langinfo.h>
#include "tlocaledefines.h"


_LIT(KTestctypeApi,"TestctypeApi");
_LIT(KTestisascii, "Testisascii");
_LIT(KTesttoascii, "Testtoascii");
_LIT(KTestmblen0,"Testmblen0");
_LIT(KTestmblen1,"Testmblen1");
_LIT(KTestmblen2,"Testmblen2");
_LIT(KTestmblen3,"Testmblen3");
_LIT(KTestmblen4,"Testmblen4");
_LIT(KTestwctomb0,"Testwctomb0");
_LIT(KTestwctomb1,"Testwctomb1");
_LIT(KTestwctomb2,"Testwctomb2");
_LIT(KTestwctomb3,"Testwctomb3");
_LIT(KTestmbtowc0,"Testmbtowc0");
_LIT(KTestmbtowc1,"Testmbtowc1");
_LIT(KTestmbstowcs,"Testmbstowcs");
_LIT(KTestwcstombs,"Testwcstombs");
_LIT(KTestwcrtomb0,"Testwcrtomb0");
_LIT(KTestwcrtomb1,"Testwcrtomb1");
_LIT(KTestwcrtomb_NULL,"Testwcrtomb_NULL");
_LIT(KTestwcrtomb_Neg1,"Testwcrtomb_Neg1");
_LIT(KTestwcrtomb_Neg2,"Testwcrtomb_Neg2");
_LIT(KTestwcscoll,"Testwcscoll");
_LIT(KTestwcsicoll,"Testwcsicoll");
_LIT(KTestwcslwr,"Testwcslwr");
_LIT(KTestwcsupr,"Testwcsupr");
_LIT(KTestwctrans,"Testwctrans");
_LIT(KTestwctrans_Neg,"Testwctrans_Neg");
_LIT(KTestwctype,"Testwctype");
_LIT(KTestwctype_Neg,"Testwctype_Neg");
_LIT(KTestwcstod ,"Testwcstod");
_LIT(KTestwcstod_Neg ,"Testwcstod_Neg");
_LIT(KTestwctob,"Testwctob");
_LIT(KTestwctob_Neg,"Testwctob_Neg");
_LIT(KTestwcswidth,"Testwcswidth");
_LIT(KTestwcswidth_Neg,"Testwcswidth_Neg");
_LIT(KTestwcwidth,"Testwcwidth");
_LIT(KTestmbrlen0,"Testmbrlen0");
_LIT(KTestmbrlen1,"Testmbrlen1");
_LIT(KTestmbrtowc,"Testmbrtowc");
_LIT(KTestmbrtowc_Neg,"Testmbrtowc_Neg");
_LIT(KTestmbsrtowcs,"Testmbsrtowcs");
_LIT(KTestmbsrtowcs1,"Testmbsrtowcs1");
_LIT(KTestmbsinit,"Testmbsinit");
_LIT(KTestwcsrtombs,"Testwcsrtombs");
_LIT(KTestsetlocale,"Testsetlocale");
_LIT(Ksynclocale,"synclocale");
_LIT(Ksynclocalehybrid,	"synclocalehybrid");
_LIT(Kstrcoll_locale,"strcoll_locale");
_LIT(Kstrcoll_synclocale,"strcoll_synclocale");
_LIT(Kclocale,"clocale");


_LIT(Ktestnewlocale,"testnewlocale");





class CTestLocale : public CTestStep
	{
public:
	~CTestLocale(); 
	CTestLocale(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	TInt SetCustomLocale(TDesC& aDllName);
	TInt getlocale(TLocaleAspect aspect);
	TInt comparelang();
	TInt compare();
private:
		
		TLibraryFunction data[NumLocaleExports];
#ifdef  SYMBIAN_DISTINCT_LOCALE_MODEL
		TLibraryFunction data_reg[NumLocaleExports];
#endif
		TInt ReadStringAndSetlocale();
		TBool CompareString_WcharChar(wchar_t *aString1, char *aString2);
		TVerdict TestctypeApi();
		TVerdict Testisascii();
		TVerdict Testtoascii();
		
		//Multibyte chars
		TVerdict Testmblen0();
		TVerdict Testmblen1();
		TVerdict Testmblen2();
		TVerdict Testmblen3();
		TVerdict Testmblen4();
		
		TVerdict Testwctomb0();
		TVerdict Testwctomb1();
		TVerdict Testwctomb2();
		TVerdict Testwctomb3();
		TVerdict Testmbtowc0();
		TVerdict Testmbtowc1();
		TVerdict Testmbstowcs();
		TVerdict Testwcstombs();
		TVerdict Testwcrtomb0();
		TVerdict Testwcrtomb1();
		TVerdict Testwcrtomb_NULL();
		TVerdict Testwcrtomb_Neg1();
		TVerdict Testwcrtomb_Neg2();
		TVerdict Testwcscoll();
		TVerdict Testwcsicoll();
		TVerdict Testwcslwr();
		TVerdict Testwcsupr();
		TVerdict Testwctrans();
		TVerdict Testwctrans_Neg();
		TVerdict Testwctype();
		TVerdict Testwctype_Neg();
		TVerdict Testwcstod();
		TVerdict Testwcstod_Neg();
		TVerdict Testwctob();
		TVerdict Testwctob_Neg();
		TVerdict Testwcswidth();
		TVerdict Testwcswidth_Neg();
		TVerdict Testwcwidth();
		TVerdict Testmbrlen0();
		TVerdict Testmbrlen1();
		TVerdict Testmbrtowc();
		TVerdict Testmbrtowc_Neg();
		TVerdict Testmbsrtowcs();
		TVerdict Testmbsrtowcs1();
		TVerdict Testmbsinit();
		TVerdict Testwcsrtombs();
		TVerdict Testsetlocale();
		TVerdict synclocale();
    	TVerdict synclocalehybrid();
    	TVerdict strcoll_locale();
    	TVerdict strcoll_synclocale();
    	TVerdict clocale();
    	TVerdict testnewlocale();
			
	};
#endif 
