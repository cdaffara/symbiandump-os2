// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// 
//

/**
 @file inetprottextutils.h
 @publishedAll
 @released
*/

#ifndef __INETPROTTEXTUTILS_H__
#define __INETPROTTEXTUTILS_H__

#include <e32base.h>

/**
	Comments :
@publishedAll
@released
*/
class InetProtTextUtils
	{
public:	// Enums

/**	
	enum TRemoveMode
	Enum defining whitespace removal modes.
 */
	enum TRemoveMode
		{
		/** 
			Specifies removal any contiguous whitespace characters at the 
			beginning of some data.
		*/ 
		ERemoveLeft		= 0,
		/** 
			Specifies removal any contiguous whitespace characters at the end of
			some data.
		*/ 
		ERemoveRight,
		/** 
			Specifies removal any contiguous whitespace characters at the 
			beginning and end of some data.
		*/ 
		ERemoveBoth
		};

public:	// Methods
	
	IMPORT_C static TInt RemoveWhiteSpace(TPtrC8& aData, TRemoveMode aMode);
	IMPORT_C static TInt RemoveWhiteSpace(TPtrC16& aData, TRemoveMode aMode);

	IMPORT_C static void ConvertIntToDescriptorL(TInt aInt, HBufC8*& aBuffer);
	IMPORT_C static void ConvertIntToDescriptorL(TInt aInt, HBufC16*& aBuffer);

	IMPORT_C static void ConvertHexToDescriptorL(TInt aHex, HBufC8*& aBuffer);
	IMPORT_C static void ConvertHexToDescriptorL(TInt aHex, HBufC16*& aBuffer);

	IMPORT_C static TInt ConvertDescriptorToInt(const TDesC8& aData, TInt& aInt);
	IMPORT_C static TInt ConvertDescriptorToInt(const TDesC16& aData, TInt& aInt);

	IMPORT_C static TInt ConvertDescriptorToHex(const TDesC8& aData, TInt& aHex);
	IMPORT_C static TInt ConvertDescriptorToHex(const TDesC16& aData, TInt& aHex);

	IMPORT_C static TInt ExtractNextTokenFromList(TPtrC8& aBuffer, TPtrC8& aToken, TChar aSeparator);
	IMPORT_C static TInt ExtractNextTokenFromList(TPtrC16& aBuffer, TPtrC16& aToken, TChar aSeparator);

	IMPORT_C static TInt ExtractNextTokenFromList(TPtrC8& aBuffer, TPtrC8& aToken, const TDesC8& aSeparators);
	IMPORT_C static TInt ExtractNextTokenFromList(TPtrC16& aBuffer, TPtrC16& aToken, const TDesC16& aSeparators);

	IMPORT_C static TInt ExtractQuotedStringL(TPtrC8& aBuffer, TPtrC8& aQuotedString);
	IMPORT_C static TInt ExtractQuotedStringL(TPtrC16& aBuffer, TPtrC16& aQuotedString);

	IMPORT_C static TInt ExtractIntegerValueL(TPtrC8& aBuffer, TInt& aIntVal, TBool aAllowNonWsTerminator);
	};

#endif	// __INETPROTTEXTUTILS_H__
