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


#ifndef __RIGHTSPARSER_H__
#define __RIGHTSPARSER_H__

#include "parsehandlerbase.h"
#include "stringparser.h"

namespace ReferenceTestAgent
	{
	class CDrmRights;
	class CRightsObject;
	
	/** parser for the <Rights> tag
	*/
	class CRightsParser : public CParseHandlerBase, public MStringParser
		{
	public:
		static CRightsParser* NewL(CDrmRights& aDrmFiles);
		virtual ~CRightsParser();
		
		virtual void ParseStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes);
		virtual void ParseEndElementL(const Xml::RTagInfo& aElement);
		
		// From MStringParser
		virtual void StringParseCompleteL(const TDesC& aString);

	private:
		CRightsParser(CDrmRights& aDrmRights);

	private:
		CDrmRights& iDrmRights;
		HBufC8 *iCid;
		TParserWaitingForTag iWaitingForTag;
		CRightsObject* iRights;
		};
	}

#endif
