/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* TASN1DecX500DirectoryString class implementation
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __X500DEC_H__
#define __X500DEC_H__

#include <asn1dec.h>
#include <x520ava.h>
#include <x500dn.h>

class TASN1DecX500DirectoryString

	{
public:
	TASN1DecX500DirectoryString();
	HBufC* DecodeDERL(const TDesC8& aSource,TInt& aPos, TInt aMaxLength = KMaxTInt);
	HBufC* DecodeDERL(const TASN1DecGeneric& aSource, TInt aMaxLength = KMaxTInt);
	TInt EncodeDER(TDes8& aDestination,TInt& aPos);
	TInt LengthDERContent(void) const;
	void SetMaxLength(TInt aLength);
private:
	HBufC* DecodeContentsL(const TASN1DecGeneric& aSource, TInt aMaxLength);
	TInt iMaxLength;
	};

#endif
