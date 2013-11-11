/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32twin.h>
#include <e32svr.h>
#include "activeconsole.h"
#include "testbase.h"
#include "tests.h"

CActiveConsole::CActiveConsole(CConsoleBase& aConsole)
 :	CActive(CActive::EPriorityStandard),
	iConsole(aConsole)
	{
	CActiveScheduler::Add(this);
	}

CActiveConsole* CActiveConsole::NewLC(CConsoleBase& aConsole)
	{
	CActiveConsole* self = new(ELeave) CActiveConsole(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->DisplayMainMenu();
	return self;
	}

void CActiveConsole::ConstructL()
	{
	// Launch of FDTest test.
	iTest = gTestDefinitions[0].iFactoryL(*this);
	}

CActiveConsole::~CActiveConsole()
	{
	Cancel();

	StopCurrentTest();
	}

void CActiveConsole::DoCancel()
	{
	iConsole.ReadCancel();
	}

void CActiveConsole::StopCurrentTest()
	{
	delete iTest;
	iTest = NULL;
	}

void CActiveConsole::RunL()
// Only process when we get a return, otherwise cache- i.e. support multi-character selections
	{
	DoActionKeyL(iConsole.KeyCode());

	// Repost asynchronous request.
	RequestCharacter();
	}

void CActiveConsole::DoActionKeyL(TKeyCode aKeyCode)
	{
	WriteNoReturn(_L8("%c"), aKeyCode);

	// Examine the key that just came in.
	switch ( TUint(aKeyCode) )
		{
	case EKeyEscape:
		{
		Write(_L8("Exiting"));
		CActiveScheduler::Stop();
		return;
		}

	case EKeyEnter:
		// Tell the test about what's in the buffer so far, if anything.
		Write(_L8("You entered \'%S\'"), &iInputBuffer);
		switch ( iInputBuffer.Length() )
			{
		case 0:
			// Don't pass anything on- nothing to pass on.
			break;

		case 1:
			if ( 	iInputBuffer == _L8("S") 
				||	iInputBuffer == _L8("s") 
				)
				{
				StopCurrentTest();
				}
			else
				{
				// Tell the test via the old 'single character' interface.
				// If there is a test, then let it process the key. If there isn't a 
				// test, we process it to (possibly) create and run a new test object.
				if ( iTest )
					{
					TRAPD(err, iTest->ProcessKeyL((TKeyCode)iInputBuffer[0]));
					if ( err )
						{
						Write(_L8("CTestBase::ProcessKeyL left with %d"), err);
						StopCurrentTest();
						}
					}
				else
					{
					SelectTestL();
					}
				}
			iInputBuffer = KNullDesC8();
			break;
		
		default:
			// Tell the test via the new 'multi character' interface.
			// If there is a test, then let it process the key. If there isn't a 
			// test, we process it to (possibly) create and run a new test object.
			if ( iTest )
				{
				TRAPD(err, iTest->ProcessKeyL(iInputBuffer));
				if ( err )
					{
					Write(_L8("CTestBase::ProcessKeyL left with %d"), err);
					StopCurrentTest();
					}
				}
			else
				{
				SelectTestL();
				}
			iInputBuffer = KNullDesC8();
			break;
			}
		DisplayMainMenu();
		break;

	default:
		iInputBuffer.Append(aKeyCode);
		break;
		}
	}

void CActiveConsole::RequestCharacter()
	{
	iConsole.Read(iStatus);
	SetActive();
	}

void CActiveConsole::DisplayMainMenu()
	{
	Write(KNullDesC8);

	// If there's a current test, display its step menu. Otherwise, display 
	// all the available tests.
	if ( iTest )
		{
		iTest->DisplayTestSpecificMenu();
		Write(_L8("s - stop and close current test"));
		}
	else
		{
		const TUint numberOfTests = sizeof(gTestDefinitions) / sizeof(TTestDefinition);
		for ( TUint ii = 0 ; ii < numberOfTests ; ii ++ )
			{
			Write(_L8("%d - %S"), ii, &gTestDefinitions[ii].iDescription);
			}
		}

	Write(_L8("Escape - exit program"));
	Write(KNullDesC8);
 	}

void CActiveConsole::Write(TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);

	TBuf8<0x100> buf;
	buf.AppendFormatList(aFmt, list);
	TBuf<0x100> wideBuf;
	wideBuf.Copy(buf);
	iConsole.Write(wideBuf);
	iConsole.Write(_L("\n"));

	RDebug::Print(wideBuf);
	}

void CActiveConsole::WriteNoReturn(TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);

	TBuf8<0x100> buf;
	buf.AppendFormatList(aFmt, list);
	TBuf<0x100> wideBuf;
	wideBuf.Copy(buf);
	iConsole.Write(wideBuf);

	RDebug::Print(wideBuf);
	}

TKeyCode CActiveConsole::Getch()
	{
	return iConsole.Getch();
	}

void CActiveConsole::SelectTestL()
	{
	StopCurrentTest();

	// Pick a test out of the global array of tests.
	const TUint numberOfTests = sizeof(gTestDefinitions) / sizeof (TTestDefinition);
	TLex8 lex(iInputBuffer);
	TUint index;
	TInt err = lex.Val(index);

	if (	err == KErrNone
		&&	index < numberOfTests
		)
		{
		iTest = gTestDefinitions[index].iFactoryL(*this);
		}
	else
		{
		Write(_L8("Unknown selection"));
		}
	}

void CActiveConsole::TestFinished()
/**
 * Called by the test when it has finished. Results in the destruction of the 
 * test.
 */
	{
	StopCurrentTest();
	}

TInt CActiveConsole::RunError(TInt aError)
/**
 * Called by the Active Scheduler when a RunL in this active object leaves.
 */
	{
	// This actually happens when a test object fails to construct properly.
	Write(_L8("Error creating test object: %d"), aError);

	iInputBuffer = KNullDesC8();
	DisplayMainMenu();

	// It's OK to carry on with the program itself, so repost asynchronous 
	// request.
	RequestCharacter();

	return KErrNone;
	}

void CActiveConsole::GetNumberL(TUint& aNumber)
	{
	TBuf<12> addrAsText;
	addrAsText.Zero();
	if ( aNumber != 0 )
		{
		addrAsText.Format(_L("%d"), aNumber);
		}
	WriteNoReturn(_L8("Enter a number: "));
	if ( addrAsText.Length() > 0 )
		{
		TBuf8<100> narrowBuf;
		narrowBuf.Copy(addrAsText);
		WriteNoReturn(narrowBuf);
		}
	TKeyCode code;
	TBuf<1> character;
	FOREVER
		{
		code = Getch();
		character.SetLength(0);
		character.Append(code);
	
		// If <CR> finish editing string
		if (code == 0x0d)
			break;
		
		// if <BS> remove last character
		if ((code == 0x08)&&(addrAsText.Length() != 0))
			{
			WriteNoReturn(_L8("%S"),&character);
			addrAsText.SetLength((addrAsText.Length()-1));
			}
		else
			{
			if (addrAsText.Length() < addrAsText.MaxLength())
				{
				WriteNoReturn(_L8("%S"),&character);
				addrAsText.Append(code);
				}
			}
		}
	//now extract the new address from the string...
	if( !addrAsText.Length() )
		{
		addrAsText.Append('0'); //null string causes TLex::Val to return an error
		}
	TLex lex(addrAsText);
	TInt err = lex.Val(aNumber, EDecimal);
	User::LeaveIfError(err);
	}
