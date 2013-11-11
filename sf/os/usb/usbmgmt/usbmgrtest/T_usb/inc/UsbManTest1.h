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
* This is the header file for Usbman test section 1
*
*/

#ifndef __USBMANTEST1_H__
#define __USBMANTEST1_H__

///////////////////////////////////////////////
// Test 1.1
class CTestUsbManNotifyStateChange : public CTestStepUsb
{
public:
	CTestUsbManNotifyStateChange();
	virtual ~CTestUsbManNotifyStateChange();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.2
class CTestUsbManWaitForStateChange : public CTestStepUsb
{
public:
	CTestUsbManWaitForStateChange();
	virtual ~CTestUsbManWaitForStateChange();

	virtual enum TVerdict doTestStepL( void );

private:
};


#endif
