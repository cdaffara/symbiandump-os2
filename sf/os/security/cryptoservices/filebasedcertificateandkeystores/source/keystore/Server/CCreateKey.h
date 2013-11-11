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
* Implements CKeyCreator  
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CCREATEKEYASYNC_H__
#define __CCREATEKEYASYNC_H__

#include "fsdatatypes.h"
#include <e32std.h>
#include <mctkeystore.h>
#include <asymmetrickeys.h>
#include <bigint.h>

class CSimpleDHKey;
class CKeyCreatorData;

//! Server side
//!	Class to make key creation "asynchronous" by spinning off a thread
//!	to make the synchronous call to the appropriate key creation function
//!	The thread entry point is a static member of this class, which holds the 
//! thread and thread parameter data too.  When the key has been created, the
//! thread terminates using Rendezvous to notify the main thread.  The created key
//! is returned through the CAsymmetricCipher member of CKeyCreatorData
class CKeyCreator : public CActive
{
public:
	CKeyCreator();
	~CKeyCreator();
public:	//	Spin a thread to create an appropriate key, if successful, left on CleanupStack
	void DoCreateKeyAsync(CKeyInfo::EKeyAlgorithm aAlgorithm, TInt aSize, TRequestStatus& aStatus);
public:
//	JCS this needs improvement when new crypto api is reviewed
	CRSAKeyPair* GetCreatedRSAKey();
	CDSAKeyPair* GetCreatedDSAKey();
	void GetCreatedDHKey(RInteger& aDHKey);
protected:
	void DoCancel();
	void RunL();
	TInt RunError(TInt anError);
private:
	static TInt CreatorThreadEntryPoint(TAny*);
private:
	enum TAction {EIdle, EReadyToCreateKey, ECreatedKey};
	TAction iAction;
private:
	TRequestStatus* iClientStatus;
	RThread iCreatorThread;
private:
	class CKeyCreatorData : public CBase
	{
	public:
		CKeyCreatorData(CKeyInfo::EKeyAlgorithm aAlgorithm, TInt aSize);
		~CKeyCreatorData();
	public:	//	Don't bother hiding the data from myself
		TInt iSize;
		CKeyInfo::EKeyAlgorithm iKeyAlgorithm;
	//	Algorithm identified by iKeyAlgorithm
		
		union CreatedKey
		{
			CRSAKeyPair* iRSAKey;
			CDSAKeyPair* iDSAKey;
			CSimpleDHKey* iDHKey;
		}	iKey;
	};
	
	CKeyCreatorData* iCreateData;
};

#endif	//	__CCREATEKEYASYNC_H__
