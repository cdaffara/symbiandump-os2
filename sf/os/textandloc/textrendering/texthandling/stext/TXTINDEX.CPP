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
#include <s32stor.h>
#include "TXTFMLYR.H"
#include "TXTETEXT.H"
#include "TXTLAYDC.H"
#include "TXTSTYLE.H"
#include "TXTINDEX.H"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TXTINDEXTraces.h"
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TXTFMLYR_INTERNAL.H"
#include "TXTSTYLE_INTERNAL.H"
#endif

TGlobalLayerInfoAppend::TGlobalLayerInfoAppend()
	: iAggParaFormatLayer(NULL),iAggCharFormatLayer(NULL),iComParaFormatLayer(NULL),iComCharFormatLayer(NULL)
	{}

TGlobalLayerInfoAppend::TGlobalLayerInfoAppend(const CParaFormatLayer* aAggParaFormatLayer,const CCharFormatLayer* aAggCharFormatLayer,
							const CParaFormatLayer* aComParaFormatLayer,const CCharFormatLayer* aComCharFormatLayer)
	: iAggParaFormatLayer(aAggParaFormatLayer),iAggCharFormatLayer(aAggCharFormatLayer),
	  iComParaFormatLayer(aComParaFormatLayer),iComCharFormatLayer(aComCharFormatLayer)
	{}


TTextFragment::TTextFragment():
	iLength(0),
	iPhraseCount(0)
	{
	}


TCurrentIndexRecords::TCurrentIndexRecords()
	{
	}


DLLEXPORT_C void CRichTextIndex::__DbgTestInvariant()const
// Provides class invariants.  Explanations below:
//
	{
#ifdef _DEBUG
// ASSERT: Every phrase index is consistent with its corresponding paragraph.
	TInt zeroLengthPhraseCount=0;
	TInt maxPara=iParaIx->Count();
	TInt numberOfReferencesToSharedList=0;
	TInt currentPhraseElement=0;
	for (TInt para=0;para<maxPara;para++)
		{
		// ASSERT: The basedOn link is valid.
		CFormatLayer* thisLayer=(*iParaIx)[para].iParaAttribs->iParaFormat;
		CFormatLayer* base=CONST_CAST(CFormatLayer*,thisLayer->SenseBase());
		if (base==NULL)
		    {
		    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_DBGTESTINVARIANT, "base==NULL" );
		    }
		__ASSERT_DEBUG(base!=NULL,User::Invariant());
		if ((*iParaIx)[para].iParaAttribs->iRefCount>0)
			numberOfReferencesToSharedList++;
		TInt paragraphLength=(*iParaIx)[para].iLength;
		TInt sumOfPhraseLengths=0;
		TInt maxPhrase=(*iParaIx)[para].iParaAttribs->PhraseCount();
		for (TInt phrase=0;phrase<maxPhrase;phrase++)
			{
			if (maxPhrase>1)
				{
				const RPhraseAttribsEntry* phrase=&(*iPhraseIx)[currentPhraseElement];
				CCharFormatLayer* charFormatLayer=phrase->CharFormat();
				// ASSERT: The basedOn link is valid.
				if (charFormatLayer->SenseBase()==NULL)
				    {
				    OstTrace0( TRACE_DUMP, DUP1_CRICHTEXTINDEX_DBGTESTINVARIANT, "charFormatLayer->SenseBase()==NULL" );
				    }
				__ASSERT_DEBUG(charFormatLayer->SenseBase()!=NULL,User::Invariant());
				if (TInt(charFormatLayer->SenseBase())<=0x1000)
				    {
				    OstTrace0( TRACE_DUMP, DUP2_CRICHTEXTINDEX_DBGTESTINVARIANT, "TInt(charFormatLayer->SenseBase())<=0x1000" );
				    }
				__ASSERT_DEBUG(TInt(charFormatLayer->SenseBase())>0x1000,User::Invariant());
				sumOfPhraseLengths+=(*iPhraseIx)[currentPhraseElement].Length();
				if ((*iPhraseIx)[currentPhraseElement].Length()==0)
					zeroLengthPhraseCount++;
				currentPhraseElement++;
				}
			else
				{
				CCharFormatLayer* charFormatLayer=(*iParaIx)[para].iParaAttribs->iCharFormat;
				// ASSERT: The basedOn link is valid.
				if (charFormatLayer->SenseBase()==NULL)
				    {
				    OstTrace0( TRACE_DUMP, DUP3_CRICHTEXTINDEX_DBGTESTINVARIANT, "charFormatLayer->SenseBase()==NULL" );
				    }
				__ASSERT_DEBUG(charFormatLayer->SenseBase()!=NULL,User::Invariant());
				sumOfPhraseLengths+=(*iParaIx)[para].iLength;
				}
			}
		if (sumOfPhraseLengths!=paragraphLength)
		    {
		    OstTrace0( TRACE_DUMP, DUP4_CRICHTEXTINDEX_DBGTESTINVARIANT, "sumOfPhraseLengths!=paragraphLength" );
		    }
		__ASSERT_DEBUG(sumOfPhraseLengths==paragraphLength,User::Invariant());
		}
// ASSERT: We have no unexpected phrases left over
	if (currentPhraseElement!=-1 &&
            currentPhraseElement!=iPhraseIx->Count())
	    {
	    OstTrace0( TRACE_DUMP, DUP5_CRICHTEXTINDEX_DBGTESTINVARIANT, "We have no unexpected phrases left over" );
	    }
	__ASSERT_DEBUG(currentPhraseElement==-1 ||
					currentPhraseElement==iPhraseIx->Count(),User::Invariant());
// ASSERT: There is either zero(0) or one(1) zero length phrase in the whole index
	if (!((zeroLengthPhraseCount==0) ||
            (zeroLengthPhraseCount==1 && iPendingNewPhrasePos!=EInsertCharFormatReset)))
	    {
	    OstTrace0( TRACE_DUMP, DUP6_CRICHTEXTINDEX_DBGTESTINVARIANT, "There is either zero(0) or one(1) zero length phrase in the whole index" );
	    }
	__ASSERT_DEBUG( (zeroLengthPhraseCount==0) ||
					(zeroLengthPhraseCount==1 && iPendingNewPhrasePos!=EInsertCharFormatReset),
					User::Invariant());
// ASSERT: the number of paraEntries with paraAttribs of refCount>0 == the sum of refCounts in the shared list.
//			or is only one less - as is set when SetInsertCharFormat is called on a shared paraAttribs.
	TInt totalReferenceCount=0;
	if (!iSharedParaQueHead.IsEmpty())
		{
		CParaAttribs* currentSharedPara;
		TDblQueIter<CParaAttribs> iterator(((CRichTextIndex*)this)->iSharedParaQueHead);
		while ((currentSharedPara=iterator++)!=NULL)
			totalReferenceCount+=currentSharedPara->iRefCount;
		}
	if ((numberOfReferencesToSharedList!=totalReferenceCount) &&
            (numberOfReferencesToSharedList!=totalReferenceCount-1))
	    {
	    OstTrace0( TRACE_DUMP, DUP7_CRICHTEXTINDEX_DBGTESTINVARIANT, "Invariant" );
	    }
	__ASSERT_DEBUG((numberOfReferencesToSharedList==totalReferenceCount) ||
				   (numberOfReferencesToSharedList==totalReferenceCount-1),User::Invariant());
// ASSERT: iPictureCount corresponds to the number of pictures in the stored in the index.
	TInt picCount=0;
	TInt phraseCount=(iPhraseIx) ? iPhraseIx->Count() : 0;
	for (TInt item=0;item<phraseCount;item++)
		picCount+=((*iPhraseIx)[item].IsPicturePhrase())
					? 1
					: 0;
	if (iPictureCount!=picCount)
	    {
	    OstTrace0( TRACE_DUMP, DUP8_CRICHTEXTINDEX_DBGTESTINVARIANT, "Invariant" );
	    }
	__ASSERT_DEBUG(iPictureCount==picCount,User::Invariant());
#endif
	}


CRichTextIndex* CRichTextIndex::NewL(const CParaFormatLayer* aGlobalParaLayer,const CCharFormatLayer* aGlobalCharLayer,
									 const CRichText& aText,TInt aParaGran,TInt aPhraseGran)
// Return a handle to a new instance of this class.
// Requires the global format layer handles on which to base the first content.
//
	{
	CRichTextIndex* self=new(ELeave) CRichTextIndex(aText);
	CleanupStack::PushL(self);
	self->ConstructL(aGlobalParaLayer,aGlobalCharLayer,aParaGran,aPhraseGran);
	CleanupStack::Pop();
	return self;
	}


CRichTextIndex::CRichTextIndex(const CRichText& aText):
	iText(aText),
	iPendingNewPhrasePos(EInsertCharFormatReset),
	iSharedParaQueHead(_FOFF(CParaAttribs,link))
	{
	}


void CRichTextIndex::ConstructL(const CParaFormatLayer* aGlobalParaLayer,const CCharFormatLayer* aGlobalCharLayer,TInt aParaGran,TInt aPhraseGran)
// Provides a fully initialised rich text index.
// Upon construction, the index contains a single paragraph of length 1 (para.terminator).
// This first paragraph initially has constant character formatting, ie the shared para list
// must have one item in it.
//
	{
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL();  // Creates empty layer.
	paraLayer->SetBase(aGlobalParaLayer);  // Sets basedOn to global default.
	CleanupStack::PushL(paraLayer);
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();  // Creates empty layer.
	charLayer->SetBase(aGlobalCharLayer);  // Sets basedOn to global default.
	CleanupStack::PushL(charLayer);
	CParaAttribs* paraAttribs=GetParaAttribsL(paraLayer,charLayer);
	CleanupStack::PopAndDestroy(2);
	CleanupStack::PushL(TCleanupItem(ReleaseOnCleanup,paraAttribs));
	iParaIx=new(ELeave) CArrayFixSeg<TParaAttribsEntry>(aParaGran);
	TParaAttribsEntry para(1,paraAttribs);
	iParaIx->AppendL(para);
	CleanupStack::Pop();
	iPhraseIx=new(ELeave) CArrayFixSeg<RPhraseAttribsEntry>(aPhraseGran);

	__TEST_INVARIANT;
	}


CRichTextIndex::~CRichTextIndex()
// Free up all storage allocated in the rich text index.
//
	{
	TInt count;
	if (iPhraseIx)
		{// Destroy the phrase index and its contents.
		if (!iParaIx)
		    {
		    OstTrace0( TRACE_FATAL, CRICHTEXTINDEX_CRICHTEXTINDEX, "EPhraseIxPresentWithNoParaIx" );
		    }
		__ASSERT_ALWAYS(iParaIx,Panic(EPhraseIxPresentWithNoParaIx));
		count=iPhraseIx->Count();
		for (TInt offset=0;offset<count;offset++)
			(*iPhraseIx)[offset].Discard();
		}
	delete iPhraseIx;
	if (iParaIx)
		{// Destroy the para index and its contents.
		count=iParaIx->Count();
		for (TInt offset=0;offset<count;offset++)
			{
			CParaAttribs* pA=(*iParaIx)[offset].iParaAttribs;
			if (!pA->IsShared())
				pA->Release();
			}
		}
	delete iParaIx;
	RebalanceIndex();
	//
	// Clear the shared list.  (Will usually be empty by now
	// unless internalize failed after getting shared list & before getting all para data.
	CParaAttribs* currentSharedPara;
	TDblQueIter<CParaAttribs> iterator(iSharedParaQueHead);
	while ((currentSharedPara=iterator++)!=NULL)
		currentSharedPara->Release(currentSharedPara->iRefCount);
	if (!iSharedParaQueHead.IsEmpty())
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXTINDEX_CRICHTEXTINDEX, "ERichTextIndexIntegrityErr" );
	    }
	__ASSERT_ALWAYS(iSharedParaQueHead.IsEmpty(),Panic(ERichTextIndexIntegrityErr));
	}


TInt CRichTextIndex::CharPosOfParagraph(TInt& aLength,TInt aParaOffset)const
// Returns the character position of the first character of paragraph aParaOffset,
// where aParaOffset specifies the nth paragraph.
// The length of this nth paragraph is written to aLength.
//
// If aParaOffset specifies a paragraph that does not exist, EScanEndOfData is returned.
//
	{
	__TEST_INVARIANT;

	if (aParaOffset>=iParaIx->Count())
		return CPlainText::EScanEndOfData;
	TInt pos=0,offset=0;
	for (offset=0;offset<aParaOffset;offset++)
		pos+=(*iParaIx)[offset].iLength;
	aLength=(*iParaIx)[offset].iLength;
	return pos;
	}


TInt CRichTextIndex::ParagraphNumberForPos(TInt& aPos)const
// Returns the paragraph offset for the specified character position aPos.
// aPos is in turn modified to hold the character position of the first character
// of this paragraph.  If aPos is already on a paragraph boundary then do nothing.
//
	{
	__TEST_INVARIANT;

	((CRichTextIndex*)this)->ScanToPosition(aPos,EScanToPositionAbsolute);
	aPos-=iPos.iParaElementOffset;
	return iPos.iParaElement;
	}


void CRichTextIndex::DocumentChanged()const
	{
 	MUTABLE_CAST(TLogicalPosition&,iLastUsed).Clear();
	}


void CRichTextIndex::DoSoloInsertL(TInt aPos,TInt aLength)
// Updates the index following the insertion of content into a single phrase.
// First find the phrase record the governs the insert pos.
// (1) If the current phrase is a text phrase, then simply extend the length of the phrase record.
// (2) If the current phrase is a picture phrase then this cannot be extended.
// There is 1 pathological case here:
// (i) The [current] picture phrase is the 1st phrase in the paragraph, and the insert pos
// is before this phrase. (paragraph insert pos == 0).
// In this case we must insert a new text phrase *before* the picture phrase.
// (ii) In normal circumstances the inserted text is after the picture phrase.  So, if the
// following phrase is of the same character format as the picture then we can re-use this phrase.
// If it is not of the same character format (or is also a picture phrase),
// then we must insert a new text phrase of the correct format immediately following the current picture phrase.
//
	{
	RebalanceIndex();
	if (!((iPendingNewPhrasePos == EInsertCharFormatReset) || (aPos == iPendingNewPhrasePos)))
		CancelInsertCharFormat();

	ScanToPosition(aPos,EScanToPositionMatchLeft);
	TCurrentIndexRecords current; GetCurrentRecords(current);
	if (current.iPhrase && current.iPhrase->IsPicturePhrase())
		{// Paragraph has specific char format, and current phrase is picture phrase.
		TInt newPhraseRequired=EFalse;
		CCharFormatLayer* charLayer=current.iPhrase->CharFormat();
		if (FirstPhraseOfParagraph() && iPos.iPhraseElementOffset==0)
			newPhraseRequired=ETrue;  // Text is inserted at the start of the para behind the picture.
		else
			{// Check for re-use of the next text phrase.
			iPos.iPhraseElement++;
			iPos.iPhraseElementOffset=0;
			const RPhraseAttribsEntry& nextPhrase=(*iPhraseIx)[iPos.iPhraseElement];
			if (nextPhrase.IsPicturePhrase() || !nextPhrase.CharFormat()->IsIdentical(charLayer,EFalse))
				{// Need a new phrase if the formats don't match OR the next phrase is a picture phrase.
				newPhraseRequired=ETrue;  // Need to create a new phrase to take this insert.
				}
			}
		if (newPhraseRequired)
			{// Insert new phrase & record this fact
			CCharFormatLayer* charFormat=CCharFormatLayer::NewCopyBaseL(charLayer);
			RPhraseAttribsEntry newPhrase(charFormat);
			CleanupStack::PushL(charFormat);
			iPhraseIx->InsertL(iPos.iPhraseElement,newPhrase);
			CleanupStack::Pop();
			current.iParaAttribs->iPhraseCount++;
			}
		GetCurrentRecords(current);  // Update current records, cos used below.
		}
	// Extend the lengths in the index.
	current.iParaEntry->iLength+=aLength;  // Increase length of paragraph.
	if (current.iPhrase)
		current.iPhrase->AdjustLength(aLength);  // Increase the length of this phrase.
	iPos.iPhraseElementOffset+=aLength;
	iPos.iParaElementOffset+=aLength;
	iPos.iDocPos+=aLength;
	}


void CRichTextIndex::InsertL(TInt aPos,const TDesC& aBuf,const CParaFormatLayer& aGlobalParaFormatLayer)
// Updates the index following the insertion of a descriptor of text into the document.
// Correctly handles embedded paragraph delimiters.
//
	{
	__TEST_INVARIANT;
	TInt bufLen=aBuf.Length();

	RebalanceIndex();
	TInt paragraphCount=0;
	const TText* start=aBuf.Ptr();
	const TText* end=start+bufLen;
	while (start<end)
		{
		if (*start++==CEditableText::EParagraphDelimiter)
			paragraphCount++;
		}
	if (paragraphCount==0)
		{
		if (bufLen > 0)
			{
			DoSoloInsertL(aPos,bufLen);
			CancelInsertCharFormat();
			}
		__TEST_INVARIANT;
		return;
		}
	ScanToPosition(aPos,EScanToPositionMatchLeft);
	TLogicalPosition pastePos=iPos;  // Used to adjust paragraph/phrase lengths later

	if (paragraphCount>1)
		{
		TCurrentIndexRecords current;
		GetCurrentRecords(current);
		const CParaAttribs& paraAttribs=*current.iParaAttribs;
		CCharFormatLayer* charLayer=(paraAttribs.IsShared())
								? paraAttribs.iCharFormat
								: (*iPhraseIx)[iPos.iPhraseElement].CharFormat();
		CParaFormatLayer* paraLayer=paraAttribs.iParaFormat;
		CParaAttribs* theParaAttribs=GetParaAttribsL(paraLayer,charLayer);
		CleanupStack::PushL(TCleanupItem(ReleaseOnCleanup,theParaAttribs));
		iParaIx->InsertL(pastePos.iParaElement,TParaAttribsEntry(0,theParaAttribs),paragraphCount-1);
		CleanupStack::Pop();
		theParaAttribs->iRefCount+=paragraphCount-2;	// add the extra references
		}
	TLogicalPosition pos;
	TRAPD(ret,
	SplitParagraphAtPastePosL(pastePos,pos,aGlobalParaFormatLayer));
		if (ret!=KErrNone)
			{
			RbRemoveInsertedParaAttribsEntries(pastePos.iParaElement,paragraphCount-1);
			User::Leave(ret);
			}

	if (paragraphCount>1)
		{	// swap the entries for the split and first inserted paragraph
		TParaAttribsEntry& paraEntry=(*iParaIx)[pastePos.iParaElement+paragraphCount-1];
		TParaAttribsEntry& insertEntry=(*iParaIx)[pastePos.iParaElement];
		TParaAttribsEntry temp=paraEntry;
		paraEntry=insertEntry;
		insertEntry=temp;
		}// Cos weve inserted new paragraphs in front of the governing one.

	// Sort out the front para
	TParaAttribsEntry& frontPara=(*iParaIx)[pastePos.iParaElement];
	TPtrC buf(aBuf);
	TInt lengthOfFirstPara=ParaLengthFromBuffer(buf);
	frontPara.iLength+=lengthOfFirstPara;  // Adjust the para length
	buf.Set(aBuf.Right(aBuf.Length()-lengthOfFirstPara-1));
	if (!frontPara.iParaAttribs->IsShared() && lengthOfFirstPara != 0)
		{
		RPhraseAttribsEntry* phrase=&(*iPhraseIx)[pastePos.iPhraseElement];
		if (phrase->IsPicturePhrase())
			{	// move insertion past any picture phrase
			pastePos.iPhraseElement++;
			pastePos.iPhraseElementOffset=0;
			phrase=&(*iPhraseIx)[pastePos.iPhraseElement];
			}
		phrase->AdjustLength(lengthOfFirstPara);  // Adjust the phrase length
		}
	for (TInt paraItem=1;paraItem<paragraphCount;paraItem++)
		{// For each para inserted between the fist and last
		TParaAttribsEntry& para=(*iParaIx)[pastePos.iParaElement+paraItem];
		TInt length=ParaLengthFromBuffer(buf)+1;
		if (length==KErrNotFound)
		    {
		    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_INSERTL, "EInsertEmbeddedParaErr" );
		    }
		__ASSERT_DEBUG(length!=KErrNotFound,Panic(EInsertEmbeddedParaErr));
		para.iLength=length;
		buf.Set(buf.Right(buf.Length()-length));
		}
	// For final paragrph
	TInt trailingTextLen=buf.Length();
	if (trailingTextLen>0)
		{
		TParaAttribsEntry& backPara=(*iParaIx)[pos.iParaElement];
		backPara.iLength+=trailingTextLen;
		if (!backPara.iParaAttribs->IsShared())
			{
			RPhraseAttribsEntry& phrase=(*iPhraseIx)[pos.iPhraseElement];
			phrase.AdjustLength(trailingTextLen);  // Adjust phrase length
			}
		}
	//
	// Now tidy up
	if (bufLen>1 && iPendingNewPhrasePos!=EInsertCharFormatReset)
		{
		iPendingNewPhrasePos=aPos+(bufLen-trailingTextLen);
		CancelInsertCharFormat();
		}

	__TEST_INVARIANT;
	}


void CRichTextIndex::SplitParagraphAtPastePosL(TLogicalPosition& aPastePos,TLogicalPosition& aNewPos,
												const CParaFormatLayer& aGlobalParaFormatLayer)
// Breaks the paragraph specified by the logical position aPastePos, inserting a paragraph delimiter.
//
	{
	TInt insertPendingPos = iPendingNewPhrasePos;
	DoSoloInsertL(aPastePos.iDocPos,1);
	TBool insertCharFormatDeleted = EFalse;
	if (InsertCharFormatIsActive())
		{
		insertCharFormatDeleted = DeleteInsertCharFormat();
		iPendingNewPhrasePos = EInsertCharFormatReset;
		}
	TRAPD(ret,
	InsertParagraphL(aPastePos.iDocPos+1,aGlobalParaFormatLayer));  // Split the current para (and maybe phrase index).
	if (ret!=KErrNone)
		{
		// locate the character inserted by DoSoloInsertL() above
		ScanToPosition(aPastePos.iDocPos,EScanToPositionAbsolute);
		TCurrentIndexRecords current;
		GetCurrentRecords(current);
		current.iParaEntry->iLength--;
		if (current.iPhrase)
			{
			current.iPhrase->AdjustLength(-1);  // collapse phrase by right amount
			if (insertPendingPos!=EInsertCharFormatReset)
				{
				if (current.iPhrase->Length()!=0)
				    {
				    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_SPLITPARAGRAPHATPASTEPOSL, "Invariant" );
				    }
				__ASSERT_DEBUG(current.iPhrase->Length()==0,User::Invariant());
				iPendingNewPhrasePos=insertPendingPos;
				}
			else if (current.iPhrase->Length()==0)
				{
				RemoveFromPhraseIx(iPos.iPhraseElement,1);
				current.iParaAttribs->iPhraseCount--;
				if (current.iParaAttribs->PhraseCount()<=1)
				    {
				    OstTrace0( TRACE_DUMP, DUP1_CRICHTEXTINDEX_SPLITPARAGRAPHATPASTEPOSL, "Invariant" );
				    }
				__ASSERT_DEBUG(current.iParaAttribs->PhraseCount()>1,User::Invariant());
				}
			}
		OstTrace1( TRACE_DUMP, DUP2_CRICHTEXTINDEX_SPLITPARAGRAPHATPASTEPOSL, "Leave code=%d", ret );
		User::Leave(ret);
		}
	if (insertPendingPos != EInsertCharFormatReset)
		ConsolidateAt(insertPendingPos, insertCharFormatDeleted?
			EPositionOnly : EFollowingPhrase);
	ScanToPosition(aPastePos.iDocPos+1,EScanToPositionMatchLeft);  // Gives us the next para.
	aNewPos=iPos;
	}


TInt CRichTextIndex::ParaLengthFromBuffer(TDesC& aBuf)const
// Returns the length of the first para found in the buffer.
// The returned length excludes the paragraph delimiter character.
// Returns KNotFound if there is no paragraph delimiter.
//
	{return aBuf.Locate(CEditableText::EParagraphDelimiter);}


void CRichTextIndex::InsertL(TInt aPos,const TPictureHeader& aPicHdr, TBool& aPictureOwnershipTaken)
// Updates the index following the insertion of a picture header object into the text
// component.  This is accomplished by creating & inserting a picture phrase at the
// relevant place.
//
	{
	__TEST_INVARIANT;

	RebalanceIndex();
// ASSERT: A valid picture header, referencing a valid picture has been inserted.
	if (!aPicHdr.iPicture.IsPtr() || aPicHdr.iPicture.AsPtr()==NULL)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXTINDEX_INSERTL, "EInsertNullPicHdrData" );
	    }
	__ASSERT_ALWAYS(aPicHdr.iPicture.IsPtr() && aPicHdr.iPicture.AsPtr()!=NULL,Panic(EInsertNullPicHdrData));
// ASSERT: The current insert pos hasn't been changed without cancelling SetInsertCharFormat.
	if ((iPendingNewPhrasePos!=EInsertCharFormatReset) && (aPos!=iPendingNewPhrasePos))
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CRICHTEXTINDEX_INSERTL, "ESetInsertCharFormatIntegrityErr" );
	    }
	__ASSERT_ALWAYS((iPendingNewPhrasePos==EInsertCharFormatReset) || (aPos==iPendingNewPhrasePos),
					Panic(ESetInsertCharFormatIntegrityErr));
	if (iPendingNewPhrasePos!=EInsertCharFormatReset)
		CancelInsertCharFormat();  // Cancel this state before inserting picture. Rebalances the index.
	ScanToPosition(aPos,EScanToPositionMatchLeft);
	TCurrentIndexRecords current; GetCurrentRecords(current);
	TCharFormatX format;
	TCharFormatXMask mask;  //...and build up its format.
	CCharFormatLayer* baseChar;
	GetPhraseFormat(current,format,mask,baseChar);  //...inherit format from prev. phrase.
	// Create the picture phrase. Takes ownership of the aPicHdr.iPicture
	CPicturePhrase* picture=CPicturePhrase::NewL(aPicHdr,format,mask,baseChar,aPictureOwnershipTaken);
	CleanupStack::PushL(picture);
	//New reclaimed CParaAttribs instance
	CParaAttribs* reclaimed=RequestReclaimShareL(current.iParaAttribs,current.iParaEntry);
    //Store the old CParaAttribs instance in rollbackParaAttribsHandle
	CParaAttribs* rollbackParaAttribsHandle=current.iParaAttribs;
	if (reclaimed)
		{
		CleanupStack::PushL(TCleanupItem(ReleaseOnCleanup,reclaimed));
		current.iParaEntry->iParaAttribs=reclaimed;  // Use this reclaimed para attribs (the new CParaAttribs instance)
		}
	GetCurrentRecords(current);
//	ASSERT: The reclaim succeeded.  We must always end up with a PhraseIx-not constant char format.
	if (current.iPhrase==NULL)
	    {
	    OstTrace0( TRACE_DUMP, DUP3_CRICHTEXTINDEX_INSERTL, "EReclaimShareError" );
	    }
	__ASSERT_DEBUG(current.iPhrase!=NULL,Panic(EReclaimShareError));
	TRAPD(ret1,
	SplitPhraseL(aPos));  // Phrase may not be split if at boundary.

    if (ret1!=KErrNone)
        {
        RbInsertPicture(rollbackParaAttribsHandle);//Restore the old CParaAttribs instance
        User::Leave(ret1);
        }

	TInt offset=(PhraseSplit())?1:0;  // Insert position of new phrase relative to current.
	RPhraseAttribsEntry newPhrase(picture);
	TRAPD(ret2,
	iPhraseIx->InsertL(iPos.iPhraseElement+offset,newPhrase));
    if (ret2!=KErrNone)
        {
        RbInsertPicture(rollbackParaAttribsHandle);//Restore the old CParaAttribs instance
        User::Leave(ret2);
        }

	if(reclaimed)
	    {
        CleanupStack::Pop();//"Pop" for CleanupStack::PushL(TCleanupItem(ReleaseOnCleanup,reclaimed));
	    }
	CleanupStack::Pop(picture);
	// Update counts etc. - cannot leave now.
	current.iParaEntry->iLength+=RPhraseAttribsEntry::EPicturePhraseLength;
	current.iParaAttribs->iPhraseCount++;  // for the picture phrase
	iPictureCount++;

	// Commit
	if(reclaimed)
	    {
        rollbackParaAttribsHandle->Release();  // Release hold on original shared paraAttribs.
	    }

	__TEST_INVARIANT;
	//coverity[memory_leak]
	}

void CRichTextIndex::RbInsertPicture(CParaAttribs* aGoodParaAttribs)
// Reinstate the original good paraAttribs.
// Then rollback the SplitPhrase() call if it succeeded.
//
	{
	(*iParaIx)[iPos.iParaElement].iParaAttribs=aGoodParaAttribs;
	if (PhraseSplit())
		{// Rollback the SplitPhrase()
		TInt length=(*iPhraseIx)[iPos.iPhraseElement+1].Length();
		RemoveFromPhraseIx(iPos.iPhraseElement+1);
		(*iPhraseIx)[iPos.iPhraseElement].AdjustLength(length);
		}
	}


// Insert a new paragraph immediately following character position aPos, fixing the length of the preceeding
// paragraph.  The new paragraph preserves any explicit paragraph/character formatting, and is based on the
// global layers.  (Do not need to rebalance the index here; a previous call to DoSoloInsertL accomplishes this)

void CRichTextIndex::InsertParagraphL(TInt aPos,const CParaFormatLayer& aGlobalParaFormatLayer)
	{
	ScanToPosition(aPos,EScanToPositionMatchLeft);
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	TParaAttribsEntry newPara;
	CCharFormatLayer* charLayer;
  	if (current.iPhrase)  // entry in phrase index
		charLayer=current.iPhrase->CharFormat();
	else
		charLayer=current.iParaAttribs->iCharFormat;
	//
	// New para format layer, based on normal, inheriting specific format
	CParaFormatLayer* currentParaFormat=current.iParaAttribs->iParaFormat;
	CParaFormatLayer* newParaLayer=CParaFormatLayer::NewL(currentParaFormat);
	const CParaFormatLayer& currentStyle=STATIC_CAST(const CParaFormatLayer&,*currentParaFormat->SenseBase());
	const TUid currentStyleType=currentStyle.Type();

	// !!
	// Only change to Normal if current style is a built-in one
	// or we are not at the end of a heading style.
	TBool useNormal;
	if (currentStyleType==KNormalParagraphStyleUid)
		useNormal=ETrue;
	else if (currentStyleType==KUserDefinedParagraphStyleUid)
		useNormal=EFalse;
	else if (iPos.iParaElementOffset<=(current.iParaEntry->iLength-2))  // cos of previous call to DoSoloInsertL()
		useNormal=EFalse;
	else
		useNormal=ETrue;
	newParaLayer->SetBase((useNormal)
		? &aGlobalParaFormatLayer
		: &currentStyle);
	const CCharFormatLayer* newCharBase=(useNormal)
		? iText.GlobalCharFormatLayer()
		: STATIC_CAST(const CParagraphStyle&,currentStyle).CharFormatLayer();
	CleanupStack::PushL(newParaLayer);
	//
	if (current.iParaAttribs->IsShared())
		{// Current para has constant char format - so the new one also has constant char format
		// New char format layer, based on normal, inheriting specific format
		CCharFormatLayer* newCharLayer=CCharFormatLayer::NewL(charLayer);
		newCharLayer->SetBase(newCharBase);
		CleanupStack::PushL(newCharLayer);
		newPara.iParaAttribs=GetParaAttribsL(newParaLayer,newCharLayer);
		CleanupStack::PopAndDestroy(2);  // newCharLayer/newParaLayer
		CleanupStack::PushL(TCleanupItem(ReleaseOnCleanup,newPara.iParaAttribs));
		iParaIx->InsertL(iPos.iParaElement+1,newPara);
		CleanupStack::Pop();  // paraAttribs cleanup item
		GetCurrentRecords(current);		// could be changed by InsertL() above
		}
	else  // Do the split myself since this para has specific character formatting.
		{// Make the new CParaAttribs
		CParaAttribs* newParaAttribs=CParaAttribs::NewL(newParaLayer);
		CleanupStack::PopAndDestroy();  // newParaLayer - copy owned by newParaAttribs
		CleanupStack::PushL(TCleanupItem(ReleaseOnCleanup,newParaAttribs));
		//
		// Split current phrase & insert if necessary.
		// Split even when we are at a phrase boundary -> this introduces an z.l.p. for the insertion point
		RPhraseAttribsEntry& insertPhrase=iPhraseIx->At(iPos.iPhraseElement);
		TInt insertPendingPos=(iPos.iPhraseElementOffset==insertPhrase.Length()) ? aPos : EInsertCharFormatReset;
		DoSplitPhraseL(insertPhrase,iPos.iPhraseElementOffset,current.iParaAttribs);  // Ups iPhraseCount
		//
		// Insert the new paragraph.
		newPara.iParaAttribs=newParaAttribs;
		TRAPD(ret,
		iParaIx->InsertL(iPos.iParaElement+1,newPara));  // Inserts the new paraAttribsEntry record.
			if (ret!=KErrNone)
				{
				RemoveFromPhraseIx(iPos.iPhraseElement+1,1);	// inserted by DoSplitPhraseL
				current.iParaAttribs->iPhraseCount--;
				User::Leave(ret);
				}

		iPendingNewPhrasePos=insertPendingPos;
		CleanupStack::Pop();			// newParaAttribs. All OK now
		GetCurrentRecords(current);		// could be changed by InsertL() above
		//
		// Calculate new paraAttribs phrase counts.
		TInt remainder=(iPos.iPhraseElement+1)-iPos.iParaBasePhraseElement;
		TInt newPhraseCount=current.iParaAttribs->iPhraseCount-remainder;
		newParaAttribs->iPhraseCount=newPhraseCount;
		current.iParaAttribs->iPhraseCount=remainder;

		const CArrayFix<RPhraseAttribsEntry>& phraseIx=*iPhraseIx;
		TInt startPhrase=iPos.iPhraseElement+1;
		for (TInt ii=startPhrase; ii<startPhrase+newPhraseCount; ii++)
			{
			RPhraseAttribsEntry phrase=phraseIx[ii];
			phrase.CharFormat()->SetBase(newCharBase);
			}
		//
		// The index now reflects the correct state.
		// Next, the efficiency thing - see if the new paras can share existing ones.
		if (newPhraseCount==1)
			Share(iParaIx->At(iPos.iParaElement+1),iPos.iParaBasePhraseElement+remainder);
		if (remainder==1)
			Share(iParaIx->At(iPos.iParaElement),iPos.iParaBasePhraseElement);
		}
	// Alter the length of the original paragraph and the new paragraph.
	TInt currentLength=current.iParaEntry->iLength;
	current.iParaEntry->iLength=iPos.iParaElementOffset;
	((*iParaIx)[iPos.iParaElement+1]).iLength+=currentLength-current.iParaEntry->iLength;  // Alters the length of the copy of aNewPara.
	}


void CRichTextIndex::SetForDeleteL(TIndexDeleteInfo& aInfo,TInt aPos,TInt aLength)
//
	{
	__TEST_INVARIANT;  // Do not need to RebalanceIndex(); part of defined behaviour for delete.

	aInfo.iDeleteLength=aLength;
	//
	// Check for simple cases first
	DocumentChanged();  // clears internal position record.
	ScanToPosition(aPos,EScanToPositionAbsolute,&iLastUsed);
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	aInfo.iStartPara=iPos.iParaElement;
	aInfo.iEndPara=iPos.iParaElement;  // default
	aInfo.iDeletePos=iPos;  // default
	//
	TInt startParaLength=current.iParaEntry->iLength;
	TInt lengthRemainingInPara=startParaLength-iPos.iParaElementOffset;
	if (aLength<lengthRemainingInPara)
		{// Case is delete-from-paragraph
		aInfo.iDeleteType=TIndexDeleteInfo::EDeleteFromParagraph;
		return;
		}
	//
	ScanToPosition(aPos+aLength,EScanToPositionMatchLeft,&iLastUsed);  // Forces endPara to be next para when just removing a para delimiter.
	aInfo.iEndPara=iPos.iParaElement;
//	if (iPos.iParaElementOffset==0)
//		{// Can use delete-paragraph
//		aInfo.iDeleteType=TIndexDeleteInfo::EDeleteParagraph;
//		return;
//		}
	//
	// Set for the general (leaving) delete.
	GetCurrentRecords(current);
	CParaAttribs* reclaimedEndPara=RequestReclaimShareL(current.iParaAttribs,current.iParaEntry); // does not release share.
	TParaAttribsEntry* origEndParaEntry=current.iParaEntry;
	CParaAttribs* origParaAttribs=current.iParaAttribs;
	TInt endPosPhrase=iPos.iPhraseElement;
	if (reclaimedEndPara)
		origEndParaEntry->iParaAttribs=reclaimedEndPara;
	// Get start para info.
	ScanToPosition(aPos,EScanToPositionAbsolute);
	GetCurrentRecords(current);
	CParaAttribs* reclaimedStartPara=NULL;
	TRAPD(ret,
	reclaimedStartPara=RequestReclaimShareL(current.iParaAttribs,current.iParaEntry));
		if (ret!=KErrNone)
			{
			if (reclaimedEndPara)
				{
				reclaimedEndPara->Release();
				RemoveFromPhraseIx(endPosPhrase);
				origEndParaEntry->iParaAttribs=origParaAttribs;
				}
			User::Leave(ret);
			}
	if (reclaimedEndPara)
		origParaAttribs->Release();  // Release share on the original end para attribs
	if (reclaimedStartPara)
		{// Use the specific start  para
		current.iParaAttribs->Release();
		current.iParaEntry->iParaAttribs=reclaimedStartPara;
		ScanToPosition(aPos,EScanToPositionAbsolute);  // Pick up reclaimed phrase.
		}
	aInfo.iDeletePos=iPos;  // internal position of aPos after any reclaim
	// Note: iDeleteType can surely be made obsolete now? TPB 7/11/2000
	aInfo.iDeleteType=TIndexDeleteInfo::EDeleteFromParagraph;
	
	/*
	 * Pointer to memory allocated to 'reclaimedEndPara' is assigned to 
	 * 'origEndParaEntry->iParaAttribs' on line 706. The memory will be 
	 * released in CRichTextIndex's destructor.
	 */ 
	// coverity[memory_leak]
	}


TBool CRichTextIndex::DeleteParagraph(TInt aPos,TInt aLength)
// Remove aCount entire paragraphs from the text.
// Leave-safe
// Returns EFalse indicating that no paragraphs were merged together,
// as a result of the delete action.
// Does NOT preserve any zero-length/insert pending state.
//
	{
	__TEST_INVARIANT;  // Do not need to RebalanceIndex(); part of defined behaviour for delete.

	CancelInsertCharFormat();
	ScanToPosition(aPos,EScanToPositionAbsolute,&iLastUsed);

	if (iPos.iParaElementOffset!=0)
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_DELETEPARAGRAPH, "EDeleteParagraphInvalidStartValue" );
	    }
	__ASSERT_DEBUG(iPos.iParaElementOffset==0,Panic(EDeleteParagraphInvalidStartValue));

	TIndexDeleteInfo info;
	info.iDeleteType=TIndexDeleteInfo::EDeleteParagraph;
	info.iDeletePos=iPos;
	info.iStartPara=iPos.iParaElement;
	//
	TInt documentLength=iText.DocumentLength();
	TInt pos=(aPos+aLength>documentLength
		? documentLength
		: aPos+aLength);
	ScanToPosition(pos,EScanToPositionMatchLeft,&iLastUsed);  // Forces endPara to be next para when just removing a para delimiter.

	info.iEndPara=iPos.iParaElement;
	info.iDeleteLength=aLength;

	DeleteNow(info);
	// do not want to call TidyAfterDelete()

	return EFalse;
	}



void CRichTextIndex::DeleteFromParagraph(TInt aPos,TInt aLength)
// Special case delete for removing content from within a single paragraph only.
// Not to be used for deleting an entire paragraph or paragraphs.
// Returns EFalse indicating that no paragraphs were merged together,
// as a result of the delete action.
//
	{
	__TEST_INVARIANT;  // Do not need to RebalanceIndex(); part of defined behaviour for delete.

	ScanToPosition(aPos,EScanToPositionAbsolute);

#ifdef _DEBUG
	{
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	TInt startParaLength=current.iParaEntry->iLength;
	TInt lengthRemainingInPara=startParaLength-iPos.iParaElementOffset;

	if (aLength>=lengthRemainingInPara)
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXTINDEX_DELETEFROMPARAGRAPH, "EDeleteFromParagraphInvalidRange" );
	    }
	__ASSERT_ALWAYS(aLength<lengthRemainingInPara,Panic(EDeleteFromParagraphInvalidRange));
	}
#endif

	TIndexDeleteInfo info;
	info.iDeleteLength=aLength;
	info.iStartPara=iPos.iParaElement;
	info.iEndPara=iPos.iParaElement;
	info.iDeletePos=iPos;
	info.iDeleteType=TIndexDeleteInfo::EDeleteFromParagraph;

	DoDeleteFromParagraph(info);

	__TEST_INVARIANT;
	}


TBool CRichTextIndex::DoDeleteFromParagraph(const TIndexDeleteInfo& aInfo)
// Delete content from *within* the boundary of a single paragraph only.0
// Returns EFalse indicating that no paragraphs were merged together,
// as a result of the delete action.
//
	{
	iPos=aInfo.iDeletePos;
	TInt length=aInfo.iDeleteLength;
	DeleteParagraphText(length);
	TidyAfterDelete(aInfo);

	return EFalse;
	}


TBool CRichTextIndex::DeleteNow(TIndexDeleteInfo& aInfo)
// Deletes index data corresponding the info argument.
// Returns ETrue is 2 paragraphs are merged as a result of the delete, otherwise returns false.
//
	{
	iPos=aInfo.iDeletePos;
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	TInt leftToDelete=aInfo.iDeleteLength;
	TInt charsLeftInPara=(current.iParaEntry->iLength)-(iPos.iParaElementOffset);
	TBool doParaMerge=((iPos.iPhraseElement>0 || iPos.iPhraseElementOffset>0) && leftToDelete>=charsLeftInPara);
	// ETrue if the 1st para has content remaining but no paragraph delimiter.
	//
	TBool firstParaRemoved=(FirstPhraseOfParagraph() && iPos.iPhraseElementOffset==0 && aInfo.iDeleteLength>=current.iParaEntry->iLength);
	// ETrue if the 1st para has been *wholly* deleted.
	//
	DeleteParagraphText(leftToDelete);  // Delete range will be in a minimum of 1 paragraph.
	if (aInfo.iStartPara<aInfo.iEndPara)
		{// The delete range crosses paragraph boundaries.
		for (TInt currentPara=aInfo.iStartPara+1;currentPara<=aInfo.iEndPara;currentPara++)
			{
			ScanToPosition(aInfo.iDeletePos.iDocPos,EScanToPositionAbsolute);
			DeleteParagraphText(leftToDelete);
			}
		}
	// Now tidy up
	if (doParaMerge && !firstParaRemoved)
		{// Merge the 2 paras together.
		TParaAttribsEntry* paraEntry=&(*iParaIx)[aInfo.iStartPara];
		TParaAttribsEntry* paraEntryFollowing=&(*iParaIx)[aInfo.iStartPara+1];
		paraEntryFollowing->iLength+=paraEntry->iLength;  // Extend length of remaining para.
		paraEntryFollowing->iParaAttribs->iPhraseCount+=paraEntry->iParaAttribs->iPhraseCount;  // Extend phrase count
		paraEntry->iParaAttribs->Release();
		iParaIx->Delete(aInfo.iStartPara);
		}
	if (aInfo.iDeleteType!=TIndexDeleteInfo::EDeleteParagraph)
		TidyAfterDelete(aInfo);

	__TEST_INVARIANT;
	return doParaMerge;
	}


void CRichTextIndex::TidyAfterDelete(const TIndexDeleteInfo& aInfo)
//
//
	{
	MergePhrases(aInfo.iDeletePos.iDocPos);  // Alters internal position record.
	TCurrentIndexRecords current;
	GetCurrentRecords(current);  // So must get records again.
	if (!current.iParaAttribs->IsShared())
		{// May be able to reclaim a share from this *specific* record
		CParaAttribs* sharedParaAttribs=RequestShare(iPos);
		if (sharedParaAttribs!=NULL && current.iParaAttribs!=sharedParaAttribs)
			{// Use this shared record
			current.iParaAttribs->Release();
			RemoveFromPhraseIx(iPos.iPhraseElement);
			current.iParaEntry->iParaAttribs=sharedParaAttribs;
			}
		}
	}


void CRichTextIndex::Normalize(TInt aPos)
//
	{
	ScanToPosition(aPos,EScanToPositionAbsolute);
	NormalizeNow(iPos);

	__TEST_INVARIANT;
	}


void CRichTextIndex::NormalizeNow(const TLogicalPosition& aNormalizePos)
//
	{
	CParaAttribs* currentParaAttribs=(*iParaIx)[aNormalizePos.iParaElement].iParaAttribs;
	if (!currentParaAttribs->IsShared())
		{
		CParaAttribs* sharedParaAttribs=RequestShare(iPos);
		if (sharedParaAttribs!=NULL && currentParaAttribs!=sharedParaAttribs)
			{// We must have been given a share on something already in the shared list.  Dump current stuff.
			currentParaAttribs->Release();
			(*iParaIx)[aNormalizePos.iParaElement].iParaAttribs=sharedParaAttribs;
			RemoveFromPhraseIx(aNormalizePos.iParaBasePhraseElement);
			}
		}
	}


CParaAttribs* CRichTextIndex::ReserveCellLC()
// Returns a handle to a newly created CParaAttribs object.  This may be used
// during a call to GetParaAttribsL() as a pre-allocated cell, thus ensuring
// that the call cannot possibly leave.
// ASSUMES: that the internal position record has been set correctly.
//
	{
	CParaAttribs* reservedCell=ReserveCellL();
	CleanupStack::PushL(TCleanupItem(ReleaseOnCleanup,reservedCell));
	return reservedCell;
	}


CParaAttribs* CRichTextIndex::ReserveCellL()
// Returns a handle to a newly created CParaAttribs object.  This may be used
// during a call to GetParaAttribsL() as a pre-allocated cell, thus ensuring
// that the call cannot possibly leave.
// ASSUMES: that the internal position record has been set correctly.
//
	{
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	const CParaAttribs& paraAttribs=*current.iParaAttribs;
	CParaFormatLayer* paraLayer=paraAttribs.iParaFormat;
	CCharFormatLayer* charLayer=(paraAttribs.IsShared())
								? paraAttribs.iCharFormat
								: (*iPhraseIx)[iPos.iPhraseElement].CharFormat();
	CParaAttribs* reservedCell=CParaAttribs::NewL(paraLayer,charLayer);
	return reservedCell;
	}


TBool CRichTextIndex::DeleteParagraphText(TInt& aLength)
// Called once for each paragraph that's included in the delete range.
// Assumes the internal position record has already been set correctly.
// The delete range may cover: (1) The entire paragraph - so just destroy the paragraph,
// (2i) At least a portion of a single phrase, and possibly
// (2ii) 0..n whole contiguous phrases, and possibly
// (2iii) a trailing partial phrase.
// Returns ETrue if the full paragraph is deleted, otherwise returns EFalase.
//
	{
	TCurrentIndexRecords current; GetCurrentRecords(current);
	if (FirstPhraseOfParagraph() && iPos.iPhraseElementOffset==0 && aLength>=current.iParaEntry->iLength)
		{// The entire paragraph needs to be deleted.
		aLength-=current.iParaEntry->iLength;
		if (!current.iParaAttribs->IsShared())
			RemoveFromPhraseIx(iPos.iParaBasePhraseElement,current.iParaAttribs->iPhraseCount);
		current.iParaAttribs->Release();
		iParaIx->Delete(iPos.iParaElement);
		return ETrue;
		}
	TInt deleteFromPhrase=CurrentPhraseLength()-iPos.iPhraseElementOffset;
	TInt maxPhrase=current.iParaAttribs->PhraseCount();
	TInt currentPhrase=(iPos.iPhraseElement-iPos.iParaBasePhraseElement);
	while (currentPhrase<maxPhrase)
		{
		TInt deletable=Min(deleteFromPhrase,aLength);
		current.iParaEntry->iLength-=deletable;  // Adjust the paragraph length.
		if (current.iPhrase && deletable>=CurrentPhraseLength())
			{// Remove the now empty phrase from the phrase index.
			RemoveFromPhraseIx(iPos.iPhraseElement);
			current.iParaAttribs->iPhraseCount--;
			}
		else if (current.iPhrase)
			{// Adjust phrase length and move onto the next phrase.
			current.iPhrase->AdjustLength(-deletable);
			iPos.iPhraseElement++;
			iPos.iPhraseElementOffset=0;
			}
		currentPhrase++;
		aLength-=deletable;
		if(aLength==0)
			break;  // Nothing left to delete in this paragraph.
		// Get the data for the next phrase.
		GetCurrentRecords(current);
		deleteFromPhrase=CurrentPhraseLength();
		}
	return EFalse;
	}

TBool CRichTextIndex::InsertCharFormatIsActive()
	{
	return iPendingNewPhrasePos != EInsertCharFormatReset;
	}

/** Sets an *InsertPending* state, where format has been inserted into the
text, but no content has yet been inserted. This *state* is cancelled by cursor
movement etc. Split the current phrase at aPos (if necessary) and insert a zero
length phrase, ready to accept the pending content of the specified format.
*/
void CRichTextIndex::SetInsertCharFormatL(const TCharFormatX& aFormat,const TCharFormatXMask& aMask,TInt aPos)
	{
	if (InsertCharFormatIsActive() && aPos!=iPendingNewPhrasePos)
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXTINDEX_SETINSERTCHARFORMATL, "ESetInsertCharFormatIntegrityErr" );
	    }
	__ASSERT_ALWAYS(!InsertCharFormatIsActive() || aPos==iPendingNewPhrasePos,
					Panic(ESetInsertCharFormatIntegrityErr));
	if (InsertCharFormatIsActive())
		UpdateInsertCharFormatL(aFormat, aMask);
	else
		NewInsertCharFormatL(aFormat, aMask, aPos);
	}

void CRichTextIndex::NewInsertCharFormatL(const TCharFormatX& aFormat,
	const TCharFormatXMask& aMask, TInt aPos)
	{
	if (InsertCharFormatIsActive())
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXTINDEX_NEWINSERTCHARFORMATL, "ESetInsertCharFormatIntegrityErr" );
	    }
	__ASSERT_ALWAYS(!InsertCharFormatIsActive(),
		Panic(ESetInsertCharFormatIntegrityErr));
	ScanToPosition(aPos,EScanToPositionMatchLeft);
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	CCharFormatLayer* basedOn;
	TCharFormatX applyFormat=aFormat;
	TCharFormatXMask applyMask=aMask;
	GetPhraseFormat(current,applyFormat,applyMask,basedOn);  // Inherit phrase attributes to the left, over what is present.
	TBool origParaAttribsShared=current.iParaAttribs->IsShared();
	if (origParaAttribsShared)
		{// Current paraAttribs is shared.
		iRollbackParaAttribsHandle=current.iParaAttribs;
		current.iParaEntry->iParaAttribs=RequestReclaimShareL(current.iParaAttribs,current.iParaEntry);  // Does not release share.
		ScanToPosition(aPos,EScanToPositionMatchLeft);  // Pick up reclaimed phrase.
		}  // Now current.iParaAttribs has specific character formatting - guaranteed.
	GetCurrentRecords(current);
	TRAPD(ret, DoNewInsertCharFormatL(applyFormat, applyMask,
		basedOn, current.iParaAttribs));
	if (ret!=KErrNone)
		{// Rollback as if this function call never happened.
		if (origParaAttribsShared)
			{// Revert back to sharing the original.
			current.iParaAttribs->Release();
			RemoveFromPhraseIx(iPos.iPhraseElement);
			current.iParaEntry->iParaAttribs=iRollbackParaAttribsHandle;
			}
		else
			{// Restore the original phrase index.
			if (PhraseSplit())
				MergePhrases(aPos);
			}
		OstTrace0( TRACE_FATAL, DUP1_CRICHTEXTINDEX_NEWINSERTCHARFORMATL, "LeaveNoMemory" );
		User::LeaveNoMemory();
		}
	iPendingNewPhrasePos=aPos;
	}

void CRichTextIndex::UpdateInsertCharFormatL(const TCharFormatX& aFormat,
	const TCharFormatXMask& aMask)
	{
	CCharFormatLayer* currentLayer = GetCurrentInsertCharFormat();
	CCharFormatLayer* newLayer = CCharFormatLayer::NewCopyBaseL(currentLayer);
	CleanupStack::PushL(newLayer);
	newLayer->SetL(aFormat, aMask);
	currentLayer->Swap(*newLayer);
	CleanupStack::PopAndDestroy(newLayer);
	}

CCharFormatLayer* CRichTextIndex::GetCurrentInsertCharFormat()
	{
	if (!InsertCharFormatIsActive())
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_GETCURRENTINSERTCHARFORMAT, "ESetInsertCharFormatIntegrityErr" );
	    }
	__ASSERT_DEBUG(InsertCharFormatIsActive(),
		Panic(ESetInsertCharFormatIntegrityErr));
	ScanToPosition(iPendingNewPhrasePos,EScanToPositionMatchLeft);
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	if ((*iPhraseIx)[iPos.iPhraseElement].Length() != 0)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CRICHTEXTINDEX_GETCURRENTINSERTCHARFORMAT, "ESetInsertCharFormatIntegrityErr" );
	    }
	__ASSERT_DEBUG((*iPhraseIx)[iPos.iPhraseElement].Length() == 0,
		Panic(ESetInsertCharFormatIntegrityErr));
	return (*iPhraseIx)[iPos.iPhraseElement].CharFormat();
	}

void CRichTextIndex::DoNewInsertCharFormatL(const TCharFormatX& aFormat,const TCharFormatXMask& aMask,
											CCharFormatLayer* aBasedOn,CParaAttribs* aParaAttribs)
	{
	SplitPhraseL(iPos.iPhraseElement,iPos.iPhraseElementOffset,aParaAttribs);
	CCharFormatLayer* layer=CCharFormatLayer::NewL();
	layer->SetBase(aBasedOn);  // must be done before the SetL().
	CleanupStack::PushL(layer);
	layer->SetL(aFormat,aMask);
	RPhraseAttribsEntry pendingNewPhrase(layer);
	TInt pendingNewPhraseElement=(FirstPhraseOfParagraph() && iPos.iPhraseElementOffset==0)
		?iPos.iParaBasePhraseElement:iPos.iPhraseElement+1;
	iPhraseIx->InsertL(pendingNewPhraseElement,pendingNewPhrase);
	CleanupStack::Pop();
	aParaAttribs->iPhraseCount++;
	}


void CRichTextIndex::RebalanceIndex()
// Returns the index to a good state, by releasing the extra share taken on the paraAttribs
//
	{
	if (iRollbackParaAttribsHandle)
		{
		// ASSERT: The specified para attribs is indeed in the share list.
		if (!iRollbackParaAttribsHandle->IsShared())
		    {
		    OstTrace0( TRACE_FATAL, CRICHTEXTINDEX_REBALANCEINDEX, "EParaAttribsNotInSharedList" );
		    }
		__ASSERT_ALWAYS(iRollbackParaAttribsHandle->IsShared(),Panic(EParaAttribsNotInSharedList));
		iRollbackParaAttribsHandle->Release();
		iRollbackParaAttribsHandle=NULL;
		}
	}

/** Cancels the transitory state where a specified character format is applied
on top of any inherited formatting. eg, when bold is on. Cancel when: (1) the
text position is altered. (2) the first character (or picture) has been
inserted following the setting of this state. If a zero length phrase is
removed OR has content entered into it, the newly abutting phrases are checked
to see if they can be merged. Then a request share of this para is issued.
*/
void CRichTextIndex::CancelInsertCharFormat()
	{
	if (InsertCharFormatIsActive())
		{
		TBool isDeleted = DeleteInsertCharFormat();
		ConsolidateAt(iPendingNewPhrasePos, isDeleted?
			EPositionOnly : EFollowingPhrase);
		iPendingNewPhrasePos = EInsertCharFormatReset;
		}
	}

/** Attempts to delete a zero-length phrase at the insert character format
position. Does not delete any phrase of non-zero length.
@pre The insert character format must be active
@return ETrue if a zero-length phrase was deleted.
*/
TBool CRichTextIndex::DeleteInsertCharFormat()
	{
	if (!InsertCharFormatIsActive())
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_DELETEINSERTCHARFORMAT, "Invariant" );
	    }
	__ASSERT_DEBUG(InsertCharFormatIsActive(), User::Invariant());
	ScanToPosition(iPendingNewPhrasePos,EScanToPositionMatchLeft);
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	if (current.iPhrase && current.iPhrase->Length() == 0)
		{
		RemoveFromPhraseIx(iPos.iPhraseElement);
		current.iParaAttribs->iPhraseCount--;  // Para has 1 less phrase in it now.
		return ETrue;
		}
	return EFalse;
	}

/** Attempts to merge phrases and share paragraphs.
@param aPosition
	Phrase boundary here is merged if possible, paragraph here is shared if
	possible.
@param aPositionOrPhrase
	If EPositionOnly the phrases either side of aPosition are considered for merging. If EFollowingPhrase,
	the end of the phrase following aPosition is also considered.
*/
void CRichTextIndex::ConsolidateAt(TInt aPosition,
	TPositionOrPhrase aPositionOrPhrase)
	{
	ScanToPosition(aPosition, EScanToPositionAbsolute);
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	if (!current.iPhrase)
		return;

	TInt length = current.iPhrase->Length();
	MergePhrases(aPosition);
	if (aPositionOrPhrase == EFollowingPhrase)
		{
		ScanToPosition(aPosition, EScanToPositionAbsolute);
		GetCurrentRecords(current);
		if (current.iPhrase)
			MergePhrases(aPosition + length);
		}
	Normalize(aPosition);
	RebalanceIndex();
	}

TBool CRichTextIndex::DelSetInsertCharFormatL(TInt aPos,TInt aLength)
// Delete aLength characters, commencing at, and including, aPos.
// Adds value by the following behaviour:
// If aPos is on a phrase boundary, then remember temporarily the phrase format.
// This is applied to any content that is immediately inserted.
//
	{
	__TEST_INVARIANT;

	CancelInsertCharFormat();
	ScanToPosition(aPos,EScanToPositionAbsolute);
	TCurrentIndexRecords current; GetCurrentRecords(current);
	if ((!current.iParaAttribs->IsShared()) && iPos.iPhraseElementOffset==0)
		{// aPos is on phrase boundary so SetState.
		TCharFormatX format;
		TCharFormatXMask mask;
		CCharFormatLayer* charBase;
		GetPhraseFormat(current,format,mask,charBase);
		SetInsertCharFormatL(format,mask,aPos);
		}
	TIndexDeleteInfo deleteInfo;
	SetForDeleteL(deleteInfo,aPos,aLength);
	TBool parasMerged=DeleteNow(deleteInfo);

	__TEST_INVARIANT;
	return parasMerged;
	}


void CRichTextIndex::ApplyParaFormatL(const CParaFormat* aFormat,const TParaFormatMask& aMask,TInt aPos,TInt aLength)
// Applies the specified format attributes to the paragraphs covering character position aPos to aPos+aLength-1.
// Preserves any attributes that are currently stored in this layer.
// If the specified para(s) is in the shared list, a new shared para of the desired format must be created,
// and the usage count of the original decremented.
//
	{
	__TEST_INVARIANT;

	TInt offset=(aLength==0)?0 :-1;
	TInt endPara=OwningParagraph(aPos+(aLength+offset));
	TInt paraItem=OwningParagraph(aPos);
	TCurrentIndexRecords current; GetCurrentRecords(current);
	CParaFormat* pf=CParaFormat::NewL(*aFormat);  // preserve the desired tablist.
	CleanupStack::PushL(pf);
	for (;paraItem<=endPara;paraItem++)
		{// For each paragraph, apply the specified format.
		TParaFormatMask applyMask=aMask;
		CParaAttribs* currentParaAttribs=(*iParaIx)[paraItem].iParaAttribs;
		TBool shared=currentParaAttribs->IsShared();
		if (!shared)
			{
			currentParaAttribs->iParaFormat->SenseL(pf,applyMask);
			currentParaAttribs->iParaFormat->SetL(pf,applyMask);
			}
		else
			{// Must create a new shared para attribs of the specified format
			// Make a new para format layer
			currentParaAttribs->iParaFormat->SenseL(pf,applyMask);
			CParaFormatLayer* newParaLayer=CParaFormatLayer::NewL(pf,applyMask);
			newParaLayer->SetBase(currentParaAttribs->iParaFormat->SenseBase());
			CleanupStack::PushL(newParaLayer);
			// Make a new char format layer
			CCharFormatLayer* newCharLayer=CCharFormatLayer::NewL(currentParaAttribs->iCharFormat);
			newCharLayer->SetBase(currentParaAttribs->iCharFormat->SenseBase());
			CleanupStack::PushL(newCharLayer);
			//
			CParaAttribs* sharedParaAttribs=GetParaAttribsL(newParaLayer,newCharLayer);
			CleanupStack::PopAndDestroy(2);
			if (sharedParaAttribs)
				(*iParaIx)[paraItem].iParaAttribs=sharedParaAttribs;
			currentParaAttribs->Release();
			}
		}
	CleanupStack::PopAndDestroy();  // pf
	__TEST_INVARIANT;
	}


void CRichTextIndex::ApplyParagraphStyleL(const CParagraphStyle& aStyle,TInt aPos,TInt aLength,
										  const CCharFormatLayer* aCharStyleNormal,CParagraphStyle::TApplyParaStyleMode aMode)
// Applies the specified paragraph style to the paragraphs covering
// character positions aPos to aPos+aLength-1.
// Alters the specific formatting of the covered paragraphs as specified by aMode.
//
	{
	__TEST_INVARIANT;

	TInt offset=(aLength==0)?0 :-1;
	TInt endPara=OwningParagraph(aPos+(aLength+offset));
	TInt paraItem=OwningParagraph(aPos);
	TInt paragraphBasePhrase=iPos.iParaBasePhraseElement;
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	for (;paraItem<=endPara;paraItem++)
		{// For each paragraph, apply the specified style
		CParaAttribs& currentParaAttribs=*(*iParaIx)[paraItem].iParaAttribs;
		TBool shared=currentParaAttribs.IsShared();
		TUid type=aStyle.Type();
		if (!shared)
			{
			currentParaAttribs.iParaFormat->SetBase(&aStyle);
			TInt phraseCount=currentParaAttribs.PhraseCount();
			for (TInt phraseItem=0;phraseItem<phraseCount;phraseItem++)
				{
				CCharFormatLayer& charLayer=*(*iPhraseIx)[paragraphBasePhrase+phraseItem].CharFormat();
				if (type==KNormalParagraphStyleUid)
					charLayer.SetBase(aCharStyleNormal);
				else
					charLayer.SetBase(aStyle.CharFormatLayer());
				ModifySpecificFormatting(*currentParaAttribs.iParaFormat,charLayer,aMode);
				}
			paragraphBasePhrase+=phraseCount;
			}
		else
			{// Must create a new shared para attribs of the same format, but a different based on link
			// Make a new para format layer
			CParaFormatLayer* newParaLayer=NULL;
			if (aMode==CParagraphStyle::ERetainNoSpecificFormats || aMode==CParagraphStyle::ERetainSpecificCharFormat)
				newParaLayer=CParaFormatLayer::NewL();
			else
				newParaLayer=CParaFormatLayer::NewL(currentParaAttribs.iParaFormat);
			newParaLayer->SetBase(&aStyle);
			CleanupStack::PushL(newParaLayer);
			//
			// Make a new char format layer
			CCharFormatLayer* newCharLayer=NULL;
			if (aMode==CParagraphStyle::ERetainNoSpecificFormats || aMode==CParagraphStyle::ERetainSpecificParaFormat)
				newCharLayer=CCharFormatLayer::NewL();
			else
				newCharLayer=CCharFormatLayer::NewL(currentParaAttribs.iCharFormat);
			if (type==KNormalParagraphStyleUid)
				newCharLayer->SetBase(aCharStyleNormal);
			else
				newCharLayer->SetBase(aStyle.CharFormatLayer());
			CleanupStack::PushL(newCharLayer);
			//
			(*iParaIx)[paraItem].iParaAttribs=GetParaAttribsL(newParaLayer,newCharLayer);
			CleanupStack::PopAndDestroy(2);
			currentParaAttribs.Release();
			}
		}
	}


void CRichTextIndex::ModifySpecificFormatting(CParaFormatLayer& aPl,CCharFormatLayer& aCl,CParagraphStyle::TApplyParaStyleMode aMode)
//
//
	{
	switch(aMode)
		{
		case(CParagraphStyle::ERetainNoSpecificFormats):
			aPl.Reset();
			aCl.Reset();
			break;
		case(CParagraphStyle::ERetainSpecificParaFormat):
			aCl.Reset();
			break;
		case(CParagraphStyle::ERetainSpecificCharFormat):
			aPl.Reset();
			break;
		case(CParagraphStyle::ERetainAllSpecificFormats):
		default:
			break;
		}
	}


/*
For every paragraph in the document: (i) if it uses the style aFrom, make it use the style aTo,
or the global style if aTo is null; (ii) if any phrase in the paragraph has a character format
based on the character format owned by aFrom, change it so that it is based on the character
format owned by aTo, or the global character format if aTo is null.

The action described in (ii) should only occur for an 'orphaned' character insertion format; that is
an insertion format left after deletion of a block in a certain style that is itself then deleted.
*/
void CRichTextIndex::NotifyStyleChangedL(const CParagraphStyle* aTo,const CParagraphStyle* aFrom,
										 const CParaFormatLayer& aGlobalParaFormatLayer,
										 const CCharFormatLayer& aGlobalCharFormatLayer)
	{
	__TEST_INVARIANT;

	TInt paraCount=ParagraphCount();
	TInt currentPhrase = 0;
	const CCharFormatLayer* oldCharFormatLayer = aFrom->CharFormatLayer();
	const CParaFormatLayer* newParFormatLayer = aTo ? aTo : &aGlobalParaFormatLayer;
	const CCharFormatLayer* newCharFormatLayer = aTo ? aTo->CharFormatLayer() : &aGlobalCharFormatLayer;
	for (TInt paraItem = 0;paraItem < paraCount; paraItem++)
		{
		TParaAttribsEntry* currentPara = &(*iParaIx)[paraItem];
		CParaAttribs* currentParaAttribs = currentPara->iParaAttribs;

		TBool changeParStyleBase = currentParaAttribs->iParaFormat->SenseBase() == aFrom;
		if (!currentParaAttribs->IsShared())
			{
			if (changeParStyleBase)
				currentParaAttribs->iParaFormat->SetBase(newParFormatLayer);
			TInt phraseCount = currentParaAttribs->PhraseCount();
			for (TInt phraseItem = currentPhrase; phraseItem < (currentPhrase + phraseCount); phraseItem++)
				{
				CCharFormatLayer* charFormat = (*iPhraseIx)[phraseItem].CharFormat();
				if (charFormat->SenseBase() == oldCharFormatLayer)
					charFormat->SetBase(newCharFormatLayer);
				}
			currentPhrase += phraseCount;
			}
		else
			{	// Maintain the shared list reference
			CParaAttribs* resultantParaAttribs = CParaAttribs::NewL(currentParaAttribs);
			CleanupStack::PushL(TCleanupItem(ReleaseOnCleanup,resultantParaAttribs));
			if (changeParStyleBase)
				resultantParaAttribs->iParaFormat->SetBase(newParFormatLayer);
			if (resultantParaAttribs->iCharFormat->SenseBase() == oldCharFormatLayer)
				resultantParaAttribs->iCharFormat->SetBase(newCharFormatLayer);
			CParaAttribs* shared = RequestShareL(resultantParaAttribs); // will return a non-NULL handle
			__ASSERT_DEBUG(shared,Panic(EDebug));
			currentParaAttribs->Release();
			iSharedParaQueHead.AddLast(*resultantParaAttribs); // allows correct release of cell.
			CleanupStack::PopAndDestroy();
			currentPara->iParaAttribs = shared;
			}
		}

	__TEST_INVARIANT;
	}


const CParaFormatLayer* CRichTextIndex::ParagraphStyle(TBool& aStyleChangesOverRange,
															   TInt aPos,
															   TInt aLength)const
// Return the handle of the first paragraph style encountered in the specified range.
// Set aStyleChangesOverRange to ETrue, if different paragraph styles are encountered
// across the specified range, otherwise set it to EFalse.
//
	{
	__TEST_INVARIANT;

	aStyleChangesOverRange=EFalse;
	CParaFormatLayer* style=NULL;
	TInt para=CONST_CAST(CRichTextIndex*,this)->OwningParagraph(aPos);
	TInt offset=(aLength==0)?0 :-1;
	TInt endPara=CONST_CAST(CRichTextIndex*,this)->OwningParagraph(aPos+(aLength+offset));
	style=(CParaFormatLayer*)(*iParaIx)[para].iParaAttribs->iParaFormat->SenseBase();
	++para;
	for (;para<=endPara;para++)
		{
		CParaFormatLayer* nextStyle=(CParaFormatLayer*)(*iParaIx)[para].iParaAttribs->iParaFormat->SenseBase();
		if (nextStyle!=style)
			aStyleChangesOverRange=ETrue;
		}

	__TEST_INVARIANT;
	return style;
	}


void CRichTextIndex::SplitPhraseL(TInt aPhrase,TInt anOffset,RPhraseAttribsEntry& aPhraseAttribs,CParaAttribs& aParaAttribs)
	{
	if (anOffset<=0 || anOffset>=aPhraseAttribs.Length())
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_SPLITPHRASEL, "Invariant" );
	    }
	__ASSERT_DEBUG(anOffset>0 && anOffset<aPhraseAttribs.Length(),User::Invariant());
//
	CCharFormatLayer* charLayer=CCharFormatLayer::NewCopyBaseL(aPhraseAttribs.CharFormat());
	CleanupStack::PushL(charLayer);
	iPhraseIx->InsertL(aPhrase+1,RPhraseAttribsEntry(charLayer,aPhraseAttribs.Length()-anOffset));
	CleanupStack::Pop();
	//
	// InsertL() has invalidated the phrase index.
	iPhraseIx->At(aPhrase).SetLength(anOffset);  // Adjust the length of the orginal phrase
	aParaAttribs.iPhraseCount++;
	}


TBool CRichTextIndex::MergePhrases(TInt aPhrase,RPhraseAttribsEntry& aPhraseAttribs,CParaAttribs& aParaAttribs)
	{
	RPhraseAttribsEntry& prevPhrase=iPhraseIx->At(aPhrase-1);
	if (!aPhraseAttribs.IsIdentical(prevPhrase))
		return EFalse;
	// Merge the abutting phrases together.
	prevPhrase.AdjustLength(aPhraseAttribs.Length());  //  Extend the remaining phrase
	RemoveFromPhraseIx(aPhrase);		// Free the resources taken by the redundant phrase
	aParaAttribs.iPhraseCount--;
	return ETrue;
	}


void CRichTextIndex::Share(TParaAttribsEntry& aParaEntry,TInt aPhrase)
//
// aParaEntry is not shared and can be (phrase count 1), aPhrase is the single phrase element
//
	{
	CParaAttribs* paraAttribs=aParaEntry.iParaAttribs;
	if (paraAttribs->iPhraseCount!=1)
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_SHARE, "Invariant" );
	    }
	__ASSERT_DEBUG(paraAttribs->iPhraseCount==1,User::Invariant());

	RPhraseAttribsEntry& phraseAttribs=iPhraseIx->At(aPhrase);
	if (phraseAttribs.IsPicturePhrase())
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CRICHTEXTINDEX_SHARE, "Invariant" );
	    }
	__ASSERT_DEBUG(!phraseAttribs.IsPicturePhrase(),User::Invariant());

	CParaAttribs* share=GetParaAttribs(paraAttribs,*phraseAttribs.CharFormat());
	if (share!=paraAttribs)
		{	// re-use an existing share, so release the current attribs
		paraAttribs->Release();
		phraseAttribs.Discard();
		aParaEntry.iParaAttribs=share;
		}
	iPhraseIx->Delete(aPhrase);
	}


void CRichTextIndex::ApplyCharFormatCleanup(TAny* aPtr)
// CLeanup function for ApplyCharFormatL()
//
	{REINTERPRET_CAST(CRichTextIndex*,aPtr)->ApplyCharFormatRollback();}


void CRichTextIndex::ApplyCharFormatRollback()
// Paragraph and phrase we were working on are stored in iPos
// Return them the canonical form
//
	{
	TParaAttribsEntry& paraEntry=iParaIx->At(iPos.iParaElement);
	CParaAttribs* paraAttribs=paraEntry.iParaAttribs;

	if (paraAttribs->IsShared())
		return;

	TInt phrase=iPos.iPhraseElement;
	TInt base=iPos.iParaBasePhraseElement;
	if (phrase<base || phrase>=base+paraAttribs->iPhraseCount)
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_APPLYCHARFORMATROLLBACK, "Invariant" );
	    }
	__ASSERT_DEBUG(phrase>=base && phrase<base+paraAttribs->iPhraseCount,User::Invariant());
	if (phrase<base+paraAttribs->iPhraseCount-1)	// merge to the right
		MergePhrases(phrase+1,iPhraseIx->At(phrase+1),*paraAttribs);
	if (phrase>base)								// merge to the left
		MergePhrases(phrase,iPhraseIx->At(phrase),*paraAttribs);
	if (paraAttribs->iPhraseCount==1)				// Share the paragraph
		Share(paraEntry,base);
	}


void CRichTextIndex::ApplyCharFormatL(const TCharFormatX& aFormat,const TCharFormatXMask& aMask,TInt aPos,TInt aLength,TBool aRemoveSpecific)
// Applies the specified character formatting to the characters contained within the range
// aPos to aPos+(aLength-1).
//
	{
	if (aLength<0)
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_APPLYCHARFORMATL, "Invariant" );
	    }
	__ASSERT_DEBUG(aLength>=0,User::Invariant());
	__TEST_INVARIANT;

	ScanToPosition(aPos,EScanToPositionAbsolute);
	TInt paraOffset=iPos.iParaElementOffset;
	TInt phraseOffset=iPos.iPhraseElementOffset;
	TInt phrase=iPos.iPhraseElement;

// prepare for failure
	CleanupStack::PushL(TCleanupItem(ApplyCharFormatCleanup,this));

	for (;;)
		{	// a paragraph at a time
		TParaAttribsEntry& paraEntry=iParaIx->At(iPos.iParaElement);
		CParaAttribs* paraAttribs=paraEntry.iParaAttribs;
		TInt charsToFormat=Min(aLength,paraEntry.iLength-paraOffset);
		aLength-=charsToFormat;
#ifdef _DEBUG
		aPos+=charsToFormat;
#endif

// STEP 1. Reclaim any shared paragraph into non shared form. Re-use the object if possible

		if (paraAttribs->IsShared())
			{
			CCharFormatLayer* charLayer=paraAttribs->iCharFormat;
			if (paraAttribs->iRefCount==CParaAttribs::EPrimeSharedCount)
				{	// we are the sole user of this attribute
				iPhraseIx->InsertL(phrase,RPhraseAttribsEntry(charLayer,paraEntry.iLength));
				// adjust attribute to be non-shared
				paraAttribs->link.Deque();
				paraAttribs->iRefCount=CParaAttribs::EPrimeNonSharedCount;
				paraAttribs->iPhraseCount=1;
				}
			else
				{	// create a new para attribs object
				CParaAttribs* newAttribs=CParaAttribs::NewL(paraAttribs->iParaFormat);
				CleanupReleasePushL(*newAttribs);
				charLayer=CCharFormatLayer::NewCopyBaseL(charLayer);
				CleanupStack::PushL(charLayer);
				iPhraseIx->InsertL(phrase,RPhraseAttribsEntry(charLayer,paraEntry.iLength));
				CleanupStack::Pop(2);		// charlayer, newAttribs
				paraAttribs->Release();		// lose a share on the old attribs
				paraEntry.iParaAttribs=paraAttribs=newAttribs;
				}
			phraseOffset=paraOffset; // we are now in the current position
			}

// STEP 2.	Walk through all affected phrases in this paragraph
//			For each one, we may need to split it, and then apply the new format

		do
			{
			if (phrase>=iPos.iParaBasePhraseElement+paraAttribs->iPhraseCount)
			    {
			    OstTrace0( TRACE_DUMP, DUP1_CRICHTEXTINDEX_APPLYCHARFORMATL, "Invariant" );
			    }
			__ASSERT_DEBUG(phrase<iPos.iParaBasePhraseElement+paraAttribs->iPhraseCount,User::Invariant());
//
			RPhraseAttribsEntry* phraseAttribs=&iPhraseIx->At(phrase);
			TInt len=phraseAttribs->Length();

// STEP 2.1	Split the phrase at the beginning of the range?

			if (phraseOffset>0)
				{		// can only happen for the first phrase
				/*
				 * The pointer paraAttribs is also stored in
				 * 'paraEntry.iParaAttribs'. The memory pointed to by this
				 * pointer will be released in CRichTextIndex's destructor.
				 */
				// coverity[leave_without_push]
				SplitPhraseL(phrase,phraseOffset,*phraseAttribs,*paraAttribs);	// inserts new phrase at correct position
				len-=phraseOffset;
				phraseOffset=0;
				iPos.iPhraseElement=++phrase;
				phraseAttribs=&iPhraseIx->At(phrase);
				}

// STEP 2.2	Split the phrase at the end of the range?

			if (len>charsToFormat)
				{	// phrase is longer than required format, so split it
				/*
				 * The pointer paraAttribs is also stored in
				 * 'paraEntry.iParaAttribs'. The memory pointed to by this
				 * pointer will be released in CRichTextIndex's destructor.
				 */
				// coverity[leave_without_push]
				SplitPhraseL(phrase,charsToFormat,*phraseAttribs,*paraAttribs);
				len=charsToFormat;
				phraseAttribs=&iPhraseIx->At(phrase);		// SplitPhraseL modifies the index array, we must do this!
				}

			if (phraseAttribs->Length()!=len)
			    {
			    OstTrace0( TRACE_DUMP, DUP2_CRICHTEXTINDEX_APPLYCHARFORMATL, "Invariant" );
			    }
			__ASSERT_DEBUG(phraseAttribs->Length()==len,User::Invariant());

// STEP 2.3	Change the format of the current phrase layer

			TCharFormatX format=aFormat;
			TCharFormatXMask mask=aMask;
			CCharFormatLayer* charLayer=phraseAttribs->CharFormat();
			if (!aRemoveSpecific)
				charLayer->Sense(format,mask);  // preserve current specific character formatting
			charLayer->SetL(format,mask);

// STEP 2.4	Check for merging with previous phrase

			if (phrase==iPos.iParaBasePhraseElement || !MergePhrases(phrase,*phraseAttribs,*paraAttribs))
				// if we don't merge this phrase, move on to the next one
				iPos.iPhraseElement=++phrase;

			charsToFormat-=len;
			} while (charsToFormat);

		if (phrase!=iPos.iParaBasePhraseElement+paraAttribs->iPhraseCount && aLength!=0)
		    {
		    OstTrace0( TRACE_DUMP, DUP3_CRICHTEXTINDEX_APPLYCHARFORMATL, "Invariant" );
		    }
		__ASSERT_DEBUG(phrase==iPos.iParaBasePhraseElement+paraAttribs->iPhraseCount || aLength==0,User::Invariant());

// STEP 3	Reduce the paragraph attributes back to canonical form

// STEP 3.1	Check for merging at the end of the changes

		if (phrase>iPos.iParaBasePhraseElement && phrase<iPos.iParaBasePhraseElement+paraAttribs->iPhraseCount)
			MergePhrases(phrase,iPhraseIx->At(phrase),*paraAttribs);	// mustn't adjust phrase index to follow merge

// STEP 3.2	See if we can re-share the paragraph

		if (paraAttribs->iPhraseCount==1)
			{	// This para has constant character formatting - can be shared.
			iPos.iPhraseElement=--phrase;
			Share(paraEntry,phrase);
			}

// loop into next paragraph
		if (aLength==0)
			break;
		iPos.iParaElement++;
		paraOffset=0;
		iPos.iParaBasePhraseElement=phrase;
#ifdef _DEBUG
		ScanToPosition(aPos,EScanToPositionAbsolute);
		if (iPos.iDocPos!=aPos)
		    {
		    OstTrace0( TRACE_DUMP, DUP4_CRICHTEXTINDEX_APPLYCHARFORMATL, "Invariant" );
		    }
		__ASSERT_DEBUG(iPos.iDocPos==aPos,User::Invariant());
		if (iPos.iPhraseElement!=phrase)
		    {
		    OstTrace0( TRACE_DUMP, DUP5_CRICHTEXTINDEX_APPLYCHARFORMATL, "Invariant" );
		    }
		__ASSERT_DEBUG(iPos.iPhraseElement==phrase,User::Invariant());
		if (iPos.iParaElementOffset!=paraOffset)
		    {
		    OstTrace0( TRACE_DUMP, DUP6_CRICHTEXTINDEX_APPLYCHARFORMATL, "Invariant" );
		    }
		__ASSERT_DEBUG(iPos.iParaElementOffset==paraOffset,User::Invariant());
		if (iPos.iPhraseElementOffset!=phraseOffset)
		    {
		    OstTrace0( TRACE_DUMP, DUP7_CRICHTEXTINDEX_APPLYCHARFORMATL, "Invariant" );
		    }
		__ASSERT_DEBUG(iPos.iPhraseElementOffset==phraseOffset,User::Invariant());
		__TEST_INVARIANT;
#endif
		}

	CleanupStack::Pop();	// rollback item

	__TEST_INVARIANT;
	}


void CRichTextIndex::RemoveSpecificParaFormatL(TInt aPos,TInt aLength)
// Removes all specific paragraph formatting from the specified region.
// For each paragraph covered by the range, check if its para attribs is in the
// shared list, or not.
// If its not shared, then simply reset the para format layer.
// If it is in the shared list, then a new shared para attribs must be created,
// and the reference count of the original decremented.
//
	{
	__TEST_INVARIANT;

	TInt endPara=OwningParagraph(aPos+aLength);
	TInt currentPara=OwningParagraph(aPos);
	CParaAttribs* currentParaAttribs=NULL;
	while (currentPara<=endPara)
		{
		currentParaAttribs=(*iParaIx)[currentPara].iParaAttribs;
		if (!currentParaAttribs->IsShared())
			{// Reset specific paragraph format layer to be empty.
			currentParaAttribs->iParaFormat->Reset();  // remove specific formatting.
			}
		else
			{// Maintain the shared list reference
			CParaAttribs* resultantParaAttribs=CParaAttribs::NewL(currentParaAttribs);
			CleanupStack::PushL(TCleanupItem(ReleaseOnCleanup,resultantParaAttribs));
			resultantParaAttribs->iParaFormat->Reset();  // remove specific formatting
			CParaAttribs* shared=RequestShareL(resultantParaAttribs);  // Will return a non-NULL handle.
			__ASSERT_DEBUG(shared,Panic(EDebug));
			currentParaAttribs->Release();
			iSharedParaQueHead.AddLast(*resultantParaAttribs);  // Allows correct release of cell.
			CleanupStack::PopAndDestroy();
			(*iParaIx)[currentPara].iParaAttribs=shared;
			}
		currentPara++;
		}


	__TEST_INVARIANT;
	}


void CRichTextIndex::RemoveSpecificCharFormatL(TInt aPos,TInt aLength)
// Removes all specific character formatting from the specified region.
// For each paragraph covered by the range, check if its para attribs is in the
// shared list, or not.
// If its not shared, then simply reset the para format layer.
// If it is in the shared list, then a new shared para attribs must be created,
// and the reference count of the original decremented.
//
	{
	__TEST_INVARIANT;

	TCharFormatX format;  // dummy format
	TCharFormatXMask mask;
	mask.ClearAll();
	ApplyCharFormatL(format,mask,aPos,aLength,ETrue);

	__TEST_INVARIANT;
	}


TBool CRichTextIndex::MergePhrases(TInt aPos)
// Checks if the specified character position aPos is a phrase boundary.
// If so, then examines the two abutting phrases at aPos to see if they
// are identical;  in which case they are merged into one phrase that covers
// the sum of their lengths.
//
	{
	ScanToPosition(aPos,EScanToPositionAbsolute);
	return MergePhrases(iPos);
	}


TBool CRichTextIndex::MergePhrases(const TLogicalPosition& aPos)
// Checks if the specified character position record is a phrase boundary.
// If so, then examines the two abutting phrases at aPos to see if they
// are identical;  in which case they are merged into one phrase that covers
// the sum of their lengths.
//
	{
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	TBool phrasesMerged=EFalse;
	if (!FirstPhraseOfParagraph() && iPos.iPhraseElementOffset==0)
		{// Check if the 2 abutting phrases can be merged together.
		TInt rightPhraseElement=iPos.iPhraseElement;
		RPhraseAttribsEntry* rightPhrase=&(*iPhraseIx)[rightPhraseElement];
		RPhraseAttribsEntry* leftPhrase=&(*iPhraseIx)[rightPhraseElement-1];
		if (rightPhrase->IsIdentical(*leftPhrase))
			{// Merge the abutting phrases together.  Cannot merge picture/non-picture/z.l.p. phrase combinations.
			rightPhrase->AdjustLength(leftPhrase->Length());  //  Extend the right phrase length
			RemoveFromPhraseIx(rightPhraseElement-1);  // Free the resources taken by the left phrase - redundant
			(*iParaIx)[iPos.iParaElement].iParaAttribs->iPhraseCount--; // Update phrase count of owning CParaAttribs
			ScanToPosition(aPos.iDocPos,EScanToPositionAbsolute);  // Pick up new phrase index.
			phrasesMerged=ETrue;
			}
		}
	return phrasesMerged;
	}


/** Remove phrases from the containing object.  This includes
freeing referenced resources. (pictures etc.)
@param aPhraseIndex The first phrase to be deleted
@param aCount The number of phrases to be deleted
*/
void CRichTextIndex::RemoveFromPhraseIx(TInt aPhraseIndex,TInt aCount)
	{
 	// if the phrase being deleted is <= iLastPos  
 	// then iLastPos will become invalid so should be reset
 	if (aPhraseIndex <= iLastUsed.iPhraseElement )
 		iLastUsed.Clear();
 		
	for (TInt offset=aPhraseIndex;offset<(aPhraseIndex+aCount);offset++)
		{
		// discard phrases & book-keep the picture count
		RPhraseAttribsEntry& phrase=(*iPhraseIx)[offset];
		phrase.Discard();
		if (phrase.IsPicturePhrase())
			iPictureCount--;
		}
	iPhraseIx->Delete(aPhraseIndex,aCount);
	}

void CRichTextIndex::GetParagraphFormatL(CParaFormat* aFormat,TInt aPos)const
// Fills aFormat with the effective Paragraph format attributes for the paragraph
// in which character position aPos is contained.
//
	{
	__TEST_INVARIANT;

	TLogicalPosition cachePos(iLastUsed);
	TInt para=CONST_CAST(CRichTextIndex*,this)->OwningParagraph(aPos,&cachePos);
	(*iParaIx)[para].iParaAttribs->iParaFormat->SenseEffectiveL(aFormat);
	}

void CRichTextIndex::GetSpecificParagraphFormatL(CParaFormat* aFormat,
												 TParaFormatMask& aMask,
												 TInt aPos)const
	{
	__TEST_INVARIANT;

	TLogicalPosition cachePos(iLastUsed);
	TInt para=CONST_CAST(CRichTextIndex*,this)->OwningParagraph(aPos,&cachePos);
	CParaFormatLayer* pLayer = (*iParaIx)[para].iParaAttribs->iParaFormat;
	pLayer->SenseL(aFormat, aMask);
	}

TInt CRichTextIndex::GetChars(TCharFormatX& aFormat,TInt aPos) const
// Returns the number of characters, commencing at aStartPos, that occupy the same phrase, and
// modifies aFormat, to hold the effective format of that phrase.
//
	{
	__TEST_INVARIANT;

	CONST_CAST(CRichTextIndex*,this)->ScanToPosition(aPos,EScanToPositionAbsolute,&MUTABLE_CAST(TLogicalPosition&,iLastUsed));
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	TInt phraseLength;
	CCharFormatLayer* charFormatLayer;
	if (current.iPhrase)
		{// Specific character formatting held in phrase index.
		charFormatLayer=current.iPhrase->CharFormat();
		phraseLength=(current.iPhrase->Length())-(iPos.iPhraseElementOffset);
		}
	else
		{// Constant character formatting held in the para attribs
		charFormatLayer=current.iParaAttribs->iCharFormat;
		phraseLength=current.iParaEntry->iLength-iPos.iParaElementOffset;
		}
	charFormatLayer->SenseEffective(aFormat);
	return phraseLength;
	}


TInt CRichTextIndex::GetPictureSizeInTwips(TSize& aSize,TInt aPos)const
// Get the size of the specified picture into aSize.  The picture is specified by its
// character position.  Return KErrNotFound if there is no picture at the specified
// document position.
//
	{
	__TEST_INVARIANT;

	CONST_CAST(CRichTextIndex*,this)->ScanToPosition(aPos,EScanToPositionAbsolute,&MUTABLE_CAST(TLogicalPosition&,iLastUsed));
	TCurrentIndexRecords current;
	GetCurrentRecords(current);

	return (current.iPhrase)
		? current.iPhrase->GetPictureSizeInTwips(aSize)
		: KErrNotFound;
	}

TPictureHeader* CRichTextIndex::PictureHeaderPtr(TInt aPos)
	{
	__TEST_INVARIANT;

	CONST_CAST(CRichTextIndex*,this)->ScanToPosition(aPos,EScanToPositionAbsolute);
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	return current.iPhrase? current.iPhrase->PictureHeaderPtr() : 0;
	}

TPictureHeader CRichTextIndex::PictureHeader(TInt aPos) const
// Return the picture header describing the picture at character position aPos.
// If there is no picture at character position aPos, a default picture header is returned.
//
	{
	const TPictureHeader* p = const_cast<CRichTextIndex*>(this)->PictureHeaderPtr(aPos);
	return p? *p : TPictureHeader();
	}


CPicture* CRichTextIndex::PictureHandleL(TInt aPos,MLayDoc::TForcePictureLoad aForceLoad)const
// Returns the handle of the concrete picture at character position aPos, if one exists;
// otherwise returns NULL.
//
	{
	__TEST_INVARIANT;

	CONST_CAST(CRichTextIndex*,this)->ScanToPosition(aPos,EScanToPositionAbsolute,&MUTABLE_CAST(TLogicalPosition&,iLastUsed));
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	return (current.iPhrase)
		? (CPicture*)current.iPhrase->PictureHandleL(iText.PictureFactory(),iText.StoreResolver(),aPos,aForceLoad)
		: NULL;
	}


void CRichTextIndex::GetParaFormatL(CParaFormat* aFormat,TParaFormatMask& aVaries,TInt aPos,TInt aLength,CParaFormat::TParaFormatGetMode aMode)const
// Senses the paragraph format of para(s) covered by the region aPos to aPos+aLength-1.
// aFormat takes the values of all attributes, and the mask aMask indicates those values that change
// over the selected region, and are therefore *indeterminate*.
// Application: seeding paragraph formatting dialogs.
//
	{
	__TEST_INVARIANT;

	TInt para=CONST_CAST(CRichTextIndex*,this)->OwningParagraph(aPos);
	TInt offset=(aLength==0)?0 :-1;
	TInt endPara=CONST_CAST(CRichTextIndex*,this)->OwningParagraph(aPos+(aLength+offset));
	aVaries.ClearAll();
	(*iParaIx)[para].iParaAttribs->iParaFormat->SenseEffectiveL(aFormat,aMode);  // Sense 1st paras' format.
	++para;
	CParaFormat* format=CParaFormat::NewLC();
	for (;para<=endPara;para++)
		{
		(*iParaIx)[para].iParaAttribs->iParaFormat->SenseEffectiveL(format,aMode);
		if (format->iLanguage!=aFormat->iLanguage)
			aVaries.SetAttrib(EAttParaLanguage);
		if (format->iFillColor!=aFormat->iFillColor)
			aVaries.SetAttrib(EAttFillColor);
		if (format->iLeftMarginInTwips!=aFormat->iLeftMarginInTwips)
			aVaries.SetAttrib(EAttLeftMargin);
		if (format->iRightMarginInTwips!=aFormat->iRightMarginInTwips)
			aVaries.SetAttrib(EAttRightMargin);
		if (format->iIndentInTwips!=aFormat->iIndentInTwips)
			aVaries.SetAttrib(EAttIndent);
		if (format->iHorizontalAlignment!=aFormat->iHorizontalAlignment)
			aVaries.SetAttrib(EAttAlignment);
		if (format->iVerticalAlignment!=aFormat->iVerticalAlignment)
			aVaries.SetAttrib(EAttVerticalAlignment);
		if (format->iLineSpacingInTwips!=aFormat->iLineSpacingInTwips)
			aVaries.SetAttrib(EAttLineSpacing);
		if (format->iLineSpacingControl!=aFormat->iLineSpacingControl)
			aVaries.SetAttrib(EAttLineSpacingControl);
		if (format->iSpaceBeforeInTwips!=aFormat->iSpaceBeforeInTwips)
			aVaries.SetAttrib(EAttSpaceBefore);
		if (format->iSpaceAfterInTwips!=aFormat->iSpaceAfterInTwips)
			aVaries.SetAttrib(EAttSpaceAfter);
		if (format->iKeepTogether!=aFormat->iKeepTogether)
			aVaries.SetAttrib(EAttKeepTogether);
		if (format->iKeepWithNext!=aFormat->iKeepWithNext)
			aVaries.SetAttrib(EAttKeepWithNext);
		if (format->iWidowOrphan!=aFormat->iWidowOrphan)
			aVaries.SetAttrib(EAttWidowOrphan);
		if (format->iWrap!=aFormat->iWrap)
			aVaries.SetAttrib(EAttWrap);
		if (format->iBorderMarginInTwips!=aFormat->iBorderMarginInTwips)
			aVaries.SetAttrib(EAttBorderMargin);
		if (format->iDefaultTabWidthInTwips!=aFormat->iDefaultTabWidthInTwips)
			aVaries.SetAttrib(EAttDefaultTabWidth);
		if (aMode==CParaFormat::EAllAttributes)
			{
			// Borders
			for (TInt border=0;border<CParaFormat::EMaxParaBorder;border++)
				{// Check each para border individually.  Assumes border format attributes run consecutively.
				if (!format->IsBorderEqual((CParaFormat::TParaBorderSide)border,*aFormat))
					aVaries.SetAttrib((TTextFormatAttribute)(EAttTopBorder+border));
				}
			// Bullet
			if (!format->iBullet && !aFormat->iBullet)
				{ /* neither para has bullet, so no variation */ }
			else if (!format->iBullet || !aFormat->iBullet
				|| *format->iBullet!=*aFormat->iBullet)
				aVaries.SetAttrib(EAttBullet);
			// The Tab-List
			if (format->TabCount()!=aFormat->TabCount())
				aVaries.SetAttrib(EAttTabStop);  // TabLists are different.
			else
				{// The 2 tablists have the same number of tab stops - but are not necessarily the same.
				TBool matched=ETrue;
				TInt tabCount=format->TabCount();
				for (TInt tabItem=0;tabItem<tabCount;tabItem++)
					{// Compare the 2 tabs.
					TTabStop comp1,comp2;
					comp1=format->TabStop(tabItem);
					comp2=aFormat->TabStop(tabItem);
					if (comp1!=comp2)
						matched=EFalse;
					}
				if (!matched)
					aVaries.SetAttrib(EAttTabStop);
				}
			}
		}
	CleanupStack::PopAndDestroy();
	}


// Compare all attributes in two formats and where they differ set the appropriate flag in the aVaries mask.
void CRichTextIndex::CheckForUndetermined(const TCharFormatX& aFormatA,const TCharFormatX& aFormatB,
										  TCharFormatXMask& aVaries) const
	{
	const TCharFormat& a = aFormatA.iCharFormat;
	const TCharFormat& b = aFormatB.iCharFormat;
	if (a.iLanguage!=b.iLanguage)
		aVaries.SetAttrib(EAttCharLanguage);
	if (a.iFontPresentation.iTextColor!=b.iFontPresentation.iTextColor)
		aVaries.SetAttrib(EAttColor);
	if (a.iFontPresentation.iHighlightColor!=b.iFontPresentation.iHighlightColor)
		aVaries.SetAttrib(EAttFontHighlightColor);
	if (a.iFontPresentation.iHighlightStyle!=b.iFontPresentation.iHighlightStyle)
		aVaries.SetAttrib(EAttFontHighlightStyle);
	if (a.iFontPresentation.iStrikethrough!=b.iFontPresentation.iStrikethrough)
		aVaries.SetAttrib(EAttFontStrikethrough);
	if (a.iFontPresentation.iUnderline!=b.iFontPresentation.iUnderline)
		aVaries.SetAttrib(EAttFontUnderline);
	if (a.iFontPresentation.iHiddenText!=b.iFontPresentation.iHiddenText)
		aVaries.SetAttrib(EAttFontHiddenText);
	if (a.iFontPresentation.iPictureAlignment!=b.iFontPresentation.iPictureAlignment)
		aVaries.SetAttrib(EAttFontPictureAlignment);
	if (a.iFontSpec.iHeight!=b.iFontSpec.iHeight)
		aVaries.SetAttrib(EAttFontHeight);
	if (!(a.iFontSpec.iTypeface==b.iFontSpec.iTypeface))
		aVaries.SetAttrib(EAttFontTypeface);
	if (a.iFontSpec.iFontStyle.Posture()!=b.iFontSpec.iFontStyle.Posture())
		aVaries.SetAttrib(EAttFontPosture);
	if (a.iFontSpec.iFontStyle.StrokeWeight()!=b.iFontSpec.iFontStyle.StrokeWeight())
		aVaries.SetAttrib(EAttFontStrokeWeight);
	if (a.iFontSpec.iFontStyle.PrintPosition()!=b.iFontSpec.iFontStyle.PrintPosition())
		aVaries.SetAttrib(EAttFontPrintPos);
	if (aFormatA.iParserTag != aFormatB.iParserTag)
		aVaries.SetAttrib(EAttParserTag);
	}


void CRichTextIndex::GetCharFormat(TCharFormatX& aFormat,TCharFormatXMask& aVaries,TInt aPos,TInt aLength)const
// Senses the character formatting of the phrase(s) covered by the region aPos to aPos+aLength-1.
// aFormat takes the values of all character format attributes, and the mask aMask indicates those
// values that change over the selected region, and are therefore *indeterminate*.
// Application: seeding character formatting dialogs.
// If aLength is zero, the character format sensed is that of the charcter immediatley to the left (behind) the cursor.
//
	{
	__TEST_INVARIANT;

	aVaries.ClearAll();
	if (aLength==0)  // Get the format of the character to the left of the cursor.
		((CRichTextIndex*)this)->ScanToPosition(aPos,EScanToPositionMatchLeft);
	else
		((CRichTextIndex*)this)->ScanToPosition(aPos,EScanToPositionAbsolute);
	// Get char format of first phrase
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	if (current.iPhrase)
		current.iPhrase->CharFormat()->SenseEffective(aFormat);
	else
		current.iParaAttribs->iCharFormat->SenseEffective(aFormat);
	// Place pos at start of next phrase
	TInt pos=aPos+(CurrentPhraseLength()-CurrentPhraseOffset());
	while (pos<=aPos+aLength-1)
		{// Get the format of the next phrase and check if attributes change value
		((CRichTextIndex*)this)->ScanToPosition(pos,EScanToPositionAbsolute);
		GetCurrentRecords(current);
		TCharFormatX format;
		if (current.iPhrase)
			current.iPhrase->CharFormat()->SenseEffective(format);
		else
			current.iParaAttribs->iCharFormat->SenseEffective(format);
		CheckForUndetermined(format,aFormat,aVaries);
		pos+=CurrentPhraseLength();
		}
	}


void CRichTextIndex::GetSpecificCharFormatDirection(TCharFormatX& aFormat,
												   TCharFormatXMask& aMask,
												   TInt aPos,
												   TBool aGetLeft) const
	{
	__TEST_INVARIANT;

	aMask.ClearAll();
	((CRichTextIndex*)this)->ScanToPosition(aPos,
		aGetLeft? EScanToPositionMatchLeft : EScanToPositionAbsolute);
	TCurrentIndexRecords current;
	GetCurrentRecords(current);
	if (current.iPhrase)
		current.iPhrase->CharFormat()->Sense(aFormat,aMask);
	else
		current.iParaAttribs->iCharFormat->Sense(aFormat,aMask);
	}

void CRichTextIndex::GetSpecificCharFormat(TCharFormatX& aFormat,TCharFormatXMask& aMask,TInt aPos)const
// Return the format attributes store in the specific layer only, for the specified document position.
// THIS IS NOT THE EFFECTIVE FORMAT, BUT THE SPECIFIC FORMATTING ONLY.
//
	{
	GetSpecificCharFormatDirection(aFormat, aMask, aPos, ETrue);
	}


CParaAttribs* CRichTextIndex::RequestReclaimShareL(CParaAttribs* aParaAttribs,TParaAttribsEntry* aParaEntry)
// If the specified para attribs is currently on the shared list, then
// a specific para attribs is *reclaimed* and returned.
// The current share on the CParaAttribs is not *Released*.
// A new CParaAttribs is created of the same paragraph format as the shared one,
// but with a reference count of zero, (as this now has specific character formatting),
// and 1 phrase that is of the same character format as the shared one.
// The reclaimed specific para attribs is attactched to the specified para entry.
// NOTE:
// If the specified CParaAttribs is not currently on the shared list, NULL
// is returned, and aParaAttribs left unchanged..
// Assumes a previous call to ScanToPosition has correctly set the internal position record.
// This function can be called safely in any situation. (Except that it may *LEAVE*).
//
	{
	if (!(aParaAttribs->IsShared()))
		return NULL;  // This para attribs is not currently shared.
	// We are dealing with a shared paraAttribs from now on.
	CParaAttribs* reclaimedPara=CParaAttribs::NewL(aParaAttribs->iParaFormat);  // Create the re-claimed paraAttribs.
	CleanupStack::PushL(TCleanupItem(ReleaseOnCleanup,reclaimedPara));
	CCharFormatLayer* newFormat=CCharFormatLayer::NewCopyBaseL(aParaAttribs->iCharFormat);
	CleanupStack::PushL(newFormat);
	RPhraseAttribsEntry phrase(newFormat,aParaEntry->iLength);
	// Now insert this phrase into the index.
	iPhraseIx->InsertL(iPos.iPhraseElement,phrase);
	CleanupStack::Pop(2);  // newFormat, reclaimedPara
	return reclaimedPara;
	}


CParaAttribs* CRichTextIndex::RequestShareL(CParaAttribs* aParaAttribs,CCharFormatLayer* aCharFormat,CParaAttribs* aReservedCell)
// Attempts to re-use an existing paraAttribs that matches the one specified.
// Returns the handle of a CParaAttribs, or NULL if the specified argument cannot
// be shared.
// aCharFormat may be NULL, as may aReservedCell.
//
	{
	if (aParaAttribs->iRefCount<=0 && aParaAttribs->iPhraseCount>1)
		return NULL;  // This para has specific character formatting & multiple phrases and so cannot be shared.
	// This para has constant character formatting - can be shared.
	CCharFormatLayer* charFormat;
	if (aCharFormat)
		charFormat=aCharFormat;  // Has a phrase index.
	else
		charFormat=aParaAttribs->iCharFormat;  // Has constant char formatting.
	return GetParaAttribsL(aParaAttribs->iParaFormat,charFormat,aReservedCell);
	}


CParaAttribs* CRichTextIndex::RequestShare(const TLogicalPosition& aLogicalPosition)
// Returns a handle to a paraAttribs on the shared list that matches the paragraph at the
// specified position.  Returns NULL if the specified paragraph does not have constant
// character formatting.
//
	{
	CParaAttribs* paraAttribs=(*iParaIx)[aLogicalPosition.iParaElement].iParaAttribs;
	if (paraAttribs->iRefCount<=0 && paraAttribs->iPhraseCount>1)
		return NULL;  // This para has specific character formatting and so cannot be shared.
	// This para has constant character formatting - can be shared.
	return GetParaAttribs(aLogicalPosition);
	}


CParaAttribs* CRichTextIndex::GetParaAttribsL(const CParaFormatLayer* aParaFormat,const CCharFormatLayer* aCharFormat,
											  CParaAttribs* aReservedCell)
// Attempts to match the specified arguments to a CParaAttribs in the shared list.
// If matched, the handle of the matched para attribs is returned, and the reference count
// of that item is incremented.
//
// aReservedCell is a pre-allocated CParaAttribs that has been correctly setup.  If no match
// is found, and the reserved cell is specified, this is used in preference to creating a new one.
// If the reserved cell is not specified and there is no match, a new CParaAttribs of the correct
// specification is created and added to the shared list.
//
	{
	CParaAttribs* handle=FindSharedParaAttribs(*aParaFormat,*aCharFormat);
	if (handle)
		return handle;  // match found already in the shared list.
// There is no match, so create new sharedPara and add to list.
	if (!aReservedCell)
		aReservedCell=CParaAttribs::NewL(aParaFormat,aCharFormat);  // Reusing aReservedCell saves an automatic.
	iSharedParaQueHead.AddLast(*aReservedCell);
	return aReservedCell;
	}


CParaAttribs* CRichTextIndex::GetParaAttribs(CParaAttribs* aParaAttribs,CCharFormatLayer& aCharFormatLayer)
// Called by Reset.
//
	{
	CParaAttribs* handle=FindSharedParaAttribs(*aParaAttribs->iParaFormat,aCharFormatLayer);
	if (handle)
		return handle;
	else
		{// No match, so piece together new shared paraAttribs and add to shared para list.
		aParaAttribs->iRefCount=1;
		aParaAttribs->iCharFormat=&aCharFormatLayer;
		iSharedParaQueHead.AddLast(*aParaAttribs);
		return aParaAttribs;
		}
	}


CParaAttribs* CRichTextIndex::GetParaAttribs(const TLogicalPosition& aLogicalPosition)
// Attempts to match the specified arguments to a CParaAttribs in the shared list.
// If matched, the handle of the matched para attribs is returned, and the reference count
// of that item is incremented. If no match is found, the current para attribs is
// transformed into one that is placed in the shared list.
//
	{
	CParaAttribs* sourceParaAttribs=(*iParaIx)[aLogicalPosition.iParaElement].iParaAttribs;
	RPhraseAttribsEntry* sourcePhrase=&(*iPhraseIx)[aLogicalPosition.iPhraseElement];
	//
	CParaAttribs* handle=FindSharedParaAttribs(*sourceParaAttribs->iParaFormat,*sourcePhrase->CharFormat());
	if (handle)
		return handle;
	else
		{// No match, so piece together new shared paraAttribs and add to shared para list
		sourceParaAttribs->iRefCount=1;
		if (sourcePhrase->IsPicturePhrase())
		    {
		    OstTrace0( TRACE_FATAL, CRICHTEXTINDEX_GETPARAATTRIBS, "EReleasCharFormatLayerOwnershipCalledOnPicturePhrase" );
		    }
		__ASSERT_ALWAYS(!sourcePhrase->IsPicturePhrase(),Panic(EReleasCharFormatLayerOwnershipCalledOnPicturePhrase));
		sourceParaAttribs->iCharFormat=sourcePhrase->ReleaseCharFormatLayerOwnership();
		sourcePhrase->Discard();
		iPhraseIx->Delete(aLogicalPosition.iPhraseElement);  // remove the deleted phrase from the phrase index.
		iSharedParaQueHead.AddLast(*sourceParaAttribs);
		return sourceParaAttribs;
		}
	}


CParaAttribs* CRichTextIndex::FindSharedParaAttribs(const CParaFormatLayer& aParaFormatLayer,const CCharFormatLayer& aCharFormatLayer)
// Attempts to match the specified arguments to an item in the shared para list.
// If found, the handle of the matched para attribs is returned, and the reference count
// of that item is incremented.
// If no match is made NULL is returned.
//
	{
	CParaAttribs* currentSharedPara=NULL;
	TBool matched=EFalse;
	if (!iSharedParaQueHead.IsEmpty())
		{
		TDblQueIter<CParaAttribs> iterator(iSharedParaQueHead);
		while ((currentSharedPara=iterator++)!=NULL)
			{// Try and match each item in the shared para list.
			matched=aParaFormatLayer.IsIdentical(currentSharedPara->iParaFormat);
			if (!matched)
				continue;
			matched=aCharFormatLayer.IsIdentical(currentSharedPara->iCharFormat);
			if (!matched)
				continue;
			// We have a match, so adjust reference count.
			currentSharedPara->iRefCount++;
			return currentSharedPara;
			}
		}
	return currentSharedPara;
	}


void CRichTextIndex::ScanToPosition(TInt aCharPos,TScanToPositionMode aMode,TLogicalPosition* aLastUsed/*=NULL*/)
// Move the internal position record to that indicated by the specified character position, aCharPos.
// Behaviour follows:
// aCharPos is considered to be goverened by the phrase covering aCharPos-1,
// except when aCharPos is the fist character of a paragraph, when
// aCharPos is goverened by the phrase coverng aCharPos.
// (If nothing else, aCharPos will be a paragraph delimiter or the end-of-document
// character.
// The implementation below matches to the right as standard, then checks if a left
// phrase match is available.
//
	{
	if (!aLastUsed || aLastUsed->iDocPos-aLastUsed->iParaElementOffset>aCharPos)
		iPos.Clear();  // Reset the internal position record.
	else
		{
		iPos=*aLastUsed;
		if (iPos.iDocPos>aCharPos || (aMode==EScanToPositionMatchLeft && iPos.iDocPos==aCharPos))
			{// reset to the start of paragraph if aPos < cache pos or aPos==chache Pos whilst matching left
			iPos.iDocPos-=iPos.iParaElementOffset;
			iPos.iParaElementOffset=iPos.iPhraseElementOffset=0;
			iPos.iPhraseElement=iPos.iParaBasePhraseElement;
			}
		}

	TInt phraseElement=iPos.iPhraseElement;
	TInt startOfPara=iPos.iDocPos-iPos.iParaElementOffset;
	TInt paraElement=iPos.iParaElement;
	const CArrayFix<TParaAttribsEntry>& paraIx=*iParaIx;
	const TParaAttribsEntry* para=&paraIx[paraElement];
	TInt len=para->iLength;
	if (aCharPos>=(startOfPara+len))
		{
		iPos.iParaElementOffset=iPos.iPhraseElementOffset=0;
		phraseElement=iPos.iParaBasePhraseElement;
		const TParaAttribsEntry* end=paraIx.End(paraElement);
		do
			{// Find the paragraph...
			startOfPara+=len;
			if (!(para->iParaAttribs->IsShared()))  // Adjust position within phrase index
				phraseElement+=para->iParaAttribs->iPhraseCount;
			++paraElement;
			if (++para==end)
				{
				para=&paraIx[paraElement];
				end=paraIx.End(paraElement);
				}
			len=para->iLength;
			}
				while (aCharPos>=(startOfPara+len) && (paraElement+1) < paraIx.Count());
		iPos.iParaBasePhraseElement=phraseElement;
		iPos.iParaElement=paraElement;
		}
	TInt startOfPhrase=iPos.iParaElementOffset-iPos.iPhraseElementOffset;
// the offset within the paragraph.
	TInt paraElementOffset=aCharPos-startOfPara;
	iPos.iParaElementOffset=paraElementOffset;

	if (!(para->iParaAttribs->IsShared()))
		{// Find phrase & offset within it.
		TInt lastPhraseLength=-1;  // Record phrase length in case left match required.
		const CArrayFix<RPhraseAttribsEntry>& phraseIx=*iPhraseIx;
		const RPhraseAttribsEntry* phrase=&phraseIx[phraseElement];
		const RPhraseAttribsEntry* end=NULL;
		for (;;)
			{	// Find the phrase in the paragraph...
			len=phrase->Length();
			if (paraElementOffset<(startOfPhrase+len))
				break;
			startOfPhrase+=len;
			lastPhraseLength=len;
			if (end==NULL)
				end=phraseIx.End(phraseElement);
			phraseElement++;
			if (++phrase<end)
				continue;
			phrase=&phraseIx[phraseElement];
			end=phraseIx.End(phraseElement);
			}//...and the offset within this.
		// Check now for match left.
		if ((aMode==EScanToPositionMatchLeft) && lastPhraseLength>=0 && paraElementOffset==startOfPhrase)
			{// Match to the left most phrase if at the start of a phrase.
			phraseElement--;
			iPos.iPhraseElementOffset=lastPhraseLength;
			}
		else
			iPos.iPhraseElementOffset=paraElementOffset-startOfPhrase;
		}
	else
		{
		if (iPos.iParaBasePhraseElement!=phraseElement)
		    {
		    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_SCANTOPOSITION, "EDebug" );
		    }
		__ASSERT_DEBUG(iPos.iParaBasePhraseElement==phraseElement,Panic(EDebug));
		}
	iPos.iPhraseElement=phraseElement;
	iPos.iDocPos=aCharPos;
	if (aLastUsed)
		*aLastUsed=iPos;
	}


TBool CRichTextIndex::FirstPhraseOfParagraph()const
// Interogates the current internal position record.
// Return ETrue if the current phrase element is the first phrase
// of specific character format in the current paragraph;
// Otherwise return EFalse.
//
	{return iPos.iPhraseElement==iPos.iParaBasePhraseElement;}


TInt CRichTextIndex::CurrentPhraseLength()const
// Return the length of the current phrase, where the current
// phrase is specified by the state of the internal position record.
//
	{
	if ((*iParaIx)[iPos.iParaElement].iParaAttribs->IsShared())
		return (*iParaIx)[iPos.iParaElement].iLength;
	else
		return (*iPhraseIx)[iPos.iPhraseElement].Length();
	}


TInt CRichTextIndex::CurrentPhraseOffset()const
// Returns the offset within the current phrase, where the current
// phrase is specified by the state of the internal position record.
//
	{
	if ((*iParaIx)[iPos.iParaElement].iParaAttribs->IsShared())
		return iPos.iParaElementOffset;  // only 1 phrase in para.
	else
		return iPos.iPhraseElementOffset;
	}


void CRichTextIndex::GetCurrentRecords(TCurrentIndexRecords& aRecord)const
// Package the phrase and paragraph index records that apply to the
// current paragraph and return this package.  It is assumed that
// the caller has already set the internal position record to a valid state.
//
	{
	aRecord.iParaEntry=&(*iParaIx)[iPos.iParaElement];
	aRecord.iParaAttribs=aRecord.iParaEntry->iParaAttribs;
	if (aRecord.iParaAttribs->IsShared())
		aRecord.iPhrase=NULL;
	else
		aRecord.iPhrase=&((*iPhraseIx)[iPos.iPhraseElement]);
	}


void CRichTextIndex::GetPhraseFormat(TCurrentIndexRecords& aCurrent,TCharFormatX& aFormat,TCharFormatXMask& aMask,
									 CCharFormatLayer*& aCharBase)const
// Fills aFormat and aMask with the character formatting information of the current record.
// aCharBase is set to the basedOn link if present.
// Encapsulates the concepts of the specific phrase index, and the constant character format.
// Only senses the format in the layer, does *NOT* perform a SenseEffective.
//
	{
	CCharFormatLayer* charFormatLayer=NULL;
	if (aCurrent.iPhrase)
		{// Specific character formatting held by phrase index.
		charFormatLayer=aCurrent.iPhrase->CharFormat();
		}
	else
		{// Constant character formatting held in the para attribs
		charFormatLayer=aCurrent.iParaAttribs->iCharFormat;
		}
	charFormatLayer->Sense(aFormat,aMask);
	aCharBase=(CCharFormatLayer*)(charFormatLayer->SenseBase());
	}


TInt CRichTextIndex::OwningParagraph(TInt aPos,TLogicalPosition* aLastUsed/*=NULL*/)const
// Return the paragraph element number that contains character position aPos.
// Assumes the caller has validated aPos.  Alters the internal record position.
//
	{
	((CRichTextIndex*)this)->ScanToPosition(aPos,EScanToPositionMatchLeft,aLastUsed);
	return iPos.iParaElement;
	}


void CRichTextIndex::SplitPhraseL(TInt aSplitPos)
// Splits the phrase at the offset aSplitPos, creating a new phrase
// which is filled with the split part of the current phrase, includig aSplitPos.
// The character format applied to the new phrase is the format of the phrase from which it has been split.
// The resulting new phrase is inserted into the phrase index immediately following the
// current element.  If aSplitPos is already at a phrase boundary, then no split is performed.
// (This means that a picture phrase in effect can never be split).
//
	{
	SetPhraseSplit(EFalse);
	ScanToPosition(aSplitPos,EScanToPositionAbsolute);
	if (iPos.iPhraseElementOffset==0)
		return;  // aSplitPos on a phrase boundary; urgo no split.
	TCurrentIndexRecords current; GetCurrentRecords(current);
// ASSERT: This function set can only be called on CParaAttribs that specific char format.
	if (current.iPhrase==NULL)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CRICHTEXTINDEX_SPLITPHRASEL, "ESplitPhraseCalledOnSharedPara" );
	    }
	__ASSERT_ALWAYS(current.iPhrase!=NULL,Panic(ESplitPhraseCalledOnSharedPara));
	DoSplitPhraseL(*current.iPhrase,iPos.iPhraseElementOffset,current.iParaAttribs);
	}


void CRichTextIndex::SplitPhraseL(TInt aPhraseElement,TInt aPhraseOffset,CParaAttribs* aParaAttribs)
// Splits the specified phrase at the offset aOffsetInPhrase, creating a new phrase
// which is filled with the split part of the current phrase, includig the split pos.
// The character format applied to the new phrase is the format of the phrase from which it has been split.
// The resulting new phrase is inserted into the phrase index immediately following the
// current element.  If the split pos is already at a phrase boundary, then no split is performed.
// (This means that a picture phrase in effect can never be split).
//
	{
	SetPhraseSplit(EFalse);
	RPhraseAttribsEntry& phrase=(*iPhraseIx)[aPhraseElement];
	if ((aPhraseOffset>0) && (aPhraseOffset<phrase.Length()))
		{// Not at a phrase boundary so split the current phrase.
		DoSplitPhraseL(phrase,aPhraseOffset,aParaAttribs);
		}
	}


void CRichTextIndex::DoSplitPhraseL(RPhraseAttribsEntry& aCurrentPhrase,TInt aPhraseOffset,CParaAttribs* aParaAttribs)
// Splits the specified phrase, creating a new phrase of the same character format.
// This new phrase is inserted into the phrase index immediately following the current one.
//
	{
// ASSERT: Cannot split a picture phrase.
	if (aCurrentPhrase.IsPicturePhrase())
	    {
	    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_DOSPLITPHRASEL, "ESplitPhraseCalledOnPicturePhrase" );
	    }
	__ASSERT_DEBUG(!aCurrentPhrase.IsPicturePhrase(),Panic(ESplitPhraseCalledOnPicturePhrase));
	CCharFormatLayer* layer=CCharFormatLayer::NewCopyBaseL(aCurrentPhrase.CharFormat());
	CleanupStack::PushL(layer);
	RPhraseAttribsEntry newPhrase(layer,aCurrentPhrase.Length()-aPhraseOffset);
	iPhraseIx->InsertL(iPos.iPhraseElement+1,newPhrase);
	CleanupStack::Pop();
	//
	// InsertL() has invalidated the current internal position record.
	iPhraseIx->At(iPos.iPhraseElement).SetLength(aPhraseOffset);
	SetPhraseSplit(ETrue);
	aParaAttribs->iPhraseCount++;
	}


TBool CRichTextIndex::HasMarkupData(const CFormatLayer* aGlobalParaFormatLayer)const
// Returns ETure if this rich text instance has any specific markup,
// otherwise returns EFalse.
// The presence of specific markup is indicated by the following...
// 1) Style list is present (if style table is owned by the rich text)
// 2) Any phrase index content
// 3) >1 shared para attribs
//   or
// 3) 1 shared para attribs that has specific markup
// 4) any paragraph is based on a style other than normal (the global paraformatlayer)
//
	{
	TInt phraseCount=PhraseCount();
	if (phraseCount>0)
		return ETrue;
	//
	TInt sharedParaCount=SharedParaCount(this);
	if (sharedParaCount<1 && (sharedParaCount!=0 || phraseCount<=0))
	    {
	    OstTrace0( TRACE_FATAL, CRICHTEXTINDEX_HASMARKUPDATA, "ERichTextIndexIntegrityErr" );
	    }
	__ASSERT_ALWAYS(sharedParaCount>=1 || (sharedParaCount==0 && phraseCount>0),Panic(ERichTextIndexIntegrityErr));
	if (sharedParaCount>1)
		return ETrue;
	const CParaAttribs* paraAttribs=iSharedParaQueHead.First();
	if (!paraAttribs->iParaFormat->IsEmpty())
		return ETrue;
	if (!paraAttribs->iCharFormat->IsEmpty())
		return ETrue;
	if (paraAttribs->iParaFormat->SenseBase()!=aGlobalParaFormatLayer)
		return ETrue;
	return EFalse;
	}


TInt CRichTextIndex::SharedParaCount(const CRichTextIndex* aSource)const
// Return a count of the number of shared paragraph formats present
// in the specified object.
//
	{
	TInt sharedParaCount = 0;
	TDblQueIter<CParaAttribs> iterator( MUTABLE_CAST(TDblQue<CParaAttribs>&, aSource->iSharedParaQueHead) );
	while ( iterator++ != NULL )
		sharedParaCount++;
	return sharedParaCount;
	}


void CRichTextIndex::AppendTakingSolePictureOwnershipL(const CRichTextIndex* aSource,const TGlobalLayerInfoAppend& aGlobalLayerInfo)
// No paragraph style information is appended.
//
	{
	CancelInsertCharFormat();
	CONST_CAST(CRichTextIndex*,aSource)->CancelInsertCharFormat();

	TInt origParaCount=ParagraphCount();
	TInt origPhraseCount=iPhraseIx->Count();
	TRAPD(ret,
		AppendParaIndexL(aSource,aGlobalLayerInfo);
		AppendPhraseIndexL(aSource,aGlobalLayerInfo);
		);
	if (ret!=KErrNone)
		{
		RemoveFromPhraseIx(origPhraseCount,iPhraseIx->Count()-origPhraseCount);	// remove any added phrases etc.
		RbRemoveInsertedParaAttribsEntries(origParaCount,ParagraphCount()-origParaCount);	// remove any added paragraphs etc.
		NormalizeSharedList();		// remove any added shared paragraph attributes
		User::Leave(ret);
		}

	__TEST_INVARIANT;
	}


void CRichTextIndex::AppendParaIndexL(const CRichTextIndex* aSource,const TGlobalLayerInfoAppend& aGlobalLayerInfo)
//
	{
	CRichTextStoreMap<CParaAttribs>* map=CRichTextStoreMap<CParaAttribs>::NewLC(SharedParaCount(aSource));

	AppendSharedFormatsL(*map,aSource,aGlobalLayerInfo);

	// Extend para index by required amount
	TInt originalParaCount=iParaIx->Count();
	TInt requiredParaCount=aSource->iParaIx->Count();
	iParaIx->AppendL(TParaAttribsEntry(),requiredParaCount);

	for (TInt ii=0;ii<requiredParaCount;ii++)
		{// Copy the paragraph data for each of the appended paragraphs.
		const TParaAttribsEntry& sParaEntry=(*aSource->iParaIx)[ii];
		const CParaAttribs* sParaAttribs=sParaEntry.iParaAttribs;
		CParaAttribs* tParaAttribs;
		if (sParaAttribs->IsShared())
			{
			tParaAttribs=map->Item(sParaAttribs);
			if (tParaAttribs==NULL)
			    {
			    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_APPENDPARAINDEXL, "ESharedFormatsMapIntegrityError" );
			    }
			__ASSERT_DEBUG(tParaAttribs!=NULL,Panic(ESharedFormatsMapIntegrityError));
			tParaAttribs->iRefCount++;
			}
		else
			{// Have to build up the specific para attribs
			tParaAttribs=CParaAttribs::NewL(sParaAttribs->iParaFormat);  // sets iRefCount=0, copies the format layer
			tParaAttribs->iParaFormat->SetBase(aGlobalLayerInfo.iAggParaFormatLayer);
			tParaAttribs->iPhraseCount=sParaAttribs->iPhraseCount;
			}
		TParaAttribsEntry& tParaEntry=(*iParaIx)[originalParaCount+ii];
		tParaEntry.iLength=sParaEntry.iLength;
		tParaEntry.iParaAttribs=tParaAttribs;
		
		// tParaAttribs is attached to CRichTextIndex::iParaIx, and will be
		// released in destructor CRichTextIndex::~CRichTextIndex().
		// To prevent Coverity from reporting defect, add a comment:
		// coverity[memory_leak]
		}

	CleanupStack::PopAndDestroy();  // map
	}


void CRichTextIndex::AppendSharedFormatsL(CParaAttribsMap& aMap,const CRichTextIndex* aSource,
											const TGlobalLayerInfoAppend& aGlobalLayerInfo)
// A map is kept, that for each original format specifies the corresponding new one that appended
// paragraphs should use.
//
	{
	TDblQueIter<CParaAttribs> iterator(MUTABLE_CAST(TDblQue<CParaAttribs>&,aSource->iSharedParaQueHead));
	CParaAttribs* currentSharedPara;
	while ((currentSharedPara=iterator++)!=NULL)
		{
		if (!currentSharedPara->IsShared())
		    {
		    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_APPENDSHAREDFORMATSL, "Invariant" );
		    }
		__ASSERT_DEBUG(currentSharedPara->IsShared(),User::Invariant());

		CParaFormatLayer* sPl=currentSharedPara->iParaFormat;
		CCharFormatLayer* sCl=currentSharedPara->iCharFormat;
		sPl->SetBase(aGlobalLayerInfo.iAggParaFormatLayer);  // alter the original so that the following GetParaAttribsL() call will
		sCl->SetBase(aGlobalLayerInfo.iAggCharFormatLayer);  // match based on our global format layers, not those of aSource's.
		CParaAttribs* newParaAttribs=FindSharedParaAttribs(*sPl,*sCl);
		sPl->SetBase(aGlobalLayerInfo.iComParaFormatLayer);  // set the global format layers back again, cos we don't want to
		sCl->SetBase(aGlobalLayerInfo.iComCharFormatLayer);  // corrupt aSource.
		if (newParaAttribs==NULL)
			{
			newParaAttribs=CParaAttribs::NewL(currentSharedPara);
			// change the based-on links (they are copied in the construction)
			newParaAttribs->iParaFormat->SetBase(aGlobalLayerInfo.iAggParaFormatLayer);
			newParaAttribs->iCharFormat->SetBase(aGlobalLayerInfo.iAggCharFormatLayer);
			iSharedParaQueHead.AddLast(*newParaAttribs);
			}
		newParaAttribs->iRefCount--;  // we have not yet linked incoming para - taken out no shares just yet
		aMap.Bind(currentSharedPara,newParaAttribs);
		}
	}

void CRichTextIndex::AppendPhraseIndexL(const CRichTextIndex* aSource,const TGlobalLayerInfoAppend& aGlobalLayerInfo)
//
	{
	TInt originalPhraseCount=iPhraseIx->Count();
	TInt requiredPhraseCount=aSource->iPhraseIx->Count();

	// Extend phrase index by required amount
	iPhraseIx->AppendL(RPhraseAttribsEntry(),requiredPhraseCount);

	CArrayFixFlat<TInt>* pictureMap=new(ELeave) CArrayFixFlat<TInt>(16);
	CleanupStack::PushL(pictureMap);

	for (TInt jj=0;jj<requiredPhraseCount;jj++)
		{
		RPhraseAttribsEntry& sPhrase=(*aSource->iPhraseIx)[jj];
		CCharFormatLayer* sCharFormatLayer=sPhrase.CharFormat();
		CCharFormatLayer* charLayer=CCharFormatLayer::NewL(sCharFormatLayer);
		charLayer->SetBase(aGlobalLayerInfo.iAggCharFormatLayer);
		RPhraseAttribsEntry& tPhrase=(*iPhraseIx)[jj+originalPhraseCount];
		if (!sPhrase.IsPicturePhrase())
			tPhrase=RPhraseAttribsEntry(charLayer,sPhrase.Length());
		else
			{
			TPictureHeader hdr;
			hdr=sPhrase.PictureHeader();  // copy the header from the source
			if (hdr.iPicture.IsPtr())
				hdr.iPicture=NULL;		  // pic.ownership transferred later.
			CleanupStack::PushL(charLayer);
			TBool ownershipTaken(EFalse);
			CPicturePhrase* picPhrase=CPicturePhrase::NewL(hdr,charLayer,ownershipTaken);
			CleanupStack::Pop();  // charLayer
			tPhrase=RPhraseAttribsEntry(picPhrase);
			iPictureCount++;
			pictureMap->AppendL(jj);
			}
		}

	// transfer pictures now
	for (TInt kk=pictureMap->Count();--kk>=0;)
		{
		TInt jj=pictureMap->At(kk);
		TPictureHeader* sHeader=(*aSource->iPhraseIx)[jj].PictureHeaderPtr();
		if (!sHeader)
		    {
		    OstTrace0( TRACE_DUMP, CRICHTEXTINDEX_APPENDPHRASEINDEXL, "Invariant" );
		    }
		__ASSERT_DEBUG(sHeader,User::Invariant());
		if (sHeader->iPicture.IsPtr())
			{	// transfer picture to us
			TPictureHeader* tHeader=(*iPhraseIx)[jj+originalPhraseCount].PictureHeaderPtr();
			if (!tHeader)
			    {
			    OstTrace0( TRACE_DUMP, DUP1_CRICHTEXTINDEX_APPENDPHRASEINDEXL, "Invariant" );
			    }
			__ASSERT_DEBUG(tHeader,User::Invariant());
			tHeader->iPicture=sHeader->iPicture.AsPtr();
			sHeader->iPicture=NULL;
			}
		}

	CleanupStack::PopAndDestroy();	// pictureMap
	}


void CRichTextIndex::AppendParagraphL(const CParaFormatLayer* aGlobalParaFormatLayer,
									  const CCharFormatLayer* aGlobalCharFormatLayer,
									  TInt aReplicas)
// Append aReplicas empty paragraphs, the format of which is based on the
// global format layers.
//
	{
	__TEST_INVARIANT;

	// add the shared para format record
	CParaAttribs* paraAttribs=CParaAttribs::NewL(aGlobalParaFormatLayer,aGlobalCharFormatLayer);
	paraAttribs->iParaFormat->SetBase(aGlobalParaFormatLayer);  // reset the base properly
	paraAttribs->iCharFormat->SetBase(aGlobalCharFormatLayer);  // reset the base properly.
	CleanupStack::PushL(TCleanupItem(ReleaseOnCleanup,paraAttribs));
	CParaAttribs* paUsed=GetParaAttribsL(aGlobalParaFormatLayer,aGlobalCharFormatLayer,paraAttribs);
	// guaranteed not to leave as 3rd argument specified.
	// The refCount of paUsed is incremented here by one share.
	//
	// add the paragraph records
	TParaAttribsEntry paraEntry(1,paUsed);
	iParaIx->AppendL(paraEntry,aReplicas);
	if (paUsed!=paraAttribs)
		CleanupStack::PopAndDestroy();
	else
		CleanupStack::Pop();
	//
	// set the ref count of paUsed
	paUsed->iRefCount+=(aReplicas-1);  // compensate for already adding 1 share.

	__TEST_INVARIANT;
	}


TParaAttribsEntry::TParaAttribsEntry():
	iLength(0),
	iParaAttribs(NULL)
	{
	}


TParaAttribsEntry::TParaAttribsEntry(TInt aLength,CParaAttribs* aParaAttribs):
	iLength(aLength),
	iParaAttribs(aParaAttribs)
	{
	}


CParaAttribs* CParaAttribs::NewL(const CParaFormatLayer* aParaLayer,const CCharFormatLayer* aCharLayer)
// Returns a handle to a new instance of this class.
// Creates a CParaAttribs with constant character formatting.
//
	{
	CParaAttribs* self=new(ELeave) CParaAttribs();
	CleanupStack::PushL(self);
	self->iParaFormat=CParaFormatLayer::NewCopyBaseL(aParaLayer);
	// iParaFormat will be released in destructor.
	// To prevent Coverity from reporting defect, add a comment:
	// coverity[leave_without_push]
	self->iCharFormat=CCharFormatLayer::NewCopyBaseL(aCharLayer);
	CleanupStack::Pop();
	self->iRefCount=EPrimeSharedCount;
	return self;
	}


CParaAttribs* CParaAttribs::NewL(const CParaFormatLayer* aParaLayer)
// Returns a handle to a new instance of this class.
// Creates a CParaAttribs for specific character formatting.
//
	{
	CParaAttribs* self=new(ELeave) CParaAttribs();
	CleanupStack::PushL(self);
	self->iParaFormat=CParaFormatLayer::NewCopyBaseL(aParaLayer);
	CleanupStack::Pop();
	self->iRefCount=EPrimeNonSharedCount;
	self->iPhraseCount=1;
	return self;
	}


CParaAttribs* CParaAttribs::NewL(const CParaAttribs* aParaAttribs)
	{
	return NewL(aParaAttribs->iParaFormat,aParaAttribs->iCharFormat);
	}


CParaAttribs::CParaAttribs():
	iRefCount(-1)  // Ensures Destruct works correctly when called on a semi-initialised object.
	{
	}


void CParaAttribs::Release()
// Release a share on this CParaAttribs.
// If after this, no shares remain, destroy this CParaAttribs.
//
	{
	iRefCount--;
	if (iRefCount<=0)
		delete this;
	}


void CParaAttribs::Release(TInt aCount)
// Release aCount number of shares of this CParaAttribs.
// If after this, no shares remain, destroy this CParaAttribs.
//
	{
	iRefCount-=aCount;
	if (iRefCount<=0)
		delete this;
	}


CParaAttribs::~CParaAttribs()
// Release the memory associated with this object.
//
	{
	delete iParaFormat;
	if (iRefCount==0)
		{// Constant character formatting - in the shared list.
		delete iCharFormat;
		link.Deque();  // Remove this para attribs from the shared list.
		}
	}


TInt CParaAttribs::PhraseCount()const
// Return a count of the number of phrases in this para attribs.
//
	{return (iRefCount>=1)?1:iPhraseCount;}


DLLEXPORT_C void RPhraseAttribsEntry::__DbgTestInvariant()const
// Class invariants.
//
	{
#ifdef _DEBUG
// ASSERT: iLength is +ve (applying to character formatting, or	is set to indicate a picture phrase.
	if (iLength<0 && !IsPicturePhrase())
	    {
	    OstTrace0( TRACE_DUMP, RPHRASEATTRIBSENTRY_DBGTESTINVARIANT, "Invariant" );
	    }
	__ASSERT_DEBUG(iLength>=0 || IsPicturePhrase(),User::Invariant());
#endif
	}


RPhraseAttribsEntry::RPhraseAttribsEntry():
	iLength(0),
	iCharFormat(NULL)
	{
	}


RPhraseAttribsEntry::RPhraseAttribsEntry(CCharFormatLayer* aCharFormat,TInt aLength):
	iLength(aLength),
	iCharFormat(aCharFormat)
	{
	}


RPhraseAttribsEntry::RPhraseAttribsEntry(CPicturePhrase* aPicturePhrase):
	iLength(EPictureIndicator),
	iPicturePhrase(aPicturePhrase)
	{
	}


void RPhraseAttribsEntry::AssignAndRelease(const RPhraseAttribsEntry& aPhrase)
// Assign the state of the specified object to this,
//
	{
	iLength=aPhrase.iLength;
	iCharFormat=aPhrase.iCharFormat;  // both union members share the same address space, so this is fine.
	}


void RPhraseAttribsEntry::Discard()
// Free storage.
//
	{
	__TEST_INVARIANT;

	if (iLength==EPictureIndicator)
		delete iPicturePhrase;
	else
		delete iCharFormat;
	}


void RPhraseAttribsEntry::ExternalizeL(RWriteStream& aStream)const
// Save this phrase into aStream.
//
	{
	TUint8 picIndicator=(TUint8)(IsPicturePhrase()!=EFalse);
	aStream.WriteUint8L(picIndicator);
	aStream.WriteInt32L(Length());
	aStream<< *CharFormat();
	if ((TBool)picIndicator)
		aStream<< *PictureHeaderPtr();
	}


CCharFormatLayer* RPhraseAttribsEntry::CharFormat()const
 // Returns a pointer the CCharFormatLayer of this phrase.
 //
 	{return (iLength==EPictureIndicator)?iPicturePhrase->iCharFormat:iCharFormat;}


void RPhraseAttribsEntry::SetLength(TInt aLength)
// Sets the phrase length.
//
	{
	__TEST_INVARIANT;

	iLength=aLength;
	}


void RPhraseAttribsEntry::AdjustLength(TInt aIncrement)
// Adjusts the length of the phrase by the signed value aIncrement.
//
	{
// ASSERT: The length of a picture phrase may only be altered by deleting it, in which case
//			the only adjustment made will be an increment of -1 (EPictureIndicator).
	if (IsPicturePhrase() && (!IsPicturePhrase() || aIncrement!=EPictureIndicator)
                                      && (!IsPicturePhrase() || aIncrement!=0) )
	    {
	    OstTrace0( TRACE_DUMP, RPHRASEATTRIBSENTRY_ADJUSTLENGTH, "EModifiedPicturePhraseLength" );
	    }
	__ASSERT_DEBUG(!IsPicturePhrase() || (IsPicturePhrase() && aIncrement==EPictureIndicator)
									  || (IsPicturePhrase() && aIncrement==0)
									  ,Panic(EModifiedPicturePhraseLength));
	TInt len=iLength;
	iLength=(len==EPictureIndicator)
		? len-aIncrement
		: len+aIncrement;
	}


TInt RPhraseAttribsEntry::GetPictureSizeInTwips(TSize& aSize)const
// If this is a picture phrase, write the size of the picture to aSize,
// otherwise return KErrNotFound.
//
	{
	if (!IsPicturePhrase())
		return KErrNotFound;
	if (iPicturePhrase->iPicHdr.iPicture.IsPtr() && iPicturePhrase->iPicHdr.iPicture.AsPtr())
		iPicturePhrase->iPicHdr.iPicture->GetSizeInTwips(aSize);
	else
		aSize=iPicturePhrase->iPicHdr.iSize;
	return KErrNone;
	}


TPictureHeader RPhraseAttribsEntry::PictureHeader()const
// Return the picture header describing the picture at character position aPos.
// If there is no picture at character position aPos, a default picture header is returned.
//
	{
	return (IsPicturePhrase())
		? iPicturePhrase->iPicHdr
		: TPictureHeader();
	}


const CPicture* RPhraseAttribsEntry::PictureHandleL(const MPictureFactory* aFactory,
													const MRichTextStoreResolver* aResolver,
													TInt aPos,
													MLayDoc::TForcePictureLoad aForceLoad)const
// For a text phrase returns NULL.
// For a picture phrase returns a pointer to the picture object itself.
// May leave since loading of pictures is deferred until this point,
// ie, when the picture is required.
// Also returns NULL if the picture is not in memory *and* TForcePictureLoad is false.
//
	{
	if (iLength!=EPictureIndicator)
		return NULL;
	else
		{// Check if the picture is in memory
		CPicture* handle=NULL;
		if (iPicturePhrase->iPicHdr.iPicture.IsPtr())
			handle=iPicturePhrase->iPicHdr.iPicture;
		else
			{// Check if the picture should be loaded from persistent storage
			if (aForceLoad==MLayDoc::EForceLoadTrue)  // picture not in memory, so load it.
				{
				if (aResolver==NULL || aFactory==NULL)
					return NULL;
				const CStreamStore& store=aResolver->StreamStoreL(aPos);
				aFactory->NewPictureL(iPicturePhrase->iPicHdr,store);
				handle=iPicturePhrase->iPicHdr.iPicture;
				}
			}
		return handle;
		}
	}


TPictureHeader* RPhraseAttribsEntry::PictureHeaderPtr()const
// For a [text] phrase returns NULL.
// For a picture phrase returns a pointer to the picture header.
//
	{return (iLength==EPictureIndicator)? &iPicturePhrase->iPicHdr:NULL;}


CCharFormatLayer* RPhraseAttribsEntry::ReleaseCharFormatLayerOwnership()
// Return a handle to the character format layer, then set this handle to it to NULL.
// thus giving up ownership of the object.
//
	{
	CCharFormatLayer*& layer=(iLength==EPictureIndicator)?iPicturePhrase->iCharFormat:iCharFormat;
	CCharFormatLayer* charFormatLayer=layer;
	layer=NULL;
	return charFormatLayer;
	}


TBool RPhraseAttribsEntry::IsIdentical(const RPhraseAttribsEntry& aPhrase)const
// Returns ETrue if this phrase is identical to the specified phrase.
// otherwise returns EFalse.
// The 2 are equal only if they are both non-zero length text phrases, and of identical character format.
//
	{
	if (iLength<=0)				// picture or zero-length
		return EFalse;
	if (aPhrase.iLength<=0)		// picture or zero-length
		return EFalse;
	return iCharFormat->IsIdentical(aPhrase.iCharFormat,EFalse); // EFalse=do not compare based-on link
	}


CPicturePhrase* CPicturePhrase::NewL(const TPictureHeader& aPicHdr,TCharFormatX& aFormat,
									 TCharFormatXMask& aMask,CCharFormatLayer* aCharBase,
									 TBool& aPictureOwnershipTaken)
	{
	CPicturePhrase* self=new(ELeave) CPicturePhrase(aPicHdr,aPictureOwnershipTaken);
	CleanupStack::PushL(self);
	self->ConstructL(aFormat,aMask,aCharBase);
	CleanupStack::Pop();
	return self;
	}


// The charLayer is assumed to have a valid based-on link already set.
// Called as part of the rich text index Internalize only.
CPicturePhrase* CPicturePhrase::NewL(const TPictureHeader& aPicHdr,
                                     CCharFormatLayer* aCharLayer,
                                     TBool& aPictureOwnershipTaken)
	{
	return new(ELeave) CPicturePhrase(aPicHdr,aCharLayer,aPictureOwnershipTaken);
	}


CPicturePhrase::CPicturePhrase(const TPictureHeader& aPicHdr,TBool& aPictureOwnershipTaken):
	iPicHdr(aPicHdr)
	{
   	aPictureOwnershipTaken=ETrue;
	}


CPicturePhrase::CPicturePhrase(const TPictureHeader& aPicHdr,
                               CCharFormatLayer* aCharLayer,
                               TBool& aPictureOwnershipTaken):
	iCharFormat(aCharLayer),
	iPicHdr(aPicHdr)
	{
	aPictureOwnershipTaken=ETrue;
	}


void CPicturePhrase::ConstructL(TCharFormatX& aFormat,TCharFormatXMask& aMask,CCharFormatLayer* aCharBase)
	{
	iCharFormat=CCharFormatLayer::NewL(aFormat,aMask);
	iCharFormat->SetBase(aCharBase);
	}


CPicturePhrase::~CPicturePhrase()
	{
	iPicHdr.DeletePicture();
	delete iCharFormat;
	}

TLogicalPosition::TLogicalPosition():
	iDocPos(0),
	iParaElement(0),
	iParaElementOffset(0),
	iPhraseElement(0),
	iPhraseElementOffset(0)
	{
	}


void TLogicalPosition::Clear()
// Reset this logical position record.
//
	{
	*this=TLogicalPosition();
	iParaBasePhraseElement=0;
	}
