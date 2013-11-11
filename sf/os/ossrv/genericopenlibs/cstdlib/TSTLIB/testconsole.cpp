// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Console redirection server 
// 
//

#include "testconsole.h"
#include <e32svr.h>
#include <utf.h>
#include <e32cons.h>


NONSHARABLE_CLASS(CActiveConsole) : public CActive
	{
	public:
		CActiveConsole();
		void ConsoleRead();
		static CActiveConsole* NewLC(CConsoleBase* aConsole, CTestConsole * aStream);
		static CActiveConsole* NewL(CConsoleBase* aConsole, CTestConsole * aStream);
		void ConstructL(CConsoleBase* aConsole, CTestConsole * aStream);

	private:
		void DoCancel();
		void RunL();
		CTestConsole * iStream;
		CConsoleBase * iConsole;
	};

CActiveConsole::CActiveConsole() : CActive(CActive::EPriorityStandard)
	{
	}

void CActiveConsole::DoCancel()
	{
	}


void CActiveConsole::ConsoleRead()
	{
	iConsole->Read(iStatus);
	SetActive();
	}

void CActiveConsole::RunL()
	{
	iStream->DoneReading(iConsole->KeyCode());
	}




CActiveConsole* CActiveConsole::NewLC(CConsoleBase* aConsole, CTestConsole * aStream)
	{
	CActiveConsole* self=new (ELeave) CActiveConsole;
	CleanupStack::PushL(self);
	self->ConstructL(aConsole, aStream);
	return self;
	}

CActiveConsole* CActiveConsole::NewL(CConsoleBase* aConsole, CTestConsole * aStream)
	{
	CActiveConsole* self = NewLC(aConsole, aStream);
	CleanupStack::Pop();
	return self;
	}

void CActiveConsole::ConstructL(CConsoleBase* aConsole, CTestConsole * aStream)
	{
	iConsole = aConsole;
	iStream = aStream;
	CActiveScheduler::Add(this);
	}


CTestConsole::CTestConsole()
	{
#ifdef __MARM
	TRAPD(r,iConsole=Console::NewL(_L("Redirection Server"),TSize(KConsFullScreen, KConsFullScreen)))
#else
	TRAPD(r,iConsole=Console::NewL(_L("Redirection Server"),TSize(KDefaultConsWidth,KDefaultConsHeight)))
#endif
	__ASSERT_ALWAYS(r==KErrNone,User::Panic(_L("Redirection Server"),0));

	//create and add the active object
	TRAPD(x, iActiveChap=CActiveConsole::NewL(iConsole, this));
	__ASSERT_ALWAYS(x==KErrNone,User::Panic(_L("Redirection Server"),0));

	}


CTestConsole::~CTestConsole()
	{
	if (iConsole)
		{
		iConsole->Write(_L("Console Closed"));
		User::After(2*1000000);
		delete iConsole;
		}
	}


 void CTestConsole::Write(TDes8& aDes)
	{

	TBuf<256> str;
	CnvUtfConverter::ConvertToUnicodeFromUtf8(str, aDes);

	iConsole->Write(str);
	
	}

 void CTestConsole::Read(const RMessage2& aMessage)
	{
	iMessage = aMessage;
	iActiveChap->ConsoleRead();		//read from the console
	}


 void CTestConsole::DoneReading(TKeyCode aKeyCode)
	{
	TText8 ch;
	switch (aKeyCode)
		{
		case EKeyEnter:
			ch = '\n';
			break;
		case EKeyBackspace:
			ch = 0x08;
			break;
		default:
			ch = (TText8)(aKeyCode&0xFF);
			break;
		}
	
	//append it to the buffer
	iBuf.Append(ch);
	Write(iBuf);

	CompleteRead(iMessage);
	}

 CStreamBase2* CTestConsoleFactory::GetStream()
	{
	return new CTestConsole();
	}

 void CTestConsoleFactory::Configure()
	{
	}


CTestConsoleFactory* CTestConsoleFactory::NewL()
	{
	CTestConsoleFactory* f=new(ELeave) CTestConsoleFactory();

	return f;
	}


