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


#ifndef __ATTRIBUTESET_H__
#define __ATTRIBUTESET_H__

#include <e32base.h>
#include <caf/caftypes.h>

class RReadStream;
class RWriteStream;

namespace ContentAccess
	{
	
	/** Holds the values of a predefined set of attributes based upon 
	ContentAccess::TAttribute
	
	It is also possible for an agent to provide an extended set of these 
	attributes beyond EAgentSpecificAttributeBase but only applications written 
	to support that particular agent will support this.

	No duplicate attributes are allowed in the set
	*/
	class RAttributeSet
		{
		/** Holds the value of an attribute along with any error encountered
		while retrieving the attribute
		*/
		class TAttributeValue 
			{
		public:
			TInt iAttribute;
			TInt iValue;
			TInt iError;
			};

	public:
		IMPORT_C RAttributeSet();
		
		/** Release all resources used by the RAttributeSet
		
		This must be called before the RAttribute set goes out of scope
		*/
		IMPORT_C void Close();

		/** Add a new attribute to the set
		The attribute is initalised with a default value of EAttributeNotSupported
		
		@param aAttribute The attribute to add to the set
		*/
		IMPORT_C void AddL(TInt aAttribute);

		/** Get the value of a specified attribute
		@param aAttribute The attribute to query
		@param aValue The value of the attribute
		@return Whether the value parameter was updated
		@return KErrNone The attribute value was updated
		@return KErrNotFound The attribute is not part of the set
		*/
		IMPORT_C TInt GetValue(TInt aAttribute, TInt& aValue) const;

		/** Set the value of an attribute within the set
		@param aAttribute The attribute to set
		@param aValue The value of the attribute
		@param aErrorCode The error code to return when this attribute is requested via the GetValue() function
		@return Whether the new attribute value was set
		@return KErrNone if the value was set
		@return KErrNotFound if the attribute is not part of the set
		*/
		IMPORT_C TInt SetValue(TInt aAttribute, TInt aValue, TInt aErrorCode);
		
		/** Find the attribute stored at a particular index in the set
		@param aIndex The index of the attribute
		@return The attribute, NOT the value of the attribute
		*/
		IMPORT_C TInt operator [] (TInt aIndex) const;

		/** The number of attributes in the set
		@return The number of attributes in the set
		*/
		IMPORT_C TInt Count() const;
		
		/** Write the RAttributeSet to a stream
		@param aStream The stream to write the attribute set to
		*/
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

		/** Read the RAttributeSet from a stream
		This does not clear the contents of the attribute set before reading. The 
		values of any attributes already in the set are updated with new values 
		from the stream. New attributes from the stream and their values are added
		to the set.
		@param aStream The stream to read the attribute set from
		*/
		IMPORT_C void InternalizeL(RReadStream& aStream);

	private:
		void AddL(TInt aAttribute, TInt aValue, TInt aErrorCode);

		RArray <TAttributeValue> iAttributes;
		};
	}
#endif
