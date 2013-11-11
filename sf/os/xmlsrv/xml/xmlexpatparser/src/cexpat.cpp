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

#include <charconv.h>

#include <xml/contenthandler.h>
#include <xml/documentparameters.h>
#include <xml/taginfo.h>
#include <xml/xmlparsererrors.h>
#include <xml/plugins/charsetconverter.h>

#include "cexpat.h"

using namespace Xml;

const XML_Char KNamespaceSeparator = '!';
const TInt KExpatHeapSize = 64*1024-256;

LOCAL_C void AttributeArrayDelete(TAny *aPtr);

/*
aDebugFailCount is for testing only, to configure the parser heap to fail during CExpat construction.

After construction CExpat::GetInternalHeap() can be used in conjunction with __RHEAP_FAILNEXT
for out-of-memory testing.
*/
CExpat* CExpat::NewL(MContentHandler& aContentHandler, RStringPool& aStringPool, CCharSetConverter& aCharSetConverter,
					 RElementStack& aElementStack, TInt aDebugFailCount)
	{
	CExpat* self = new(ELeave) CExpat(aContentHandler, aStringPool, aCharSetConverter, aElementStack);

	CleanupStack::PushL(self);
	self->ConstructL(aDebugFailCount);
	CleanupStack::Pop(self);

	return self;
	}

CExpat::CExpat(MContentHandler& aContentHandler, RStringPool& aStringPool, CCharSetConverter& aCharSetConverter, 
			   RElementStack& aElementStack) :
	iContentHandler(&aContentHandler), iStringPool(aStringPool), iCharSetConverter(aCharSetConverter), 
					iElementStack(aElementStack), iInputBuffer(0,0,0),
	iPrevStatus(XML_STATUS_OK),
	iPrevError(KErrNone)
	{
	}

void CExpat::ConstructL(TInt aDebugFailCount)
	{
	iAllocator.ConstructL(KExpatHeapSize, aDebugFailCount);
	ResetL();
	}

CExpat::~CExpat()
	{
	// iParser must be freed here.  The destructor of iAllocator (~TExpat) is called automatically
	// when CExpat is destructed, as it is a member variable of CExpat.  *But* ~TExpat() will only close its heap
	// which does not free any memory allocated on it.  So - we have to free the memory here, before
	// deleting the allocator.  This memory is guaranteed to be freed here, before the heap is closed
	// as the body of the destructor completes before the member destructors (~TExpat) are called.
	if ( iParser )
		{
		XML_ParserFree( iParser );		
		}
	}

/*
ResetL resets an the parser or allocates one if none exists.  This could be the case for one of two reasons:

  1. This is called as part of the construction of CExpat
  2. A MContentHandler function left.  In this case the parser and all its memory will have been deleted.

*/
void CExpat::ResetL()
	{
	iParseMode = EReportNamespaces | EReportNamespacePrefixes;
	iBufferReady = EFalse;
	
	iPrevStatus = XML_STATUS_OK;
	iPrevError = KErrNone;

	if(iParser)
		XML_ParserReset(iParser, NULL);
	else
		{
		XML_Memory_Handling_Suite m;
		iAllocator.FillAllocStruct(m);

		// Expat element/attribute names are passed as a single string containing the prefix, uri and local part.
		// This character is used to separate those three parts.
		XML_Char sep[] = { KNamespaceSeparator, 0 };

		iParser = XML_ParserCreate_MM(0, &m, sep);
		User::LeaveIfNull(iParser);
		}

	// UserData is the first argument passed to us by all Expat callbacks
	XML_SetUserData(iParser, this);

	XML_SetElementHandler(iParser, StartElementHandlerL, EndElementHandlerL);
	XML_SetCharacterDataHandler(iParser, CharacterDataHandlerL);
	XML_SetProcessingInstructionHandler(iParser, ProcessingInstructionHandlerL);
	XML_SetXmlDeclHandler(iParser, XmlDeclHandlerL);

	XML_SetNamespaceDeclHandler(iParser, StartNamespaceDeclHandlerL, EndNamespaceDeclHandlerL);

	// This tells Expat to pass all three parts of element/attribute names to us: prefix, uri and local part
	XML_SetReturnNSTriplet(iParser, 1);

	XML_SetSkippedEntityHandler(iParser, SkippedEntityHandlerL);
	XML_SetExternalEntityRefHandler(iParser, ExternalEntityRefHandlerL);
	}

TDes8& CExpat::GetBufferL(TInt aLength)
	{
	TUint8* buf = (TUint8*) User::LeaveIfNull(XML_GetBuffer(iParser, aLength));

	iInputBuffer.Set(buf, 0, aLength);

	iBufferReady = ETrue;

	return iInputBuffer;
	}

void CExpat::ParseL(TBool done)
	{
	// GetBufferL must be called before each call to this function
	if(!iBufferReady)
		User::Leave(KErrNotReady);

	iBufferReady = EFalse;

	XML_Status status = XML_STATUS_OK;
	TInt error = KErrNone;

	// Two forms of error come out of here: error will be set if an MContentHandler function has left,
	// status notifies us of an internal from Expat
	TRAP(error, status=XML_ParseBuffer(iParser, iInputBuffer.Length(), done));

	if(error!=KErrNone)
		{
		// If a MContentHandler function left it is possible that some allocated memory may be
		// leaked.  We don't take that chance and de-allocate the parser and all its memory.
		iParser = 0;
		iAllocator.ResetL();

		ClearElementStack();
		
		// ResetL must be called to re-activate it.
		ResetL();
		
		User::Leave(error);
		}
		
	if(status==XML_STATUS_ERROR)
		{
		error = (TInt)XML_GetErrorCode(iParser);

		if(error == (TInt)XML_ERROR_NO_MEMORY)
			error = KErrNoMemory;
		else
			error += (EXmlSyntax-XML_ERROR_SYNTAX);  // Convert to our external error range

		ClearElementStack();

		// We don't want to regenerate the same callback. So check
		// that the conditions are different.
		// I.e. if the last parse resulted in a fatal error then
		// subsequent parses will invoke the errorProcessor with the 
		// same error and status returned. Nothing more will be parsed.
		if (iPrevStatus != XML_STATUS_ERROR || iPrevError != error)
			{
			iContentHandler->OnError(error);
			}
		}
	else if(done)
		{
		iContentHandler->OnEndDocumentL(KErrNone);
		}

	iPrevError = error;
	iPrevStatus = status;
	}

const TInt KOptionalFeatures  = EConvertTagsToLowerCase | EReportNamespaceMapping;
const TInt KMandatoryFeatures = EReportNamespaces | EReportNamespacePrefixes;

TInt CExpat::EnableFeature(TInt aParseMode)
	{
	if(aParseMode & ~(KOptionalFeatures | KMandatoryFeatures))
		return KErrNotSupported;

	iParseMode |= aParseMode | KMandatoryFeatures;

	return KErrNone;
	}

TInt CExpat::DisableFeature(TInt aParseMode)
	{
	if(aParseMode & KMandatoryFeatures)
		return KErrNotSupported;

	iParseMode &= ~aParseMode;

	return KErrNone;
	}

TBool CExpat::IsFeatureEnabled(TInt aParseMode) const
	{
	return iParseMode & aParseMode;
	}


void CExpat::SetContentSink(class MContentHandler &aContentSink)
	{
	iContentHandler = &aContentSink;
	}

/*
CreateRStringL converts the UTF-16 descriptor to UTF-8 and stores it in the stringpool.
*/
void CExpat::CreateRStringL(const TDesC& aInput, RString& aString, TBool aLowerCase)
	{
	TPtr8 conversionOutput(0,0);
	//Uses the more efficient TPtr overload of ConvertFromUnicode.
	TInt error = iCharSetConverter.ConvertFromUnicodeL(aInput, KCharacterSetIdentifierUtf8, conversionOutput);	
	User::LeaveIfError(error>0 ? KErrCorrupt : error);

	if(aLowerCase)
		conversionOutput.LowerCase();

	aString = iStringPool.OpenStringL(conversionOutput);
	}

/*
ScanNameL takes a null-terminated UTF-16 string of the form "uri!prefix!localpart" (although prefix, or uri and prefix,
may not be there.  The separated parts will be converted to UTF-8 and stored in the stringpool.

The prefix and local part will be converted to lowercase if EConvertTagsToLowerCase is one of the
current parse modes.
*/
void CExpat::ScanNameL(const XML_Char* aName16, RString& aUriString, RString& aLocalPartString, RString& aPrefixString)
	{
	const TUint16* uri = 0;
	const TUint16* prefix = 0;
	const TUint16* localPart;
	const TUint16* p;

	p = localPart = (TUint16*)aName16;

	while(*p)
		if(*p++ == KNamespaceSeparator)
			{
			uri = (TUint16*)aName16;
			localPart = p;
			break;
			}

	while(*p)
		if(*p++ == KNamespaceSeparator)
			{
			prefix = p;
			break;
			}

	while(*p)
		p++;

	TBool lowerCase = iParseMode & EConvertTagsToLowerCase;
	XML_Char nullString[] = "";

	if(uri)
		CreateRStringL(TPtrC16(uri, localPart-uri-1), aUriString, EFalse);
	else
		aUriString = iStringPool.OpenStringL(TPtrC8((unsigned char*)nullString, 0));
	CleanupClosePushL(aUriString);


	if(prefix)
		CreateRStringL(TPtrC16(prefix, p-prefix), aPrefixString , lowerCase);
	else
		aPrefixString = iStringPool.OpenStringL(TPtrC8((unsigned char*)nullString, 0));
	CleanupClosePushL(aPrefixString);

	CreateRStringL(TPtrC16(localPart, (prefix ? prefix-1 : p)-localPart), aLocalPartString, lowerCase);

	CleanupStack::Pop(2);
	} 

void CExpat::StartElementHandlerL(void* aUserData, const XML_Char* aName, const XML_Char** aAtts)
	{
	((CExpat*)aUserData)->HandleStartElementL(aName, aAtts);
	}

void CExpat::HandleStartElementL(const XML_Char* aName, const XML_Char** aAtts)
	{
	RString uriString, nameString, prefixString;
	ScanNameL(aName, uriString, nameString, prefixString);

	RTagInfo tagInfo;
	tagInfo.Open(uriString, prefixString, nameString);
	CleanupClosePushL(tagInfo);

	// Only create a copy of nameString if the append succeeds
	User::LeaveIfError(iElementStack.Append(nameString));
	nameString.Copy(); // i.e. iElementStack now has a copy so flag this to the stringpool

	// Count the number of attributes in aAtts
	//
	// The layout of aAtts is an array of name, value, name, value, name, ...
	// ... hence each attribute has two strings giving rise to the "att += 2;"
	const XML_Char** att = aAtts;
	while (*att)
		att += 2;
	TInt nAttributes = (att-aAtts)/2;

	RAttributeArray attributes;
	CleanupStack::PushL(TCleanupItem(AttributeArrayDelete, &attributes));

	att = aAtts;
	for(TInt i=0; i<nAttributes; i++)
		{
		RString attUriString, attNameString, attPrefixString;
		ScanNameL(*att, attUriString, attNameString, attPrefixString);
		CleanupClosePushL(attUriString);
		CleanupClosePushL(attNameString);
		CleanupClosePushL(attPrefixString);
		att++;

		
		RString attValueString;
		CreateRStringL(TPtrC((TUint16*)*att, StringLen((TUint16*)*att)), attValueString, EFalse);
		att++;

		CleanupStack::Pop(3);  // RAttribute will own the RStrings

		RAttribute attribute;
		attribute.Open(attUriString, attPrefixString, attNameString, attValueString);

		CleanupClosePushL(attribute);
		User::LeaveIfError(attributes.Append(attribute));
		CleanupStack::Pop();
		}

	iContentHandler->OnStartElementL(tagInfo, attributes, KErrNone);

	CleanupStack::PopAndDestroy(2);  // Finished with RTagInfo and RAttributeArray
	}

void CExpat::EndElementHandlerL(void* aUserData, const XML_Char* aName)
	{
	((CExpat*)aUserData)->HandleEndElementL(aName);
	}

void CExpat::HandleEndElementL(const XML_Char* aName)
	{
	RString uriString, nameString, prefixString;
	ScanNameL(aName, uriString, nameString, prefixString);

	RTagInfo tagInfo;
	tagInfo.Open(uriString, prefixString, nameString);
	CleanupClosePushL(tagInfo);

	TInt i = iElementStack.Count() - 1;

	// Expat will raise an error if the wrong element is closed.  We only get an error here
	// if somehow our element stack is wrong.
	if(iElementStack[i]!=nameString)
		User::Leave(KErrCorrupt);
	
	iElementStack[i].Close();
	iElementStack.Remove(i);

	iContentHandler->OnEndElementL(tagInfo, KErrNone);

	CleanupStack::PopAndDestroy();  // Finished with RTagInfo
	}

void CExpat::CharacterDataHandlerL(void* aUserData, const XML_Char* aString, int aLen)
	{
	((CExpat*)aUserData)->HandleCharacterDataL(aString, aLen);
	}

void CExpat::HandleCharacterDataL(const XML_Char* aString, int aLen)
	{
	TPtr8 conversionOutput(0,0);
	//Uses the more efficient TPtr overload of ConvertFromUnicode
	TInt error = iCharSetConverter.ConvertFromUnicodeL(TPtrC16((TUint16*)aString, aLen), KCharacterSetIdentifierUtf8, conversionOutput);
	User::LeaveIfError(error>0 ? KErrCorrupt : error);
	iContentHandler->OnContentL(conversionOutput, KErrNone);
	}

void CExpat::ProcessingInstructionHandlerL(void* aUserData, const XML_Char* aTarget, const XML_Char* aData)
	{
	((CExpat*)aUserData)->HandleProcessingInstructionL(aTarget, aData);
	}

void CExpat::HandleProcessingInstructionL(const XML_Char* aTarget, const XML_Char* aData)
	{
	TPtr8 utf8target(0,0);
	//Uses the more efficient TPtr overload of the ConvertFromUnicodeL function
	TInt error = iCharSetConverter.ConvertFromUnicodeL(TPtrC16((TUint16*)aTarget, StringLen((TUint16*)aTarget)), KCharacterSetIdentifierUtf8, utf8target);
	User::LeaveIfError(error>0 ? KErrCorrupt : error);

	HBufC8 *utf8data;
	//Uses the HBufC overload of ConvertFromUnicodeL as the data in the TPtr overload is still needed. 
	error = iCharSetConverter.ConvertFromUnicodeL(TPtrC16((TUint16*)aData, StringLen((TUint16*)aData)), KCharacterSetIdentifierUtf8, utf8data);
	CleanupStack::PushL(utf8data);
	User::LeaveIfError(error>0 ? KErrCorrupt : error);

	iContentHandler->OnProcessingInstructionL(utf8target, utf8data->Des(), KErrNone);

	CleanupStack::PopAndDestroy(utf8data);
	}

void CExpat::XmlDeclHandlerL(void* aUserData, const XML_Char* aVersion, const XML_Char* aEncoding, int aStandalone)
	{
	((CExpat*)aUserData)->HandleXmlDeclL(aVersion, aEncoding, aStandalone);
	}

void CExpat::HandleXmlDeclL(const XML_Char*, const XML_Char* aEncoding16, int)
	{
	//
	// Encoding
	//
	RString encodingString;
	
	if (aEncoding16)
		{
		TInt len = StringLen((TUint16*)aEncoding16);
		TPtr8 encoding8 = HBufC8::NewLC(len)->Des();
		encoding8.Copy(TPtrC16((TUint16*)aEncoding16, len));
		encodingString = iStringPool.OpenStringL(encoding8);
		CleanupStack::PopAndDestroy();
		}
	else
		{
		encodingString = iStringPool.OpenStringL(KNullDesC8());
		}
		
	CleanupClosePushL(encodingString);

	RDocumentParameters params;
	params.Open(encodingString);
	CleanupStack::Pop(); // encodingString
	CleanupClosePushL(params);

	iContentHandler->OnStartDocumentL(params, KErrNone);

	CleanupStack::PopAndDestroy();  // Finished with params
	}

void CExpat::StartNamespaceDeclHandlerL(void* aUserData, const XML_Char* aPrefix, const XML_Char* aUri)
	{
	((CExpat*)aUserData)->HandleStartNamespaceDeclL(aPrefix, aUri);
	}

void CExpat::HandleStartNamespaceDeclL(const XML_Char* aPrefix, const XML_Char* aUri)
	{
	if(iParseMode & EReportNamespaceMapping)
		{
		RString prefixString;
		RString uriString;
		unsigned char nullString[] = "";

		if(aPrefix)
			CreateRStringL(TPtrC((TUint16*)aPrefix, StringLen((TUint16*)aPrefix)), prefixString, EFalse);
		else
			{
			prefixString = iStringPool.OpenStringL(TPtrC8(nullString, 0));
			}
		CleanupClosePushL(prefixString);


		if(aUri)
			CreateRStringL(TPtrC((TUint16*)aUri, StringLen((TUint16*)aUri)), uriString, EFalse);
		else
			{
			uriString = iStringPool.OpenStringL(TPtrC8(nullString, 0));
			}
		CleanupClosePushL(uriString);

		iContentHandler->OnStartPrefixMappingL(prefixString, uriString, KErrNone);

		CleanupStack::PopAndDestroy(2);
		}
	}

void CExpat::EndNamespaceDeclHandlerL(void* aUserData, const XML_Char* aPrefix)
	{
	((CExpat*)aUserData)->HandleEndNamespaceDeclL(aPrefix);
	}

void CExpat::HandleEndNamespaceDeclL(const XML_Char* aPrefix)
	{
	if(iParseMode & EReportNamespaceMapping)
		{
		RString prefixString;

		if(aPrefix)
			CreateRStringL(TPtrC((TUint16*)aPrefix, StringLen((TUint16*)aPrefix)), prefixString, EFalse);
		else
			{
			unsigned char nullString[] = "";
			prefixString = iStringPool.OpenStringL(TPtrC8(nullString, 0));
			}

		CleanupClosePushL(prefixString);
		iContentHandler->OnEndPrefixMappingL(prefixString, KErrNone);
		CleanupStack::PopAndDestroy();
		}
	}

void CExpat::SkippedEntityHandlerL(void* aUserData, const XML_Char* aName, int aIsParamEntity)
	{
	((CExpat*)aUserData)->HandleSkippedEntityL(aName, aIsParamEntity);
	}

void CExpat::HandleSkippedEntityL(const XML_Char* aName, int)
	{
	RString nameString;
	CreateRStringL(TPtrC((TUint16*)aName, StringLen((TUint16*)aName)), nameString, EFalse);

	CleanupClosePushL(nameString);
	iContentHandler->OnSkippedEntityL(nameString, KErrNone);
	CleanupStack::PopAndDestroy();
	}

int CExpat::ExternalEntityRefHandlerL(void* aUserData, const XML_Char* aName)
	{
	((CExpat*)aUserData)->HandleSkippedEntityL(aName, 0);
	return 1;
	}

void CExpat::ClearElementStack()
	{
	for(TInt i=iElementStack.Count()-1; i>=0; i--)
		iElementStack[i].Close();

	iElementStack.Reset();
	}

LOCAL_C void AttributeArrayDelete(TAny *aPtr)
	{
	RAttributeArray& attributes = *(RAttributeArray*)aPtr;

	TInt nAttributes = attributes.Count();

	for(TInt i=0; i<nAttributes; i++)
		attributes[i].Close();

	attributes.Close();
	}

RHeap* CExpat::GetInternalHeap() const
	{
	return iAllocator.GetHeap();
	}


//
// CExpat::TExpatAlloc
//
#ifndef _DEBUG
void CExpat::TExpatAlloc::ConstructL(TInt aHeapSize, TInt)
	{
#else
void CExpat::TExpatAlloc::ConstructL(TInt aHeapSize, TInt aDebugFailCount)
	{
	// The first failure is a simulated chunk/heap creation failure
	if(aDebugFailCount==1)
		User::Leave(KErrNoMemory);
#endif

	iHeapSize = aHeapSize;
	iHeap = (RHeap*)User::LeaveIfNull(UserHeap::ChunkHeap(0, aHeapSize, aHeapSize));

#ifdef _DEBUG
	if(aDebugFailCount)
		iHeap->__DbgSetAllocFail(RHeap::EFailNext, aDebugFailCount-1);
#endif
	}

CExpat::TExpatAlloc::~TExpatAlloc()
	{
	if(iHeap)
		{
		iHeap->Close();
		iHeap = 0;
		}
	}

void CExpat::TExpatAlloc::ResetL()
	{
	__ASSERT_DEBUG(iHeap, User::Invariant());
	
	if(iHeap)
		iHeap->Close();
	iHeap = UserHeap::ChunkHeap(0, iHeapSize, iHeapSize);
	User::LeaveIfNull(iHeap);
	}

void CExpat::TExpatAlloc::FillAllocStruct(XML_Memory_Handling_Suite& aMem) const
	{
	aMem.malloc_fcn = CExpat::TExpatAlloc::Alloc;
	aMem.realloc_fcn = CExpat::TExpatAlloc::ReAlloc;
	aMem.free_fcn = CExpat::TExpatAlloc::Free;
	aMem.allocData = (void*)this;
	}

void* CExpat::TExpatAlloc::Alloc(void* aUserData, size_t aSize)
	{
	RHeap* heap = ((TExpatAlloc*)aUserData)->iHeap;
	return heap ? heap->Alloc(aSize) : 0;
	}

void* CExpat::TExpatAlloc::ReAlloc(void* aUserData, void* aPtr, size_t aSize)
	{
	RHeap* heap = ((TExpatAlloc*)aUserData)->iHeap;

	// Used during development to ensure good heap.  Left in for future reference
	__ASSERT_DEBUG(heap, User::Invariant());

	return heap ? heap->ReAlloc(aPtr, aSize) : 0;
	}

void CExpat::TExpatAlloc::Free(void* aUserData, void *aPtr)
	{
	RHeap* heap = ((TExpatAlloc*)aUserData)->iHeap;

	// Used during development to ensure good heap.  Left in for future reference
	__ASSERT_DEBUG(heap, User::Invariant());

	if(heap)
		heap->Free(aPtr);
	}

RHeap* CExpat::TExpatAlloc::GetHeap() const
	{
	return iHeap;
	}
