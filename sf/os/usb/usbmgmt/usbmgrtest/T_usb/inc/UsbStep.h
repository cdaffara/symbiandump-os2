/*
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
* This contains CTestStepUsb which is the base class for all 
* the Usb test step cases
*
*/

#ifndef __USBSTEP_H__
#define __USBSTEP_H__

#include <networking/teststep.h>
#include <usbman.h>

class CTestSuite;
class CTestSuiteUsb;
NONSHARABLE_CLASS(CTestStepUsb) : public CTestStep
{
public:
	CTestStepUsb();
	virtual ~CTestStepUsb();

	// pointer to suite which owns this test 
	CTestSuiteUsb* iUsbSuite;

protected:
	void OpenCommServerL();
	TInt OpenLdd();
};



#endif /* __USBSTEP_H__ */
