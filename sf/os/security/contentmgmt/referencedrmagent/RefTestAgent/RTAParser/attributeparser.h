/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @internalComponent
 @released
*/


#ifndef __ATTRIBUTEPARSER_H__
#define __ATTRIBUTEPARSER_H__

#include <e32base.h>
#include "parsehandlerbase.h"
#include "stringparser.h"

namespace ReferenceTestAgent
	{
	class CContentParser;

	/** Parser to extract an attribute tag
	<attribute name="x">
	value 
	</attribute>
	*/
	class CAttributeParser : public CParseHandlerBase, public MStringParser
		{
	public:
		static CAttributeParser* NewL(CContentParser& aDrmFileContent);
		virtual ~CAttributeParser();
		
		// Overrides CParseHandlerBase behavior.
		virtual void ParseStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes);
		virtual void ParseEndElementL(const Xml::RTagInfo& aElement);

		// From MStringParser
		virtual void StringParseCompleteL(const TDesC& aString);

	private:
		CAttributeParser(CContentParser& aDrmFileContent);

	private:
		HBufC8* iAttributeName;
		CContentParser& iContentParser;
		};
	}

#endif
