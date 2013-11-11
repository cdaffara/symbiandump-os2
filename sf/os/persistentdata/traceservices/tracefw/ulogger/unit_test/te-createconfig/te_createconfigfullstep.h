/**
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



/**
 @file te_createconfigfullstep.h
 @internalTechnology
*/

#ifndef TE_CREATECONFIGFULLSTEP_H
#define TE_CREATECONFIGFULLSTEP_H

#include <test/testexecutestepbase.h>

//Test Sysconfig of Ulogger
class CCreateConfigFUllTestStep : public CTestStep
	{
public:
	CCreateConfigFUllTestStep();
	~CCreateConfigFUllTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:	
	
	};
	
_LIT(KCreateConfigFullStep,"CreateConfigFullTestStep");		

#endif
