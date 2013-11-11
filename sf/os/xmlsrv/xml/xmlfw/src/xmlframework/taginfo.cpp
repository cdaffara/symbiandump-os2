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

#include <xml/taginfo.h>

using namespace Xml;

EXPORT_C RTagInfo::RTagInfo()
/**
Default constructor

Only Parsers have this access.
@see CParser

*/
	{
	// do nothing;
	}
	
	
EXPORT_C RTagInfo RTagInfo::Copy()
/**
Copy method. The original and copy must be closed seperately.

Only Parsers have this access.
@see CParser

*/
	{
	RTagInfo copy;
	copy.Open(iUri.Copy(), iPrefix.Copy(), iLocalName.Copy());
	return copy;
	}
	
	
EXPORT_C void RTagInfo::Close()
/**
This method cleans up the object before destruction. It releases all resources in
accordance to the R Class pattern.

@post				This object may be allowed to go out of scope.

Only Parsers have this access.
@see CParser

*/
	{
	iUri.Close();
	iPrefix.Close();
	iLocalName.Close();
	}



EXPORT_C void RTagInfo::Open(const RString& aUri, const RString& aPrefix, const RString& aLocalName)
/**
This method sets the object's contents, taking ownership of the RStrings.

@post				The objects members have been set to the values given.

@param				aUri the uri of the object.
@param				aPrefix the prefix of the object.
@param				aLocalName the localName of the object.

Only Parsers have this access.
@see CParser

*/
	{
	iUri = aUri;
	iPrefix = aPrefix;
	iLocalName = aLocalName;
	}



EXPORT_C const RString& RTagInfo::Uri() const
/**
This method obtains the uri member of this object.

@return				the uri member.

*/
	{
	return iUri;
	}



EXPORT_C const RString& RTagInfo::LocalName() const
/**
This method obtains the LocalName member of this object.

@return				the LocalName member.

*/
	{
	return iLocalName;
	}


	
EXPORT_C const RString& RTagInfo::Prefix() const
/**
This method obtains the Prefix member of this object.

@return				the Prefix member.

*/
	{
	return iPrefix;
	}
