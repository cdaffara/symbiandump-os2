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
*
*/


/**
 @file 
 @publishedPartner
 @released 
*/
 
#ifndef __MODE_H__
#define __MODE_H__

#include "blocktransformation.h"

/**
* Abstract class defining the use of block transformation objects as block
* chaining modes.
*
* It is initialised with a subclass of CBlockTransformation,
* which it subsequently owns.  Calls to its Transform() function will call the
* Transform() function in the underlying CBlockTransformation object, and perform
* the additional transformation for block chaining in that mode.  This all means
* that if you want to do, say, AES encryption in CBC mode, you need to construct
* a CAESEncryptor object, then pass it to the CModeCBCEncryptor subclass of
* CBlockChainingMode, and subsequently use the CModeCBCEncryptor object to call
* Transform().
* 
* @publishedPartner
* @released 
*/
class CBlockChainingMode : public CBlockTransformation
{
public:
	virtual void Reset();
	virtual TInt BlockSize() const;
	virtual TInt KeySize() const;
public:
	/**
	* Sets the initialization vector.
	* 
	* @param aIV	The initialization vector.  The length of this descriptor must be
	*				the same as the underlying cipher's block size.
	*/
	virtual void SetIV(const TDesC8& aIV);
protected:
	/** Default constructor */
	IMPORT_C CBlockChainingMode();
	/** 
	 * Second phase constructor
	 * 
	 * This should be called last by derived classes' ContructL()s .
	 *
	 * @param aBT	A block transformation object
	 * @param aIV	Initialization vector, the length of this descriptor must be
	 *				the same as the underlying cipher's block size.
	 */
	IMPORT_C void ConstructL(CBlockTransformation* aBT, const TDesC8& aIV);
	
	/** The destructor frees all resources owned by the object, prior to its destruction. */
	IMPORT_C virtual ~CBlockChainingMode();
protected:
	/** A block transformation object */
	CBlockTransformation* iBT;

	/** 
	 * A buffer containing the feedback register
	 *
	 * This must equal the underlying cipher's block size in length. 
	 * Initially this register is filled with the initialization vector.
	 */
	HBufC8* iRegisterBuf;

	/** Encapsulates a pointer to iRegisterBuf */
	TPtr8 iRegister;

	/** 
	 * A buffer containing the Initialisation Vector (IV) 
	 *
	 * This must equal the underlying cipher's block size in length. 
	 */
	HBufC8* iIVBuf;

	/** Encapsulates a pointer to iIVBuf */
	TPtr8 iIV;
};

#endif	//	__MODE_H__
