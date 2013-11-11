// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#if (!defined __TE_HEADERSTESTSTEP_H__)
#define __TE_HEADERSTESTSTEP_H__

#include <e32std.h>
#include <TestExecuteStepBase.h>

_LIT(KCHeadersTest, "CXmlDomHeadersTestCase");
class CXmlDomHeadersTestStep : public CTestStep
	{
	public:
	inline CXmlDomHeadersTestStep(){SetTestStepName(KCHeadersTest);};
	virtual TVerdict doTestStepL(void);
	
	};

#endif
