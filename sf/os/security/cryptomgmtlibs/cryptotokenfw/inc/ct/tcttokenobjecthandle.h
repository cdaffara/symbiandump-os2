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

#ifndef __TCTTOKENOBJECTHANDLE_H__
#define __TCTTOKENOBJECTHANDLE_H__

#include <e32std.h>
#include <ct/tcttokenhandle.h>


/** 
 * Defines a handle to an object represented by a class which is a subclass of 
 * the MCTTokenObject class.
 * 
 * The handle identifies the object, not the instance of the subclass of MCTTokenObject 
 * used to manage that object. It is guaranteed to be unique within the context 
 * of a specific device (e.g. mobile phone, etc.) at a specific point in time. 
 * This limitation in time is due to the fact that objects can be created and 
 * destroyed. We cannot guarantee that a handle will still point to the same 
 * object after we add or remove one or more objects from the store from which 
 * the object comes. However, it is guaranteed that the addition of objects to 
 * the store will not affect the validity of the handles that were obtained previously. 
 * For instance, if the object is a certificate in a certificate store, the subclass 
 * of MCTTokenObject will be CCTCertInfo and the handle will identify the certificate 
 * in the certificate store, not the instance of CCTCertInfo.
 * 
 * The handle is especially useful for communication across process boundaries. 
 * For example: a process (A) can get a handle to an object, and then give the handle 
 * to another process (B). This process (B) can then try to get the same object using that handle. 
 *
 * @since v7.0 
 */
class TCTTokenObjectHandle
	{
public:
	/** 
	 * Creates an invalid handle.
	 *
	 * An invalid handle has an invalid iTokenHandle and an iObjectId equal to 0. 
	 */
	IMPORT_C TCTTokenObjectHandle();
	
	/** 
	 * Creates a handle.
	 *
	 * @param aTokenHandle	The handle that identifies the Token where the object 
	 * 						is stored.
	 * @param aObjectId		The identifier of the object within the Token. 
	 */
	IMPORT_C TCTTokenObjectHandle(TCTTokenHandle aTokenHandle, TInt aObjectId);
	
	/** 
	 * Copy Constructor.
	 *
	 * @param aTokenHandle	The handle to copy. 
	 */
	IMPORT_C TCTTokenObjectHandle(const TCTTokenObjectHandle& aTokenHandle);
	
	/** 
	 * Equality operator.
	 *
	 * Tests whether this TCTTokenObjectHandle object is equal to the specified 
	 * TCTTokenObjectHandle object.	
	 *
	 * @param aTokenHandle	The TCTTokenObjectHandle object to be compared.
	 * @return				ETrue, if thisTCTTokenObjectHandle object is equal to 
	 * 						the specified TCTTokenObjectHandle object; EFalse otherwise. 
	 */
	IMPORT_C TBool operator ==(const TCTTokenObjectHandle& aTokenHandle) const;
	
	/** Inequality operator */
	inline TBool operator !=(const TCTTokenObjectHandle& aTokenHandle) const;
 
 public:
	/** The handle that identifies the Token where the object is stored. */
	TCTTokenHandle iTokenHandle;
	
	/** 
	 * The identifier of the object within the Token.
	 *
	 * This is unique within the context of a particular Token and only at a specific 
	 * point in time. 
	 */
	TInt iObjectId;
	};

inline TBool TCTTokenObjectHandle::operator !=(const TCTTokenObjectHandle& aHandle) const
	{
	return !(*this == aHandle);
	}

#endif
