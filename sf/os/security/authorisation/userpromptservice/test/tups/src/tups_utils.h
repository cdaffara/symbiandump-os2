/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
 @file  
 @test
*/


#if (!defined UTILSTEP_H)
#define UTILSTEP_H
#include <test/testexecutestepbase.h>

#include <ups/upsclient.h>

class CCloseUpsStep: public CTestStep
	{
public:
	CCloseUpsStep();
	virtual ~CCloseUpsStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	};

_LIT(KCloseUps,"UtilsCloseUps");


#endif
