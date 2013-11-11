/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
	 * buffer with securely generated random data up to the 
	 * current length, discarding any current contents.
	 *
	 * @param aDest The buffer in to which to write random data.
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
 * Sets a secure random number generator implementation to use
 * for this thread.
 * 
 * @param aRNG The secure random number generator to use.
 *
 */
IMPORT_C void SetThreadRandomL(CRandom* aRNG);

/**
 *
 * Sets a secure random number generator implementation to use
 * for this thread, placing it on the cleanup stack.
 * 
 * @param aRNG The secure random number generator to use.
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
 * Generates cryptographically secure random data, filling
 * the provided buffer up to its current length, discarding
 * any data that it may currently contain.
 *
 * @param aDest The buffer to fill with random data
 *
 */
IMPORT_C void GenerateRandomBytesL(TDes8& aDest);

class CSystemRandom : public CRandom
/**
 *
 * This default secure random number generator uses
 * system state to generate entropy for the generation
 * of cryptographically secure random numbers.
 *
 * @publishedAll
 * @released
 *
 */

	{
public:

	/**
	 *
	 * Constructs a new system random number generator.
	 *
	 * @return A ready-to-use random number generator.
	 */
	IMPORT_C static CSystemRandom* NewL(void);
	
	/**
	 *
	 * Constructs a new system random number generator,
	 * and places it on the cleanup stack.
	 *
	 * @return A ready-to-use random number generator.
	 *
	 */
	IMPORT_C static CSystemRandom* NewLC(void);
	
	/**
	 *
	 * Implements the contract as specified in the base
	 * class, CRandom, filling the buffer supplied with
	 * cryptographically secure random data up to its
	 * current length, discarding its current content.
	 *
	 * @param aDest The buffer to which to write random data
	 *
	 */
	virtual void GenerateBytesL(TDes8& aDest);
private:
	CSystemRandom(void);
	CSystemRandom(const CSystemRandom&);
	CSystemRandom& operator=(const CSystemRandom&);
	};

class TRandom
/**
 *
 * The user interface to the system cryptographically 
 * secure random number generator.
 *
 * @publishedAll
 * @released
 */
	{
public:

	/**
	 * 
	 * Fills the provided buffer with secure random data up to its
	 * current length, discarding any current content.
	 *
	 * @param aDestination The buffer in which to write the random data.
	 * @deprecated Use RandomL() instead
	 * @panic This function can panic under low memory conditions
	 *
	 */
	IMPORT_C static void Random(TDes8& aDestination);

	/**
	 * 
	 * Fills the provided buffer with secure random data up to its
	 * current length, discarding any current content.
	 *
	 * @param aDestination The buffer in which to write the random data.
	 * @leave This function can leave under low memory conditions
	 *
	 */
	IMPORT_C static void RandomL(TDes8& aDestination);
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
	 * Fills the provided buffer with secure random data up to its
	 * current length, discarding any current content.
	 *
	 * @param aDestination The buffer in to which to write the random data 
	 *
	 */
	IMPORT_C TInt GetRandom(TDes8& aDestination);
	
	/**
	 *
	 * Opens a new session with the random number server.
	 *
	 */
	IMPORT_C void ConnectL(void);
	};

#endif // __RANDOM_H__
