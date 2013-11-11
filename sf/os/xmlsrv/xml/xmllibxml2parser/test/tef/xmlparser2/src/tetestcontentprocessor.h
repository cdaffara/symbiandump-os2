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
 @file tetestcontentprocessor.h
 @internalTechnology
*/

#ifndef _TE_TESTCONTENTPROCESSOR_H_
#define _TE_TESTCONTENTPROCESSOR_H_

#include <xml/contentprocessor.h>
#include <xml/contenthandler.h>
#include <xml/parser.h>

using namespace Xml;
/**
 * Dummy Content processor for XML
 */
class CTestContentProcessor: public Xml::MContentProcessor
	{
	public:
	void Release();

	void SetContentSink(MContentHandler& aContentHandler);

	void OnStartDocumentL(
			const RDocumentParameters& aDocParam, TInt aErrorCode);

	void OnEndDocumentL(TInt aErrorCode);

	void OnStartElementL(const RTagInfo& aElement,
			const RAttributeArray& aAttributes, TInt aErrorCode);

	void OnEndElementL(const RTagInfo& aElement,
			TInt aErrorCode);

	void OnContentL(const TDesC8& aBytes, TInt aErrorCode);

	void OnStartPrefixMappingL(const RString& aPrefix,
			const RString& aUri, TInt aErrorCode);

	void OnEndPrefixMappingL(const RString& aPrefix,
			TInt aErrorCode);

	void OnIgnorableWhiteSpaceL(const TDesC8& aBytes,
			TInt aErrorCode);

	void OnSkippedEntityL(const RString& aName,
			TInt aErrorCode);

	void OnProcessingInstructionL(const TDesC8& aTarget,
			const TDesC8& aData, TInt aErrorCode);

	void OnError(TInt aErrorCode);

	TAny* GetExtendedInterface(const TInt32 aUid);

	static CTestContentProcessor* NewL();
	};
#endif /*TE_TESTCONTENTPROCESSOR_H_*/
