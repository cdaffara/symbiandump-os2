/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* TWTLSDecVector class implementation
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __WTLSDEC_H__
#define __WTLSDEC_H__

#include <e32base.h>
#include <e32std.h>
#include <bigint.h>

//implementation of decoder for variable length vector
class TWTLSDecVector
	{
public:
	TWTLSDecVector(const TDesC8& aSource, TInt aMinLength, TInt aMaxLength);
	void InitL();

//	TPtrC8 Encoding() const;
	TInt EncodingLength() const;
	
//	TPtrC8 Content() const;
//	TInt ContentLength() const;
	
//	TInt HeaderLength() const;
private:
	const TPtrC8 iEncoding;
	const TInt iMinLength;
	const TInt iMaxLength;
	TInt iLengthOfContents;
	TInt iLengthOfLength;
	};

class TWTLSDecUnsignedInteger
	{
public:
	TWTLSDecUnsignedInteger();
	TInt DecodeShortL(const TDesC8& aSource,TInt& aPos, TInt aLengthOfLength);
	RInteger DecodeLongL(const TDesC8& aSource,TInt& aPos, TInt aLength);
	};

class TWTLSDecTime
	{
public:
	TWTLSDecTime();
	TTime DecodeL(const TDesC8& aSource, TInt& aPos);
	};

#endif
