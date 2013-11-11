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


#include "CKeyStoreSession.h"
#include "cfskeystoreserver.h"
#include "keystorepassphrase.h"
#include "OpenedKeys.h"

CKeyStoreSession* CKeyStoreSession::NewL(CFSKeyStoreServer& aServer, CPassphraseManager* aPassMan)
	{
	return new (ELeave) CKeyStoreSession(aServer, aPassMan);
	}

CKeyStoreSession::CKeyStoreSession(CFSKeyStoreServer& aServer, CPassphraseManager* aPassMan)
	: iServer(aServer), iPassphraseMan(aPassMan)
	{
	}

CKeyStoreSession::~CKeyStoreSession()
	{
	iServer.RemoveSession(*this);
	delete iPassphraseMan;

	for (TInt i = 0 ; i < iOpenedKeys.Count() ; ++i)
		{
		COpenedKey* object = iOpenedKeys[i].iObject;
		delete object;
		}
	iOpenedKeys.Close();
	}

CPassphraseManager& CKeyStoreSession::PassphraseManager()
	{
	return *iPassphraseMan;
	}

void CKeyStoreSession::DoServiceL(const RMessage2& aMessage)
	{
	iPassphraseMan->ExpireCacheL();
	iServer.ServiceRequestL(aMessage, *this);
	}

TInt CKeyStoreSession::AddOpenedKeyL(COpenedKey& aObject)
	{
	TObjectIndex oi;
	oi.iObject = &aObject;
	oi.iHandle = ++iLastHandle;
	User::LeaveIfError(iOpenedKeys.InsertInSignedKeyOrder(oi));
	return oi.iHandle;
	}

void CKeyStoreSession::RemoveOpenedKeyL(TInt aHandle)
	{
	TObjectIndex oi;
	oi.iHandle = aHandle;
	TInt pos = iOpenedKeys.Find(oi);
	User::LeaveIfError(pos);
	delete iOpenedKeys[pos].iObject;
	iOpenedKeys.Remove(pos);	
	}

COpenedKey* CKeyStoreSession::OpenedKey(TInt aHandle)
	{
	TObjectIndex oi;
	oi.iHandle = aHandle;
	TInt pos = iOpenedKeys.Find(oi);
	return (pos != KErrNotFound) ? iOpenedKeys[pos].iObject : NULL;
	}

TBool CKeyStoreSession::HasOpenKey(TInt aHandle)
	{
	TBool result = EFalse;
	for (TInt i = 0 ; i < iOpenedKeys.Count() ; ++i)
		{
		COpenedKey* object = iOpenedKeys[i].iObject;
		if (object->Handle() == aHandle)
			{
			result = ETrue;
			break;
			}
		}
	return result;
	}
