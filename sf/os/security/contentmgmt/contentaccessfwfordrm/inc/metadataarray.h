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


#ifndef __CAF_METADATAARRAY_H__
#define __CAF_METADATAARRAY_H__

#include <e32base.h>
#include <f32file.h>

class RWriteStream;
class RReadStream;

namespace ContentAccess
	{
	class CMetaData;

	/**
	Stores an array of CMetaData objects for use in a supplier import operation
	
	It provides search and routines (for both 8 bit and Unicode) to find the
	value corresponding to a particular field in the array.

  	Applications should use the unicode search function whenever possible because 
	it removes the risk of losing information during the unicode to 8 bit 
	conversion.

	*/
	class CMetaDataArray : public CBase
		{
	public:
	
		IMPORT_C static CMetaDataArray* NewL();
		IMPORT_C static CMetaDataArray* NewLC();

		/** Construct a CMetaDataArray from a stream
		@param aStream the stream containing the metadata array
		@return a new CMetaDataArray
		*/
		IMPORT_C static CMetaDataArray* NewL(RReadStream& aStream);

		/** Destructor */
		virtual ~CMetaDataArray();

		/** Adds a new meta data,
		@param aField The name used to index the value of interest
		@param aData		The data
		*/		
		IMPORT_C void AddL(const TDesC8& aField, const TDesC8& aData);

		/** Adds a new meta data,
		@param aField The name used to index the value of interest
		@param aData		The data
		*/		
		IMPORT_C void AddL(const TDesC& aField, const TDesC& aData);

		/** Retrieves the MetaData at the given index
		*
		* @param aIndex	The index of the meta-data
		* @return		
		*/
		IMPORT_C const CMetaData& operator [] (TInt aIndex) const;

		/** The number of metadata objects in the list
		*/
		IMPORT_C TInt Count() const;

		/** Find the unicode data for a given unicode field. 

		@param aField	The field to retrieve
		@param aMatchCase ETrue To perform case sensitive search, EFalse - To perform non case sensitive search
		@return The corresponding data or else a zero length string if the field was not found	
		*/
		IMPORT_C const TDesC& SearchL(const TDesC& aField, TBool aMatchCase = EFalse) const;
		
		/** Find the 8 bit data for a given 8 bit field. 

		@param aField8	The field to retrieve
		@param aMatchCase ETrue To perform case sensitive search, EFalse - To perform non case sensitive search
		@return The corresponding data or else a zero length string if the field was not found
		*/
		IMPORT_C const TDesC8& SearchL(const TDesC8& aField8, TBool aMatchCase = EFalse) const;
		
		/** Externalizes the CMetaDataArray object to a stream.
		* 
		* @param aStream	The stream to write the header information to.
		*/
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	private:
		CMetaDataArray();
		void InternalizeL(RReadStream& aStream);
	private:

		RPointerArray <CMetaData> iArray;  

		// Length of the longest data item in the array
		TInt iMaxFieldLength;
		};
	}

#endif
