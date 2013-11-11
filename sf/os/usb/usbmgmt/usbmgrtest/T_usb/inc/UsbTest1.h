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
* This is the header file for Usb test section 1
*
*/

#ifndef __USBTEST1_H__
#define __USBTEST1_H__


///////////////////////////////////////////////
// Test 1.1
NONSHARABLE_CLASS(CTestStartUsb) : public CTestStepUsb
{
public:
	CTestStartUsb();
	virtual ~CTestStartUsb();

	virtual enum TVerdict doTestStepL( void );

private:
};


///////////////////////////////////////////////
// Test 1.2
NONSHARABLE_CLASS(CTestWaitForUsb) : public CTestStepUsb
{
public:
	CTestWaitForUsb();
	virtual ~CTestWaitForUsb();

	virtual enum TVerdict doTestStepL( void );

private:
};


///////////////////////////////////////////////
// Test 1.3
NONSHARABLE_CLASS(CTestStartCsy) : public CTestStepUsb
{
public:
	CTestStartCsy();
	virtual ~CTestStartCsy();

	virtual enum TVerdict doTestStepL( void );

private:
};


///////////////////////////////////////////////
// Test 1.4
NONSHARABLE_CLASS(CTestOpenDTEPort) : public CTestStepUsb
{
public:
	CTestOpenDTEPort();
	~CTestOpenDTEPort();

	virtual enum TVerdict doTestStepL( void );

private:
};


///////////////////////////////////////////////
// Test 1.5
NONSHARABLE_CLASS(CTestOpenDCEPort) : public CTestStepUsb
{
public:
	CTestOpenDCEPort();
	virtual ~CTestOpenDCEPort();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.6
NONSHARABLE_CLASS(CTestPostRead) : public CTestStepUsb
{
public:
	CTestPostRead();
	virtual ~CTestPostRead();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.7
NONSHARABLE_CLASS(CTestPostWrite) : public CTestStepUsb
{
public:
	CTestPostWrite();
	virtual ~CTestPostWrite();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.8
NONSHARABLE_CLASS(CTestCancelRead) : public CTestStepUsb
{
public:
	CTestCancelRead();
	virtual ~CTestCancelRead();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.9
NONSHARABLE_CLASS(CTestCancelWrite) : public CTestStepUsb
{
public:
	CTestCancelWrite();
	virtual ~CTestCancelWrite();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.10
NONSHARABLE_CLASS(CTestWaitForRead) : public CTestStepUsb
{
public:
	CTestWaitForRead();
	virtual ~CTestWaitForRead();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.11
NONSHARABLE_CLASS(CTestWaitForWrite) : public CTestStepUsb
{
public:
	CTestWaitForWrite();
	virtual ~CTestWaitForWrite();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.12
NONSHARABLE_CLASS(CTestPostReadOneOrMore) : public CTestStepUsb
{
public:
	CTestPostReadOneOrMore();
	virtual ~CTestPostReadOneOrMore();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.13
NONSHARABLE_CLASS(CTestClosePort) : public CTestStepUsb
{
public:
	CTestClosePort();
	virtual ~CTestClosePort();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.14
NONSHARABLE_CLASS(CTestCloseCommServer) : public CTestStepUsb
{
public:
	CTestCloseCommServer();
	virtual ~CTestCloseCommServer();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.15
NONSHARABLE_CLASS(CTestCloseUsbServer) : public CTestStepUsb
{
public:
	CTestCloseUsbServer();
	virtual ~CTestCloseUsbServer();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.16
NONSHARABLE_CLASS(CTestStopUsb) : public CTestStepUsb
{
public:
	CTestStopUsb();
	virtual ~CTestStopUsb();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.17
NONSHARABLE_CLASS(CTestNotifySignalChange) : public CTestStepUsb
{
public:
	CTestNotifySignalChange();
	virtual ~CTestNotifySignalChange();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.18
NONSHARABLE_CLASS(CTestWaitForSignalChange) : public CTestStepUsb
{
public:
	CTestWaitForSignalChange();
	virtual ~CTestWaitForSignalChange();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.19
NONSHARABLE_CLASS(CTestWaitForReadCancel) : public CTestStepUsb
{
public:
	CTestWaitForReadCancel();
	virtual ~CTestWaitForReadCancel();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.20
NONSHARABLE_CLASS(CTestPostReadThenCancel) : public CTestStepUsb
{
public:
	CTestPostReadThenCancel();
	virtual ~CTestPostReadThenCancel();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.21
NONSHARABLE_CLASS(CTestNotifyConfigChange) : public CTestStepUsb
{
public:
	CTestNotifyConfigChange();
	virtual ~CTestNotifyConfigChange();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.23
NONSHARABLE_CLASS(CTestNotifyBreak) : public CTestStepUsb
{
public:
	CTestNotifyBreak();
	virtual ~CTestNotifyBreak();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.24
NONSHARABLE_CLASS(CTestWaitForBreak) : public CTestStepUsb
{
public:
	CTestWaitForBreak();
	virtual ~CTestWaitForBreak();

	virtual enum TVerdict doTestStepL( void );

private:
};

///////////////////////////////////////////////
// Test 1.25
NONSHARABLE_CLASS(CTestCloseAll) : public CTestStepUsb
{
public:
	CTestCloseAll();
	virtual ~CTestCloseAll();

	virtual enum TVerdict doTestStepL( void );

private:
};

#endif /* __USBTEST1_H__ */
