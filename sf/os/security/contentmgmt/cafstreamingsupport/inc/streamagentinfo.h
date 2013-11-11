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
 @internalComponent
 @released
*/

#ifndef STREAMAGENTINFO_H
#define STREAMAGENTINFO_H

#include <e32base.h>

class CImplementationInformation;
class CSdpMediaField;

namespace StreamAccess
	{
	
	class CStreamAgentFactory;
	
	/* Stores information regarding a Stream Agent interface plug-in implementation.
	 * 
	 * Registration data from the plug-in's resource file and details about the
	 * agents decryption capabilities are contained so that the stream agents credentials
	 * can be obtained without the need to instantiate an instantce of it. 
	 * 
	 * If an instance of the agent is required, the StreamAgentFactoryL() method provides a
	 * handle to it's factory object.
	 */
	class CStreamAgentInfo : public CBase
		{
	public:
		
		/* Constructs a new instance of CStreamAgentInfo and pushs it onto the cleanup stack
		 * @param aImplementationInfo Contains regristration information relating to the stream 
		 * agent interface implementation 
		 * @return Pointer to the new CStreamAgentInfo object
		 */
		static CStreamAgentInfo* NewLC(const CImplementationInformation& aImplementationInfo);
		
		/* Destructor
		 */
		~CStreamAgentInfo();
		
		/* Returns the name of the stream agent plug-in implementation
		 * @return Name of the steam agent interface implementation
		 */
		const TDesC& Name() const;
		
		/* Returns the implementation UID of the stream agent plug-in DLL
		 * @return Implementation UID of the stream agent plug-in DLL
		 */
		TUid ImplementationUid() const;
		
		/* Determines whether the stream agent is capable of decoding the specified key stream
		 * @param aSdpKeyStream Conatins the metadata for SDP key management scheme of the stream
		 * @return ETrue if the stream agent recognises the SDP format and is able to
		 * decode the key stream
		 * @return EFalse if the stream agent fails to recognise the SDP format or is unable to
		 * decode the key stream
		 */
		TBool IsKeyStreamSupportedL(const CSdpMediaField& aSdpKeyStream) const;
		
		/* Returns a handle to the stream agents factory.
		 * 
		 * The factory is instantiated once and can then be called many times. This method
		 * DOES NOT transfer ownership of the factory to the calling client.
		 * 
		 * @return Reference to the CStreamAgentFactory instance 
		 */
		CStreamAgentFactory& StreamAgentFactory() const;
		
	protected:
	
		/* Constructor
		 */
		CStreamAgentInfo();
				
		/* Second phase of construction for CStreamAgentInfo 
		 * @param aImplementationInfo Contains registration information relating to the stream 
		 * agent interface implementation 
		 */
		void ConstructL(const CImplementationInformation& aImplementationInfo);
		
	private:
		
		/* Name of the stream agent plug-in implementation
		 */
		HBufC* iName;
		
		/* Implementation UID of the stream agent plug-in DLL
		 */
		TUid iImplementationUid;
		
		/* Stream agent factory handle used to generate instances of the agent key stream decoder
		 */
		CStreamAgentFactory* iStreamAgentFactory;
		
		// ECOM session key. This is set by REComSession::CreateImplementationL() during construction
		TUid iEcomKey;	
		};
	} // namespace StreamAccess
#endif /*STREAMAGENTINFO_H*/
