/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __DSAKEYPAIRSHIM_H__
#define __DSAKEYPAIRSHIM_H__

#include "asymmetrickeys.h"

/** 
* This class generates an DSA public/private key pair via CryptoSPI
*/

NONSHARABLE_CLASS(CDSAKeyPairShim) : public CDSAKeyPair
	{
public:
	/**
	 * Creates a new DSA key pair
	 */
	static CDSAKeyPairShim* NewLC(TUint aKeyBits);

	~CDSAKeyPairShim(void);

protected:
	/** Constructor */
	CDSAKeyPairShim(void);

	/** 
	 * 2nd stage constructor
	 */
	void ConstructL(TUint aKeyBits);

private:
	CDSAKeyPairShim(const CDSAKeyPairShim&);
	CDSAKeyPairShim& operator=(const CDSAKeyPairShim&);	
	};

#endif	//	__DSAKEYPAIRSHIM_H__
