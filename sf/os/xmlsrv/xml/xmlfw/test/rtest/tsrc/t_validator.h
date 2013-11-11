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
//

#ifndef __T_VALIDATOR_H__
#define __T_VALIDATOR_H__

#include <xml/contentprocessor.h>
#include <xml/wbxmlextensionhandler.h>
#include <xml/plugins/contentprocessorinitparams.h>


class CValidator : public CBase, public MWbxmlExtensionHandler, public MContentProcessor
	{
public:
	static MContentProcessor* NewL(TAny* aInitParams);
	virtual ~CValidator();


	// From MContentHandler

	void OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode);
	void OnEndDocumentL(TInt aErrorCode);
	void OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttribute, TInt aErrorCode);
	void OnEndElementL(const RTagInfo& aElement, TInt aErrorCode);
	void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
	void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
	void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
	void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
	void OnSkippedEntityL(const RString& aName, TInt aErrorCode) ;
	void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
	TAny* GetExtendedInterface(const TInt32 aUid);

	// From MContentSource

	void SetContentSink (MContentHandler& aContentHandler);

	
	void OnError(TInt aErrorCode);


	// From MWbxmlExtensionHandler
	void OnExtensionL(const RString& aData, TInt aToken, TInt aErrorCode);

	// From MContentProcessor
	void Release();

private:

	CValidator(TContentProcessorInitParams* aInitParams);

private:

/**
The child to callback.
We do not own this.
*/
	MContentHandler*				iContentHandler;

/**
Useful stringpool collection of RStrings.
We do not own this.
*/
	RStringDictionaryCollection*	iStringDictionaryCollection;

/**
A stack that helps correllate the start and end of elements.
We do not own this.
*/
	RElementStack* 					iElementStack;

	};


#endif //__T_VALIDATOR_H__
