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

#ifndef KEYSTREAMSINK_H
#define KEYSTREAMSINK_H

#include <e32base.h>
#include <caf/streaming/keyassociation.h>

class RWriteStream;

namespace StreamAccess
	{
	enum TEncryptionAlgorithm
		{
		ENoEncryption = 0,
		EAES_128_CBC,
		EAES_128_CTR
		};
	
	enum TAuthenticationAlgorithm
		{
		ENoAuthentication = 0,
		EHMAC_SHA1		 
		};
	/**
 	Defines the key stream sink abstraction. Key stream sink is used for applying keys decrypted by the CAF streaming agent
	to their correct destination.
 
 	Typical implementations would be CIpSecKeyStreamSink, CSrtpKeyStreamSink etc.
 	*/
	class CKeyStreamSink : public CBase
		{
	public:

		/**  Clone the key stream sink. Required to support copying of key stream sinks without knowing their runtime type. 
		*/
		virtual CKeyStreamSink *CloneLC() const = 0;

		/**  Push a decrypted key association to its sink. This function should be called only by CAgentKeyStreamDecoder
		instances. Please note that this interface is synchronous, i.e. the function returns when the key is implemented.
						 
		@see CAgentKeyStreamDecoder

		@param aKeyAssociation The key association extracted from the short-term key message. 
		@leave One of the system-wide error codes.
		*/
		virtual void ProcessNewKeyAssociationL(const CKeyAssociation& aKeyAssociation) = 0;
		 
		/**  Set the encryption algorithm. If encryption is used, the encryption algorithm must be set before processing any key associations. 
		Please note that changing the encryption algorithm once key associations have been processed is not supported. 
		 
		@param aEncryptionAlgorithm The encryption algorithm
		@leave One of the system-wide error codes.
		*/		 
		virtual void SetEncryptionAlgorithmL(const TEncryptionAlgorithm& aEncryptionAlgorithm) = 0;
		 
		/**  Set the authentication algorithm. If authentication is used, the authetication algorithm must be set before processing any key associations. 
		Please note that changing the authentication algorithm once key associations have been processed is not supported. 
		 
		@param aAuthenticationAlgorithm The authentication algorithm
		@leave One of the system-wide error codes.
		*/		 		 
		virtual void SetAuthenticationAlgorithmL(const TAuthenticationAlgorithm& aAuthenticationAlgorithm) = 0;

		/**  Externalize the key stream sink to a buffer. Leaves the allocated buffer on the cleanup stack.
		 
		@return Pointer to the buffer
		@leave One of the system-wide error codes.
		*/		 		 
		IMPORT_C TPtr8 ExternalizeLC() const;

		/**  Internalize the key stream sink from a buffer. Leaves the allocated key stream sink on the cleanup stack.
		
		@param aBuf Buffer with the externalized key stream sink
		@return Pointer to the key stream sink
		@leave One of the system-wide error codes.
		*/		 		 
		IMPORT_C static CKeyStreamSink *InternalizeLC(const TDesC8 &aBuf);
		 
		virtual ~CKeyStreamSink() {}
	protected:
		virtual void DoExternalizeL(RWriteStream &aStream) const = 0;
	protected:
		// Used for serialization
		enum TSinksEnum
			{
			EIpSecSinkId = 1
			};
		};
	} // namespace StreamAccess
#endif // KEYSTREAMSINK_H

