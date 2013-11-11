// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
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

#include <xml/attribute.h>

using namespace Xml;


EXPORT_C RAttribute::RAttribute()
/**
Default constructor

*/
	{
	// do nothing;
	}
	
	
EXPORT_C RAttribute RAttribute::Copy()
/**
Copy method. The original and copy must be closed seperately.

*/
	{
	RAttribute copy;
	RTagInfo tagCopy = iAttribute.Copy();
	copy.Open(const_cast <RString&> (tagCopy.Uri()),
			  const_cast <RString&> (tagCopy.Prefix()),
			  const_cast <RString&> (tagCopy.LocalName()),
			  iValue.Copy(),
			  iType);
	return copy;
	}
	


EXPORT_C void RAttribute::Close()
/**
This method cleans up the object before destruction. It releases all resources in
accordance to the R Class pattern.

@post				This object may be allowed to go out of scope.

*/
	{
	iAttribute.Close();
	iValue.Close();
	}



EXPORT_C void RAttribute::Open(const RString& aUri, const RString& aPrefix, 
							   const RString& aLocalName, 
							   const RString& aValue, const TAttributeType aType)
/**
This method sets all the attribute's contents, taking ownership of the RStrings.

@pre				The object has just been constructed and no other values have been set.
@post				The objects members have been set to the values given.

@param				aUri The URI of the namespace.
@param				aPrefix The prefix of the qualified name.
@param				aLocalName The local name of the qualified name.
@param				aValue The value associated with this attribute.
@param				aType The type associated with this attribute.
*/
	{
	iAttribute.Open(aUri, aPrefix, aLocalName);
	iValue = aValue;
	iType = aType;
	}



EXPORT_C void RAttribute::Open(const RString& aUri, const RString& aPrefix, 
							   const RString& aLocalName, const TAttributeType aType)
/**
This method sets the attribute's contents, taking ownership of the RStrings.

@pre				The object has just been constructed and no other values have been set.
@post				The objects members have been set to the values given.

@param				aUri The URI of the namespace.
@param				aPrefix The prefix of the qualified name.
@param				aLocalName The local name of the qualified name.
@param				aType The type associated with this attribute.
*/
	{
	iAttribute.Open(aUri, aPrefix, aLocalName);
	iType = aType;
	}



EXPORT_C const RTagInfo& RAttribute::Attribute() const
/**
This method returns a handle to the attribute's name details.

@return				The attribute's name details.

@pre				The objects members have been pre-set accordingly.

*/
	{
	return iAttribute;
	}



EXPORT_C const RString& RAttribute::Value() const
/**
This method returns a handle to the attribute value. 
If the attribute value is a list of tokens (IDREFS, ENTITIES or NMTOKENS), 
the tokens will be concatenated into a single RString with each token separated 
by a single space.

@return				A handle to the attribute value.

@pre				The objects members have been pre-set accordingly.

*/
	{
	return iValue;
	}



EXPORT_C TAttributeType RAttribute::Type() const
/**
This method returns the attribute's type. 

@return				The attribute's type.

@pre				The objects members have been pre-set accordingly.

*/
	{
	return iType;
	}



EXPORT_C void RAttribute::SetValue(const RString& aValue)
/**
This method sets the attribute's value, taking ownership of the RString.

@param				aValue The attributes value.
*/
	{
	iValue.Close();
	iValue = aValue;
	}
