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
*
*/


#include "CKeyStoreAuthObject.h"
#include "cfskeystoreclient.h"
#include "ct/logger.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
const TUid KKeyStoreAuthObjectUID = {0x101FE681};
#endif

_LIT(KAuthObjectName, "Software key store authentication object");

CKeyStoreAuthObject* CKeyStoreAuthObject::NewL(CFSKeyStoreClient& aClient)
	{
	return new (ELeave) CKeyStoreAuthObject(aClient);
	}

CKeyStoreAuthObject::CKeyStoreAuthObject(CFSKeyStoreClient& aClient) :
	MCTAuthenticationObject(aClient.Token()),
	iClient(aClient),
	iRefCount(0)
	{
	LOG1(_L("keystore auth object created, ref count == %d"), iRefCount);
	}

void CKeyStoreAuthObject::AddRef()
	{
	++iRefCount;

	LOG1(_L("keystore auth object referenced, ref count == %d"), iRefCount);
	LOG_INC_INDENT();

	// The first reference is from the client object: don't reference the token
	// (it gets one reference "for free" when this object is created), or
	// reference the client (this would lead to circular reference)
	if (iRefCount != 1)
		{
		// Need to increment the token's reference count here as well as this is
		// automatically decremented by Release()
		AddTokenRef();

		// This object depends on the client
		iClient.AddRef();
		}

	LOG_DEC_INDENT();
	}

/**
 * Implementation of reference counting.  The superclass automatically calls
 * Release() on the token for us.
 */
void CKeyStoreAuthObject::DoRelease()
	{
	--iRefCount;

	LOG1(_L("keystore auth object released, ref count == %d"), iRefCount);
	LOG_INC_INDENT();
	
	ASSERT(iRefCount >= 0);

	// The first reference is from the client, and didn't result in a call to
	// iClient.AddRef(), so don't try and release this
	if (iRefCount != 0)
		{
		iClient.Release();
		}

	// No more references, call superclass to delete the object
	if (iRefCount == 0)
		{
		MCTTokenObject::DoRelease();
		}
	
	LOG_DEC_INDENT();
	}

/**
 * Returns the object's human-readable label.
 */
const TDesC& CKeyStoreAuthObject::Label() const
	{
	return KAuthObjectName;
	}
	
/**
 * Returns a reference to the associated token.
 */
MCTToken& CKeyStoreAuthObject::Token() const
	{
	return iClient.Token();
	}

/**
 * Gets the UID representing the type of the token object.
 */
TUid CKeyStoreAuthObject::Type() const
	{
	return KKeyStoreAuthObjectUID;
	}

/**
 * Gets a handle for the object.
 */
TCTTokenObjectHandle CKeyStoreAuthObject::Handle() const
	{
	return TCTTokenObjectHandle(Token().Handle(), 0);
	}

/**
 * Lists all keys useable or manageable by the calling process.
 */
void CKeyStoreAuthObject::ListProtectedObjects(RMPointerArray<MCTTokenObject>& aObjects, TRequestStatus& aStatus)
	{
	iClient.ListProtectedObjects(aObjects, aStatus);
	}

void CKeyStoreAuthObject::CancelListProtectedObjects()
	{
	// Synchronous, no cancel
	}

/**
 * Prompt the user to change the passphrase for all keys in the keystore.
 */
void CKeyStoreAuthObject::ChangeReferenceData(TRequestStatus &aStatus)
	{
	iClient.ChangeReferenceData(aStatus);
	}

void CKeyStoreAuthObject::CancelChangeReferenceData()
	{
	iClient.CancelChangeReferenceData();
	}

/**
 * Prompt the user to enter the unblocking passphrase and unblock the store.
 */
void CKeyStoreAuthObject::Unblock(TRequestStatus &aStatus)
	{
	// Not supported
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CKeyStoreAuthObject::CancelUnblock()
	{
	// not supported, nothing to do
	}

/**
 * Get the status of this auth object.
 */
TUint32 CKeyStoreAuthObject::Status() const
	{
	return iClient.AuthStatus();
	}

/**
 * Disabling this authentication object is not allowed, Disable() and
 * Enable() complete with KErrNotSupported.
 */
void CKeyStoreAuthObject::Disable(TRequestStatus &aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CKeyStoreAuthObject::CancelDisable()
	{
	}

void CKeyStoreAuthObject::Enable(TRequestStatus &aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CKeyStoreAuthObject::CancelEnable()
	{
	}

/**
 * Prompt the user to enter the passphrase and open the keystore for this
 * process.
 */
void CKeyStoreAuthObject::Open(TRequestStatus& aStatus)
	{
	iClient.AuthOpen(aStatus);
	}

void CKeyStoreAuthObject::CancelOpen()
	{
	iClient.CancelAuthOpen();
	}

/**
 * Close the keystore.
 */
void CKeyStoreAuthObject::Close(TRequestStatus& aStatus)
	{
	iClient.AuthClose(aStatus);
	}

void CKeyStoreAuthObject::CancelClose()
	{
	// Synchronous, no cancel
	}

/**
 * Get time remaining for the keystore.
 */
void CKeyStoreAuthObject::TimeRemaining(TInt& aTime, TRequestStatus& aStatus)
	{
	iClient.TimeRemaining(aTime, aStatus);
	}

void CKeyStoreAuthObject::CancelTimeRemaining()
	{
	// Synchronous, no cancel
	}

/**
 * Set the timeout for the keystore.
 */
void CKeyStoreAuthObject::SetTimeout(TInt aTime, TRequestStatus& aStatus)
	{
	iClient.SetTimeout(aTime, aStatus);
	}

void CKeyStoreAuthObject::CancelSetTimeout()
	{
	// Synchronous, no cancel
	}

/**
 * Get the current timeout for the keystore.
 */
void CKeyStoreAuthObject::Timeout(TInt& aTime, TRequestStatus& aStatus)
	{
	iClient.Timeout(aTime, aStatus);
	}

void CKeyStoreAuthObject::CancelTimeout()
	{
	// Synchronous, no cancel
	}
