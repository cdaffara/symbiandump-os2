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

#ifndef __MCTTOKEN_H__
#define __MCTTOKEN_H__

#include <ct/tcttokenobjecthandle.h>
#include <ct/mcttokenobject.h>
#include <ct/tcttokenhandle.h>
#include <ct/mcttokentype.h>

class MCTTokenInterface;


/** 
 * The base class for a token.
 * 
 * Token types must implement this class. It is created from a MCTTokenType using 
 * MCTTokenType::OpenToken().
 * 
 * A token represents one instance of a particular kind of cryptographic module; 
 * for instance, on a device with two WIMs, each WIM is a token. The token contains 
 * several interfaces, representing particular kinds of functions supported by 
 * the token (e.g. certificate management, key management, etc.) 
 * 
 * @since v7.0 
 */
class MCTToken
	{
 public:
  //Functions for opening an interface. 
  //The base class implements the reference counting then calls the corresponding pure virtual Do... methods	
	/** 
	 * Gets a token interface, or NULL if it's not supported by this token.
	 *
	 * This is an asynchronous request.
	 *
	 * @param aRequiredInterface	The UID of the interface that should be returned.
	 * @param aReturnedInterface	On success, this will be set to a pointer to the 
	 * 								returned interface.
	 * @param aStatus				The request status object; contains the result of the 
	 * 								GetInterface() request when complete. Set to KErrCancel 
	 * 								if an outstanding request is cancelled. 
	 */
	IMPORT_C void GetInterface(TUid aRequiredInterface,
							  MCTTokenInterface*& aReturnedInterface, 
							  TRequestStatus& aStatus);

	/** 
	 * Cancels an asynchronous GetInterface() operation.
	 *
	 * The operation completes with KErrCancel. 
	 */
	IMPORT_C void CancelGetInterface();
	
	/**
	 * Allows the client to add a reference to the token (for
	 * example, when a reference to a token is copied elsewhere).  
	 * 
	 * Does not need to be called if token is referenced through OpenToken().
	 */
	inline void AddRef();
	
	/** 
	 * Destroys the object.
	 *	
	 * The interface should be destroyed via this method as the destructor is protected.
	 *
	 * The token implements reference counting, with one count
	 * for itself and one for every opened interface. Once the count
	 * reaches 0, it releases its count with the token type.
	 */
	IMPORT_C void Release();

	// Notification of token removal. The base class assumes the token is not removable, and so does nothing. Removable tokens must implement these functions.
	/** 
	 * Notifies the client when the token has been removed.
	 *	
	 * The base class assumes the token is not removable, and so does nothing. Removable 
	 * tokens must implement these functions.
	 *
	 * This is an asynchronous request.	
	 *
	 * @param aStatus	The request status object; contains the result of the 
	 * 					NotifyOnRemoval() request when complete. Set to KErrCancel 
	 * 					if an outstanding request is cancelled. 
	 */
	IMPORT_C virtual void NotifyOnRemoval(TRequestStatus& aStatus);

	/** 
	 * Cancels an asynchronous NotifyOnRemoval() operation.
	 *
	 * The operation completes with KErrCancel. 
	 */
	IMPORT_C virtual void CancelNotify();
	

	/** 
	 * Gets the associated token type.	
	 *
	 * @return	The associated token type. 
	 */
	virtual MCTTokenType& TokenType() = 0;

	/** 
	 * Gets a label for the token.
	 *
	 * This should be the same as the descriptor returned by MCTTokenType::List().
	 *
	 * @return	The label to the token type. 
	 */
	virtual const TDesC& Label() = 0;

	/** Available information strings for the token. */
	enum TTokenInformation
		{
		/** Version */
		EVersion,
		/** Serial number */
		ESerialNo,
		/** Manufacturer */
		EManufacturer
		};
	
	/** 
	 * Gets the token's handle.
	 *
	 * This can be used to identify a token to another process.
	 *
	 * See the documentation of TCTTokenHandle for an explanation of the use of token 
	 * handles.
	 *
	 * @return	The handle of the token. 
	 */
	virtual TCTTokenHandle Handle() = 0;

 protected:
 	 /** 
 	  * The destructor is protected so that users of the interface
	  * have to use the Release() function. 
	  */
	inline virtual ~MCTToken() = 0;

	// Helper functions for the reference counting
	/** 
	 * Destroys the token object.
	 *
	 * This function is called when Release() has determined that the object is ready 
	 * to be destroyed.
	 *
	 * The default implementation just does a 'delete this', but classes can override 
	 * that if they want different behaviour.
	 *
	 * It should destroy the token; it MUST NOT release the token type, as Release() 
	 * will do that. 
	 */
	IMPORT_C virtual void DoRelease();

	/** 
	 * Gets a reference to the variable used as a reference counter.
	 *
	 * The implementer should initialise this to 0. The value of the reference count 
	 * should be treated as opaque by the implementer.
	 *
	 * @return	A reference to the variable used as a reference counter. 
	 */
	virtual TInt& ReferenceCount() = 0;
	
	// Implementation of GetInterface functionality */
			
	/** 
	 * Implementation for getting a token interface.
	 *
	 * This is called by GetInterface().
	 *
	 * This is an asynchronous request.
	 *
	 * @see GetInterface
	 * @param aRequiredInterface	The UID of the interface that should be returned.
	 * @param aReturnedInterface	On success, this will be set to a pointer to the 
	 * 								returned interface.
	 * @param aStatus				The request status object; contains the result of 
	 * 								the GetInterface() request when complete. Set to 
	 * 								KErrCancel if an outstanding request is cancelled. 
	 */
	virtual void DoGetInterface(TUid aRequiredInterface,
							  MCTTokenInterface*& aReturnedInterface, 
							  TRequestStatus& aStatus) = 0;
	/** 
	 * Implements an asynchronous CancelGetInterface() request.
	 * 
	 * @see	CancelGetInterface
	 * @return	ETrue if there is an token interface running; EFalse, otherwise. 
	 */
	virtual TBool DoCancelGetInterface() = 0;
	
 private:
	// Used by the token object to increment the reference count
	void ObjectCreated();
	// Needed to allow MCTTokenObject to increment the reference count
	// of the token
	friend class MCTTokenObject;
 public:
	/**
	 * Gets the specified information string about the token.
	 * The default implementation returns an empty descriptor.
	 */
	virtual const TDesC& Information(TTokenInformation aRequiredInformation) = 0;
	};

/** 
 * Destructor.
 *
 * Frees all resources owned by the object, prior to its destruction. 
 */
inline MCTToken::~MCTToken()
	{
	}

inline void MCTToken::AddRef()
	{
	++ReferenceCount();
	}

#endif //__MCTTOKEN_H__
