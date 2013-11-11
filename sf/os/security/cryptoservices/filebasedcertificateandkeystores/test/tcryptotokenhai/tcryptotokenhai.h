/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This class implements the reference Crypto Token Hardware Abstraction 
* Interface (HAI). It is just intended to show how operations using 
* device keys can be performed using crypto token framework. In the 
* real world scenario, this HAI should be replaced by device drivers 
* by the licensees. In such a case, all the operations performed by 
* the replacing class would be performed in Kernel Space.
*
*/


/**
 * @file
 * @internalComponent
 * @released
 */
#ifndef CCRYPTOTOKENHAI_H
#define CCRYPTOTOKENHAI_H

#include <e32base.h>
#include <f32file.h>
#include <S32FILE.H>
#include <mctkeystore.h>

class CKeyDetails;
class MCTToken;

namespace CryptoSpi
	{
	class CSigner;
	}

NONSHARABLE_CLASS(CCryptoTokenHai) : public CBase
	{
	public:
		IMPORT_C static CCryptoTokenHai* NewLC(MCTToken* aToken);
		IMPORT_C static CCryptoTokenHai* NewL(MCTToken* aToken);
		IMPORT_C ~CCryptoTokenHai();
		
	public:
		IMPORT_C void DecryptL(TInt aHandle,
								const TDesC8& aCiphertext,
								HBufC8*& aPlaintext );
	
		IMPORT_C void SignL( 	TInt aHandle,
								const TDesC8& aPlaintext,
								HBufC8*& aSignature );
	
		IMPORT_C TInt KeyPresent( TInt aHandle );
		IMPORT_C void ExportPrivateKeyL( TInt aHandle, HBufC8*& aKey );
		IMPORT_C void ExportPublicKeyL( TInt aHandle, HBufC8*& aKey );
		IMPORT_C void ImportKeyL( const TDesC& aLabel, const TDesC8& aPrivateKey, const TDesC8& aPublicKey );
		IMPORT_C void ListL(const TCTKeyAttributeFilter& aFilter, RPointerArray<CCTKeyInfo>& aKeys) const;
		
	private:
		void ConstructL();
		CCryptoTokenHai(MCTToken& aToken);
	    void OpenStoreL();
	    void OpenStoreInFileL(const TDesC& aFile);
	    void CreateStoreInFileL(const TDesC& aFile);
	    void MakePrivateFilenameL(RFs& aFs, const TDesC& aLeafName, TDes& aNameOut);
		void EnsurePathL(RFs& aFs, const TDesC& aFile);
		void MakePrivateROMFilenameL(RFs& aFs, const TDesC& aLeafName, TDes& aNameOut);
		void CopyL(RFs& aFs, const TDesC& aSouce, const TDesC& aDest);
		void CompactStore();
		static void RevertStore(TAny* aStore);
		void ReadKeysFromStoreL();
		TBool KeyMatchesFilterL(const CKeyDetails& aInfo, const TCTKeyAttributeFilter& aFilter) const;
		void CopyStoreFromROML(const TDesC& fullPath, TInt result);
		void WriteKeysToStoreL(RStoreWriteStream& aRootStream);
		
	private:
		RPointerArray<CKeyDetails> iKeys;
		RFs iFs;
		CPermanentFileStore* iFileStore;
		TStreamId iRootStreamId;
		MCTToken& iToken;
	};

#endif	//	CCRYPTOTOKENHAI_H
