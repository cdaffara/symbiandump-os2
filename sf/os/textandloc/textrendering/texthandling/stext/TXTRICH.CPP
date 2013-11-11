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

#include "TXTRICH.H"
#include "TXTINDEX.H"
#include "TXTSTD.H"
#include "TXTRTPFL.H"
#include "ParseLst.h"
#include "TXTCLIPBOARD.H"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TXTRICHTraces.h"
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TXTETEXT_INTERNAL.H"
#include "TXTRICH_INTERNAL.H"
#endif

EXPORT_C void CRichText::__DbgTestInvariant()const
// Provides class invariants.  Explanations below:
//
	{
#ifdef _DEBUG
// ASSERT: The global format layers are never null.
	if (iGlobalParaFormatLayer == NULL)
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXT_DBGTESTINVARIANT, "Invariant" );
	    }
	__ASSERT_DEBUG(iGlobalParaFormatLayer != NULL, User::Invariant());
	if (iGlobalCharFormatLayer == NULL)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CRICHTEXT_DBGTESTINVARIANT, "Invariant" );
	    }
	__ASSERT_DEBUG(iGlobalCharFormatLayer != NULL, User::Invariant());
	if (IndexPresent())
		{
// ASSERT: The sum of para lengths == the length as described by the document storage.
		TInt cumulativeParaLength = 0;
		TInt maxPara = iIndex->iParaIx->Count();
		for (TInt offset = 0; offset < maxPara; offset++)
			{
			TParaAttribsEntry entry = (*iIndex->iParaIx)[offset];
			cumulativeParaLength += entry.iLength;
			}
		if (cumulativeParaLength != (DocumentLength() + 1))
		    {
		    OstTrace0( TRACE_DUMP, DUP2_CRICHTEXT_DBGTESTINVARIANT, "Invariant" );
		    }
		__ASSERT_DEBUG(cumulativeParaLength == (DocumentLength() + 1), User::Invariant());
		}
		// Change here for defect INC005336.
		// This defect is present when the assertion below fails.
        if ((iParserData != NULL) &&
                (iParserData->HaveRange()) &&
                (iParserData->EndParse() > DocumentLength()))
            {
            OstTrace0( TRACE_DUMP, DUP3_CRICHTEXT_DBGTESTINVARIANT, "Invariant" );
            }
		__ASSERT_DEBUG( (iParserData == NULL) || \
			(!iParserData->HaveRange()) || \
			(iParserData->EndParse() <= DocumentLength()), User::Invariant());
#endif
	}

EXPORT_C CRichText* CRichText::NewL(const CParaFormatLayer* aGlobalParaLayer,const CCharFormatLayer* aGlobalCharLayer,
									TDocumentStorage aStorage,TInt aDefaultTextGranularity,TParaType aParaType)
/** Allocates and constructs an empty rich text object, with a global character
and paragraph format layer. A single end-of-document delimiter is inserted.
No style list is allocated.

@param aGlobalParaLayer Pointer to the paragraph format layer referenced by
the rich text object. Must not be NULL, or a panic occurs.
@param aGlobalCharLayer Pointer to the character format layer referenced by
the rich text object. Must not be NULL, or a panic occurs.
@param aStorage The type of in-memory buffer to use. Defaults to ESegmentedStorage
which should rarely need to be changed.
@param aDefaultTextGranularity Specifies the granularity of the in-memory buffer.
Default is EDefaultTextGranularity bytes (=256), and this should rarely need
to be changed.
@param aParaType This argument indicates whether you are using a single paragraph
or multiple paragraphs, and thus affects the granularity of aggregate objects
used internally for storing paragraph attributes. Default = EMultiPara.
@return The rich text object. */
	{
	// Create new rich text containing just a single end-of-document character.
	if (aGlobalParaLayer == NULL)
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_NEWL, "ENullFormatLayerHandle" );
	    }
	__ASSERT_ALWAYS(aGlobalParaLayer != NULL, Panic(ENullFormatLayerHandle));
	if (aGlobalCharLayer == NULL)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_NEWL, "ENullFormatLayerHandle" );
	    }
	__ASSERT_ALWAYS(aGlobalCharLayer != NULL, Panic(ENullFormatLayerHandle));

	CRichText* self = new(ELeave) CRichText(aGlobalParaLayer, aGlobalCharLayer);
	CleanupStack::PushL(self);
	self->ConstructL(aStorage, aDefaultTextGranularity, aParaType);
	CleanupStack::Pop();
	return self;
	}


// Create new rich text that supports Paragraph Styles, containing just a single end-of-document character.
EXPORT_C CRichText* CRichText::NewL(const CParaFormatLayer* aGlobalParaLayer,const CCharFormatLayer* aGlobalCharLayer,
									const CStyleList& aStyleList,
									TDocumentStorage aStorage,TInt aDefaultTextGranularity,TParaType aParaType)
/** Allocates and constructs an empty rich text object which supports styles. It
is constructed with a global character and paragraph format layer and a style
list. A single end-of-document delimiter is inserted. The rich text object
takes ownership of the style list.

Note:

A rich text object not constructed with a style list may still use styles,
by calling SetStyleListExternallyOwned() at any time after construction. In
this case, the rich text object does not own the style list.

@param aGlobalParaLayer Pointer to the paragraph format layer referenced by
the rich text object. Must not be NULL, or a panic occurs.
@param aGlobalCharLayer Pointer to the character format layer referenced by
the rich text object. Must not be NULL, or a panic occurs.
@param aStyleList Style list. Holds the set of paragraph styles which can be
used in the rich text object.
@param aStorage The type of in-memory buffer to use. Defaults to ESegmentedStorage
which should rarely need to be changed.
@param aDefaultTextGranularity Specifies the granularity of the in-memory buffer.
Default is EDefaultTextGranularity bytes (=256), and this should rarely need
to be changed.
@param aParaType This argument indicates whether you are using a single paragraph
or multiple paragraphs, and thus affects the granularity of aggregate objects
used internally for storing paragraph attributes. Default = EMultiPara.
@return The new rich text object. */
	{
	if (aGlobalParaLayer == NULL)
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXT_NEWL, "ENullFormatLayerHandle" );
	    }
	__ASSERT_ALWAYS(aGlobalParaLayer != NULL, Panic(ENullFormatLayerHandle));
	if (aGlobalCharLayer == NULL)
	    {
	    OstTrace0( TRACE_FATAL, DUP3_CRICHTEXT_NEWL, "ENullFormatLayerHandle" );
	    }
	__ASSERT_ALWAYS(aGlobalCharLayer != NULL, Panic(ENullFormatLayerHandle));

	CRichText* self = new(ELeave) CRichText(aGlobalParaLayer, aGlobalCharLayer, CONST_CAST(CStyleList*, &aStyleList));
	CleanupStack::PushL(self);
	self->ConstructL(aStorage, aDefaultTextGranularity, aParaType);
	CleanupStack::Pop();
	return self;
	}


// Restore into a new rich text object, using the specified global layers.
EXPORT_C CRichText* CRichText::NewL(const CStreamStore& aStore,TStreamId aStreamId,
									const CParaFormatLayer* aGlobalParaLayer,const CCharFormatLayer* aGlobalCharLayer,
									MTextFieldFactory* aFieldFactory,
									TDocumentStorage aStorage)
/** Allocates and constructs a rich text object with a field factory. Its text
content is internalized from a stream store.

Note:

A rich text object not constructed with a field factory may still support
the addition of fields, by calling SetFieldFactory(), defined in the base
class CPlainText.

@param aStore Stream store from which the object is restored.
@param aStreamId ID of the stream store.
@param aGlobalParaLayer Pointer to the paragraph format layer referenced by
the rich text object. Must not be NULL, or a panic occurs.
@param aGlobalCharLayer Pointer to the character format layer referenced by
the rich text object. Must not be NULL, or a panic occurs.
@param aFieldFactory Pointer to a field factory. A field factory must be provided
if the text object supports fields.
@param aStorage The type of in-memory buffer to use. Defaults to ESegmentedStorage
which should rarely need to be changed.
@return The new rich text object. */
	{
	if (aGlobalParaLayer == NULL)
	    {
	    OstTrace0( TRACE_FATAL, DUP4_CRICHTEXT_NEWL, "ENullFormatLayerHandle" );
	    }
	__ASSERT_ALWAYS(aGlobalParaLayer != NULL, Panic(ENullFormatLayerHandle));
	if (aGlobalCharLayer == NULL)
	    {
	    OstTrace0( TRACE_FATAL, DUP5_CRICHTEXT_NEWL, "ENullFormatLayerHandle" );
	    }
	__ASSERT_ALWAYS(aGlobalCharLayer != NULL, Panic(ENullFormatLayerHandle));

	CRichText* self = new(ELeave) CRichText(aGlobalParaLayer, aGlobalCharLayer);
	CleanupStack::PushL(self);
	self->ConstructL(aStore, aStreamId, NULL, NULL, aFieldFactory, aStorage);
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CRichText* CRichText::NewL(const CStreamStore& aStore, TStreamId aStreamId,
									const CParaFormatLayer* aGlobalParaLayer, const CCharFormatLayer* aGlobalCharLayer,
									MPictureFactory* aPictureFactory, MRichTextStoreResolver* aStoreResolver,
									MTextFieldFactory* aFieldFactory,
									TDocumentStorage aStorage)
/** Allocates and constructs a rich text object with a field factory and a picture
factory. Its text content is internalized from a stream store.

Note:

A rich text object not constructed with a field factory may still support
the addition of fields, by calling SetFieldFactory(), defined in the base
class CPlainText.

@param aStore Stream store from which the object is restored.
@param aStreamId ID of the stream store.
@param aGlobalParaLayer Pointer to the paragraph format layer referenced by
the rich text object. Must not be NULL, or a panic occurs.
@param aGlobalCharLayer Pointer to the character format layer referenced by
the rich text object. Must not be NULL, or a panic occurs.
@param aPictureFactory The picture factory. This is needed to load pictures
into memory, (see PictureHandleL()). If a store resolver is specified (not
NULL), then a factory must also be specified, or a panic occurs.
@param aStoreResolver A store resolver. This determines which file store the
picture is stored in. It is used to get from a reference to an embedded picture
within a CRichText object to the actual picture itself. Picture loading is
done by the picture factory.
@param aFieldFactory Pointer to a field factory. A field factory must be provided
if the text object supports fields.
@param aStorage The type of in-memory buffer to use. Defaults to ESegmentedStorage
which should rarely need to be changed.
@return The new rich text object. */
	{
	// Restore a new rich text from the specified stream, that uses the specified global layers, and the
	// specified picture header factory and store, if this rich text supports pictures.
	if (!aPictureFactory && aStoreResolver)
	    {
	    OstTrace0( TRACE_FATAL, DUP6_CRICHTEXT_NEWL, "EInvalidPictureFactorySettings" );
	    }
	__ASSERT_ALWAYS(!(!aPictureFactory && aStoreResolver), Panic(EInvalidPictureFactorySettings));
	if (aGlobalParaLayer == NULL)
	    {
	    OstTrace0( TRACE_FATAL, DUP7_CRICHTEXT_NEWL, "ENullFormatLayerHandle" );
	    }
	__ASSERT_ALWAYS(aGlobalParaLayer != NULL, Panic(ENullFormatLayerHandle));
	if (aGlobalCharLayer == NULL)
	    {
	    OstTrace0( TRACE_FATAL, DUP8_CRICHTEXT_NEWL, "ENullFormatLayerHandle" );
	    }
	__ASSERT_ALWAYS(aGlobalCharLayer != NULL, Panic(ENullFormatLayerHandle));

	CRichText* self = new(ELeave) CRichText(aGlobalParaLayer, aGlobalCharLayer);
	CleanupStack::PushL(self);
	self->ConstructL(aStore, aStreamId, aPictureFactory, aStoreResolver, aFieldFactory, aStorage);
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CRichText::CRichText(const CParaFormatLayer* aGlobalParaLayer, const CCharFormatLayer* aGlobalCharLayer,
							  CStyleList* aStyleList):
	CGlobalText(aGlobalParaLayer, aGlobalCharLayer),
	iStyleList(aStyleList)
	{
	}


EXPORT_C void CRichText::ConstructL(TDocumentStorage aStorage, TInt aDefaultTextGranularity, TParaType aParaType)
// Initialises and updates the index following the CPlainText::ConstructL
// insertion of the end-of-document character.
//
	{
	CPlainText::ConstructL(aStorage, aDefaultTextGranularity);
	SetParaTypeIsSingle(aParaType == ESinglePara);
	iParserData = new(ELeave) CParserData(DocumentLength());
	TInt a;
	TInt b;
	ParseText(a, b, ETrue);

	__TEST_INVARIANT;
	}


EXPORT_C void CRichText::ConstructL(const CStreamStore& aStore,TStreamId aStreamId,
									MPictureFactory* aPictureFactory, MRichTextStoreResolver* aStoreResolver,
									MTextFieldFactory* aFieldFactory,
									TDocumentStorage aStorage)
// Initialises and updates the index following the CPlainText::ConstructL
// insertion of the end-of-document character.
// Sets the picture header factory if provided.
//
	{
	CPlainText::ConstructL(aStore, aStreamId, aFieldFactory, aStorage);
	SetPictureFactory(aPictureFactory, aStoreResolver);
	if (iParserData == NULL)
		iParserData = new(ELeave) CParserData(DocumentLength());
	TInt a;
	TInt b;
	ParseText(a, b, ETrue);

	__TEST_INVARIANT;
	}


EXPORT_C CRichText::~CRichText()
/** The destructor frees all resources owned by the rich text object, prior to
its destruction. */
	{
	// We cannot call DestroyParserSystem() here because it applies to all instances in the thread
	delete iParserData;
	KillStyleList();
	KillIndex();
	}


void CRichText::KillStyleList()
// Free up the style table
//
	{
	if (StyleListPresent() && !StyleListExternallyOwned())
		{
		delete iStyleList.AsPtr();
		iStyleList = NULL;
		}
	}


void CRichText::KillIndex()
// Delete the rich text index if it's resident in memory.
//
	{
	if (IndexPresent())
		delete iIndex.AsPtr();
	iIndex=NULL;
	}


TBool CRichText::CreateEmptyMarkupComponentL()
// If necessary, creates an empty markup component.
// Returns ETrue if the markup component was created as a result of this function,
// otherwise returns EFalse.
//
	{
	if (IndexPresent())
		return EFalse;
	TInt paraGran = (ParaTypeIsSingle()) ? KSingleParaGranularity : KMultiParaGranularity;
	TInt phrGran = (ParaTypeIsSingle()) ? KSmallPhraseGranularity : KLargePhraseGranularity;
	iIndex = CRichTextIndex::NewL(iGlobalParaFormatLayer, iGlobalCharFormatLayer, *this,paraGran,phrGran);
	return ETrue;
	}


void CRichText::CreateAndGenerateMarkupComponentL()
// Checks if the rich text index needs to be created and does so if necessary.
// Called by all public rich text functions that manipulate specific formatting.
//
	{
	if (CreateEmptyMarkupComponentL())
		{
		TRAPD(ret, GenerateMarkupL());
		if (ret != KErrNone)
			{// destroy this partially set markup component
			delete iIndex.AsPtr();
			iIndex = NULL;
			User::Leave(ret);
			}
		}
	}


void CRichText::GenerateMarkupL()
// Generate markup data corresponding to the current text content.
//
	{
	TInt remainingLength = DocumentLength();
	TInt startPos = 0;
	while (remainingLength)
		{
		TPtrC buf = Read(startPos);
		TInt consumed = buf.Length();
		if (consumed > remainingLength)
			{
			consumed = remainingLength;
			buf.Set(buf.Ptr(), consumed);
			}
		iIndex->InsertL(startPos, buf, *iGlobalParaFormatLayer);
		remainingLength -= consumed;
		startPos += consumed;
		}
	}

EXPORT_C void CRichText::CopyToStoreL(CStreamStore& aStore,CStreamDictionary& aDictionary,TInt aPos,TInt aLength) const
/** Copies a portion of the rich text object, with components to the clipboard.

A panic occurs in the following circumstances:

aPos is an invalid document position

aLength is invalid (zero or less)

the sum of aPos and aLength is greater than or equal to the number of characters
in the document

@param aStore Stream store to which the rich text is written.
@param aDictionary The stream dictionary.
@param aPos The document position from which to begin copying.
@param aLength The number of characters to copy. */
	{
	if (aLength > 0)
		{
		TStreamId plainTextId = CPlainText::DoCopyToStoreL(aStore,aDictionary,aPos,aLength);
		TStreamId id = DoCopyToStoreL(aStore,aPos,aLength,plainTextId,FALSE);
		aDictionary.AssignL(KClipboardUidTypeRichText,id);
		TStreamId idStyles = DoCopyToStoreL(aStore,aPos,aLength,plainTextId,TRUE);
		aDictionary.AssignL(KClipboardUidTypeRichTextWithStyles,idStyles);
		}
	}


// Copy the selected region of rich text, with components, to the specified store.
TStreamId CRichText::DoCopyToStoreL(CStreamStore& aStore,TInt aPos,TInt aLength,TStreamId aPlainTextId,TBool aCopyStyles) const
	{
	__TEST_INVARIANT;

	TInt documentLength = DocumentLength();
	if (aPos < 0 || aPos > documentLength)
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_DOCOPYTOSTOREL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= documentLength,Panic(ECharPosBeyondDocument));
	if (aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_DOCOPYTOSTOREL, "ECopyToClipboardNegativeLength" );
	    }
	__ASSERT_ALWAYS(aLength >= 0,Panic(ECopyToClipboardNegativeLength));
	if (aPos + aLength > documentLength)
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXT_DOCOPYTOSTOREL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + aLength <= documentLength,Panic(ECharPosBeyondDocument));

	if (aLength == 0)
		return KNullStreamId;

	CStoreMap* map = CStoreMap::NewLC(aStore);
	CopyComponentsL(aStore,*map,aPos,aLength,aPlainTextId);
	RStoreWriteStream stream(*map);
	TStreamId id = stream.CreateLC(aStore);
	CopyToStreamL(stream,aPos,aLength,aPlainTextId,aCopyStyles);
	stream.CommitL();
	map->Reset();
	CleanupStack::PopAndDestroy(2);

	__TEST_INVARIANT;
	return id;
	}


EXPORT_C void CRichText::CopyComponentsL(CStreamStore& aStore,CStoreMap& aMap,TInt aPos,TInt aLength,TStreamId aPlainTextId) const
	{
	if (aPlainTextId == KNullStreamId)
		CPlainText::CopyComponentsL(aStore,aMap,aPos,aLength);
	if (IndexPresent())
		iIndex->StorePicturesL(aStore,aMap,aPos,aLength);
	}


// Copy the selected region of rich text and components to the specified stream.
EXPORT_C void CRichText::CopyToStreamL(RWriteStream& aStream,TInt aPos,TInt aLength) const
	{
	__TEST_INVARIANT;
	CopyToStreamL(aStream, aPos, aLength, KNullStreamId);
	}


/*
Copy the selected region of rich text and components to the specified stream.
If aPlainTextId is NULL the plain text component is stored in-line,
otherwise the reference to the plain text stream component is stored.
*/
EXPORT_C void CRichText::CopyToStreamL(RWriteStream& aStream,TInt aPos,TInt aLength,TStreamId aPlainTextId) const
	{
	CopyToStreamL(aStream,aPos,aLength,aPlainTextId,TRUE);
	}


void CRichText::CopyToStreamL(RWriteStream& aStream,TInt aPos,TInt aLength,TStreamId aPlainTextId,TBool aCopyStyles) const
	{
	if (aPlainTextId == KNullStreamId)
		{
		aStream.WriteUint8L(TRUE);
		CPlainText::CopyToStreamL(aStream,aPos,aLength);
		}
	else
		{
		aStream.WriteUint8L(FALSE);
		aStream << aPlainTextId;
		}

	if (!IndexPresent())
		CONST_CAST(CRichText*,this)->CreateAndGenerateMarkupComponentL();
	aStream.WriteUint8L(TRUE);
	iIndex->CopyToStreamL(aStream,aPos,aLength,aCopyStyles);
	}

EXPORT_C TInt CRichText::PasteFromStoreL(const CStreamStore& aStore, const CStreamDictionary& aDictionary, TInt aPos, CParagraphStyle::TStylePasteMode aStylePasteMode)
/** Pastes the contents of the clipboard into the rich text object at the
specified document position. Returns the number of characters pasted.

If the text in the clipboard has been formatted using styles, the
aStylePasteMode argument indicates whether the styles should be preserved
or discarded. If the argument is not specified, the pasted rich text
retains all formatting specified in the styles, and any new style
definitions are added to the style list of the rich text object into which
it is pasted.

@param aStore The stream store from which to paste the rich text.
@param aDictionary The stream dictionary.
@param aPos The document position at which to paste the rich text. Must be
valid, or a panic occurs.
@param aStylePasteMode Indicates whether styles in the pasted text should be
preserved or discarded.
@return The number of characters pasted. */
	{
	return DoPasteRtFromStoreL(aStore, aDictionary, aPos, aStylePasteMode);
	}


EXPORT_C TInt CRichText::PasteFromStoreL(const CStreamStore& aStore, const CStreamDictionary& aDictionary, TInt aPos)
// Paste the lesser of i) aMaxPasteLength ii) the entire clipboard contents.
// Return a count of the number of characters pasted.
//
	{
	return DoPasteRtFromStoreL(aStore, aDictionary, aPos, CParagraphStyle::EAddNewStyles);
	}


TInt CRichText::DoPasteRtFromStoreL(const CStreamStore& aStore, const CStreamDictionary& aDictionary, TInt aPos, CParagraphStyle::TStylePasteMode aStylePasteMode)
// Paste the rich text data from the specified clipboard, inserting it into this
// instance at character position aPos.  Returns the number of characters pasted.
// May be 0.
//
	{
	__TEST_INVARIANT;
	if (aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_DOPASTERTFROMSTOREL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));

	TUid type = KClipboardUidTypeRichTextWithStyles;
	TStreamId id = aDictionary.At(type);
	if (id == KNullStreamId)
		{
		type = KClipboardUidTypeRichText;
		id = aDictionary.At(type);
		}
	if (id == KNullStreamId)
		{
		type = KClipboardUidTypePlainText;
		id = aDictionary.At(type);
		}
	TInt consumed = 0;
	if (id == KNullStreamId)
		return consumed;
	if (type == KClipboardUidTypeRichText || type == KClipboardUidTypeRichTextWithStyles)
		consumed = PasteRichTextFromStoreL(aStore, aDictionary, id, aPos, aStylePasteMode);
	else if (type == KClipboardUidTypePlainText)
		consumed = PastePlainTextFromStoreL(aStore, id, aPos);

	if (consumed)
		{
		iParserData->MergeRange(aPos,0,consumed);
		CallEditObserver(aPos, consumed);
		}

	SetHasChanged(ETrue);
	__TEST_INVARIANT;
	return consumed;
	}


TInt CRichText::PasteRichTextFromStoreL(const CStreamStore& aStore, const CStreamDictionary& aDictionary, TStreamId& aRichTextStreamId, TInt aPos,CParagraphStyle::TStylePasteMode aStylePasteMode)
// Paste the plain text stream, then open and paste the rich text markup from the specified stream.
//
	{
	if (!IndexPresent())
		CreateAndGenerateMarkupComponentL();  // create the index if it does not already exist
	TStreamId id = aDictionary.At(KClipboardUidTypePlainText);
// ASSERT: We have rich text, so the plain text stream must exist.
	if (id == KNullStreamId)
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_PASTERICHTEXTFROMSTOREL, "EClipboardIntegrity" );
	    }
	__ASSERT_ALWAYS(id != KNullStreamId,Panic(EClipboardIntegrity));
	TInt consumed = CPlainText::DoPasteFromStoreL(aStore, id, aPos);
	TRAPD(ret, CompletePasteRichTextFromStoreL(aStore, aRichTextStreamId, aPos, aStylePasteMode));
	if (ret != KErrNone)
		{
		CPlainText::Delete(aPos,consumed);
		OstTrace1( TRACE_FATAL, DUP1_CRICHTEXT_PASTERICHTEXTFROMSTOREL, "Leave code=%d", ret );
		User::Leave(ret);
		}
	return consumed;
	}


void CRichText::CompletePasteRichTextFromStoreL(const CStreamStore& aStore, TStreamId& aRichTextStreamId, TInt aPos, CParagraphStyle::TStylePasteMode aStylePasteMode)
//
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore, aRichTextStreamId);
	TBool plainTextInline = (TBool)stream.ReadUint8L();
	if (!plainTextInline)
		{
		TStreamId dummy;
		stream >> dummy;
		}
	TBool markupPresentInClipboard = (TBool)stream.ReadUint8L();
	if (markupPresentInClipboard)
		iIndex->PasteFromStreamL(aStore, stream, aPos,aStylePasteMode, iGlobalParaFormatLayer, iGlobalCharFormatLayer);
	CleanupStack::PopAndDestroy();  // stream
	}


TInt CRichText::PastePlainTextFromStoreL(const CStreamStore& aStore, TStreamId& anId, TInt aPos)
// Paste the plain text stream, then update the rich text index to show any paragraph delimiters pasted.
//
	{
	TInt consumed = CPlainText::DoPasteFromStoreL(aStore, anId, aPos);
	//
	if (IndexPresent())
		{
		TRAPD(ret, CompletePastePlainTextL(aPos, consumed));
		if (ret != KErrNone)
			{
			CPlainText::Delete(aPos,consumed);
			User::Leave(ret);
			}
		}
	return consumed;
	}

void CRichText::CompletePastePlainTextL(TInt aPos, TInt aCharacterCount)
// Updates the rich text index following the pasting of global text.
//
	{
	HBufC* buf = HBufC::NewLC(aCharacterCount);
	TPtr bf = buf->Des();
	Extract(bf, aPos, aCharacterCount);
	iIndex->InsertL(aPos, bf, *iGlobalParaFormatLayer);
	CleanupStack::PopAndDestroy();  // buf
	}

EXPORT_C void CRichText::Reset()
/** Resets the document's contents to a single end-of-document delimiter. Also
deletes the style list if owned by the object. */
	{
	// Resets document contents to single end-of-document charcter,
	// and resets the index to match document content.
	TInt length = DocumentLength();
	CPlainText::Reset();  // remove all content bar eod character
	KillStyleList();  // does not get destroyed if externally owned
	KillIndex();
	SetHasChanged(ETrue);
	iParserData->KillRange();
	iParserData->iLastKnownCursor = -1;
	CallEditObserver(0, -length);

	__TEST_INVARIANT;
	}

EXPORT_C void CRichText::InsertL(TInt aPos, const TChar& aChar)
/** Inserts either a single character or a descriptor into the text object
at a specified document position.

The insertion position must be valid or a panic occurs.Note:A panic
occurs if the text object is in an "insert pending" state (i.e.
SetInsertCharFormatL() has been called and has not been
cancelled using CancelInsertCharFormat()) and aPos is not
the same as the insertion point.

@param aPos The document position at which to insert the character/descriptor.
@param aChar The character to insert. Must not be a picture character or a
panic occurs.
@param aBuf The descriptor to insert. */
	{
	__TEST_INVARIANT;
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_INSERTL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));
	if (aChar == EPictureCharacter)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_INSERTL, "ENonOverloadedInsertCalledWithPictureCharacter" );
	    }
	__ASSERT_ALWAYS(aChar!= EPictureCharacter, Panic(ENonOverloadedInsertCalledWithPictureCharacter));

	if (aChar < 0x10000)
		{
		TBuf<1> content;
		content.Append(aChar);
		RtInsertL(aPos, content);
		}
	else
		{
		TText16 high = TChar::GetHighSurrogate(aChar);
		TText16 low = TChar::GetLowSurrogate(aChar);
		RDebug::Print(_L("CRichText::InsertL(), %X expand to %X %X."), TUint(aChar), high, low);
		
		TBuf<2> content;
		content.Append(high);
		content.Append(low);
		RtInsertL(aPos, content);
		}

	__TEST_INVARIANT;
	}


EXPORT_C void CRichText::InsertL(TInt aPos, const TDesC& aBuf)
// Inserts the contents of aBuf into the text component at character position aPos.
//
	{
	__ETEXT_WATCH(INSERT_DESC);

	RtInsertL(aPos, aBuf);

	__ETEXT_WATCH_END(INSERT_DESC);
	}


EXPORT_C void CRichText::RtInsertL(TInt aPos, const TDesC& aBuf)
// Inserts the contents a aBuf into the text component at position aPos.
// Updates the index accordingly, and accounts for all embedded paragraph delimiters
// in the passed buffer aBuf.
//
	{
	__TEST_INVARIANT;
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_RTINSERTL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));

	int length = aBuf.Length();
	CPlainText::InsertL(aPos, aBuf);
	if (IndexPresent())
		{
		TRAPD(ret, iIndex->InsertL(aPos, aBuf, *iGlobalParaFormatLayer));
		if (ret != KErrNone)
			{
			CPlainText::Delete(aPos,length);
			User::Leave(ret);
			}
		SetHasChanged(ETrue);
		}

	iParserData->MergeRange(aPos,0,length);
	CallEditObserver(aPos,length);

	__TEST_INVARIANT;
	}


EXPORT_C void CRichText::InsertL(TInt aPos,const TPictureHeader& aHeader)
/**Inserts a picture header into the text object at a specified document
position. The picture header specified must reference a valid picture, or
a panic occurs.

A panic also occurs if the text object is in an "insert pending" state
(SetInsertCharFormatL() has been called and has not been cancelled using
CancelInsertCharFormat()) and aPos is not the same as the insertion point.

This method takes ownership of a picture referenced in aHeader.

@param aPos The document position at which to insert the picture header. Must
be valid, or a panic occurs.
@param aHeader A picture header. This holds a pointer to the picture to insert,
and information about the picture. */
	{
	__TEST_INVARIANT;
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXT_INSERTL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));

	CleanupStack::PushL(aHeader.iPicture);

	if (!IndexPresent())
		CreateAndGenerateMarkupComponentL();
	TBuf<1> picture;
	picture.Append(EPictureCharacter);
	CPlainText::InsertL(aPos, picture);

    CleanupStack::Pop(aHeader.iPicture);

	TBool pictureOwnershipTaken(EFalse);
	TRAPD(ret, iIndex->InsertL(aPos, aHeader,pictureOwnershipTaken));
	if (ret != KErrNone)
		{
		if(!pictureOwnershipTaken)
		    {
		    const_cast <TPictureHeader&> (aHeader).DeletePicture();
		    }
		CPlainText::Delete(aPos,picture.Length());  // remove the picture place-holder
    	User::Leave(ret);
		}

	SetHasChanged(ETrue);
	iParserData->MergeRange(aPos,0,1);
	CallEditObserver(aPos,1);
	__TEST_INVARIANT;
	}


EXPORT_C TBool CRichText::DeleteL(TInt aPos, TInt aLength)
/** Deletes one or more characters beginning at, and including, the character at
a specified document position. Can leave because paragraphs may be merged
and reformatted by the function.

@param aPos The start of the range of characters to delete. Must be valid
or a panic occurs.
@param aLength The number of characters to delete. Must be positive or a panic
occurs. The sum of aPos and aLength must be less than the document length,
or a panic occurs.
@return Indicates whether two paragraphs have been merged as a result of the
delete, indicating that the formatting of part of the resulting paragraph
may have changed. */
	{
	// Deletes aRange number of characters from the text component
	// and the corresponding index data.
	// Delete commences at, and includes, character position aPos.
	__TEST_INVARIANT;

	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_DELETEL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));
	if (aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_DELETEL, "EDebugDeleteZeroLength" );
	    }
	__ASSERT_ALWAYS(aLength >= 0, Panic(EDebugDeleteZeroLength));
	if (aPos + (aLength - 1) > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXT_DELETEL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + (aLength - 1) <= DocumentLength(), Panic(ECharPosBeyondDocument));

	TBool requireMerge = EFalse;
	if (!IndexPresent())
		CPlainText::Delete(aPos,aLength);
	else
		{
		iIndex->CancelInsertCharFormat();
		TIndexDeleteInfo info;
		iIndex->SetForDeleteL(info, aPos, aLength);
		CPlainText::Delete(aPos,aLength);
		requireMerge = iIndex->DeleteNow(info);
		}
	iParserData->MergeRange(aPos,aLength,0);
	CallEditObserver(aPos,-aLength);

	__TEST_INVARIANT;
	return requireMerge;
	}

EXPORT_C void CRichText::DeleteParagraph(TInt aPos, TInt aLength)
/** Deletes one or more whole paragraphs of text. No paragraphs can be merged together
by this function, so it cannot leave it must only be used to delete entire
paragraphs.

@param aPos The document position of the start of the first paragraph to delete.
Must be a valid position or a panic occurs.
@param aLength The number of characters to delete. */
	{
	// Use to remove entire paragraphs of text.
	// Leave-safe if called in this context.
	// MUST NOT CALL if not in this context.
	__TEST_INVARIANT;

	// Store the length of the text before we commence with deletions.
	TInt initialDocLen=DocumentLength();

	if (aPos < 0 || aPos > initialDocLen)
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_DELETEPARAGRAPH, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= initialDocLen, Panic(ECharPosBeyondDocument));
	if (aPos + aLength > initialDocLen + 1)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_DELETEPARAGRAPH, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + aLength <= initialDocLen + 1, Panic(ECharPosBeyondDocument));

	if (aLength <= 0)
		return;

	if (IndexPresent())
		iIndex->DeleteParagraph(aPos, aLength);
	CPlainText::Delete(aPos,aLength);

	// Change here for defect INC005336.
	// Previously the MergeRange call (see else line) was incorrectly handling
	// the scenario where all but the first paragraph was deleted e.g. pasting
	// multi line buffer into a single line text control in a new email on Techview
	// emulator.
	// This was because the deletion in the two lines above would delete the
	// 'end-of-text' maker causing the iEndParse member of CParserData to
	// index the char right after the end of test marker. This would panic
	// plugin parsers thatsubsquently executed.

	if (aPos+aLength > initialDocLen)
		// When deletion includes the end-of-text marker, adjust start
		// supplied so that it appears to MergeRange that we are
		// deleting the paragraph and CR just before it which belongs
		// to the previous paragraph and not the end-of-text marker.
		// This is actually the end result of the deletion anyway!
		iParserData->MergeRange(initialDocLen-aLength,aLength,0);
	else
		// All other deletions which don't.
		iParserData->MergeRange(aPos,aLength,0);

	CallEditObserver(aPos,-aLength);

	__TEST_INVARIANT;
	}

EXPORT_C void CRichText::DeleteFromParagraph(TInt aPos, TInt aLength)
/** Removes a range of characters from within a single paragraph only. Should not
be used for deleting an entire paragraph or paragraphs because this may cause
it to leave. Otherwise, it is guaranteed not to leave.

@param aPos The document position of the start of the range of characters
to delete. Must be a valid document position, or a panic occurs.
@param aLength The number of characters to delete. The sum of aPos and aLength
must be less than the document length, or a panic occurs. */
	{
	// Removes aLength characters from *within* a single paragraph only.
	// Guaranteed not to leave if this pre-condition holds true.
	__TEST_INVARIANT;

	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_DELETEFROMPARAGRAPH, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));
	if (aPos + (aLength - 1) > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_DELETEFROMPARAGRAPH, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + (aLength - 1) <= DocumentLength(), Panic(ECharPosBeyondDocument));

	if (aLength <= 0)
		return;

	if (IndexPresent())
		iIndex->DeleteFromParagraph(aPos, aLength);
	CPlainText::Delete(aPos,aLength);
	iParserData->MergeRange(aPos,aLength,0);
	CallEditObserver(aPos,-aLength);

	__TEST_INVARIANT;
	}

EXPORT_C TInt CRichText::ParagraphCount()const
/** Gets a count of the number of paragraphs in the text object.

Note:

The paragraph delimiter which terminates every text object means this function
always returns a count of at least one.

@return The number of paragraphs in the document. */
	{
	// Returns a count of the number of paragraphs
	// in the document.
	//
	__TEST_INVARIANT;

	if (IndexPresent())
		return iIndex->ParagraphCount();
	else
		return CPlainText::ParagraphCount();
	}


EXPORT_C TEtextComponentInfo CRichText::ComponentInfo() const
/** Gets information about the number of components contained in the text object
(the field count, the picture count and the style count).

@return Contains the component information. */
	{
	return TEtextComponentInfo(FieldCount(), PictureCount(), StyleCount());
	}

EXPORT_C TInt CRichText::CharPosOfParagraph(TInt& aLength, TInt aParaOffset) const
/** Finds the length and the start position of a paragraph identified by its
paragraph number. The first paragraph is numbered zero.

Notes:

if aParaOffset is invalid, (equal to or greater than the total number of
paragraphs), the function's return value is EScanEndOfData (= -1)

@param aLength On return contains the length of the specified paragraph.
@param aParaOffset The paragraph number. The first paragraph is numbered zero.
@return The document position of the first character in the paragraph. */
	{
	// Returns the character position of the first character of paragraph aParaOffset,
	// where aParaOffset specifies the nth paragraph.
	// The length of this nth paragraph is written to aLength.
	//
	// If aParaOffset specifies a paragraph that does not exist, EScanEndOfData is returned.
	//
	__TEST_INVARIANT;

	if (IndexPresent())
		return iIndex->CharPosOfParagraph(aLength, aParaOffset);
	else
		return CPlainText::CharPosOfParagraph(aLength, aParaOffset);
	}

EXPORT_C TInt CRichText::ParagraphNumberForPos(TInt& aPos) const
/** Gets the number of the paragraph which contains a document position.
Paragraph numbering begins at zero.

@param aPos A document position. Must be valid or a panic occurs. On return,
contains the document position of the first character in the paragraph in
which it is located.
@return The number of the paragraph containing the specified document position. */
	{
	// Returns the paragraph offset for the specified character position aPos.
	// aPos is in turn modified to hold the character position of the first character
	// of this paragraph.  If aPos is already on a paragraph boundary then do nothing.
	//
	__TEST_INVARIANT;
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_PARAGRAPHNUMBERFORPOS, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));

	if (IndexPresent())
		return iIndex->ParagraphNumberForPos(aPos);
	else
		return CPlainText::ParagraphNumberForPos(aPos);
	}

/** Applies character formatting to a zero length selection, for example
turning bold on. This has the effect that the formatting will be applied to
text subsequently inserted at the position. This "insert pending" state is
cancelled by calling CancelInsertCharFormat().

Note 1: After calling this function, if text is inserted at a different
position to aPos, a panic will occur, unless CancelInsertCharFormat() has been
called before the insertion to cancel the "insert pending" state.

Note 2: If the insert character format is being set for the end of the
paragraph, the paragraph delimiter is set to that format as well. This helps
end-of-paragraph behaviour be more similar to other places.

@param aFormat The character format values to apply.
@param aMask Character format mask specifying the attributes affected.
@param aPos The document position at which to insert the character format.
@pre aPos must be a valid position, or a panic will occur. */
EXPORT_C void CRichText::SetInsertCharFormatL(const TCharFormat& aFormat,
	const TCharFormatMask& aMask, TInt aPos)
	{
	SetExtendedInsertCharFormatL(aFormat, aMask,aPos);
	}

void CancelInsertCharFormat(TAny* aCRichTextIndex)
	{
	reinterpret_cast<CRichTextIndex*>(aCRichTextIndex)->CancelInsertCharFormat();
	}

void CRichText::SetExtendedInsertCharFormatL(const TCharFormatX& aFormat, const TCharFormatXMask& aMask, TInt aPos)
	{
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_SETEXTENDEDINSERTCHARFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));
	CreateAndGenerateMarkupComponentL();
	CRichTextIndex* index = iIndex.AsPtr();
	if (index->InsertCharFormatIsActive())
		{
		TCharFormatX format = aFormat;
        TCharFormatXMask mask = aMask;
        CCharFormatLayer* currentLayer = index->GetCurrentInsertCharFormat();
        currentLayer->Sense(format,mask);
		CCharFormatLayer* newLayer = CCharFormatLayer::NewCopyBaseL(currentLayer);
		CleanupStack::PushL(newLayer);
        newLayer->SetL(format,mask);
		if (Read(aPos, 1)[0] == EParagraphDelimiter)
			index->ApplyCharFormatL(aFormat, aMask, aPos, 1, EFalse);
		if (index->InsertCharFormatIsActive())
			{
			currentLayer = index->GetCurrentInsertCharFormat();
			currentLayer->Swap(*newLayer);
			}
		CleanupStack::PopAndDestroy(newLayer);
		}
	else
		{
		TCleanupItem cleanup(::CancelInsertCharFormat, index);
		CleanupStack::PushL(cleanup);
		index->NewInsertCharFormatL(aFormat, aMask, aPos);
		if (Read(aPos, 1)[0] == EParagraphDelimiter)
			index->ApplyCharFormatL(aFormat, aMask, aPos, 1, EFalse);
		CleanupStack::Pop();
		}
	SetHasChanged(TRUE);
	}
	
EXPORT_C void CRichText::ExtendedInterface(TAny*& aInterface, TUid aInterfaceId)
/**
Returns the interface corresponding to the specified UID if it exists, or 0 if not. 
Overridden versions should base call rather than returning 0.
For KUidRichText, CRichText will be returned if rich text is supported.

@param aInterfaceId The UID indicating the interface to return
@param aInterface The interface corresponding to aInterfaceId
if it is supported, or 0 if it is not
*/
	{ 
	if(KUidRichText == aInterfaceId) 
		{
		aInterface = REINTERPRET_CAST(TAny*, this);
		}
	else
		{
	    CGlobalText::ExtendedInterface(aInterface, aInterfaceId);
		}
	}

EXPORT_C TBool CRichText::DelSetInsertCharFormatL(TInt aPos, TInt aLength)
/** Deletes a range of characters. The range affected is from aPos to
aPos+(aLength-1) inclusive. It differs from DeleteL() in that this function
preserves the formatting of the deleted character at position aPos, so that
any text subsequently inserted at aPos will have that formatting applied to it.

A panic occurs if:

after calling this function, text is inserted at a different position to aPos,
without calling CancelInsertCharFormat() before the insertion

aPos is invalid

aLength is negative

the range goes beyond the end of the document

@param aPos The document position of the first character to delete.
@param aLength The number of characters to delete.
@return ETrue if two paragraphs have been merged as a result of the deletion;
EFalse if there has been no paragraph merge. */
	{
	// Delete aLength characters, commencing at, and including, aPos.
	// If aPos is on a phrase boundary, and the whole phrase or more is deleted then
	// remember temporarily the phrase format.  This is applied to any content that is
	// immediately inserted.
	//
	__TEST_INVARIANT;
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_DELSETINSERTCHARFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));
	if (aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_DELSETINSERTCHARFORMATL, "EDebugDeleteZeroLength" );
	    }
	__ASSERT_ALWAYS(aLength >= 0, Panic(EDebugDeleteZeroLength));
	if (aPos + (aLength - 1) > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXT_DELSETINSERTCHARFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + (aLength - 1) <= DocumentLength(), Panic(ECharPosBeyondDocument));

	TBool parasMerged = EFalse;
	if (!IndexPresent())
		CPlainText::DeleteL(aPos, aLength);
	else
		{
		parasMerged = iIndex->DelSetInsertCharFormatL(aPos, aLength);
		CPlainText::Delete(aPos,aLength);
		SetHasChanged(ETrue);
		}
	iParserData->MergeRange(aPos,aLength,0);
	CallEditObserver(aPos,-aLength);

	__TEST_INVARIANT;
	return parasMerged;
	}

EXPORT_C void CRichText::CancelInsertCharFormat()
/** Cancels the "insert pending" state set by a call to SetInsertCharFormatL()
or DelSetInsertCharFormatL().

This function removes the restriction on the text insertion position imposed
by these two functions. It is recommended that it is called before every cursor
movement, scroll, paste, etc. This call is a small overhead, and has no effect
if not applicable. */
	{
	// Cancels the transitory state where a specified character format
	// is applied *on top of* any inherited formatting.  eg, when bold is on.
	// Cancel when: (1) the text position is altered.  (2) the first character
	// has been inserted following the setting of this state.
	//
	if (IndexPresent() && iIndex->InsertCharFormatIsActive())
		{
		iIndex->CancelInsertCharFormat();
		SetHasChanged(ETrue);
		}
	}

EXPORT_C void CRichText::ApplyParaFormatL(const CParaFormat* aFormat, const TParaFormatMask& aMask, TInt aPos, TInt aLength)
/** Applies paragraph formatting to a range of paragraphs. The attributes which
are set in the mask are taken from aFormat and applied. The attributes which
are not set in the mask are not changed.

The region affected consists of every paragraph containing one or more
characters in the range aPos to aPos+(aLength-1).

@param aFormat Contains the paragraph format attribute values to apply.
@param aMask Specifies which paragraph format attributes should be affected.
@param aPos The document position of the start of the range.
@param aLength The number of characters in the range. */
	{
	// Applies the specified format attributes to the paragraphs covering
	// character position aPos to aPos+aLength-1.
	//
	__TEST_INVARIANT;
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_APPLYPARAFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));
	if (aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_APPLYPARAFORMATL, "EApplyParaFormatNegativeLength" );
	    }
	__ASSERT_ALWAYS(aLength >= 0,Panic(EApplyParaFormatNegativeLength));
	if (aPos + (aLength - 1) > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXT_APPLYPARAFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + (aLength - 1) <= DocumentLength(), Panic(ECharPosBeyondDocument));

	CreateAndGenerateMarkupComponentL();
	iIndex->ApplyParaFormatL(aFormat, aMask, aPos, aLength);
	SetHasChanged(ETrue);

	__TEST_INVARIANT;
	}

EXPORT_C void CRichText::ApplyCharFormatL(const TCharFormat& aFormat, const TCharFormatMask& aMask, TInt aPos, TInt aLength)
/** Applies character formatting to a range of characters. The attributes which
are set in the mask are read from aFormat and applied. The attributes which
are not set in the mask are not changed. The range of characters affected
is from aPos to aPos+(aLength-1) inclusive. The sum of aPos and aLength
must be less than or equal to the document length, or a panic occurs.

@param aFormat Contains the character format attribute values to apply.
@param aMask Bitmask specifying which character format attributes should be
applied.
@param aPos Document position from which to apply the new character formatting.
Must be greater than or equal to zero, or a panic occurs.
@param aLength The number of characters to which the new formatting should
be applied. Must be greater than or equal to zero, or a panic occurs. If the
length is zero, the function has the same effect as SetInsertCharFormatL(). */
	{
	// Applies the specified character formatting to the characters conatined within the range
	// aPos to aPos+(aLength-1).
	// If aLength is zero, the SetInsertCharFormat state is called.
	//
	__ETEXT_WATCH(APPLY_CHAR_FORMAT);
	__TEST_INVARIANT;
	
	TInt document_length = DocumentLength();
	if (aPos < 0)
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_APPLYCHARFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0,Panic(ECharPosBeyondDocument));
	if (aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_APPLYCHARFORMATL, "EApplyCharFormatNegativeLength" );
	    }
	__ASSERT_ALWAYS(aLength >= 0,Panic(EApplyCharFormatNegativeLength));
	if (aPos + aLength - 1 > document_length)
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXT_APPLYCHARFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + aLength - 1 <= document_length,Panic(ECharPosBeyondDocument));

	//If some characters are highlighted AND current position + highlighted txt = document length	
	// Fix for INC097216. Compensate for the changes introduced to Form in defect fix INC087637,
	// which now considers the height of the EOD character; meaning that this character now 
	// needs to be formatted along with rest of text.
	if ((aLength > 0) && (aPos + aLength == document_length))
		{
		aLength++;
		}
	
	DoApplyExtendedCharFormatL(aFormat, aMask, aPos, aLength);
	
	__TEST_INVARIANT;
	__ETEXT_WATCH_END(APPLY_CHAR_FORMAT);
	}
	
// This method is used internally only. It does not format the EOD character.
void CRichText::ApplyExtendedCharFormatL(const TCharFormatX& aFormat,const TCharFormatXMask& aMask,TInt aPos,TInt aLength)
	{
	TInt document_length = DocumentLength();
	if (aPos < 0)
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_APPLYEXTENDEDCHARFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0,Panic(ECharPosBeyondDocument));
	if (aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_APPLYEXTENDEDCHARFORMATL, "EApplyCharFormatNegativeLength" );
	    }
	__ASSERT_ALWAYS(aLength >= 0,Panic(EApplyCharFormatNegativeLength));
	if (aPos + aLength - 1 > document_length)
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXT_APPLYEXTENDEDCHARFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + aLength - 1 <= document_length,Panic(ECharPosBeyondDocument));
	
	DoApplyExtendedCharFormatL(aFormat, aMask, aPos, aLength);
	}	
	
// Apply the extended character formatting.	
void CRichText::DoApplyExtendedCharFormatL(const TCharFormatX& aFormat,const TCharFormatXMask& aMask,TInt aPos,TInt aLength)
	{
	if (aLength > 0)
		{
		CreateAndGenerateMarkupComponentL();
		iIndex->ApplyCharFormatL(aFormat,aMask,aPos,aLength);
		SetHasChanged(ETrue);
		}
	else
		SetExtendedInsertCharFormatL(aFormat,aMask,aPos);
	}		

EXPORT_C void CRichText::SetStyleListExternallyOwned(const CStyleList& aStyleList)
/** Assigns an externally owned style list to the rich text object.
Replaces any previous style list used by the object. Calls
SetStyleListExternallyOwned(ETrue).

@param aExternallyOwned The style list to assign to this rich text object.
Not owned by the rich text object. */
	{
	CStyleList* styleList = CONST_CAST(CStyleList*, &aStyleList);
	iStyleList = styleList;
	SetStyleListExternallyOwned(ETrue);
	}

EXPORT_C void CRichText::ApplyParagraphStyleL(const CParagraphStyle& aStyle, TInt aPos, TInt aLength, CParagraphStyle::TApplyParaStyleMode aMode)
/** Applies a specified paragraph style to a range of paragraphs. The region
affected consists of every paragraph containing one or more characters in the
range aPos to aPos+(aLength-1).

A panic occurs if:

aPos is invalid, or

aLength is negative, or

the range goes beyond the end of the document, or

the rich text object has no style list

@param aStyle The style to apply.
@param aPos The document position of the start of the range.
@param aLength The number of characters in the range.
@param aMode Controls what specific formatting, if any, should be preserved
when the style is applied. */
	{
	// Applies the specified paragraph style to the paragraphs covering
	// character positions aPos to aPos+aLength-1.
	//
	__TEST_INVARIANT;
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_APPLYPARAGRAPHSTYLEL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));
	if (aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_APPLYPARAGRAPHSTYLEL, "EApplyParaStyleNegativeLength" );
	    }
	__ASSERT_ALWAYS(aLength >= 0, Panic(EApplyParaStyleNegativeLength));
	if (aPos + (aLength - 1) > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXT_APPLYPARAGRAPHSTYLEL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + (aLength - 1) <= DocumentLength(), Panic(ECharPosBeyondDocument));
	if (!StyleListPresent())
	    {
	    OstTrace0( TRACE_FATAL, DUP3_CRICHTEXT_APPLYPARAGRAPHSTYLEL, "ERichTextNotSetForUsingStyles" );
	    }
	__ASSERT_ALWAYS(StyleListPresent(), Panic(ERichTextNotSetForUsingStyles));

	CreateAndGenerateMarkupComponentL();
	iIndex->ApplyParagraphStyleL(aStyle, aPos, aLength, iGlobalCharFormatLayer, aMode);
	SetHasChanged(ETrue);

	__TEST_INVARIANT;
	}

EXPORT_C void CRichText::NotifyStyleChangedL(const CParagraphStyle* aTo, const CParagraphStyle* aFrom)
/** Removes a style from every paragraph in the document to which it applies,
and replaces it with another.

If style aTo is NULL, aFrom is replaced by the global character and paragraph
format layers, so that in effect, style aFrom is removed. Any specific
formatting which has been applied to the paragraphs is retained.

Notes:

This function should be called on the text content object after changing a
style in the style list.

A panic occurs if the rich text object does not use a style list (this can
be tested for using StyleListPresent()).

@param aTo The new paragraph style to apply.
@param aFrom The paragraph style to remove. */
	{
	// Update the rich text index following the change of an applied paragraph style.
	//
	__TEST_INVARIANT;
	if (!StyleListPresent())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_NOTIFYSTYLECHANGEDL, "ERichTextNotSetForUsingStyles" );
	    }
	__ASSERT_ALWAYS(StyleListPresent(), Panic(ERichTextNotSetForUsingStyles));

	CreateAndGenerateMarkupComponentL();
	iIndex->NotifyStyleChangedL(aTo, aFrom, *iGlobalParaFormatLayer, *iGlobalCharFormatLayer);
	SetHasChanged(ETrue);

	__TEST_INVARIANT;
	}

EXPORT_C const CParaFormatLayer* CRichText::ParagraphStyle(TBool& aStyleChangesOverRange, TInt aPos, TInt aLength) const
/** Gets a pointer to the first paragraph style encountered in the specified
range.

@param aStyleChangesOverRange On return, set to ETrue if more than one paragraph
style is used over the specified range of characters. Otherwise EFalse
@param aPos The document position of the start of the range. Must be valid.
@param aLength The number of characters in the range. Must be greater than
or equal to zero.
@return Pointer to the paragraph style which applies to the paragraph containing
document position aPos. Its type (returned by CParaFormatLayer::Type())
indicates whether this object is a style, or just a paragraph format layer. */
	{
	__TEST_INVARIANT;
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_PARAGRAPHSTYLE, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));
	if (aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_PARAGRAPHSTYLE, "EParagraphStyleNegativeLength" );
	    }
	__ASSERT_ALWAYS(aLength >= 0,Panic(EParagraphStyleNegativeLength));
	if (aPos + (aLength - 1) > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXT_PARAGRAPHSTYLE, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + (aLength - 1) <= DocumentLength(), Panic(ECharPosBeyondDocument));

	if (IndexPresent())
		return iIndex->ParagraphStyle(aStyleChangesOverRange, aPos, aLength);
	else
		{
		aStyleChangesOverRange = EFalse;
		return iGlobalParaFormatLayer;
		}
	}

EXPORT_C void CRichText::SetHasChanged(TBool aHasChanged)
/** Sets whether the document's content or formatting has changed. This function
is called with an value of ETrue by all functions which modify the text content
or formatting. Use CEditableText::HasChanged() to test whether the document
has changed.

@param aHasChanged ETrue if the text object has been changed, EFalse if not. */
	{
	// Replaces the base class method of the same name.
	//
	if (aHasChanged && IndexPresent())
		iIndex->DocumentChanged();
	CGlobalText::SetHasChanged(aHasChanged);
	}

EXPORT_C void CRichText::RemoveSpecificParaFormatL(TInt aPos, TInt aLength)
/** Removes all specific paragraph formatting from a range of paragraphs. This
does not remove formatting from the object's global paragraph format layer.
The region affected consists of every paragraph containing one or more
characters in the range covered by document position aPos to aPos+(aLength-1)
inclusive.

A panic occurs in the following situations:

the position is negative,

the length is negative,

the range goes beyond the end of the document

@param aPos The document position of the start of the range.
@param aLength The number of characters in the range. */
	{
	// Removes all specific paragraph formatting from the selected region.
	//
	__ETEXT_WATCH(REMOVE_PARA_FORMAT);

	__TEST_INVARIANT;

	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_REMOVESPECIFICPARAFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));
	if (aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_REMOVESPECIFICPARAFORMATL, "ERemoveSpecificParaFormatNegativeLength" );
	    }
	__ASSERT_ALWAYS(aLength >= 0, Panic(ERemoveSpecificParaFormatNegativeLength));
	if (aPos + (aLength - 1) > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXT_REMOVESPECIFICPARAFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + (aLength - 1) <= DocumentLength(), Panic(ECharPosBeyondDocument));

	if (IndexPresent())
		{
		iIndex->RemoveSpecificParaFormatL(aPos, aLength);
		SetHasChanged(ETrue);
		}

	__TEST_INVARIANT;

	__ETEXT_WATCH_END(REMOVE_PARA_FORMAT);
	}

EXPORT_C void CRichText::RemoveSpecificCharFormatL(TInt aPos, TInt aLength)
/** Removes all specific character formatting from a range of characters (does
not remove the formatting which has been taken from the object's global character
format layer). A panic occurs in the following situations:

the position is negative,

the length is negative,

the range goes beyond the end of the document

@param aPos The document position of the start of the region affected.
@param aLength The number of characters in the region affected. If zero, the
function has no effect. */
	{
	__TEST_INVARIANT;
	
	TInt document_length = DocumentLength();
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_REMOVESPECIFICCHARFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));
	if (aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_REMOVESPECIFICCHARFORMATL, "ERemoveSpecificParaFormatNegativeLength" );
	    }
	__ASSERT_ALWAYS(aLength >= 0, Panic(ERemoveSpecificParaFormatNegativeLength));
	if (aPos + (aLength - 1) > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXT_REMOVESPECIFICCHARFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + (aLength - 1) <= DocumentLength(), Panic(ECharPosBeyondDocument));
	
	// in correspondance to INC097216, character format removing considers the height of end of document
    // character
    if (aPos + aLength == document_length)
   		{
        aLength++;
        }

	if (aLength > 0 && IndexPresent())
		{
		iIndex->RemoveSpecificCharFormatL(aPos, aLength);
		SetHasChanged(ETrue);
		}

	__TEST_INVARIANT;
	}

EXPORT_C void CRichText::GetChars(TPtrC& aText, TCharFormat& aFormat, TInt aPos) const
/** Gets a constant pointer descriptor to a portion of the text object
with constant character formatting.

The view starts at the document position specified, and ends at: the
last character which shares the same character formatting, orthe end
of the document, orthe end of the segment, if segmented storage is
being usedwhichever occurs first. Also fills a character format object
with the character formatting of the range of characters.

@param aView On return, a constant pointer to a portion of the text.
@param aFormat On return, contains the character formatting of the text.
@param aPos The start position for the view. Must be a valid document
position, or a panic occurs. */
	{
	// Get a run of text and its format, starting at aPos.
	__ETEXT_WATCH(GET_CHARS)
	TCharFormatX format;
	GetTextAndExtendedFormat(aText, format, aPos);
	OverrideFormatForParsersIfApplicable(aText, format, aPos);
	aFormat = format.iCharFormat;
	OverrideFormatOfInlineTextIfApplicable(aText, aFormat, aPos);
	__ETEXT_WATCH_END(GET_CHARS)
	}


void CRichText::GetTextAndExtendedFormat(TPtrC& aText,TCharFormatX& aFormat,TInt aPos) const
	{
	__TEST_INVARIANT;
	TInt documentLength = DocumentLength();
	if (aPos < 0 || aPos > documentLength)
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_GETTEXTANDEXTENDEDFORMAT, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= documentLength, Panic(ECharPosBeyondDocument));
	if (!IndexPresent())
		{
		aText.Set(Read(aPos));
		iGlobalCharFormatLayer->SenseEffective(aFormat);
		}
	else
		{
		int phrase_length = iIndex->GetChars(aFormat,aPos);
		aText.Set(Read(aPos,phrase_length));
		}
	__TEST_INVARIANT;
	}

EXPORT_C TInt CRichText::GetPictureSizeInTwips(TSize& aSize, TInt aPos) const
/** Gets the size of a picture located at a specified document position.

@param aSize On return, contains the size of the picture located at aPos.
@param aPos Document position of the picture. Must be a valid position.
@return KErrNotFound if there is no picture at the specified document position,
KErrNone if there is. */
	{
	__TEST_INVARIANT;
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_GETPICTURESIZEINTWIPS, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));

	if (IndexPresent())
		return iIndex->GetPictureSizeInTwips(aSize, aPos);
	else
		return KErrNotFound;
	}

EXPORT_C CPicture* CRichText::PictureHandleL(TInt aPos, MLayDoc::TForcePictureLoad aForceLoad) const
/** Gets a pointer to the picture located at a specified document position, if
one exists. If the picture is not in memory, the function loads it (if the
second argument has a value of MLayDoc::EForceLoadTrue).

Note:

In order to load the picture, a picture factory and a store resolver must
have been set.

@param aPos Document position of the picture character. Must be a valid position.
@param aForceLoad If the picture is not loaded into memory,
MLayDoc::EForceLoadTrue loads it using the picture factory;
MLayDoc::EForceLoadFalse does not, and in this case, the function returns NULL.
@return A pointer to the picture located at aPos. NULL if aPos does not specify
a picture character, or if there is a picture at aPos which is not in memory,
and the second argument is MLayDoc::EForceLoadFalse. */
	{
	__ETEXT_WATCH(PICTURE_HANDLE);

	__TEST_INVARIANT;
	if (aPos<0 || aPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_PICTUREHANDLEL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=DocumentLength(),Panic(ECharPosBeyondDocument));

	if (IndexPresent())
		return iIndex->PictureHandleL(aPos, aForceLoad);
	else
		return NULL;

	__ETEXT_WATCH_END(PICTURE_HANDLE);
	}

EXPORT_C void CRichText::GetParagraphFormatL(CParaFormat* aFormat, TInt aPos) const
/** Gets the effective paragraph formatting which applies to the paragraph which
contains a specified document position. On return, aFormat is filled with
values for all paragraph format attributes.

@param aFormat On return, filled with the paragraph's effective paragraph
formatting.
@param aPos Any document position within the paragraph of interest. */
	{
	__ETEXT_WATCH(GET_PARAGRAPH_FORMAT)

	__TEST_INVARIANT;
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_GETPARAGRAPHFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));

	if (!IndexPresent())
		CGlobalText::GetParagraphFormatL(aFormat, aPos);
	else
		{
		aFormat->Reset();
		iIndex->GetParagraphFormatL(aFormat, aPos);
		}

	__ETEXT_WATCH_END(GET_PARAGRAPH_FORMAT)
	}

EXPORT_C void CRichText::GetSpecificParagraphFormatL(CParaFormat* aFormat,
													 TParaFormatMask& aMask,
													 TInt aPos) const
// Fills aFormat with the effective Paragraph format attributes for the paragraph
// in which character position aPos is contained.
//
	{
	__ETEXT_WATCH(GET_PARAGRAPH_FORMAT)

	__TEST_INVARIANT;
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_GETSPECIFICPARAGRAPHFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));

	aFormat->Reset();
	aMask.ClearAll();
	if (IndexPresent())
		iIndex->GetSpecificParagraphFormatL(aFormat, aMask, aPos);

	__ETEXT_WATCH_END(GET_PARAGRAPH_FORMAT)
	}

EXPORT_C void CRichText::GetParaFormatL(CParaFormat* aFormat, TParaFormatMask& aVaries, TInt aPos, TInt aLength,
										CParaFormat::TParaFormatGetMode aMode) const
/** Gets the effective paragraph formatting which applies to a range of paragraphs.
The region involved is every paragraph containing one or more characters in
the range aPos to aPos+(aLength-1) inclusive. On return, aFormat is filled
with values for all paragraph format attributes and the mask indicates the
values that change over the region, and whose value is therefore indeterminate.

Note:

If aMode has a value of EFixedAttributes, the function cannot leave.

@param aFormat Must not be NULL or a panic occurs. On return, contains the
effective paragraph formatting for the range of paragraphs.
@param aVaries On return, a bitmask indicating which paragraph format attributes
vary over the range of characters selected.
@param aPos The document position of the start of the range.
@param aLength The number of characters in the range.
@param aMode The default, EAllAttributes means that values for all paragraph
format attributes are written to aFormat. EFixedAttributes means that tabs,
bullets and borders are not written to aFormat. */
	{
	// Senses the paragraph format of para(s) covered by the region aPos to aPos+aLength-1.
	// aFormat takes the values of all attributes, and the mask aMask indicates those values that change
	// over the selected region, and are therefore *indeterminate*.
	// Application: seeding paragraph formatting dialogs.
	//
	__TEST_INVARIANT;
	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_GETPARAFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));
	if (aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_GETPARAFORMATL, "EGetParaFormatNegativeLength" );
	    }
	__ASSERT_ALWAYS(aLength >= 0, Panic(EGetParaFormatNegativeLength));
	if (aPos + aLength > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXT_GETPARAFORMATL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + aLength <= DocumentLength(), Panic(ECharPosBeyondDocument));

	if (IndexPresent())
		iIndex->GetParaFormatL(aFormat, aVaries, aPos, aLength, aMode);
	else
		CGlobalText::GetParaFormatL(aFormat, aVaries, aPos, aLength, aMode);
	}

EXPORT_C void CRichText::GetCharFormat(TCharFormat& aFormat, TCharFormatMask& aVaries, TInt aPos, TInt aLength) const
/** Gets the effective character formatting which applies to a range of
characters. The range of characters involved is from aPos to aPos+(aLength-1)
inclusive. On return, aFormat is filled with values for all character format
attributes, and on return, the mask indicates the values that change over the
region, and whose value is therefore indeterminate.

The length value can be zero. In this case, the character formatting sensed
is that of the character immediately to the left of the position specified.

@param aFormat On return, contains the character format values for the range
of characters.
@param aVaries On return, indicates which character format attributes vary
over the range.
@param aPos Document position of the start of the range. Must be valid or a
panic occurs.
@param aLength Number of characters in the range. Must be greater than or equal
to zero, or a panic occurs. */
	{
	// Senses the character formatting of the phrase(s) covered by the region aPos to aPos+aLength-1.
	// May be called with zero length.
	// aFormat takes the values of all character format attributes, and the mask aMask indicates those
	// values that change over the selected region, and are therefore *indeterminate*.
	// Application: seeding character formatting dialogs.
	//
	TCharFormatX format;
	TCharFormatXMask varies;
	GetExtendedCharFormat(format, varies, aPos, aLength);
	aFormat = format.iCharFormat;
	varies.ClearExtendedAttribs();
	aVaries = varies;
	}


void CRichText::GetExtendedCharFormat(TCharFormatX& aFormat, TCharFormatXMask& aVaries, TInt aPos, TInt aLength) const
	{
	__TEST_INVARIANT;
	int document_length = DocumentLength();
	if (aPos < 0 || aPos > document_length)
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_GETEXTENDEDCHARFORMAT, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= document_length, Panic(ECharPosBeyondDocument));
	if (aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_GETEXTENDEDCHARFORMAT, "EGetCharFormatNegativeLength" );
	    }
	__ASSERT_ALWAYS(aLength >= 0, Panic(EGetCharFormatNegativeLength));
	if (aPos + aLength - 1 > document_length)
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXT_GETEXTENDEDCHARFORMAT, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + aLength - 1 <= document_length, Panic(ECharPosBeyondDocument));

	if (IndexPresent())
		iIndex->GetCharFormat(aFormat, aVaries, aPos, aLength);
	else
		{
		iGlobalCharFormatLayer->SenseEffective(aFormat);
		aVaries.ClearAll();
		}
	}


void CRichText::GetSpecificCharFormatLeftRight(TCharFormat& aFormat,
									TCharFormatMask& aMask,
									TInt aPos,
									TBool aLeft) const
	{
	__ETEXT_WATCH(GET_SPECIFIC_CHARS);

	__TEST_INVARIANT;

	if (aPos < 0 || aPos > DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_GETSPECIFICCHARFORMATLEFTRIGHT, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= DocumentLength(), Panic(ECharPosBeyondDocument));

	if (IndexPresent())
		{
		TCharFormatX format;
		TCharFormatXMask mask;
		iIndex->GetSpecificCharFormatDirection(format, mask, aPos, aLeft);
		aFormat = format.iCharFormat;
		mask.ClearExtendedAttribs();
		aMask = mask;
		}

	__ETEXT_WATCH_END(GET_SPECIFIC_CHARS);
	}

EXPORT_C void CRichText::GetSpecificCharFormat(TCharFormat& aFormat, TCharFormatMask& aMask, TInt aPos) const
/** Gets the specific character formatting which applies to the character to the
left of a document position. Specific formatting is just the formatting which
has been applied over the object's global format layers - it is not the
same as the effective formatting.

@param aFormat On return contains the specific character formatting which
applies to the character to the left of the document position.
@param aMask On return, indicates which character format attributes have been
applied specifically, (not taken from the global layers).
@param aPos The document position. Must be valid or a panic occurs. */
	{
	// Return the format attributes store in the specific layer only, for the specified document position.
	// THIS IS NOT THE EFFECTIVE FORMAT, BUT THE SPECIFIC FORMATTING ONLY.
	//
	GetSpecificCharFormatLeftRight(aFormat, aMask, aPos, ETrue);
	}

EXPORT_C void CRichText::GetSpecificCharFormatRight(TCharFormat& aFormat, TCharFormatMask& aMask, TInt aPos) const
/** Gets the specific character formatting which applies to the character logically
after a document position. Note that this is not necessarily the character
to the right of the document position, because right to left text is supported.

Specific formatting is just the formatting which has been applied over the
object's global format layers it is not the same as the effective formatting.

@param aFormat On return, contains the specific character formatting which
applies to the character logically after the document position.
@param aMask On return, indicates which character format attributes have been
applied specifically, (not taken from the global layers).
@param aPos The document position. Must be valid or a panic occurs. */
	{
	GetSpecificCharFormatLeftRight(aFormat, aMask, aPos, EFalse);
	}

TBool CRichText::IndexPresent()const
// Returns ETrue if the rich text index is present, otherwise
// returns EFalse.
//
	{
	return iIndex.IsPtr() && iIndex.AsPtr();
	}


EXPORT_C TInt CRichText::PictureCount() const
/** Gets a count of the number of pictures contained in the rich text object.

@return The picture count. */
	{return (IndexPresent()) ? iIndex->iPictureCount : 0;}


void CRichText::SetParaTypeIsSingle(TBool aBool)
	{
	if (aBool)
		iFlags |= KParaTypeIsSingle;
	else
		iFlags &= ~KParaTypeIsSingle;
	}


TBool CRichText::ParaTypeIsSingle() const
	{return iFlags & KParaTypeIsSingle;}


EXPORT_C void CRichText::AppendTakingSolePictureOwnershipL(const CRichText& aSource)
/** Appends a rich text object to this one. The text is appended immediately after
the end-of-text paragraph delimiter. The incoming text's formatting is set
to be based on the global formatting of this rich text object.

Notes:

If this rich text object is empty (e.g. because it is newly initialised, or
has been reset), then the end-of-text delimiter of the incoming rich text
is not appended. This avoids the possibility of having a trailing paragraph
delimiter, giving one more empty line than would typically be desired.

If the incoming rich text contains pictures which have been loaded into memory,
their sole ownership is transferred to the current rich text object. In aSource,
these picture handles are set to NULL.

@param aSource The rich text object to append. */
	{
	// Appends the specified rich text object to this one.
	// If this rich text is empty or has been newly initialised or reset, then the final paragraph delimiter
	// of the incoming rich text is NOT appended, thus avoiding the prospect of having a trailing, empty
	// paragraph delimiter, giving one more empty line than would typically be desired.
	//
	__ETEXT_WATCH(APPEND_RICH_TEXT);

	// Append the text
	const TInt thisDocumentLength = DocumentLength();
	MPictureFactory* factory = NULL;
	MRichTextStoreResolver* resolver = NULL;
	if (thisDocumentLength == 0)  // this is an empty document
		{
		factory = iPictureFactory;
		resolver = iStoreResolver;
		Reset();  // destroy the markup component if it exists.  Makes the job easier
		}
	//
	PrepareAppendMarkupL(aSource);  // requires no rollback - objects in a (bigger) stable state
	//
	TRAPD(ret, DoAppendTakingSolePictureOwnershipL(aSource));
	if (ret != KErrNone)
		{
		CPlainText::Delete(thisDocumentLength,DocumentLength() - thisDocumentLength);
		User::Leave(ret);
		}
	if (thisDocumentLength == 0)
		{
		DeleteParagraph(0, 1);   // remove excess first paragraph from empty documents
		SetPictureFactory(factory, resolver);
		}
	int new_length = DocumentLength() - thisDocumentLength;
	iParserData->MergeRange(thisDocumentLength,0,new_length);
	CallEditObserver(thisDocumentLength,new_length);

	__TEST_INVARIANT;

	__ETEXT_WATCH_END(APPEND_RICH_TEXT);
	}


void CRichText::DoAppendTakingSolePictureOwnershipL(const CRichText& aSource)
	{
	TInt lengthRemaining = aSource.DocumentLength() + 1;  // we want to append the para delimiters also!
	TInt consumed = 0;
	FOREVER
		{
		TPtrC view = aSource.Read(consumed);
		if (view.Length() > lengthRemaining)
			view.Set(view.Ptr(), lengthRemaining);
		CPlainText::DoPtInsertL(DocumentLength() + 1, view);  // insert AFTER the final paragraph delimiter
		TInt viewLength = view.Length();
		lengthRemaining -= viewLength;
		if (lengthRemaining == 0)
			break;
		consumed += viewLength;
		}

	if (IndexPresent())
		{
		if (!aSource.IndexPresent())
		    {
		    OstTrace0( TRACE_DUMP, DUP1_CRICHTEXT_DOAPPENDTAKINGSOLEPICTUREOWNERSHIPL, "Invariant" );
		    }
		__ASSERT_DEBUG(aSource.IndexPresent(), User::Invariant());       // PrepareAppend should have sorted this

		TGlobalLayerInfoAppend info(GlobalParaFormatLayer(), GlobalCharFormatLayer(), aSource.GlobalParaFormatLayer(), aSource.GlobalCharFormatLayer());
		iIndex->AppendTakingSolePictureOwnershipL(aSource.iIndex, info);
		}

	__TEST_INVARIANT;
	}


void CRichText::PrepareAppendMarkupL(const CRichText& aSource)
// Guarantees that both the component and aggregate objects have a valid markup component.
//
	{
	if (aSource.HasMarkupData() && !HasMarkupData())
		CreateAndGenerateMarkupComponentL();
	else if (IndexPresent())
		CONST_CAST(CRichText&, aSource).CreateAndGenerateMarkupComponentL();
	}


EXPORT_C void CRichText::AppendParagraphL(TInt aReplicas)
/** Appends one or more empty paragraphs to the document. The new paragraphs take
on the formatting specified in the global format layers.

@param aReplicas The number of empty paragraphs to append to the document.
By default, a single paragraph. */
	{
	// Inserts an empty paragraph at the end of the document.
	// The new paragraph takes on the format as described
	// by the global format layers.
	//
	__ETEXT_WATCH(APPEND_PARAGRAPH);

	__TEST_INVARIANT;

	TInt documentLength = DocumentLength();
	if (aReplicas == 1)
		CPlainText::InsertL(documentLength, CEditableText::EParagraphDelimiter);
	else
		{
		HBufC* bb = HBufC::NewLC(aReplicas);
		TPtr buf = bb->Des();
		buf.Fill(CEditableText::EParagraphDelimiter, aReplicas);
		CPlainText::InsertL(documentLength, buf);
		CleanupStack::PopAndDestroy();  // bb
		}
	//
	if (IndexPresent())
		{
		TRAPD(ret,
		iIndex->AppendParagraphL(iGlobalParaFormatLayer, iGlobalCharFormatLayer, aReplicas));
		if (ret != KErrNone)
			{
			CPlainText::Delete(DocumentLength() - aReplicas, aReplicas);
			User::Leave(ret);
			}
		SetHasChanged(ETrue);
		}

	int new_length = DocumentLength() - documentLength;
	iParserData->MergeRange(documentLength,0,new_length);
	CallEditObserver(documentLength,new_length);
	__TEST_INVARIANT;

	__ETEXT_WATCH_END(APPEND_PARAGRAPH);
	}
