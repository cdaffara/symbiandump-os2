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

#ifndef __TAGINFO_H__
#define __TAGINFO_H__

#include <stringpool.h>


namespace Xml
{

class RTagInfo
/**
The RTagInfo class holds information that describes an element or attribute.
@see MContentHandler
@see RAttribute

@publishedAll
@released
*/
	{
public:

/**
@publishedAll
@released
*/
	IMPORT_C RTagInfo();

/**
@publishedAll
@released
*/
	IMPORT_C RTagInfo Copy();

/**
@publishedAll
@released
*/
	IMPORT_C void Close();

/**
@publishedAll
@released
*/
	IMPORT_C void Open(const RString& aUri, const RString& aPrefix, const RString& aLocalName);
	
	IMPORT_C const RString& Uri() const;
	IMPORT_C const RString& LocalName() const;
	IMPORT_C const RString& Prefix() const;

private:

/**
Holds the namespace uri of the object it represents.
*/
	RString iUri;

/**
Holds the namespace prefix of the object it represents.
*/
	RString iPrefix;

/**
Holds the local name of the object it represents.
*/
	RString iLocalName;

	};

}

#endif //__TAGINFO_H__
