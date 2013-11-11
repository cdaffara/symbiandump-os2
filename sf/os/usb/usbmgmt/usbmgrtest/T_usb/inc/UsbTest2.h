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
* This is the header file for Usb test section 2
*
*/

#ifndef __USBTEST2_H__
#define __USBTEST2_H__

///////////////////////////////////////////////
// Test 2.1
class CTestSetSignalsToMark : public CTestStepUsb
{
public:
	CTestSetSignalsToMark();
	virtual ~CTestSetSignalsToMark();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 2.2
class CTestSetSignalsToSpace : public CTestStepUsb
{
public:
	CTestSetSignalsToSpace();
	virtual ~CTestSetSignalsToSpace();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 2.3
class CTestCheckSignals : public CTestStepUsb
{
public:
	CTestCheckSignals();
	virtual ~CTestCheckSignals();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 2.4
class CTestWaitForReadFailure : public CTestStepUsb
{
public:
	CTestWaitForReadFailure();
	virtual ~CTestWaitForReadFailure();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 2.5
class CTestWaitForWriteFailure : public CTestStepUsb
{
public:
	CTestWaitForWriteFailure();
	virtual ~CTestWaitForWriteFailure();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 2.6
class CTestOpenPort : public CTestStepUsb
{
public:
	CTestOpenPort();
	virtual ~CTestOpenPort();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 2.7
class CTestWaitForConfigChange : public CTestStepUsb
{
public:
	CTestWaitForConfigChange();
	virtual ~CTestWaitForConfigChange();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 2.8
class CTestNotifyStateChange : public CTestStepUsb
{
public:
	CTestNotifyStateChange();
	virtual ~CTestNotifyStateChange();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 2.9
class CTestWaitForStateChange : public CTestStepUsb
{
public:
	CTestWaitForStateChange();
	virtual ~CTestWaitForStateChange();

	virtual enum TVerdict doTestStepL( void );

private:
};

#endif
