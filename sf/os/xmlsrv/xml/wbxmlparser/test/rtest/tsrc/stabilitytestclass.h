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

#ifndef __STABILITYTESTCLASS_H__
#define __STABILITYTESTCLASS_H__

#include <f32file.h>

#include <xml/contenthandler.h>
#include <xml/parser.h>
#include <xml/stringdictionarycollection.h>
#include <xml/wbxmlextensionhandler.h>

class RTest;

struct TPassOrFailureSettings {
	TInt			iParseMode;
	const TDesC8*	iStringDictionaryUri;
	TBool			iDoParseDocument;
	TBool			iFilenameProvided;
	TInt			iExpectedCode;
	
};


#define BUFFER_SIZE  2000             // buffer to build strings. per element
#define XBUFFER_SIZE BUFFER_SIZE+1000 // additional size increase
#define OUTPUT_SIZE  16000            // concatenate BUFFER strings


class CStabilityTestClass : public CBase, public Xml::MWbxmlExtensionHandler, public Xml::MContentHandler
/**
This class provides common usage between stability tests.
Keeping code to a minimum and reuse high.

*/
	{
public:

	static CStabilityTestClass* NewL(RTest& aTest, 
									 TBool aIsOomTest = EFalse, 
									 TInt aChunkSize = 0);

	virtual ~CStabilityTestClass();

	typedef void (CStabilityTestClass::*ClassFuncPtrL) (const TDesC&);
	typedef void (CStabilityTestClass::*ClassFuncPtr2L) (CStabilityTestClass::ClassFuncPtrL);

	void StabilityTestL(const TDesC& aAbsoluteDirPath, 
						const TDesC& aExt, 
						ClassFuncPtrL aTestFuncL);

	void ParseDirL(const TDesC& aAbsoluteDirPath, const TDesC& aExt, 
				   ClassFuncPtrL aTestFuncL);

	void ParseEntryL(const TDesC& aAbsoluteFilename,
					 ClassFuncPtrL aTestFuncL);
									  
	void OomProcess(ClassFuncPtrL aTestFuncL);
	
	void TestWholeL(const TDesC& aFileName);
	void TestChunkL(const TDesC& aFileName);

	void TestBehaviour(const TDesC& aSrc, TPassOrFailureSettings& aTestSettings);
	void BehaviourTestL(const TDesC& aSrc, TPassOrFailureSettings& aTestSettings);

	// From MContentHandler

	void OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode);
	void OnEndDocumentL(TInt aErrorCode);
	void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
	void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode);
	void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
	void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
	void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
	void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
	void OnSkippedEntityL(const RString& aName, TInt aErrorCode);
	void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
	TAny* GetExtendedInterface(const TInt32 aUid);
	
	void OnError(TInt aErrorCode);


	// From MWbxmlExtensionHandler

	void OnExtensionL(const RString& aData, TInt aToken, TInt aErrorCode);


public:

#ifdef __COMPARE_OUTPUT__
	TBuf16<OUTPUT_SIZE> *iOutput;
#endif // __COMPARE_OUTPUT__

protected:

	CStabilityTestClass(RTest& aTest, TBool aIsOomTest = EFalse, TInt aChunkSize = 0);

	void ConstructL();

protected:

	Xml::RStringDictionaryCollection iStringDictionaryCollection;
	Xml::CParser* iParser;
	TInt						iError;
	TInt						iParseMode;
	TParse						iFileName;
	RFs							iFs;
	RTest&						iTest;
	TInt						iChunkSize;
	TBool						iIsOomTest;
	RArray<TInt>				iCurrentIndex;
	RPointerArray<CDir>			iEntries;

	TBuf16<BUFFER_SIZE>	*		iBuffer;

#ifdef __SHOW_MANUAL_OUTPUT__
	TBuf16<XBUFFER_SIZE> *		iFormat;
#endif // __SHOW_MANUAL_OUTPUT__
	};

#endif // __STABILITYTESTCLASS_H__
