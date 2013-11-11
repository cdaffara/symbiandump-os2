/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
*/


#ifndef __CAF_METADATA_H__
#define __CAF_METADATA_H__

#include <e32base.h>
#include <f32file.h>

class RReadStream;
class RWriteStream;

namespace ContentAccess
	{
	/** Stores a field and data pair for use in a supplier import operation
	
	The field and data can be any values the application doing the import thinks 
	the agent will find useful. Some information that could be included here are
	headers, eg:

	"Content Type", "application/vnd.oma.drm.dm"

	The application can include anything it likes, if the agent doesn't understand
	the header it can just ignore it, eg:

	"Colour", "Yellow"


	CMetaData can be used with 8-bit or unicode data and automatically performs
	conversions between the two. 
	
	Applications should use the unicode functions whenever possible because it 
	removes the risk of losing information during the unicode to 8 bit conversion.

	*/
	class CMetaData : public CBase
		{
	public:
		/** Construct a new CMetaData with 8 bit data 
		The 8-bit data will be stored in CMetaData as unicode 
		
		@param aField The name of the field
		@param aData The data associated with the field
		@return A CMetaData object populated with the given field and data		
		*/
		static CMetaData* NewL(const TDesC8& aField, const TDesC8& aData);
		/** Construct a new CMetaData with unicode data 

		@param aField The name of the field
		@param aData The data associated with the field
		@return A CMetaData object populated with the given field and data		

		*/
		static CMetaData* NewL(const TDesC16& aField, const TDesC16& aData);

		/** Construct a new CMetaData from a data stream

		@param aStream The stream to read the CMetaData object from
		@return A CMetaData object read from the stream
		
		*/
		static CMetaData* NewL(RReadStream& aStream);

		/** destructor 
		*/
		virtual ~CMetaData();

		/** The field name 
		@return The field name
		*/
		IMPORT_C const TDesC& Field() const;
		
		/** The data value 
		@return The data value
		*/
		IMPORT_C const TDesC& Data() const;

		/** The field name 
		@return The field name
		*/
		IMPORT_C const TDesC8& Field8() const;
		
		/** The data value 
		@return The data value
		*/
		IMPORT_C const TDesC8& Data8() const;

		/** Write the CMetaData to a data stream
		
		@param aStream The stream to write the CMetaData object to
		*/
		void ExternalizeL(RWriteStream& aStream) const;

	private:
		CMetaData(TBool aUnicode);
		void ConstructL(const TDesC8& aField, const TDesC8& aData);
		void ConstructL(const TDesC16& aField, const TDesC16& aData);

		/** Read the CMetaData from a data stream
		*/
		void InternalizeL(RReadStream& aStream);

	private:
		HBufC* iField;
		HBufC* iData;
		HBufC8* iField8;
		HBufC8* iData8;
		TBool iUnicode;
		};
	}

#endif
