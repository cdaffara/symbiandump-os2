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


#ifndef __INTENTPARSER_H__
#define __INTENTPARSER_H__

#include <caf/caf.h>
#include "parsehandlerbase.h"
#include "stringparser.h"

namespace ReferenceTestAgent
	{
	class CRightsObject;
	class CIntent;
	
	_LIT8(KIntentPeek, "peek");
	_LIT8(KIntentPlay, "play");
	_LIT8(KIntentDisplay, "display");
	_LIT8(KIntentExecute, "execute");
	_LIT8(KIntentPrint, "print");
	_LIT8(KIntentPause, "pause");
	_LIT8(KIntentContinue, "continue");
	_LIT8(KIntentStop, "stop");
	_LIT8(KIntentUnknown, "unknown");
	_LIT8(KIntentInstall, "install");
	
	/** parser for the <Intent> tag
	*/
	class CIntentParser : public CParseHandlerBase, public MStringParser
		{
	public:
		static CIntentParser* NewL(CRightsObject& aRights);
		virtual ~CIntentParser();
		
		virtual void ParseStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes);
		virtual void ParseEndElementL(const Xml::RTagInfo& aElement);
		
		// From MStringParser
		virtual void StringParseCompleteL(const TDesC& aString);

	private:
		CIntentParser(CRightsObject& aRights);

	private:
		TBool iFoundStartTag;
		CRightsObject& iRights;
		CIntent* iIntent;
		TParserWaitingForTag iWaitingForTag;
		TInt iCriteriaAdded;
		};
	}

#endif
