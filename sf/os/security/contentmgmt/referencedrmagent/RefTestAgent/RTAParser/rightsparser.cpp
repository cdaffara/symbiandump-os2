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
* drmrightsparser.cpp
*
*/


#include "rightsparser.h"
#include "intentparser.h"
#include "stringparser.h"
#include "drmrights.h"
#include "rightsobject.h"
#include "tags.h"
#include "rtapanic.h"

using namespace ReferenceTestAgent;
using namespace Xml;


CRightsParser* CRightsParser::NewL(CDrmRights& aDrmRights)
	{
	CRightsParser* self = new (ELeave) CRightsParser(aDrmRights);
	return self;
	}
		
CRightsParser::CRightsParser(CDrmRights& aDrmRights) : iDrmRights(aDrmRights)

	{
	}

CRightsParser::~CRightsParser()
	{
	delete iCid;
	delete iRights;
	}


void CRightsParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes)
	{
	TPtrC8 tag(aElement.LocalName().DesC());
	if(KRightsTag().CompareF(tag) == 0)
		{
		if(iCid)
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: <rights> element not expected inside <rights> element"));
			#endif
			
			// We have already found the <rights> tag, there shouldn't be two of them
			User::Leave(KErrCorrupt);
			}
			
		TRAPD(err, iCid = GetTagAttributeL(aAttributes, KCidTag()).AllocL());
		if(err != KErrNone)
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: Start element <rights> missing \"cid\" attribute"));	
			#endif

			User::Leave(err);
			}

		#ifdef _DEBUG
		LogL(_L("Start Element <rights cid=\"%S\">"),*iCid);
		#endif
		}
	else if(KForwardLockTag().CompareF(tag) == 0)
		{
		if(iRights)
			{
		#ifdef _DEBUG
			LogL(_L("  Only one of <forwardlock>, <pending> or <intent> can appear inside a single rights element"));
		#endif

			// must be too many tags within the rights tag
			User::Leave(KErrCorrupt);
			}

		// <forwardlock>
		// create a parser for forwardlock element
		#ifdef _DEBUG
		LogL(_L("Start Element <forwardlock>"));
		#endif

		iWaitingForTag = EForwardLockTag;
		StartChildParserL(CStringParser::NewL(*this), aElement, aAttributes);
		}
	else if(KPendingTag().CompareF(tag) == 0)
		{
		if(iRights)
			{
		#ifdef _DEBUG
			LogL(_L("  Only one of <forwardlock>, <pending> or <intent> can appear inside a single rights element"));
		#endif

			// must be too many tags within the rights tag
			User::Leave(KErrCorrupt);
			}

		// <pending>
		// create a parser for forwardlock element
		#ifdef _DEBUG
		LogL(_L("Start Element <pending>"));
		#endif

		iWaitingForTag = EPendingTag;
		StartChildParserL(CStringParser::NewL(*this), aElement, aAttributes);
		}
	else if(KIntentTag().CompareF(tag) == 0)
		{
		if(iRights)
			{
			#ifdef _DEBUG
			LogL(_L("  Only one of <forwardlock>, <pending> or <intent> can appear inside a single rights element"));
			#endif

			// must be too many tags within the rights tag
			User::Leave(KErrCorrupt);
			}

		// <intent>
		// create a parser for the intent tag
		iRights = CRightsObject::NewL(*iCid, EFalse, EFalse);
		StartChildParserL(CIntentParser::NewL(*iRights), aElement, aAttributes);
		}
	else
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: Unexpected Start Element <%S>"), tag);
		#endif
		
		// unexpected start tag
		User::Leave(KErrCorrupt);	
		}
	}
		

void CRightsParser::ParseEndElementL(const RTagInfo& aElement)
	{
	// Check the end tag is </rights> and that we now have a rights element we can add
	if(KRightsTag().CompareF(aElement.LocalName().DesC()) != 0 || !iRights)
		{
		#ifdef _DEBUG
		LogL(_L("  End Element <%S> not expected"), aElement.LocalName().DesC());
		#endif

		User::Leave(KErrCorrupt);
		}
	
	#ifdef _DEBUG
	LogL(_L("  End Element </rights>"));
	#endif

	// Validate and add rights object to iDrmRights
	iDrmRights.AddL(iRights);
	iRights = NULL;
	}

void CRightsParser::StringParseCompleteL(const TDesC& aString)
	{
	TInt delay = 0;
	TLex16 lex(aString);
	
	switch(iWaitingForTag)
		{
	case EForwardLockTag:
		iRights = CRightsObject::NewL(*iCid, ETrue, EFalse);
		break;
	case EPendingTag:
		lex.Val(delay);
		iRights = CRightsObject::NewL(*iCid, EFalse, ETrue, delay);
		break;

	default:
		User::Panic(KRtaPanicString(), ERtaPanicParserInvalidState);
		break;
		}
	}
