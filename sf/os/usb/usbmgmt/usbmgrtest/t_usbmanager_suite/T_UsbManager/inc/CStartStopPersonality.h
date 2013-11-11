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
#if (!defined __CSTARTSTOPPERSONALITY_H__)
#define __CSTARTSTOPPERSONALITY_H__
#include <test/testexecutestepbase.h>
#include "CUsbTestStepBase.h"
	
_LIT(KStartStopPersonality1,"StartStopPersonality1");
NONSHARABLE_CLASS(CStartStopPersonality1) : public CUsbTestStepBase 
	{
public:
	CStartStopPersonality1();
	virtual TVerdict doTestStepL();
protected:
	TInt TryStart(RUsb aSess1, RUsb aSess2, RUsb aSess3, TInt aRet1, TInt aRet2, TInt aRet3, TInt aPersonality1, TInt aPersonality2, TUsbServiceState aState);
	TInt TryStop(RUsb aSess1, RUsb aSess2, RUsb aSess3, TInt aRet1, TInt aRet2, TInt aRet3);
	};

_LIT(KStartStopPersonality2,"StartStopPersonality2");
NONSHARABLE_CLASS(CStartStopPersonality2) : public CUsbTestStepBase 
	{
public:
	CStartStopPersonality2();
	virtual TVerdict doTestStepL();
protected:
	TInt CheckReturnCodes(TRequestStatus& aStat1, TRequestStatus& aStat2, TInt aRet1, TInt aRet2);
	};

#endif
