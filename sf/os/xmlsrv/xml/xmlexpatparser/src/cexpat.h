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

#ifndef CEXPAT_H
#define CEXPAT_H

#include <xml/parserfeature.h>
#include <xml/plugins/parserinitparams.h>

#include "expat.h"

class CXmlParser;

/*
CExpat class.

1.
This class encapsulates the Expat xml parser.  It is the engine behind
CXmlParserPlugin.

2.
Handles construction and deletion of Expat xml parser.  Defines handlers
required for Expat, transforms handler data to required types and passes
this to the client by MContentHandler callbacks.

3.
MContentHandler functions can leave.  Rather than trapping every one of
these, which could prove costly, the functions are allowed to leave 'through'
Expat but a tight control is kept on Expat-allocated memory to ensure no
leaks occur.  This is handled via the sub-class TExpatAlloc.

If a MContentHandler function does leave, the Expat parser is entirely
deleted for safety and a call to Reset is required to begin using it again.
*/

class CExpat : public CBase
	{
public:
	static CExpat* NewL(Xml::MContentHandler& aContentHandler, RStringPool& aStringPool, Xml::CCharSetConverter& aCharSetConverter, 
		RElementStack& aElementStack, TInt aDebugFailCount);
	~CExpat();

	void ResetL();
	TInt EnableFeature(TInt aParserFeature);
	TInt DisableFeature(TInt aParserFeature);
	TBool IsFeatureEnabled(TInt aParserFeature) const;

	/*
	Allocate a buffer in which to write xml to be parsed by the ParseL and
	ParseLastL methods.  Memory allocated with this method is managed
	internally.  Thus it must not be freed and not put on the cleanup stack.

	This must be called before every call to ParseL or ParseLastL
	*/
	TDes8& GetBufferL(TInt aLength);

	/*
	Parsing methods:
	
	ParseLastL should be used if the buffer contains the last segment of
	the document.  ParseL should be called in any other case.

	It is perfectly valid to parse the entire document with ParseL and then
	call ParseLastL with a zero-length buffer.
	*/
	void ParseL(TBool done=EFalse);
	inline void ParseLastL();

	void SetContentSink(class Xml::MContentHandler &);

	/*
	Function used to access internal parser heap for testing
	*/
	RHeap* GetInternalHeap() const;

private:
	CExpat(Xml::MContentHandler& aContentHandler, RStringPool& aStringPool, Xml::CCharSetConverter& aCharSetConverter,
		RElementStack& aElementStack);
	void ConstructL(TInt aDebugFailCount);

	void CreateRStringL(const TDesC& aInput, RString& aString, TBool aLowerCase);
	void ScanNameL(const XML_Char* aName, RString& aUriString, RString& aLocalPartString, RString& aPrefixString);
	inline TInt StringLen(TUint16* p);

	void ClearElementStack();

	static void StartElementHandlerL(void* aUserData, const XML_Char* aName, const XML_Char** aAtts);
	static void EndElementHandlerL(void* aUserData, const XML_Char* aName);
	static void CharacterDataHandlerL(void* aUserData, const XML_Char* aString, int aLen);
	static void ProcessingInstructionHandlerL(void* aUserData, const XML_Char* aTarget, const XML_Char* aData);
	static void XmlDeclHandlerL(void* aUserData, const XML_Char* aVersion, const XML_Char* aEncoding, int aStandalone);
	static void StartNamespaceDeclHandlerL(void* aUserData, const XML_Char* aPrefix, const XML_Char* aUri);
	static void EndNamespaceDeclHandlerL(void* aUserData, const XML_Char* aPrefix);
	static void SkippedEntityHandlerL(void* aUserData, const XML_Char* aName, int aIsParamEntity);
	static int ExternalEntityRefHandlerL(void* aUserData, const XML_Char* aName);

	void HandleStartElementL(const XML_Char* aName, const XML_Char** aAtts);
	void HandleEndElementL(const XML_Char* aName);
	void HandleCharacterDataL(const XML_Char* aString, int aLen);
	void HandleProcessingInstructionL(const XML_Char* aTarget, const XML_Char* aData);
	void HandleXmlDeclL(const XML_Char* aVersion, const XML_Char* aEncoding, int aStandalone);
	void HandleStartNamespaceDeclL(const XML_Char* aPrefix, const XML_Char* aUri);
	void HandleEndNamespaceDeclL(const XML_Char* aPrefix);
	void HandleSkippedEntityL(const XML_Char* aName, int aIsParamEntity);

private:
	/*
	Objects supplied on construction
	*/
	Xml::MContentHandler* iContentHandler;
	RStringPool& iStringPool;
	Xml::CCharSetConverter& iCharSetConverter;
	RElementStack& iElementStack;

	TInt iParseMode;

	/*
	Used to ensure GetBufferL has been called before a call to ParseL or ParseLastL
	*/
	TBool iBufferReady;
	TPtr8 iInputBuffer;

	/*
	Used to record the last status and error raised whilst parsing.
	*/
	XML_Status iPrevStatus;
	TInt iPrevError;
	
	/*
	The expat parser itself
	*/
	XML_Parser iParser;

	/*
	TExpatAlloc class managing fenced-off memory area separate from the thread's heap.
	This memory is contained in its own chunk created by UserHeap.
	*/
	class TExpatAlloc
		{
	public:
		TExpatAlloc() {}
		void ConstructL(TInt aHeapSize, TInt aDebugFailCount);
		~TExpatAlloc();

		/*
		Called when a leave from within Expat leaves the memory in an unknown state.
		All memory will be freed.
		*/
		void ResetL();

		/*
		Fill supplied XML_Memory_Handling_Suite structure which may then be passed
		to Expat's XML_ParserCreate_MM
		*/
		void FillAllocStruct(XML_Memory_Handling_Suite& aMem) const;

		static void* Alloc(void* aUserData, size_t aSize);
		static void* ReAlloc(void* aUserData, void* aPtr, size_t aSize);
		static void Free(void* aUserData, void *aPtr);

		/*
		For testing
		*/
		RHeap* GetHeap() const;

	private:
		TInt iHeapSize;
		RHeap* iHeap;
		} iAllocator;
	};

inline void CExpat::ParseLastL()
	{
	ParseL(ETrue);
	}

/*
strlen for null-terminated 16-bit strings
*/
inline TInt CExpat::StringLen(TUint16* aStart)
	{
	TUint16* p = aStart;
	while (*p++)
		;
	return p-1-aStart;
	}

#endif // CEXPAT_H
