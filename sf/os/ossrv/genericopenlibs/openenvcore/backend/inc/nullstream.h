/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : nullstream.h
* Part of     : stdio server
* Null File stream for Stdio Redirection app
*
*/




#ifndef _NULLSTREAM_H_
#define _NULLSTREAM_H_

#include "streambase.h"

//-----------------------------------------------------------------------
//Class name:  CNullStream 
//Description: This class represents the Null File stream abstraction.
//-----------------------------------------------------------------------
class CNullStream : public CStreamBase
	{
public:
	static CNullStream* NewL( );
	~CNullStream();
 	virtual void WriteL(TDes8& aDes);
    virtual void Read(TRequestStatus& aStatus, const TInt aLength);
	virtual void ReadCancel() {}
   	TStreamType Type() { return ENull;}
   	
private:
	CNullStream( );
 	};


#endif //_NULLSTREAM_H_
