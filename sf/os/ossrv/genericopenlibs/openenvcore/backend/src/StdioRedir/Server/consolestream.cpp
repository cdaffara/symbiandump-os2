// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : consolestream.cpp
// Part of     : stdio server
// To handle console read and write
// 
//

#include "consolestream.h"
#include <e32cons.h>//CConsoleBase


//
// -------------- ConsoleStream implementation ------------------------
//

//-------------------------------------------------------------------------------
// Function Name : 	CConsoleStream::NewL()
// Description   : 	Implements two phase construction. Creates the CConsoleStream
//					object and then creates the Console object by calling the 
//					ConstructL().	 
//-------------------------------------------------------------------------------	
	
CConsoleStream* CConsoleStream::NewL(TInt aWidth, TInt aHeight) 
	{
    CConsoleStream* r = new(ELeave) CConsoleStream();
    CleanupStack::PushL(r);
    r->ConstructL(TSize (aWidth, aHeight)) ;
    CleanupStack::Pop();
    return r;
    }

//-------------------------------------------------------------------------------
// Function Name : 	CConsoleStream::ConstructL() 
// Description   : 	Creates the Console object, with the default colour and full 
//					size.
//					 				 
//-------------------------------------------------------------------------------	
	
void CConsoleStream::ConstructL(TSize aConsoleSize) 
	{
 	iConsole = Console::NewL(_L("STDIO CONSOLE"),aConsoleSize);
	}

//-------------------------------------------------------------------------------
// Function Name : 	CConsoleStream::CConsoleStream()
// Description   : 	Simple constructor
//					 				 
//-------------------------------------------------------------------------------	
	

CConsoleStream::CConsoleStream()
    {
    iEchoVal = EOn; //on the echo
    iBuf.Zero();
    iBufPos = 0;
    }

//-------------------------------------------------------------------------------
// Function Name : 	CConsoleStream::~CConsoleStream()
// Description   : 	Deletes the Created CReadRequestHandler and created Console
//					 				 
//-------------------------------------------------------------------------------	
	
CConsoleStream::~CConsoleStream()
    {
    delete iConsole;
    }

//-------------------------------------------------------------------------------
// Function Name : 	CConsoleStream::WriteL()
// Description   : 	Calls the console output interface to print the descriptor
// 					onto the console		 				 
//-------------------------------------------------------------------------------	
	
void CConsoleStream::WriteL(TDes8& aDes) 
	{
   	HBufC16* hBuf = HBufC16::NewL(aDes.Length());
  	TPtr ptr(hBuf->Des());
  	ptr.Copy(aDes);
    iConsole->Write(*hBuf);
    //Get the current cursor position to check if the backspace does not delete 
    //the previous output
 
    delete hBuf;
	}

	
//-------------------------------------------------------------------------------
// Function Name : 	CConsoleStream::Read()
// Description   : 	Issues the Read Request with the iStatus so that read request 
//					status associated with an asynchronous request. Also the 
//					active scheduler uses this to check whether the active 
//					object's request has completed 				 
//-------------------------------------------------------------------------------	
	
void CConsoleStream::Read(TRequestStatus& iStatus, const TInt /*aLength*/)
 	{
 	iConsole->Read(iStatus);
 	}
 	
//-------------------------------------------------------------------------------
// Function Name : 	CConsoleStream::ReadCancel()
// Description   : 	Cancels any pending read requests 				 
//-------------------------------------------------------------------------------	

void CConsoleStream::ReadCancel()
	{
	iConsole->ReadCancel();
	}

//---------------------------------------------------------------------------------------
// Function Name : 	CConsoleStream::HandleDisplay()
// Description   : 	Maintains a internal buffer
//                  
//----------------------------------------------------------------------------------------	
void CConsoleStream::HandleBuf(TKeyCode & key)
	{
	TBuf8<1> tbuf;
	
	switch(key)
		{
		case EKeyBackspace:
			{
			if(!iBufPos)
				break;
			iBuf.Delete(--iBufPos,1);
			break;
			}
		default:
			{
			if(iBuf.Length() == 256)
			{
				iBuf.Zero();
				iBufPos = 0;		
			}
			tbuf.Append(key);
			iBuf.Insert(iBufPos++,tbuf);
			break;
			}
	
		}
	}
//-------------------------------------------------------------------------------
// Function Name : CConsoleStream::Buf()
// Description   : fills the iBuf from the char just read from console
//-------------------------------------------------------------------------------	
		
TDes8& CConsoleStream::Buf()
 	{
 	TKeyCode code = iConsole->KeyCode();
	TText8 ch;
	iTbuf.Zero();
	switch(code)
		{
		case EKeyPrintScreen:
		return iTbuf;		// unknowable keycodes are ignored

		case EKeyEnter:
			ch='\n';
			iBuf.Zero();
			iBufPos = 0;
			iTbuf.Append(ch);
			iConsole->Printf(_L("%c"),ch);
			return iTbuf;   
			
		case EKeyNull:
			//no input char is there
			return iTbuf;
		case EKeyTab:
		case EKeyBell:
		case EKeyVerticalTab:
		case EKeyEscape:
			return iTbuf;
		default:
		    if(code < ENonCharacterKeyBase)
				ch=(TText8)(code& 0xff);
		    else
    	    	return iTbuf;
		    break;
		}
	iTbuf.Append(ch); 
	TBuf<2> tembuf;
	tembuf.Copy(iTbuf);
	
	if( !(!iBufPos && (code == EKeyBackspace)))
		{
		HandleBuf(code);
		if(iEchoVal == EOn) 
	   		{ 
	   		iConsole->Write(tembuf);	
	   		}
		else if( (iEchoVal >= EPrintValid) && (iEchoVal < EPrintInvalid) )	
			{
			switch(code)
				{
				case EKeyBackspace:
					//erase the previous character from the display.
					iConsole->Write(tembuf);
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
		else
			{
			//dont echo any of the input characters
			return iBuf;	
			}
			
		if(code == EKeyBackspace)
	   		iConsole->ClearToEndOfLine();
		}
		
	return iTbuf;
 	}
	
//-------------------------------------------------------------------------------
// Function Name : TInt CConsoleStream::SetEcho(TUint8 aEcho)
// Description   : Sets the echo flag accordingly. 
//-------------------------------------------------------------------------------	
TInt CConsoleStream::SetEcho(TUint8 aEcho)
	{
	if( (aEcho > EOn && aEcho < EPrintValid) || (aEcho >= EPrintInvalid) )
		return KErrArgument;
	
	iEchoVal=aEcho;
	return KErrNone;
	}


