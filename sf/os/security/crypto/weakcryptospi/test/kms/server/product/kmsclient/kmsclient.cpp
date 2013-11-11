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
* Implements client API to the key management server.
*
*/


/**
 @file
*/

#include <e32uid.h>
#include "kmsclient.h"

#include "kmsservercommon.h"
#include "kmslddcommon.h"

using namespace KeyMgmtSvrImpl;


static TInt StartServerProcess()
/**
	Start the process which hosts the key management server.

	@return					Symbian OS error code.  KErrNone indicates success,
							and any other value indicates failure.
 */
	{
	RProcess sProc;
	_LIT(KServerImgName, "kmsserver.exe");
	const TUidType serverUid(KExecutableImageUid, KNullUid, KmsLddImpl::KKmsServerUid);
	RProcess pr;
	TInt r = pr.Create(KServerImgName, /* aCommand */ KNullDesC, serverUid);
	
	if (r != KErrNone)
		return r;
	
	TRequestStatus rs;
	pr.Rendezvous(rs);
	if (rs != KRequestPending)
		return rs.Int();
	
	pr.Resume();
	User::WaitForRequest(rs);
	
	r = (pr.ExitType() == EExitPanic) ? KErrGeneral : rs.Int();
	pr.Close();
	return r;
	}

EXPORT_C TInt RKeyMgmtSession::Connect()
/**
	Connect to the key managment server using the version with which this
	client API was built.

	@return					Symbian OS error code.  KErrNone indicates success,
							and any other value indicates failure.
 */
	{
	const TVersion v = KeyMgmtSvrImpl::Version();
	return Connect(v);
	}

EXPORT_C TInt RKeyMgmtSession::Connect(const TVersion& aReqVer)
/**
	Connect to the key managment server using the supplied version.

	@param	aReqVer			The server must be at least this version.
	@return					Symbian OS error code.  KErrNone indicates success,
							and any other value indicates failure.
 */
	{
	TInt retries = 2;		// number of remaining retries
	
	for (;;)
		{
		TInt r = CreateSession(KKeyMgmtSrvName, aReqVer);
		
		// if successful connection then finished
		if (r == KErrNone)
			return r;
		
		// if any reason other than server not available then retry
		if (r != KErrNotFound && r != KErrServerTerminated)
			return r;
		
		if (--retries == 0)
			return r;

		r = StartServerProcess();
		if (r != KErrNone && r != KErrAlreadyExists)
			return r;
		}	// for (;;)
	}

EXPORT_C TInt RKeyMgmtSession::GenerateKey(TInt aLength, TKeyHandle& aHandle)
/**
	Create a new key in the hardware store.

	@param	aLength			Key length in bytes.
	@param	aHandle			On success, this is set to the new key's handle.
							It's value is undefined on failure.
	@return					Symbian OS error code.  KErrNone indicates success,
							and any other value indicates failure.
 */
	{
	TPckg<TKeyHandle> hPckg(aHandle);
	TIpcArgs ipc(aLength, &hPckg);
	return SendReceive(EGenerateKey, ipc);
	}

EXPORT_C TInt RKeyMgmtSession::StoreKey(const TDesC8& aData, TKeyHandle& aHandle)
/**
	Store existing key data in the hardware store.
	
	@param	aData			Data to store in hardware.
	@param	aHandle			On success, this is set to the new key's handle.
							It's value is undefined on failure.
	@return					Symbian OS error code.  KErrNone indicates success,
							and any other value indicates failure.
 */
	{
	TPckg<TKeyHandle> hPckg(aHandle);
	TIpcArgs ipc(&aData, &hPckg);
	return SendReceive(EStoreKey, ipc);
	}

EXPORT_C TInt RKeyMgmtSession::DeleteKey(TKeyHandle aHandle)
/**
	Delete a key from the hardware store.

	@param	aHandle			Identifies existing key created with GenerateKey
							or StoreKey.
	@return					Symbian OS error code.  KErrNone indicates success,
							and any other value indicates failure.
 */
	{
	TIpcArgs ipc(aHandle.iValue);
	return SendReceive(EDeleteKey, ipc);
	}

EXPORT_C TInt RKeyMgmtSession::AddUsage(TKeyHandle aHandle, TInt aOperation, const TSecurityPolicy& aSecPol)
/**
	Add a usage to the hardware store.  If a usage is already defined for the supplied
	key and oepration, then it is overwritten.

	@param	aHandle			Key for which to add usage.
	@param	aOperation		Numeric operation identifier.
	@param	aSecPol			Security policy to apply to clients which want to perform
							the supplied operation on the supplied key.
	@return					Symbian OS error code.  KErrNone indicates success,
							and any other value indicates failure.
	@see DeleteUsage
 */
	{
	TPckg<TSecurityPolicy> secPolPckg(aSecPol);
	TIpcArgs ipc(aHandle.iValue, aOperation, &secPolPckg);
	return SendReceive(EAddUsage, ipc);
	}

EXPORT_C TInt RKeyMgmtSession::DeleteUsage(TKeyHandle aHandle, TInt aOperation)
/**
	Remove a usage from the hardware store.

	@param	aHandle			Key for which to add usage.
	@param	aOperation		Numeric operation identifier.
	@return					Symbian OS error code.  KErrNone indicates success,
							and any other value indicates failure.
	@see AddUsage
 */
	{
	TIpcArgs ipc(aHandle.iValue, aOperation);
	return SendReceive(EDeleteUsage, ipc);
	}

EXPORT_C void RKeyMgmtSession::SetHeapFail(TInt aValue)
/**
	Ask the KMS to fail its heap allocation.  This function is empty
	in release builds.
	
	@param	aValue			The deterministic rate of failure.  I.e.,
							fail on every aValue'th allocation where 1
							is the next allocation.
	@see EndHeapFail
 */
	{
#ifndef _DEBUG
	(void) aValue;
#else
	TIpcArgs ipc(aValue);
	SendReceive(ESetHeapFail, ipc);
#endif
	}

EXPORT_C void RKeyMgmtSession::EndHeapFail()
/**
	Cancel the heap allocation failure set up with SetHeapFail.
	This function is empty in release builds.
	
	@see SetHeapFail
 */
	{
#ifdef _DEBUG
	SendReceive(EEndHeapFail);
#endif
	}
