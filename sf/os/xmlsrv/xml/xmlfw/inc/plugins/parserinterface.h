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


#ifndef __PARSERINTERFACE_H__
#define __PARSERINTERFACE_H__

/** 
@file
@publishedPartner
@released
*/

#include <e32std.h>
#include <xml/contentsource.h>

namespace Xml
{

/** A parser ECOM plugin for the XML Framework must implement this interface. */
class MParser : public MContentSource
	{
public:
	/** Enable a feature. */
	virtual TInt EnableFeature(TInt aParserFeature) = 0;
	/** Disable a feature. */
	virtual TInt DisableFeature(TInt aParserFeature) = 0;
	/** See if a feature is enabled. */
	virtual TBool IsFeatureEnabled(TInt aParserFeature) const = 0;

	/** Parses a descriptor that contains part of a document. */
	virtual void ParseChunkL(const TDesC8& aChunk) = 0;

	/** Parses a descriptor that contains the last  part of a document. */
	virtual void ParseLastChunkL(const TDesC8& aFinalChunk) = 0;

	/** Interfaces don't have a destructor, so we have an explicit method instead. */
	virtual void Release() = 0;
	};

}

#endif //__PARSERINTERFACE_H__
