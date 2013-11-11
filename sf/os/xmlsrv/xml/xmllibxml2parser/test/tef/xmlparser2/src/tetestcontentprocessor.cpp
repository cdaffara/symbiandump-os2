// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file tetestcontentprocessor.cpp
 @internalTechnology
*/

#include "tetestcontentprocessor.h"

using namespace Xml;
/**
 * Empty routine
 */
void CTestContentProcessor::Release()
	{}
/**
 * Empty routine
 */
void CTestContentProcessor::SetContentSink(MContentHandler& /*aContentHandler*/)
	{}
/**
 * Empty routine
 */
void CTestContentProcessor::OnStartDocumentL(
		const RDocumentParameters& /*aDocParam*/, TInt /*aErrorCode*/)
	{}
/**
 * Empty routine
 */
void CTestContentProcessor::OnEndDocumentL(TInt /*aErrorCode*/)
	{}
/**
 * Empty routine
 */
void CTestContentProcessor::OnStartElementL(const RTagInfo& /*aElement*/,
		const RAttributeArray& /*aAttributes*/, TInt /*aErrorCode*/)
	{}
/**
 * Empty routine
 */
void CTestContentProcessor::OnEndElementL(const RTagInfo& /*aElement*/,
		TInt /*aErrorCode*/)
	{}
/**
 * Empty routine
 */
void CTestContentProcessor::OnContentL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/)
	{}
/**
 * Empty routine
 */
void CTestContentProcessor::OnStartPrefixMappingL(const RString& /*aPrefix*/,
		const RString& /*aUri*/, TInt /*aErrorCode*/)
	{}
/**
 * Empty routine
 */
void CTestContentProcessor::OnEndPrefixMappingL(const RString& /*aPrefix*/,
		TInt /*aErrorCode*/)
	{}
/**
 * Empty routine
 */
void CTestContentProcessor::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/,
		TInt /*aErrorCode*/)
	{}
/**
 * Empty routine
 */
void CTestContentProcessor::OnSkippedEntityL(const RString& /*aName*/,
		TInt /*aErrorCode*/)
	{}
/**
 * Empty routine
 */
void CTestContentProcessor::OnProcessingInstructionL(const TDesC8& /*aTarget*/,
		const TDesC8& /*aData*/, TInt /*aErrorCode*/)
	{}
/**
 * Empty routine
 */
void CTestContentProcessor::OnError(TInt /*aErrorCode*/)
	{}
/**
 * Empty routine
 */
TAny* CTestContentProcessor::GetExtendedInterface(const TInt32 /*aUid*/)
	{
	return NULL;
	}
/**
 * Static constructor
 */
CTestContentProcessor* CTestContentProcessor::NewL()
	{
	CTestContentProcessor* self = new (ELeave) CTestContentProcessor();
	return self;
	}
