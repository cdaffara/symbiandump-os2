// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the TEComResolverParams class.
// Provide the inline implementation of TEComResolverParams 
// TEComResolverParams allows the user to define the characteristics
// of the Interface Implementation plugin to be found.
// Default constructor of TEComResolverParams. It creates an empty uninitialized
// resolver parameter object. Such an object maybe used in calls to REComSession
// CreateImplementationL() and ListImplementationsL() methods when no specific 
// matching or filtering is required i.e. default type matching will be used.
// 
//

TEComResolverParams::TEComResolverParams()
:	iDataType(NULL,0),
	iGenericMatch(EFalse)
	{
	// do nothing;
	}


/**
Provides read access to the Interface Implementation plugin 'datatype' match 
pattern. Note, since TEComResolveParams has a default constructor this data 
member may be an invalid descriptor which implies 'default matching'.

@return			The read only 'datatype' match pattern.
@pre 			The object is constructed
*/

const TDesC8& TEComResolverParams::DataType() const
	{
	return iDataType;
	}


/**
Set the Interface Implementation plugin 'datatype' match pattern.

@param			aDataType The 'datatype' match pattern to be stored.
@pre 			The object is constructed
@post			iDataType equals aDataType.
*/

void TEComResolverParams::SetDataType(const TDesC8& aDataType)
	{
	iDataType.Set(aDataType);
	}

/**
Check if 'generic' matching is allowed.

@return			ETrue if generic matching is allowed, EFalse otherwise.
@pre 			The object is constructed
*/
TBool TEComResolverParams::IsGenericMatch() const
	{
	return iGenericMatch;
	}

/**
Indicates that a 'generic' match is required.

Either allow Interface Implementation plugins that are specific (use no wildcards
in their registry file) or generic (use wildcards in their registry file).

In any case the user of a should NOT use wildcards in the datatype string that
is passed into this object.

Rationale:
The client that sets up the resolver is expected to know what type of data
it is handling.

Use Case:
"I have this gif to convert, but I'd prefer only gif-specific plugins" or 
"I have this gif to convert, but I'm happy with some generic image conversion
plugin".

NOT:
"I have this image I want some plugin to convert, but I don't know the type
of the image".

@param			aGenericMatch ETrue if a generic match is required, EFalse if not.
@pre 			The object is constructed
@post			iGenericMatch equals aGenericMatch.
*/
void TEComResolverParams::SetGenericMatch(TBool aGenericMatch)
	{
	iGenericMatch=aGenericMatch;
	}
	

/**
@deprecated
@see			IsGenericMatch
*/

TBool TEComResolverParams::IsWildcardMatch() const
	{
	return iGenericMatch;
	}


/**
@deprecated
@see			SetGenericMatch
*/

void TEComResolverParams::SetWildcardMatch(TBool aWildcardMatch)
	{
	iGenericMatch=aWildcardMatch;
	}




