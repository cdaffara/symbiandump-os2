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
* Implements method to encrypt the private key.  
*
*/


/**
 @file 
 @internalTechnology
*/

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER

#ifndef CFSKEYSTORESERVER_INL
#define CFSKEYSTORESERVER_INL

// the size allocated to retrieve the private key.
const TInt KSize = 2048;

#include "cfskeystoreserver.h"

template <class T> inline void CFSKeyStoreServer::EncryptAndStoreL(const T& aKey, RStoreWriteStream& aStream )
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

#endif // CFSKEYSTORESERVER_INL

#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
