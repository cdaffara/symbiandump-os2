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


#include "intentparser.h"
#include "expiryparser.h"
#include "stringparser.h"
#include "drmrights.h"
#include "intent.h"
#include "rightsobject.h"
#include "rightscriteriacount.h"
#include "tags.h"
#include "rtapanic.h"

using namespace ReferenceTestAgent;
using namespace ContentAccess;
using namespace Xml;

CIntentParser* CIntentParser::NewL(CRightsObject& aRights)
	{
	CIntentParser* self = new (ELeave) CIntentParser(aRights);
	return self;
	}
		
CIntentParser::CIntentParser(CRightsObject& aRights) : iRights(aRights)
	{
	}

CIntentParser::~CIntentParser()
	{
	delete iIntent;
	}


void CIntentParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes)
	{
	TPtrC8 tag(aElement.LocalName().DesC());
	if(KIntentTag().CompareF(tag) == 0)
		{
		if(iIntent)
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: Start element <intent> found inside <intent> element "));
			#endif

			// there should not be an intent tag embedded within another intent tag
			User::Leave(KErrCorrupt);
			}
		
		HBufC8* type = NULL;
		TRAPD(err, type = GetTagAttributeL(aAttributes, KTypeTag()).AllocL());
		if(err != KErrNone)
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: start element <intent> missing \"type\" attribute"));
			#endif

			User::Leave(err);
			}
		CleanupStack::PushL(type);
		
		if(type->Des().CompareF(KIntentPeek()) == 0)
			{
			iIntent = CIntent::NewL(EPeek);
			}
		else if(type->Des().CompareF(KIntentPlay()) == 0)
			{
			iIntent = CIntent::NewL(EPlay);
			}
		else if(type->Des().CompareF(KIntentDisplay()) == 0)
			{
			iIntent = CIntent::NewL(EView);
			}
		else if(type->Des().CompareF(KIntentExecute()) == 0)
			{
			iIntent = CIntent::NewL(EExecute);
			}
		else if(type->Des().CompareF(KIntentPrint()) == 0)
			{
			iIntent = CIntent::NewL(EPrint);
			}
		else if(type->Des().CompareF(KIntentPause) == 0)
   			{
			iIntent = CIntent::NewL(EPause);
   			}
		else if(type->Des().CompareF(KIntentContinue) == 0)
   			{
			iIntent = CIntent::NewL(EContinue);
   			}
		else if(type->Des().CompareF(KIntentStop()) == 0)
			{
			iIntent = CIntent::NewL(EStop);
			}
		else if(type->Des().CompareF(KIntentUnknown()) == 0)
			{
			iIntent = CIntent::NewL(EUnknown);
			}
		else if(type->Des().CompareF(KIntentInstall()) == 0)
			{
			iIntent = CIntent::NewL(EInstall);
			}
		else
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: <intent> type \"%S\" not supported"), *type);
			#endif

			User::Leave(KErrCorrupt);
			}

		#ifdef _DEBUG
		LogL(_L("Start element <intent type=\"%S\">"),*type);	
		#endif

		CleanupStack::PopAndDestroy(type);
		}
	else if(KCountTag().CompareF(tag) == 0)
		{
		// <count>
		// create a parser for the count criteria

		#ifdef _DEBUG
		LogL(_L("Start element <count>"));				
		#endif

		iWaitingForTag = ECountTag;
		StartChildParserL(CStringParser::NewL(*this), aElement, aAttributes);
		}
	else if(KExpiryTag().CompareF(tag) == 0)
		{
		// <expiry>
		// create a parser for the expiry criteria
		StartChildParserL(CExpiryParser::NewL(*iIntent), aElement, aAttributes);
		}
	else
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: Start element <%S> not expected"), tag);
		#endif

		// unexpected start tag
		User::Leave(KErrCorrupt);	
		}
	}
		

void CIntentParser::ParseEndElementL(const RTagInfo& aElement)
	{
	// Check the end tag is </intent>
	if(KIntentTag().CompareF(aElement.LocalName().DesC()) != 0 || !iIntent)
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: Unexpected end element </%S>, expecting </intent>"), aElement.LocalName().DesC());	
		#endif

		User::Leave(KErrCorrupt);
		}
	
	#ifdef _DEBUG
	LogL(_L("  End element </intent>"));
	#endif			

	// Validate and add rights object to iDrmRights
	iRights.AddIntentL(iIntent);
	iIntent = NULL;
	}

void CIntentParser::StringParseCompleteL(const TDesC& aString)
	{
	MCriteria* countCriteria = NULL;
	TInt count = 0;
	TLex16 lex(aString);
	
	switch(iWaitingForTag)
		{
	case ECountTag:
		lex.Val(count);
		
		countCriteria = CRightsCriteriaCount::NewL(count);
		CleanupStack::PushL(countCriteria);
		iIntent->AddCriteriaL(countCriteria);
		CleanupStack::Pop(countCriteria);
		break;
	default:
		User::Panic(KRtaPanicString(), ERtaPanicParserInvalidState);
		break;
		}
	}
