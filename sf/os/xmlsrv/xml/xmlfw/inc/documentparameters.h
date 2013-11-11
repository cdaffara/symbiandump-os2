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

#ifndef __DOCUMENTPARAMETERS_H__
#define __DOCUMENTPARAMETERS_H__


#include <stringpool.h>

namespace Xml
{

class RDocumentParameters
/**
The RDocumentParameters class contains information about the document to be passed to the client.

@publishedAll
@released
*/
	{
public:

/**
@publishedAll
@released
*/
	IMPORT_C RDocumentParameters();

/**
@publishedAll
@released
*/
	IMPORT_C void Close();

/**
@publishedAll
@released
*/
	IMPORT_C void Open(const RString& aCharacterSetName);

	IMPORT_C const RString& CharacterSetName() const;

private:


/**
@publishedAll
@released
*/
	RDocumentParameters(const RDocumentParameters& aOriginal);

/**
@publishedAll
@released
*/
	RDocumentParameters &operator=(const RDocumentParameters& aRhs);

private:

/**
The character encoding of the document.
The Internet-standard name of a character set, which is identified in Symbian OS by its UID
*/
	RString	iCharacterSetName;

	};

}

#endif
