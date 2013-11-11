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

#ifndef PROTECTEDSTREAMDESC_H
#define PROTECTEDSTREAMDESC_H

#include <e32base.h>

// Should be removed if and when direct TInetAddr usage is removed.
#include <in_sock.h>

class TInetAddr;

class RSubConnection;

namespace StreamAccess
	{

	class CKeyStreamSink;
	/**
	Represents parameters for the protected stream. Specific subclasses of this interface are used to define parameters relevant to
	the protection layer. 
	Instances of this interface are created by the clients of the consumer interface before calling CKeyStreamDecoder::NewL
	*/
	class CProtectedStreamDesc : public CBase
		{
	public:
		/**
 		Instantiates the correct implementation of key stream sink. Should be used for creating the binding between
		the short-term key stream that delivers the decryption keys for the traffic and the protocol which protects the
		traffic itself, e.g. IPSec, ISMACryp	
		*/
		virtual CKeyStreamSink* CreateKeyStreamSinkLC() const = 0;
		virtual ~CProtectedStreamDesc() {}
		};

	/**
	Represents a media stream protected by IPSec. 
	@see StreamAccess::CProtectedStreamDesc
	*/
	NONSHARABLE_CLASS(CIpSecProtectedStreamDesc) : public CProtectedStreamDesc
		{
	public:		
		/**
		Create a description of media stream protected by IPSec. 
		
		@param aSourceAddr Source connection address.
		@param aTargetAddr Target connection address.
		@return New CIpSecProtectedStreamDesc instance		
		
		*/
		IMPORT_C static CIpSecProtectedStreamDesc* NewLC(const TInetAddr& aSourceAddr, const TInetAddr& aTargetAddr);
			
		/**
		@see CProtectedStreamDesc::CreateKeyStreamSinkLC()
		*/		
		IMPORT_C CKeyStreamSink* CreateKeyStreamSinkLC() const;		
		
		//Destructor
		~CIpSecProtectedStreamDesc();
		
	private:
		CIpSecProtectedStreamDesc(const TInetAddr& aSourceAddr, const TInetAddr& aTargetAddr);
		
	private:	
		TInetAddr iSourceAddr;
		TInetAddr iTargetAddr;
		
		};
	} // namespace StreamAccess
#endif // PROTECTEDSTREAMDESC_H

