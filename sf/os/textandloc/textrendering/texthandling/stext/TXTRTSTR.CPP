/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TXTRTSTRTraces.h"
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TXTETEXT_INTERNAL.H"
#include "TXTRICH_INTERNAL.H"
#endif

///////////////////////////////////////////////////////////
//
// Default persist functions - Deferred loading of pictures
//
///////////////////////////////////////////////////////////


EXPORT_C void CRichText::RestoreWithStyleListL(const CStreamStore& aStore, TStreamId aStreamId, const CStyleList& aExternalStyleList)
/** Restores a rich text object, including all of its components and an 
externally owned style list, from a stream store.

@param aStore Stream store from which the object is restored. 
@param aStreamId ID of the stream store. 
@param aExternalStyleList An externally owned style list. */
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore,aStreamId);
	//
	InternalizeL(stream,&aExternalStyleList);
	CleanupStack::PopAndDestroy();  // stream
	RestoreComponentsL(aStore);
	}

EXPORT_C void CRichText::StoreComponentsL(CStreamStore& aStore, CStoreMap& aMap) const
/** Stores the rich text object's components (field set, style list, formatting 
and pictures) to the stream store specified.

@param aStore Stream store to which the text components are written. 
@param aMap A store map. This binds the address of text components to the 
stream ID of aStore. This is needed to support the deferred loading of pictures 
in rich text. */
	{
	CGlobalText::StoreComponentsL(aStore,aMap);
	StoreStylesL(aStore,aMap);
	StoreMarkupL(aStore,aMap);
	}
 
EXPORT_C void CRichText::RestoreComponentsL(const CStreamStore& aStore)
/** Restores the rich text object's components (field set, style list,
 formatting and pictures) from a stream store.

@param aStore The stream store from which the text object's components are 
restored. */
	{
	CPlainText::RestoreComponentsL(aStore);  // field components
	if (iStyleList.IsId() && iStyleList.AsId()!=KNullStreamId)
		RestoreStylesL(aStore,iStyleList.AsId(),iGlobalParaFormatLayer,iGlobalCharFormatLayer);
	if (iIndex.AsId()!=KNullStreamId)
		iIndex=CRichTextIndex::NewL(aStore,iIndex.AsId(),iGlobalParaFormatLayer,iGlobalCharFormatLayer,*this);	
	}

 
	
EXPORT_C void CRichText::ExternalizeL(RWriteStream& aStream)const
/** Externalises the rich text object and all of its components (field set, 
style list, rich text formatting, pictures) to a read stream. The presence 
of this function means that the standard templated operator<<() (defined in 
s32strm.h) is available to externalise objects of this class.

Notes:

Any "insert pending" state is cancelled (calls CancelInsertCharFormat()).

The global format layers are not owned by the object; they are supplied by 
the owner of the object and not saved when the object is externalised or loaded 
when it is internalized.

StoreComponentsL() must be called before calling this function. This is necessary to set
up the the stream store needed to externalize rich text objects. It is not possible to get
a stream store into a stream and a rich text object cannot be  externalized to an arbitrary 
stream thus StoreComponentsL() will set up this stream store before externalizing.
 
The inherited function CEditableText::StoreL() combines both calling of StoreComponentsL()
to set up the stream store and externalizing the rich text object to this stream store.

@param aStream Stream to which to write the rich text object. */
	{
	__TEST_INVARIANT;

	CONST_CAST(CRichText*,this)->CancelInsertCharFormat();
	CEditableText::ExternalizeL(aStream);
	DoExternalizeFieldDataL(aStream);
	DoExternalizeStyleDataL(aStream);
	DoExternalizeMarkupDataL(aStream);
	DoExternalizePlainTextL(aStream);
	}

 
EXPORT_C void CRichText::InternalizeL(RReadStream& aStream)
/** Internalises the rich text object and all of its components (field set, 
style list, rich text formatting and pictures) from a read stream. The 
presence of this function means that the standard templated operator>>() 
(defined in s32strm.h) is available to internalise objects of this class.

Note: The global format layers are not owned by the object; they are supplied by 
the owner of the object and not saved when the object is externalised or loaded 
when it is internalized.

@param aStream Stream from which to read the rich text object. */
	{InternalizeL(aStream,NULL);}


void CRichText::InternalizeL(RReadStream& aStream,const CStyleList* aExternalStyleList)
// Restore this rich text object.
// The global format layers are not present in this persistent state.
//
	{
	CEditableText::InternalizeL(aStream);
	DoInternalizeFieldDataL(aStream);
	DoInternalizeStyleDataL(aStream,aExternalStyleList);
	DoInternalizeMarkupDataL(aStream);
	DoInternalizePlainTextL(aStream);
	if (iParserData == NULL)
		iParserData = new(ELeave) CParserData(DocumentLength()); 
	iParserData->KillRange();
	iParserData->MergeRange(0,0,DocumentLength());
	CallEditObserver(0,DocumentLength());
	}


void CRichText::StoreStylesL(CStreamStore& aStore,CStoreMap& aMap)const
// Write the style list, if present, out of line.
//
	{
	if (StyleListPresent() && !StyleListExternallyOwned())
		{
		TStreamId id=iStyleList->StoreL(aStore);
		aMap.BindL(iStyleList,id);
		}
	}


void CRichText::RestoreStylesL(const CStreamStore& aStore,TStreamId aId,const CParaFormatLayer* aParaFormatLayer,const CCharFormatLayer* aCharFormatLayer)
// Restore the style list
//
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore,aId);
	//
	iStyleList=CStyleList::NewL(stream,aParaFormatLayer,aCharFormatLayer);
	CleanupStack::PopAndDestroy();  // stream
	}


void CRichText::StoreMarkupL(CStreamStore& aStore,CStoreMap& aMap)const
// Write the index/markup data, if present, out of line.
//
	{
	if (IndexPresent())
		{
		TStreamId id=iIndex->StoreMarkupL(aStore,iStyleList);
		aMap.BindL(iIndex,id);
		}
	}


EXPORT_C void CRichText::DoExternalizeStyleDataL(RWriteStream& aStream)const
// Write to the stream, the T.V. representing the style definitions.
//
	{
	aStream<< KRichTextStyleDataUid;
	if (StyleListPresent() && !StyleListExternallyOwned())
		aStream<< iStyleList;
	else
		aStream<< KNullStreamId;
	}


EXPORT_C void CRichText::DoInternalizeStyleDataL(RReadStream& aStream)
// Load the out-of-line component representing the style list.
//
	{DoInternalizeStyleDataL(aStream,NULL);}


void CRichText::DoInternalizeStyleDataL(RReadStream& aStream,const CStyleList* aExternalStyleList)
// Load the out-of-line component representing the style list.
//
	{
	TUid uid=UidFromStreamL(aStream);
	while (uid!=KRichTextStyleDataUid)
		{
		if (uid==KPlainTextCharacterDataUid)
			User::Leave(KErrCorrupt);  // There is no style or markup Data !!!!!
		CPlainText::ConsumeAdornmentL(aStream);
		uid=UidFromStreamL(aStream);
		}
	aStream>> iStyleList;
	if (aExternalStyleList)
		iStyleList=CONST_CAST(CStyleList*,aExternalStyleList);
	}



EXPORT_C void CRichText::DoExternalizeMarkupDataL(RWriteStream& aStream)const
// Write to the stream, the T.V. representing the markup.
//
	{
	aStream<< KRichTextMarkupDataUid;
	aStream<< iIndex;  // the specific markup
	}


EXPORT_C void CRichText::DoInternalizeMarkupDataL(RReadStream& aStream)
// Load the out-of-line component representing the markup data.
	{
	TUid uid=UidFromStreamL(aStream);
	while (uid!=KRichTextMarkupDataUid)
		{
		if (uid==KPlainTextCharacterDataUid)
			User::Leave(KErrCorrupt);  // There is no style or markup Data !!!!!
		CPlainText::ConsumeAdornmentL(aStream);
		uid=UidFromStreamL(aStream);
		}
	KillIndex();  // prevent alloc heaven when current index handle is lost to internalized swizzle
	aStream>> iIndex;
	}


///////////////////////////////////////////////////////////
//
// Custom persist functions
//
///////////////////////////////////////////////////////////


 

EXPORT_C void CRichText::ExternalizeStyleDataL(RWriteStream& aStream)const
/** Externalises the style list owned by the rich text object.

@param aStream Stream to which to write the style list. */
	{
	__TEST_INVARIANT;
	
	if (StyleListPresent() && !StyleListExternallyOwned())
		{
		aStream.WriteUint8L((TUint8)1);  // there follows a style list
		aStream<< *iStyleList;
		}
	else
		aStream.WriteUint8L((TUint8)0);  // there are no paragraph styles
	}

EXPORT_C void CRichText::InternalizeStyleDataL(RReadStream& aStream)
/** Internalises the style list owned by the rich text object from a read stream, 
if one is present in the stream.

@param aStream Stream from which to read the style list. */
	{
	TUint8 styleListPresent=aStream.ReadUint8L();
	if (styleListPresent)
		{
		CStyleList* styleList=CStyleList::NewL(aStream,iGlobalParaFormatLayer,iGlobalCharFormatLayer);
		//coverity[leave_without_push]
		delete iStyleList.AsPtr();
		iStyleList=styleList;
		}
	}

EXPORT_C void CRichText::ExternalizeMarkupDataL(RWriteStream& aStream)const
/** Externalises the rich text object's markup (specific formatting, styles and 
pictures). CRichText::HasMarkupData() can be used to test whether the object 
has any markup information.

@param aStream Stream to which to store the rich text markup information. */
	{
	__TEST_INVARIANT;

	CONST_CAST(CRichText*,this)->CancelInsertCharFormat();
	if (!IndexPresent())		
		CONST_CAST(CRichText*,this)->CreateAndGenerateMarkupComponentL();
	aStream<< *iIndex;
	}

EXPORT_C void CRichText::InternalizeMarkupDataL(RReadStream& aStream)
/** Internalises the rich text object's markup (specific formatting, styles and 
pictures).

@param aStream Stream from which to internalise the rich text markup. */
	{
	KillIndex();
	CreateEmptyMarkupComponentL();  // set iIndex::iStyleList
	iIndex->InternalizeL(aStream,iGlobalParaFormatLayer,iGlobalCharFormatLayer,iStyleList.AsPtr());
	}

EXPORT_C void CRichText::StoreMarkupComponentsL(CStreamStore& aStore,CStoreMap& aMap)const
/** Stores all pictures in the rich text object to a stream store.

@param aStore Stream store to which the pictures are to be stored. 
@param aMap A store map. This binds the address of pictures to the stream ID 
of aStore. This is needed to support the deferred loading of pictures. */
	{
	__TEST_INVARIANT;
	
	if (!IndexPresent())
		CONST_CAST(CRichText*,this)->CreateAndGenerateMarkupComponentL();
	iIndex->StorePicturesL(aStore,aMap);
	}


///////////////////////////////////////////////////////////
//
// Utility persist functions
//
///////////////////////////////////////////////////////////

EXPORT_C TBool CRichText::HasMarkupData()const
/** Tests whether the rich text object has any markup, or owns a style list.

Markup is rich text-specific information, for instance specific formatting, 
styles and pictures. It may be stored in a separate stream from the text.

@return ETrue if the rich text object has markup, or owns a style list. EFalse 
if not. */
	{
	if (StyleListPresent() && !StyleListExternallyOwned())
		return ETrue;
	if (IndexPresent())
		return iIndex->HasMarkupData(iGlobalParaFormatLayer);
	else
		return EFalse;
	}




EXPORT_C void CRichText::DetachFromStoreL(CPicture::TDetach aDegree)
/** Loads pictures not already present in memory overloaded function.
Either loads all pictures in the document, or just the pictures located within 
a specified range. Note: In order to load pictures, a picture factory and a 
store resolver must have been set.

@param aDegree There are two possible values: EDetachFull and EDetachDraw.
If you detach using EDetachFull then you can both draw and edit the picture(s),
but if you detach with EDetachDraw then you can only draw them any attempt 
at editing them causes a panic.
@param aPos The start of the range of characters to search.
@param aLength The number of characters in the range. */	
    {
	__ETEXT_WATCH(DETACH);
	
	__TEST_INVARIANT;
	
	if (IndexPresent())
		iIndex->DetachFromStoreL(aDegree,0,DocumentLength());

	__TEST_INVARIANT;

	__ETEXT_WATCH_END(DETACH);
	}


EXPORT_C void CRichText::DetachFromStoreL(CPicture::TDetach aDegree,TInt aPos,TInt aLength)
/** Loads pictures not already present in memory  overloaded function. Either 
loads all pictures in the document, or just the pictures located within a 
specified range. 

Note: In order to load pictures, a picture factory and a store resolver must have 
been set.

@param aDegree There are two possible values: EDetachFull and EDetachDraw. 
If you detach using EDetachFull then you can both draw and edit the picture(s), 
but if you detach with EDetachDraw then you can only draw them  any attempt 
at editing them causes a panic. 
@param aPos The start of the range of characters to search. 
@param aLength The number of characters in the range. */
	{
	__ETEXT_WATCH(DETACH_POS_LENGTH);

	__TEST_INVARIANT;
	TInt documentLength = DocumentLength();
	if (aPos < 0 || aPos > documentLength)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXT_DETACHFROMSTOREL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos >= 0 && aPos <= documentLength,Panic(ECharPosBeyondDocument));
	if (aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_DETACHFROMSTOREL, "ECopyToStreamNegativeLength" );
	    }
	__ASSERT_ALWAYS(aLength >= 0,Panic(ECopyToStreamNegativeLength));
	if (aPos + aLength > documentLength)
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXT_DETACHFROMSTOREL, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos + aLength <= documentLength,Panic(ECharPosBeyondDocument));
	
	if (aLength > 0 && IndexPresent())
		iIndex->DetachFromStoreL(aDegree,aPos,aLength);

	__TEST_INVARIANT;

	__ETEXT_WATCH_END(DETACH_POS_LENGTH);
	}

EXPORT_C void CRichText::SetPictureFactory(MPictureFactory* aPictureFactory,MRichTextStoreResolver* aStoreResolver)
// Sets up a callback for the index.
// Only called if the owner of this rich text component knows they will wanting to restore pictures
// into this rich text.
// If the index component is not yet present, the callbacks are preserved until required.
//
/** Sets the picture factory.

Pictures in a rich text object may be represented as picture headers, or by 
the picture data itself. CRichText supports the deferred loading of picture 
data, so that it is only loaded when it is needed to be displayed, thus 
economising on memory use. A rich text object which supports the deferred 
loading of pictures needs to be supplied with a picture factory. It can either 
be set using this function, or during construction.

Note: A panic occurs if the factory is NULL, but the store resolver is not NULL.

@param aPictureFactory The picture factory. 
@param aStoreResolver The store resolver. This determines which file store 
the picture is stored in. */
	{
	__TEST_INVARIANT;
	if (!aPictureFactory && aStoreResolver)
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_SETPICTUREFACTORY, "EInvalidPictureFactorySettings" );
	    }
	__ASSERT_ALWAYS(!(!aPictureFactory && aStoreResolver),Panic(EInvalidPictureFactorySettings));

	iPictureFactory=aPictureFactory;
	iStoreResolver=aStoreResolver;
	}

EXPORT_C TPictureHeader CRichText::PictureHeader(TInt aPos)const
/** Gets the picture header which describes the picture located at a specified 
document position. If no picture header is located at the position, the function 
constructs and returns a default one.

@param aPos The document position. Must be valid. 
@return The picture header located at document position aPos, or a default 
picture header. */
	{
	__TEST_INVARIANT;
	if (aPos<0 || aPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_PICTUREHEADER, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=DocumentLength(),Panic(ECharPosBeyondDocument));

	return (IndexPresent())
		? iIndex->PictureHeader(aPos)
		: TPictureHeader();
	}

EXPORT_C void CRichText::DropPictureOwnership(TInt aPos)
/** Removes ownership from this rich text object of the picture located at a 
document position. The picture character is deleted from the document and 
ownership of the picture passes to the caller of this function. 
Use PictureHandleL() beforehand to get a pointer to the picture.

@param aPos The document position of the picture character. Must be valid 
or a panic occurs.
@see CRichText::PictureHandleL() */
	{
	__TEST_INVARIANT;
	if (aPos<0 || aPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXT_DROPPICTUREOWNERSHIP, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=DocumentLength(),Panic(ECharPosBeyondDocument));

	TPictureHeader* p = IndexPresent()? iIndex->PictureHeaderPtr(aPos) : 0;
	if (!p)
		return;

	p->iPicture = 0;
	DeleteFromParagraph(aPos, 1);
	}

