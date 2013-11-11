// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



/**
 @file te_check_record_content_step.h
 @internalTechnology
*/

#ifndef TE_CHECK_RECORD_CONTENT_STEP_H_
#define TE_CHECK_RECORD_CONTENT_STEP_H_
#include <test/testexecutestepbase.h>
#include "te_suite_step_base.h"
#include "te_parser.h"


class CCheckRecordContentStep : public CTe_UTraceSuiteStepBase
	{
public:
	CCheckRecordContentStep();
	~CCheckRecordContentStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
//	virtual TVerdict doTestStepPostambleL();
private:
	TInt CompareTraces();
	TInt WriteTracesL();
	TInt ReadTraces();
	TTraceConfigs* CreateTrace(TTraceApiUsed aApi);
	TTraceParser	iParser;
	
	RPointerArray<TTraceConfigs> iLoggedTraces;
	RPointerArray<TTraceConfigs> iSentTraces;
	};

_LIT(KCheckRecordContentStep,"CCheckRecordContentStep");

#endif /*TE_CHECK_RECORD_CONTENT_STEP_H_*/
