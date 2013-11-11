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

#ifndef STREAMAGENTINTERFACE_H
#define STREAMAGENTINTERFACE_H

#include <e32base.h>
#include <f32file.h>
#include <caf/streaming/streamcaftypes.h>

#include <caf/rightsinfo.h>

namespace StreamAccess
	{
	
	class CAgentKeyStreamDecoder : public CBase
		{
	public:

		/** Destructor. */
		IMPORT_C virtual ~CAgentKeyStreamDecoder();
		
		/**  
		@see CKeyStreamDecoder::GetAttributeL
		*/
		virtual void GetAttributeL(const TAttribute& aAttribute, TBool& aValue) =0;

		/**  
		@see CKeyStreamDecoder::GetStringAttributeL	
		*/
		virtual HBufC* GetStringAttributeLC(const TStringAttribute& aAttribute) =0;

		/**  Gets information data about the post-delivery rights objects for the protected service/content. 
		  	 Used when the content is imported into protected file storage for later viewing.
		
		 @param aFile A reference to the rights object file handle. The file handle must be closed by the client application.
		 @param aCid  The content Id of the post-acquisition rights object.
		 @param aRightsMimeType The mime type of the rights object file. 
		 @param aContentMimeType The mime type of the content which is recorded.
		 @leave KErrCANotSupported if the protection scheme does not have or is not capable of exporting rights info.
		 @leave Otherwise one of the other system-wide error codes.
		 */
		virtual void GetPostDeliveryRightsL(RFile& aFile, TDes8& aCid, TDes8& aRightsMimeType, TDes8& aContentMimeType) =0;

protected:	
		/** Constructor.*/
		IMPORT_C CAgentKeyStreamDecoder();
		};
	} // namespace StreamAccess
#endif // STREAMAGENTINTERFACE_H

