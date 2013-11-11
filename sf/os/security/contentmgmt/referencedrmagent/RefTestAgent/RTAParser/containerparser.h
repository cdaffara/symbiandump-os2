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

#ifndef __CONTAINERPARSER_H__
#define __CONTAINERPARSER_H__

#include "parsehandlerbase.h"
#include "stringparser.h"

namespace ReferenceTestAgent
	{
	class CDrmFileContainer;
	
	class CContainerParser : public CParseHandlerBase, public MStringParser
		{
	public:
		// Operates on a container to parse subcontainers
		static CContainerParser* NewL(CDrmFileContainer& aCurrentContainer);
		~CContainerParser();

		virtual void ParseStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes);
		virtual void ParseEndElementL(const Xml::RTagInfo& aElement);
	
		// From MStringParser
		virtual void StringParseCompleteL(const TDesC& aString);
	private:
		CContainerParser(CDrmFileContainer& aCurrentContainer);

	private:
		TBool iFoundOwnContainerTag;
		TParserWaitingForTag iWaitingForTag;
		
		CDrmFileContainer& iCurrentContainer;
		HBufC* iName;
		HBufC* iUniqueId;
		};
	}
#endif

