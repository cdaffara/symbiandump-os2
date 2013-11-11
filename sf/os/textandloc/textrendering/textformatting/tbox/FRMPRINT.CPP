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


#include "FRMPRINT.H"
#include "FRMCONST.H"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "FRMPRINTTraces.h"
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "FRMCONST_INTERNAL.H"
#include "FRMCONST_PARTNER.H"
#endif

/** Page region printer interface.
 @publishedAll
 @return CTextPageRegionPrinter returned
 @param aLayDoc Information needed by the text layout engine to lay out a text object.
 @param aPrinterDevice Physical graphics device.
 @post CTextPageRegionPrinter object is now fully initialised
 */
EXPORT_C CTextPageRegionPrinter* CTextPageRegionPrinter::NewL(MLayDoc* aLayDoc,CPrinterDevice* aPrinterDevice)
	{
	CTextPageRegionPrinter* self=new(ELeave) CTextPageRegionPrinter(); 
	CleanupStack::PushL(self);
	self->ConstructL(aLayDoc,aPrinterDevice);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CTextPageRegionPrinter::~CTextPageRegionPrinter()
	{
	delete iLayout;
	delete iGc;
	delete iPictureGc;
	}

/**
 Sets the page list.
 @publishedAll
 @param aPageList Page list.
 */
EXPORT_C void CTextPageRegionPrinter::SetPageList(const CArrayFix<TInt>* aPageList)
	{
	iPageList=aPageList;
	}


/**
 Sets the printer device.
 @publishedAll
 @param aPrinterDevice Physical graphics device.
 */
 EXPORT_C void CTextPageRegionPrinter::SetPrinterDevice(CPrinterDevice* aPrinterDevice)
	{
	iPrinterDevice=aPrinterDevice;
	iImageDevice=aPrinterDevice;

	iCurrentPage=KMinTInt32;	 // To force a call to SetPageL when PrintBandL first called.

	iPageSizeInTwips=aPrinterDevice->CurrentPageSpecInTwips().OrientedPageSize();
	}

/**
 Sets the layout document.
 @publishedAll
 @param aDoc Layout document.
 */
EXPORT_C void CTextPageRegionPrinter::SetDocument(MLayDoc *aDoc)
	{
	iCurrentPage=KMinTInt32;	 // To force a call to SetPageL when PrintBandL first called.
	iLayout->SetLayDoc(aDoc);
	}

/**
 Sets the print preview.
 @publishedAll
 @param aPrintPreview.
 */
EXPORT_C void CTextPageRegionPrinter::SetPrintPreview(TBool aPrintPreview)
	{
	iPrintPreview=aPrintPreview;
	iCurrentPage=KMinTInt32;	 // To force a call to SetPageL when PrintBandL first called.
	}

/**
 Sets the Page Spec In Twips.
 @publishedAll
 @param aPageSpec.
 */
EXPORT_C void CTextPageRegionPrinter::SetPageSpecInTwips(const TPageSpec& aPageSpec)
	{
	iPageSizeInTwips=aPageSpec.OrientedPageSize();
	}

/**
 Sets the page margin in twips.
 @publishedAll
 @param aPageMargins.
 */
EXPORT_C void CTextPageRegionPrinter::SetPageMarginsInTwips(const TMargins& aPageMargins)
	{
	iPageMarginsInTwips=aPageMargins;
	}

/**
 Sets the text margin widths in twips.
 @publishedAll
 @param aLabelMarginWidth
 @param aGutterMarginWidth
 */
EXPORT_C void CTextPageRegionPrinter::SetTextMarginWidthsInTwips(TInt aLabelMarginWidth,TInt aGutterMarginWidth) 
	{
	iLabelMarginWidthInTwips=aLabelMarginWidth;
	iGutterMarginWidthInTwips=aGutterMarginWidth;
	}

/**
 Set the number given to the first page of the document that has been paginated.
 Typically 1, but might be different (eg for document containing second chapter of book).
 @publishedAll
 @param aFirstPage
 */
EXPORT_C void CTextPageRegionPrinter::SetFirstPageOfDoc(TInt aFirstPage)
	{
	iFirstPage=aFirstPage;
	}


/**
 Set the number given to the first page of the document that has been paginated.
 Typically 1, but might be different (eg for document containing second chapter of book).
 @publishedAll
 @param aImageDevice
 @param aPageNo
 @param aBandInPixels
 */
EXPORT_C void CTextPageRegionPrinter::PrintBandL(CGraphicsDevice* aImageDevice,TInt aPageNo,const TBandAttributes& aBandInPixels)
//
// Traps leaves
//
	{
	if (!iPageList)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CTEXTPAGEREGIONPRINTER_PRINTBANDL, "EFInvalidPageList" );
	    }
	__ASSERT_ALWAYS(iPageList!=NULL,FormPanic(EFInvalidPageList));
	if (aPageNo-iFirstPage>=iPageList->Count())
	    {
	    OstTrace0( TRACE_DUMP, CTEXTPAGEREGIONPRINTER_PRINTBANDL, "EFInvalidPageNumber" );
	    }
	if (aPageNo<iFirstPage)
	    {
	    OstTrace0( TRACE_DUMP, DUP2_CTEXTPAGEREGIONPRINTER_PRINTBANDL, "EFInvalidPageNumber" );
	    }
	__ASSERT_DEBUG(aPageNo-iFirstPage<iPageList->Count(),FormPanic(EFInvalidPageNumber));
	__ASSERT_DEBUG(aPageNo>=iFirstPage,FormPanic(EFInvalidPageNumber));

	TRAPD(err,PrintBand2L(aImageDevice,aPageNo,aBandInPixels));
	if (err)
		LeaveL(err);
	}

/**
 Sets the fill to either the text box or whole view rect occupied by paragraph.
 @publishedAll
 @deprecated 7.0 
 @param aFillTextOnly
 */
EXPORT_C void CTextPageRegionPrinter::SetParagraphFillTextOnly(TBool aFillTextOnly)
	{
	iDrawTextLayoutContext.SetParagraphFillTextOnly(aFillTextOnly);
	}

/**
 Takes the specified band and draws to specified Gc those lines that intersect the band on the page.
 @param aImageDevice The specified Graphics device.
 @param aPageNo The page number.
 @param aBandInPixels The specified band in pixels.
 */
void CTextPageRegionPrinter::PrintBand2L(CGraphicsDevice* aImageDevice,TInt aPageNo
															,const TBandAttributes& aBandInPixels)
	{

	iImageDevice=aImageDevice;

	if (aBandInPixels.iFirstBandOnPage || aPageNo!=iCurrentPage)
		SetPageL(aPageNo);

	if (!iGc)
		{
		User::LeaveIfError(iImageDevice->CreateContext((CGraphicsContext *&) iGc));
		if (!aBandInPixels.iGraphicsIsIgnored)
			User::LeaveIfError(iImageDevice->CreateContext((CGraphicsContext *&) iPictureGc));
		iDrawTextLayoutContext.SetGc(iGc,iPictureGc);
		}

	iDrawTextLayoutContext.SetDrawTextAndGraphics();
	if (aBandInPixels.iTextIsIgnored && aBandInPixels.iGraphicsIsIgnored)
		return;
	else if (aBandInPixels.iTextIsIgnored)
		iDrawTextLayoutContext.SetDrawGraphicsOnly();
	else if (aBandInPixels.iGraphicsIsIgnored)
		iDrawTextLayoutContext.SetDrawTextOnly();


	iLayout->DrawL(aBandInPixels.iRect,&iDrawTextLayoutContext);
	iDrawTextLayoutContext.SetDrawTextAndGraphics();

	delete iGc;
	iGc=NULL;
	delete iPictureGc;
	iPictureGc=NULL;
	}

 CTextPageRegionPrinter::CTextPageRegionPrinter()
	{
	iDrawTextLayoutContext.SetDrawToEveryPixel(EFalse);
	iDrawTextLayoutContext.SetClipping(EFalse);
	}

/**
 Constructs a CTextPageRegionPrinter.

 Defaults are that no label margin nor gutter margin and that text is formatted and displayed
 to the printable page.
 
 @param aLayDoc
 @param aPrinterDevice The printer device.
 */
 void CTextPageRegionPrinter::ConstructL(MLayDoc* aLayDoc,CPrinterDevice* aPrinterDevice)
	{

	iPrintPreview=EFalse;
	iPageSizeInTwips=aPrinterDevice->CurrentPageSpecInTwips().OrientedPageSize();
	iPrinterDevice=aPrinterDevice;
	iImageDevice=aPrinterDevice;

	iCurrentPage=KMinTInt32;	 // To force a call to SetPageL when PrintBandL first called.

	TInt textWidth=aPrinterDevice->HorizontalTwipsToPixels(LayoutWidthInTwips());
	iLayout=CTextLayout::NewL(aLayDoc,textWidth);
	iLayout->SetAmountToFormat(CTextLayout::EFFormatBand);

	SetFirstPageOfDoc(1);
	}


/**
 Set the format and Image devices.
 All these calls must pass values in twips to textlayout and image pixels to textdraw
 */
void CTextPageRegionPrinter::SetFormatAndImageDevices()
	{

	iDrawTextLayoutContext.iViewRect=iImageDevice->TwipsToPixels(ViewRectInTwips());
	iDrawTextLayoutContext.iGutterMarginWidth=iImageDevice->HorizontalTwipsToPixels(iGutterMarginWidthInTwips);
	iDrawTextLayoutContext.iLabelMarginWidth=LabelMarginWidthInPixels();
	iDrawTextLayoutContext.iTextStartX=TextStartXInPixels();

	// iLayout - everything in twips
	iLayout->SetBandHeight(ViewRectInTwips().Height());
	iLayout->SetImageDeviceMap(iImageDevice);
	iLayout->SetLabelsMarginWidth(LabelMarginWidthInPixels());
	if (iPrintPreview)
		iLayout->SetFormatMode(CLayoutData::EFPrintPreviewMode,LayoutWidthInTwips(),iPrinterDevice);
	else
		iLayout->SetFormatMode(CLayoutData::EFPrintMode,LayoutWidthInTwips(),iPrinterDevice);
	
	}

/**
 Gets the view rect in twips.
 @return Returns the view rect in pixels
 */
TRect CTextPageRegionPrinter::ViewRectInTwips() const
	{
	TRect viewRect;

	if (iLabelMarginWidthInTwips==0)
		viewRect.iTl.iX=0;
	else
		viewRect.iTl.iX=iPageMarginsInTwips.iLeft;
	viewRect.iTl.iY=iPageMarginsInTwips.iTop;
	viewRect.iBr.iX=iPageSizeInTwips.iWidth;
	viewRect.iBr.iY=iPageSizeInTwips.iHeight-iPageMarginsInTwips.iBottom;

	return viewRect;
	}

/**
 Returns the width of page, excluding margins.
 @return Returns the width of page, excluding margins
 */
TInt CTextPageRegionPrinter::LayoutWidthInTwips() const
	{
	TInt width=iPageSizeInTwips.iWidth-(iPageMarginsInTwips.iLeft+iPageMarginsInTwips.iRight);
	return width-(iGutterMarginWidthInTwips+iLabelMarginWidthInTwips);
	}

/**
 Gets the label margin width in pixels.
 @return Label margin width in pixels.
 */
TInt CTextPageRegionPrinter::LabelMarginWidthInPixels()	const
	{

	return iImageDevice->HorizontalTwipsToPixels(iLabelMarginWidthInTwips);
	}

/**
 Get the text start X in pixels.
 @return text start X in pixels.
 */
TInt CTextPageRegionPrinter::TextStartXInPixels() const
	{

	if (iLabelMarginWidthInTwips==0)
		return iImageDevice->HorizontalTwipsToPixels(iPageMarginsInTwips.iLeft);
	else
		return 0;
	}

/**
 Calculates the docPos at the top of this page, by summing throught the CharsPerPage.
 Can be called for aPage==0, when loop not entered.
 
 @return Explanation of the object returned
 @param aPage Page Number.
 */
void CTextPageRegionPrinter::SetPageL(TInt aPage)
	{
	TInt pagesToScroll=0;
	TInt pageTextHeightInPixels=0;		//To stop warning
	iCurrentPage=aPage;
	iTopPageDocPos=0;
	TInt page=iFirstPage;

	SetFormatAndImageDevices();

	while (page<aPage)
		{
		__ASSERT_DEBUG((*iPageList)[page-iFirstPage]>=0,FormPanic(EFInvalidPageList));
		if ((*iPageList)[page-iFirstPage]>0)
			{
			iTopPageDocPos+=(*iPageList)[page-iFirstPage];
			pagesToScroll=0;
			}
		else
			++pagesToScroll;
		page++;
		//Count zeros --> n
		}

	__ASSERT_DEBUG((*iPageList)[iCurrentPage-iFirstPage]>=0,FormPanic(EFInvalidPageList));
	TInt bottomPageDocPos=iTopPageDocPos+(*iPageList)[iCurrentPage-iFirstPage]-1;
	if (bottomPageDocPos<iTopPageDocPos)
		++bottomPageDocPos;
	__ASSERT_DEBUG(iTopPageDocPos<=iLayout->DocumentLength(),FormPanic(EFInvalidPageList));
	__ASSERT_DEBUG(bottomPageDocPos>=0 && bottomPageDocPos<=iLayout->DocumentLength(),FormPanic(EFInvalidPageList));
	__ASSERT_DEBUG(bottomPageDocPos>=iTopPageDocPos,FormPanic(EFInvalidPageList));

	if (pagesToScroll>0)
		pageTextHeightInPixels=iImageDevice->VerticalTwipsToPixels(iPageSizeInTwips.iHeight-iPageMarginsInTwips.iTop-iPageMarginsInTwips.iBottom);
	iLayout->DiscardFormat();
	TRAPD(err,iLayout->FormatCharRangeL(iTopPageDocPos,bottomPageDocPos,pagesToScroll*pageTextHeightInPixels));
	//Scroll by n*Page Height
	if (err)
		LeaveL(err);
	}

void CTextPageRegionPrinter::LeaveL(TInt aErr)
//
// There is not enough memory to do the formatting, or other leave
// Reset everything.
//
	{

	iCurrentPage=KMinTInt32;
	iLayout->DiscardFormat();

	delete iGc;
	iGc=NULL;

	delete iPictureGc;
	iPictureGc=NULL;
	
	OstTrace1( TRACE_FATAL, CTEXTPAGEREGIONPRINTER_LEAVEL, "CTextPageRegionPrinter::LeaveL;aErr=%d", aErr );
	User::Leave(aErr);
	}

