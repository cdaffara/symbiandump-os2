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
// Name        : stdiosesion.cpp
// Part of     : stdio server
// To implement the stdio session
// Class definition for Server Side Session
//




#include <e32cons.h>	  //KConsFullScreen

#include "StdioSession.h"	//CStdioSession
#include "consolestream.h"	//CConsoleStream
#include "serialstream.h"	//CSerialStream
#include "filestream.h"		//CFileStream
#include "nullstream.h"		//CNullStream
#include "ReadRequestHandler.h" //CReadRequestHandler

_LIT8(KErrOpeningIn, "Error opening IN Media: %d. defaults to console\n");
_LIT8(KErrMediaType, "Wrong %s Media Type Mentioned in config.ini: defaults to console\n");
_LIT8(KErrOpeningOut, "Error opening OUT Media: %d. defaults to console\n");
_LIT8(KErrOpeningIni, "Error opening config.ini: %d. stdio defaults to console\n");


//-------------------------------------------------------------------------------
// Function Name : CStdioSession::ReadIniL()
// Description   : 	ReadIniL is a helper function to parse the .ini file and create 
//					the corresponding factory classes. If the corresponding Section
//					is not found in the .ini file then console factory class is 
//					created.
//-------------------------------------------------------------------------------	

void CStdioSession::ReadIniL(CIniData* aIniData, TPtrC& aMedia, 
							 CStreamBase*& aStream, TBool aIsWrite)
 	{
 	TPtrC mediaType;
 	TInt x;
 	TInt y;
 	
 	//set default value
 	mediaType.Set(_L("console"));
 	
 	if( aIniData->FindVar(aMedia,_L("type"),mediaType) )
 		{
 		if( !mediaType.Compare(_L("file")) )
 			{
 			TPtrC path;
 			if(!aIniData->FindVar(aMedia, _L("path"), path))
 				{
 				User::Leave(KErrNotFound);
 				}
 			aIniData->FindVar(aMedia, _L("max size"), x);
 			aStream = CFileStream::NewL(path, aIsWrite);
 			iIsInMediaFile = ETrue;
 			}
 		else if( !mediaType.Compare(_L("serial")) )
 			{
 			TPtrC portName;
 			aIniData->FindVar(aMedia, _L("baud"), x);
  			if(!aIniData->FindVar(aMedia, _L("port"), portName))
 				{
 				//Defaults to Console
 				User::Leave(KErrNotFound);
 				}
 			aStream = CSerialStream::NewL(x, portName);				
 			}
 		else if( !mediaType.Compare(_L("nulldevice")))
 			{
			aStream = CNullStream::NewL( );
			iIsInMediaFile = ETrue;
 			
 			}
 		else
 			{
 			if ( !iIsInputConsole )
 				{
 				//no console is created for input
 				iIsInputConsole = ETrue;
 				if(!aIniData->FindVar(aMedia, _L("height"), y))
 					{
 					y = -1;
 					}
 				if(!aIniData->FindVar(aMedia, _L("width"), x))
 					{
 					x = -1;
 					}
 				aStream = CConsoleStream::NewL(x, y);
 				}
 			else
 				{
 				//assign input console to output
 				aStream = iStreamRead;
 				}
 			}
		}
 	else
 		{
 		if ( !iIsInputConsole )
 			{
 			//no console is created for input
 			aStream = CConsoleStream::NewL(KConsFullScreen, KConsFullScreen);
 			//This condition for in case where STDIN is NONE and STDOUT is a valid 
 			//Media type. 
 			if( aIsWrite == EFalse )
 				{
 				iErrInBuf.Format(KErrMediaType, "IN");
 				iIsInputConsole = ETrue;
 				}
 			else
 				{
 				iErrOutBuf.Format(KErrMediaType, "OUT");
 				}
 			}
 		else
 			{
 			//assign input console to output
 			aStream = iStreamRead;
 			iErrOutBuf.Format(KErrMediaType, "OUT");
 			}
 		}
 	}

//-------------------------------------------------------------------------------
// Function Name : 	CStdioSession::NewL()
// Description   : 	Two phase construction. NewL is static function. 
//					CallsConstructL(). Returns the constructed Session object.	  				 
//-------------------------------------------------------------------------------	
 
CSession2* CStdioSession::NewL()
	{
	CStdioSession* self = new (ELeave) CStdioSession();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CStdioSession::ConstructL()
// Description   : 	Read the .ini file.Uses the helper function ReadIniL function
//					to create the Stream class as mentioned in the .ini file.
//					If .ini file is not found or if there is any other error in 
//					reading the .ini file, then by console stream is created.			 				 
//-------------------------------------------------------------------------------	
 
void CStdioSession::ConstructL()
	{
	//read ini and set stream accordingly
	TPtrC mediaIn;
	TPtrC mediaOut;
	
	
 	TRAPD(ret, iIniData = CIniData::NewL(KFileName));
	if ( KErrNone == ret )
		{
		if ( (iIniData->FindVar(_L("STDIO"),_L("STDIN"),mediaIn)) && (mediaIn.Compare(_L("NONE"))) )
			{
			TRAP(ret, ReadIniL(iIniData, mediaIn, iStreamRead, EFalse));
			}
		else 
			{
			ret = KErrNotFound;
			}
		
			
		if ( KErrNone != ret && mediaIn.Compare(_L("NONE")) )
			{
			iIsInputConsole = ETrue;
			iIsInMediaFile = EFalse;
			iErrInBuf.Format(KErrOpeningIn, ret);
			iStreamRead = CConsoleStream::NewL(KConsFullScreen, KConsFullScreen);
			}
			
		if ( iIniData->FindVar(_L("STDIO"),_L("STDOUT"),mediaOut)
		     && (mediaOut.Compare(_L("NONE"))) )
			{
			if( !mediaIn.Compare(mediaOut) && !iIsInMediaFile )
				{ 
				iStreamWrite = iStreamRead;
				if( iErrInBuf.Length() )
					{
					iErrOutBuf.Format(KErrMediaType, "OUT");
					}
				}
			else
				{
				TRAP(ret, ReadIniL(iIniData, mediaOut, iStreamWrite, ETrue));
				}
			}
		else
			{
			ret = KErrNotFound;
			}
		//If Both the Media mentioned is NONE then there is no need to go further
		//we can leave here only.	
		if( !(mediaOut.Compare(_L("NONE"))) && !(mediaIn.Compare(_L("NONE"))) )	
			{
			User::Leave(KErrNotFound);
			}
		//Still need to check for NONE as the ret value can be 	KErrNotFound
		//and STDIN in not NONE
		if ( KErrNone != ret && (mediaOut.Compare(_L("NONE"))) )
			{
			iErrOutBuf.Format(KErrOpeningOut, ret);
			if ( !iIsInputConsole )
				{
				iStreamWrite = CConsoleStream::NewL(KConsFullScreen, KConsFullScreen);
				}
			else
				{
				iStreamWrite = iStreamRead;
				}
			}
		}
	else
		{
		//default media
		iErrInBuf.Format(KErrOpeningIni, ret);
		iStreamWrite = iStreamRead = CConsoleStream::NewL(KConsFullScreen, KConsFullScreen);
		}
		
	
	iReadRequestHandler = CReadRequestHandler::NewL(iStreamRead);
	
	
	if ( iErrInBuf.Length() )	
		{
		iStreamRead->WriteL(iErrInBuf);
		}
	
	if ( iErrOutBuf.Length() )	
		{
		iStreamWrite->WriteL(iErrOutBuf);
		} 
	 
	}
//-------------------------------------------------------------------------------
// Function Name : 	CStdioSession::~CStdioSession()
// Description   : 	Destructor. Deletes the Factory class as created and the 
//					iIniData created in ConstructL function.Also calls the 
//					RemoveSession() to reduce the count of session.	 				 
//-------------------------------------------------------------------------------	

CStdioSession::~CStdioSession()
	{
	if ( iIniData )
		{
		delete iIniData;
		}
	if ( iReadRequestHandler )
		{
		delete iReadRequestHandler;
		iReadRequestHandler = 0;
		}
	if ( (iStreamWrite == iStreamRead) && iStreamWrite)	
		{
		delete iStreamWrite;
		}
	else
		{
		if ( iStreamWrite )
			{
			delete iStreamWrite;
			}
			
		if ( iStreamRead )
			{
			delete iStreamRead;
			}

		}
	//Only case where iStreamWrite & iStreamRead is NULL which means leave 
	//occured in CStdioSession::ConstructL(). This destructor is called by the 
	//cleanup stack.
	if(iStreamWrite || iStreamRead)
		{
		Server().RemoveSession();
		}
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CStdioSession::CreateL()
// Description   : 	Increaments the session count. Calls the AddSession method to
//					increament the session count.
//					 				 
//-------------------------------------------------------------------------------	

void CStdioSession::CreateL()
	{
	Server().AddSession();		
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CStdioSession::ServiceL()
// Description   : 	For any request from the client process, the this method
//					checks the request and issues the corresponding request.
//					the iStreamRead and iStreamWrite depends on the media type
//					defined as in the .ini file. 			 				 
//-------------------------------------------------------------------------------	
		
void CStdioSession::ServiceL(const RMessage2& aMessage)
	{
	TInt ret = KErrNone;
	switch (aMessage.Function())
		{
		case ERead:
		   	TRAP(ret, iReadRequestHandler->ServiceL(aMessage));
		   	if ( ret != KErrNone )
		   		{
		   	   	aMessage.Complete(ret);
		   		}
			break;
		case EWrite:
			ret = WriteStream(aMessage);
			aMessage.Complete(ret);
			break;
		case EFlush:
			aMessage.Complete(ret);
			break;
		case ECheckMedia:
			if( NULL == iStreamRead )
				{
				TRAP( ret,aMessage.WriteL(0, _L8("R")) );
				}
			else
				{
				if( NULL == iStreamWrite )
					{
					TRAP( ret,aMessage.WriteL(0, _L8("W")) );
					}
				}
			aMessage.Complete(ret);	
			break;		

		case ENotifyActivity:
		
			TRAP(ret, iReadRequestHandler->NotifyActivityL(aMessage));
			if ( ret != KErrNone )
		   		{
		   		aMessage.Complete(ret);
		   		}
			break;
		
		case ECancelNotify:
		
			iReadRequestHandler->CancelNotify();
			aMessage.Complete(KErrNone);
			break;

		case EEcho:
			{
			//The intent of this service is (to/not to) echo the characters read from the
			//readstream(STDIN) of this server, therefore the echo flag of the readstream will
			//only be altered for this request unless both read/write refers to the same. This echo flag
			//will only be used during the read service call.
			
			//The stream should be of Console type for this call to be successfull, otherwise
			//it returns KErrNotSupported.
			 
			switch(iStreamRead->Type())
				{
				case EConsole:
					{
					TInt err = static_cast<CConsoleStream*> (iStreamRead)->SetEcho(aMessage.Int0());
					aMessage.Complete(err);
					}
					break;	
				default:
					aMessage.Complete(KErrNotSupported);
					break;	
				}
			}
			break;
			
		default:
			aMessage.Complete(KErrNotSupported);
			break;
		}
		return;
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CStdioSession::WriteStreamL()
// Description   : 	This methos implements the write functionality in any media 
//					type. The media type can be either be console or serial.
//-------------------------------------------------------------------------------	

TInt CStdioSession::WriteStream(const RMessage2& aMessage)
	{
	TInt ret;
	//make a heap buffer to accumulate all client data
	HBufC8* hBuf = HBufC8::NewL(aMessage.Int1());
  	TPtr8 ptr(hBuf->Des());
	// retrieve the descriptor to be written
	aMessage.ReadL(0, ptr);
	// send descriptor to stream for printing
	TRAP(ret, iStreamWrite->WriteL(ptr));
	if(ret != KErrNone)
		{
		return ret;
		}
	TInt length = ptr.Length();
	delete hBuf;	
	return length;
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CStdioSession::Server()
// Description   : 	Returns the handle to the server
//					 				 
//-------------------------------------------------------------------------------	

CStdioServer& CStdioSession::Server()
    {
    return *(static_cast<CStdioServer*>(const_cast<CServer2*>(CSession2::Server())));
    }
