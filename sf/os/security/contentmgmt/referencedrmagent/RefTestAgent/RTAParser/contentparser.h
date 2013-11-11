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

#ifndef __CONTENTPARSER_H__
#define __CONTENTPARSER_H__

#include <caf/attributeset.h>
#include <caf/stringattributeset.h>
#include "parsehandlerbase.h"
#include "stringparser.h"

namespace ReferenceTestAgent
	{
	class CDrmFileContainer;

	/** Parses everything between the <Content> </Content> tags
	and writes the results to the relevant CDrmFileContent object
	*/
	class CContentParser : public CParseHandlerBase, public MStringParser
		{
	public:
		static CContentParser* NewL(CDrmFileContainer& aCurrentContainer);
		virtual ~CContentParser();

		// Overrides CParseHandlerBase functionality
		virtual void ParseStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes);
		virtual void ParseEndElementL(const Xml::RTagInfo& aElement);
			
		// From MStringParser
		virtual void StringParseCompleteL(const TDesC& aString);
		
		void SetAttributeL(TInt aAttribute, TInt aValue);
		void SetStringAttributeL(TInt aStringAttribute, const TDesC& aValue);
	
	private:
		CContentParser(CDrmFileContainer& aCurrentContainer);

	private:
		TParserWaitingForTag iWaitingForTag;
		
		CDrmFileContainer& iCurrentContainer;
		HBufC* iName;
		HBufC* iUniqueId;
		HBufC* iFileName;
		HBufC8* iCid;
		ContentAccess::RAttributeSet iAttributeSet;
		ContentAccess::RStringAttributeSet iStringAttributeSet;
		};
	}

#endif
