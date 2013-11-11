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
* Describes Short Term Key Message (STKM) class which defines simulated short term 
* traffic encryption keys (TEKs) for the streaming reference agent.
* Protection of streamed content is accomplished by using a 4-layer key management
* architecture in OMA BCAST. In this solution, the TEKs are applied to the actual
* content and then they are encrypted by a long term encryption key (i.e. a service 
* or program encryption key). The encrypted TEKs are transported separately from the 
* streamed content. The messages carrying TEKs are called as Short Term Key messages.
* The STKM protocol is very roughly modeled here. Only IPSec is considered as 
* the traffic protection protocol of STKMs. In a real-life agent, other protection
* protocols (e.g. SRTP and ISMACryp) shall be required.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SRASTKM_H
#define SRASTKM_H

#include <e32base.h> 
#include <s32strm.h>

NONSHARABLE_CLASS(CStkm) : CBase
/**
 	Short Term Key Message description. This class contains a subset of the property 
 	fields defined in OMA BCAST. IPSec is the only protocol considered to protect STKMs
 	in this class implementation. A real-life STKM class shall have more members than this one.
 */
	{
public:
	/**
	 	Enumeration of the algorithms used to protect the traffic key material.
	 */
	enum TTkmAlgorithm
		{
		TKM_ALGO_IPSEC    = 0,	///< The protection algorithm is IPSec.
		TKM_ALGO_SRTP 	  = 1,	///< The protection algorithm is SRTP.
		TKM_ALGO_ISMACRYP = 2,	///< The protection algorithm is ISMACryp.
		TKM_ALGO_FILTER   = 0x00E0  ///< The filter which is used to get the algorithm bits of the STKM flag.
		};
public:
	IMPORT_C static CStkm* NewLC(RReadStream& aStream);
	IMPORT_C static CStkm* NewL(RReadStream& aStream);
	
	IMPORT_C static CStkm* NewLC(TUint16 aFlags, TUint32 aSpi, const TDesC8& aEncryptionKey, const TDesC8& aAuthenticationKey);
	IMPORT_C static CStkm* NewL(TUint16 aFlags, TUint32 aSpi, const TDesC8& aEncryptionKey, const TDesC8& aAuthenticationKey);

	IMPORT_C ~CStkm();
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C TUint32 Spi() const;
	IMPORT_C HBufC8* EncryptionKey() const;
	IMPORT_C HBufC8* AuthenticationKey() const;
	
private:
	CStkm();
	void ConstructL(TUint16 aFlags, TUint32 aSpi, const TDesC8& aEncryptionKey, const TDesC8& aAuthenticationKey);
	void InternalizeL(RReadStream& aStream);

private:
	TUint16 iFlags;					///< Selectors and flags.
	TUint32 iSpi; 					///< Security parameter index.
	TUint8 iEncryptedKeyLength; 	///< Encrypted traffic key material length.
	HBufC8* iEncryptionKey; 		///< Encryption key part of the encrypted traffic key material.
	HBufC8* iAuthenticationKey; 	///< Authentication key part of the encrypted traffic key material.
	};

const TInt KMaxKeyMaterialLength     = 512;
const TInt KIpsecEncryptionKeyLength = 16;
const TInt KMinKeyMaterialLength 	 = KIpsecEncryptionKeyLength;
#endif