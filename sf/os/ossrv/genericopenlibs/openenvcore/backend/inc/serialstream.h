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
* Name        : serialstream.h
* Part of     : stdio server
* To handle the read and write on serial(comm)
* 
*
*/



#ifndef _SERIALSTREAM_H_
#define _SERIALSTREAM_H_

#include "StdioServer.h"
#include "streambase.h" //CStreamBase2
#include "ReadRequestHandler.h" //CReadRequestHandler

//-----------------------------------------------------------------------
//Class name: 	CSerialStream
//Description: 	This class represents the serial stream.
//-----------------------------------------------------------------------

class CSerialStream : public CStreamBase
    {
public:
	static CSerialStream* NewL(TInt aBaudRate,TDesC& aPortName);
    virtual void WriteL(TDes8& aDes);
    virtual void Read(TRequestStatus& aStatus, const TInt aLength);
    ~CSerialStream();
	virtual void ReadCancel();
	TStreamType Type() { return ESerial;}
	
private:
   	CSerialStream(TInt aBaudRate,TDesC& aPortName);
    void ConstructL();
    RComm iPort;
    TInt iOpenErr;
    TInt iConfigErr;
    TSerialInfo iPortInfo;
    TFileName iPortName;
    TInt iBaudRate;
    };

#endif //_SERIALSTREAM_H_
