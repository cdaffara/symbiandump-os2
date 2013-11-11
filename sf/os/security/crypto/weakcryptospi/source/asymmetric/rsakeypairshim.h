/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __RSAKEYPAIRSHIM_H__
#define __RSAKEYPAIRSHIM_H__

#include "asymmetrickeys.h"

/** 
* This class generates an RSA public/private key pair via CryptoSPI
*/

NONSHARABLE_CLASS(CRSAKeyPairShim) : public CRSAKeyPair
	{
public:
	/**
	 * Creates a new RSA key pair
	 */
	static CRSAKeyPairShim* NewLC(TUint aModulusBits, TRSAPrivateKeyType aKeyType);

	~CRSAKeyPairShim(void);

protected:
	/** Constructor */
	CRSAKeyPairShim(void);

	/** 
	 * 2nd stage constructor
	 */
	void ConstructL(TUint aModulusBits, TRSAPrivateKeyType aKeyType, TInt aPublicExponent);

private:
	CRSAKeyPairShim(const CRSAKeyPairShim&);
	CRSAKeyPairShim& operator=(const CRSAKeyPairShim&);	
	};

#endif	//	__RSAKEYPAIRSHIM_H__
