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
 @file UDPStep.h
*/
#if (!defined __UDP_STEP_H__)
#define __UDP_STEP_H__
#include <test/testexecutestepbase.h>
#if !(defined TEF_LITE)
#include <in_sock.h>
#include <es_sock.h>
#endif
#include "ipsuitestepbase.h"

// __EDIT_ME__ - Create your own test step definition
class CUDPStep : public CIPSuiteStepBase
	{
public:
	CUDPStep();
	~CUDPStep();
	virtual TVerdict doTestStepL();
private:
	};
// EDIT_ME__ - Use your own class step name
_LIT(KDemoUDPStep,"DemoUDPStep");

#endif
