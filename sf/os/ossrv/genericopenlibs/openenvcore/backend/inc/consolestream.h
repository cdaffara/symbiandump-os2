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
* Name        : consolestream.h
* Part of     : stdio server
* To handle console read and write
* 
*
*/



 
#ifndef _CONSOLESTREAM_H_
#define _CONSOLESTREAM_H_

#include "streambase.h" //CStreamBase
#include "ReadRequestHandler.h" //MRead and CReadRequestHandler
#include <e32keys.h>


class CConsoleBase;

//Enumerations for the console echo - server side
enum TConsoleEcho
	{
	EOff = 0, //Turn Off the echo
	EOn = 1,  //Turn On the echo
	EPrintValid = 33, //Printable ascii character set <33-126>
	EPrintInvalid=127 
	};

//-----------------------------------------------------------------------
//Class name:  CConsoleStream
//Description: This class represents the console stream abstraction.
// 			   It is created through the factory class CConsoleStreamFactory
//-----------------------------------------------------------------------
class CConsoleStream : public CStreamBase
	{
public:
    static CConsoleStream* NewL(TInt aWidth, TInt aHeight);
    virtual void WriteL(TDes8& aDes);
    virtual void Read(TRequestStatus& aStatus, const TInt aLength);
 	~CConsoleStream();
 	virtual void ReadCancel();
 	TDes8& Buf();
 	TInt SetEcho(TUint8 aEcho);
 	TStreamType Type() { return EConsole;}
	
private:
    CConsoleStream();	
    void ConstructL(TSize aConsoleSize);	 	 
    CConsoleBase* iConsole;
    TInt iBufPos;
    TUint8 iEchoVal; //decides what to echo
    void HandleBuf(TKeyCode & key);
    TBuf8<1> iTbuf;
    };

 #endif //_CONSOLESTREAM_H_
