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
// Simple implementation of stdin/stdout/stderr class which uses a console, created on demand.
// 
//

#include "FDESC.H"
#include <e32cons.h>	// for CConsoleBase
#include <sys/ioctl.h>

void CTtyDesc::CheckConsoleCreated()
	{
	if (iConsole==NULL)
		{
		TRAPD(r,iConsole=Console::NewL(_L("STDOUT"),TSize(KConsFullScreen,KConsFullScreen)))
		__ASSERT_ALWAYS(r==KErrNone,User::Panic(_L("ESTLIB Console"),0));
		}
	}

CTtyDesc::~CTtyDesc()
	{
	CConsoleBase *it = iConsole;
	iConsole = 0;
	delete it;
	}

TInt CTtyDesc::FinalClose()
	{
	CConsoleBase *it = iConsole;
	iConsole = 0;
	if (it)
		{
		// it->SetTitle(_L("Console closed - press any key"));
		it->Printf(_L("Console closed - press any key"));
		it->Getch();
		delete it;
		}
	return KErrNone;
	}

void CTtyDesc::Read(TDes8& /*aDesc*/, TRequestStatus& aStatus)
	{
	CheckConsoleCreated();
	// See implemention of Getch() in E32\UBAS\UB_CONS.CPP
	iConsole->Read(aStatus);
	}

void CTtyDesc::ReadCancel()
	{
	if (iConsole)
		iConsole->ReadCancel();
	}

TInt CTtyDesc::ReadCompletion(TDes8& aDesc, TInt aStatus)
	{
	MapCodeAndEcho(aDesc, iConsole->KeyCode());
	return aStatus;
	}

void CTtyDesc::Write(TDes8 &aDesc, TRequestStatus& aStatus)
	{
	Write(aDesc);
	Complete(aStatus, KErrNone);
	}

#if !defined(_UNICODE)
void CTtyDesc::Write(TDes8 &aDesc)
	{
	CheckConsoleCreated();
	iConsole->Write(aDesc);
	}
#else
void CTtyDesc::Write(TDes8 &aDesc)
	{
	CheckConsoleCreated();

	TInt remaining = aDesc.Length();
	const TText8* cp = aDesc.Ptr();
	while (remaining)
	    {
	    TBuf16<256> buffer;
	    TInt len = Min(remaining, 256);
	    buffer.Copy(TPtrC8(cp, len));

	    iConsole->Write(buffer);

	    cp += len;
	    remaining -= len;
	    }
	}
#endif

void CTtyDesc::MapCodeAndEcho(TDes8& aDesc, TKeyCode aCode)
	{
	TText8 ch;

	aDesc.Zero();
	switch (aCode)
	{
	case EKeyPrintScreen:
		return;		// unknowable keycodes are ignored

	case EKeyEnter:
		ch='\n'; break;
	case EKeyBackspace:
		ch=0x08; break;
	default:
		ch=(TText8)(aCode&0xff);	// who knows - it's not documented
		break;
	}
	aDesc.Append(ch);
	// Could suppress echoing at this point
	Write(aDesc);
	return;
	}

void CTtyDesc::Ioctl(int /*aCmd*/, void* /*aParam*/, TRequestStatus& aStatus)
	{
	// bodge for now - this will become more complicated if we develop a better
	// terminal device implementation
	Complete(aStatus,KErrNone);
	}

TInt CTtyDesc::IoctlCompletion(int aCmd, void* aParam, TInt aStatus)
	{
	TInt ret=aStatus;
	if (ret!=KErrNone)
		return ret;
	int *param=REINTERPRET_CAST(int*,aParam);
	switch (aCmd)
		{
	case E32IONREAD:
		*param=1;	// claim that there is always data available
		break;
	case E32IOSELECT:
		*param=(*param)&(E32SELECT_READ|E32SELECT_WRITE);
		break;
	default:
		ret=KErrNotSupported;
		break;
		}
	return ret;
	}

