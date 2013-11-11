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


#ifndef __STRINGPARSER_H__
#define __STRINGPARSER_H__

#include "parsehandlerbase.h"

namespace ReferenceTestAgent
	{
	/** Enums to describe the string the parser is waiting for 
	*/
	enum TParserWaitingForTag
			{
			ENameTag		= 1,
			EUniqueIdTag	= 2,
			EFileNameTag	= 3,
			EForwardLockTag = 4,
			EIntentTag      = 5,
			EPendingTag     = 6,
			ECountTag		= 7,
			EDayTag			= 8,
			EMonthTag		= 9,
			EYearTag		= 10
			};
	
	/** Callback interface used when the string parser finishes parsing the string 
	*/
	class MStringParser
		{
	public:
		virtual void StringParseCompleteL(const TDesC& aString) = 0;
		};

	/** Generic parser to extract the text between any tags containing a string
	<anytag> string </anytag>

	On finding the closing tag calls iObserver->StringParseCompleteL("string"); 
	*/
	class CStringParser : public CParseHandlerBase
		{
	public:
		static CStringParser* NewL(MStringParser& aObserver);
		virtual ~CStringParser();
		
		// Overrides CParseHandlerBase behavior.
		virtual void ParseStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes);
		virtual void ParseEndElementL(const Xml::RTagInfo& aElement);
		virtual void ParseContentL(const TDesC8& aBytes);
	private:
		CStringParser(MStringParser& aObserver);

		TPtrC8 iTag;
		HBufC8* iBuffer; // stores the results from ParseContentL()
		MStringParser& iObserver;
		};
	}

#endif
