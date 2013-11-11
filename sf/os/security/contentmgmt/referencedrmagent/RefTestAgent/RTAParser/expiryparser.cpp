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


#include "expiryparser.h"
#include "rightscriteriaexpiry.h"
#include "intent.h"
#include "tags.h"
#include "rtapanic.h"

using namespace ReferenceTestAgent;
using namespace Xml;

CExpiryParser* CExpiryParser::NewL(CIntent& aIntent)
	{
	CExpiryParser* self = new (ELeave) CExpiryParser(aIntent);
	return self;
	}
		
CExpiryParser::CExpiryParser(CIntent& aIntent) : iIntent(aIntent)
	{
	}

CExpiryParser::~CExpiryParser()
	{
	}


void CExpiryParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes)
	{
	TPtrC8 tag(aElement.LocalName().DesC());
	if(KExpiryTag().CompareF(tag) == 0)
		{
		if(iFoundStartTag)
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: start element <expiry> found inside <expiry> element"));
			#endif

			// We have already found the <expiry> tag, there shouldn't be two of them
			User::Leave(KErrCorrupt);
			}
			
		iFoundStartTag = ETrue;
		
		#ifdef _DEBUG
		LogL(_L("Start Element <expiry>"));	
		#endif
		}
	else if(KDayTag().CompareF(tag) == 0)
		{
		// <day>

		#ifdef _DEBUG
		LogL(_L("Start Element <day>"));
		#endif

		iWaitingForTag = EDayTag;
		StartChildParserL(CStringParser::NewL(*this), aElement, aAttributes);
		}
	else if(KMonthTag().CompareF(tag) == 0)
		{
		// <month>

		#ifdef _DEBUG
		LogL(_L("Start Element <month>"));	
		#endif

		iWaitingForTag = EMonthTag;
		StartChildParserL(CStringParser::NewL(*this), aElement, aAttributes);
		}
	else if(KYearTag().CompareF(tag) == 0)
		{
		// <year>
		
		#ifdef _DEBUG
		LogL(_L("Start Element <year>"));
		#endif

		iWaitingForTag = EYearTag;
		StartChildParserL(CStringParser::NewL(*this), aElement, aAttributes);
		}
	else
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: unexpected start element <%S>"),tag);
		#endif

		// unexpected start tag
		User::Leave(KErrCorrupt);	
		}
	}
		

void CExpiryParser::ParseEndElementL(const RTagInfo& aElement)
	{
	TPtrC8 tag(aElement.LocalName().DesC());
	
	// Check the end tag is </expiry> 
	if(KExpiryTag().CompareF(tag) != 0 || !iDay || !iMonth || !iYear)
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: Unexpected end element </%S>"), aElement.LocalName().DesC());	
		#endif

		User::Leave(KErrCorrupt);
		}

	#ifdef _DEBUG
	LogL(_L("  End element </expiry>"));						
	#endif

	TMonth month = EJanuary;
	switch(iMonth)
		{
		case 1:
			month = EJanuary;
			break;
		case 2:
			month = EFebruary;
			break;
		case 3:
			month = EMarch;
			break;
		case 4:
			month = EApril;
			break;
		case 5:
			month = EMay;
			break;
		case 6:
			month = EJune;
			break;
		case 7:
			month = EJuly;
			break;
		case 8:
			month = EAugust;
			break;
		case 9:
			month = ESeptember;
			break;
		case 10:
			month = EOctober;
			break;
		case 11:
			month = ENovember;
			break;
		case 12:
			month = EDecember;
			break;
		default:
			User::Leave(KErrCorrupt);
		}

	// TDateTime has zero based days (ie. December days 0 - 30)
	MCriteria* criteria = CRightsCriteriaExpiry::NewL(iDay - 1, month, iYear);
	CleanupStack::PushL(criteria);
	iIntent.AddCriteriaL(criteria);		
	CleanupStack::Pop(criteria);
	}

void CExpiryParser::StringParseCompleteL(const TDesC& aString)
	{
	TInt value = 0;
	TLex16 lex(aString);
	lex.Val(value);

	switch(iWaitingForTag)
		{
	case EDayTag:
		iDay = value;
		break;
	case EMonthTag:
		iMonth = value;
		break;
	case EYearTag:
		iYear = value;
		break;
	
	default:
		User::Panic(KRtaPanicString(), ERtaPanicParserInvalidState);
		break;
		}
	}
