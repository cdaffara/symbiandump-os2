/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @publishedPartner
 @deprecated
*/


#ifndef __CAFMIMEHEADER_H__
#define __CAFMIMEHEADER_H__

#ifndef REMOVE_CAF1

#include <caf/caftypes.h>

#include <e32std.h>
#include <e32base.h>
#include <caf/caftypes.h>

class RWriteStream;
class RReadStream;

namespace ContentAccess
	{
	class CMimeFieldAndData;
	
	/**
	* Packages the MIME header information for easy use by a Content Access Agent.
	*
	* The "Content-Type" MIME header must be correct; other fields are optional from
	* CAF's perspective, but may be mandatory for some agents.
	*
	* Consider the following MIME header
	*
	* @code
	* Content-type: application/vnd.oma.drm.content;
	* Content-Length: 1234
	* X-Oma-Drm-Separate-Delivery: 12
	*
	* @endcode
	*
	* The CCafMimeHeader is constructed as follows:
	*
	* @code
	* // initialise the header with the content type
	* _LIT8(KMyMime,"application/vnd.oma.drm.content");
	* CCafMimeHeader *header = CCafMimeHeader::NewL(KMyMime());
	* 
	* // Set the content length
	* _LIT8(KMyLength,"1234");
	* header->SetStandardMimeDataL(EContentLength, KMyLength());
	* 
	* // Set the non-standard header X-Oma-Drm-Separate-Delivery
	* _LIT8(KMyField,"X-Oma-Drm-Separate-Delivery");
	* _LIT8(KMyData,"12");
	* header->AddNonStandardMimeL(KMyField(), KMyData());
	*
	* @endcode
	*
	* To retreive the values from the CCafMimeHeader
	* 
	* @code
	* TPtrC8 contentType = header->StandardMimeData(EContentType);     // application/vnd.oma.drm.content
	* TPtrC8 contentLength = header->StandardMimeData(EContentLength); // 1234
	*
	* TInt numNonStandard = header->NonStandardMimeCount();            // 1
	* TPtrC8 field = header->NonStandardMimeField(0);                  // X-Oma-Drm-Separate-Delivery
	* TPtrC8 data = header->NonStandardMimeData(0);                    // 12
	*
	* @endcode
	*
	* @publishedPartner
	* @deprecated 
	*/
	class CCafMimeHeader : public CBase
		{
	public:
		/** This creates the CCafMimeHeader object and calls SetFieldDataL(EContentType, aContent_Type);
		*
		* A CCafMimeHeader must have a valid EContentType field, so the only 
		* constructor available requires this as a parameter. 
		*
		* @param aContent_Type	The "Content-Type :" field from the MIME header.
		*/
		IMPORT_C static CCafMimeHeader* NewL(const TDesC8& aContent_Type);
		
		/** Destructor */
		IMPORT_C virtual ~CCafMimeHeader();

		/** Sets the MIME data associated with a standard MIME field.
		*
		* @param aIndex	The TMimeFields MIME header field.
		* @param aData	The data corresponding to the MIME header.
		*/
		IMPORT_C void SetStandardMimeDataL(const TMimeFields &aIndex, const TDesC8& aData);

		/** Gets the data associated with the standard MIME field.
		* 
		* @param aIndex	The TMimeFields MIME header to retrieve corresponding to the MIME data.
		* @return		The MIME data associated with the field corresponding to aIndex.
		*/
		IMPORT_C TPtrC8 StandardMimeData(const TMimeFields& aIndex) const;
		
		/** Adds a new non-standard MIME header field.
		* 
		* e.g., OMA requires a MIME header "X-Oma-Drm-Separate-Delivery".
		*
		* @param aFieldName The ....
		* @param aData		The data corresponding to the MIME header.
		*/
		IMPORT_C void AddNonStandardMimeL(const TDesC8& aFieldName, const TDesC8& aData);

		/** Retrieves the field name for a non-standard MIME header. 
		*
		* @param aIndex	The index of the non-standard MIME header.
		* @return		The non-standard MIME field.
		*/
		IMPORT_C TPtrC8 NonStandardMimeField(TInt aIndex) const;

		/** Retrieves the data for a non-standard MIME header. 
		*
		* @param aIndex	The index of the non-standard MIME header.
		* @return		The data associated with the non-standard MIME field. 
		*/
		IMPORT_C TPtrC8 NonStandardMimeData(TInt aIndex) const;

		/** The number of non-standard MIME header fields.
		* 
		* e.g., OMA requires a MIME header "X-Oma-Drm-Separate-Delivery".
		*
		* @return	The number of non-standard MIME header fields.
		*/
		IMPORT_C TInt NonStandardMimeCount() const;

		/** Externalizes the CCafMimeHeader object.
		* 
		* Allows the MIME header information to be passed between the client proxy 
		* and the CA agent server.
		*
		* @param aStream	The stream to write the header information to.
		*/
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

		/** Internalize the CCafMimeHeader object.
		* 
		* Allows the MIME header information to be passed between the client proxy 
		* and the CA agent server.
		*
		* @param aStream	The stream to read the header information from.
		*/
		IMPORT_C void InternalizeL(RReadStream& aStream);

	private:
		CCafMimeHeader();

		void ConstructL(const TDesC8& aContent_Type);
	
		// Index used to map the fixed TMimeFields field value to the position in
		// iStandardMimeHeaders used to store it
		HBufC8* iStandardMimeHeader[EMimeMax];

		/** Non-standard header fields, some agents (e.g., OMA) need access to 
		* other MIME headers.	
		*/
		RPointerArray <CMimeFieldAndData> iNonStandardMimeHeaders;  
		
		};
	}
#endif // REMOVE_CAF1

#endif // __CAFMIMEHEADER_H__
