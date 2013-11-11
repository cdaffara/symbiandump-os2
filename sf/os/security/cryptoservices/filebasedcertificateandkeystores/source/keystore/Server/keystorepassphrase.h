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


/**
 @file 
 @internalTechnology
*/
 
#ifndef __KEYSTOREPASSPHRASE_H__
#define __KEYSTOREPASSPHRASE_H__

#include "pbe.h"
#include "FSResources.h"
#include <e32std.h>
#include <s32crypt.h>
#include <e32base.h>
#include <secdlg.h>

class CStreamStore;
class CPassphraseManager;
class RWriteStream;

/**
 * Provides static methods to prompt the user for passwords.
 */
class TPasswordManager
	{
 public:
	static void KeystorePassword(TPINValue& aValue, TRequestStatus& aStatus, TBool aRetry = EFalse);
	static void CreatePassword(TPINValue& aValue, TRequestStatus& aStatus);
	static void ImportPassword(TPINValue& aValue, TRequestStatus& aStatus);
	static void ExportPassword(TPINValue& aValue, TRequestStatus& aStatus);
	static void Cancel();
 private:
	static void GetPassword(FSResources::TStrings aLabel, TPINValue& aValue, TRequestStatus& aStatus, TBool aRetry = EFalse);
	};

/**
 * Represents a correct passphrase.
 */

class CPassphrase : public CBase
	{
 public:
	/**
	 * Creates a passphrase object that represents an existing passphrase.
	 */	
	static CPassphrase* NewLC(TInt aTimeout, CStreamStore& aStore,
							  TStreamId aStreamId, const TDesC& aPassphrase);

	/**
	 * Creates a passphrase object that represents an existing passphrase.
	 */	
	static CPassphrase* NewL(TInt aTimeout, CStreamStore& aStore,
							  TStreamId aStreamId, const TDesC& aPassphrase);

	/**
	 * Creates a passphrase object that represents a new passphrase, including
	 * writing the PBE data to a stream. 
	 */	
	static CPassphrase* NewLC(TInt aTimeout, CStreamStore& aStore, 
							  const TDesC& aPassphrase);

	/**
	 * Creates a passphrase object that represents a new passphrase, including
	 * writing the PBE data to a stream. 
	 */	
	static CPassphrase* NewL(TInt aTimeout, CStreamStore& aStore, 
							  const TDesC& aPassphrase);
	
	~CPassphrase();

	/**
	 * Returns an opened secure store.  This can be used to access the encrypted
	 * contents of the store.
	 */
	CStreamStore& Store() const;

	/** Returns the stream id associated with this passphrase. */
	TStreamId StreamId() const;

	/** Get the number of seconds before this passphrase expires, or -1 if it never does. */
	TInt TimeRemainingL(const TTime& aCurrentTime) const;

	/** Change the passphrase. */
	void ChangePassphraseL(const TDesC& aNewPassphrase);

 private:
	CPassphrase(CStreamStore& aStore, TInt aTimeout);
	void ConstructL(TStreamId aStreamId, const TDesC& aPassphrase);
	void ConstructL(const TDesC& aPassphrase);
	static void RevertStore(TAny* aStore);
 private:
	TStreamId iStreamId;
	CStreamStore& iStore;
	CPBEncryptSet* iSecurity;
	CSecureStore* iSecureStore;
	TBool iExpires;
	TTime iExpiryTime;
	};

class CPassphraseManager : public CActive
	{
 public:
	static CPassphraseManager* NewL(CStreamStore& aStore);
	~CPassphraseManager();

	/**
	 * Creates a new passphrase by asking the user.
	 *
	 * @param aStreamId This is set to the id of a newly created stream that
	 * holds the PBE data for the passphrase.
	 * @param aPassphrase This is set to the created passphrase object.
	 */
	void CreatePassphrase(TInt aTimeout, CPassphrase*& aPassphrase,
						  TRequestStatus& aStatus);
	
	/**
	 * Gets a particular passphrase, identified by a stream id.
	 * 
	 * If the passphrase is cached, it is simply returned, otherwise the user is
	 * asked.  The stream id references the stream that holds the PBE data
	 * structures associated with the passphrase.
	 */
	void GetPassphrase(TStreamId aStreamId, TInt aTimeout,
					   CPassphrase*& aPassphrase, TRequestStatus& aStatus);

	/**
	 * Change an existing passphrase.
	 *
	 * The user is prompted for the current and new passphrases.  If the current
	 * passphrase is entered correctly, the encrypted master key is updated in
	 * the store.
	 */
	void ChangePassphrase(TStreamId aStreamId, TRequestStatus& aStatus);

	/**
	 * Remove any cached passphrases that have expired.
	 */
	void ExpireCacheL();

	/**
	 * Remove instances of the specified passphrases from the cache.
	 * @param aStreamId The passphrase to remove, or KNullStreamId for all
	 */
	void RemoveCachedPassphrases(TStreamId aStreamId);

	/**
	 * Get the time a specified passphrase will remain cached for.  Returns zero
	 * if the passphrase isn't cached.  Returns -1 if the passphrase will never
	 * expire.
	 */
	TInt TimeRemainingL(TStreamId aStreamId);

 private:
	// From CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

 private:
	enum TState
		{
		EIdle,
		ECreatePassphrase,
		EGetPassphrase,
		EChangePassphrase
		};

 private:
	CPassphraseManager(CStreamStore& aStore);
	CPassphrase* GetCachedPassphrase(TStreamId aStreamId);
    void HandleGetPassphraseL();
    void Complete(TInt aError);
 private:
	enum TState iState;
	RPointerArray<CPassphrase> iCache;
	CStreamStore& iStore;
	TRequestStatus* iClientStatus;

	TPINValue iOldPassphrase;
	TPINValue iNewPassphrase;
	TStreamId iNewPassphraseStream;
	CPassphrase** iNewPassphrasePtr;
	TInt iTimeout;
    TInt iRetries;
	};

#endif	//	__KEYSTOREPASSPHRASE_H__
