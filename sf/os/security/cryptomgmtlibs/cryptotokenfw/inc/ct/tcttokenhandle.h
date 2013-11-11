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

#ifndef __TCTTOKENHANDLE_H__
#define __TCTTOKENHANDLE_H__

#include <e32std.h>


/** 
 * Defines a handle to a subclass of the MCTToken class.
 *
 * The handle identifies the class, not a particular instance of that class. 
 * It is guaranteed to be unique within the context of a specific device (e.g. 
 * mobile phone, etc.).
 * 
 * The handle is especially useful for communication across process boundaries. 
 * For example: a process (A) can open a token (T), get a handle to it and then 
 * give the handle to another process (B). This process (B) can then try to open 
 * the token (T) using that handle. 
 *
 * @since v7.0 
 */
class TCTTokenHandle
	{
public:
	/** 
	 * Creates an invalid handle for the token.
	 *
	 * An invalid handle has a iTokenTypeUid and a iTokenId equal to 0. 
	 */
	IMPORT_C TCTTokenHandle();

	/** 
	 * Creates a handle for the token.
	 *
	 * @param aTokenTypeUid	The value of the TokenType Uid.
	 * @param aTokenId		The identity of the Token. 
	 */
	IMPORT_C TCTTokenHandle(TUid aTokenTypeUid, TInt aTokenId);
	
	/** The copy constructor.
	 *
	 * @param aTokenHandle	The Token Handle object to copy 
	 */
	IMPORT_C TCTTokenHandle(const TCTTokenHandle& aTokenHandle);

	/** 
	 * Equality operator.
	 *
	 * Tests whether this Token Handle object is equal to the specified 
	 * Token Handle object.	
	 *
	 * @param aTokenHandle	The Token Handle object to be compared.
	 * @return				ETrue, if this Token Handle object is equal to 
	 * 						the specified Token Handle object; EFalse otherwise. 
	 */
	IMPORT_C TBool operator ==(const TCTTokenHandle& aTokenHandle) const;
	
	/** 
	 * Inequality operator.
	 *
	 * Tests whether this Token Handle object is not equal to the specified 
	 * Token Handle object. 
	 *
	 * @param aTokenHandle	The Token Handle object to be compared.
	 * @return				ETrue, if this Token Handle object is not equal to
	 *						the specified Token Handle object; EFalse, otherwise.
	 */
	inline TBool operator !=(const TCTTokenHandle& aTokenHandle) const;

public:
	/** 
	 * The UID of the TokenType.
	 *
	 * This identifies to which TokenType the Token belongs. 
	 */
	TUid iTokenTypeUid;
	
	/** 
	 * The identity of the Token.
	 *
	 * This Id is unique within the context of a TokenType. 
	 */
	TInt iTokenId;
	};

inline TBool TCTTokenHandle::operator !=(const TCTTokenHandle& aTokenHandle) const
	{
	return !(*this == aTokenHandle);
	}

#endif
