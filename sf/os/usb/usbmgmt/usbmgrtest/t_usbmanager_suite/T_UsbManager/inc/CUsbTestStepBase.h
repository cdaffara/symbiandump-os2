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
#if (!defined __CUSBTESTSTEPBASE_H__)
#define __CUSBTESTSTEPBASE_H__
#include <test/testexecutestepbase.h>
#include <usbman.h>
#include "T_UsbManagerServer.h"

NONSHARABLE_CLASS(CUsbTestStepBase) : public CTestStep
	{
public:
	CUsbTestStepBase();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
protected:
	RUsb iUsb;
	TBool iDoNotConnect;
	TBool iConnected;
	TBool iStopService;

protected:
	TInt CheckState(RUsb sess, TUsbServiceState aState);
	TInt SetIdle(RUsb sess);
	TInt SetStarted(RUsb sess);
	TInt SetStarting(RUsb sess);
	TInt SetStopping(RUsb sess);
	};

#endif
