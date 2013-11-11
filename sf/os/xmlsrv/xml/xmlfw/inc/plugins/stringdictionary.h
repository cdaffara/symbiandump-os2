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
// Plugin interface
// 
//


#ifndef __STRINGDICTIONARY_H__
#define __STRINGDICTIONARY_H__

#include <e32base.h>


class RString;

namespace Xml
{

class MStringDictionary
/**
This interface defines the API of a single string dictionary. 
This class is not used directly, but via a derived class the user provides.
Derived classes must add all elements, attributes names, and attribute values that belong
to this string dictionary as static string tables to the StringPool. WBXML token mappings between 
strings and WBXML tokens should also be maintained.
@see RStringPool

@publishedPartner
@released
*/
	{
public:

/**
This method allows for the correct destrution of the string dictionary plugin.

@post				the objects memory is cleaned up.

*/
	virtual void Release() = 0;

/**
This method returns the element string associated with the given token.

@param				aToken the element token.
@param				aElement On return reflects the string corresponding to the token.
@leave				KErrXmlUnsupportedElement, if the token doesn't correspond to an element.
*/
	virtual void ElementL(TInt aToken, RString& aElement) const = 0;

/**
This method returns the attribute string associated with the given token.

@param				aToken the attribute token.
@param				aAttribute On return reflects the string corresponding to the token.
@leave				KErrXmlUnsupportedAttribute, if the token doesn't correspond to an attribute.
*/
	virtual void AttributeL(TInt aToken, RString& aAttribute) const = 0;

/**
This method returns the attribute and value string associated with the given token.

@param				aToken the attribute token.
@param				aAttribute On return reflects the string corresponding to the token.
@param				aValue On return reflects the string corresponding to the value
					for this attibute.
@leave				KErrXmlUnsupportedAttribute, if the token doesn't correspond to an attribute 
@leave				KErrXmlUnsupportedAttributeValue, if the token doesn't correspond to a value 
*/
	virtual void AttributeValuePairL(TInt aToken, RString& aAttribute, RString& aValue) const = 0;

/**
This method returns the value string associated with the given token.

@param				aToken the attribute token.
@param				aValue On return reflects the string corresponding to the value
					for this attibute.
@leave				KErrXmlUnsupportedAttributeValue, if the token doesn't correspond to value.
*/
	virtual void AttributeValueL(TInt aToken, RString& aValue) const = 0;


/**
This method compares the Dictionary description with that provided.

@return				ETrue if this is the same Dictionary (i.e. a match), EFalse otherwise.

@param				aDictionaryDescription The description we want to compare.
*/
	virtual TBool CompareThisDictionary(const RString& aDictionaryDescription) const = 0;

/**
This method switches the dictionary to use the specified codepage.

@return				The codepage switched to, or KErrXmlMissingStringDictionary if it is not supported.

@param				aCodePage The code page to switch to.
*/
	virtual TInt SwitchCodePage(TInt aCodePage) = 0;
		
/**
This method obtains the public identifier from the StringTable.
Either the formal or non formal public id will do.
The stringDictionary .rss files must list both these as wbxml
documents have one or the other.
					
@param				aPubId The public identifier for this string 
					dictionary.
*/

	virtual void PublicIdentifier(RString& aPubId) = 0;
	
/**
The element types in the Device Information DTD are defined within
a namespace associated with the Uri/Urn available from the StringTable.
The RString need not be closed, but closing is harmless. 

@param				aUri The associated namespace for this string 
					dictionary.
*/

	virtual void NamespaceUri(RString& aUri) = 0;
	};

}

#endif //__STRINGDICTIONARY_H__
