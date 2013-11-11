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


/**
 @file
 @internalComponent
 @released
*/

#ifndef __PARSEHANDLERBASE_H__
#define __PARSEHANDLERBASE_H__

#include <xml/contenthandler.h>
#include <e32base.h>

#include <flogger.h>
_LIT(KRtaLogDir, "rta");
_LIT(KRtaLogName, "rta.log");

namespace ReferenceTestAgent
	{
	/** Provides an MContentHandler inteface that has a virtual destructor and implements
	default behavior for parsers which can be overriden by derived parsers as required
	*/
	class CParseHandlerBase : public CBase, public Xml::MContentHandler
		{
	public:
		virtual ~CParseHandlerBase();
		
		// Overridden as required by derived classes
		virtual void ParseStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes);
		virtual void ParseEndElementL(const Xml::RTagInfo& aElement);
		virtual void ParseContentL(const TDesC8& aBytes);


		// From MContentHandler
		virtual void OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode);
		virtual void OnEndDocumentL(TInt aErrorCode);
		virtual void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
		virtual void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode);
		virtual void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
		virtual void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
		virtual void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
		virtual void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
		virtual void OnSkippedEntityL(const RString& aName, TInt aErrorCode);
		virtual void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
		virtual void OnError(TInt aErrorCode);
		virtual TAny* GetExtendedInterface(const TInt32 aUid);
		static const TDesC8& GetTagAttributeL(const Xml::RAttributeArray& aAttributes, const TDesC8& aTag);
		static HBufC* ConvertDes8ToDes16L(const TDesC8& aString);

#ifdef _DEBUG

		static void LogL(const TDesC& aMessage);
		static void LogL(const TDesC& aMessage, const TDesC8& aParam);
		static void LogL(const TDesC& aMessage, TInt aParam);

#endif

	protected:
		void StartChildParserL(CParseHandlerBase* aChildParser, const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes);
		
	private:
		CParseHandlerBase* iChildParser; // delete when depth = 0
		TInt iChildDepth;   // when this is zero we process the tags, otherwise child does.
		};
	}

#endif
