/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements hardware key store API.
*
*/


/**
 @file
*/

#include <nkern/nk_priv.h>
#include "kmskext.h"

#ifdef _DEBUG
	/** Panic if not running in the key store DFC thread. */
	#define __ASSERT_KEYSTORE_THREAD(_reason)	ConfirmCurrentThread(_reason)
#else
	/**
		In debug builds this panics if not running in the key store DFC thread.
		It does nothing in release builds.
	 */
	#define __ASSERT_KEYSTORE_THREAD(_reason)
#endif

// -------- generic interface to key store --------

EXPORT_C TInt HwKeyStore::GenerateKey(TInt aLength, TSecureId aOwner, TKeyHandle& aHandle)
/**
	Generates key of the supplied length and returns a handle to it.  On hardware, this
	would write the newly-created key data to the secure storage area.
	
	@param	aLength			Length of new key in bytes.
	@param	aOwner			Identifies process which will own this key.
	@param	aHandle			On success this is set to the newly-created key's handle.
							Its value is undefined on failure.
	@return					KErrNone on success, any other error code on failure.
 */
	{
	__ASSERT_KEYSTORE_THREAD(EGKWrongThread);
	
	TKeyHandleImpl& khiRef = reinterpret_cast<TKeyHandleImpl&>(aHandle);
	return KeyStore.GenerateKey(aLength, aOwner, khiRef);
	}

EXPORT_C TInt HwKeyStore::StoreKey(const TDesC8& aData, TSecureId aOwner, TKeyHandle& aHandle)
/**
	Store the supplied key in the hardware store.  On hardware this would write the
	supplied key data to the secure storage area.
	
	@param	aData			Unformatted key data.
	@param	aOwner			Identifies process which will own this key.
	@param	aHandle			On success this is set to the newly-created key's handle.
							Its value is undefined on failure.
	@return					KErrNone on success, any other error code on failure.
 */
	{
	__ASSERT_KEYSTORE_THREAD(ESKWrongThread);
	
	TKeyHandleImpl& khiRef = reinterpret_cast<TKeyHandleImpl&>(aHandle);
	return KeyStore.StoreKey(aData, aOwner, khiRef);
	}

EXPORT_C TInt HwKeyStore::DeleteKey(TSecureId aClaimedOwner, TKeyHandle aHandle)
/**
	Delete the identified key from the secure storage area.  Any usages for this
	key are also removed.
	
	@param	aClaimedOwner	Process which claims to own this key.  Only the process
							which generated or stored a key can delete it.
	@param	aHandle			Identifies existing key in store.
	@return					KErrNone on success, any other error code on failure.
							For example, KErrNotFound is returned if the handle does
							not identify a user-defined or -generated key.
 */
	{
	__ASSERT_KEYSTORE_THREAD(EDKWrongThread);
	
	return KeyStore.DeleteKey(aClaimedOwner, aHandle);
	}

EXPORT_C TInt HwKeyStore::AddUsage(TSecureId aClaimedOwner, TKeyHandle aHandle, TInt aOperation, const TSecurityPolicy& aSecPol)
/**
	Add a usage policy for an existing key.  If a policy is already defined for the key
	and operation then it is overwritten.

	@param	aClaimedOwner	Process which claims to own this key.  Only the process
							which generated or stored a key can add a usage policy.
	@param	aHandle			Identifies existing key in store.
	@param	aOperation		Numeric identifier for operation.  Example operations
							include signing, but the actual meaning is not defined here.
	@param	aSecPol			Security policy to apply.  A process which wants to use the
							identified key for the identified operation must satisfy this
							security policy.  Only one policy can be defined for each
							key, operation pair.
	@return					KErrNone on success, any other error code on failure.
 */
	{
	__ASSERT_KEYSTORE_THREAD(EAUWrongThread);
	
	return KeyStore.AddUsage(aClaimedOwner, aHandle, aOperation, aSecPol);
	}

EXPORT_C TInt HwKeyStore::DeleteUsage(TSecureId aClaimedOwner, TKeyHandle aHandle, TInt aOperation)
/**
	Delete a usage policy for an existing key, operation pair.

	@param	aClaimedOwner	Process which claims to own this key.  Only the process
							which generated or stored a key can delete a usage policy.
	@param	aHandle			Identifies existing key in store.
	@param	aOperation		Numeric identifier for operation.  Example operations
							include signing, but the actual meaning is not defined here.
	@return					KErrNone on success, any other error code on failure.
 */
	{
	__ASSERT_KEYSTORE_THREAD(EDUWrongThread);
	
	return KeyStore.DeleteUsage(aClaimedOwner, aHandle, aOperation);
	}

EXPORT_C TInt HwKeyStore::UsageAllowed(DProcess* aProcess, TKeyHandle aHandle, TInt aOperation, TBool& aAllowed)
/**
	Predicate function tests whether the supplied process matches the usage policy
	for the identified key and operation.

	@param	aProcess		Process which wants to use the key.
	@param	aHandle			Identifies existing key in store.
	@param	aOperation		Numeric identifier for operation.  Example operations
							include signing, but the actual meaning is not defined here.
	@param	aAllowed		On success this is set to whether or not the process can
							perform the requested operation with the key.  Its value
							is undefined on failure.
	@return					KErrNone on success, any other error code on failure.
							In this context, "success" means can determine whether or
							not the operation is allowed.  It does not necessarily mean
							the operation <em>is</em> allowed.
 */
	{
	__ASSERT_KEYSTORE_THREAD(EUAWrongThread);
	
	return KeyStore.UsageAllowed(aProcess, aHandle, aOperation, aAllowed);
	}

class TExtractKeyDfcInfo
/**
	Information passed from requesting thread to DFC running in
	key store DFC queue context.
 */
	{
public:
	DProcess* iProcess;		///< Client of the accelerator LDD which wants to use the key.
	TKeyHandle iHandle;		///< The key which they want to use.
	TInt iOperation;		///< The operation they want to perform with the key.
	HBuf8** iData;			///< Set to heap-based descriptor containing key data.
	TInt iResult;			///< Symbian OS error code result.
	NFastSemaphore* iSem;	///< Semaphore to signal when DFC has completed.
	};

/** Priority of extract DFC on key store DFC queue. */
const TInt KExtractDfcPriority = 0;

EXPORT_C TInt HwKeyStore::ExtractKey(DProcess* aProcess, TKeyHandle aHandle, TInt aOperation, HBuf8*& aData)
/**
	This function is provided for the accelerator LDD, so it can use a key which is
	passed by handle.
	
	@param	aProcess		Client of the accelerator LDD which wants to use the key.
	@param	aHandle			The key which they want to use.
	@param	aOperation		The operation they want to perform with the key.
	@param	aData			On success this points to a heap-based descriptor which
							contains the key data.  Ownership is transferred to the caller.
							Its value is undefined on failure.
	@return					KErrNone on success, any other error code on failure.
 */
	{
	if (NKern::CurrentThread() == KeyStoreDfcQ.iThread)
		return KeyStore.ExtractKey(aProcess, aHandle, aOperation, aData);
	
	NFastSemaphore sem;
	NKern::FSSetOwner(&sem, /* aThread */ 0);	// set current thread to owner
	
	// initial value of iResult not required
	TExtractKeyDfcInfo extrDfcInfo;
	extrDfcInfo.iProcess = aProcess;
	extrDfcInfo.iHandle = aHandle;
	extrDfcInfo.iOperation = aOperation;
	extrDfcInfo.iData = &aData;
	extrDfcInfo.iSem = &sem;
	
	TDfc extrDfc(RunExtractDfc, &extrDfcInfo, &KeyStoreDfcQ, KExtractDfcPriority);
	extrDfc.Enque();
	
	NKern::FSWait(&sem);
	return extrDfcInfo.iResult;
	}

void HwKeyStore::RunExtractDfc(TAny* aPtr)
/**
	This DFC function is called on the key store DFC queue.  It attempts
	to extract the requested key and then signals the calling thread to
	continue.
	
	@param	aPtr			Standard DFC entrypoint argument.  This actually
							points to an instance of TExtractKeyDfcInfo.
 */
	{
	TExtractKeyDfcInfo& extrDfcInfo = *reinterpret_cast<TExtractKeyDfcInfo*>(aPtr);
	
	extrDfcInfo.iResult = KeyStore.ExtractKey(
		extrDfcInfo.iProcess, extrDfcInfo.iHandle, extrDfcInfo.iOperation, *extrDfcInfo.iData);
	
	NKern::FSSignal(extrDfcInfo.iSem);
	}

EXPORT_C TDfcQue& HwKeyStore::DfcQ()
/**
	Return a reference to the thread on which all key store requests must be run.
	A separate thread is provided to serialize requests from multiple clients,
	specifically the KMS LDD and ACC LDD.
	
	@return					Thread on which all key store requests must be run.
 */
	{
	return KeyStoreDfcQ;
	}

// -------- debugging --------

#ifdef _DEBUG

void HwKeyStore::ConfirmCurrentThread(HwKeyStore::TPanic aPanic)
/**
	Confirm whether the current thread is the key store DFC queue.
	If it is not, then panic the current thread with the supplied
	reason.
	
	@param	aPanic			Reason with which to panic the current
							thread if it is not the key store DFC queue.
 */
	{
	NThreadBase* curThread = NKern::CurrentThread();
	if (curThread != KeyStoreDfcQ.iThread)
		Panic(aPanic);
	}

void HwKeyStore::Panic(HwKeyStore::TPanic aPanic)
/**
	This function is called when the key store API (i.e. HwKeyStore function)
	detects an invalid state.  It panicks the current thread with category
	"HwKeyStore" and uses aPanic as the reason.
	
	@param	aPanic			Identifies reason for panic.  This is used as the
							thread's panic reason.
 */
	{
	_LIT(KPanicCat, "HwKeyStore");
	Kern::PanicCurrentThread(KPanicCat, aPanic);
	}

#endif

