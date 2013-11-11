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


#include "contentparser.h"
#include "attributeparser.h"
#include "stringattributeparser.h"
#include "stringparser.h"
#include "drmfilecontent.h"
#include "drmfilecontainer.h"
#include "tags.h"
#include "rtapanic.h"
#include <caf/caftypes.h>
#include <caf/caferr.h>

using namespace ContentAccess;
using namespace ReferenceTestAgent;
using namespace Xml;


CContentParser* CContentParser::NewL(CDrmFileContainer& aCurrentContainer)
	{
	CContentParser* self = new (ELeave) CContentParser(aCurrentContainer);
	return self;
	}

CContentParser::CContentParser(CDrmFileContainer& aCurrentContainer) : iCurrentContainer(aCurrentContainer)
	{	
	}

CContentParser::~CContentParser()
	{
	delete iCid;
	delete iName;
	delete iUniqueId;
	delete iFileName;
	
	iAttributeSet.Close();
	iStringAttributeSet.Close();
	}

void CContentParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes)
	{
	TPtrC8 tag(aElement.LocalName().DesC());

	if(KContentTag().CompareF(tag) == 0)
		{
		if(iCid)
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: <content> found within a <content>"));	
			#endif

			// Should not find a content tag within a content tag
			User::Leave(KErrCorrupt);
			}
		// content tag has the form <content cid="something">
		TRAPD(err, iCid = GetTagAttributeL(aAttributes, KCidTag()).AllocL());
		if(err != KErrNone)
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: Start Element <content> missing cid attribute"));	
			#endif

			User::Leave(err);
			}

		#ifdef _DEBUG
		LogL(_L("Start Element <content cid=\"%S\">"),*iCid);
		#endif

		}
	else if(KNameTag().CompareF(tag) == 0)
		{
		if(iName)
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: Second <name> tag found for content"));
			#endif

			// Should not find two name tags within a content tag
			User::Leave(KErrCorrupt);
			}

		#ifdef _DEBUG
		LogL(_L("Start element <name>"));
		#endif

		iWaitingForTag = ENameTag;
		// This will parse the name and call StringParseCompleteL() when done
		StartChildParserL(CStringParser::NewL(*this), aElement, aAttributes);
		}	
	else if(KUniqueIdTag().CompareF(tag) == 0)
		{
		if(iUniqueId)
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: Second <uniqueid> tag found for content"));
			#endif

			// Should not find two UniqueId tags within a content tag
			User::Leave(KErrCorrupt);
			}

		#ifdef _DEBUG
		LogL(_L("Start element <uniqueid>"));
		#endif
		
		iWaitingForTag = EUniqueIdTag;
		StartChildParserL(CStringParser::NewL(*this), aElement, aAttributes);
		}	
	else if(KFileNameTag().CompareF(tag) == 0)
		{
		if(iFileName)
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: Second <filename> tag found for content"));
			#endif

			// Should not find two filename tags within a content tag
			User::Leave(KErrCorrupt);
			}

		#ifdef _DEBUG
		LogL(_L("Start element <filename>"));
		#endif

		iWaitingForTag = EFileNameTag;
		StartChildParserL(CStringParser::NewL(*this), aElement, aAttributes);
		}
	else if(KAttributeTag().CompareF(tag) == 0)
		{
		// Set an attribute
		StartChildParserL(CAttributeParser::NewL(*this), aElement, aAttributes);
		}
	else if(KStringAttributeTag().CompareF(tag) == 0)
		{
		// Set a string attribute
		StartChildParserL(CStringAttributeParser::NewL(*this), aElement, aAttributes);		
		}
	else
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: Unexpected tag <%S> found"),tag);
		#endif

		// unexpected start tag
		User::Leave(KErrCorrupt);	
		}
	}
	
void CContentParser::ParseEndElementL(const RTagInfo& aElement)
	{
	TInt err = KErrNone;
	TBuf <KMaxDataTypeLength> mimeType;
	if(KContentTag().CompareF(aElement.LocalName().DesC()) == 0)
		{
		// Validate and add to iCurrentContainer
		// iCurrentContainer has ownership of the content
		if(iCid && iName && iUniqueId && iFileName)
			{
			#ifdef _DEBUG
			LogL(_L("  End Element </content>"));
			#endif

			err = iStringAttributeSet.GetValue(EMimeType, mimeType);
			if(err != KErrNone || mimeType.Length() == 0)
				{
				#ifdef _DEBUG
				LogL(_L("ERROR: A string attribute \"mimetype\" not specified for content with cid: %S"), *iCid);
				#endif

				User::Leave(KErrCorrupt);
				}
			else
				{
				iCurrentContainer.AddContentL(*iCid, *iName, *iUniqueId, *iFileName, iAttributeSet, iStringAttributeSet);
				}
			}
		else
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: </content> tag not expected yet, name, uniqueid, filename and the mimetype stringattribute must all be specified first"));
			#endif

			User::Leave(KErrCorrupt);	
			}
		}
	else
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: Unexpected end element </%S>, expected </content>"),aElement.LocalName().DesC());
		#endif

		// We were expecting the <\content> tag
		User::Leave(KErrCorrupt);	
		}
	}

void CContentParser::StringParseCompleteL(const TDesC& aString)
	{
	switch(iWaitingForTag)
		{
	case ENameTag:
		iName = aString.AllocL();
		if(iName->Length() > KMaxCafContentName)
		{
			#ifdef _DEBUG
			LogL(_L("ERROR: content of <name> tag is too large"));
			#endif

			// enforce from boundary check
			User::Leave(KErrCAOutOfRange);
		}
		break;
	case EUniqueIdTag:
		iUniqueId = aString.AllocL();
		if(iUniqueId->Length() > KMaxCafUniqueId)
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: content of <uniqueid> tag is too large"));
			#endif

			// enforce from boundary check
			User::Leave(KErrCAOutOfRange);
			}
		break;
	case EFileNameTag:
		iFileName = aString.AllocL();
		break;
	default:
		User::Panic(KRtaPanicString(), ERtaPanicParserInvalidState);
		break;
		}
	}


void CContentParser::SetAttributeL(TInt aAttribute, TInt aValue)
	{
	// Called by CAttributeParser
	// This will leave if the same attribute tag appears twice
	iAttributeSet.AddL(aAttribute); 
	User::LeaveIfError(iAttributeSet.SetValue(aAttribute, aValue, KErrNone));
	}
	
void CContentParser::SetStringAttributeL(TInt aStringAttribute, const TDesC& aValue)
	{
	// Called by CStringAttributeParser
	// This will leave if the same attribute tag appears twice
	iStringAttributeSet.AddL(aStringAttribute); 
	User::LeaveIfError(iStringAttributeSet.SetValue(aStringAttribute, aValue, KErrNone));
	}
