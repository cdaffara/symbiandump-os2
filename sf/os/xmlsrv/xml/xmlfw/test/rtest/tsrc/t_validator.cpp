// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Written to test compilation
// 
//

#include <e32std.h>
#include <e32svr.h> // RDebug
#include <ecom/implementationproxy.h>

#include <xml/xmlframeworkerrors.h>

using namespace Xml;

#include "t_validator.h"
#include "t_testconstants.h"


_LIT(KTestMsgPrefix, "CValidator::");


MContentProcessor* CValidator::NewL(TAny* aInitParams)
	{
	CValidator* self = new(ELeave) CValidator(reinterpret_cast<TContentProcessorInitParams*>(aInitParams));
	return (static_cast<MContentProcessor*>(self));
	}


CValidator::CValidator(TContentProcessorInitParams* aInitParams)
:	iContentHandler (reinterpret_cast<MContentHandler*>(aInitParams->iContentHandler)),
	iStringDictionaryCollection (reinterpret_cast<RStringDictionaryCollection*>(aInitParams->iStringDictionaryCollection))
	{
	}


CValidator::~CValidator()
	{
	// We don't own this
	iContentHandler = NULL;
	iStringDictionaryCollection = NULL;
	iElementStack = NULL;
	}



void CValidator::OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode)
	{
	TBuf<50> wholeMsg(KTestMsgPrefix);
	_LIT(KFuncName, "OnStartDocumentL\n");
	wholeMsg.Append(KFuncName);
	RDebug::Print(wholeMsg);

	__ASSERT_ALWAYS(aErrorCode == KErrorCodeOnStartDocument, User::Invariant());
	iContentHandler->OnStartDocumentL(aDocParam, aErrorCode);
	}


void CValidator::OnEndDocumentL(TInt aErrorCode)
	{
	TBuf<50> wholeMsg(KTestMsgPrefix);
	_LIT(KFuncName, "OnEndDocumentL\n");
	wholeMsg.Append(KFuncName);
	RDebug::Print(wholeMsg);

	__ASSERT_ALWAYS(aErrorCode == KErrorCodeOnEndDocument, User::Invariant());
	iContentHandler->OnEndDocumentL(aErrorCode);
	}

void CValidator::OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttribute, TInt aErrorCode)
	{
	TBuf<50> wholeMsg(KTestMsgPrefix);
	_LIT(KFuncName, "OnStartElementL\n");
	wholeMsg.Append(KFuncName);
	RDebug::Print(wholeMsg);

	__ASSERT_ALWAYS(aErrorCode == KErrorCodeOnStartElement, User::Invariant());
	iContentHandler->OnStartElementL(aElement, aAttribute, aErrorCode);
	}

void CValidator::OnEndElementL(const RTagInfo& aElement, TInt aErrorCode)
	{
	TBuf<50> wholeMsg(KTestMsgPrefix);
	_LIT(KFuncName, "OnEndElementL\n");
	wholeMsg.Append(KFuncName);
	RDebug::Print(wholeMsg);

	__ASSERT_ALWAYS(aErrorCode == KErrorCodeOnEndElement, User::Invariant());
	iContentHandler->OnEndElementL(aElement, aErrorCode);
	}

void CValidator::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
	{
	TBuf<50> wholeMsg(KTestMsgPrefix);
	_LIT(KFuncName, "OnContentL\n");
	wholeMsg.Append(KFuncName);
	RDebug::Print(wholeMsg);

	__ASSERT_ALWAYS(aErrorCode == KErrorCodeOnContent, User::Invariant());
	iContentHandler->OnContentL(aBytes, aErrorCode);
	}

void CValidator::OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode)
	{
	TBuf<50> wholeMsg(KTestMsgPrefix);
	_LIT(KFuncName, "OnStartPrefixMappingL\n");
	wholeMsg.Append(KFuncName);
	RDebug::Print(wholeMsg);

	__ASSERT_ALWAYS(aErrorCode == KErrorCodeOnStartPrefixMapping, User::Invariant());
	iContentHandler->OnStartPrefixMappingL(aPrefix, aUri, aErrorCode);
	}

void CValidator::OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode)
	{
	TBuf<50> wholeMsg(KTestMsgPrefix);
	_LIT(KFuncName, "OnEndPrefixMappingL\n");
	wholeMsg.Append(KFuncName);
	RDebug::Print(wholeMsg);

	__ASSERT_ALWAYS(aErrorCode == KErrorCodeOnEndPrefixMapping, User::Invariant());
	iContentHandler->OnEndPrefixMappingL(aPrefix, aErrorCode);
	}

void CValidator::OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode)
	{
	TBuf<50> wholeMsg(KTestMsgPrefix);
	_LIT(KFuncName, "OnIgnorableWhiteSpaceL\n");
	wholeMsg.Append(KFuncName);
	RDebug::Print(wholeMsg);

	__ASSERT_ALWAYS(aErrorCode == KErrorCodeOnIgnorableWhiteSpace, User::Invariant());
	iContentHandler->OnIgnorableWhiteSpaceL(aBytes, aErrorCode);
	}

void CValidator::OnSkippedEntityL(const RString& aName, TInt aErrorCode)
	{
	TBuf<50> wholeMsg(KTestMsgPrefix);
	_LIT(KFuncName, "OnSkippedEntityL\n");
	wholeMsg.Append(KFuncName);
	RDebug::Print(wholeMsg);

	__ASSERT_ALWAYS(aErrorCode == KErrorCodeOnSkippedEntity, User::Invariant());
	iContentHandler->OnSkippedEntityL(aName, aErrorCode);
	}

void CValidator::OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode)
	{
	TBuf<50> wholeMsg(KTestMsgPrefix);
	_LIT(KFuncName, "OnProcessingInstructionL\n");
	wholeMsg.Append(KFuncName);
	RDebug::Print(wholeMsg);

	__ASSERT_ALWAYS(aErrorCode == KErrorCodeOnProcessingInstruction, User::Invariant());
	iContentHandler->OnProcessingInstructionL(aTarget, aData, aErrorCode);
	}

void CValidator::OnError(TInt aError)
	{
	TBuf<50> wholeMsg(KTestMsgPrefix);
	_LIT(KFuncName, "OnError\n");
	wholeMsg.Append(KFuncName);
	RDebug::Print(wholeMsg);

	iContentHandler->OnError(aError);
	}

TAny* CValidator::GetExtendedInterface(const TInt32 aUid)
/**
This method obtains the interface matching the specified uid.
@return				0 if no interface matching the uid is found.
					Otherwise, the this pointer cast to that interface.
@param				aUid the uid identifying the required interface.
*/
	{
	if (aUid == MWbxmlExtensionHandler::EExtInterfaceUid)
		{
		return static_cast<MWbxmlExtensionHandler*>(this);
		}
	return 0;
	}


// From MWbxmlExtensionHandler

void CValidator::OnExtensionL(const RString& aData, TInt aToken, TInt aErrorCode)
	{
	TBuf<50> wholeMsg(KTestMsgPrefix);
	_LIT(KFuncName, "OnExtensionL\n");
	wholeMsg.Append(KFuncName);
	RDebug::Print(wholeMsg);

	__ASSERT_ALWAYS(aErrorCode == KErrorCodeOnExtension, User::Invariant());
	
	MWbxmlExtensionHandler* ptr = 
		static_cast<MWbxmlExtensionHandler*>
			(iContentHandler->GetExtendedInterface(MWbxmlExtensionHandler::EExtInterfaceUid));
	
	if (!ptr)
		{
		User::Leave(KErrXmlUnsupportedExtInterface);
		}
		
	ptr->OnExtensionL(aData, aToken, aErrorCode);
	}


// From MContentSource

void CValidator::SetContentSink (MContentHandler& aContentHandler)
/**
This method allows for the correct streaming of data to another plugin in the chain.

@post				the next plugin in the chain is set to receive our callbacks.

*/
	{
	iContentHandler = &aContentHandler;
	}



// From MContentProcessor

void CValidator::Release()
	{
	delete (this);
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = {
	IMPLEMENTATION_PROXY_ENTRY(0x101FAA11,CValidator::NewL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

