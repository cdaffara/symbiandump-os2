/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* ** IMPORTANT ** PublishedPartner API's in this file are published to 3rd party developers via the 
* Symbian website. Changes to these API's should be treated as PublishedAll API changes and the Security TA should be consulted.
* CBlockTransformation class implementation
*
*/


/**
 @file 
 @publishedPartner
* @released
*/
 
#ifndef	__BLOCKTRANSFORMATION_H__
#define	__BLOCKTRANSFORMATION_H__

#include <e32base.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
/** The maximum block size supported (in bytes) */
const TUint KMaxBlockSizeSupported = 32;
#endif

/**
* Abstract base class defining the interface to block transformation schemes.
*
* Block transformation schemes process a fixed-size block of input to return a
* block of output the same size.  
*
*/
class CBlockTransformation : public CBase
{
public:
	/**
	* Transforms the supplied block, returning the new value using the same
	* parameter. aBlock.Size() must be the same length as BlockSize().
	*
	* @param aBlock	On input, the data to be transformed;
	*				on return, the data after transformation.
	*/
	virtual void Transform(TDes8& aBlock) = 0;
	
	/**
	* Resets the transformation back to its original state. Clears all its buffers.
	*/	
	virtual void Reset() = 0;
	
	/**
	* Gets the block size in bytes.
	*
	* @return	Block size in bytes.
	*/
	virtual TInt BlockSize() const = 0;
	
	/**
	* Gets the key size in bits.	
	* 
	* @return	Key size in bits.
	*/
	virtual TInt KeySize() const = 0;

	/**
	@internalComponent
	Used to retrieve the extended interfaces by id. For Crypto
	SPI internal use only.
	*/		
	TInt GetExtension(TUint aExtensionId, TAny*& a0, TAny* a1);	
};

#endif	//	__BLOCKTRANSFORMATION_H__
