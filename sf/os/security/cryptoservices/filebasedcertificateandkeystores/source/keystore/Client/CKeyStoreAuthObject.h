/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Keystore client implementation of MCTAuthenticationObject.
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CKEYSTOREAUTHOBJECT_H__
#define __CKEYSTOREAUTHOBJECT_H__

#include <mctauthobject.h>
#include <ct/mcttokenobject.h>

class CFSKeyStoreClient;

/**
 * Keystore client implementation of MCTAuthenticationObject.
 *
 * The software keystore has a couple of anomalys than mean it doesn't map well
 * to the concept of authentication objects:
 *
 *   1. There is only one passphrase
 *   2. Timeout is set globally
 *
 * The operations defined by the auth object interface relate to single keys,
 * but when applied to the software keystore, they will affect other keys as
 * well.  Thus:
 *
 *   1. setting the passphrase for any key will set the passphrase for the entire
 *      keystore
 *   2. setting the timeout will set the timeout for the entire keystoer
 *
 * For the software keystore, each auth object is like a view onto the
 * authentication mechanism for the current process.  One protector objects is
 * used to protect all keys used by a single process.  Thus a single instance of
 * this object is owned by the client object.  Because the mutual dependency
 * between the auth object and the client object there is no separate reference
 * counting for this object - the AddRef and Release methods just call the same
 * methods on the client object.
 */

NONSHARABLE_CLASS(CKeyStoreAuthObject) : protected CBase, public MCTAuthenticationObject
	{
 public:

	/**
	 * Create a new object.
	 */
	static CKeyStoreAuthObject* NewL(CFSKeyStoreClient& aClient);

	/**
	 * Increment this object's reference count.
	 */
	void AddRef();
	
 private:
 	CKeyStoreAuthObject(CFSKeyStoreClient& aClient);

	// From MCTTokenObject
	
	/**
	 * Returns the object's human-readable label.
	 */
	virtual const TDesC& Label() const;
	
	/**
	 * Returns a reference to the associated token.
	 */
	virtual MCTToken& Token() const;

	/**
	 * Gets the UID representing the type of the token object.
	 */
	virtual TUid Type() const;

	/**
	 * Gets a handle for the object.
	 */
	virtual TCTTokenObjectHandle Handle() const;

	/**
	 * Called by MCTTokenObject::Release()
	 */
	virtual void DoRelease(void);
	
	// From MCTAuthenticationObject

	/**
	 * Lists all keys useable or manageable by the calling process.
	 */
	virtual void ListProtectedObjects(RMPointerArray<MCTTokenObject>& aObjects, TRequestStatus& aStatus);
	virtual void CancelListProtectedObjects();

	/**
	 * Prompt the user to change the passphrase for all keys in the keystore.
	 */
	virtual void ChangeReferenceData(TRequestStatus &aStatus);
	virtual void CancelChangeReferenceData();

	/**
	 * Prompt the user to enter the unblocking passphrase and unblock the store.
	 */
	virtual void Unblock(TRequestStatus &aStatus);
	virtual void CancelUnblock();

	/**
	 * Get the status of this auth object.
	 */
	virtual TUint32 Status() const;

	/**
	 * Disabling this authentication object is not allowed, Disable() and
	 * Enable() complete with KErrNotSupported.
	 */
	virtual void Disable(TRequestStatus &aStatus);
	virtual void CancelDisable();
	virtual void Enable(TRequestStatus &aStatus);
	virtual void CancelEnable();

	/**
	 * Prompt the user to enter the passphrase and open the keystore for this
	 * process.
	 */
	virtual void Open(TRequestStatus& aStatus);
	virtual void CancelOpen();

	/**
	 * Close the keystore.
	 */
	virtual void Close(TRequestStatus& aStatus);
	virtual void CancelClose();

	/**
	 * Get time remaining for the keystore.
	 */
	virtual void TimeRemaining(TInt& aStime, TRequestStatus& aStatus);
	virtual void CancelTimeRemaining();

	/**
	 * Set the timeout for the keystore.
	 */
	virtual void SetTimeout(TInt aTime, TRequestStatus& aStatus);
	virtual void CancelSetTimeout();

	/**
	 * Get the current timeout for the keystore.
	 */
	virtual void Timeout(TInt& aTime, TRequestStatus& aStatus);
	virtual void CancelTimeout();
	
 private:
	CFSKeyStoreClient& iClient;
	TInt iRefCount;
	};

#endif
