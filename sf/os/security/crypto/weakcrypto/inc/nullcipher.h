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
* CNullCipher class implementation
*
*/


/**
 @file 
 @publishedPartner
 @released 
*/
 
#ifndef __NULLCIPHER_H__
#define __NULLCIPHER_H__

#include "streamcipher.h"

/**
* Stream cipher that does no encryption or decryption, but simply returns the
* data given to it.
* From CBlockTransformation
*
*/
class CNullCipher : public CStreamCipher
{
public:	
	/**
	 * Creates a new CNullCipher object.
	 * 
	 * @return	A pointer to a new CNullCipher object
	 */
	IMPORT_C static CNullCipher* NewL(void);

	/**
	 * Creates a new CNullCipher object.
	 * 
	 * The returned pointer is put onto the cleanup stack.
	 * 
	 * @return	A pointer to a new CNullCipher object
	 */
	IMPORT_C static CNullCipher* NewLC(void);
	virtual void Reset(void);
	virtual TInt KeySize(void) const;
protected:
	virtual void DoProcess(TDes8& aData);
	/**
	 * @internalAll
	 */
	CNullCipher();
};

#endif	//	__NULLCIPHER_H__
