/*
* Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "FRMPAGE.H"
#include "FRMTLAY.H"
#include "FRMCONST.H"
#include <txtlaydc.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "FRMPAGETraces.h"
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "FRMCONST_INTERNAL.H"
#include "FRMCONST_PARTNER.H"
#endif

/** Allocates and constructs a CTextPaginator object with a page list, the
printer device for which the document is to be paginated and an active object
priority.

@param aPrinterDevice Pointer to the printer device for which the document is
to be paginated. This must be provided.
@param aCharsPerPage The page list. This is a client-provided array into which
characters-per-page values are written. Ownership of the array remains with the
client.
@param aPriority Integer specifying the active object priority. A number of
standard priorities are specified in CActive::TPriority.
@return Pointer to the new paginator object. */
EXPORT_C CTextPaginator* CTextPaginator::NewL(CPrinterDevice* aPrinterDevice,CArrayFix<TInt>* aPageList,TInt aPriority)
	{
	CTextPaginator* self=new(ELeave) CTextPaginator(aPriority); 
	CleanupStack::PushL(self);
	self->ConstructL(aPrinterDevice,aPageList);
	CleanupStack::Pop();
	return self;
	}

/** Destructor. Cancels the active object, if any. */
EXPORT_C CTextPaginator::~CTextPaginator()
	{
	Cancel();
	delete iLayout;
	delete iPageLineArray;
	delete iTempPageList;
	}

/** Sets a pagination observer (an instance of a class inherited from
MPaginateObserver). The use of an observer is optional.

An observer may be used when paginating a complete document in the background
using the function PaginateCompleteDocumentL(). The observer notifies
completion of pages, cancellation, errors, and on completion of multiple
pagination.

@param aObserver Observer object inherited from MPaginateObserver. */
EXPORT_C void CTextPaginator::SetObserver(MPaginateObserver* aObserver)
	{

	iObserver=aObserver;
	}

/** Sets a pointer to the document which is to be paginated.
@param aLayDoc The document to paginate. */
EXPORT_C void CTextPaginator::SetDocumentL(MLayDoc* aLayDoc)
	{

	SetOrReplaceDocumentL(aLayDoc);
	iPaginator.Reset();
	iDocPos=0;
	SetLayoutDimensions();
	}

/** Sets a pointer to the printer device for which the document is to be
paginated.

Note: This function must be called, and SetDocumentL() must have been called
beforehand.

@param aPrinterDevice The printer device. */
EXPORT_C void CTextPaginator::SetPrinterDevice(CPrinterDevice* aPrinterDevice)
	{
	if (!iLayout)
	    {
	    OstTrace0( TRACE_FATAL, CTEXTPAGINATOR_SETPRINTERDEVICE, "EFDocumentToPaginateNotSet" );
	    }
 	__ASSERT_ALWAYS(iLayout,FormPanic(EFDocumentToPaginateNotSet));
	iPrinterDevice=aPrinterDevice;
	iLayout->SetImageDeviceMap(aPrinterDevice);

	iPageSizeInTwips=aPrinterDevice->CurrentPageSpecInTwips().OrientedPageSize();
 	SetLayoutDimensions();
	}

/** Sets the page width and height in twips, overriding the current values
specified in the printer device.
@param aPageSpec Contains the new page dimensions. */
EXPORT_C void CTextPaginator::SetPageSpecInTwips(const TPageSpec& aPageSpec)
	{

	iPageSizeInTwips=aPageSpec.OrientedPageSize();
 	SetLayoutDimensions();
	}

/** Sets the widths of the page margins in twips.

The page margin exists on all four sides of the page. It does not include the
line cursor or labels margins. The labels and line cursor margins are set using
SetTextMarginWidthsInTwips().

@param aPageMargins The page margin widths. */
EXPORT_C void CTextPaginator::SetPageMarginsInTwips(const TMargins& aPageMargins)
	{
	iPageMarginsInTwips=aPageMargins;
 	SetLayoutDimensions();
	}

 /** Sets the widths in twips of:

the labels margin the area within which paragraph labels are displayed,

the gutter margin (also known as the line cursor margin) exists between the
labels margin and the text area.

@param aLabelMarginWidth The labels margin width.
@param aGutterMarginWidth The gutter margin width. */
EXPORT_C void CTextPaginator::SetTextMarginWidthsInTwips(TInt aLabelMarginWidth
	,TInt aGutterMarginWidth) 
	{
	iLabelMarginWidthInTwips=aLabelMarginWidth;
	iGutterMarginWidthInTwips=aGutterMarginWidth;
 	SetLayoutDimensions();
	}

CTextPaginator::CTextPaginator(TInt aPriority)
	:CActive(aPriority)
	{
	}

void CTextPaginator::ConstructL(CPrinterDevice* aPrinterDevice,CArrayFix<TInt>* aPageList)
	{

	iPageLineArray=new(ELeave) CArrayFixFlat<TPageLine>(EPageLineArrayGranularity);
	iTempPageList=new(ELeave) CArrayFixFlat<TInt>(EPageListArrayGranularity);
	iPrinterDevice=aPrinterDevice;
	iPageList=aPageList;

	iPaginator.SetArray(iTempPageList);
	iPaginator.SetPageHeight(TextSizeInPixels().iHeight);

	iPageSizeInTwips=iPrinterDevice->CurrentPageSpecInTwips().OrientedPageSize();
	SetLayoutDimensions();
	ResetPaginator();
	}

/** Initiates pagination of a complete document in the background using an
active object. To start pagination, use either this function, or else
incrementally paginate with AppendTextL() do not try to use both functions
together.

Note: SetDocumentL() must have been called beforehand, or a panic occurs. */
EXPORT_C void CTextPaginator::PaginateCompleteDocumentL()
	{
	if (!iLayout)
	    {
	    OstTrace0( TRACE_FATAL, CTEXTPAGINATOR_PAGINATECOMPLETEDOCUMENTL, "EFDocumentToPaginateNotSet" );
	    }
 	__ASSERT_ALWAYS(iLayout,FormPanic(EFDocumentToPaginateNotSet));
  	if (iPageList->Count()==0)
		iPageList->AppendL(iLayDoc->LdDocumentLength());
	if (!IsAdded())
		CActiveScheduler::Add(this); // Adds itself to the active scheduler
 	iMode=EFPaginateCompleteDocument;
	ResetPaginator();
	Reque();
	}

/** Paginates incrementally as a document is being constructed (by appending
paragraphs, for example). Call this function every time text is added to the
document.

The function PaginationCompletedL() should be called at the end (in order to
complete the last entry in the characters-per-page array).

Use either this function, or else paginate in the background with
PaginateCompleteDocumentL() - do not try to use both functions together.

Note: SetDocumentL() must have been called beforehand, or a panic occurs.

@param aCumulativeDocPos The first time the function is called, this should be
given a value of zero. Returns the last document position which has been
paginated.
@return A count of the current number of pages. */
EXPORT_C TInt CTextPaginator::AppendTextL(TInt& aCumulativeDocPos)
	{
	if (!iLayout)
	    {
	    OstTrace0( TRACE_FATAL, CTEXTPAGINATOR_APPENDTEXTL, "EFDocumentToPaginateNotSet" );
	    }
	__ASSERT_ALWAYS(iLayout,FormPanic(EFDocumentToPaginateNotSet));
	if (aCumulativeDocPos>=iLayout->DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CTEXTPAGINATOR_APPENDTEXTL, "EFInvalidDocPos" );
	    }
 	__ASSERT_ALWAYS(aCumulativeDocPos<iLayout->DocumentLength(),FormPanic(EFInvalidDocPos));
 	iMode=EFPaginateIncrementally;

  	if (iPageList->Count()==0)
		ResetPaginator();

	TBool moreToDo=ETrue;
	while(moreToDo)
		{
		moreToDo = iDocPos<=iLayDoc->LdDocumentLength();
		if (moreToDo)
			{
			TrapPaginateParagraphL();
			}
		else
			{
			if (iMode==EFPaginateCompleteDocument)
				{
				iPaginator.FlushL(iDocPos);
				PageCompleted();
				}
			iPageLineArray->Reset();
			iPageLineArray->Compress();
			}
		}

	aCumulativeDocPos=iDocPos;

	TRAPD(err,CopyTempPageListL());
	if (err)
	    {
		LeaveL(err);
	    }
	return iPageList->Count();
	}

/** This function should be called when incremental pagination has completed
(see AppendTextL()), to complete the final entry in the page list. If an
observer has been set, calls its NotifyCompletion() function.

@return Count of total number of pages. */
EXPORT_C TInt CTextPaginator::PaginationCompletedL()
	{
	TRAPD(err,iPaginator.FlushL(iDocPos));
	if (err)
		LeaveL(err);
	iLayout->DiscardFormat();
	TRAP(err,CopyTempPageListL());
	if (err)
		LeaveL(err);
	if (iObserver)
		iObserver->NotifyCompletion();
	return iPageList->Count();
	}

void CTextPaginator::RunL()
//
// Called by active scheduler.
// Paginates a document one paragraph at a time through succeeding    
// calls.
//
	{
	TBool moreToDo = iDocPos<=iLayDoc->LdDocumentLength();
	if (moreToDo)
		{
		TrapPaginateParagraphL();
		}
	else
		{
		if (iMode==EFPaginateCompleteDocument)
			{
			iPaginator.FlushL(iDocPos);
			PageCompleted();
			}
		iPageLineArray->Reset();
		iPageLineArray->Compress();
		}
	
	if (moreToDo)
		Reque();
	else
		{
		iLayout->DiscardFormat();
		TRAPD(err,CopyTempPageListL());
		if (err)
			LeaveL(err);
		if (iObserver)
			iObserver->NotifyCompletion();
		}
	}

void CTextPaginator::DoCancel()
	{
	iPaginator.Reset();
	iLayout->DiscardFormat();
	iDocPos=0;
	iPageBreakChar=EFalse;

	if (iObserver)
		iObserver->NotifyError(KErrCancel);	
	}

void CTextPaginator::SetLayoutDimensions()
	{
	iPaginator.SetPageHeight(TextSizeInPixels().iHeight);
	if (iLayout)
		iLayout->SetFormatMode(CLayoutData::EFPrintMode,TextRectInTwips().Width(),iPrinterDevice);
	}

void CTextPaginator::SetOrReplaceDocumentL(MLayDoc* aLayDoc)
	{

	iLayDoc=aLayDoc;
	if (iLayout)
		iLayout->SetLayDoc(aLayDoc);
	else
		iLayout=CTextLayout::NewL(aLayDoc,TextSizeInPixels().iWidth);
	iLayout->SetImageDeviceMap(iPrinterDevice);
	}

TRect CTextPaginator::TextRectInTwips() const
	{
	TRect textRect;

	textRect.iTl.iX=iPageMarginsInTwips.iLeft+iGutterMarginWidthInTwips+iLabelMarginWidthInTwips;
	textRect.iTl.iY=iPageMarginsInTwips.iTop;
	textRect.iBr.iX=iPageSizeInTwips.iWidth-iPageMarginsInTwips.iRight;
	textRect.iBr.iY=iPageSizeInTwips.iHeight-iPageMarginsInTwips.iBottom;

	return textRect;
	}

TSize CTextPaginator::TextSizeInPixels() const
	{
	TRect textRect=iPrinterDevice->TwipsToPixels(TextRectInTwips());

	return textRect.Size();
	}

void CTextPaginator::TrapPaginateParagraphL()
	{
	TRAPD(err,PaginateParagraphL());
	if (err)
		LeaveL(err);
	}

void CTextPaginator::PaginateParagraphL()
	{
	TInt lineHeight;
	TBool keepTogether;  // Prevents page break in a paragraph when ETrue.
	TBool keepWithNext;  // Prevents page break between this & next para when ETrue.
	TBool startNewPage;  // Inserts page break before this paragraph when ETrue.
	TBool widowOrphan;   // Prevents widowing/orphaning of para. lines when ETrue.
	TPageLine pageLine;
	TInt numLines;
	CParaFormat* paraFormat=NULL;

	paraFormat=CParaFormat::NewLC();
	iLayDoc->GetParagraphFormatL(paraFormat,iDocPos);
	TInt docPos=iDocPos;
	TBool startOfPara=(iLayDoc->LdToParagraphStart(docPos)==0);

	keepTogether=paraFormat->iKeepTogether;
	keepWithNext=paraFormat->iKeepWithNext;
	startNewPage=paraFormat->iStartNewPage;
	widowOrphan=paraFormat->iWidowOrphan;

	iPageLineArray->Reset();		   // Better safe than sorry at the moment ###
	iPageLineArray->Compress();

	TInt lines=0;
	TBool moreToDo = ETrue;
	do 
		{
		pageLine.iDocPos=iDocPos;
		pageLine.iStartNewPage=EFalse;
		if (iPageBreakChar)
			pageLine.iStartNewPage=ETrue;
		moreToDo=iLayout->FormatLineL(paraFormat,iDocPos,lineHeight,iPageBreakChar);
		lines++;
		pageLine.iLineHeight=lineHeight;
		if (keepTogether)
			pageLine.iKeepWithNext=ETrue;
		else
			pageLine.iKeepWithNext=EFalse;
		iPageLineArray->AppendL(pageLine);
		} while (moreToDo && lines<EFMaximumNumberLinesInBlock);


	TBool endOfPara=(!moreToDo);
	TBool penultimateLine=EFalse;
	numLines=iPageLineArray->Count();
	if (!endOfPara)
		{
		docPos=iDocPos;
		TBool pageBreakChar;
		penultimateLine=(!iLayout->FormatLineL(paraFormat,docPos,lineHeight,pageBreakChar));
		}

	if (startNewPage && startOfPara)
		(*iPageLineArray)[0].iStartNewPage=ETrue;

	if (keepTogether && endOfPara)
		(*iPageLineArray)[numLines-1].iKeepWithNext=EFalse;

	if (keepWithNext && endOfPara)
		(*iPageLineArray)[numLines-1].iKeepWithNext=ETrue;
		
	if (widowOrphan)
		{
		if (startOfPara)
			(*iPageLineArray)[0].iKeepWithNext=ETrue;
		if (endOfPara && numLines>=2)
			(*iPageLineArray)[numLines-2].iKeepWithNext=ETrue;
		else if (penultimateLine)
			(*iPageLineArray)[numLines-1].iKeepWithNext=ETrue;
		}

	TBool pageBreak = EFalse;
	for (TInt i=0; i<numLines; i++)
		{
		pageBreak=iPaginator.AppendLineL((*iPageLineArray)[i]);
		if (pageBreak)
			PageCompleted();
		}

	iPageLineArray->Reset();
	iPageLineArray->Compress();

	CleanupStack::PopAndDestroy();	// delete format;
	}

void CTextPaginator::PageCompleted()
	{
	if (iObserver)
		iObserver->NotifyPageCompletion(iTempPageList->Count());
	}

void CTextPaginator::Reque()
//
// Called just before Paginate Process goes to sleep to set it active 
//
	{
	TRequestStatus *pS=(&iStatus);
	User::RequestComplete(pS,0);
	SetActive();
	}

void CTextPaginator::ResetPaginator()
//
	{
	
	iDocPos=0;
	iPageBreakChar=EFalse;
	iPaginator.Reset();
	}

void CTextPaginator::CopyTempPageListL()
//
// Copies temp page list to one that external user sees
//
	{
	if (iTempPageList->Count()<1 && iMode!=EFPaginateIncrementally)
	    {
	    OstTrace0( TRACE_DUMP, CTEXTPAGINATOR_COPYTEMPPAGELISTL, "EFPageListEmpty" );
	    }
	__ASSERT_DEBUG(iTempPageList->Count()>=1||iMode==EFPaginateIncrementally,FormPanic(EFPageListEmpty));
	TRAPD(err,iPageList->ResizeL(iTempPageList->Count()));
	if (err)
		LeaveL(err);

	{for(TInt ii=0;ii<iTempPageList->Count();ii++)
		(*iPageList)[ii]=(*iTempPageList)[ii];
	}
	}


void CTextPaginator::LeaveL(TInt aErr)
//
// Something has left
// Reset everything.
//
	{
	iPaginator.Reset();
	iLayout->DiscardFormat();
	iDocPos=0;

	iPageLineArray->Reset();
	iPageLineArray->Compress();
	iTempPageList->Reset();
	iTempPageList->Compress();

	if (iObserver)
		iObserver->NotifyError(aErr);

	OstTrace1( TRACE_FATAL, CTEXTPAGINATOR_LEAVEL, "CTextPaginator::LeaveL;aErr=%d", aErr );
	
	User::Leave(aErr);
	}

