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

#ifndef STREAMAGENTFACTORY_H
#define STREAMAGENTFACTORY_H

#include <caf/streaming/keystreamsink.h>
#include <caf/streaming/streamagentinterface.h>
#include <e32base.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

class CSdpMediaField;
class CSdpDocument;

namespace StreamAccess
	{

	/**
	 Abstract interface handed out by an ECOM agent implementation by REComSession::CreateImplementationL().

	 CStreamAgentFactory defines an abstract factory that is responsible for creating instances of CKeyStreamDecoder.
	 
	 Derived classes will hand out concrete product implementations which are  specific for that particular agent.

	 Derived classes effectively represent the ECOM session handle used by Streaming CAF classes.
	   
	 */
	 class CStreamAgentFactory : public CBase
	 	{
	 public:
	 	/** Destructor. */
	 	IMPORT_C ~CStreamAgentFactory();
	 	
		/**
		Factory function that creates a CAgentKeyStreamDecoder object for decoding and applying the short-term key stream.
		 
		@param aKeyStreamSink The reference to the necessary CKeyStreamSink object used to create the binding between
		the short-term key stream that delivers the decryption keys for the traffic and the protocol which protects the
		traffic itself. 
		
		NOTE: A copy of the key stream sink is made by the stream agent implementation during construction. This means
		that the local copy can be destroyed once it has been used to retrieve a handle to the desired CAgentKeyStreamDecoder
		object.

		@param aSdpKeyStream Conatins the metadata for the SDP key management scheme of the stream
		@param aSdpDoc Contains a collection of SDP fields and values describing session.
		@return A pointer to the CAgentKeyStreamDecoder object. Ownership on the object is transferred to the client of the interface.
		@return NULL if the agent does not support the key management scheme specified in the SDP key stream definition.
		@leave KErrPermissionDenied if the process does not have the rights to instantiate key stream decoders.
		@leave ...		One of the system-wide error codes.
		*/
		virtual CAgentKeyStreamDecoder* GetKeyStreamDecoderL(const CKeyStreamSink& aKeyStreamSink,
													    	 const CSdpMediaField& aSdpKeyStream,
													    	 const CSdpDocument& aSdpDoc) = 0;
		
		/**
		Function to determine whether the stream agent supports the key management scheme specified in the SDP key stream definition
		
		@param aSdpKeyStream Conatins the metadata for the SDP key management scheme of the stream
		@return ETrue if the stream agent recognises the SDP format and is able to decode the key stream
		@return EFalse if the stream agent fails to recofnise the SDP format or is unable to decode the key stream
		*/
		virtual TBool IsKeyStreamSupportedL(const CSdpMediaField& aSdpKeyStream) = 0;
			
	 protected:
	 	/** Constructor. */
	 	IMPORT_C CStreamAgentFactory();
		};
		
	/** Hosting the Stream Agent Factory Interface UID. */	
	const TUid KUidStreamAgentFactoryInterface = {0x10285890};
		 
	} // namespace StreamAccess
#endif // STREAMAGENTFACTORY_H
