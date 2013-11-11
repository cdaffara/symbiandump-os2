/**
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file 
*/

#include <test/testexecutestepbase.h>
#include <usbman.h>
#include "T_UsbManagerServer.h"
#include "CUsbTestStepBase.h"

NONSHARABLE_CLASS(CUsbComponentTest) :public CUsbTestStepBase
{
public:
	virtual TVerdict CompareUsbPersonalityL();
	
};

_LIT(KUsbLoadPersonalityNormal,"CUsbLoadPersonalityNormal");
NONSHARABLE_CLASS(CUsbLoadPersonalityNormal) : public CUsbComponentTest
	{
public:
	CUsbLoadPersonalityNormal();
	virtual TVerdict doTestStepL();
	};

_LIT(KUsbLoadPersonalityAbNormal,"CUsbLoadPersonalityAbNormal");
NONSHARABLE_CLASS(CUsbLoadPersonalityAbNormal) : public CUsbComponentTest
	{
public:
	CUsbLoadPersonalityAbNormal();
	virtual TVerdict doTestStepL();
	};
	
_LIT(KUsbLoadPersonalityMissing,"CUsbLoadPersonalityMissing");
NONSHARABLE_CLASS(CUsbLoadPersonalityMissing) : public CUsbComponentTest
	{
public:
	CUsbLoadPersonalityMissing();
	virtual TVerdict doTestStepL();
	};
	
_LIT(KUsbTestOOM,"CUsbTestOOM");
NONSHARABLE_CLASS(CUsbTestOOM) : public CUsbComponentTest
	{
public:
	CUsbTestOOM();
	virtual TVerdict doTestStepL();
	};

_LIT(KUsbSwitchPersonalityNormal,"CUsbSwitchPersonalityNormal");
NONSHARABLE_CLASS(CUsbSwitchPersonalityNormal) : public CUsbComponentTest
	{
public:
	CUsbSwitchPersonalityNormal();
	virtual TVerdict doTestStepL();
	};

_LIT(KUsbSwitchPersonalityAbNormal,"CUsbSwitchPersonalityAbNormal");
NONSHARABLE_CLASS(CUsbSwitchPersonalityAbNormal) : public CUsbComponentTest
	{
public:
	CUsbSwitchPersonalityAbNormal();
	virtual TVerdict doTestStepL();
	};
	
_LIT(KUsbStartStopPersonality1,"CUsbStartStopPersonality1");
NONSHARABLE_CLASS(CUsbStartStopPersonality1) : public CUsbComponentTest
	{
public:
	CUsbStartStopPersonality1();
	virtual TVerdict doTestStepL();
	};

_LIT(KUsbStartStopPersonality2,"CUsbStartStopPersonality2");
NONSHARABLE_CLASS(CUsbStartStopPersonality2) : public CUsbComponentTest
	{
public:
	CUsbStartStopPersonality2();
	virtual TVerdict doTestStepL();
	};
	
_LIT(KUsbStartStopPersonality3,"CUsbStartStopPersonality3");
NONSHARABLE_CLASS(CUsbStartStopPersonality3) : public CUsbComponentTest
	{
public:
	CUsbStartStopPersonality3();
	virtual TVerdict doTestStepL();
	};

_LIT(KMemAllocationFailure,"CMemAllocationFailure");
NONSHARABLE_CLASS(CMemAllocationFailure) : public CUsbComponentTest
	{
public:
	CMemAllocationFailure();
	virtual TVerdict doTestStepL();
	};
	
