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

#ifndef __DHKEYPAIRSHIM_H__
#define __DHKEYPAIRSHIM_H__

#include "asymmetrickeys.h"

	
/** 
* This class is capable of generating a Diffie-Hellman (DH) public/private key pair using the new crypto SPI interface.
*/
NONSHARABLE_CLASS(CDHKeyPairShim) : public CDHKeyPair
	{
public:
	/**
	 * Creates a new DH key pair from a random large integer,
	 * and a specified large prime and generator.
	 *
	 * The returned pointer is put onto the cleanup stack.
	 *
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 * @return A pointer to a CDHKeyPairShim instance
	 * 
	 * @leave KErrArgument	If aG is out of bounds 
	 */
	static CDHKeyPairShim* NewLC(RInteger& aN, RInteger& aG);

	/**
	 * Creates a new DH key pair from a specified 
	 * large prime, generator, and random large integer.
	 *
	 * The returned pointer is put onto the cleanup stack.
	 *
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 * @param ax	The DH value, x (a random large integer)
	 * @return A pointer to a CDHKeyPairShim instance
	 * 
	 * @leave KErrArgument	If either aG or ax are out of bounds 
	 */
	static CDHKeyPairShim* NewLC(RInteger& aN, RInteger& aG, RInteger& ax);

	/** The destructor frees all resources owned by the object, prior to its destruction. */
	~CDHKeyPairShim(void);
protected:
	/** Default constructor */
	CDHKeyPairShim(void);

	/** 
	 * Constructor
	 *
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 */
	void ConstructL(RInteger& aN, RInteger& aG);

	/** 
	 * Constructor
	 *
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 * @param ax	The DH value, x (a random large integer)
	 */
	void ConstructL(RInteger& aN, RInteger& aG, RInteger& ax);
	
	/**
	Creates the DH keypair from the given parameters
	@param aN	The DH parameter, n (a large prime)
	@param aG	The DH parameter, g (the generator)
	@param ax	The DH value, x (a random large integer)
	@param xIncluded if the x is included or not.
	*/
	void KeyConstructorL(RInteger& aN, RInteger& aG, RInteger& ax, TBool xIncluded);
	
private:
	CDHKeyPairShim(const CDHKeyPairShim&);
	CDHKeyPairShim& operator=(const CDHKeyPairShim&);
	};

#endif	//	__DHKEYPAIRSHIM_H__
