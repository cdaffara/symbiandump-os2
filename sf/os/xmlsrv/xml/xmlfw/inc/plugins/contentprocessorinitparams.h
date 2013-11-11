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

#ifndef __CONTENTPROCESSORINITPARAMS_H__
#define __CONTENTPROCESSORINITPARAMS_H__

#include <xml/contenthandler.h>
#include <xml/plugins/elementstack.h> // needed for the typedef


namespace Xml
{

class RStringDictionaryCollection;

struct TContentProcessorInitParams
/**
A structure for passing initialisation parameters to the derived class being instantiated.

@publishedPartner
@released
*/
	{
/**
The client callback to pass the data to.
We do not own this.
*/
	MContentHandler*					iContentHandler;

/**
The collection of string dictionaries that can be loaded by the user or by the framework at runtime.
We do not own this.
*/
	RStringDictionaryCollection*		iStringDictionaryCollection;

/**
The Element stack that allows checking on tag ordering.
An object may want to check this in associating with validation.
We do not own this.
*/
	RElementStack*						iElementStack;

	};

}

#endif //__CONTENTPROCESSORINITPARAMS_H__
