/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* (c) 1999-2003 Symbian Ltd
*
*/




/**
 @file
 @publishedAll
 @released
*/

#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <e32base.h>

class CRandom : public CBase
/**
 * @publishedAll
 * @released
 */
	{
public:

	/**
	 * Implementations of this method should fill the passed
	 * buffer with the generated pseudo-random data up to the
	 * current length, discarding any current contents. The 
	 * implementations should leave with KErrNotSecure when 
	 * the generated random data is not secure enough. 
	 *
	 * @param aDest The buffer to fill with random data
	 * @leave KErrNotSecure Random data generated is not 
	 *        secure enough for crytographic operations
	 *        otherwise, leaves with any other system wide error code.
	 *
	 */
	virtual void GenerateBytesL(TDes8& aDest) = 0;
protected:
	IMPORT_C CRandom(void);
private:
	CRandom(const CRandom&);
	CRandom& operator=(const CRandom&);
	};

/**
 *
 * Sets a pseudo-random number generator implementation to use for this thread.
 * 
 * @param aRNG The pseudo-random number generator to use.
 *
 */
IMPORT_C void SetThreadRandomL(CRandom* aRNG);

/**
 *
 * Sets a pseudo-random number generator implementation to use
 * for this thread, placing it on the cleanup stack.
 * 
 * @param aRNG The pseudo-random number generator to use.
 *
 */
IMPORT_C void SetThreadRandomLC(CRandom* aRNG);

/** @internalAll */
void DeleteThreadRandom(TAny* aPtr);

/**
 *
 * Destroys the currently installed random number generator
 * that is in use for this thread.
 *
 */
IMPORT_C void DestroyThreadRandom(void);

/**
 *
 * Generates pseudo-random data.
 * Fills the provided buffer up to its current length,
 * discarding any data that it may currently contain.
 *
 * @param aDest The buffer to fill with random data
 * @leave KErrNotSecure The random data generated is  
 *        not secure enough for cryptographic operations
 *        otherwise, leaves with any other system wide error codes. 
 *
 */
IMPORT_C void GenerateRandomBytesL(TDes8& aDest);

class CRandomShim;
class CSystemRandom : public CRandom
/**
 *
 * This default pseudo-random number generator uses system state 
 * to generate entropy for the generation of random numbers.
 *
 * @publishedAll
 * @released
 *
 */

	{
public:

	/**
	 *
	 * Constructs a new pseudo-random number generator.
	 *
	 * @return A ready-to-use random number generator.
	 */
	IMPORT_C static CSystemRandom* NewL(void);
	
	/**
	 *
	 * Constructs a new pseudo-random number generator,
	 * and places it on the cleanup stack.
	 *
	 * @return A ready-to-use random number generator.
	 *
	 */
	IMPORT_C static CSystemRandom* NewLC(void);
	
	/**
	 *
	 * Implements the contract as specified in the base class,  CRandom, filling the buffer
	 * supplied with random data  up to its current length, discarding its current content.
	 * It will leave under no memory condition.
	 *
	 * @param aDest The buffer to be filled with random data
	 *        
	 */
	virtual void GenerateBytesL(TDes8& aDest);
	
	~CSystemRandom();
private:
	CSystemRandom(void);
	CSystemRandom(const CSystemRandom&);
	CSystemRandom& operator=(const CSystemRandom&);
	
	void ConstructL();
	
	CRandomShim *iShim;
	};

class TRandom
/**
 *
 * The user interface to the random number generator.
 *
 * @publishedAll
 * @released
 */
	{
public:

	/**
	 * 
	 * Fills the provided buffer with pseudo-random data up to its current length, 
	 * discarding any current content.
	 *
	 * This method will not return secure random numbers for some time after the phone boot-up. Because,
	 * pseudo-random number generator will take some time to attain a secure state by collecting enough 
	 * entropy samples after the boot-up. Till that time, the pseudo-random numbers generated may not be
	 * cryptographically secure and there is no way to get to know about it with this API. 
	 * So, if explcit notification on the strength of the random numbers is necessary, use TRandom::SecureRandomL.
	 *
	 * @param aDestination The buffer in which to write the random data.
	 * @deprecated Use RandomL() instead
	 * @panic This function can panic under low memory conditions
	 *
	 */
	IMPORT_C static void Random(TDes8& aDestination);

	/**	
	 * 
	 * Fills the provided buffer with pseudo-random data up to its current length,
	 * discarding any current content.
	 *
	 * This method will not return secure random numbers for some time after the phone boot-up. Because,
     * pseudo-random number generator will take some time to attain a secure state by collecting enough 
     * entropy samples after the boot-up. Till that time, the pseudo-random numbers generated may not be
     * cryptographically secure and there is no way to get to know about it with this API. 
     * So, if explcit notification on the strength of the random numbers is necessary, use TRandom::SecureRandomL.
	 *
	 * @param aDestination The buffer in which to write the random data.
	 * @leave This function can leave under low memory conditions
	 *
	 */
	IMPORT_C static void RandomL(TDes8& aDestination);
	
	/**
	 * 
	 * Fills the provided buffer with the pseudo-random data up to its current length, discarding any current
	 * content of the descriptor. When this method returns normally (with out leave), the system state is secure
	 * and hence the random numbers generated are cryptographically secure as well. When this method leaves with
	 * the error code KErrNotSecure, the system internal state is not secure and hence the random numbers too.
	 * 
	 * Though this method leaves when the system internal state is not secure, still the descriptor will be filled 
	 * with pseudo-random bytes. This random data may or may not be secure enough. Recommended to treat these numbers 
	 * as not secure.
	 *
	 * @param aDestination The buffer in which to write the random data.
	 * @leave KErrNotSecure The generated random numbers is not secure enough for cryptographic operations.
	 *        Otherwise, leaves with some other system wide error codes.
	 *
	 */
	IMPORT_C static void SecureRandomL(TDes8& aDestination);
	};

class RRandomSession:public RSessionBase
/**
 *
 * The client interface to the system random number generator. End
 * users should use TRandom instead of this interface.
 *
 * @publishedAll
 * @released
 */
	{
public:

	IMPORT_C RRandomSession(void);
	
	/**
	 * 
	 * Fills the provided buffer with pseudo-random data up to its
	 * current length, discarding any current content.
	 *
	 * @param aDestination The buffer in to which to write the random data 
	 *
	 */
	IMPORT_C TInt GetRandom(TDes8& aDestination);
	
	/**
	 *
	 * Opens a new session with the random number generator.
	 *
	 */
	IMPORT_C void ConnectL(void);
	};

#endif // __RANDOM_H__
