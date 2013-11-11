/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file Te_Outfrwk_TestStep.h
*/
#ifndef T_OUTFRWK_TESTSTEP_H
#define T_OUTFRWK_TESTSTEP_H

#include <test/testexecutestepbase.h>
#include <e32base.h> // CBase
#include <e32test.h>
#include <f32file.h> // RFs, RFile
#include "outputframework.h"	// COutputFramework
#include "uloggersysconfig.h" 	// CConfig, CConfigSettingsIter
#include <uloggeroutputplugin.h>
using namespace Ulogger;

const TBool addTimeStamp = ETrue;

//Test Outputframework of ulogger
class CUloggerOutfrwkTest: public CTestStep
	{
public:
	virtual ~CUloggerOutfrwkTest();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL()=0;
public:	
	Ulogger::COutputFramework* iOutfrwk;
	CPlugin *iPlugin;
	};



class CTextMessageTest: public CUloggerOutfrwkTest
{
	public:
		virtual TVerdict doTestStepL();
};


_LIT(KTextMessageTeststep,"TextMessageTeststep");	

#endif
