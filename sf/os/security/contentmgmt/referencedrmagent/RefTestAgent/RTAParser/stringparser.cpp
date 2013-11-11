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


#include "stringparser.h"

using namespace ReferenceTestAgent;
using namespace Xml;

CStringParser* CStringParser::NewL(MStringParser& aObserver)
	{
	CStringParser* self = new (ELeave) CStringParser(aObserver);
	return self;
	}
		
CStringParser::CStringParser(MStringParser& aObserver) : iTag(KNullDesC8()), iObserver(aObserver)
	{
	}

CStringParser::~CStringParser()
	{
	delete iBuffer;
	}

void CStringParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/)
	{
	if(iTag.Length() > 0)
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: Start Element <%S> tag not expected"), aElement.LocalName().DesC());
		#endif

		// We have already found the tag we are parsing, there shouldn't be any other tags found
		User::Leave(KErrCorrupt);
		}
	iTag.Set(aElement.LocalName().DesC());
	}

void CStringParser::ParseEndElementL(const RTagInfo& aElement)
	{
	// Check the end tag we just came across is the same as the tag we started with
	if(iTag.CompareF(aElement.LocalName().DesC()) != 0)
		{
		#ifdef _DEBUG
		LogL(_L("ERROR: End Element <%S> not expected"), aElement.LocalName().DesC());
		#endif

		// We should only expect the end tag of the tag we are processing
		User::Leave(KErrCorrupt);
		}
		
	if(!iBuffer)
		{
		User::Leave(KErrCorrupt);
		}
	
	// Cleanup the string we received, remove unnecessary tab, space and line feed characters
	// The parser should probably do this for us already
	TPtr8 buffer = iBuffer->Des();
	
	TInt i = 0;
	TInt start = 0;
	TInt end = 0;
	for(i = 0; i < buffer.Length(); i++)
		{
		TChar c(buffer[i]);
		if(!c.IsSpace())
			{
			start = i;
			break;
			}
		}
	
	for(i = buffer.Length(); i > 0; i--)
		{
		TChar c(buffer[i-1]);
		if(!c.IsSpace())
			{
			end = i;
			break;
			}
		}

	TPtrC8 cleanBuffer8 = buffer.Mid(start, end-start);
	HBufC* cleanBuffer = HBufC::NewLC(cleanBuffer8.Length());
	cleanBuffer->Des().Copy(cleanBuffer8);
	
	#ifdef _DEBUG
	LogL(_L("              %S"), cleanBuffer8);	
	LogL(_L("  End Element </%S>"), aElement.LocalName().DesC());
	#endif
	
	// Pass the parsed string back to the observer, finished
	iObserver.StringParseCompleteL(*cleanBuffer);
	CleanupStack::PopAndDestroy(cleanBuffer);
	}

void CStringParser::ParseContentL(const TDesC8& aBytes)
	{
	if(iBuffer)
		{
		// If aBuffer already exists append the new data to the end of the existing data
		HBufC8* newBuffer = HBufC8::NewL(iBuffer->Length() + aBytes.Length());
		TPtr8 newBufferPtr = newBuffer->Des();
		newBufferPtr.Copy(*iBuffer);
		newBufferPtr.Append(aBytes);
		delete iBuffer;
		iBuffer = newBuffer;
		}
	else
		{
		// If we have no data in the buffer, copy the supplied data
		iBuffer = aBytes.AllocL();
		}
	}

