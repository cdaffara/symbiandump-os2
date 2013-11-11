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
* CBC mode encryptor and decryptor implementation
*
*/


/**
 @file 
 @publishedPartner
 @released 
*/
 
#ifndef __CBCMODE_H__
#define __CBCMODE_H__

#include "mode.h"

/**
* Concrete subclass of CBlockChainingMode implementing CBC mode block chaining
* for encryption.
*
*/
class CModeCBCEncryptor : public CBlockChainingMode
{
public:
	/**
	* Creates an object of this class for CBC mode encryption.
	*
	* @param aBT	An appropriate CBlockTransformation derived encryptor.
	* @param aIV	Initialization vector, the length of this descriptor must be
	*				the same as the underlying cipher's block size.
	* @return		A pointer to the new CModeCBCEncryptor object
	*/
	IMPORT_C static CModeCBCEncryptor* NewL(CBlockTransformation* aBT, 
		const TDesC8& aIV);

	/**
	* Creates an object of this class for CBC mode encryption.
	*
	* The returned pointer is put onto the cleanup stack.
	*
	* @param aBT	An appropriate CBlockTransformation derived encryptor.
	* @param aIV	Initialization vector, the length of this descriptor must be
	*				the same as the underlying cipher's block size.
	* @return		A pointer to the new CModeCBCEncryptor object
	*/
	IMPORT_C static CModeCBCEncryptor* NewLC(CBlockTransformation* aBT, 
		const TDesC8& aIV);
	virtual void Transform(TDes8& aBlock);	
protected:
	/**
	 * @internalAll
	 */
	CModeCBCEncryptor();
private:
	const CModeCBCEncryptor& operator=(const CModeCBCEncryptor&);
};

/**
* Concrete subclass of CBlockChainingMode implementing CBC mode block chaining
* for decryption.
*
*/
class CModeCBCDecryptor : public CBlockChainingMode
{
public:
	/**
	* Creates an object of this class for CBC mode decryption.
	* 
	* @param aBT	An appropriate CBlockTransformation derived decryptor.
	* @param aIV	Initialization vector, the length of this descriptor must be
	*				the same as the underlying cipher's block size.
	* @return		A pointer to the CModeCBCDecryptor new object.
	*/
	IMPORT_C static CModeCBCDecryptor* NewL(CBlockTransformation* aBT, 
		const TDesC8& aIV);

	/**
	* Creates an object of this class for CBC mode decryption.
	*
	* The returned pointer is put onto the cleanup stack.
	* 
	* @param aBT	An appropriate CBlockTransformation derived decryptor.
	* @param aIV	Initialization vector, the length of this descriptor must be
	*				the same as the underlying cipher's block size.
	* @return		A pointer to the CModeCBCDecryptor new object.
	*/
	IMPORT_C static CModeCBCDecryptor* NewLC(CBlockTransformation* aBT, 
		const TDesC8& aIV);
	virtual ~CModeCBCDecryptor(void);
public:
	virtual void Transform(TDes8& aBlock);	
protected:
	/** @internalAll */
	CModeCBCDecryptor();
	/** @internalAll */
	void ConstructL(CBlockTransformation* aBT, const TDesC8& aIV);
private:
	HBufC8* iIVBakBuf;
	TPtr8 iIVBak;
private:
	const CModeCBCDecryptor& operator=(const CModeCBCDecryptor&);
};

#endif	//	__CBCMODE_H__
