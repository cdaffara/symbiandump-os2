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
* X509bitstring.h
*
*/


/**
 @file 
 @internalTechnology
 @released
*/

#if !defined (X509BITSTRING_H)
#define X509BITSTRING_H

//3) key usage

class CX509BitString : public CBase
/** An X.509 bit string. 
*
* @since v6.0 */
	{
public:
	/** Destructor.
	* 
	* Frees all resources owned by the object. */
	~CX509BitString();
	
	/** Tests whether the specified bit is set.
	* 
	* @param aBit	The offset of the bit to be tested. This is a value relative to 
	* 				zero. Any value greater than or equal to the length of the bit 
	*				string will always cause EFalse to be returned.
	* @return		ETrue, if the bit is set; EFalse, otherwise. */
	TBool IsSet(TInt aBit) const;
	
	/** Creates the X.509 bit string.
	* 
	* @param aData				A heap descriptor representing the bit string data.
	* @param aEffectiveLength	The number of bits in the string. */
	CX509BitString(HBufC8* aData, TInt aEffectiveLength);
private:
	HBufC8* iData;
	TInt iLength;
	};


#endif
