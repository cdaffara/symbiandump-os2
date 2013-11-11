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
// File contains internal classes for Ascii To Base64 converison
// and vice-versa.
// 
//

/**
 @file TConvBase64.h
 @internalAll
 @released
*/

#ifndef __TCONVBASE64_H__
#define __TCONVBASE64_H__

#include <e32base.h>


const TInt KMaxB64EncodedCharsPerLine = 60;
// Could be increased to 75 characters for every encoded line if KDecodeLineLength = 675.  
// 60 was chosen to maintain existing behaviour.
const TInt8 KImcvLookUpStartOffset = 43;
const TUint8 KImcvConvEquals = '=';
const TInt8 AsciiToBase64[80]=
	{
    62, -1, -1, -1, 63, 52, 53, 54, 55, 56,
    57, 58, 59, 60, 61, -1, -1, -1, 64, -1,
    -1, -1,  0,  1,  2,  3,  4,  5,  6,  7,
    8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23, 24, 25, -1, -1,
    -1, -1, -1, -1, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
    42, 43, 44, 45, 46, 47, 48, 49, 50, 51
    };
const TInt8 Base64ToAscii[65]=
	{
    65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
    75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
    85, 86, 87, 88, 89, 90, 97, 98, 99,100,
    101,102,103,104,105,106,107,108,109,110,
    111,112,113,114,115,116,117,118,119,120,
    121,122, 48, 49, 50, 51, 52, 53, 54, 55,
    56, 57, 43, 47, 61 
    };
    
/**
Base64 encoding and decoding class.
@internalAll
@released
*/    
class TBase64
	{
private:
	/** 
		enum base64 coding defines 
	*/
	enum
	{ 
	/** Padding characters	*/
	EPadChar = 64 
	};
	
	/** 
	enum for EMaskValues
	*/
	enum EMaskValues
	{ 
	/**	Mask Six bits */
	ESixBitMask = 0x3F, 
	/** Mask Eight bits */
	EEightBitMask = 0xFF 
	};
	
	/** 
	enum for EMaskShiftValues
	*/
	enum EMaskShiftValues
	{ 
	/** enum for 6 	*/
	ESix = 6,
	/** enum for 4 	*/ 
	EFour = 4, 
	/** enum for 2	*/
	ETwo = 2, 
	/** enum for 0	*/
	EZero = 0 
	};

public:
	IMPORT_C TBase64();
	IMPORT_C TInt Encode( const TDesC8& aSrcString, TDes8& rDestString);
	IMPORT_C TBool Decode( const TDesC8& aSrcString, TDes8& rDestString);
	IMPORT_C TInt PortableEncode(const TDesC8& aSrcString, TDes8& aDestString, TInt aLineLength=-1);
private:

	TInt iShiftStored;
	TInt iMaskShiftStored;	
	};
	
#endif	// __TCONVBASE64_H__
