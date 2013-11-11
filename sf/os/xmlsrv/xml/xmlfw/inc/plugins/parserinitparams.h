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

#ifndef __PARSERINITPARAMS_H__
#define __PARSERINITPARAMS_H__

#include <xml/plugins/elementstack.h> // needed for the typedef

namespace Xml
{

class CCharSetConverter;
class MContentHandler;
class RStringDictionaryCollection;


struct TParserInitParams
/**
A structure for passing initialisation parameters to the derived class being instantiated.

@publishedPartner
@released
*/
	{

/**
The helper class for character conversions and encoding resolution.
We do not own this.
*/	
	CCharSetConverter*				iCharSetConverter;

/**
The client callback to pass the data to.
We do not own this.
*/
	MContentHandler*				iContentHandler;

/**
The collection of string dictionaries that can be loaded by the user or by the framework at runtime.
We do not own this.
*/
	RStringDictionaryCollection*	iStringDictionaryCollection;

/**
The Element stack that allows checking on tag ordering.
An object may want to check this in associating with validation.
We do not own this.
*/
	RElementStack*					iElementStack;

	};

}

#endif //__PARSERINITPARAMS_H__
