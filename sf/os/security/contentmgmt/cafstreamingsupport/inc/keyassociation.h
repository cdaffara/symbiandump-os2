// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @publishedPartner
 @released
*/

#include <e32base.h>

#ifndef KEYASSOCIATION_H
#define KEYASSOCIATION_H

namespace StreamAccess
	{
	/**
 	Defines the key association abstraction. In streaming protection, keys can be used slightly differently
	for different protection layers. In IPSec they are used as a part of SA (Security Associations), while in SRTP they
	are used as the Master Key - used for deriving the decryption and the authentication keys.
 	*/
	class CKeyAssociation : public CBase
		{
	public:	 
		IMPORT_C virtual ~CKeyAssociation();
		};


	/**
 	Defines a key association for the IPSec protocol.
 	*/
	class CIpSecKeyAssociation : public CKeyAssociation
		{
	friend class CIpSecKeyStreamSink;
	public:
		 /** Constructs a new key association for IPSec.
		 @param aSpi The SPI (security parameter index) to be used.
		 @param aEncryptionKey The encryption key to be used in this association. Please note that the ownership on aEncryptionKey is passed to the newly created object.
		 @param aAuthenticationKey The authentication key to be used in this association. Please note that the ownership on aAuthenticationKey is passed to the newly created object.		 
		 @leave One of the system-wide error codes.
		 */
		 IMPORT_C static CIpSecKeyAssociation* NewL(TInt32 aSpi, HBufC8* aEncryptionKey, HBufC8* aAuthenticationKey);

		 /**
		 @see CIpSecKeyAssociation::NewL
		 */
		 IMPORT_C static CIpSecKeyAssociation* NewLC(TInt32 aSpi, HBufC8* aEncryptionKey, HBufC8* aAuthenticationKey);

	private:
		 TInt32 GetSpiL() const;
		 const HBufC8* GetEncryptionKeyL() const;
		 const HBufC8* GetAuthenticationKeyL() const;
	private:
		 CIpSecKeyAssociation(TInt32 aSpi, HBufC8* aEncryptionKey, HBufC8* aAuthenticationKey);
	private:
		 TInt32 iSpi;
		 HBufC8 *iEncryptionKey;
		 HBufC8 *iAuthenticationKey;
		};

	} // namespace StreamAccess
#endif // KEYASSOCIATION_H

