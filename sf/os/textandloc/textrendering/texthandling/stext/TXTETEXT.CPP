/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#include <e32std.h>
#include <e32base.h>

#include <s32std.h>
#include <s32strm.h>
#include <s32stor.h>
#include <s32mem.h>
#include <s32file.h>
#include <s32ucmp.h>

#include <fepitfr.h>

#include "FLDDEF.H"
#include "FLDINFO.H"
#include "FLDSET.H"

#include "TXTETEXT.H"
#include "TXTRICH.H"
#include "TXTOPT.H"
#include "TXTFEP.H"
#include "TXTPLAIN.H"
#include "TXTSTD.H"
#include "TXTRTPFL.H"
#include "TXTCLIPBOARD.H"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TXTETEXTTraces.h"
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TXTETEXT_INTERNAL.H"
#endif

const TUint KFieldCountLimit = 255;
#define UNUSED_VAR(a) a = a

// Write some or all of the text in a buffer to a stream, writing the length first if aWriteLength is true.
static void ExternalizeTextL(RWriteStream& aStream,const CBufBase& aText,TInt aPos,TInt aLength,TBool aWriteLength)
	{
	if (aWriteLength)
		aStream << TCardinality(aLength);

	// Use the Standard Unicode Compression Scheme.
	RBufReadStream input_stream(aText,aPos * sizeof(TText));
	TMemoryStreamUnicodeSource source(input_stream);
	TUnicodeCompressor c;
	c.CompressL(aStream,source,KMaxTInt,aLength);
	input_stream.Close();
	}

// Read text from a stream and write it to a buffer.
static void InternalizeTextL(RReadStream& aStream,CBufBase& aText,TInt aLength)
	{
	// Use the Standard Unicode Compression Scheme.
	RBufWriteStream output_stream(aText);
	TMemoryStreamUnicodeSink sink(output_stream);
	TUnicodeExpander e;
	e.ExpandL(sink,aStream,aLength);
	output_stream.CommitL();
	output_stream.Close();
	}

// Read text from a stream and write it to a buffer; read the length first.
static void InternalizeTextL(RReadStream& aStream,CBufBase& aText)
	{
	TCardinality length;
	aStream >> length;
	InternalizeTextL(aStream,aText,length);
	}
/**
Returns the interface corresponding to the
specified UID if it exists, or 0 if not. Overridden
versions should base call rather than returning 0.

@param aInterfaceId The UID indicating the interface to return
@param aInterface The interface corresponding to aInterfaceId
if it is supported, or 0 if it is not
*/
EXPORT_C void CEditableText::ExtendedInterface(TAny*& /*aInterface*/, TUid /*aInterfaceId*/) {}

/**
Returns the interface corresponding to the
specified UID if it exists, or 0 if not. Overridden
versions should base call rather than returning 0.

@param aInterfaceId The UID indicating the interface to return
@param aInterface The interface corresponding to aInterfaceId
if it is supported, or 0 if it is not
*/
EXPORT_C void CPlainText::ExtendedInterface(TAny*& /*aInterface*/, TUid /*aInterfaceId*/) {}

/**
 @internalAll
 @released
 */
EXPORT_C void CPlainText::Reserved_2() {}

//////////////////////////////////
// CEditableText
//////////////////////////////////

EXPORT_C CEditableText::~CEditableText()
	{
	delete iOptionalData;
	}



EXPORT_C TInt CEditableText::ScanWords(TInt& /*aPos*/,TUint& /*aScanMask*/) const
/** Scans the text from a specified document position to a location 
determined by the flags specified in a bitmask. The function can scan 
forwards or backwards to the beginning or end of a word.

@param aPos A valid document position from which to scan. On return, 
contains the new document position. 
@param aScanMask The scan mask to use. See the scanning enumeration defined 
in class CPlainText. 
@return The number of characters skipped to reach the new document position. 
Notes: If the scan passes the end of text delimiter, on return, aPos is set 
to EScanEndOfData  and the function's return value indicates the 
number of characters skipped in passing the end of text delimiter. */
	{
	return 0;
	}


EXPORT_C TInt CEditableText::ScanParas(TInt& /*aPos*/,TUint& /*aScanMask*/) const
/** Scans the text from a specified document position to a location determined 
by the flags specified in a bitmask. The function can scan forwards or backwards 
to the beginning or end of a paragraph.

@param aPos A valid document position from which to scan. On return, contains 
the new document position. 
@param aScanMask The scan mask to use. See the scanning enumeration defined 
in class CPlainText. 
@return The number of characters skipped to reach the new document position. 
Notes: If the scan passes the end of text delimiter, on return, aPos is set 
to EScanEndOfData  and the function's return value indicates the 
number of characters skipped in passing the end of text delimiter. */
	{
	return 0;
	}


 
EXPORT_C void CEditableText::SetHasChanged(TBool aHasChanged)
/** Sets whether a change has occurred to the editable text object. This is called 
by functions which change the text object in some way.

@param aHasChanged ETrue if a change has occurred to the text object. EFalse 
if no change has occurred. */
	{
	iHasChanged = aHasChanged;
	}


// Save the editable text type identifier.
void CEditableText::ExternalizeL(RWriteStream& aStream)const
	{
	aStream << KEditableTextUid;
	}


void CEditableText::InternalizeL(RReadStream& aStream)
// Read from the stream, expecting the editable text type identifier
//
	{
	TUid uid;
	aStream>> uid;
	if (uid!=KEditableTextUid)
		User::Leave(KErrCorrupt);
	}


 

EXPORT_C TStreamId CEditableText::StoreL(CStreamStore& aStore)const
/** Stores the text and its components. The components (e.g. fields and pictures) 
are stored in separate streams within the stream store.

@param aStore Stream store to which the text and text components are written. 
@return The ID of the stream store. */
	{
	CStoreMap* map=CStoreMap::NewLC(aStore);
	StoreComponentsL(aStore,*map);
//
	RStoreWriteStream stream(*map);
	TStreamId id=stream.CreateLC(aStore);
	stream<< *this;
	stream.CommitL();
//
	map->Reset();
	CleanupStack::PopAndDestroy(2);  // map,stream
	return id;
	}


 
EXPORT_C void CEditableText::RestoreL(const CStreamStore& aStore,TStreamId aStreamId)
/** Restores the text and its components from a stream store.

@param aStore Stream store containing the text and its components. 
@param aStreamId The ID of the stream store in which the text was previously 
stored. */
	{
	// Load text and field components only.  (Pictures, if present, are deferred loaded).
	__ETEXT_WATCH(RESTORE)

	RStoreReadStream stream;
	stream.OpenLC(aStore,aStreamId);
	//
	stream>> *this;
	CleanupStack::PopAndDestroy();  // stream
	RestoreComponentsL(aStore);

	__ETEXT_WATCH_END(RESTORE)
	}

TBool CEditableText::DeleteWithoutDestroyingFormatL(TInt aPos, TInt aLength)
/** Deletes a range of characters. For rich text the format of the deleted character 
at position aPos is preserved, so that any text subsequently inserted at aPos will have 
that format applied to it.
@param aPos The document position from which to begin deleting including aPos.
@param aLength The number of characters to delete. Must be positive or a panic 
occurs. The sum of aPos and aLength must be less than the document length, 
or a panic occurs.  
@return Indicates whether two paragraphs have been merged together as a result 
of the delete, indicating that the resulting paragraph must be reformatted. 
*/	
	{
	TAny* richTextInterface = NULL;	
	ExtendedInterface(richTextInterface, KUidRichText);

	if(richTextInterface)
		{
		return REINTERPRET_CAST(CRichText*, richTextInterface)->DelSetInsertCharFormatL(aPos, aLength);
		}
	else
		{
		return DeleteL(aPos, aLength);
		}
	}

EXPORT_C void CEditableText::StartFepInlineEditL(TBool& aParagraphContainingStartPositionOfInlineTextHasChangedFormat,TInt& aNumberOfCharactersSuccessfullyDeleted,TInt& aNumberOfCharactersSuccessfullyInserted,TInt& aPositionOfInsertionPointInDocument,TInt aNewPositionOfInsertionPointInDocument,const TDesC& aInitialInlineText,TInt aPositionOfInlineTextInDocument,TInt aNumberOfCharactersToHide,MFepInlineTextFormatRetriever& aInlineTextFormatRetriever)
/** @internalAll */	
	{
	if (aPositionOfInlineTextInDocument<0 || aNumberOfCharactersToHide<0 || aPositionOfInlineTextInDocument+aNumberOfCharactersToHide>DocumentLength())
	    {
	    OstTrace0( TRACE_DUMP, CEDITABLETEXT_STARTFEPINLINEEDITL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPositionOfInlineTextInDocument>=0 && aNumberOfCharactersToHide>=0 && aPositionOfInlineTextInDocument+aNumberOfCharactersToHide<=DocumentLength(),Panic(ECharPosBeyondDocument));
	if (InlineEditData()!=NULL)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CEDITABLETEXT_STARTFEPINLINEEDITL, "EAlreadyFepInlineEditing" );
	    }
	__ASSERT_ALWAYS(InlineEditData()==NULL,Panic(EAlreadyFepInlineEditing));
	aParagraphContainingStartPositionOfInlineTextHasChangedFormat=EFalse;
	aNumberOfCharactersSuccessfullyDeleted=0;
	aNumberOfCharactersSuccessfullyInserted=0;
	CInlineEditData* const inlineEditData=new(ELeave) CInlineEditData;
	CleanupStack::PushL(inlineEditData);
	HBufC* hiddenText=NULL;
	CleanupStack::PushL(hiddenText);
	if (aNumberOfCharactersToHide>0)
		{
		CleanupStack::Pop(); // hiddenText
		hiddenText=HBufC::NewLC(aNumberOfCharactersToHide);
		TPtr hiddenTextAsWritableDescriptor=hiddenText->Des();
		Extract(hiddenTextAsWritableDescriptor,aPositionOfInlineTextInDocument,aNumberOfCharactersToHide);
		aParagraphContainingStartPositionOfInlineTextHasChangedFormat=DeleteWithoutDestroyingFormatL(aPositionOfInlineTextInDocument,aNumberOfCharactersToHide);
		aNumberOfCharactersSuccessfullyDeleted=aNumberOfCharactersToHide;
		aPositionOfInsertionPointInDocument=aPositionOfInlineTextInDocument;
		}
	inlineEditData->iPositionOfInlineTextInDocument=aPositionOfInlineTextInDocument;
	inlineEditData->iLengthOfInlineText=aInitialInlineText.Length();
	inlineEditData->iInlineText=aInitialInlineText.AllocL();
	inlineEditData->iHiddenText=hiddenText;
	CleanupStack::Pop(); // hiddentext now owned by inlineEditData. 
	inlineEditData->iInlineTextFormatRetriever=&aInlineTextFormatRetriever;
	InsertL(aPositionOfInlineTextInDocument,aInitialInlineText);
	SetAndTransferOwnershipOfInlineEditDataL(inlineEditData);
	CleanupStack::Pop(); // inlineEditData
	aNumberOfCharactersSuccessfullyInserted=inlineEditData->iLengthOfInlineText;
	aPositionOfInsertionPointInDocument=aNewPositionOfInsertionPointInDocument;
	}


EXPORT_C void CEditableText::UpdateFepInlineTextL(TBool& aParagraphContainingStartPositionOfInlineTextHasChangedFormat,TInt& aNumberOfCharactersSuccessfullyDeleted,TInt& aNumberOfCharactersSuccessfullyInserted,TInt& aPositionOfInsertionPointInDocument,TInt aNewPositionOfInsertionPointInDocument,const TDesC& aNewInlineText)
/** @internalAll */	
    {
	CInlineEditData* const inlineEditData=InlineEditData();
	if (inlineEditData==NULL)
	    {
	    OstTrace0( TRACE_DUMP, CEDITABLETEXT_UPDATEFEPINLINETEXTL, "ENotCurrentlyFepInlineEditing" );
	    }
	__ASSERT_ALWAYS(inlineEditData!=NULL,Panic(ENotCurrentlyFepInlineEditing));
	aParagraphContainingStartPositionOfInlineTextHasChangedFormat=EFalse;
	aNumberOfCharactersSuccessfullyDeleted=0;
	aNumberOfCharactersSuccessfullyInserted=0;
	HBufC*& inlineText=inlineEditData->iInlineText;
	HBufC* oldInlineText=inlineText;
	if (oldInlineText!=NULL && inlineEditData->iLengthOfInlineText!=oldInlineText->Length())
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CEDITABLETEXT_UPDATEFEPINLINETEXTL, "EDebug" );
	    }
	__ASSERT_DEBUG(oldInlineText==NULL || inlineEditData->iLengthOfInlineText==oldInlineText->Length(),Panic(EDebug));
	const TInt lengthOfNewInlineText=aNewInlineText.Length();
	if (oldInlineText!=NULL && *oldInlineText==aNewInlineText)
		{
		aNumberOfCharactersSuccessfullyDeleted=lengthOfNewInlineText;
		aNumberOfCharactersSuccessfullyInserted=lengthOfNewInlineText;
		}
	else
		{
		if (oldInlineText==NULL)
			{
			oldInlineText=HBufC::NewL(lengthOfNewInlineText);
			}
		else if (lengthOfNewInlineText>oldInlineText->Length())
			{
			oldInlineText=oldInlineText->ReAllocL(lengthOfNewInlineText);
			}
		CleanupStack::PushL(oldInlineText);
		inlineText=NULL; // sets inlineEditData->iLengthOfInlineText in case either the delete or the insert leaves
		const TInt positionOfInlineTextInDocument=inlineEditData->iPositionOfInlineTextInDocument;
		TInt& lengthOfInlineText=inlineEditData->iLengthOfInlineText;
		if (lengthOfInlineText>0)
			{
			aParagraphContainingStartPositionOfInlineTextHasChangedFormat=DeleteWithoutDestroyingFormatL(positionOfInlineTextInDocument,lengthOfInlineText);
			aNumberOfCharactersSuccessfullyDeleted=lengthOfInlineText;
			lengthOfInlineText=0; // sets inlineEditData->iLengthOfInlineText in case the insert leaves
			aPositionOfInsertionPointInDocument=inlineEditData->iPositionOfInlineTextInDocument;
			}
		InsertL(positionOfInlineTextInDocument,aNewInlineText);
		lengthOfInlineText=aNewInlineText.Length(); // sets inlineEditData->iLengthOfInlineText
		aNumberOfCharactersSuccessfullyInserted=lengthOfInlineText;
		inlineText=oldInlineText;
		CleanupStack::Pop(); // oldInlineText
		*oldInlineText=aNewInlineText;
		}
	aPositionOfInsertionPointInDocument=aNewPositionOfInsertionPointInDocument;
	}


EXPORT_C void CEditableText::CommitFepInlineEditL(TBool& aParagraphContainingStartPositionOfInlineTextHasChangedFormat,TInt& aNumberOfCharactersSuccessfullyDeleted,TInt& aNumberOfCharactersSuccessfullyInserted,TInt& aPositionOfInsertionPointInDocument,TInt aNewPositionOfInsertionPointInDocument)
/**
 * @internalAll
 */	
    {
	const CInlineEditData* const inlineEditData=InlineEditData();
	if (inlineEditData==NULL)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CEDITABLETEXT_COMMITFEPINLINEEDITL, "ENotCurrentlyFepInlineEditing" );
	    }
	__ASSERT_ALWAYS(inlineEditData!=NULL,Panic(ENotCurrentlyFepInlineEditing));
	if (inlineEditData->iInlineText!=NULL && inlineEditData->iLengthOfInlineText!=inlineEditData->iInlineText->Length())
	    {
	    OstTrace0( TRACE_DUMP, CEDITABLETEXT_COMMITFEPINLINEEDITL, "EDebug" );
	    }
	__ASSERT_DEBUG(inlineEditData->iInlineText==NULL || inlineEditData->iLengthOfInlineText==inlineEditData->iInlineText->Length(),Panic(EDebug));
	aParagraphContainingStartPositionOfInlineTextHasChangedFormat=EFalse;
	const TInt lengthOfInlineText=inlineEditData->iLengthOfInlineText;
	aNumberOfCharactersSuccessfullyDeleted=lengthOfInlineText;
	aNumberOfCharactersSuccessfullyInserted=lengthOfInlineText;
	aPositionOfInsertionPointInDocument=aNewPositionOfInsertionPointInDocument;
	DeleteInlineEditDataAndSetToNull();
	}


EXPORT_C void CEditableText::CancelFepInlineEdit(TBool& aParagraphContainingStartPositionOfInlineTextHasChangedFormat,TInt& aNumberOfCharactersSuccessfullyDeleted,TInt& aNumberOfCharactersSuccessfullyInserted,TInt& aPositionOfInsertionPointInDocument,TInt aNewPositionOfInsertionPointInDocument)
/**
 * @internalAll
 */	
    {
	aParagraphContainingStartPositionOfInlineTextHasChangedFormat=EFalse;
	aNumberOfCharactersSuccessfullyDeleted=0;
	aNumberOfCharactersSuccessfullyInserted=0;
	const CInlineEditData* inlineEditData=InlineEditData();
	if (inlineEditData!=NULL)
		{
		const TInt positionOfInlineTextInDocument=inlineEditData->iPositionOfInlineTextInDocument;
		const TInt lengthOfInlineText=inlineEditData->iLengthOfInlineText;
		if (inlineEditData->iInlineText!=NULL && lengthOfInlineText!=inlineEditData->iInlineText->Length())
		    {
		    OstTrace0( TRACE_DUMP, CEDITABLETEXT_CANCELFEPINLINEEDIT, "EDebug" );
		    }
		__ASSERT_DEBUG(inlineEditData->iInlineText==NULL || lengthOfInlineText==inlineEditData->iInlineText->Length(),Panic(EDebug));
		TRAPD(notUsed,
						if (lengthOfInlineText>0)
							{
							aParagraphContainingStartPositionOfInlineTextHasChangedFormat=DeleteWithoutDestroyingFormatL(positionOfInlineTextInDocument,lengthOfInlineText);
							aNumberOfCharactersSuccessfullyDeleted=lengthOfInlineText;
							aPositionOfInsertionPointInDocument=inlineEditData->iPositionOfInlineTextInDocument;
							}
						const HBufC* const hiddenText=inlineEditData->iHiddenText;
						if (hiddenText!=NULL)
							{
							if (hiddenText->Length()<=0)
							    {
							    OstTrace0( TRACE_DUMP, DUP1_CEDITABLETEXT_CANCELFEPINLINEEDIT, "EDebug" );
							    }
							__ASSERT_DEBUG(hiddenText->Length()>0, Panic(EDebug));
							InsertL(positionOfInlineTextInDocument,*hiddenText);
							aNumberOfCharactersSuccessfullyInserted=hiddenText->Length();
							aPositionOfInsertionPointInDocument=aNewPositionOfInsertionPointInDocument;
							}
			);
        UNUSED_VAR(notUsed);
		DeleteInlineEditDataAndSetToNull();
		}
	}

EXPORT_C void CEditableText::OverrideFormatOfInlineTextIfApplicable(TPtrC& aView,TCharFormat& aFormat,TInt aStartPos)const
	{
	const CInlineEditData* inlineEditData=InlineEditData();
	if (inlineEditData!=NULL)
		{
		const TInt positionOfInlineTextInDocument=inlineEditData->iPositionOfInlineTextInDocument;
		const TInt lengthOfInlineText=inlineEditData->iLengthOfInlineText;
		const TInt originalLengthOfView=aView.Length();
		TInt maximumLengthOfView=originalLengthOfView;
		if (aStartPos<positionOfInlineTextInDocument)
			{
			maximumLengthOfView=positionOfInlineTextInDocument-aStartPos;
			}
		else if (aStartPos<positionOfInlineTextInDocument+lengthOfInlineText)
			{
			inlineEditData->iInlineTextFormatRetriever->GetFormatOfFepInlineText(aFormat,maximumLengthOfView,aStartPos-positionOfInlineTextInDocument);
			}
		if (originalLengthOfView>maximumLengthOfView)
			{
			aView.Set(aView.Left(maximumLengthOfView));
			}
		}
	}


EXPORT_C TInt CEditableText::GetPositionOfInlineTextInDocument() const
	{
	const CInlineEditData* inlineEditData=InlineEditData();
	if (inlineEditData==NULL)
		return KErrNotFound;
	return inlineEditData->iPositionOfInlineTextInDocument;	
	}


EXPORT_C TInt CEditableText::GetLengthOfInlineText() const
	{
	const CInlineEditData* inlineEditData=InlineEditData();
	if (inlineEditData==NULL)
		return KErrNotFound;
	return inlineEditData->iLengthOfInlineText;		
	}


//////////////////////////////////
// TEtextComponentInfo
//////////////////////////////////



EXPORT_C TEtextComponentInfo::TEtextComponentInfo()
	: iFieldCount(0),iPictureCount(0),iStyleCount(0)
/** C++ constructor overloaded function.

The object can be constructed either:by default this initializes the
field, picture and style counts to zerowith a field, picture and style
count

@param aFieldCount Specifies the number of fields in the text object.
@param aPictureCount Specifies the number of pictures in the text object
(rich text only).
@param aStyleCount  Specifies the number of styles owned or referenced by
the text object (rich text only). */
    {}


EXPORT_C TEtextComponentInfo::TEtextComponentInfo(TInt aFieldCount,TInt aPictureCount,TInt aStyleCount)
	: iFieldCount(aFieldCount),iPictureCount(aPictureCount),iStyleCount(aStyleCount)
/** C++ constructor overloaded function. The object can be constructed either:

by default  this initializes the field, picture and style counts to zero

with a field, picture and style count

@param aFieldCount Specifies the number of fields in the text object. 
@param aPictureCount Specifies the number of pictures in the text object (rich 
text only). 
@param aStyleCount Specifies the number of styles owned or referenced by the 
text object (rich text only). */
	{}


//////////////////////////////////
// CPlainText
//////////////////////////////////


EXPORT_C void CPlainText::__DbgTestInvariant()const
// Provides class invariants.  Explanations below:
//
	{
#ifdef _DEBUG
// ASSERT: Storage handle is good.
	__ASSERT_DEBUG(iByteStore!=NULL,User::Invariant());
// ASSERT: The text component must be non-negative in length
	__ASSERT_DEBUG(DocumentLength()>=0,User::Invariant());
#endif
	}




EXPORT_C CPlainText* CPlainText::NewL(TDocumentStorage aStorage,TInt aDefaultTextGranularity)
/** Allocates and constructs a plain text object overloaded function.

The text object's contents may be restored from a stream store. If the
text object supports fields, a field factory should be specified.

@param aStorage  The type of in-memory buffer to use. Defaults to ESegmentedStorage.
@param aDefaultTextGranularity  Specifies the granularity of the in-memory 
buffer. Default is EDefaultTextGranularity bytes (=256).
@param aStore   Stream store from which the object is restored.
@param aStreamId  ID of the stream store.
@param aFieldFactory Pointer to a field factory. A field factory must be
provided if the text object supports the addition of fields.
@return Pointer to the new plain text object. */	
	{
	CPlainText* self=new(ELeave) CPlainText();
	CleanupStack::PushL(self);
	self->ConstructL(aStorage,aDefaultTextGranularity);
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CPlainText* CPlainText::NewL(const CStreamStore& aStore,TStreamId aStreamId,MTextFieldFactory* aFieldFactory,TDocumentStorage aStorage)
/** Returns a handle to a new instance of this class, restored from the specified
read stream.*/
	{
	CPlainText* self=new(ELeave) CPlainText();
	CleanupStack::PushL(self);
	self->ConstructL(aStore,aStreamId,aFieldFactory,aStorage);
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CPlainText::CPlainText()
	{
	SetHasChanged(EFalse);
	}


EXPORT_C void CPlainText::ConstructL(TDocumentStorage aStorage,TInt aDefaultTextGranularity)
/** Allocates storage of CBufFlat or CBufSeg, according
to the parameter aStorage.
Creates & initializes the field set.*/
	{
	DoConstructL(aStorage,aDefaultTextGranularity);
	InsertEodL();

	__TEST_INVARIANT;
	}


EXPORT_C void CPlainText::ConstructL(const CStreamStore& aStore,TStreamId aStreamId,MTextFieldFactory* aFieldFactory,
									 TDocumentStorage aStorage)
/** Allocates storage of CBufFlat or CBufSeg, according
 to the parameter aStorage, restoring contents from the specified read-stream aStream.*/
	{
	DoConstructL(aStorage,EDefaultTextGranularity,aFieldFactory);	
	RestoreL(aStore,aStreamId);

	__TEST_INVARIANT;
	}


EXPORT_C void CPlainText::DoConstructL(TDocumentStorage aStorage,TInt aDefaultTextGranularity,MTextFieldFactory* aFieldFactory)
/** Allocates storage of CBufFlat or CBufSeg, according to the parameter aStorage.
Creates & initializes the field set.*/
	{
	if (iByteStore!=NULL)
	    {
	    OstTrace0( TRACE_DUMP, CPLAINTEXT_DOCONSTRUCTL, "EConstructCalledTwice" );
	    }
	__ASSERT_DEBUG(iByteStore==NULL,Panic(EConstructCalledTwice));
	
	iByteStore=(aStorage==ESegmentedStorage)
		? (CBufBase*)CBufSeg::NewL(aDefaultTextGranularity*sizeof(TText))
		: (CBufBase*)CBufFlat::NewL(aDefaultTextGranularity*sizeof(TText));
	if (aFieldFactory)
		SetFieldFactory(aFieldFactory);
	}



EXPORT_C CPlainText::~CPlainText()
/** The destructor frees the object's text storage and field set, prior to its 
destruction. */
	{
	KillFieldSet();
	delete iByteStore;
	}


void CPlainText::KillFieldSet()
/** Delete the field set if it is resident in memory.*/
	{
	if (FieldSetPresent())
		delete iFieldSet.AsPtr();
	iFieldSet=NULL;
	}


//
// CPlainText - Persistence


EXPORT_C void CPlainText::StoreComponentsL(CStreamStore& aStore,CStoreMap& aMap)const
/** Stores the plain text object's components to the stream store specified.

@param aStore Stream store to which the text object's components are written. 
@param aMap A store map. This binds the address of each component to the stream 
ID of aStore. This is needed to support the deferred loading of pictures in 
rich text. */
	{
	// Store any field components, then store the field set out-of-line, if present.
	if (FieldSetPresent())
		{
		TStreamId id=iFieldSet->StoreL(aStore);
		aMap.BindL(iFieldSet,id);
		}
	}



EXPORT_C void CPlainText::RestoreComponentsL(const CStreamStore& aStore)
/** Restores the plain text object's field set from a stream store.

@param aStore The stream store from which the field set is restored. */
	{
	// Load the field set, and load any referenced pictures
	TStreamId id=iFieldSet.AsId();
	if (id!=KNullStreamId)
		{
		CreateFieldSetL(0);
		iFieldSet->SetFieldFactory(iFieldFactory);
		iFieldSet->RestoreL(aStore,id);
		}
	}



EXPORT_C void CPlainText::StoreFieldComponentsL(CStreamStore& aStore,CStoreMap& aMap)const
/** Stores the plain text object's field components to a stream store.

@param aStore Stream store to which the fields are written. 
@param aMap A store map. This binds the address of each text component to the 
stream ID of aStore. This is needed to support the deferred loading of pictures 
in rich text. */
	{
	// 2' StoreComponents() 
	// Only has effect if a field set is present.
	if (FieldSetPresent())
		iFieldSet->StoreFieldsL(aStore,aMap);
	}



EXPORT_C void CPlainText::RestoreFieldComponentsL(const CStreamStore& aStore)
/** Restores the plain text object's field set.

@param aStore The stream store from which the fields are restored. */
	{
	// 2' RestoreComponents()
	// Only has effect if a field set is present - (has been Internalized())
	if (FieldSetPresent())
		iFieldSet->RestoreFieldsL(aStore);
	}


	
EXPORT_C void CPlainText::ExternalizeL(RWriteStream& aStream)const
/** Externalises a plain text object to a write stream. The presence of this function 
means that the standard templated operator<<() (defined in s32strm.h) is available 
to externalise objects of this class.

@param aStream Stream to which the object should be externalised. */
	{
	// Store this object in the specified write-stream.
	CEditableText::ExternalizeL(aStream);
	DoExternalizeFieldDataL(aStream);
	DoExternalizePlainTextL(aStream);
	}



EXPORT_C void CPlainText::InternalizeL(RReadStream& aStream)
/** Internalises the text object's text content and field set from a read stream. 
The presence of this function means that the standard templated operator>>() 
(defined in s32strm.h) is available to internalise objects of this class. 
InternalizeL() has construct rather than assignment semantics. You should 
not use it for fully initialised objects.

@param aStream Stream from which the object should be internalised. */
	{
	// Restores plain text from the specified read-stream.
	// Internalize has construction semantics, not assignment semantics.
	CEditableText::InternalizeL(aStream);
	DoInternalizeFieldDataL(aStream);
	DoInternalizePlainTextL(aStream);
	
	__TEST_INVARIANT;
	}



EXPORT_C void CPlainText::ExternalizeFieldDataL(RWriteStream& aStream)const
/** Externalises the plain text object's field set.

@param aStream The stream to which the field set should be written. */
	{
	// Save just the field set
	__TEST_INVARIANT;

	TUint fieldCount=(TUint)FieldCount();
	if(fieldCount<KFieldCountLimit)
		aStream.WriteUint8L(fieldCount);
	else
		{
		aStream.WriteUint8L(KFieldCountLimit);
		aStream.WriteUint32L(fieldCount);
		}
	if (fieldCount>0)
		aStream<< *iFieldSet;
	}



EXPORT_C void CPlainText::InternalizeFieldDataL(RReadStream& aStream)
/** Internalizes the field set.

@param aStream The read stream from which the field set is read. */
	{
	// 2' InternalizeL()
	// Restores field records from the specified read-stream.
	// Internalize has construction semantics, not assignment semantics.

	TUint fieldCount=aStream.ReadUint8L();
	if (fieldCount==KFieldCountLimit)
		fieldCount=aStream.ReadUint32L();
	if (fieldCount>0)
		{
		if (!iFieldSet)
			CreateFieldSetL(DocumentLength());
		aStream>> *iFieldSet;
		}
	}


EXPORT_C void CPlainText::DoInternalizeFieldDataL(RReadStream& aStream)
/** Read from the stream until the field data is identified, and consume it.*/
	{
	TUid uid=UidFromStreamL(aStream);
	while (uid!=KPlainTextFieldDataUid)
 		{
		if (uid==KPlainTextCharacterDataUid)
			User::Leave(KErrCorrupt);  // There is no field Data !!!!!
		CPlainText::ConsumeAdornmentL(aStream);
		uid=UidFromStreamL(aStream);
		}
	if (FieldSetPresent())
		iFieldFactory=iFieldSet->FieldFactory();
	KillFieldSet();
	aStream>> iFieldSet;  // a swizzle
	}


EXPORT_C void CPlainText::DoExternalizeFieldDataL(RWriteStream& aStream)const
/** Write to the stream, the T.V. representing the field set.*/
	{
	aStream<< KPlainTextFieldDataUid;
	if (FieldSetPresent())
		aStream<< iFieldSet;
	else
		aStream<< KNullStreamId;
	}


EXPORT_C void CPlainText::DoExternalizePlainTextL(RWriteStream& aStream)const
/** Write to the stream, the T.V. representing the plain text.*/
	{
	aStream<< KPlainTextCharacterDataUid;
	ExternalizePlainTextL(aStream);
	}



EXPORT_C void CPlainText::ExternalizePlainTextL(RWriteStream& aStream)const
	
/** Externalises the plain text object's text content (preceded by a length count) 
to a write stream.

@param aStream Stream to which the text content should be externalised. */
	{
	// Save just the bytestore
	__TEST_INVARIANT;
	::ExternalizeTextL(aStream,*iByteStore,0,iByteStore->Size() / sizeof(TText),TRUE);
	}

EXPORT_C void CPlainText::DoInternalizePlainTextL(RReadStream& aStream)
/** Read from the stream until the character data is found, and consume it.*/
	{
	TUid uid=UidFromStreamL(aStream);
	while (uid!=KPlainTextCharacterDataUid)
		{
		CPlainText::ConsumeAdornmentL(aStream);
		uid=UidFromStreamL(aStream);
		}
	CPlainText::InternalizePlainTextL(aStream);
	}

EXPORT_C void CPlainText::InternalizePlainTextL(RReadStream& aStream)
/** Internalises an empty text object's text content from a read stream
overloaded function.

This function has construct rather than assignment semantics. You
should not use it for fully initialised objects.NoteThe overload which
takes a length argument is not intended for general use and its use is
deprecated.

@param aStream Stream from which the object should be internalised.
@param  aLength Indicates the number of characters which should be
read, after expansion from their compressed format. */	
    {
	// Restores plain text content from the specified read-stream.
	// Internalize has construction semantics, not assignment semantics.
	::InternalizeTextL(aStream,*iByteStore);
	SetHasChanged(EFalse);

	__TEST_INVARIANT;
	}


EXPORT_C void CPlainText::ExternalizePlainTextNoLengthCountL(RWriteStream& aStream)const
/** Externalises the plain text object's text content to a write stream.

This function differs from ExternalizePlainTextL() in that 
it does not precede the text content with a length count.
This function is not intended for general use and is deprecated.
@see void CPlainText::ExternalizePlainTextL(RWriteStream& aStream)const
@deprecated */
	{
	::ExternalizeTextL(aStream,*iByteStore,0,iByteStore->Size() / sizeof(TText),FALSE);
	}


EXPORT_C void CPlainText::InternalizePlainTextL(RReadStream& aStream,TInt aLength)
/** Internalises an empty text object's text content from a read stream's 
overloaded function.

This function has construct rather than assignment semantics. You should not 
use it for fully initialised objects.

Note

The overload which takes a length argument is not intended for general use 
and its use is deprecated. 

@param aStream Stream from which the object should be internalised. 
@param aLength Indicates the number of characters which should be read, after 
expansion from their compressed format. 
@deprecated */
	{
	::InternalizeTextL(aStream,*iByteStore,aLength);
	}


// Copy the specified section of plain text to the specified store.


EXPORT_C void CPlainText::CopyToStoreL(CStreamStore& aStore,CStreamDictionary& aDictionary,TInt aPos,TInt aLength) const
/** Copies plain text including fields, if present, to the clipboard.

A panic occurs in any of the following circumstances:

aPos is invalid

aLength is invalid (zero or less)

the sum of aPos and aLength is greater than or equal to the number of characters 
in the document

@param aStore Stream store to which the text is written. 
@param aDictionary The stream dictionary. 
@param aPos The document position from which to begin copying. 
@param aLength The number of characters to copy. */
	{
	if (aLength > 0)
		DoCopyToStoreL(aStore,aDictionary,aPos,aLength);
	}


TStreamId CPlainText::DoCopyToStoreL(CStreamStore& aStore,CStreamDictionary& aDictionary,TInt aPos,TInt aLength) const
	{
	__TEST_INVARIANT;
	TInt documentLength = DocumentLength();
	if (aPos < 0 || aPos > documentLength)
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_DOCOPYTOSTOREL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= documentLength,Panic(ECharPosBeyondDocument));
	if (aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CPLAINTEXT_DOCOPYTOSTOREL, "ECopyToStreamNegativeLength" );
	    }
	__ASSERT_ALWAYS(aLength >= 0,Panic(ECopyToStreamNegativeLength));
	if (aPos + aLength > documentLength)
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CPLAINTEXT_DOCOPYTOSTOREL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + aLength <= documentLength,Panic(ECharPosBeyondDocument));

	if (aLength == 0)
		return KNullStreamId;

	CStoreMap* map=CStoreMap::NewLC(aStore);
	CopyComponentsL(aStore,*map,aPos,aLength);

	// create custom externalizer over the map
	TFieldMapExternalizer fMap(*map);
	RStoreWriteStream stream(fMap);
	TStreamId id=stream.CreateLC(aStore);
	CopyToStreamL(stream,aPos,aLength);
	stream.CommitL();

	aDictionary.AssignL(KClipboardUidTypePlainText,id);
	map->Reset();
	CleanupStack::PopAndDestroy(2);

	__TEST_INVARIANT;
	return id;
	}


void CPlainText::CopyComponentsL(CStreamStore& aStore,CStoreMap& aMap,TInt aPos,TInt aLength)const
// Copy/Paste 2' StoreComponentsL() - only if a field set is present.
// 
	{
	if (FieldSetPresent())
		iFieldSet->CopyComponentsL(aStore,aMap,aPos,aLength);
	}


// Write the plain text to the stream.
void CPlainText::CopyToStreamL(RWriteStream& aStream,TInt aPos,TInt aLength)const
	{
	__TEST_INVARIANT;
	TInt documentLength = DocumentLength();
	if (aPos < 0 || aPos > documentLength)
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_COPYTOSTREAML, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= documentLength,Panic(ECharPosBeyondDocument));
	if (aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CPLAINTEXT_COPYTOSTREAML, "ECopyToStreamNegativeLength" );
	    }
	__ASSERT_ALWAYS(aLength >= 0,Panic(ECopyToStreamNegativeLength));
	if (aPos + aLength > documentLength)
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CPLAINTEXT_COPYTOSTREAML, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + aLength <= documentLength,Panic(ECharPosBeyondDocument));

	aStream.WriteInt32L(aLength);
	::ExternalizeTextL(aStream,*iByteStore,aPos,aLength,FALSE);

	// Write the field set if any.
	TBool fieldSetPresent = FieldSetPresent();
	aStream.WriteUint8L(fieldSetPresent != EFalse);
	if (fieldSetPresent)
		iFieldSet->CopyToStreamL(aStream,aPos,aLength);
	}


 

EXPORT_C TInt CPlainText::PasteFromStoreL(const CStreamStore& aStore,const CStreamDictionary& aDictionary,TInt aPos)
/** Pastes plain text and fields, if present, from the clipboard into the current 
text object at the specified document position. The entire contents of the 
store are pasted.

@param aStore The steam store from which to paste the text. 
@param aDictionary The stream dictionary. 
@param aPos Document position at which to paste. Must be valid or the function 
raises a panic. 
@return The number of characters pasted. */
	{
	// Paste the lesser of aMaxPasteLength and the entire clipboard contents.
	// Return the number of characters pasted.
	TStreamId id=aDictionary.At(KClipboardUidTypePlainText);
	return DoPasteFromStoreL(aStore,id,aPos);
	}


TInt CPlainText::DoPasteFromStoreL(const CStreamStore& aStore,TStreamId aStreamId,TInt aPos)
	{
	if (aPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_DOPASTEFROMSTOREL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos<=DocumentLength(),Panic(ECharPosBeyondDocument));

	TInt charsPasted=0;
	if (aStreamId!=KNullStreamId)
		{// There is a recognised type in the clipboard.
		RStoreReadStream stream;
		stream.OpenLC(aStore,aStreamId);
		charsPasted=PasteFromStreamL(stream,aPos);
		CleanupStack::PopAndDestroy();
		//
		PasteComponentsL(aStore,aPos);
		SetHasChanged(ETrue);
		}

	__TEST_INVARIANT;
	return charsPasted;
	}


void CPlainText::PasteComponentsL(const CStreamStore& aStore,TInt aPos)
// Copy/Paste 2' RestoreComponentsL() - only if a field set is present.
//
	{
	if (FieldSetPresent())
		iFieldSet->PasteComponentsL(aStore,aPos);
	}


// Paste everything in the stream.
TInt CPlainText::PasteFromStreamL(RReadStream& aStream,TInt aPos)
	{
	TInt chars_read = 0;
	TInt error = KErrNone;

	TRAP(error, chars_read=CPlainText::DoPasteFromStreamL(aStream, aPos));

	UpdatePageTable(aPos,chars_read);

	/*
	If there was an exception delete any inserted text and propagate the exception.
	Not deleting the text would cause the size of the text to be inconsistent with the size
	implied elsewhere, such as in the formatting information stored in CRichText objects.
	*/
	if (error != KErrNone)
		{
		DoPtDelete(aPos,chars_read);
		OstTrace1( TRACE_FATAL, DUP1_CPLAINTEXT_PASTEFROMSTREAML, "Leave code=%d", error );
		User::Leave(error);
		}

	__TEST_INVARIANT;
	return chars_read;
	}


TInt CPlainText::DoPasteFromStreamL(RReadStream& aStream, TInt aPos)
	{
	TInt chars_read = 0;

	CBufSeg* buffer = CBufSeg::NewL(512);
	CleanupStack::PushL(buffer);
	TInt length = aStream.ReadInt32L();
	::InternalizeTextL(aStream,*buffer,length);

	/*
	Insert the text bit by bit so that memory consumed by the CPlainText object is freed from the buffer;
	this is important if pasting huge amounts of text.
	*/
	while (buffer->Size() > 0)
		{
		TPtr8 p8 = buffer->Ptr(0);
		TInt bytes = p8.Length();
		TInt chars = bytes / sizeof(TText);
		TPtrC p((TText*)p8.Ptr(),chars);	// platform dependency in the Unicode build; relies on little-endianness
		PtInsertL(aPos + chars_read,p);
		buffer->Delete(0,bytes);
		chars_read += chars;
		}

	CleanupStack::PopAndDestroy();	// buffer
	buffer = NULL;

	// If there's a field set, internalize it.
	if (aStream.ReadUint8L() != 0)	// next byte is non-zero if there's a field set
		{
		if (!FieldSetPresent())
			CreateFieldSetL(DocumentLength());
		iFieldSet->PasteFromStreamL(aStream,aPos,chars_read);
		}
	
	return chars_read;
	}


void CPlainText::InsertEodL()
/** Inserts the end-of-document character upon document construction.*/
	{
// ASSERT: The plain text component is empty.
	if (DocumentLength()!=-1)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CPLAINTEXT_INSERTEODL, "ECorruptTextStore" );
	    }
	__ASSERT_DEBUG(DocumentLength()==-1,Panic(ECorruptTextStore));
	TBuf<1> content;
	content.Append(EParagraphDelimiter);
	TPtrC eod(content);
	DoPtInsertL(0,eod);

	__TEST_INVARIANT;
	}

 
EXPORT_C void CPlainText::Reset()
/** Deletes all text content, formatting and fields from the document, leaving 
the single paragraph delimiter which terminates the text object. */
	{	
	// Resets document contents to a single end-of-document character, with no other content.
	// (No reset occurs if the component is already in the reset state.  Avoids an assertion
	// failure in the delete method, where length to delete !> 0).
	__TEST_INVARIANT;
		
	TInt content=DocumentLength();
	if (content>0)
		DoPtDelete(0,content);
	if (FieldSetPresent())
		KillFieldSet();
	SetHasChanged(ETrue);
	
	__TEST_INVARIANT;
	}


 

EXPORT_C TInt CPlainText::DocumentLength()const
/** Gets the the number of characters in the text object.

Note: the count includes all non-printing characters but excludes the end 
of text paragraph delimiter, so that the smallest possible return value is 
zero.

@return The number of characters in the text object. */
	{return ((iByteStore->Size()/sizeof(TText))-1);}


EXPORT_C void CPlainText::InsertL(TInt aInsertPos,const TChar& aChar)
/** Inserts either a single character or a descriptor into the text object
at a specified document position.

Updates the page table.

@param aPos The document position at which to insert the character/descriptor. 
Must be valid, or a panic occurs.
@param aChar The character to insert.
@param aBuf The descriptor to insert. */
	{
	__TEST_INVARIANT;
	if (aInsertPos<0 || aInsertPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_INSERTL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aInsertPos>=0 && aInsertPos<=DocumentLength(),Panic(ECharPosBeyondDocument));

	TInt contentLength = 1;
	if (aChar < 0x10000)
		{
		TBuf<1> content;
		content.Append(aChar);
		DoPtInsertL(aInsertPos,content);
		}
	else
		{
		TText16 high = TChar::GetHighSurrogate(aChar);
		TText16 low = TChar::GetLowSurrogate(aChar);
		RDebug::Print(_L("CPlainText::InsertL(%d), %X expand to %X %X."), aInsertPos, TUint(aChar), high, low);
	
		TBuf<2> content;
		contentLength = 2;
		content.Append(high);
		content.Append(low);
		DoPtInsertL(aInsertPos,content);
		}
	if (FieldSetPresent())
		iFieldSet->NotifyInsertion(aInsertPos,contentLength);

	SetHasChanged(ETrue);

	__TEST_INVARIANT;
	}


EXPORT_C void CPlainText::InsertL(TInt aPos,const TDesC& aBuf)
/** Inserts the contents of aBuf into the document at position aPos.*/	
	{
	PtInsertL(aPos,aBuf);
	SetHasChanged(ETrue);
	}


EXPORT_C void CPlainText::PtInsertL(TInt aPos,const TDesC& aBuf)
/** Inserts the contents a aBuf into the document at position aInsertPos.
Maintain field set.*/
	{
	__TEST_INVARIANT;
	if (aPos<0 || aPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_PTINSERTL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=DocumentLength(),Panic(ECharPosBeyondDocument));

	DoPtInsertL(aPos,aBuf);
	if (FieldSetPresent())
		iFieldSet->NotifyInsertion(aPos,aBuf.Length());

	__TEST_INVARIANT;
	}


EXPORT_C void CPlainText::DoPtInsertL(TInt aPos,const TDesC& aBuf)
/** Inserts the contents a aBuf into the document at position aInsertPos.
Maintain field set.*/
	{
	TPtrC8 buf((TUint8*)aBuf.Ptr(),aBuf.Size());
	iByteStore->InsertL(aPos*sizeof(TText),buf);
	UpdatePageTable(aPos,aBuf.Length());
	}


void CPlainText::InsertL(TInt aPos,const CPlainText* aText)
/** Insert the specified plain text object at the specified character position.
(Called by CRichText::Insert()*/
	{
	TInt lengthRemaining=aText->DocumentLength();
	TInt readPos=0;
	FOREVER
		{
		TPtrC view=aText->Read(readPos);
		TInt consumed=view.Length();
		if (consumed>lengthRemaining)
			consumed=lengthRemaining;
		InsertL(aPos,view);
		lengthRemaining-=consumed;
		if (lengthRemaining==0)
			return;
		aPos+=consumed;
		readPos+=consumed;
		}
	}


EXPORT_C TBool CPlainText::DeleteL(TInt aPos,TInt aLength)
/** Deletes one or more characters beginning at, and including, the character at 
a specified document position. Updates the page table. Any fields wholly contained 
in the range of characters to delete are removed from the field set.

@param aPos The document position from which to begin deleting. Must be valid 
or a panic occurs. 
@param aLength The number of characters to delete. Must be positive or a panic 
occurs. The sum of aPos and aLength must be less than the document length, 
or a panic occurs. 
@return Indicates whether two paragraphs have been merged together as a result 
of the delete, indicating that the resulting paragraph must be reformatted. 
Has no meaning for plain text, so always EFalse. */
	{
	return Delete(aPos,aLength);
	}


// A non-virtual non-leaving delete function for use inside ETEXT.
TBool CPlainText::Delete(TInt aPos,TInt aLength)
	{
	__TEST_INVARIANT;

	TBool ret = DoPtDelete(aPos,aLength);
	if (FieldSetPresent())
		iFieldSet->NotifyDeletion(aPos,aLength);
	SetHasChanged(ETrue);

	__TEST_INVARIANT;
	return ret;
	}


EXPORT_C TBool CPlainText::DoPtDelete(TInt aPos,TInt aLength)
/** Deletes aLength number of characters from the document,
 commencing at, and including, position aPos.
 The return value indicates if 2 paragraphs have been merged together
 as a result of the delete, indicating that the resulting paragraph
 must be reformatted.
 In global text, this clearly has no reasonable meaning, so always returns
 EFalse, so no reformatting occurs.*/
	{
	TInt documentLength=DocumentLength()+1;
	if (aPos<0 || aPos>documentLength)
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_DOPTDELETE, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=documentLength,Panic(ECharPosBeyondDocument));
	if (aLength<0)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CPLAINTEXT_DOPTDELETE, "EDeleteNegativeLength" );
	    }
	__ASSERT_ALWAYS(aLength>=0,Panic(EDeleteNegativeLength));
	if (aPos+aLength>documentLength)
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CPLAINTEXT_DOPTDELETE, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos+aLength<=documentLength,Panic(ECharPosBeyondDocument));

	iByteStore->Delete(aPos*sizeof(TText),aLength*sizeof(TText));
	UpdatePageTable(aPos,-aLength);

	return EFalse;
	}



EXPORT_C TInt CPlainText::ImportTextFileL(TInt aPos,const TDes& aFileName,TTextOrganisation aTextOrganisation)
/** Imports a plain text file into this text object.

Translates non-printing characters in the source text file into Symbian OS 
special characters, for instance tabs are converted into 
CEditableText::ETabCharacters, and form feeds into CEditableText::EPageBreaks. 
Line feeds in the source text file are translated according to the 
aTextOrganisation argument.

The function leaves if there is any problem in opening the file.

@param aPos Document position at which to insert the text. Must be a valid 
position, or a panic occurs.
@param aFileName The name of the text file to import.
@param aTextOrganisation If EOrganiseByLine, a single line feed or a line feed 
and carriage return is converted into a space character. A line feed which 
is followed by another line feed is converted into a paragraph delimiter. 
If EOrganiseByParagraph, all line feeds are converted into paragraph delimiters. 

@return The number of characters imported. */
	{
	TInt chars_inserted = 0;
	RFs file_session;
	TInt error = file_session.Connect();
	if (error == KErrNone)
		{
		RFile file;
		error = file.Open(file_session,aFileName,EFileStream | EFileRead | EFileShareReadersOnly);
		if (error == KErrNone)
			{
			RFileReadStream input_stream(file);
			TRAP(error,ImportTextL(aPos,input_stream,aTextOrganisation,KMaxTInt,KMaxTInt,&chars_inserted));
			input_stream.Close();
			}
		file.Close();
		file_session.Close();
		}
	if (error < 0)
	    {
	    OstTrace1( TRACE_FATAL, DUP1_CPLAINTEXT_IMPORTTEXTFILEL, "Leave code=%x", error );
	    }
	User::LeaveIfError(error);
	return chars_inserted;
	}



EXPORT_C void CPlainText::ExportAsTextL(const TDes& aFileName,TTextOrganisation aTextOrganisation,TInt aLineWrap)const
/** Writes the contents of the plain text object to a text file.
	
The filename is given by aFileName. Any existing file with that name is 
replaced. A wrap width can be specified. This is only used when exporting 
by line (aTextOrganisation is EOrganiseByLine).

The function leaves if there is any problem in creating or replacing the file.

@param aFileName The name of the file to export the text to. If a file with 
this name already exists, it is replaced. Otherwise, a new file is created.
@param aTextOrganisation Defines how to translate line delimiters. If 
EOrganiseByLine, lines wrap at the wrap width, as specified in aMaxLineLength. 
If EOrganiseByParagraph, lines do not wrap and paragraph delimiters are 
converted into CR/LF pairs.
@param aMaxLineLength The maximum number of characters in each line, (only 
relevant if the text organisation is EOrganiseByLine). */
	{
	if (aTextOrganisation != EOrganiseByParagraph && aLineWrap <= 0)
	    {
	    OstTrace0( TRACE_DUMP, CPLAINTEXT_EXPORTASTEXTL, "EExportLineWrapInvalid" );
	    }
	__ASSERT_DEBUG(aTextOrganisation == EOrganiseByParagraph || aLineWrap > 0,Panic(EExportLineWrapInvalid));
	RFs file_session;
	TInt error = file_session.Connect();
	if (error == KErrNone)
		{
		RFile file;
		error = file.Replace(file_session,aFileName,EFileStream | EFileWrite | EFileShareExclusive);
		if (error == KErrNone)
			{
			RFileWriteStream output_stream(file);
			TRAP(error,output_stream.WriteUint16L(EByteOrderMark));
			if (error == KErrNone)
				TRAP(error,ExportTextL(0,output_stream,aTextOrganisation,KMaxTInt,DocumentLength(),aLineWrap));
			output_stream.Close();
			}
		file.Close();
		file_session.Close();
		}
	if (error < 0)
	    {
	    OstTrace1( TRACE_FATAL, DUP2_CPLAINTEXT_EXPORTASTEXTL, "Leave code=%x", error );
	    }
	User::LeaveIfError(error);
	}



EXPORT_C void CPlainText::ImportTextL(TInt aPos,RReadStream& aInput,TTextOrganisation aTextOrganisation,
									  TInt aMaxOutputChars,TInt aMaxInputChars,
									  TInt* aOutputChars,TInt* aInputChars)
/** Imports plain text from a stream into this text object.

Translates line feeds in the source text according to the
aTextOrganisation argument.

@param aPos Document position at which to insert the text. Must be a valid 
position, or a panic occurs.
@param aInput Stream from which to read the text.
@param aTextOrganisation If EOrganiseByLine, a single line feed 
or a line feed and carriage return is converted into a space character. A 
line feed which is followed by another line feed is converted into a paragraph
delimiter. If EOrganiseByParagraph, all line feeds are converted 
into paragraph delimiters.
@param aMaxOutputChars The maximum number of characters to write to the text
object.
@param aMaxInputChars The maximum number of characters to read from the stream.
@param aOutputChars  On return, the number of characters written to the text 
object.
@param aInputChars  On return, the number of characters read from the stream. */
    {
	TImportExportParam param;
	param.iOrganisation = aTextOrganisation;
	param.iMaxOutputChars = aMaxOutputChars;
	param.iMaxInputChars = aMaxInputChars;
	TImportExportResult result;
	ImportTextL(aPos,aInput,param,result);
	if (aOutputChars)
		*aOutputChars = result.iOutputChars;
	if (aInputChars)
		*aInputChars = result.iInputChars;
	}


EXPORT_C void CPlainText::ExportTextL(TInt aPos,RWriteStream& aOutput,TTextOrganisation aTextOrganisation,
									  TInt aMaxOutputChars,TInt aMaxInputChars,TInt aMaxLineLength,
									  TInt* aOutputChars,TInt* aInputChars) const
/**  Writes plain text to a stream, optionally converting it from Unicode
into a foreign encoding.

@since 6.1
@param aPos A document position in the source plain text object from which 
to start reading the text to export.
@param aOutput  On return, the write stream to which the text is  written.
@param aParam  Export parameters, including an optional foreign encoding to 
convert the Unicode text into, a file  server connection, (this is needed for 
the character conversion - if not specified, one will be created), a line 
wrap width, and the maximum number of characters to export.
@param aResult  On return, contains the number of characters read and written. */
    {
	TImportExportParam param;
	param.iOrganisation = aTextOrganisation;
	param.iMaxOutputChars = aMaxOutputChars;
	param.iMaxInputChars = aMaxInputChars;
	param.iMaxLineLength = aMaxLineLength;
	TImportExportResult result;
	ExportTextL(aPos,aOutput,param,result);
	if (aOutputChars)
		*aOutputChars = result.iOutputChars;
	if (aInputChars)
		*aInputChars = result.iInputChars;
	}

EXPORT_C void CPlainText::ImportTextL(TInt aPos,RReadStream& aInput,
									  const TImportExportParam& aParam,TImportExportResult& aResult)
/** Imports plain text from a stream into this text object, optionally 
converting it from a foreign encoding into Unicode.

@param aPos Document position at which to insert the text. Must be a valid 
position, or a panic occurs.
@param aInput Stream from which to read the text.
@param aParam Import parameters, including the foreign encoding to convert 
from, whether to guess the foreign encoding and the maximum number of characters 
to import.
@param aResult On return, contains the number of characters read and written 
and the foreign encoding used by the imported text.
@see CPlainText::TImportExportParam
@see CPlainText::TImportExportResult */
	{
	CBufSeg* buffer = CBufSeg::NewL(512);
	CleanupStack::PushL(buffer);
	RBufWriteStream output(*buffer,0);
	TImportExportParam param = aParam;
	param.iOutputInternal = TRUE; // force output to internal format
	TPlainTextReader::TranslateL(param,aResult,output,aInput);

	TInt chars_inserted = 0;
	while (buffer->Size() > 0)
		{
		TPtr8 p8 = buffer->Ptr(0);
		TInt bytes = p8.Length();
		TInt chars = bytes / sizeof(TText);
		TPtrC p((TText*)p8.Ptr(),chars);
		/*
		Insert text using the virtual function InsertL to allow derived classes
		like CRichText to update their attributes.
		*/
		InsertL(aPos + chars_inserted,p);
		buffer->Delete(0,bytes);
		chars_inserted += chars;
		}

	CleanupStack::PopAndDestroy();	// buffer;
	}

EXPORT_C void CPlainText::ExportTextL(TInt aPos,RWriteStream& aOutput,
									  const TImportExportParam& aParam,TImportExportResult& aResult) const
/** Writes plain text to a stream, optionally converting it from Unicode into a 
foreign encoding.

@param aPos A document position in the source plain text object from which 
to start reading the text to export.
@param aOutput On return, the write stream to which the text is written.
@param aParam Export parameters, including an optional foreign encoding to 
convert the Unicode text into, a file server connection, (this is needed for 
the character conversion if not specified, one will be created), a line 
wrap width, and the maximum number of characters to export.
@param aResult On return, contains the number of characters read and written.
@see CPlainText::TImportExportParam
@see CPlainText::TImportExportResult */
	{
	TImportExportParam param = aParam;
	param.iInputInternal = TRUE; // force input to internal format
	param.iMaxInputChars = Min(param.iMaxInputChars,DocumentLength() - aPos); // ensure final paragraph delimiter is not exported
	RBufReadStream input(*iByteStore,aPos);
	TPlainTextWriter::TranslateL(param,aResult,aOutput,input);
	}


EXPORT_C TPtrC CPlainText::Read(TInt aStartPos)const
/** Gets a read-only view of a portion of the text object.

The extent of the view is the range of characters starting at aStartPos and ending at whichever of the
following document positions is reached first:

- The end of the document. 
- The end of the segment if using segmented storage (the storage type is specified in the NewL(). 

Therefore, when using a segmented buffer to store the document, the length of the resultant view may be
less than the requested length. In this case multiple calls to Read() may be necessary.

@param aStartPos The document position at which to begin reading. Must be valid or a panic occurs.
@return Constant pointer to a section of the text object. 
*/	
    {
	__TEST_INVARIANT;
	if (aStartPos<0 || aStartPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CPLAINTEXT_READ, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aStartPos>=0 && aStartPos<=DocumentLength(),Panic(ECharPosBeyondDocument));
	
	TPtr8 buf=iByteStore->Ptr(aStartPos*sizeof(TText));
	return TPtrC((TText*)buf.Ptr(),buf.Length()/sizeof(TText));
	}


EXPORT_C TPtrC CPlainText::Read(TInt aStartPos,TInt aLength)const
 /** Gets a read-only view of a portion of the text object.

The extent of the view is the range of characters starting at aStartPos and ending at whichever of the
following document positions is reached first:

- The end of the document.
- The end of the segment if using segmented storage (the storage type is specified in the NewL().
- The sum of aStartPos and (aLength-1).

Therefore, when using a segmented buffer to store the document, the length of the resultant view may be
less than the requested length. In this case multiple calls to Read() may be necessary.

@param aStartPos The document position at which to begin reading. Must be valid or a panic occurs. 
@param aLength The number of characters to read, inclusive of the character at position aStartPos. 
@return Constant pointer to a section of the text object.
*/
	{
	__TEST_INVARIANT;
	if (aStartPos<0 || aStartPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_READ, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aStartPos>=0 && aStartPos<=DocumentLength(),Panic(ECharPosBeyondDocument));
	
	TPtr8 buf=iByteStore->Ptr(aStartPos*sizeof(TText));
	TInt length=Min(aLength,((TInt)buf.Length()/sizeof(TText)));
	return TPtrC((TText*)buf.Ptr(),length);
	}


EXPORT_C void CPlainText::Extract(TDes& aBuf,TInt aPos)const
/**  Copies the contents of the text object into a descriptor overloaded
function. The function copies all characters from and including the document
position specified, to the end of the document or the end of the range
of characters, if specified.

The buffer's maximum length must be greater than or equal to the number 
of characters to extract, or a panic occurs.

@param aBuf A buffer; on return contains the extracted text.
@param aPos The document position from which to copy. Must be valid or a 
panic occurs.*/
    {
	__TEST_INVARIANT;
	TInt documentLength=DocumentLength();
	if (aPos<0 || aPos>documentLength)
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_EXTRACT, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=documentLength,Panic(ECharPosBeyondDocument));
	if (aBuf.MaxLength()<documentLength - aPos)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CPLAINTEXT_EXTRACT, "EExtractBufferTooSmall" );
	    }
	__ASSERT_ALWAYS(aBuf.MaxLength()>=documentLength - aPos,Panic(EExtractBufferTooSmall));

	DoExtract(aBuf,aPos,documentLength-aPos);
	}


EXPORT_C void CPlainText::Extract(TDes& aBuf,TInt aPos,TInt aLength)const
/** Copies the contents of the text object into a descriptor-overloaded function. 
The function copies all characters from and including the document position 
specified, to the end of the document or the end of the range of characters, 
if specified.

The buffer's maximum length must be greater than or equal to aLength, or a 
panic ocurs.

@param aBuf A buffer; on return contains the extracted text. 
@param aPos The document position from which to copy. Must be valid or a panic 
occurs. 
@param aLength The number of characters to copy. */
	{
	__TEST_INVARIANT;
	if (aPos<0 || aPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CPLAINTEXT_EXTRACT, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=DocumentLength(),Panic(ECharPosBeyondDocument));
	if (aBuf.MaxLength()<aLength)
	    {
	    OstTrace0( TRACE_FATAL, DUP3_CPLAINTEXT_EXTRACT, "EExtractBufferTooSmall" );
	    }
	__ASSERT_ALWAYS(aBuf.MaxLength()>=aLength,Panic(EExtractBufferTooSmall));

	DoExtract(aBuf,aPos,aLength);
	}

// Extract text, optionally discarding some characters such as control characters and soft hyphens or
// inline text, depending on the flag.
EXPORT_C void CPlainText::ExtractSelectively(TDes& aBuf,TInt aPos,TInt aLength,TUint aFlags)
	{
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_EXTRACTSELECTIVELY, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(),Panic(ECharPosBeyondDocument));
	DoExtract(aBuf,aPos,aLength,aFlags);
	}


void CPlainText::DoExtract(TDes& aBuf,TInt aPos,TInt aLength,TUint aFlags) const
/** Copies aLength characters, commencing at aPos, into the specified
buffer aBuf.  If aLength is greater than the amount of data left,
then all remaining characters are written.*/
	{
	aBuf.SetLength(0);
	TInt remainingLength=Min(aLength,DocumentLength()-aPos);
	TInt startPos=aPos;
	FOREVER
		{
		TPtrC textRead=Read(startPos);
		TInt lengthRead=textRead.Length();
		if (lengthRead>remainingLength)
			lengthRead=remainingLength;
		
		// Remove specific characters
		if (aFlags & EExtractVisible)
			{
			const TText* p = textRead.Ptr();
			const TText* q = p + lengthRead;
			while (p < q)
				{
				TChar c(*p++);
				if (c == EParagraphDelimiter || c == ELineBreak || c == ETabCharacter)
					aBuf.Append(' ');
				else if (c != EPotentialHyphen && c.IsPrint())
					aBuf.Append(c);
				}
			}
			
		// Remove inline text from the specified section
		else if (aFlags & EExcludeInlineEditedText)
			{
			const TInt inlineTextPos = GetPositionOfInlineTextInDocument();
			const TText* p = textRead.Ptr();
			
			if (inlineTextPos != KErrNotFound)
				{
				for (TInt i=aPos; i<(aPos+lengthRead); i++)
					{
		    		if (!((i >= inlineTextPos) && (i <= (inlineTextPos + GetLengthOfInlineText() - 1))))
		    			{
						TChar c(*p++);
						aBuf.Append(c);
						continue;
		    			}
		    		++p;
					}
				}
			else
				aBuf.Append(textRead.Ptr(),lengthRead);
			}
		else
			aBuf.Append(textRead.Ptr(),lengthRead);
		remainingLength-=lengthRead;
		if (remainingLength==0)
			return;
		startPos+=lengthRead;
		}
	}



EXPORT_C void CPlainText::SetPageTable(TPageTable* aPageTable)
/** Links the text object to a page table. A page table is an array of integers; 
each integer represents the number of characters on a page. It is required 
for pagination. The page table is updated when changes are made to the document, 
e.g. after pasting from the clipboard, inserting, importing or deleting text.

The text object does not take ownership of the page table specified.

@param aPageTable The page table to be referenced by the text object. */
	{
	iPageTable=aPageTable;
	SetHasChanged(ETrue);
	}


void CPlainText::UpdatePageTable(TInt aPos,TInt aLength)
// Adds aLength number of characters to the page in the page table
// specified by the character position aPos.
// Called from i) Insert  ii) Delete  iii) Paste from clipboard  iv) Text file import.
//
	{
	__TEST_INVARIANT;
	if (aPos<0 || aPos>DocumentLength()+1)
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_UPDATEPAGETABLE, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=DocumentLength()+1,Panic(ECharPosBeyondDocument));
	
	if (iPageTable)
		(*iPageTable)[PageContainingPos(aPos)]+=aLength;
	}


EXPORT_C TInt CPlainText::PageContainingPos(TInt aPos)const
/** Gets the number of the page which contains the specified document position. 
If no page table has been set up, the function returns a value of zero. Use 
SetPageTable() to set up the page table.

@param aPos A document position. Must be valid or a panic occurs. 
@return The page number containing document position aPos. */
    {
	__TEST_INVARIANT;
	if (aPos<0 || aPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_PAGECONTAININGPOS, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=DocumentLength(),Panic(ECharPosBeyondDocument));

    if (!iPageTable || (iPageTable->Count()<1))
        return 0;
    else
        {
        TInt pageOffset=0;
        TInt charCount=(*iPageTable)[pageOffset];
        while (charCount<=aPos)
            {
            pageOffset++;
            charCount+=(*iPageTable)[pageOffset];
            }
        return pageOffset;
        }
    }

EXPORT_C TEtextComponentInfo CPlainText::ComponentInfo()const
/** Gets information about the number of components contained in the text object. 
For plain text, only the field count has relevance.

@return Contains the field count. */
	{return TEtextComponentInfo(FieldCount(),0,0);}



EXPORT_C TInt CPlainText::FieldCount() const
/** Gets a count of the number of fields in the text object's field set.

@return The number of fields in the field set. */
	{
	__TEST_INVARIANT;

	return (FieldSetPresent())
		? iFieldSet->FieldCount()
		: 0;
	}

EXPORT_C void CPlainText::InsertFieldL(TInt aPos,CTextField* aField,TUid aFieldType)
/** Inserts a field into the text object at a specified document position. creating a zero-length field record.
 Maintain the field set.

Note: After insertion, the field should be evaluated in order to make its contents 
visible; use UpdateFieldL().

@param aPos The document position at which to insert the field. Must be valid, 
or a panic occurs. 
@param aField The field to insert, created by NewTextFieldL(). Must not be 
NULL, or a panic occurs. 
@param aFieldType Identifies the type of field to insert. For the built in 
field types, see the UID values defined in flddef.h. */
	{
	__TEST_INVARIANT;
	if (aPos<0 || aPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_INSERTFIELDL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=DocumentLength(),Panic(ECharPosBeyondDocument));
	if (!aField)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CPLAINTEXT_INSERTFIELDL, "ENoTextField" );
	    }
	__ASSERT_ALWAYS(aField,Panic(ENoTextField));

	if (!FieldSetPresent())
		CreateFieldSetL(DocumentLength());
	iFieldSet->InsertFieldL(aPos,aField,aFieldType);
	SetHasChanged(ETrue);

	__TEST_INVARIANT;
	}

EXPORT_C void CPlainText::UpdateFieldL(TInt aPos)
/** Re-evaluates the field which covers the document position specified. Re-evaluating 
a field means calculating the field's new value, then inserting that value 
into the text object, replacing the previous value.

Notes:

fields have a maximum length of 20 characters.

the first time a field is updated, the position specified should be the position 
at which the field was inserted.

@param aPos A document position in the field to be updated. Must be a valid 
position, or a panic occurs. */
	{
	__TEST_INVARIANT;
	if (aPos<0 || aPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_UPDATEFIELDL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=DocumentLength(),Panic(ECharPosBeyondDocument));

	if (!FieldSetPresent())
		return;
	TFindFieldInfo info;
	if (iFieldSet->FindFields(info,aPos))
		{// a field exists at aPos, so update it.
		HBufC* valueBuf=HBufC::NewL(KMaxFieldBufferSize); // will hold the new value,max length 20
		/*
		Don't put valueBuf on the cleanup stack before calling NewFieldValueL because NewFieldValueL
		sometimes changes valueBuf, in which case it itself puts it on the cleanup stack.
		*/
		iFieldSet->NewFieldValueL(valueBuf,info.iFirstFieldPos);  // get the new value
		CleanupStack::PushL(valueBuf);
		DoPtInsertL(info.iFirstFieldPos,*valueBuf);  // insert the new text into the document
		DoPtDelete(info.iFirstFieldPos+valueBuf->Length(),info.iFirstFieldLen);  // delete the old text of the field
		iFieldSet->NotifyFieldUpdate(aPos,valueBuf->Length());  // inform the field set
		CleanupStack::PopAndDestroy();
		}
	SetHasChanged(ETrue);

	__TEST_INVARIANT;
	}



EXPORT_C void CPlainText::UpdateAllFieldsL()
/** Re-evaluates all of the fields in the text object. Re-evaluating a field means 
calculating the field's new value, then inserting that value into the text 
object, replacing the previous value.

Note: Fields have a maximum length of 20 characters. */
	{
	__TEST_INVARIANT;

	TInt numFields=FieldCount();
	TInt pos=0;
	TFindFieldInfo info;
	for (TInt item=0;item<numFields;item++)
		{
		// find the next field and go to its beginning
		iFieldSet->FindFields(info,pos,DocumentLength()-pos);
		pos=info.iFirstFieldPos;
		UpdateFieldL(pos);
		// the field has changed, find out its new length and go to the end of it
		iFieldSet->FindFields(info,pos);
		pos+=info.iFirstFieldLen;
		}

	__TEST_INVARIANT;
	}



EXPORT_C TBool CPlainText::RemoveField(TInt aPos)
/** Removes the field covering the document position specified from the field set, 
and deletes all text content associated with the field from the text object.

@param aPos A document position within the field to be deleted. Must be a 
valid position or a panic occurs. 
@return ETrue if a field was located at aPos. EFalse if no field was located 
at aPos. */
	{
	__TEST_INVARIANT;
	if (aPos<0 || aPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_REMOVEFIELD, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=DocumentLength(),Panic(ECharPosBeyondDocument));

	TBool fieldRemoved=EFalse;
	
	if (FieldSetPresent())
		{
		TFindFieldInfo info;
		if (iFieldSet->FindFields(info,aPos))
			{// remove the field's record & associated text
			Delete(info.iFirstFieldPos,info.iFirstFieldLen);
			iFieldSet->RemoveField(aPos);
			fieldRemoved=ETrue;
			}
		}
	if (fieldRemoved)
		SetHasChanged(ETrue);
	
	__TEST_INVARIANT;
	return fieldRemoved;
	}



EXPORT_C TBool CPlainText::FindFields(TInt aPos) const
	{
	__TEST_INVARIANT;

	if (FieldSetPresent())
		return iFieldSet->FindFields(aPos);
	else
		return EFalse;
	}


EXPORT_C TBool CPlainText::FindFields(TFindFieldInfo& aInfo,TInt aPos,TInt aRange)const
/** Tests whether a document position is located within a field overloaded
function.

The second overload extracts information about all fields located
within a range of characters.

@param aPos The document position. Must be valid or a panic occurs.
@param aInfo  On return, contains the number of fields fully or partially 
within the range of characters specified and the start position and length 
of the first field found.
@param   aRange The number of characters to search, beginning at aPos. Must 
not be negative, or a panic occurs. The sum of aPos and aRange must be less 
than the document length, or a panic occurs. The default range is zero. In 
this case the function just returns information about the single field located 
at the position specified.
@return ETrue if aPos is located within a field, or if there were one or more 
fields found in the range. EFalse if not. */
	{
	__TEST_INVARIANT;

	if (FieldSetPresent())
		return iFieldSet->FindFields(aInfo,aPos,aRange);
	else
		return EFalse;
	}

EXPORT_C const CTextField* CPlainText::TextField(TInt aPos)const
/** Returns a pointer to the field located at the specified document position.

@param aPos A document position within the field. Must be a valid document 
position, or a panic occurs. 
@return Pointer to the field which covers position aPos. NULL if there is no 
field at the specified position. */
	{
	__TEST_INVARIANT;

	return (FieldSetPresent())
		? iFieldSet->TextField(aPos)
		: NULL;
	}

EXPORT_C TBool CPlainText::ConvertFieldToText(TInt aPos)
/** Converts the field containing the specified document position into text, leaving 
its current value in the text object. Does not update the field beforehand.

@param aPos A document position within the field to convert. Must be a valid 
position or a panic occurs. 
@return ETrue if there was a field located at aPos. EFalse if there was no 
field located at aPos. */
	{
	__TEST_INVARIANT;
	if (aPos<0 || aPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_CONVERTFIELDTOTEXT, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=DocumentLength(),Panic(ECharPosBeyondDocument));

	TBool fieldConverted=EFalse;
	
	if (FieldSetPresent())
		{
		TFindFieldInfo info;
		iFieldSet->FindFields(info,aPos);
		if (iFieldSet->RemoveField(aPos))
			fieldConverted=ETrue;
		}
	if (fieldConverted)
		SetHasChanged(ETrue);
	
	__TEST_INVARIANT;
	return fieldConverted;
	}


EXPORT_C void CPlainText::ConvertAllFieldsToText()
/** Removes all fields from the text object's field set, leaving their current 
text value in the text object. Does not update the fields beforehand. */
	{
	__TEST_INVARIANT;

	TInt fieldCount=FieldCount();
	TInt pos=0;
	for (TInt item=0;item<fieldCount;item++)
		{// Convert each field in turn.
		TFindFieldInfo info;
		iFieldSet->FindFields(info,pos);  // find the next field and go to its beginning
		pos=info.iFirstFieldPos;
		ConvertFieldToText(pos);
		}

	__TEST_INVARIANT;
	}

EXPORT_C void CPlainText::SetFieldFactory(MTextFieldFactory* aFactory)
/** Sets up a field factory. A field factory is an instance of a class deriving 
from MTextFieldFactory. It must implement a NewFieldL() function to create 
and return fields of the desired type. The field factory's NewFieldL() function 
is called by CPlainText::NewTextFieldL(). A field factory must be set up if 
you intend to add any fields to the text object.

@param aFactory The field factory. */
	{
	// Set the FieldSet's header factory handle (this points to the built-in factory by default)
	//+ This function has no way to report failure, which can happen if CreateFieldSetL leaves. It ought to leave.
	
	// Create field set if necessary & set the factory
	int error = 0;
	if (!FieldSetPresent())
		TRAP(error,CreateFieldSetL(DocumentLength()));
	if (!error)
		iFieldSet->SetFieldFactory(aFactory);
	}



EXPORT_C CTextField* CPlainText::NewTextFieldL(TUid aFieldType)const
/** Creates and returns a new field. Before calling this function, a field 
factory should have been set up, either by calling SetFieldFactory(), or by 
specifying one in the NewL(). The field factory's NewFieldL() function is 
called to create a field of the type specified in the argument. A NULL field 
is returned if no field factory has been set up.

@param aFieldType Identifies the field type. 
@return Pointer to the new text field. NULL if no field factory has been set 
up. */
	{
	__TEST_INVARIANT;

	CTextField* field=NULL;
	if (FieldSetPresent())
		field=iFieldSet->NewFieldL(aFieldType);

	__TEST_INVARIANT;
	return field;
	}

void CPlainText::CreateFieldSetL(TInt aDocumentLength)
	{
	iFieldSet = CTextFieldSet::NewL(aDocumentLength);
	}

EXPORT_C const MTextFieldFactory* CPlainText::FieldFactory()const
/** Gets a pointer to the field factory used by the text object. The field factory 
may be set up using SetFieldFactory(), or may be specified in the NewL().

@return The field factory. NULL if no field factory has been set up. */
	{return (FieldSetPresent()) ? iFieldSet->FieldFactory() : NULL;}

CEditableTextOptionalData::CEditableTextOptionalData()
	{
	// do nothing
	}

CEditableTextOptionalData::~CEditableTextOptionalData()
	{
	delete iInlineEditData;
	}
