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
#include "attributeparser.h"
#include "contentparser.h"
#include "tags.h"

using namespace ReferenceTestAgent;
using namespace ContentAccess;
using namespace Xml;

CAttributeParser* CAttributeParser::NewL(CContentParser& aContentParser)
	{
	CAttributeParser* self = new (ELeave) CAttributeParser(aContentParser);
	return self;
	}
		
CAttributeParser::CAttributeParser(CContentParser& aContentParser) : iContentParser(aContentParser)
	{
	}

CAttributeParser::~CAttributeParser()
	{
	delete iAttributeName;
	}

void CAttributeParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes)
	{
	TPtrC8 tag(aElement.LocalName().DesC());

	if(KAttributeTag().CompareF(tag) == 0)
		{
		// attribute tag has the form <attribute name="something">
		TRAPD(err, iAttributeName = GetTagAttributeL(aAttributes, KNameTag()).AllocL());
		if(err != KErrNone)
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: Start Element <attribute> missing \"name\" attribute"));
			#endif
			User::Leave(err);
			}	
		#ifdef _DEBUG
		LogL(_L("Start element <attribute name=\"%S\">"), *iAttributeName);
		#endif
		// This will parse the name and call StringParseCompleteL() when done
		StartChildParserL(CStringParser::NewL(*this), aElement, aAttributes);
		}
	else
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: Unexpected start element <%s> inside <attribute> element"), tag);
		#endif
		// Not expecting any further tags inside the attribute tag
		User::Leave(KErrCorrupt);
		}
	}

void CAttributeParser::ParseEndElementL(const RTagInfo& aElement)
	{
	// Check the end tag is </attribute>
	if(KAttributeTag().CompareF(aElement.LocalName().DesC()) != 0)
		{
		// We should only expect the end tag of the tag we are processing
		User::Leave(KErrCorrupt);
		}
	}


void CAttributeParser::StringParseCompleteL(const TDesC& aString)
	{	
	// List of all possible attributes
	_LIT8(KAttributeProtected, "protected");
	_LIT8(KAttributeForwardable, "forwardable");
	_LIT8(KAttributeModifyable, "modifyable");
	_LIT8(KAttributeCopyable, "copyable");
	_LIT8(KAttributeCanPlay, "canplay");
	_LIT8(KAttributeCanPrint, "canprint");
	_LIT8(KAttributeCanExecute, "canexecute");
	_LIT8(KAttributeCanView, "canview");
	_LIT8(KAttributeRightsNone, "rightsnone");
	_LIT8(KAttributeRightsPending, "rightspending");
	_LIT8(KAttributeRightsHaveExpired, "rightshaveexpired");
	_LIT8(KAttributePreviewAvailable, "previewavailable");
	_LIT8(KAttributeContentCDataInUse, "contentcdatainuse");
	_LIT8(KAttributeCanRewind, "canrewind");
	_LIT8(KAttributeCopyPaste, "copypaste");
	_LIT8(KAttributeRightsConsumable, "rightsconsumable");
	_LIT8(KAttributeRightsStateless, "rightsstateless");
	_LIT8(KAttributeCanMove, "canmove");
	_LIT8(KAttributeCanRename, "canrename");
	_LIT8(KAttributeCanAutomaticConsume, "canautomaticconsume");
	_LIT8(KAttributeContentVersion, "contentversion");
	
	TInt attribute = 0;
	TInt value = 0;
	TPtr8 attributeNamePtr = iAttributeName->Des();
	
	// see if the named attribute matches any we know
	if(attributeNamePtr.CompareF(KAttributeProtected()) == 0)
		{
		attribute = EIsProtected;		
		}
	else if(attributeNamePtr.CompareF(KAttributeForwardable()) == 0)
		{
		attribute = EIsForwardable;		
		}
	else if(attributeNamePtr.CompareF(KAttributeModifyable()) == 0)
		{
		attribute = EIsModifyable;		
		}
	else if(attributeNamePtr.CompareF(KAttributeCopyable()) == 0)
		{
		attribute = EIsCopyable;		
		}
	else if(attributeNamePtr.CompareF(KAttributeCanPlay()) == 0)
		{
		attribute = ECanPlay;		
		}
	else if(attributeNamePtr.CompareF(KAttributeCanPrint()) == 0)
		{
		attribute = ECanPrint;		
		}
	else if(attributeNamePtr.CompareF(KAttributeCanExecute()) == 0)
		{
		attribute = ECanExecute;		
		}
	else if(attributeNamePtr.CompareF(KAttributeCanView()) == 0)
		{
		attribute = ECanView;		
		}
	else if(attributeNamePtr.CompareF(KAttributeRightsNone()) == 0)
		{
		attribute = ERightsNone;		
		}
	else if(attributeNamePtr.CompareF(KAttributeRightsPending()) == 0)
		{
		attribute = ERightsPending;		
		}
	else if(attributeNamePtr.CompareF(KAttributeRightsHaveExpired()) == 0)
		{
		attribute = ERightsHaveExpired;		
		}
	else if(attributeNamePtr.CompareF(KAttributePreviewAvailable()) == 0)
		{
		attribute = EPreviewAvailable;		
		}
	else if(attributeNamePtr.CompareF(KAttributeContentCDataInUse()) == 0)
		{
		attribute = EContentCDataInUse;		
		}
	else if(attributeNamePtr.CompareF(KAttributeCanRewind()) == 0)
		{
		attribute = ECanRewind;		
		}
	else if(attributeNamePtr.CompareF(KAttributeCopyPaste()) == 0)
		{
		attribute = ECopyPaste;		
		}
	else if(attributeNamePtr.CompareF(KAttributeRightsConsumable()) == 0)
		{
		attribute = ERightsConsumable;		
		}
	else if(attributeNamePtr.CompareF(KAttributeRightsStateless()) == 0)
		{
		attribute = ERightsStateless;		
		}
	else if(attributeNamePtr.CompareF(KAttributeCanMove()) == 0)
		{
		attribute = ECanMove;		
		}
	else if(attributeNamePtr.CompareF(KAttributeCanRename()) == 0)
		{
		attribute = ECanRename;		
		}
	else if(attributeNamePtr.CompareF(KAttributeCanAutomaticConsume()) == 0)
		{
		attribute = ECanAutomaticConsume;		
		}
	else if(attributeNamePtr.CompareF(KAttributeContentVersion()) == 0)
		{
		attribute = EContentVersion;		
		}
	else
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: attribute %S not supported"), attributeNamePtr);
		#endif
		User::Leave(KErrCorrupt);		
		}

	// check boolean
			
	// Convert value to an integer
	TLex16 lex(aString);
	User::LeaveIfError(lex.Val(value));
	iContentParser.SetAttributeL(attribute, value);
	}
