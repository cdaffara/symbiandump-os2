/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

NONSHARABLE_CLASS(CUsbMsComponentTest) :public CUsbTestStepBase
{
public:
	virtual TVerdict CompareUsbPersonalityL();
	
};

_LIT(KUsbMsccGetPersonality,"CUsbMsccGetPersonality");
NONSHARABLE_CLASS(CUsbMsccGetPersonality) : public CUsbMsComponentTest
	{
public:
	CUsbMsccGetPersonality();
	virtual TVerdict doTestStepL();
	};

_LIT(KUsbMsccStartStop,"CUsbMsccStartStop");
NONSHARABLE_CLASS(CUsbMsccStartStop) : public CUsbMsComponentTest
	{
public:
	CUsbMsccStartStop();
	virtual TVerdict doTestStepL();
	};
	
