// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "consoleprint.h"

//
// static factory method
//
CConsolePrint* CConsolePrint::NewL(TBool aWaitForAck)
	{
	CConsolePrint* self = new(ELeave) CConsolePrint(aWaitForAck);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

//
// Constructor
//
CConsolePrint::CConsolePrint(TBool aWaitForAck)
	: iWaitForAck(aWaitForAck)
	{
	}

//
// Two phase construct method
//
void CConsolePrint::ConstructL()
	{
	iConsole = Console::NewL(KNullDesC, TSize(KConsFullScreen, KConsFullScreen));
	}

CConsolePrint::~CConsolePrint()
	{
	delete iConsole;
	}

// setter
void CConsolePrint::SetWaitMode(TBool aWaitForAck)
	{
	if (iWaitForAck != aWaitForAck)
		{
		iWaitForAck = aWaitForAck;
		}
	}

// dummy class to toss away overflow
class TTruncateOverflow16 : public TDes16Overflow
    {
public:
    virtual void Overflow(TDes&) { }
	};

//
// Output a message to console
void CConsolePrint::Printf(TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST vaList;
	VA_START(vaList, aFmt);

	TTruncateOverflow16 overflow;
	iBuf.Zero();
	iBuf.AppendFormatList(aFmt, vaList, &overflow);
	iConsole->Write(iBuf);
	if (iWaitForAck)
		{
		_LIT(KPressAKey, "\r\n[press a key to continue...]\r\n");
		iConsole->Write(KPressAKey);
		WaitForUserAck();
		}
	}

// Wait for either a hit stroke or timeout.
void CConsolePrint::WaitForUserAck()
	{
	TRequestStatus timerStatus, consoleStatus;
	RTimer timer;
	timer.CreateLocal();
	TTimeIntervalMicroSeconds32 interval = KTimeToWait;

	iConsole->Read(consoleStatus);
	timer.After(timerStatus, interval);
	User::WaitForRequest(consoleStatus, timerStatus);

	if (consoleStatus == KRequestPending)
		{
		iConsole->ReadCancel();
		}
	else
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		}
	}
