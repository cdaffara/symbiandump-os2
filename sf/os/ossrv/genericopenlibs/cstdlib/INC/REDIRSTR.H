// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

//  Abstract stream classes for Stdio Redirection app
//
#ifndef _REDIRSTR_H_
#define _REDIRSTR_H_

#include <e32base.h>
#include <e32std.h>
/**
Maximum size of TBuf<> data member in the CStreamFactoryBase2 class.
@see CStreamFactoryBase2
@publishedPartner
@released
*/
const TInt KMaxStatusTextLength = 128;

// The following functions are defined to replace previous exports so def file 
// does not need reordering.
IMPORT_C void DummyReserved9();
IMPORT_C void DummyReserved10();
IMPORT_C void DummyReserved11();
	
/**
Abstract class that defines stream like objects.

@publishedPartner
@released
*/	
class CStreamBase2 : public CBase
	{
public:
	virtual void Write(TDes8& aDes) = 0;
	virtual void Read(const RMessage2& aMessage) = 0;
protected:	
	TBuf8<256> iBuf; // buffer for holding stdin characters
	IMPORT_C void CompleteRead(const RMessage2 &aMessage);
	};

/** 
Abstract Factory class for creating stream-like classes.

@publishedPartner
@released
*/
class CStreamFactoryBase2 : public CBase
	{
public:
	virtual CStreamBase2* GetStream() = 0;
	virtual void Configure() = 0;
	IMPORT_C virtual const TDesC& GetStatus() const;
protected:
	IMPORT_C void AppendToStatus(const TDesC& aMsg);
private:
	TBuf<KMaxStatusTextLength> iStatusMsg;
	};


#endif
