/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32cons.h>	// for CConsoleBase
#include <sys/ioctl.h>
#include "fdesc.h"

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
		{
		iConsole->ReadCancel();
		}
	}

TInt CTtyDesc::ReadCompletion(TDes8& aDesc, TInt/*aStatus*/)
	{
	iReadingData = ETrue;
	MapCodeAndEcho(aDesc, iConsole->KeyCode());
	iReadingData = EFalse;
	return aDesc.Length();
	}

void CTtyDesc::Write(TDes8 &aDesc, TRequestStatus& aStatus)
	{
	Write(aDesc);
	Complete(aStatus, aDesc.Length());
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
	//If its a back-space character
	if (aDesc.Compare(_L8("\b")) == 0)
		{
		//Make sure that it wont delete non-modifiable strings
		if( iCurPosX == iConsole->WhereX() && iCurPosY == iConsole->WhereY())
			{
			return;
			}
		}

	TInt remaining = aDesc.Length();
	const TText8* cp = aDesc.Ptr();
	while (remaining)
	    {
	    TBuf16<256> tbc;
	    TInt len = Min(remaining, 256);
	    tbc.Copy(TPtrC8(cp, len));

	    iConsole->Write(tbc);

	    cp += len;
	    remaining -= len;
	    }
	    
	if (aDesc.Compare(_L8("\b")) == 0)
		{
		iConsole->ClearToEndOfLine();
		}
		    
	//Get the cursor position if its non-modifiable data or newline
	if (!iReadingData)
		{
		iCurPosX = iConsole->WhereX();
		iCurPosY = iConsole->WhereY();
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
		return;		// ignore this keycode

	case EKeyEnter:
		iReadingData = EFalse;
		ch='\n';
		break;
	case EKeyBackspace:
		ch=0x08;
		break;
	default:
		ch=(TText8)(aCode&0xff);	// who knows - it's not documented
		break;
		}
	aDesc.Append(ch);
	// Could suppress echoing at this point
	if(iEchoVal == EOn)
		{
		Write(aDesc);	
		}
	else if( (iEchoVal >= EPrintValid) && (iEchoVal < EPrintInvalid))	
		{
		switch(aCode)
			{
			case EKeyBackspace:
				//erase the previous character from the display.
				Write(aDesc);
				break;
			case EKeyEnter:
				//ignore
				break;
			default:
				//for every other input character, display the configured one.
				iConsole->Printf(_L("%c"), iEchoVal);
				break;	
			}
		}
	return;
	}

void CTtyDesc::Ioctl(int /*aCmd*/, void* /*aParam*/, TRequestStatus& aStatus)
	{
	// bodge for now - this will become more complicated if we develop a better
	// terminal device implementation
	Complete(aStatus, KErrNone);
	}

TInt CTtyDesc::IoctlCompletion(int aCmd, void* aParam, TInt aStatus)
	{
	TInt ret = aStatus;
	if (ret != KErrNone)
		{
		return ret;
		}
		
	int *param = reinterpret_cast<int*> (aParam);
	switch ((unsigned)aCmd)
		{
	case FIONREAD:
	case E32IONREAD:
		*param = 1;	// claim that there is always data available
		break;
	case E32IOSELECT:
		*param = (*param)&(E32SELECT_READ|E32SELECT_WRITE);
		break;
	default:
		ret = KErrNotSupported;
		break;
		}
	return ret;
	}
	
//-----------------------------------------------------------------------------------
//int CTtyDesc::SetEcho(TBool aEcho)
//
//Sets the echo flag to the given value.
//-----------------------------------------------------------------------------------

TInt CTtyDesc::SetEcho(TUint8 aEcho)
	{
	if( (aEcho > EOn && aEcho < EPrintValid) || (aEcho >= EPrintInvalid) )
		return KErrArgument;
	
	iEchoVal=aEcho;
	return KErrNone;
	}

