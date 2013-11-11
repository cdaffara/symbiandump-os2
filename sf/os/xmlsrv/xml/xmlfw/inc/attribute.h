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

#ifndef __ATTRIBUTE_H__
#define __ATTRIBUTE_H__

#include <xml/taginfo.h>

namespace Xml
{

enum TAttributeType
/**
Specifies the type of this attribute object.
These values are defined in the xml specification.
For other specifications they may also be used or 'none'.
@see http://www.w3.org/TR/REC-xml#sec-attribute-types

@publishedAll
@released
*/
	{
/**
Specifies a string type.
*/
	EAttributeType_CDATA,

/**
Specifies a tokenized type.
*/
	EAttributeType_ID,

/**
Specifies a tokenized type.
*/		
	EAttributeType_IDREF,

/**
Specifies a tokenized type.
*/	
	EAttributeType_IDREFS,

/**
Specifies a tokenized type.
*/	
	EAttributeType_NMTOKEN,

/**
Specifies a tokenized type.
*/	
	EAttributeType_NMTOKENS,

/**
Specifies a tokenized type.
*/	
	EAttributeType_ENTITY,

/**
Specifies a tokenized type.
*/	
	EAttributeType_ENTITIES,

/**
Specifies a tokenized type.
*/	
	EAttributeType_NOTATION,

/**
Specifies no type, or unsupported.
*/	
	EAttributeType_NONE,

	};




class RAttribute
/**
The RAttribute class holds an attribute's description belonging to an element.
@see RAttributeArray

@publishedAll
@released
*/
	{
public:

/**
@publishedPartner
@released
*/
	IMPORT_C RAttribute();

/**
@publishedPartner
@released
*/
	IMPORT_C RAttribute Copy();

/**
@publishedPartner
@released
*/
	IMPORT_C void Close();

/**
@publishedPartner
@released
*/
	IMPORT_C void Open(const RString& aUri, const RString& aPrefix, const RString& aLocalName,
					   const TAttributeType aType = EAttributeType_NONE);
					   
/**
@publishedPartner
@released
*/
	IMPORT_C void Open(const RString& aUri, const RString& aPrefix, const RString& aLocalName,
					   const RString& aValue, const TAttributeType aType = EAttributeType_NONE);

	IMPORT_C const RTagInfo& Attribute() const;
	IMPORT_C const RString& Value() const;
	IMPORT_C TAttributeType Type() const;

/**
@publishedPartner
@released
*/
	IMPORT_C void SetValue(const RString& aValue);

private:

/**
Holds information about the attribute.
*/
	RTagInfo iAttribute;

/**
Holds the attribute's value.
*/
	RString iValue;

/**
Holds the attribute's type.
*/
	TAttributeType iType;

	};


/**
Defines a list of attributes for an element.
@see MContentHandler

@publishedAll
@released
*/
typedef RArray<RAttribute> RAttributeArray; 

}

#endif //__ATTRIBUTE_H__
