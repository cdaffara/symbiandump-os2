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
* Implements CKeyStoreSession
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CKEYSTORESESSION_H__
#define __CKEYSTORESESSION_H__

#include "fsserver.h"

class CKeyStoreConduit;
class CPassphraseManager;
class COpenedKey;

/**
 * Encapsulates an opened key object and its handle.
 */
struct TObjectIndex
	{
	TInt iHandle;
	COpenedKey* iObject;
	};

/**
 * A keystore session.
 *
 * Handles client requests by passing them to the keystore conduit.  Keeps track
 * of uids/passphrases for the session.
 */

class CKeyStoreSession : public CTokenServerSession
	{
public:
	/**
	 * Create a new session object.
	 * @param aConduit The keystore conduit used to service user requests.
	 * @param aPassMan The passphrase manager object - this method takes ownership.
	 */
	static CKeyStoreSession* NewL(CFSKeyStoreServer& aServer, CPassphraseManager* aPassMan);
	virtual ~CKeyStoreSession();
	CPassphraseManager& PassphraseManager();
	TInt AddOpenedKeyL(COpenedKey& aObject);
	void RemoveOpenedKeyL(TInt aHandle);
	COpenedKey* OpenedKey(TInt aHandle);

	/**
	 * Determine if this session has a key open.
	 * @param aHandle The key data handle - not the session's open key handle!
	 */
	TBool HasOpenKey(TInt aHandle);
	
	CKeyStoreSession(CFSKeyStoreServer& aServer, CPassphraseManager* aPassMan);
private:
	virtual void DoServiceL(const RMessage2& aMessage);
private:
	CFSKeyStoreServer& iServer;
	CPassphraseManager* iPassphraseMan;
	TInt iLastHandle;
	RArray<TObjectIndex> iOpenedKeys;
	};

#endif
