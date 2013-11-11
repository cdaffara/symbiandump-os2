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
 @released
*/


#ifndef __SUPPLIEROUTPUTFILE_H__
#define __SUPPLIEROUTPUTFILE_H__

#include <e32base.h>
#include <apmstd.h>
#include <caf/caftypes.h>

class RReadStream;
class RWriteStream;

#ifndef REMOVE_CAF1
#include <caf/attribute.h>
#endif

namespace ContentAccess
	{

	/** 
	Holds the details of an output file produced by CAF. 
	
	It stores:

	@li The filename
	@li The type of file (receipt or content)
	@li The mime type of the file
	
	*/
	class CSupplierOutputFile : public CBase
		{
	public:

		#ifndef REMOVE_CAF1

		/** Create a new CSupplierOutputFile used to describe output files produced by the CAF supply operation
	
		@note Receipt files should be generated in localised unicode

		@param aFileName The name of the file produced
		@param aOutputType The type of output file produced
		@return a new CSupplierOutputFile object
		@deprecated
		*/

		IMPORT_C static CSupplierOutputFile* NewL(const TDesC& aFileName, const TOutputType aOutputType);
		#endif // REMOVE_CAF1

		/** Create a new CSupplierOutputFile used to describe output files produced by the CAF supply operation

		@note Receipt files should be generated in localised unicode
	  
		@param aFileName The name of the file produced
		@param aOutputType The type of output file produced
		@param aMimeType The mime type of the file produced
		@return a new CSupplierOutputFile object
		*/
		IMPORT_C static CSupplierOutputFile* NewL(const TDesC& aFileName, const TOutputType aOutputType, const TDesC8& aMimeType);

		
		/** Construct an instance of CSupplierOutputFile by reading from the stream
		@param aStream A stream containing a CSupplierOutputFile
		@return A new CSupplierOutputFile
		*/
		IMPORT_C static CSupplierOutputFile* NewL(RReadStream& aStream);

		virtual ~CSupplierOutputFile();

		/** Finds out the name of the file.
		
		@return	The name (with full path) of the output file produced.
		*/
		IMPORT_C TPtrC FileName() const;

		/** The type of the file, e.g., Content or a Receipt.

		@note Receipt files should be generated in localised unicode

		@return	The type of output file.
		*/
		IMPORT_C TOutputType OutputType() const;

		/** The mime type of the output file. 

		@return The mime type. 
		*/
		IMPORT_C TPtrC8 MimeTypeL();

		/** Write the CSupplierOutputFile object to a stream
		
		 @param aStream The stream to write to.
		 */
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

#ifndef REMOVE_CAF1
		/** Create a CAttributeObject for the output file
		@param aPreloaded ETrue to query all attribute values before returning the attribute object
		@deprecated 
		*/
		IMPORT_C CAttribute& AttributesL (TBool aPreloaded);
	private:
		CAttribute *iAttr;
#endif // REMOVE_CAF1

	private:
		CSupplierOutputFile();
		void ConstructL(const TDesC& aFileName, const TDesC8& aMimeType, const TOutputType aOutputType);

		void InternalizeL(RReadStream& aStream);

		HBufC* iFileName;
		TBuf8 <KMaxDataTypeLength> iMimeType;
		TOutputType iOutputType;
		};
	}

#endif
