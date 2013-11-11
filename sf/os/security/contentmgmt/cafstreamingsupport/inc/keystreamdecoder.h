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

#ifndef KEYSTREAMDECODER_H
#define KEYSTREAMDECODER_H

#include <e32base.h>
#include <f32file.h>
#include <caf/importfile.h>
#include <caf/streaming/streamcaftypes.h>

class CSdpMediaField;
class CSdpDocument;

namespace StreamAccess
	{
	class CAgentKeyStreamDecoder;
	class CProtectedStreamDesc;
	class CStreamAgentFactory;
	class ContentAccess::CImportFile;
	/**
 	Allows clients to instantiate key stream decoders for short-term key streams in protected broadcasts.
	 
 	This class is initialised with an agent implementation that supports the standard with which the stream is protected.
 	*/
	class CKeyStreamDecoder : public CBase
		{
	public:
		/** 
		 Creates a new CKeyStreamDecoder object. 

		 Sample code pattern from a media controller for using this interface. (This sample assumes that the stream is protected using
		 IPSec. Similar code pattern can be used for other protection layers.)

		 @code
		 using namespace StreamAccess;
		 
		 // Create protected stream description by using the source and target connection addresses.
		 CIpSecProtectedStreamDesc *ipSecProtectedStreamDec = CIpSecProtectedStreamDesc::NewLC(sourceAddr, targetAddr);
		  
		 // Extract the relevant media field for the key stream
		 ...
		 // Now create the key stream decoder
		 CKeyStreamDecoder *keyStreamDecoder = CKeyStreamDecoder::NewLC(ipSecProtectedStreamDesc, keyStreamSDPDesc, sdpDocument);
		 // If we reached this point, then key stream decoder was initialized correctly, and if the user has rights to
		 // view the stream, it will get decrypted.
		 // If the user doesn't have the rights to view the stream (e.g. no rights for a specific pay-per-view program), 
		 // no cryptographic context will be set. In such case, the reason for lack of decryption can be queried via
		 // the GetAttributeL interface.
		 @endcode
		 
		 @param aProtectedStreamDesc Description of the protected media stream (e.g. a specific audio track, or the video channel). This parameter defines where the keys will be applied to. 
		 Description may vary according to the protection layer. For example, for IPSec it might be a subsession, while for ISMACryp a reference to the ISMACryp decoder
		 @param aSdpKeyStream Defines the parameters for the key stream. If several key streams are present (e.g. one for video protection, and one for audio protection), then
		 several CKeyStreamDecoder objects must be instantiated. If several key streams are available for the same media, then a key stream decoder should be
		 attempted for all of them - depending on the nature of the broadcast, some of these key streams might be optional or not.
		 For example, if SRTP is used for authentication and ISMACryp for encryption then there would be two key streams - both mandatory. 
		 On the other hand, if there are two alternative purchase options for the same service, there may be two corresponding key streams.
		 If no key stream decoder can be created, then the media stream cannot be decoded on the device.
		 @param aSdpDoc Description of the Session Description Protocol (SDP) document which contains all defined SDP fields and related values describing a session.
		 By using SDP document, agents can extract SDP connection field and other fields or attributes which they may need and which are not defined in the key stream
		 SDP media field.
		 @return The new CKeyStreamDecoder object.
		  
		 @leave	KErrCANoAgent	There is no agent on the device which can support the requested key management system.
		 @leave	...		One of the system-wide error codes.

         @capability	
		 Only processes with the DRM capability are able to instantiate key stream decoders.
		 If there is prior knowledge of the DRM agent handling this stream, the documentation for that DRM agent should also be consulted. 
		 */
		IMPORT_C static CKeyStreamDecoder* NewL(const CProtectedStreamDesc& aProtectedStreamDesc, const CSdpMediaField& aSdpKeyStream, const CSdpDocument& aSdpDoc);

		/** 
		 @see CKeyStreamDecoder::NewL()
		 */
		IMPORT_C static CKeyStreamDecoder* NewLC(const CProtectedStreamDesc& aProtectedStreamDesc, const CSdpMediaField& aSdpKeyStream, const CSdpDocument& aSdpDoc);
				
		 /**  Get an attribute for the service/content protection.
			  
		 @param aAttribute The attribute to query, from StreamAccess::TAttribute.
		 @param aValue [out] Used to return the attribute value.
		 @leave KErrCANotSupported if the requested attribute does not exist.
		 @leave Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		 @capability Requires the DRM capability.
		 @see StreamAccess::TAttribute
		 */
		IMPORT_C void GetAttributeL(const TAttribute& aAttribute, TBool& aValue) const;

		 /**  Get text string attributes or meta-data for the service/content protection.

		 @param aAttribute The attribute to query, from StreamAccess::TStringAttribute.
		 @return A pointer to the value of the attribute. The pointer is put onto the cleanup stack.
		 @leave KErrCANotSupported if the requested attribute does not exist.
		 @leave Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
		 		 other system-wide error codes for any other errors.
		 @capability Requires the DRM capability.
		 @see StreamAccess::TStringAttribute				 
		 */
		IMPORT_C HBufC* GetStringAttributeLC(const TStringAttribute& aAttribute) const;
		
		/**	Creates a new import session which is used to import the streamed content into a protected file
		    storage for later viewing.
		    
		    A corresponding rights object for post-acquisition protection is delivered automatically to 
		    the CAF agent. The returned session has to be closed when the recording has finished.
		    
		    If the agent requires a new output file it will return KErrCANewFileHandleRequired from 
		    @see ContentAccess::CImportFile::Write() or @see ContentAccess::CImportFile::WriteComplete().
		    The application using this API should provide the agent with a new file handle using 
		    ContentAccess::CImportFile::ContinueWithNewOutputFile().
			
		    @param aContentMimeType The mime type of the content which is recorded (e.g. video/3gpp).
		    @return A CImportFile object to import the protected streamed content. 
		    		The newly created object is left on the cleanup stack.
		 	@leave KErrCANoAgent	No agent supports recording a protected streamed content.
		 	@leave KErrCANotSupported if the protection scheme does not have or is not capable of exporting rights info. 
		 	@leave Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the other system-wide 
		 	       error codes for any other errors.	
		 	@capability Requires the DRM capability.
		 */
		IMPORT_C ContentAccess::CImportFile* CreateImportSessionLC(const TDesC8& aContentMimeType) const;
		
		/**	Creates a new import session which is used to import the streamed content into a protected file
		    storage for later viewing.
		    
		    A corresponding rights object for post-acquisition protection is delivered automatically to 
		    the CAF agent. The returned session has to be closed when the recording has finished.
		    
		    @param aContentMimeType The mime type of the content which is recorded (e.g. video/3gpp).
		    @param aSuggestedFileName  Suggested filename for the CAF agent to use when creating the output file for
		    					   the content being recorded. Details of the output files produced can be obtained 
		    					   using ContentAccess::CImportFile::OutputFileL().
		    @param aOutputDirectory The preferred location to store the protected streaming content.
		    @return A CImportFile object to import the protected streamed content. 
		    		The newly created object is left on the cleanup stack.
		 	@leave KErrCANoAgent	No agent supports recording a protected streamed content.
		 	@leave KErrCANotSupported if the protection scheme does not have or is not capable of exporting rights info. 
		 	@leave Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the other system-wide 
		 	       error codes for any other errors.	
		 	@capability Requires the DRM capability.
		 */
		IMPORT_C ContentAccess::CImportFile* CreateImportSessionLC(const TDesC8& aContentMimeType, const TDesC& aSuggestedFileName, const TDesC& aOutputDirectory) const;
		
		// Destructor
		~CKeyStreamDecoder();
		
	protected:
		// Constructor
		CKeyStreamDecoder();
	
		// Second phase constructor
		void ConstructL(const CProtectedStreamDesc& aProtectedStreamDesc, const CSdpMediaField& aSdpKeyStream, const CSdpDocument& aSdpDoc);
	
	private:
		ContentAccess::CImportFile* DoCreateImportSessionLC(const TDesC8& aContentMimeType, const TDesC& aSuggestedName, const TDesC& aOutputDirectory) const;
	
	private:
		// Pointer to the resolved agent key stream decoder
		CAgentKeyStreamDecoder* iAgentKeyStreamDecoder;			
		
		// Handle to the factory of the resolved stream agent is required in order to successfully cleanup
		// the factory instance during destruction.
		CStreamAgentFactory *iStreamAgentFactory;
		
		// ECOM session key. This is set by REComSession::CreateImplementationL() during construction
		TUid iEcomKey;
		};
	/** 
	 	The maximum length of a mime type string. Agents should not use mime types and content IDs 
	 	which is longer than this value.
	 */
	const TInt KMimeTypesMaxLength = 512;
		
	} // namespace StreamAccess
#endif // KEYSTREAMDECODER_H
