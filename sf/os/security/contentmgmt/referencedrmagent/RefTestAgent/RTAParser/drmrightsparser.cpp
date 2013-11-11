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


#include "drmrightsparser.h"
#include "rightsparser.h"
#include "drmrights.h"
#include "virtualFile.h"
#include "tags.h"

using namespace ReferenceTestAgent;
using namespace Xml;

_LIT8(KXmlMimeType, "text/xml");

EXPORT_C CDrmRightsParser* CDrmRightsParser::NewL()
	{
	CDrmRightsParser* self = new (ELeave) CDrmRightsParser;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDrmRightsParser* CDrmRightsParser::NewL(CVirtualFile& aVirtualFile)
	{
	CDrmRightsParser* self = new (ELeave) CDrmRightsParser;
	CleanupStack::PushL(self);
	self->ConstructL(aVirtualFile);
	CleanupStack::Pop(self);
	return self;
	}

		
CDrmRightsParser::CDrmRightsParser() : iParseComplete(EFalse)
	{
	}

void CDrmRightsParser::ConstructL()	
	{	
	iDrmRights = CDrmRights::NewL();
	iXmlParser = CParser::NewL(KXmlMimeType(), *this);
	iXmlParser->ParseBeginL(KXmlMimeType());
	}

void CDrmRightsParser::ConstructL(CVirtualFile& aVirtualFile)
	{
	iDrmRights = CDrmRights::NewL();
	CParser *xmlParser = CParser::NewLC(KXmlMimeType(), *this);
	xmlParser->ParseBeginL();
	
	// Parse the xml file in chunks
	TBuf8 <1024> buffer;
	TInt length = buffer.MaxLength();
	while(length > 0)
		{
		User::LeaveIfError(aVirtualFile.Read(buffer,length));
		length = buffer.Length();
		xmlParser->ParseL(buffer);
		}

	xmlParser->ParseEndL();
	// Finished parsing
	CleanupStack::PopAndDestroy(xmlParser); 
	
	if(!iParseComplete)
		{
		#ifdef _DEBUG
		LogL(_L("Not a complete XML file"));
		#endif

		User::Leave(KErrCorrupt);
		}
	}


CDrmRightsParser::~CDrmRightsParser()
	{
	delete iDrmRights;
	delete iXmlParser;
	}

EXPORT_C void CDrmRightsParser::ParseL(const TDesC8& aFragment)
	{
	iXmlParser->ParseL(aFragment);
	}

EXPORT_C CDrmRights& CDrmRightsParser::DrmRightsL()
	{
	if(!iParseComplete && iXmlParser)
		{
		iXmlParser->ParseEndL();

		// If parsing is still not complete must be something wrong with the file	
		if(!iParseComplete)
			{
			#ifdef _DEBUG
			LogL(_L("Not a complete XML file"));	
			#endif

			User::Leave(KErrCorrupt);
			}
		}
	return *iDrmRights;
	}

void CDrmRightsParser::OnStartDocumentL(const RDocumentParameters& /*aDocParam*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	
	#ifdef _DEBUG
	LogL(_L("Start of rights XML document"));
	#endif
	}

void CDrmRightsParser::OnEndDocumentL(TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);

	#ifdef _DEBUG
	LogL(_L("End of rights XML document"));
	#endif
	}

void CDrmRightsParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes)
	{
	TPtrC8 tag(aElement.LocalName().DesC());
	if(KDrmRightsTag().CompareF(tag) == 0)
		{
		if(iFoundStartTag)
			{
			#ifdef _DEBUG
			LogL(_L("ERROR: start element <drmrights> found inside <drmrights> element"));
			#endif

			// We have already found the <DRMRights> tag, there shouldn't be two of them
			User::Leave(KErrCorrupt);
			}
		iFoundStartTag = ETrue;

		#ifdef _DEBUG
		LogL(_L("Start Element <drmrights>"));
		#endif
		}
	else if(KRightsTag().CompareF(tag) == 0)
		{
		// <rights>

		// create a parser for the rights object
		StartChildParserL(CRightsParser::NewL(*iDrmRights), aElement, aAttributes);
		}
	else
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: Unexpected start element <%S>"), tag);
		#endif

		// unexpected start tag
		User::Leave(KErrCorrupt);	
		}
	}
		

void CDrmRightsParser::ParseEndElementL(const RTagInfo& aElement)
	{
	// Check the end tag is </DRMRights>
	if(KDrmRightsTag().CompareF(aElement.LocalName().DesC()) != 0)
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: Unexpected end element </%S>, expected </drmrights>"), aElement.LocalName().DesC());
		#endif

		User::Leave(KErrCorrupt);
		}

	#ifdef _DEBUG
	LogL(_L("  End Element </drmrights>"), aElement.LocalName().DesC());
	#endif
	iParseComplete = ETrue;
	}
