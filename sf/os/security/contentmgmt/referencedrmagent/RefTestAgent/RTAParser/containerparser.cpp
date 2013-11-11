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


#include "containerparser.h"
#include "contentparser.h"
#include "drmfilecontainer.h"
#include "tags.h"
#include "rtapanic.h"
#include <caf/caftypes.h>
#include <caf/caferr.h>

using namespace ReferenceTestAgent;
using namespace Xml;

// Operates on a container to parse subcontainers
CContainerParser*  CContainerParser::NewL(CDrmFileContainer& aCurrentContainer)
	{
	CContainerParser* self = new (ELeave) CContainerParser(aCurrentContainer);
	return self;
	}

CContainerParser::CContainerParser(CDrmFileContainer& aCurrentContainer) : 
iFoundOwnContainerTag(EFalse), iCurrentContainer(aCurrentContainer)
	{
	}	

CContainerParser::~CContainerParser()
	{	
	delete iName;
	delete iUniqueId;
	}


void CContainerParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes)
		{
		TPtrC8 tag(aElement.LocalName().DesC());
		if(KContainerTag().CompareF(tag) == 0)
			{
			// <container>
			if(!iFoundOwnContainerTag)
				{
				// This is the first time we have been called. This tag relates to our
				// current container, it is not embedded within the current container
				iFoundOwnContainerTag = ETrue;

				#ifdef _DEBUG
				LogL(_L("Start Element <container>"));
				#endif
				}
			else
				{
				// We have already processed our own <container> tag so this must just
				// be some other container embedded within our current container

				// add a new container to the current container
				// iCurrentContainer has ownership of the new container
				CDrmFileContainer& newContainer = iCurrentContainer.AddContainerL();
				
				// create a parser for the new container
				StartChildParserL(CContainerParser::NewL(newContainer), aElement, aAttributes);
				}
			}
		else if(KNameTag().CompareF(tag) == 0)
			{
			if(iName)
				{
				#ifdef _DEBUG
				LogL(_L("ERROR: Second <name> element found within <container>"));
				#endif

				// we already found our name tag
				User::Leave(KErrCorrupt);
				}

			#ifdef _DEBUG
			LogL(_L("Start element <name>"));
			#endif

			// <name>
			iWaitingForTag = ENameTag;
			// This will parse the name and call StringParseCompleteL() when done
			StartChildParserL(CStringParser::NewL(*this), aElement, aAttributes);
			}	
		else if(KUniqueIdTag().CompareF(tag) == 0)
			{
			if(iUniqueId)
				{
				#ifdef _DEBUG
				LogL(_L("ERROR: Second <uniqueid> element found within <container>"));
				#endif

				// we already found our uniqueid tag
				User::Leave(KErrCorrupt);
				}

			#ifdef _DEBUG
			LogL(_L("Start element <uniqueid>"));
			#endif

			// <uniqueid>
			iWaitingForTag = EUniqueIdTag;

			StartChildParserL(CStringParser::NewL(*this), aElement, aAttributes);
			}	
		else if(KContentTag().CompareF(tag) == 0)
			{
			// <content cid = "...."> 
			// Create a parser for the content object
			StartChildParserL(CContentParser::NewL(iCurrentContainer), aElement, aAttributes);
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

void CContainerParser::ParseEndElementL(const RTagInfo& aElement)
		{
		if(KContainerTag().CompareF(aElement.LocalName().DesC()) == 0)
			{
			// Validate any container that is not the root container
			if(iCurrentContainer.ParentContainer())
				{
				// Containers must have a name and unique Id specified
				if(iName && iUniqueId)
					{
					iCurrentContainer.SetNameL(*iName);
					iCurrentContainer.SetUniqueIdL(*iUniqueId);

					#ifdef _DEBUG
					LogL(_L("  End element </container>"));	
					#endif
					}
				else
					{
					#ifdef _DEBUG
					LogL(_L("ERROR: </container> tag not expected yet, name and uniqueid must be specified first"));
					#endif

					User::Leave(KErrCorrupt);
					}
				}
			}
		else
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: Unexpected tag <%S>, expected </container> tag "), aElement.LocalName().DesC());	
			#endif

			// We expect <\container> to be the closing tag
			User::Leave(KErrCorrupt);	
			}
		}

void CContainerParser::StringParseCompleteL(const TDesC& aString)
	{
	switch(iWaitingForTag)
		{
	case ENameTag:
		iName = aString.AllocL();

		break;
	case EUniqueIdTag:
		iUniqueId = aString.AllocL();
		if(iUniqueId->Length() > ContentAccess::KMaxCafUniqueId)
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: content of <uniqueid> tag is too large"));
			#endif

			// enforce from boundary check
			User::Leave(KErrCAOutOfRange);
			}
		break;
	default:
		User::Panic(KRtaPanicString(), ERtaPanicParserInvalidState);
		break;
		};
	}

