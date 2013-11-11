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


#ifndef __EXPIRYPARSER_H__
#define __EXPIRYPARSER_H__

#include "parsehandlerbase.h"
#include "stringparser.h"

namespace ReferenceTestAgent
	{
	class CIntent;
	
	/** parser for the <expiry> tag
	*/
	class CExpiryParser : public CParseHandlerBase, public MStringParser
		{
	public:
		static CExpiryParser* NewL(CIntent& aIntent);
		virtual ~CExpiryParser();
		
		virtual void ParseStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes);
		virtual void ParseEndElementL(const Xml::RTagInfo& aElement);
		
		// From MStringParser
		virtual void StringParseCompleteL(const TDesC& aString);

	private:
		CExpiryParser(CIntent& aIntent);

	private:
		TBool iFoundStartTag;
		CIntent& iIntent;
		TParserWaitingForTag iWaitingForTag;

		TInt iDay;
		TInt iMonth;
		TInt iYear;
		};
	}

#endif
