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


#ifndef __STRINGATTRIBUTESET_H__
#define __STRINGATTRIBUTESET_H__

#include <e32base.h>
#include <caf/caftypes.h>

class RReadStream;
class RWriteStream;

namespace ContentAccess
	{
	class CStringAttribute;

	/** Holds the values of a predefined set of attributes based upon 
	ContentAccess::TStringAttribute
	
	It is also possible for an agent to provide an extended set of these 
	attributes beyond EAgentSpecificAttributeBase but only applications written 
	to support that particular agent will support this.

	No duplicate attributes are allowed in the set

	*/
	class RStringAttributeSet
		{
	public:
		IMPORT_C RStringAttributeSet();
		
		/** Release all resources used by the RStringAttributeSet
		
		This must be called before the RStringAttribute set goes out of scope
		*/
		IMPORT_C void Close();

		/** Add a new attribute to the set
		The attribute value is initalised with an empty string
		
		@param aAttribute The attribute to add to the set
		*/
		IMPORT_C void AddL(TInt aAttribute);

		/** Get the value of a specified attribute
		@param aAttribute The attribute to query
		@param aValue The value of the attribute is copied to this descriptor
		@return The value of the attribute
		@return KErrOverflow If the descriptor is not large enough to store the attribute
		@return KErrNotFound if the attribute is not part of the set
		*/
		IMPORT_C TInt GetValue(TInt aAttribute, TDes& aValue) const;
		

		/** Get the length of a buffer required to store the value of a specified attribute
		@param aAttribute The attribute to query
		@return The length of the value of the attribute
		*/
		IMPORT_C TInt GetValueLength(TInt aAttribute) const;
		

		/** Set the value of an attribute within the set
		@param aAttribute The attribute to set
		@param aValue The value of the attribute
		@param aErrorCode The error to return when GetValue() is called for this attribute
		@leave KErrNotFound if the attribute is not part of the set
		*/
		IMPORT_C TInt SetValue(TInt aAttribute, const TDesC& aValue, TInt aErrorCode);
		

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
		void AddL(TInt aAttribute, const TDesC& aValue, TInt aErrorCode);

		RPointerArray <CStringAttribute> iAttributes;
		};
	}
#endif
