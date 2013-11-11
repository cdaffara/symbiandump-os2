/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released    
*/


#ifndef __MCTTOKENOBJECT_H__
#define __MCTTOKENOBJECT_H__

class MCTToken;

/** 
 * The base class for all token objects.
 *
 * Any object representing something managed via a particular token interface 
 * should be derived from this class. It provides certain common attributes that 
 * all objects must have (e.g. Label, Type, Token, Handle) and code to interact 
 * with the token's reference counting code. Token objects themselves are not 
 * reference counted, but the token will not be closed while token objects from 
 * it remain in existence. (Token objects can implement their own reference counting 
 * framework if required.) 
 *
 * @since v7.0 
 */
class MCTTokenObject 
	{
public:
	/** 
	 * Releases the MCTTokenObject object.
	 *
	 * To be called when you have finished with the object. 
	 */
	IMPORT_C void Release();

	/** 
	 * Gets the object's human-readable label.	
	 *
	 * @return	A human-readable label of the object. 
	 */
	virtual const TDesC& Label() const = 0;
	
	/** 
	 * Gets a reference to the associated token.
	 *
	 * @return	The associated token. 
	 */
	virtual MCTToken& Token() const = 0;

	/** 
	 * Gets the UID representing the type of the token object.
	 *
	 * The meanings of possible UIDs should be documented in the documentation for 
	 * the interface that returns them.
	 *
	 * @return	The UID representing the type of the token object. 
	 */
	virtual TUid Type() const = 0;

	/** 
	 * Gets a handle for the object.
	 *
	 * The primary purpose of the handle is to allow token objects to be 'passed' 
	 * between processes. 
	 *
	 * @see TCTTokenObjectHandle for more details. 
	 * @return	The handle of the Token Object. 
	 */
	virtual TCTTokenObjectHandle Handle() const = 0;
 protected:
	/** 
	 * Releases the object once the base-class framework work has been done.
	 * 
	 * The default implementation simply does a 'delete this', but derived classes 
	 * can substitute their own behaviour; for instance, to implement reference counting 
	 * of the token objects themselves. 
	 */
	IMPORT_C virtual void DoRelease();

	/**
	 * Constructor. This needs a token in order to increment the
	 * token's reference count. 
	 * 
	 * @param aToken	The associated token.
	 */
	IMPORT_C MCTTokenObject(MCTToken& aToken);

	/** This destructor is protected as clients should use Release() instead. 
	*/
	inline virtual ~MCTTokenObject() = 0;

	/** 
	 * Increments the token's reference count by one.  
	 * 
	 * This is neccessary to allow derived classes to implement their own 
	 * reference counting, as Release() automatically call Release() on the token. 
	 */
	IMPORT_C void AddTokenRef();
	};

/** 
 * Destructor.
 *
 * Frees all resources owned by the object, prior to its destruction. 
 */
inline MCTTokenObject::~MCTTokenObject()
	{
	}

#endif
