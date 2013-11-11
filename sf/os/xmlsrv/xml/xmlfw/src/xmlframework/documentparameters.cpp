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

#include <xml/documentparameters.h>

using namespace Xml;


EXPORT_C RDocumentParameters::RDocumentParameters()
/**
Default Constructor

Only Parsers have this access.
@see CParser
*/
	{
	// do nothing;
	}



EXPORT_C void RDocumentParameters::Close()
/**
This method cleans up the object before destruction. It releases all resources in
accordance to the R Class pattern.

@post				This object may be allowed to go out of scope.

Only Parsers have this access.
@see CParser

*/
	{
	iCharacterSetName.Close();
	}



EXPORT_C void RDocumentParameters::Open(const RString& aCharacterSetName)
/**
This method opens and sets all the parameters contents.
We do not own any resources.

@pre				The object has just been constructed.
@post				The objects members have been set to the values given.

@param				aCharacterSetName the Internet-standard name of a character set, 
					which is identified in Symbian OS by its UID.

Only Parsers have this access.
@see CParser

*/
	{
	iCharacterSetName = aCharacterSetName;
	}



EXPORT_C const RString& RDocumentParameters::CharacterSetName() const
/**
This method returns a handle to the character encoding name, which is 
the Internet-standard name of a character set, which is identified in Symbian OS by its UID.

@return				The character encoding name.

@pre				The objects member have been pre-set accordingly.

*/
	{
	return iCharacterSetName;
	}
