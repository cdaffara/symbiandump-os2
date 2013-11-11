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
* stringparser.cpp
*
*/


#include <caf/caftypes.h>
#include "stringattributeparser.h"
#include "stringparser.h"
#include "contentparser.h"
#include "tags.h"

using namespace ReferenceTestAgent;
using namespace ContentAccess;
using namespace Xml;

CStringAttributeParser* CStringAttributeParser::NewL(CContentParser& aContentParser)
	{
	CStringAttributeParser* self = new (ELeave) CStringAttributeParser(aContentParser);
	return self;
	}
		
CStringAttributeParser::CStringAttributeParser(CContentParser& aContentParser) : iContentParser(aContentParser)
	{
	}

CStringAttributeParser::~CStringAttributeParser()
	{
	delete iStringAttributeName;
	}

void CStringAttributeParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes)
	{
	TPtrC8 tag(aElement.LocalName().DesC());

	if(KStringAttributeTag().CompareF(tag) == 0)
		{
		// attribute tag has the form <attribute name="something">
		TRAPD(err, iStringAttributeName = GetTagAttributeL(aAttributes, KNameTag()).AllocL());
		if(err != KErrNone)
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: <stringattribute> element missing \"name\" attribute"));			
			#endif

			User::Leave(err);
			}
		#ifdef _DEBUG
		LogL(_L("Start element <stringattribute name=\"%S\">"), *iStringAttributeName);
		#endif

		// This will parse the name and call StringParseCompleteL() when done
		StartChildParserL(CStringParser::NewL(*this), aElement, aAttributes);
		}
	else
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: Start element <%S> not expected "), tag);			
		#endif

		// Not expecting any further tags inside the attribute tag
		User::Leave(KErrCorrupt);
		}
	}

void CStringAttributeParser::ParseEndElementL(const RTagInfo& aElement)
	{
	TPtrC8 tag(aElement.LocalName().DesC());
	
	// Check the end tag is </attribute>
	if(KAttributeTag().CompareF(tag) != 0)
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: Unexpected End element </%S>, expecting </stringattribute>"), tag);			
		#endif

		// We should only expect the end tag of the tag we are processing
		User::Leave(KErrCorrupt);
		}
	}


void CStringAttributeParser::StringParseCompleteL(const TDesC& aString)
	{	
	// List of all possible string attributes
	_LIT8(KStringAttributeDescription, "description");
	_LIT8(KStringAttributeMimeType, "mimetype");
	_LIT8(KStringAttributeTransactionID, "transactionid");
	_LIT8(KStringAttributeTitle, "title");
	_LIT8(KStringAttributeAuthor, "author");
	_LIT8(KStringAttributeIconUri, "iconuri");	
	_LIT8(KStringAttributePreviewUri, "previewuri");	
	_LIT8(KStringAttributeContentUri, "contenturi");		
	_LIT8(KStringAttributeContentId, "contentid");		
	_LIT8(KStringAttributeInfoUrl, "infourl");	
	_LIT8(KStringAttributePendingRightsEta, "pendingrightseta");
	_LIT8(KStringAttributeContentLocation, "contentlocation");
	_LIT8(KStringAttributeRightsIssuerURI, "rightsissueruri");
			
	
	TInt stringAttribute = 0;
	TPtr8 stringAttributeNamePtr = iStringAttributeName->Des();
	
	// see if the named attribute matches any we know
	if(stringAttributeNamePtr.CompareF(KStringAttributeDescription()) == 0)
		{
		stringAttribute = EDescription;		
		}
	else if(stringAttributeNamePtr.CompareF(KStringAttributeMimeType()) == 0)
		{
		stringAttribute = EMimeType;		
		}
	else if(stringAttributeNamePtr.CompareF(KStringAttributeTransactionID()) == 0)
		{
		stringAttribute = ETransactionID;		
		}	
	else if(stringAttributeNamePtr.CompareF(KStringAttributeTitle()) == 0)
		{
		stringAttribute = ETitle;		
		}
	else if(stringAttributeNamePtr.CompareF(KStringAttributeAuthor()) == 0)
		{
		stringAttribute = EAuthor;		
		}
	else if(stringAttributeNamePtr.CompareF(KStringAttributeIconUri()) == 0)
		{
		stringAttribute = EIconURI;		
		}
	else if(stringAttributeNamePtr.CompareF(KStringAttributePreviewUri()) == 0)
		{
		stringAttribute = EPreviewURI;		
		}
	else if(stringAttributeNamePtr.CompareF(KStringAttributeContentUri()) == 0)
		{
		stringAttribute = EContentURI;		
		}
	else if(stringAttributeNamePtr.CompareF(KStringAttributeContentId()) == 0)
		{
		stringAttribute = EContentID;		
		}
	else if(stringAttributeNamePtr.CompareF(KStringAttributeInfoUrl()) == 0)
		{
		stringAttribute = EInfoURL;		
		}
	else if(stringAttributeNamePtr.CompareF(KStringAttributePendingRightsEta()) == 0)
		{
		stringAttribute = EPendingRightsETA;		
		}
	else if(stringAttributeNamePtr.CompareF(KStringAttributeContentLocation()) == 0)
		{
		stringAttribute = EContentLocation;		
		}
	else if(stringAttributeNamePtr.CompareF(KStringAttributeRightsIssuerURI()) == 0)
		{
		stringAttribute = ERightsIssuerURI;		
		}
	else
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: stringattribute %S not supported"), stringAttributeNamePtr);
		#endif
		
		User::Leave(KErrCorrupt);		
		}
	
	iContentParser.SetStringAttributeL(stringAttribute, aString);
	}
