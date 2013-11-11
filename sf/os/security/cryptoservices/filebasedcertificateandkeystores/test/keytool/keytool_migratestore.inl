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


#include <mctkeystore.h>
#include <f32file.h>
#include <s32mem.h>

#include <authserver/authtypes.h>
#include <centralrepository.h>
#include <authserver/authclient.h>
#include <authserver/authexpression.h>
#include <authserver/auth_srv_errs.h>
#include <authserver/aspubsubdefs.h>
#include <authserver/authtypes.h>
#include <pbedata.h>
#include <mctkeystore.h>
#include <keystore_errs.h>
#include <securityerr.h>
#include <keytool.rsg>

#include "keystorecenrepconfig.h"
#include "fsdatatypes.h"
#include "ckeydatamanager.h"
#include "keystorepassphrase.h"
#include "keystreamutils.h"
#include "asymmetrickeys.h"

#include "keytool_utils.h"
#include "keytool_commands.h"
#include "keytool_controller.h"

// the size allocated to retrieve the private key.
const TInt KSize = 2048;

template <class T> inline void CKeytoolMigrateStore::RetrieveAndStorePublicKeyL( CFileKeyData* aKeyData, T* aPublicKey)
	{	
	// open the publickeystream
	RStoreReadStream publicStream;
	publicStream.OpenLC(*iReadFileStore, aKeyData->PublicDataStreamId());
			
	CreateL(publicStream,aPublicKey);
	CleanupStack::PushL(aPublicKey);
	RStoreWriteStream writePublicStream;
	TStreamId publicStrId = writePublicStream.CreateLC(*iWriteFileStore);
	aKeyData->SetPublicDataStreamId(publicStrId);
	writePublicStream << *aPublicKey;
	writePublicStream.CommitL();
	CleanupStack::PopAndDestroy(3,&publicStream); // aPublicKey, writePublicStream
	}

template <class T> inline void CKeytoolMigrateStore::RetrieveAndStorePrivateKeyL( CFileKeyData* aKeyData, T* aPrivateKey)
	{	
	// open the privatekeystream based on the supplied passphrase
	RStoreReadStream privateStream;
	privateStream.OpenLC(iPassphrase->Store(), aKeyData->PrivateDataStreamId());
					
	CreateL(privateStream,aPrivateKey);
	CleanupStack::PushL(aPrivateKey);
	RStoreWriteStream writePrivateStream;
	TStreamId pvtStrId = writePrivateStream.CreateLC(*iWriteFileStore);
	aKeyData->SetPrivateDataStreamId(pvtStrId);
	EncryptAndStoreL(*aPrivateKey, writePrivateStream );
	writePrivateStream.CommitL();
	CleanupStack::PopAndDestroy(3,&privateStream); // aPrivateKey, writePrivateStream
	}

template <class T> inline void CKeytoolMigrateStore::EncryptAndStoreL(const T& aKey, RStoreWriteStream& aStream )
	{
	RMemWriteStream writeStream;
	CleanupClosePushL(writeStream);
	// create the buffer for storing the encrypted private key
	HBufC8* privateKey = HBufC8::NewLC(KSize);
	TPtr8 keyPtr(privateKey->Des());
	keyPtr.FillZ(KSize);
	
	// a pointer to the data is required to pass it to the stream
	TAny* ptr = const_cast<TAny*>(static_cast<const TAny*>(privateKey->Des().Ptr()));
	writeStream.Open( ptr,KSize);
	writeStream << aKey;
	writeStream.CommitL();
	TStreamPos pos = writeStream.Sink()->TellL(MStreamBuf::EWrite);
	keyPtr.SetLength(pos.Offset());
	StoreKeyL(keyPtr, aStream);
	CleanupStack::PopAndDestroy(2, &writeStream); // privateKey

	}

