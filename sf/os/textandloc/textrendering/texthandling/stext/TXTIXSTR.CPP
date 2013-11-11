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

#include <gdi.h>

#include "TXTFMLYR.H"
#include "TXTMRTSR.H"
#include "TXTRICH.H"
#include "TXTINDEX.H"
#include "TXTSTD.H"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TXTIXSTRTraces.h"
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TXTFMLYR_INTERNAL.H"
#endif

const TUint8 KRegisterItemPresent=1;


GLDEF_C void DiscardPhraseOnCleanup(TAny* aPhrase)
// Allows RPhraseAttribsEntry on the CleanupStack to be destroyed correctly,
// when included as a PushL(TCleanupItem).
//
	{
	((RPhraseAttribsEntry*)aPhrase)->Discard();
	User::Free(aPhrase);
	}


TRtPasteContext::TRtPasteContext(const CStreamStore* aStore,const CParaFormatLayer* aGlobalParaLayer,const CCharFormatLayer* aGlobalCharLayer,const CStyleList* aStyleList)
//
	:iStore(aStore),
	iGlobalParaLayer(aGlobalParaLayer),iGlobalCharLayer(aGlobalCharLayer),iStyleList(aStyleList),
	iParagraphStylesFlag(EFalse),iIncompleteParaFlag(ETrue),iPastePos(),iParasPasted(0)
	{}

//////////////////////////////////////////////////////////////////////////////////////


CRichTextIndex* CRichTextIndex::NewL(const CStreamStore& aStore,TStreamId aId,
//										MPictureFactory* aPictureFactory,MRichTextStoreResolver* aStoreResolver,
										const CParaFormatLayer* aGlobalParaLayer,
										const CCharFormatLayer* aGlobalCharLayer,
										const CRichText& aText)
//										const CStyleList* aStyleList)
//
	{
	CRichTextIndex* self=new(ELeave) CRichTextIndex(aText);
	CleanupStack::PushL(self);
	self->ConstructL(aGlobalParaLayer,aGlobalCharLayer,KMultiParaGranularity,KLargePhraseGranularity);
	self->RestoreL(aStore,aId,aGlobalParaLayer,aGlobalCharLayer,aText.StyleList());
//	self->SetPictureFactory(aPictureFactory,aStoreResolver);
	CleanupStack::Pop();
	return self;
	}


void CRichTextIndex::RestoreL(const CStreamStore& aStore,TStreamId aId,
									   const CParaFormatLayer* aGlobalParaLayer,
									   const CCharFormatLayer* aGlobalCharLayer,
									   const CStyleList* aStyleList)
//
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore,aId);
	//
	InternalizeL(stream,aGlobalParaLayer,aGlobalCharLayer,aStyleList);
	CleanupStack::PopAndDestroy();  // stream
	}


TStreamId CRichTextIndex::StoreMarkupL(CStreamStore& aStore,const CStyleList* /*aStyleList*/) const
// Store this rich text markup out-of-line along with its components.
//
	{
	CStoreMap* map=CStoreMap::NewLC(aStore);
	StorePicturesL(aStore,*map);
//
	RStoreWriteStream stream(*map);
	TStreamId id=stream.CreateLC(aStore);
//	CONST_CAST(CRichTextIndex*,this)->iStyleList=CONST_CAST(CStyleList*,aStyleList);
	ExternalizeL(stream);
	stream.CommitL();
//
	map->Reset();
	CleanupStack::PopAndDestroy(2);  // map,stream
	return id;
	}


void CRichTextIndex::StorePicturesL(CStreamStore& aStore,CStoreMap& aMap) const
// Store any picture components in the specified store.
// Due to deferred loading, any pictures that are not in memory at this point are loaded,
// and then saved.
//
	{
	__TEST_INVARIANT;

	StorePicturesL(aStore,aMap,0,iText.DocumentLength()+1);
	}


void CRichTextIndex::StorePicturesL(CStreamStore& aStore,CStoreMap& aMap,TInt aPos,TInt aLength) const
// Store all picture components that are contained within the specified range (inclusive), in the store provided.
// Prior to storing the picture, its size is written to its picture header which is stored in-line in 
// the markup.
// Any pictures that are not in memory are now deleted, as a previous call to LoadAllPicturesNowL()
// should have been made.
// We are guaranteed that all pictures that are in memory are fully restored & detached from their store
//
	{
	__TEST_INVARIANT;

	if (iPictureCount==0)
		return;
	CONST_CAST(CRichTextIndex*,this)->DetachFromStoreL(CPicture::EDetachFull,aPos,aLength);	// ensure all pictures have been loaded
	TInt currentPos=aPos;
	DocumentChanged();  // reset the cache logical position
	while (currentPos<aPos+aLength)
		{// Store next picture & increment pos.
		CONST_CAST(CRichTextIndex*,this)->ScanToPosition(currentPos,EScanToPositionAbsolute,&MUTABLE_CAST(TLogicalPosition&,iLastUsed));
		TCurrentIndexRecords current;
		GetCurrentRecords(current);
		if (current.iPhrase && current.iPhrase->IsPicturePhrase())
			{
			RPhraseAttribsEntry& phrase=*current.iPhrase;
			TPictureHeader* hdr=phrase.PictureHeaderPtr();
			if (!hdr)
			    {
			    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_STOREPICTURESL, "ERichTextStorePictureIntegrityError" );
			    }
			__ASSERT_DEBUG(hdr,Panic(ERichTextStorePictureIntegrityError));
			TBool pictureInMemory=(hdr->iPicture.IsPtr() && hdr->iPicture.AsPtr());
			if (pictureInMemory)
				{// Store the picture
				CPicture& picture=*hdr->iPicture;
				picture.GetSizeInTwips(hdr->iSize);  //  write picture size to picture header.
				TStreamId id=picture.StoreL(aStore);
				aMap.BindL(hdr->iPicture,id);
				}
			else
				{// Replace the picture phrase with a simple text equivalent.
				CCharFormatLayer* charLayer=phrase.ReleaseCharFormatLayerOwnership();
				phrase.Discard();  // destroy the redundant picture phrase
				RPhraseAttribsEntry newTextPhrase(charLayer,1);
				(*iPhraseIx)[iPos.iPhraseElement]=newTextPhrase;
				CONST_CAST(CRichTextIndex*,this)->iPictureCount--;  // the picture has now been removed
				// I do not bother tidying up the phrase index here,
				// since the benefit does not currently match the cost involved.
				//
				}
			}
		//
		// increment the current position to the *start* of the next phrase/paragraph
		TInt offsetIntoUnit=(current.iPhrase)?iPos.iPhraseElementOffset:iPos.iParaElementOffset;
		currentPos+=CurrentPhraseLength()-offsetIntoUnit;
		}
	}


void CRichTextIndex::DetachFromStoreL(CPicture::TDetach aDegree,TInt aPos,TInt aLength)
// Attempts to restore all pictures not already present in memory.
// A deep (door+document) or shallow (door only) restoration may occur, depending
// on the value of detach.
//
	{
	__TEST_INVARIANT;

	if (iPictureCount==0)
		return;
	DocumentChanged();  // reset the cache logical position
	TInt currentPos=aPos;
	while (currentPos<aPos+aLength)
		{
		CONST_CAST(CRichTextIndex*,this)->ScanToPosition(currentPos,EScanToPositionAbsolute,&iLastUsed);
		TCurrentIndexRecords current;
		GetCurrentRecords(current);
		if (current.iPhrase)
			{
			RPhraseAttribsEntry& phrase=*current.iPhrase;
			if (phrase.IsPicturePhrase())
				{
				TPictureHeader* hdr=phrase.PictureHeaderPtr();
				if (!hdr)
				    {
				    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_DETACHFROMSTOREL, "ERichTextStorePictureIntegrityError" );
				    }
				__ASSERT_DEBUG(hdr,Panic(ERichTextStorePictureIntegrityError));
				//
				if (hdr->iPicture.IsId())
					{
					TRAPD(r,
					phrase.PictureHandleL(iText.PictureFactory(),iText.StoreResolver(),iPos.iDocPos,MLayDoc::EForceLoadTrue));  // swizzles
					if (r!=KErrNone && !hdr->iPicture.IsId())
					    {
					    OstTrace0( TRACE_DUMP, DUP1_CRICHTEXTINDEX_DETACHFROMSTOREL, "ERichTextStorePictureIntegrityError" );    
					    }
					__ASSERT_DEBUG(r==KErrNone || hdr->iPicture.IsId(),Panic(ERichTextStorePictureIntegrityError));
					if (r==KErrNotSupported)  // we don't recognise the picture type
						{
						TInt offsetIntoUnit=(current.iPhrase)?iPos.iPhraseElementOffset:iPos.iParaElementOffset;
						currentPos+=CurrentPhraseLength()-offsetIntoUnit;
						continue;
						}
					
					OstTrace1( TRACE_FATAL, DUP3_CRICHTEXTINDEX_DETACHFROMSTOREL, "Leave code=%d", r );
					User::LeaveIfError(r);
					}
				//
				// recurse the call to detach the picture from the store
				if (!hdr->iPicture.IsPtr())
				    {
				    OstTrace0( TRACE_DUMP, DUP2_CRICHTEXTINDEX_DETACHFROMSTOREL, "ERichTextStorePictureIntegrityError" );
				    }
				__ASSERT_DEBUG(hdr->iPicture.IsPtr(),Panic(ERichTextStorePictureIntegrityError));
				hdr->iPicture->DetachFromStoreL(aDegree);
				}
			}
		//
		// increment the current position to the *start* of the next phrase/paragraph
		TInt offsetIntoUnit=(current.iPhrase)?iPos.iPhraseElementOffset:iPos.iParaElementOffset;
		currentPos+=CurrentPhraseLength()-offsetIntoUnit;
		}
	__TEST_INVARIANT;
	}

//////////////////////////////////////////////////////////////////////////////////////

void CRichTextIndex::ExternalizeL(RWriteStream& aStream)const
// Persist the current state of this object in the specified stream.
// Saved records include the terminating paragraph delimiter. 
//
//
	{
	__TEST_INVARIANT;

	CONST_CAST(CRichTextIndex*,this)->CancelInsertCharFormat();  // cancel any insert-pending state.
	TInt charCount=iText.DocumentLength()+1;  // includes the end of document character
	((CRichTextIndex*)this)->ScanToPosition(0,EScanToPositionAbsolute);
	TLogicalPosition startPos=iPos;
	((CRichTextIndex*)this)->ScanToPosition(charCount-1,EScanToPositionMatchLeft);
	TLogicalPosition endPos=iPos;
	CStyleList* styleList=iText.StyleList();
	ExternalizeRtiHeaderL(aStream,endPos,styleList);
	ExternalizeSharedFormatsL(aStream,startPos,endPos,styleList);
	// DEF126934 begin: memory leak
	RPhraseAttribsEntry *pForRelease = ExternalizeParaIxL(aStream,startPos,endPos,styleList);
	if (pForRelease)
		delete pForRelease;
	// DEF126934 end
	ExternalizePhraseIxL(aStream);
	}


void CRichTextIndex::InternalizeL(RReadStream& aStream,const CParaFormatLayer* aGlobalParaLayer,const CCharFormatLayer* aGlobalCharLayer,const CStyleList* aStyleList)
// Load a rich text index.  Has construct semantics only!
// Restores this object from the specified stream.  As specific format layers are restored, they are based
// on the specified global format layers. 
//
	{
	if (iParaIx->Count()!=1 || (*iParaIx)[0].iLength!=1)
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXTINDEX_INTERNALIZEL, "ERtIndexInternalizeCalledOnNonEmptySource" );
	    }
	__ASSERT_ALWAYS(iParaIx->Count()==1 && (*iParaIx)[0].iLength==1,Panic(ERtIndexInternalizeCalledOnNonEmptySource));
	TRtPasteContext context(NULL,aGlobalParaLayer,aGlobalCharLayer,aStyleList);
	InternalizeRtiHeaderL(aStream,context);
	InternalizeSharedFormatsL(aStream,context);
	InternalizeParaIxL(aStream,context);
	InternalizePhraseIxL(aStream,aGlobalCharLayer);
	if (context.iParagraphStylesFlag)
		GenerateAllPhraseLinksL();
	SetSpecificMarkupInternalized(ETrue);

	__TEST_INVARIANT;
	}

//////////////////////////////////////////////////////////////////////////////////////


void CRichTextIndex::GenerateAllPhraseLinksL()
// Go through the index, and for each specific character format layer (shared or otherwise), regenerate
// the appropriate based-on link, (normal or style).
// 
	{
	TInt count=ParagraphCount();
	TInt phraseElement=0;
	for (TInt ii=0; ii<count; ii++)
		{
		const CParaAttribs& paraAttribs=*(*iParaIx)[ii].iParaAttribs;
		const CParaFormatLayer* base=STATIC_CAST(const CParaFormatLayer*,paraAttribs.iParaFormat->SenseBase());
		TInt phraseCount=paraAttribs.PhraseCount();
		if (phraseCount==1)
			{
			// If the reference count is zero, iCharFormat cannot be valid, so the CParaAttribs object is corrupt.
			if (paraAttribs.iRefCount == 0)
				User::Leave(KErrCorrupt);

			GeneratePhraseLink(paraAttribs.iCharFormat,base);  // constant character formatting
			}
		else
			{
			if (phraseCount<=1)
			    {
			    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_GENERATEALLPHRASELINKSL, "EDebug" );
			    }
			 __ASSERT_DEBUG(phraseCount>1,Panic(EDebug));

			// If the phrase count is too great the CParaAttribs object is corrupt.
			if (phraseElement + phraseCount > iPhraseIx->Count())
				User::Leave(KErrCorrupt);

			for (TInt nn=0; nn<phraseCount; nn++)
				GeneratePhraseLink((*iPhraseIx)[phraseElement+nn].CharFormat(),base);
			phraseElement+=phraseCount;
			}
		}
	}


void CRichTextIndex::GeneratePhraseLink(CCharFormatLayer* aPhraseCharFormatLayer,const CParaFormatLayer* aBase)
// Set the based on link for this character format layer appropriately.
// By default, all specific character format layers are based on the global default.
//
	{
	TUid type=aBase->Type();
	if (type!=KNormalParagraphStyleUid)
		aPhraseCharFormatLayer->SetBase(((CParagraphStyle*)aBase)->CharFormatLayer());
	}


//////////////////////////////////////////////////////////////////////////////////////

void CRichTextIndex::CopyToStreamL(RWriteStream& aStream,TInt aPos,TInt aLength, TBool aCopyStyles)const
// Copies the markup corresponding to the specified rich text region to the specified write stream.
// aPos/aLength has already been validated in RichText
// Saved records can NOT include the terminating paragraph delimiter.
//
	{
	__TEST_INVARIANT;

	((CRichTextIndex*)this)->ScanToPosition(aPos,EScanToPositionAbsolute);
	TLogicalPosition startPos=iPos;
	((CRichTextIndex*)this)->ScanToPosition((aPos+aLength)-1,EScanToPositionAbsolute);
	TLogicalPosition endPos=iPos;

	CStyleList* docStyleList = NULL;
	if (aCopyStyles)
		docStyleList = (CStyleList*)iText.StyleList();

	ExternalizeRtiHeaderL(aStream,endPos,docStyleList);
	
	if (docStyleList)
		ExternalizeReferencedStylesL(aStream,startPos,endPos);
	
	ExternalizeSharedFormatsL(aStream,startPos,endPos,docStyleList); 
	RPhraseAttribsEntry* virtualTrailingText=ExternalizeParaIxL(aStream,startPos,endPos,docStyleList);
	if (virtualTrailingText)
	    CleanupStack::PushL(virtualTrailingText);
	ExternalizePhraseIxL(aStream,startPos,endPos,virtualTrailingText);
	if (virtualTrailingText)
	    CleanupStack::PopAndDestroy(virtualTrailingText);
	}

//////////////////////////////////////////////////////////////////////////////////////


void CRichTextIndex::ExternalizeRtiHeaderL(RWriteStream& aStream,const TLogicalPosition& aEnd,const CStyleList* aStyleList)const
// Stores index specific information.
//
	{
	TBool incompleteParaFlag=((*iParaIx)[aEnd.iParaElement].iLength>(aEnd.iParaElementOffset+1));
	aStream.WriteUint8L((TUint8)(aStyleList!=NULL));  // flags the presence of paragraph styles.
	aStream.WriteUint8L((TUint8)(incompleteParaFlag!=EFalse)); // the last para has no paragraph delimiter
	if(incompleteParaFlag)
		{//apply paragraph format to final text fragment if logical end is at document end
		aStream.WriteUint8L((TUint8)(aEnd.iDocPos+1 == iText.DocumentLength()));
		}
	}


void CRichTextIndex::InternalizeRtiHeaderL(RReadStream& aStream,TRtPasteContext& aContext)
// Load index specific information.
//
	{
	TUint8 flag=0;
	aStream>> flag;
	aContext.iParagraphStylesFlag=(TBool)flag;
	aStream>> flag;
	aContext.iIncompleteParaFlag=(TBool)flag;
	aContext.iApplyFormatToLastFlag=EFalse;
	if(aContext.iIncompleteParaFlag)
		{
		aStream>> flag;
		aContext.iApplyFormatToLastFlag=(TBool)flag;
		}
	}

//////////////////////////////////////////////////////////////////////////////////////
                                                                                                                     
void CRichTextIndex::ExternalizeReferencedStylesL(RWriteStream& aStream,const TLogicalPosition& aStart,
												  const TLogicalPosition& aEnd) const
// Write those styles that are referenced by the paragraphs in the range aStart to aEnd.
//
	{
	CStyleList* list = iText.StyleList();
	if (!list)
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_EXTERNALIZEREFERENCEDSTYLESL, "EStyleClipboardIntegrityError" );
	    }
	__ASSERT_DEBUG(list,Panic(EStyleClipboardIntegrityError));
	if (aStart.iParaElement > aEnd.iParaElement)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CRICHTEXTINDEX_EXTERNALIZEREFERENCEDSTYLESL, "EStyleClipboardIntegrityError" );
	    }
	__ASSERT_DEBUG(aStart.iParaElement <= aEnd.iParaElement,Panic(EStyleClipboardIntegrityError));

	TUint8 numStyles = 0;
	if (list) 
		numStyles = (TUint8)list->Count();

	HBufC8* buf=HBufC8::NewL(numStyles);
	TPtr8 paraRegister=buf->Des();
	paraRegister.FillZ(numStyles);
	TInt refCount=MarkStyleRegister(paraRegister,aStart.iParaElement,aEnd.iParaElement);
	CleanupStack::PushL(buf);
	ExternalizeItemsPresentInStyleRegisterL(aStream,refCount,paraRegister);
	CleanupStack::PopAndDestroy();
	}


void CRichTextIndex::InternalizeSharedFormatsL(RReadStream& aStream, const TRtPasteContext& aContext)
// Restores the list of shared para attribs.
// All existing CParaAttribs on the shared list are removed.
// The reference counts are not restored, but modified dynamically as the para index is loaded.
//
	{
	iParaIx->Reset();  // stops paraIx referencing shared paraAttribs that are about to be released.
	CParaAttribs* currentSharedPara;
	TDblQueIter<CParaAttribs> iterator(iSharedParaQueHead);
	while ( (currentSharedPara = iterator++) != NULL )
		currentSharedPara->Release(currentSharedPara->iRefCount);
	TUint8 sharedParaCount = ReadTUint8CountL(aStream);
	for (TUint8 paraItem = 0; paraItem < sharedParaCount; paraItem++)
		{
		(void)aStream.ReadInt32L();  // consume the data.
		CParaFormatLayer* paraLayer = InternalizeParagraphFormatL(aStream, aContext);
		CleanupStack::PushL(paraLayer);

		CCharFormatLayer* charLayer = CCharFormatLayer::NewL(aStream);  // Does not restore based on link.
		charLayer->SetBase(aContext.iGlobalCharLayer);  // base on 'Normal' global char format
		CleanupStack::PushL(charLayer);
		CParaAttribs* paraAttribs = CParaAttribs::NewL(paraLayer, charLayer);
		CleanupStack::PopAndDestroy(2);
		paraAttribs->iRefCount = 0;  // Otherwise recalculating reference counts on internalization of paraIx will result in 1 too many.
		iSharedParaQueHead.AddLast(*paraAttribs);
		}
	}


void CRichTextIndex::ExternalizeSharedFormatsL(RWriteStream& aStream,const TLogicalPosition& aStart,const TLogicalPosition& aEnd,const CStyleList* aStyleList)const
// Persist those items in the shared list that are referenced by the paragraphs in the 
// range aStart para to aEnd para.
//
	{
	TUint8 sharedParaCount=SharedParaAttribsEntryCountL();
	// Some paras coverd, so take register
	HBufC8* buf=HBufC8::NewL(sharedParaCount);
	TPtr8 sharedAttribsRegister=buf->Des();
	sharedAttribsRegister.FillZ(sharedParaCount);
	TInt sharedCount=MarkRegister(sharedAttribsRegister,aStart.iParaElement,aEnd.iParaElement);
	CleanupStack::PushL(buf);
	ExternalizeItemsPresentInRegisterL(aStream,sharedCount,sharedAttribsRegister,aStyleList);
	CleanupStack::PopAndDestroy();
	}


TUint8 CRichTextIndex::SharedParaAttribsEntryCountL()const
// Return a count of the number of items currently
// in the sharedParaFormats list.
//
// This will leave if more than 255 found, as this is not supported
	{
	TDblQueIter<CParaAttribs> iterator(((CRichTextIndex*)this)->iSharedParaQueHead);
	TUint16 sharedParaCount = 0;
	while (iterator++ != NULL)
		sharedParaCount++;
		
	// Currently more than 255 paragraph formats are not supported 
	// If the current document has more, leave with KErrNotSupported
	if (sharedParaCount > 255)
		User::Leave(KErrNotSupported);
	
	return (TUint8)sharedParaCount;
	}


TInt CRichTextIndex::MarkRegister(TDes8& aBuf,TInt aStartPara,TInt aEndPara)const
// Indicate in the register, aBuf, once and once only, each paraAttribs that is
// referenced by each of the paragraphs in the specified range.
// Return the number of shared para formats referenced.
//
	{
	TInt parasReferenced=0;
	for (TInt item=aStartPara;item<=aEndPara;item++)
		{
		CParaAttribs* paraAttribs=(*iParaIx)[item].iParaAttribs;
		TInt offset=RefNum(paraAttribs);  // 0 return shows para attribs not in shared list
		if (offset>0)
			{// para entry references a shared para attribs
			if (aBuf[offset-1]==0)
				{
				aBuf[offset-1]=KRegisterItemPresent;  // mark item as needing to be stored
				parasReferenced++;
				}
			}
		}
	return parasReferenced;
	}


TInt CRichTextIndex::MarkStyleRegister(TDes8& aBuf,TInt aStartPara,TInt aEndPara)const
// Indicate in the register, aBuf, once and once only, each paragraph style thar is
// referenced by each of the paragraphs in the speciifed range.
// Return the umber of paragraph styles referenced.
//
	{
	TInt stylesReferenced=0;
	const CParaAttribs* para=NULL;
	for (TInt item=aStartPara;item<=aEndPara;item++)
		{
		para=(*iParaIx)[item].iParaAttribs;
		TInt index=iText.StyleList()->IndexByPtr(STATIC_CAST(const CParaFormatLayer*,para->iParaFormat->SenseBase()));
		if (index!=KErrNotFound)
			{
			if (index>=aBuf.Length())
			    {
			    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_MARKSTYLEREGISTER, "EStyleClipboardIntegrityError" );
			    }
			__ASSERT_DEBUG(index<aBuf.Length(),Panic(EStyleClipboardIntegrityError));
			if (aBuf[index]!=KRegisterItemPresent)
				{
				aBuf[index]=KRegisterItemPresent;  // mark item as needing to be stored
		
				stylesReferenced++;
				}
			}
		}
	return stylesReferenced;
	}


void CRichTextIndex::ExternalizeItemsPresentInRegisterL(RWriteStream& aStream,TInt aSharedCount,const TDes8& aBuf,const CStyleList* aStyleList)const
// Externalize each object from the shared list that has a corresponding mark in the
// register, aBuf.
//
	{
	if (aSharedCount>(TInt)KMaxTUint8)
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_EXTERNALIZEITEMSPRESENTINREGISTERL, "ESharedParaCountStreamOverflow" );
	    }
	__ASSERT_DEBUG(aSharedCount<=(TInt)KMaxTUint8,Panic(ESharedParaCountStreamOverflow));
	aStream.WriteUint8L(aSharedCount);
	CParaAttribs* currentSharedPara=NULL;
	TDblQueIter<CParaAttribs> iterator(((CRichTextIndex*)this)->iSharedParaQueHead);
	TInt offset=0;
	while ((currentSharedPara=iterator++)!=NULL)
		{
		if (aBuf[offset]==KRegisterItemPresent)
			{
			aStream.WriteInt32L(offset+1);  // the ref-no of the shared para - so it can be spotted in the paste.
			ExternalizeParagraphFormatL(aStream,*currentSharedPara->iParaFormat,aStyleList);
			aStream<< *currentSharedPara->iCharFormat;
			}
		offset++;
		}
	}


void CRichTextIndex::ExternalizeItemsPresentInStyleRegisterL(RWriteStream& aStream,TInt aRefStyleCount,
															 const TDes8& aBuf) const
// Externalize each object from the paragraph style list that has a corresponding mark in the register aBuf.
//
	{
	if (aRefStyleCount > (TInt)KMaxTUint8)
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_EXTERNALIZEITEMSPRESENTINSTYLEREGISTERL, "EStyleClipboardIntegrityError" );
	    }
	__ASSERT_DEBUG(aRefStyleCount <= (TInt)KMaxTUint8,Panic(EStyleClipboardIntegrityError));
	aStream.WriteUint8L(aRefStyleCount);
	TInt count=aBuf.Length();

	for (TInt ii=0;ii<count;ii++)
		{
		if (aBuf[ii]==KRegisterItemPresent)
			{
			aStream.WriteInt8L(ii+1);  // cannot use 0 index as this represents an absence.
			iText.StyleList()->At(ii).iStyle->ExternalizeL(aStream);
			}
		}
  }


RPhraseAttribsEntry* CRichTextIndex::ExternalizeParaIxL(RWriteStream& aStream,
										const TLogicalPosition& aStart,
										const TLogicalPosition& aEnd,
										const CStyleList* aStyleList)const
// Externalize each para entry from the para index that falls between 
// aStart paragraph and aEnd paragraph inclusive.
// Any specific paraAttribs (ie not on the shared list) are also externalized.
// * paragraph count
// * 	paragraph length
// *	ref no.  (ref no. 0 means specific/non-shared one)
// *	[specific paragraph format layer - including based-on link]
// *	[specific phrase count]
//
	{
	TParaAttribsEntry lastParaEntry=(*iParaIx)[aEnd.iParaElement];
	TBool incompleteParaFlag=(lastParaEntry.iLength>(aEnd.iParaElementOffset+1));
	aStream.WriteInt32L(1+aEnd.iParaElement-aStart.iParaElement);  // paragraph count
	for (TInt paraItem=aStart.iParaElement;paraItem<aEnd.iParaElement;paraItem++)
		{// Externalize all but last TParaAttribsEntrys from the para index
		TParaAttribsEntry paraEntry=(*iParaIx)[paraItem];
		TInt phraseCount=(paraEntry.iParaAttribs->IsShared()) 
			? 0 
			: paraEntry.iParaAttribs->iPhraseCount;
		if (paraItem==aStart.iParaElement)
			{// Fix & write length & phrase count of the first paragraph.
			paraEntry.iLength=(paraEntry.iLength-aStart.iParaElementOffset);  // fix 1st para length
			if (phraseCount>0)
				phraseCount=phraseCount-(aStart.iPhraseElement-aStart.iParaBasePhraseElement);
			}
		aStream.WriteInt32L(paraEntry.iLength);
		CParaAttribs* paraAttribs=paraEntry.iParaAttribs;
		TUint8 refNo=RefNum(paraAttribs);
		aStream.WriteUint8L(refNo);
		if (refNo==0)
			{// Write specific para layer & phrase count
			ExternalizeParagraphFormatL(aStream,*paraAttribs->iParaFormat,aStyleList);
			aStream.WriteInt32L(phraseCount);
			}
		}
	// Fix & write the length of the last paragraph.
	if (aStart.iParaElement==aEnd.iParaElement)
		lastParaEntry.iLength=(aEnd.iParaElementOffset-aStart.iParaElementOffset)+1;  // copied text contained within 1 para
	else
		lastParaEntry.iLength-=(lastParaEntry.iLength-aEnd.iParaElementOffset)-1;
	aStream.WriteInt32L(lastParaEntry.iLength);
	// Fix & write the phrase count of the last paragraph if it has specific char format
	CParaAttribs* lastParaAttribs=lastParaEntry.iParaAttribs;
	TUint8 refNo=RefNum(lastParaAttribs);
	RPhraseAttribsEntry* virtualTextPhrase=NULL;
	if (incompleteParaFlag)
		{// Adjust the phrase count for this paragraph that is not complete
		TInt phraseCount=0;
		if (aStart.iParaElement==aEnd.iParaElement)
			phraseCount=(aEnd.iPhraseElement-aStart.iPhraseElement)+1;  // copied text contained within 1 para
		else
			phraseCount=(aEnd.iPhraseElement-aEnd.iParaBasePhraseElement)+1;
		if (phraseCount>lastParaAttribs->iPhraseCount)
		    {
		    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_EXTERNALIZEPARAIXL, "ERtExternalizeParaIx" );
		    }
		__ASSERT_DEBUG(phraseCount<=lastParaAttribs->iPhraseCount,Panic(ERtExternalizeParaIx));
		aStream.WriteInt32L(phraseCount);
		if (refNo>0)
			{// Set the virtual phrase representing the trailing text from the shared paragraph.
			virtualTextPhrase=new(ELeave) RPhraseAttribsEntry(lastParaAttribs->iCharFormat,lastParaEntry.iLength);
			CleanupStack::PushL(virtualTextPhrase);
			}

		// Write out the paragraph format for the paragraph that is not complete
		// - necessary since we need to patch up the style character formatting
		if (aStyleList || aEnd.iDocPos+1 == iText.DocumentLength())
			ExternalizeParagraphFormatL(aStream, *lastParaAttribs->iParaFormat, aStyleList);
		}
	else
		{// This is a complete para - copy region ends on a paragraph delimiter.
		aStream.WriteUint8L(refNo);
		if (refNo==0)
			{// Write specific para layer & phrase count
			ExternalizeParagraphFormatL(aStream,*lastParaAttribs->iParaFormat,aStyleList);
			TInt phraseCount=lastParaAttribs->iPhraseCount;
			if (aStart.iParaElement==aEnd.iParaElement && aStart.iParaElementOffset && aStart.iParaElementOffset>0)
				phraseCount=(aEnd.iPhraseElement-aStart.iPhraseElement)+1;  // copied text contained within 1 para
			aStream.WriteInt32L(phraseCount);
			}
		}
	if (virtualTextPhrase)
	    CleanupStack::Pop(virtualTextPhrase);
	return virtualTextPhrase;
	}


void CRichTextIndex::ExternalizeParagraphFormatL(RWriteStream& aStream,const CParaFormatLayer& aLayer,const CStyleList* aStyleList)const
// If a style list is present, write out the based-on links for every paragraph format layer,
// in order to reconnect each paragraph to the right style on loading.
//
	{
	aStream<< aLayer;
	if (aStyleList)
		{
		TInt refNo=0;
		refNo=aStyleList->IndexByPtr(((CParaFormatLayer*)aLayer.SenseBase()));
		// returns the offset in the style list, or a negative error.
		//
		refNo=(refNo!=KErrNotFound)
			? refNo+1  // the nth style where 1st item is offset 1.
			: 0;  // An error value was returned so this thing has no paragraph style as its base
		aStream.WriteInt8L((TInt8)-refNo);
		}
 	}


CParaFormatLayer* CRichTextIndex::InternalizeParagraphFormatL(RReadStream& aStream,const TRtPasteContext& aContext)
// 
	{
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL(aStream);  // specific paragraph format layer
	TUint index=0;
	if (aContext.iParagraphStylesFlag)
		{
		CleanupStack::PushL(paraLayer);
		TInt refNo=aStream.ReadInt8L();  // Read in the based on link to fix up to the style table
		CleanupStack::Pop();
		index=Abs(refNo);
		}
	if (index>0 && aContext.iStyleList)
		paraLayer->SetBase(aContext.iStyleList->At(index-1).iStyle);
	else
		paraLayer->SetBase(aContext.iGlobalParaLayer);
	return paraLayer;
	}


CParaFormatLayer* CRichTextIndex::PasteParagraphFormatL(RReadStream& aStream,const TRtPasteContext& aContext,CStyleMap* aStyleMap)
// 
	{
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL(aStream);  // specific paragraph format layer
 
	TUint index=0;
	if (aContext.iParagraphStylesFlag)
		{
		CleanupStack::PushL(paraLayer);
		TInt refNo=aStream.ReadInt8L();  // Read in the based on link to fix up to the style table
		CleanupStack::Pop(paraLayer);	// paraLayer
		index=Abs(refNo);
		}

	// If a style exists for this paragraph, set it as the base.
	// If a style list doesn't exist, the base paragraph style will be set instead.
	
	if (index>0 && aContext.iStylePasteMode != CParagraphStyle::EIgnoreNewStyles)
	{
		if (!aContext.iStyleList || (aContext.iStyleList && aContext.iStylePasteMode == CParagraphStyle::EConvertNewStyles))
		{
			CleanupStack::PushL(paraLayer);
			iLastCharacterStyle = (aStyleMap->Item(index))->iCharFormatLayer;

			// If the application doesn't have a style list, 
			// the new paragraph style has to be enforced by merging the old
			// paralayer with the style paralayer
			CParaFormat* MergedParaFormat = CParaFormat::NewLC();
			TParaFormatMask MergedParaFormatMask;

			CParaFormatLayer* newParaLayer;

			// Extract the masks from both layers
			paraLayer->SenseL(MergedParaFormat, MergedParaFormatMask);
			(aStyleMap->Item(index))->SenseL(MergedParaFormat, MergedParaFormatMask);

			newParaLayer = CParaFormatLayer::NewL(MergedParaFormat, MergedParaFormatMask);

			// Now on stack: MergedParaFormat, paraLayer, ...
			CleanupStack::PopAndDestroy();	// delete MergedParaFormat

			// Now on stack: paraLayer, ...
			CleanupStack::PopAndDestroy();	// paraLayer
			paraLayer = newParaLayer;

			paraLayer->SetBase(aContext.iGlobalParaLayer);

		}
		else 
		{
			paraLayer->SetBase(aStyleMap->Item(index));		
			iLastCharacterStyle = NULL;
		}
	}
	else 
	// Otherwise set the character style to NULL, which will set the global
	// character style (see line 983 approx)
	{
		iLastCharacterStyle = NULL;
		paraLayer->SetBase(aContext.iGlobalParaLayer);
	}
		
	return paraLayer;
	}


void CRichTextIndex::InternalizeParaIxL(RReadStream& aStream,const TRtPasteContext& aContext)
// Restore the paragraph index & all associated specific para formats,
// All existing index data is lost.
//
	{
	// The para index has been reset at the start of internalize shared formats.
	TInt paraIxCount=aStream.ReadInt32L();  // paragraph count
	for (TInt currentPara=0;currentPara<paraIxCount;currentPara++)
		{// Restore each paragraph
		TParaAttribsEntry para;
		para.iLength=aStream.ReadInt32L();  // paragraph length
		TUint8 refNo=aStream.ReadUint8L();  // ref no
		if (refNo>0)
			{// Link to para attribs in shared list & up its reference count
			para.iParaAttribs=SharedParaAttribs(refNo);
			para.iParaAttribs->iRefCount++;
			iParaIx->AppendL(para);
			}
		else
			{// Have to build up the specific para attribs
			CParaFormatLayer* paraLayer=InternalizeParagraphFormatL(aStream,aContext);
			CleanupStack::PushL(paraLayer);
			CParaAttribs* specificParaAttribs=CParaAttribs::NewL(paraLayer);
				CleanupStack::PushL(TCleanupItem(ReleaseOnCleanup,specificParaAttribs));
			specificParaAttribs->iPhraseCount=aStream.ReadInt32L();  // specific phrase count
				CleanupStack::Pop();	// specificParaAttribs
			CleanupStack::PopAndDestroy(); // paraLayer
			para.iParaAttribs=specificParaAttribs;
			CleanupStack::PushL(TCleanupItem(ReleaseOnCleanup,specificParaAttribs));
			iParaIx->AppendL(para);
			CleanupStack::Pop();  // specificParaAttribs
			}
		}
	}

void CRichTextIndex::ExternalizePhraseIxL(RWriteStream& aStream)const
// Called from an Externalize.
// All phrases must be saved.
//
	{ExternalizePhrasesL(aStream,0,PhraseCount());}


void CRichTextIndex::ExternalizePhraseIxL(RWriteStream& aStream,
										  const TLogicalPosition& aStart,
										  TLogicalPosition& aEnd,
										  const RPhraseAttribsEntry* aVirtualTrailingText)const
// Called from CopyToStream.
// Here, a text fragment only, is to be saved.
// The last argument may be NULL.  If it is other than NULL, it represents trailing text
// that has no paragraph delimiter, which was part of a shared paragraph.  This must be presented as
// a distinct phrase, since this is the context in which it is pasted back into a document.
//
	{
	TInt phraseCount=PhraseCount();
	phraseCount+=(aVirtualTrailingText) ? 1 : 0;
	TBool noRealPhrasesInRegion=ETrue;
	TInt nn=aStart.iParaElement;
	while (nn<=aEnd.iParaElement && noRealPhrasesInRegion)
		{
		if (!(*iParaIx)[nn].iParaAttribs->IsShared())
			noRealPhrasesInRegion=EFalse;
		nn++;
		}
	//
	if (phraseCount==0 || (phraseCount==1 && aVirtualTrailingText || noRealPhrasesInRegion))  // save this phrase count
		ExternalizePhrasesL(aStream,aStart.iPhraseElement,0,aVirtualTrailingText);
	else
		{
		TBool endParaShared=(*iParaIx)[aEnd.iParaElement].iParaAttribs->IsShared();
		if (endParaShared && aEnd.iPhraseElement>0)
			aEnd.iPhraseElement--;  // Due to action of ScanToPosition
		//
		RPhraseAttribsEntry* backPhrase=&(*iPhraseIx)[aEnd.iPhraseElement];
		TInt backLength=backPhrase->Length();
		if (!(endParaShared || backPhrase->IsPicturePhrase()))
			backPhrase->SetLength(aEnd.iPhraseElementOffset+1);
		//
		RPhraseAttribsEntry* frontPhrase=&(*iPhraseIx)[aStart.iPhraseElement];
		TInt frontLength=frontPhrase->Length();
		if (!frontPhrase->IsPicturePhrase())  // Fix length of first phrase.
			frontPhrase->SetLength(frontLength-aStart.iPhraseElementOffset);
		TRAPD(ret,
			ExternalizePhrasesL(aStream,aStart.iPhraseElement,aEnd.iPhraseElement-aStart.iPhraseElement+1,aVirtualTrailingText));
		//
		// Now fix the altered phrase lengths.
		if (!frontPhrase->IsPicturePhrase())
			frontPhrase->SetLength(frontLength);
		if (!(endParaShared || backPhrase->IsPicturePhrase()))
			backPhrase->SetLength(backLength);

		__TEST_INVARIANT;	// we lied about being const

		User::LeaveIfError(ret);
		}
	}


void CRichTextIndex::ExternalizePhrasesL(RWriteStream& aStream,TInt aStart,TInt aPhraseCount,
										 const RPhraseAttribsEntry* aVirtualPhrase)const
// Save the specified number of phrases present in the phrase index,
// starting from phrase offset aStart.
//
	{
	ExternalizePhraseCountL(aStream,aPhraseCount+ ((aVirtualPhrase)?1:0) );
	for (TInt phraseItem=aStart;phraseItem<aStart+aPhraseCount;phraseItem++)
		{
		RPhraseAttribsEntry& phraseEntry=(*iPhraseIx)[phraseItem];
		aStream<< phraseEntry;
		}
	if (aVirtualPhrase)
		aStream<< *aVirtualPhrase;
	}


void CRichTextIndex::InternalizePhraseIxL(RReadStream& aStream,const CCharFormatLayer* aGlobalCharFormat)
// Load all the phrases from the stream, and insert them into the phrase index
//
	{
	iPhraseIx->Reset();
	TInt phraseCount=aStream.ReadInt32L();

	// Extend phrase index by required amount
	iPhraseIx->AppendL(RPhraseAttribsEntry(),phraseCount);
	
	for (TInt phraseItem=0;phraseItem<phraseCount;phraseItem++)
		{// Restore each phrase & insert into the phrase index.
		TBool isPicture=(TBool)aStream.ReadUint8L();
		TInt phraseLength=aStream.ReadInt32L();
		CCharFormatLayer* charLayer=CCharFormatLayer::NewL(aStream);
		charLayer->SetBase(aGlobalCharFormat);
		RPhraseAttribsEntry& tPhrase=iPhraseIx->At(phraseItem);
		if (!isPicture)
			new(&tPhrase) RPhraseAttribsEntry(charLayer,phraseLength);
		else
			{// Manufacture new picture header & set its picture store
			CleanupStack::PushL(charLayer);
			TPictureHeader hdr;
			aStream>> hdr;
			TBool ownershipTaken(EFalse);
			CPicturePhrase* picPhrase=CPicturePhrase::NewL(hdr,charLayer,ownershipTaken);
			CleanupStack::Pop();  // char layer
			new(&tPhrase) RPhraseAttribsEntry(picPhrase);
			iPictureCount++;
			}
		}
	}


CParaAttribs* CRichTextIndex::SharedParaAttribs(TUint8 aOrdinal)
// Return the handle of the para attribs in the shared para attribs list,
// whose position in the list is specified by the argument aOrdinal.
//
	{
	CParaAttribs* currentSharedPara;
	TDblQueIter<CParaAttribs> iterator(iSharedParaQueHead);
	TInt match=1;
	while ((currentSharedPara=iterator++)!=NULL && aOrdinal!=match)
		match++;
	if (currentSharedPara==NULL)
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXTINDEX_SHAREDPARAATTRIBS, "EEndOfSharedParaListEncountered" );
	    }
	__ASSERT_ALWAYS(currentSharedPara!=NULL,Panic(EEndOfSharedParaListEncountered));
	return currentSharedPara;
	}


TUint8 CRichTextIndex::RefNum(const CParaAttribs* aParaAttribs)const
// If the para attribs argument is present in the shared list, return a 
// reference to it; otherwise return zero as the reference.
//
	{
	CParaAttribs* currentSharedPara;
	TDblQueIter<CParaAttribs> iterator(((CRichTextIndex*)this)->iSharedParaQueHead);
	TUint8	 refNo=1;
	while ((currentSharedPara=iterator++)!=NULL)
		{
		if (currentSharedPara==aParaAttribs)
			return refNo;
		refNo++;
		}
	return 0;
	}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////

void CRichTextIndex::PasteFromStreamL(const CStreamStore& aStore,RReadStream& aStream,TInt aPos,CParagraphStyle::TStylePasteMode aStylePasteMode,const CParaFormatLayer* aGlobalParaLayer,const CCharFormatLayer* aGlobalCharLayer)
//
	{
	__TEST_INVARIANT;
	
	CancelInsertCharFormat();
	ScanToPosition(aPos,EScanToPositionAbsolute);

	//
	//  Get the header
	TRtPasteContext context(&aStore,aGlobalParaLayer,aGlobalCharLayer,iText.StyleList());
	context.iPastePos=iPos;
	context.iStylePasteMode = aStylePasteMode;

	InternalizeRtiHeaderL(aStream,context);  // no rollback required on the header.  Does not alter iPos.
	//
	// Get the pargraph styles

	CStyleMap* styleMap = NULL;
	if (context.iParagraphStylesFlag)
	{
		TUint8 styleCount=ReadTUint8CountL(aStream);
		styleMap=CStyleMap::NewLC(styleCount);
		TRAPD(ret,
		PasteStylesL(aStream,*styleMap,context));  // Does not alter iPos.
		if (ret!=KErrNone) {RbPasteSharedFormatsL(ret);}
	}
	//
	// Get the shared formats
	TUint8 sharedParaCount=ReadTUint8CountL(aStream);
	CParaAttribsMap* paraMap=CParaAttribsMap::NewLC(sharedParaCount);
	TRAPD(ret,
	PasteSharedFormatsL(aStream,*paraMap,context,styleMap));  // Does not alter iPos.
	if (ret!=KErrNone)
		{
		// beginning of fixing DEF 126651 (1/2)
		// free orphan object before leave.
		if (context.iParagraphStylesFlag)
		{
			if (!context.iStyleList || context.iStylePasteMode == CParagraphStyle::EConvertNewStyles)
			{
			TInt maxSize = styleMap->Count();
			for (TInt count=0; count<maxSize; count++)
				{
				delete (CParagraphStyle*)styleMap->At(count).iT;	
				}
			}
			CleanupStack::PopAndDestroy();	// cleanup styleMap
		}
		CleanupStack::PopAndDestroy();  // cleanup paraMap
		// end of fixing DEF 126651 (1/2)
		RbPasteSharedFormatsL(ret);
		}
	//
	// Get the markup
	TRAP(ret,
	PasteIxL(aStream,context,*paraMap, styleMap));  // context now has both global layers & pastePos
	if (ret!=KErrNone)
		{
		// beginning of fixing DEF 126651 (2/2)
		// free orphan object before leave.
		if (context.iParagraphStylesFlag)
		{
			if (!context.iStyleList || context.iStylePasteMode == CParagraphStyle::EConvertNewStyles)
			{
			TInt maxSize = styleMap->Count();
			for (TInt count=0; count<maxSize; count++)
				{
				delete (CParagraphStyle*)styleMap->At(count).iT;	
				}
			}
			CleanupStack::PopAndDestroy();	// cleanup styleMap
		}
		CleanupStack::PopAndDestroy();  // cleanup paraMap
		// end of fixing DEF 126651 (2/2)
		RbPasteSharedFormatsL(ret);
		}


	if (context.iParagraphStylesFlag)
	{
		TRAP(ret,GenerateAllPhraseLinksL());
		if (ret != KErrNone)
			RbPasteSharedFormatsL(ret);

		if (!context.iStyleList || context.iStylePasteMode == CParagraphStyle::EConvertNewStyles)
		{
		TInt maxSize = styleMap->Count();
		for (TInt count=0; count<maxSize; count++)
			{
			delete (CParagraphStyle*)styleMap->At(count).iT;	
			}
		}
		CleanupStack::PopAndDestroy();	// cleanup styleMap
	}

	CleanupStack::PopAndDestroy();  // cleanup paraMap

	__TEST_INVARIANT;
	}


void CRichTextIndex::PasteStylesL(RReadStream& aStream,CStyleMap& aMap,const TRtPasteContext& aContext)
// Restore the paragraph styles from the specified stream.
// Restoration is controlled by the flag TStylePasteMode.
//
	{
	
	TInt styleCount=aMap.iCapacity;
//	TBool docSupportsStyles = iText.StyleListPresent();	// detect if document has style list
	CParagraphStyle* style=NULL;
	for(TUint8 ii=0; ii<styleCount; ii++)
	{
		TInt refNo=aStream.ReadInt8L();
		style=CParagraphStyle::NewL(aStream,*aContext.iGlobalParaLayer,*aContext.iGlobalCharLayer);

		// Now attempt to map this style to one in the document we are adding it to
		// refNo contains the style number when the selection was cut, which
		//   is needed to map to the reference no's coming in with the paragraph
		//   format info

		if (aContext.iStylePasteMode == CParagraphStyle::EIgnoreNewStyles)
			// We're ignoring all style info, so delete it now
			delete style;
		else
		{

		if (aContext.iStyleList && aContext.iStylePasteMode == CParagraphStyle::EAddNewStyles)
		{
			TInt docStyle = iText.StyleList()->IndexByName(style->iName);
			if (docStyle!=KErrNotFound)
			{
				/* Add the existing style into the stylemap */
				aMap.Bind(refNo,iText.StyleList()->PtrByName(style->iName)->iStyle);

				/* Since this style exists, we can safely delete this copy */
				delete style;

			}
			else
			{
				// Ok, the document doesn't have this style in it.
				// So let's add it in, and put a reference in the stylemap

				RParagraphStyleInfo newStyle(style);

				iText.StyleList()->AppendL(&newStyle);

				aMap.Bind(refNo,style);

				// the StyeList takes care of style destruction, so no deletion
				// is necessary here
			}
		}
		else
		{
			// Document doesn't support styles, so save them so they can
			// be converted to character specific formatting later on
			aMap.Bind(refNo, style);
		}
		}

	}
}


void CRichTextIndex::ImposeCharacterStyleL(CCharFormatLayer** aCharLayer)
{
	// This function is used to impose the current character style onto
	// the character layer for this paragraph. It's used when translating
	// style information into specific formatting when pasting text containing
	// styles into text without styles.

	TCharFormatX MergedCharFormat;
	TCharFormatXMask MergedCharFormatMask;
			
	CCharFormatLayer* newCharLayer;

	// Extract the masks from both layers.
	// use MergedCharFormat to hold the char info which we don't want yet

	(*aCharLayer)->Sense(MergedCharFormat, MergedCharFormatMask);
	iLastCharacterStyle->Sense(MergedCharFormat, MergedCharFormatMask);
			
	// Re-build charLayer;
	CleanupStack::PushL(*aCharLayer);
	newCharLayer = CCharFormatLayer::NewL(MergedCharFormat,MergedCharFormatMask);
	CleanupStack::Pop(*aCharLayer);

	delete (*aCharLayer);
	(*aCharLayer) = newCharLayer;
}


void CRichTextIndex::PasteSharedFormatsL(RReadStream& aStream,CParaAttribsMap& aMap,const TRtPasteContext& aContext,CStyleMap* aStyleMap)
// Load shared formats from the specified stream.
// Only adds the loaded shared format to the list of shared formats
// if it does not already exist in that list.
// A map is kept, that for each loaded format specifies its offset within
// the shared list.
//

	{
	TInt mapCount=aMap.iCapacity;
	for (TUint8 paraItem=0;paraItem<mapCount;paraItem++)
		{
		TInt refNo=aStream.ReadInt32L();
		CParaFormatLayer* paraLayer=PasteParagraphFormatL(aStream,aContext,aStyleMap);
		CleanupStack::PushL(paraLayer);

		CCharFormatLayer* charLayer=CCharFormatLayer::NewL(aStream);  // Does not restore based on link.
//// Change specific formatting for this paragraph

		if (iLastCharacterStyle != NULL)
			ImposeCharacterStyleL(&charLayer);
		
		charLayer->SetBase(aContext.iGlobalCharLayer);

		CleanupStack::PushL(charLayer);
		CParaAttribs* paraAttribs=GetParaAttribsL(paraLayer,charLayer);
		CleanupStack::PopAndDestroy(2);  // charLayer & paraLayer
		aMap.Bind(refNo,paraAttribs);
		paraAttribs->iRefCount--;
		}
	}


void CRichTextIndex::PasteIxL(RReadStream& aStream,TRtPasteContext& aContext,const CParaAttribsMap& aMap,CStyleMap* aStyleMap/*,CParaAttribs* aSecondReserved*/)
//
	{
	TInt completeParaCount=aStream.ReadInt32L();
	completeParaCount-=(aContext.iIncompleteParaFlag)? 1 : 0;
//	Create rollback states
	TParaAttribsEntry para=(*iParaIx)[aContext.iPastePos.iParaElement];
	CParaAttribs* reclaimed=RequestReclaimShareL(para.iParaAttribs,&para);  // does not release share
	iRollbackParaAttribsHandle=NULL;
	if (reclaimed)
		{
		iRollbackParaAttribsHandle=para.iParaAttribs;
		CleanupStack::PushL(TCleanupItem(ReleaseOnCleanup,reclaimed));
		(*iParaIx)[aContext.iPastePos.iParaElement].iParaAttribs=reclaimed;  // Use this reclaimed para attribs
		}
//	Split the phrase at the paste position, ready to insert clipboard content
	TRAPD(ret,
	SplitPhraseL(aContext.iPastePos.iDocPos));
		if (ret!=KErrNone) {RbPasteParaIxL(aContext.iPastePos,aContext.iParasPasted,ret);}
	ScanToPosition(aContext.iPastePos.iDocPos,EScanToPositionAbsolute);  // pick up the changes
	aContext.iPastePos=iPos;
//	Paste all complete paragraphs from the clipboard
	TInt characterCount=0;
	RPhraseAttribsEntry* firstParaVirtualPhrase=NULL;
	TRAP(ret,
	characterCount=PasteParaIxL(aStream,aContext,completeParaCount,aMap,firstParaVirtualPhrase,aStyleMap));
		if (ret!=KErrNone) {RbPasteParaIxL(aContext.iPastePos,aContext.iParasPasted,ret);}
	CleanupStack::PushL(firstParaVirtualPhrase);
//	Paste any remaining text fragment.
	TTextFragment textFragment;
	if (aContext.iIncompleteParaFlag)
		{
		TRAPD(ret,
		textFragment=GetTextFragmentL(aStream));
			if (ret!=KErrNone) {RbPasteParaIxL(aContext.iPastePos,aContext.iParasPasted,ret);}			
		characterCount+=textFragment.iLength;

		// Restore the character stle info for the final text fragment
		if (aContext.iParagraphStylesFlag || aContext.iApplyFormatToLastFlag)
			{
			CParaFormatLayer* paraLayer = PasteParagraphFormatL(aStream, aContext, aStyleMap);
			CleanupStack::PushL(paraLayer);
			//Restore the paragraph formatting for the final text fragment
			if (aContext.iApplyFormatToLastFlag)
				{
				CParaFormat* paraFormat = CParaFormat::NewLC();
				TParaFormatMask mask;
				paraLayer->SenseL(paraFormat,mask);
				(*iParaIx)[completeParaCount].iParaAttribs->iParaFormat->SetL(paraFormat,mask);
				CleanupStack::PopAndDestroy(paraFormat);
				}
			// We can now discard the format layer, but the character format has been safely
			// patched to the style character format
			CleanupStack::PopAndDestroy(paraLayer);
			}
		}
//
	TRAP(ret,
	PastePhraseIxL(aStream,aContext,firstParaVirtualPhrase));
		if (ret!=KErrNone) {RbPasteParaIxL(aContext.iPastePos,aContext.iParasPasted,ret);}
	CleanupStack::PopAndDestroy();  // firstParaVirtualPhrase / shallow destroy only - deep copy in phraseIx

//	ScanToPosition(aContext.iPastePos.iDocPos,EScanToPositionAbsolute);  // phraseIx not pasted at this point
//	TLogicalPosition headParaNormalizePos=iPos;
	
	
	if (completeParaCount==0)
		{// Adjust paragraph length & phrase count
		TParaAttribsEntry* para=&(*iParaIx)[aContext.iPastePos.iParaElement];
		para->iLength+=textFragment.iLength;
		para->iParaAttribs->iPhraseCount+=textFragment.iPhraseCount;
		ScanToPosition(aContext.iPastePos.iDocPos,EScanToPositionAbsolute);
		TLogicalPosition headParaNormalizePos=iPos;
		MergePhrases(headParaNormalizePos);
		MergePhrases(aContext.iPastePos.iDocPos+characterCount);
		DoPasteCleanup(headParaNormalizePos,reclaimed);
		}
	else
		{// Adjust paragraph lengths & phrase counts	
		TParaAttribsEntry* firstPara=&(*iParaIx)[aContext.iPastePos.iParaElement];
		firstPara->iLength+=aContext.iPastePos.iParaElementOffset;  // Update length of the first para
		firstPara->iParaAttribs->iPhraseCount+=(aContext.iPastePos.iPhraseElement-aContext.iPastePos.iParaBasePhraseElement);  // Update the phrase count of the first para attribs
		ScanToPosition(aContext.iPastePos.iDocPos,EScanToPositionAbsolute);
		TLogicalPosition headParaNormalizePos=iPos;
		MergePhrases(headParaNormalizePos);
		DoPasteCleanup(headParaNormalizePos,reclaimed);
		//
		// Adjust the length & phrase count of the new final paragraph
		TParaAttribsEntry* lastPara=&(*iParaIx)[aContext.iPastePos.iParaElement+completeParaCount];
		lastPara->iLength=(lastPara->iLength-aContext.iPastePos.iParaElementOffset)+textFragment.iLength;
		lastPara->iParaAttribs->iPhraseCount-=(aContext.iPastePos.iPhraseElement-aContext.iPastePos.iParaBasePhraseElement);
		// phrase count may be wrong - since the added phrase may have been merged into the current one.
		// Cant just add these phrases.
		lastPara->iParaAttribs->iPhraseCount+=textFragment.iPhraseCount;
		ScanToPosition(aContext.iPastePos.iDocPos+characterCount,EScanToPositionAbsolute);
		MergePhrases(iPos);
		DoPasteCleanup(iPos,(CParaAttribs*)NULL);
		}
	RebalanceIndex();
	NormalizeSharedList();
	}


void CRichTextIndex::DoPasteCleanup(TLogicalPosition& aNormalizePos,CParaAttribs* aReclaimed)
//
	{
	NormalizeNow(aNormalizePos);
	if (aReclaimed)
		CleanupStack::Pop();
	}


TInt CRichTextIndex::PasteParaIxL(RReadStream& aStream,TRtPasteContext& aContext,TInt aCompleteParaCount,const CParaAttribsMap& aMap,RPhraseAttribsEntry*& aFirstParaVirtualPhrase, CStyleMap* aStyleMap)
//
	{
	TParaAttribsEntry para;
	iParaIx->InsertL(aContext.iPastePos.iParaElement,para,aCompleteParaCount);	
	aContext.iParasPasted=aCompleteParaCount;
	TInt characterCount=0;
	if (aCompleteParaCount>0)
		{
		para=DoPasteFirstIntoParaL(aStream,aMap,aContext,aFirstParaVirtualPhrase, aStyleMap);
		TInt paraItem=0;
		(*iParaIx)[aContext.iPastePos.iParaElement+paraItem]=para;
		characterCount+=para.iLength;
		}
	CleanupStack::PushL(TCleanupItem(DiscardPhraseOnCleanup,aFirstParaVirtualPhrase));
	for (TInt paraItem=1;paraItem<aCompleteParaCount;paraItem++)
		{// Paste all the paras that have paragraph delimiters.
		para=DoPasteIntoParaL(aStream,aMap,aContext,aStyleMap);
		(*iParaIx)[aContext.iPastePos.iParaElement+paraItem]=para;
		characterCount+=para.iLength;
		}
	CleanupStack::Pop();  // firstParaVirtualPhrase
// ASSERT: At this point we have pasted all paras that were in the stream.
	if (aContext.iParasPasted!=aCompleteParaCount)
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_PASTEPARAIXL, "EPasteParaIxError" );
	    }
	__ASSERT_DEBUG(aContext.iParasPasted==aCompleteParaCount,Panic(EPasteParaIxError));
	return characterCount;
	}
			
			
TParaAttribsEntry CRichTextIndex::DoPasteFirstIntoParaL(RReadStream& aStream,const CParaAttribsMap& aMap,const TRtPasteContext& aContext,RPhraseAttribsEntry*& aFirstParaVirtualPhrase,CStyleMap* aStyleMap)
//
	{
	TParaAttribsEntry para;
	para.iLength=aStream.ReadInt32L();
	TUint8 refNo=aStream.ReadUint8L();
	if (refNo>0)
		{// This is the first pasted para, so if shared, must convert to phrase on the fly.
		CParaAttribs* paraAttribs=aMap.Item(refNo);
		CParaAttribs* specificParaAttribs=CParaAttribs::NewL(paraAttribs->iParaFormat);
		CleanupStack::PushL(TCleanupItem(ReleaseOnCleanup,specificParaAttribs));
		//
		CCharFormatLayer* layer=CCharFormatLayer::NewCopyBaseL(paraAttribs->iCharFormat);
		CleanupStack::PushL(layer);
		aFirstParaVirtualPhrase=new(ELeave) RPhraseAttribsEntry(layer,para.iLength);
		CleanupStack::Pop(2);  // layer & specificParaAttribs
		//
		para.iParaAttribs=specificParaAttribs;
		if (para.iParaAttribs==NULL)
		    {
		    OstTrace0( TRACE_FATAL, CRICHTEXTINDEX_DOPASTEFIRSTINTOPARAL, "ESharedFormatsMapIntegrityError" );
		    }
		__ASSERT_ALWAYS(para.iParaAttribs!=NULL,Panic(ESharedFormatsMapIntegrityError));
		}
	else
		{// Have to build up the specific para attribs
		CParaFormatLayer* paraLayer=PasteParagraphFormatL(aStream,aContext,aStyleMap);
		CleanupStack::PushL(paraLayer);
		TInt phraseCount=aStream.ReadInt32L();  // specific phrase count
		CParaAttribs* specificParaAttribs=CParaAttribs::NewL(paraLayer);
		specificParaAttribs->iPhraseCount=phraseCount;
		para.iParaAttribs=specificParaAttribs;
		CleanupStack::PopAndDestroy();  // a copy of paraLayer is taken!
		}
	return para;
	}


TParaAttribsEntry CRichTextIndex::DoPasteIntoParaL(RReadStream& aStream,const CParaAttribsMap& aMap,const TRtPasteContext& aContext,CStyleMap* aStyleMap)
//
	{
	TParaAttribsEntry para;
	para.iLength=aStream.ReadInt32L();
	TUint8 refNo=aStream.ReadUint8L();
	if (refNo>0)
		{// Link to para attribs in shared list & up its reference count
		para.iParaAttribs=aMap.Item(refNo);
		if (para.iParaAttribs==NULL)
		    {
		    OstTrace0( TRACE_FATAL, CRICHTEXTINDEX_DOPASTEINTOPARAL, "ESharedFormatsMapIntegrityError" );
		    }
		__ASSERT_ALWAYS(para.iParaAttribs!=NULL,Panic(ESharedFormatsMapIntegrityError));
		para.iParaAttribs->iRefCount++;
		}
	else
		{// Have to build up the specific para attribs
		CParaFormatLayer* paraLayer=PasteParagraphFormatL(aStream,aContext,aStyleMap);
		CleanupStack::PushL(paraLayer);
		TInt phraseCount=aStream.ReadInt32L();  // specific phrase count
		CParaAttribs* specificParaAttribs=CParaAttribs::NewL(paraLayer);
		specificParaAttribs->iPhraseCount=phraseCount;
		para.iParaAttribs=specificParaAttribs;
		CleanupStack::PopAndDestroy();  // a copy of paraLayer is taken!
		}
	return para;
	}


TTextFragment CRichTextIndex::GetTextFragmentL(RReadStream& aStream)
//
	{
	TTextFragment textFragment;
	textFragment.iLength=aStream.ReadInt32L();
	textFragment.iPhraseCount=aStream.ReadInt32L();
	return textFragment;
	}


void CRichTextIndex::PastePhraseIxL(RReadStream& aStream,TRtPasteContext& aContext,const RPhraseAttribsEntry* aFirstParaVirtualPhrase)
// The character position to paste at should now fall on a phrase boundary.
//
	{
// ASSERT: Having pasted the paraIx, the para containig pastePos has had the containing phrase split at that point.
	if (aContext.iPastePos.iPhraseElementOffset!=0)
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXTINDEX_PASTEPHRASEIXL, "EPastePhraseIxErr" );
	    }
	__ASSERT_ALWAYS(aContext.iPastePos.iPhraseElementOffset==0,Panic(EPastePhraseIxErr));

	TInt offset=0;
	TInt phraseCount=aStream.ReadInt32L();  // leave caught by calling function.  No state change yet.
	if (aFirstParaVirtualPhrase)
		{
		iPhraseIx->InsertL(aContext.iPastePos.iPhraseElement,*aFirstParaVirtualPhrase);
		offset++;
		}
	RPhraseAttribsEntry holdingPhrase;
	iPhraseIx->InsertL(aContext.iPastePos.iPhraseElement+offset,holdingPhrase,phraseCount);
	for (TInt phraseItem=0;phraseItem<phraseCount;phraseItem++)
		{// Restore each phrase & insert into the phrase index.
		RPhraseAttribsEntry phrase;
		TRAPD(ret,
//		phrase=DoPastePhraseL(aStream,aContext));  // !! delete this if the code works
		DoPastePhraseL(aStream,aContext,phrase));
			if (ret!=KErrNone) {RbPastePhraseIxL(aContext.iPastePos,phraseCount+offset,ret);}
		(*iPhraseIx)[aContext.iPastePos.iPhraseElement+phraseItem+offset]=phrase;
		if (phrase.IsPicturePhrase() && iText.PictureFactory())
			iPictureCount++;
		}
	}


void CRichTextIndex::DoPastePhraseL(RReadStream& aStream,const TRtPasteContext& aContext,RPhraseAttribsEntry& aPhrase)
//
	{
	TBool isPicture=(TBool)aStream.ReadUint8L();
	TInt phraseLength=aStream.ReadInt32L();

	if (!(isPicture && phraseLength==1 || !isPicture))
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXTINDEX_DOPASTEPHRASEL, "KErrCorrupt" );
	    }
	__ASSERT_ALWAYS(isPicture && phraseLength==1 || !isPicture,User::Leave(KErrCorrupt));
	
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL(aStream);
/// Change this character format if style formatting is to be applied

// Paste style for the trailing text
	if (iLastCharacterStyle != NULL)
			ImposeCharacterStyleL(&charLayer);
	
		charLayer->SetBase(aContext.iGlobalCharLayer);

	CPicturePhrase* picPhrase=NULL;
	const MPictureFactory* factory=iText.PictureFactory();
	TBool pictureLoadError=EFalse;
	if (isPicture)
		{
		TPictureHeader hdr;
		aStream>> hdr;  // hdr.iPicture always references a picture in the deferred store.
		if (isPicture && factory)
			{// Manufacture new picture phrase & set its picture store
			CleanupStack::PushL(charLayer);
			TBool ownershipTaken(EFalse);
			picPhrase=CPicturePhrase::NewL(hdr,charLayer,ownershipTaken);
			CleanupStack::Pop();  // charLayer - picPhrase takes ownership
			CleanupStack::PushL(picPhrase);
			TRAPD(r,factory->NewPictureL(hdr,*aContext.iStore));  // load picture since clipboard store is transient.
			// r=KErrNotSupported  // we don't recognise the picture type
			if (r==KErrNone)
				{
				picPhrase->iPicHdr.iPicture=hdr.iPicture;  // make picPhrase point at the restored picture object.
				TRAP(r,
				hdr.iPicture->DetachFromStoreL(CPicture::EDetachFull));  // recurse the call to detach the picture from the store
				}
			switch (r)
				{
				case(KErrNone):
					break;
				default:
					pictureLoadError=ETrue;
					picPhrase->iCharFormat=NULL;
					CleanupStack::PopAndDestroy(picPhrase);
					if (r!=KErrNotSupported)
					    {
						OstTrace1( TRACE_FATAL, DUP2_CRICHTEXTINDEX_DOPASTEPHRASEL, "Leave code=%d", r );
					    User::Leave(r);
					    }
				}
			}
		}
	if (isPicture && factory && !pictureLoadError)
		new(&aPhrase) RPhraseAttribsEntry(picPhrase);
	else
		new(&aPhrase) RPhraseAttribsEntry(charLayer,phraseLength);
	
	// The ownership has been transfered to RPhraseAttribsEntry
	if(picPhrase)
	    CleanupStack::Pop(picPhrase);
	}


TUint8 CRichTextIndex::ReadTUint8CountL(RReadStream& aStream)const
	{return aStream.ReadUint8L();}


void CRichTextIndex::RbPasteSharedFormatsL(TInt aRet)
// For each para attribs that has been read in, release all shares on it, if it
// does not already exist in shared list.
//
	{
	NormalizeSharedList();
	OstTrace1( TRACE_FATAL, CRICHTEXTINDEX_RBPASTESHAREDFORMATSL, "Leave code=%d", aRet );
	User::Leave(aRet);
	}
		
	
void CRichTextIndex::NormalizeSharedList()
// Removes unreferenced (non-shared) shared paras from the shared list.
// Called in rollback situations.
// Also called naturally following a paste, since an incoming shared para
// may be pasted into an existing para, making it non-shared.
//
	{
	CParaAttribs* currentSharedPara=NULL;
	TDblQueIter<CParaAttribs> iterator(iSharedParaQueHead);
	while ((currentSharedPara=iterator++)!=NULL)
		{// Rollback the shared list.
		if (currentSharedPara->iRefCount==0)
			{// Remove this unreferenced item from the shared list.
			currentSharedPara->iRefCount=1;
			currentSharedPara->Release();
			}
		}
	}


void CRichTextIndex::RbRemoveInsertedParaAttribsEntries(TInt aFirstParaInsertPos,TInt aInsertedParaCount)
// Rollback on leaving part way through inserting paragraph records into the para index.
// For all pasted paragraph records, release their share on their paraAttribs if present.
//
	{
	for (TInt ii=0;ii<aInsertedParaCount;ii++)
		{
		CParaAttribs* paraAttribs=(*iParaIx)[aFirstParaInsertPos+ii].iParaAttribs;
		if (paraAttribs)
			paraAttribs->Release();
		}
	iParaIx->Delete(aFirstParaInsertPos,aInsertedParaCount);
	}


void CRichTextIndex::RbPasteParaIxL(const TLogicalPosition& aPos,TInt aParasPasted,TInt aRet)
// Rollback on leaving part way through the pasting of the para index.
// For all pasted paras, release their share on their paraAttribs.
//
	{
	RbRemoveInsertedParaAttribsEntries(aPos.iParaElement,aParasPasted);
	MergePhrases(aPos.iDocPos);  // updates iPos to paste pos.
	// 
	//
	if (iRollbackParaAttribsHandle)
		{
		RemoveFromPhraseIx(iPos.iPhraseElement,1);  // Removes the phrase created from ResquestReclaim on para
		(*iParaIx)[aPos.iParaElement].iParaAttribs=iRollbackParaAttribsHandle;
		iRollbackParaAttribsHandle=NULL;
		}
	OstTrace1( TRACE_FATAL, CRICHTEXTINDEX_RBPASTEPARAIXL, "Leave code=%d", aRet );
	User::Leave(aRet);
	}


void CRichTextIndex::RbRemoveInsertedPhraseAttribsEntries(TInt aFirstPhraseInsertPos,TInt aInsertedPhraseCount)
// Rollback on leaving part way through the pasting of the phrase index.
// For all pasted phrases, discard their components.
//
	{
	for (TInt ii=0;ii<aInsertedPhraseCount;ii++)
		{
		RPhraseAttribsEntry& phrase=(*iPhraseIx)[aFirstPhraseInsertPos+ii];
		if (phrase.IsPicturePhrase())
			iPictureCount--;
		if (phrase.CharFormat())
			phrase.Discard();
		}
	iPhraseIx->Delete(aFirstPhraseInsertPos,aInsertedPhraseCount);
	}


void CRichTextIndex::RbPastePhraseIxL(const TLogicalPosition& aPos,TInt aPhraseCount,TInt aRet)
// Rollback on leaving part way through the pasting of the phrase index.
// For all pasted phrases, discard their components.
//
	{
	RbRemoveInsertedPhraseAttribsEntries(aPos.iPhraseElement,aPhraseCount);
	OstTrace1( TRACE_FATAL, CRICHTEXTINDEX_RBPASTEPHRASEIXL, "Leave code=%d", aRet );
	User::Leave(aRet);
	}
