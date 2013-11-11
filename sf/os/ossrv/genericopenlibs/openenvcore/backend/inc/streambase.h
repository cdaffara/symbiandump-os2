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
* Name        : streambase.h
* Part of     : stdio server
* base implementation for media types
*
*/




#ifndef _STREAMBASE_H_
#define _STREAMBASE_H_

#include <e32base.h>//CBase


const TInt KMaxStatusTextLength = 128;

enum TStreamType
	{
	EConsole =0,
	EFile,
	ESerial,
	ENull
	};

//-----------------------------------------------------------------------
//Class name: CStreamBase
//Description: Abstract class that defines stream like objects.
//-----------------------------------------------------------------------
class CStreamBase : public CBase
	{
public:
	virtual void WriteL(TDes8& aDes) = 0;
	virtual void Read(TRequestStatus& aStatus, const TInt aLength) = 0;
	virtual TDes8& Buf();
 	virtual void ReadCancel() = 0;	
 	virtual TStreamType Type() = 0;
	virtual ~CStreamBase()
		{
		}
protected:
	TBuf8<256> iBuf; 	// buffer for holding stdin characters
	CStreamBase()
		{
		iBuf.Zero();
		}
	};

#endif //_STREAMBASE_H_
