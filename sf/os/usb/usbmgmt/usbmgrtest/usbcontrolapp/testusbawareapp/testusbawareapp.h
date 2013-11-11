/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TESTUSBAWAREAPP_H
#define TESTUSBAWAREAPP_H

#include <e32cmn.h>
#include <e32base.h>
#include <usbman.h>
#include <e32property.h>

_LIT(KUsbAwareAppTitle, "USB Aware App");

class CUsbAwareAppKeys;
class CShutdownMonitor;

NONSHARABLE_CLASS(MShutdownInterface)
	{
public:
	virtual void Stop() const = 0;
	};

NONSHARABLE_CLASS(CUsbAwareAppConsole) : public CBase, public MShutdownInterface
	{
friend class CUsbAwareAppKeys;

public:
	static CUsbAwareAppConsole* NewLC();
	~CUsbAwareAppConsole();

public:
	void Start();     
	void Stop() const; 

private:
	CUsbAwareAppConsole();
	void ConstructL();
	void Move(TInt aX, TInt aY);
	void Draw();

private:
	CConsoleBase*		iConsole;
	CUsbAwareAppKeys*	iKeys;
	CShutdownMonitor*	iShutdownMonitor;

private:
	static const TInt KNumLinesInWindow		= 5;
	};


NONSHARABLE_CLASS(CUsbAwareAppKeys) : public CActive
	{
public:
	static CUsbAwareAppKeys* NewL(CUsbAwareAppConsole& aUsbAwareAppConsole);
	~CUsbAwareAppKeys();
	
private:
	CUsbAwareAppKeys(CUsbAwareAppConsole& aUsbAwareAppConsole);
	void ConstructL();
	void DoCancel();
	void RunL();
	
private:
	CUsbAwareAppConsole&	iUsbAwareAppConsole;
	RUsb					iUsb;
	};


NONSHARABLE_CLASS(CShutdownMonitor) : public CActive
	{
public:
	static CShutdownMonitor* NewL(MShutdownInterface& aUsbAwareAppConsole);
	~CShutdownMonitor();
private:
	CShutdownMonitor(MShutdownInterface& aUsbAwareAppConsole);
	void ConstructL();
	// From CActive
	void DoCancel();
	void RunL();
private:
	RProperty				iShutdownProp;
	MShutdownInterface&		iUsbAwareAppConsole;
	};

#endif // TESTUSBAWAREAPP_H
