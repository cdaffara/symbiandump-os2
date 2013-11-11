/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file IPSuiteStepBase.h
*/

#if (!defined __IP_SUITE_STEP_BASE__)
#define __IP_SUITE_STEP_BASE__
#include <test/testexecutestepbase.h>
#if !(defined TEF_LITE)
#include <in_sock.h>
#include <es_sock.h>
#endif
class CIPSuiteStepBase : public CTestStep
	{
public:
	virtual ~CIPSuiteStepBase();
	CIPSuiteStepBase();
	virtual TVerdict doTestStepPreambleL();
protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	#if !(defined TEF_LITE)
	RSocketServ iServer;
	TInetAddr	iDestAddr;
	#endif
	};

#endif
