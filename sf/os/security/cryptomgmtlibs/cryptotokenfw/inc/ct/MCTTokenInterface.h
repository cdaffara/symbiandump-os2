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

#ifndef __MCTTOKENINTERFACE_H__
#define __MCTTOKENINTERFACE_H__

class MCTToken;

/** 
 * The base class for all token interfaces.
 *
 * This class provides functionality to interact with the token's reference counting 
 * framework. (Interfaces themselves are not reference counted, but the token 
 * must remain open while it has open interfaces.) 
 *
 * @since v7.0 
 */
class MCTTokenInterface
	{
public:
	/** 
	 * Gets the associated token.
	 * 
	 * @return The associated token. 
	 */
	virtual MCTToken& Token() = 0;

	/** 
	 * Destroys the object.
	 *	
	 * The interface should be destroyed via this method as the destructor is protected.
	 *
	 * Also takes care of reference counting the token. 
	 */
	IMPORT_C void Release();

protected:
	/** 
	 * This function should release the interface and all its owned resources.
	 *
	 * It is called by Release(). The default implementation simply does a 'delete 
	 * this'. If other behaviour is required (for instance to implement a reference 
	 * counting system for the interfaces themselves, the interfaces can override 
	 * this function.) 
	 */
	IMPORT_C virtual void DoRelease();

	/** 
	 * The interface should be destroyed via the Release()
	 * function. Hence, this destructor is protected. 
	 */
	inline virtual ~MCTTokenInterface() = 0;
	};

/** Destructor */
inline MCTTokenInterface::~MCTTokenInterface()
	{
	}

#endif // __MCTTOKENINTERFACE_H__
