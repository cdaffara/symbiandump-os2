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


#include "parsehandlerbase.h"

using namespace ReferenceTestAgent;
using namespace Xml;

CParseHandlerBase::~CParseHandlerBase()
	{
	delete iChildParser;
	}
	
void CParseHandlerBase::StartChildParserL(CParseHandlerBase* aChildParser, const RTagInfo& aElement, const RAttributeArray& aAttributes)
	{
	if(iChildParser)
		{
		User::Leave(KErrCorrupt);
		}
	else
		{
		iChildParser = aChildParser;
		iChildDepth++;	
		iChildParser->ParseStartElementL(aElement, aAttributes);		
		}
	}
	
void CParseHandlerBase::ParseStartElementL(const RTagInfo& /*aElement*/, const RAttributeArray& /*aAttributes*/)
	{
	// not expecting a start tag
	User::Leave(KErrCorrupt);
	}
	
void CParseHandlerBase::ParseEndElementL(const RTagInfo& /*aElement*/)
	{
	// not expecting an end tag
	User::Leave(KErrCorrupt);
	}
	
void CParseHandlerBase::ParseContentL(const TDesC8& /*aBytes*/)
	{
	// ignore content, probably just tabs and white space	
	}


void CParseHandlerBase::OnStartDocumentL(const RDocumentParameters& /*aDocParam*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	
	// Not expecting the start of the document
	User::Leave(KErrCorrupt);
	}

void CParseHandlerBase::OnEndDocumentL(TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	
	// Not expecting the end of the document
	User::Leave(KErrCorrupt);
	}

void CParseHandlerBase::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
	{
	if(iChildParser)
		{
		iChildParser->OnContentL(aBytes, aErrorCode);
		}
	else
		{
		User::LeaveIfError(aErrorCode);
		ParseContentL(aBytes);
		}
	}
	
	
void CParseHandlerBase::OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes, TInt aErrorCode)
	{
	if(iChildParser)
		{
		// If the child handler is present get it to parse the tags
		iChildParser->OnStartElementL(aElement, aAttributes, aErrorCode);
		iChildDepth++;
		}
	else
		{
		User::LeaveIfError(aErrorCode);
		ParseStartElementL(aElement, aAttributes);	
		}
	}
	
void CParseHandlerBase::OnEndElementL(const RTagInfo& aElement, TInt aErrorCode)
	{
	if(iChildParser)
		{
		iChildParser->OnEndElementL(aElement, aErrorCode);
		if(--iChildDepth == 0)
			{
			// finished with child parser handler
			delete iChildParser;
			iChildParser = NULL;
			}
		}	
	else
		{
		// Overriden by derived classes to implement specific behavior for an end tag
		User::LeaveIfError(aErrorCode);
		ParseEndElementL(aElement);
		}
	}

void CParseHandlerBase::OnStartPrefixMappingL(const RString& /*aPrefix*/, const RString& /*aUri*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	
	// Not expecting prefix mapping
	User::Leave(KErrCorrupt);
	}

void CParseHandlerBase::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	
	// Not expecting prefix mapping
	User::Leave(KErrCorrupt);
	}

void CParseHandlerBase::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	// Don't care about ignorable white space
	}

void CParseHandlerBase::OnSkippedEntityL(const RString& /*aName*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	
	// Not expecting skipped entries
	User::Leave(KErrCorrupt);

	}

void CParseHandlerBase::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	
	// Not expecting processing instructions
	User::Leave(KErrCorrupt);
	}

void CParseHandlerBase::OnError(TInt /*aErrorCode*/)
	{
	// do nothing?
	}

TAny* CParseHandlerBase::GetExtendedInterface(const TInt32 /*aUid*/)
	{
	// we don't support any extended interfaces
	return NULL;
	}

const TDesC8& CParseHandlerBase::GetTagAttributeL(const RAttributeArray& aAttributes, const TDesC8& aTag)
	{
	TInt i = 0;
	
	// Find the value of the attribute matching the given Tag
	for(i = 0; i < aAttributes.Count(); i++)
		{
		if(aTag.CompareF(aAttributes[i].Attribute().LocalName().DesC()) == 0)
			{
			return aAttributes[i].Value().DesC();
			}
		}
	User::Leave(KErrNotFound);
	return KNullDesC8();
	}
	
HBufC* CParseHandlerBase::ConvertDes8ToDes16L(const TDesC8& aString)
	{
	// This may need to be implemented in a memcpy() fashion if the XML
	// file itself is unicode
	HBufC* buffer = HBufC::NewL(aString.Length());
	buffer->Des().Copy(aString);
	return buffer;
	}

#ifdef _DEBUG

void CParseHandlerBase::LogL(const TDesC& aMessage)
	{
	RFileLogger::WriteFormat(KRtaLogDir(), KRtaLogName(), EFileLoggingModeAppend, aMessage);
	}
	
void CParseHandlerBase::LogL(const TDesC& aMessage, const TDesC8& aParam)
	{
	HBufC *param16 = HBufC::NewLC(aParam.Length());
	param16->Des().Copy(aParam);
	RFileLogger::WriteFormat(KRtaLogDir(), KRtaLogName(), EFileLoggingModeAppend, aMessage, param16);
	CleanupStack::PopAndDestroy(param16);
	}

void CParseHandlerBase::LogL(const TDesC& aMessage, TInt aParam)
	{
	RFileLogger::WriteFormat(KRtaLogDir(), KRtaLogName(), EFileLoggingModeAppend, aMessage, aParam);
	}

#endif
