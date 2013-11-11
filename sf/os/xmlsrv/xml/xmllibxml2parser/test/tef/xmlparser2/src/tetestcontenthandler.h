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
 @file tetestcontenthandler.h
 @internalTechnology
*/

#ifndef _TE_TESTCONTENTHANDLER_H_
#define _TE_TESTCONTENTHANDLER_H_

#include <xml/contenthandler.h>
#include <xml/parser.h>

/**
 * Dummy content handler for XML
 */
class CTestContentHandler: public CBase, public Xml::MContentHandler
    {
public:
    static CTestContentHandler* NewL();
    ~CTestContentHandler();

    // From MContentHandler
    void OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode);
    void OnEndDocumentL(TInt aErrorCode);
    void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
    void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode);
    void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
    void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
    void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
    void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
    void OnSkippedEntityL(const RString& aName, TInt aErrorCode) ;
    void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
    void OnError(TInt aErrorCode);
    TAny* GetExtendedInterface(const TInt32 aUid);
 
private:
	CTestContentHandler();
	void ConstructL();
    };
#endif /*TE_TESTCONTENTHANDLER_H_*/
