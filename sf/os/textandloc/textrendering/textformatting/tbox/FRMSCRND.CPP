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


#include "FRMTVIEW.H"
#include "FRMCONST.H"
#include "FORMUTIL.H"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "FRMSCRNDTraces.h"
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "FRMCONST_INTERNAL.H"
#include "FRMCONST_PARTNER.H"
#include "TAGMA_INTERNAL.H"
#endif

RScreenDisplay::RScreenDisplay(TDrawTextLayoutContext* const aDrawTextLayoutContext):
	iSession(NULL),
	iGroupWin(NULL),
	iWin(NULL),
	iGc(NULL),
	iDrawTextLayoutContext(aDrawTextLayoutContext),
	iBackground(KRgbWhite)
	{
	}

void RScreenDisplay::Close()
	//
	{

	DestroyContexts();
	}

void RScreenDisplay::Destroy()
	//
	{

	Close();
	delete this;
	}

void RScreenDisplay::DestroyContexts()
	//
	//Destroy the graphics contexts
	//
	{

	if (iGc)
		delete iGc;
	iGc=NULL;
	}

void RScreenDisplay::ActivateContext()
	//
	{
	ActivateContext(iGc);
	iBackground = iDrawTextLayoutContext->iBackgroundColor;
	const MFormParam* p = MFormParam::Get();
	FormUtil::LogicalToActualColor(p,iBackground);
	}

void RScreenDisplay::ActivateContext(CGraphicsContext *aGc)
	//
	//Activate a gc on the current window
	//
	{
	
	//__ASSERT_DEBUG(aGc!=NULL,FormPanic());
	if (iWin)
		((CWindowGc *) aGc)->Activate(*iWin);
	}

void RScreenDisplay::DeactivateContext()
	//
	{
	DeactivateContext(iGc);
	}

void RScreenDisplay::DeactivateContext(CGraphicsContext *aGc)
	//
	//Deactivate a gc on the current window
	//
	{
	
	//__ASSERT_DEBUG(aGc!=NULL,FormPanic());
	if (iWin)
		((CWindowGc *) aGc)->Deactivate();
	}

void RScreenDisplay::CreateContextL()
	//
	//Create and activate the main Graphics Context
	//
	{
	if (iGc)
	    {
	    OstTrace0( TRACE_DUMP, RSCREENDISPLAY_CREATECONTEXTL, "EFGcNotSet" );
	    }
	__ASSERT_DEBUG(iGc==NULL,FormPanic(EFGcNotSet));
	User::LeaveIfError(iGd->CreateBitmapContext(iGc));
	if (iWin)
		iDrawTextLayoutContext->SetWindowGc((CWindowGc *)iGc);
	else
		iDrawTextLayoutContext->SetBitmapGc(iGc);
	}

void RScreenDisplay::CreateContextL(CBitmapDevice *aGd)
	//
	//Create and activate the main Graphics Context
	//
	{
	CBitmapContext* localGc;
	localGc=NULL;
	//__ASSERT_DEBUG(iGc==NULL,FormPanic(EFGcNotSet));
	User::LeaveIfError(aGd->CreateBitmapContext(localGc));
	delete iGc;
	iGc=NULL;
	iGc=localGc;
	iGd=aGd;
	if (iWin)
		iDrawTextLayoutContext->SetWindowGc((CWindowGc *)iGc);
	else
		iDrawTextLayoutContext->SetBitmapGc(iGc);
	}

void RScreenDisplay::SetWindow(RWindow *aWin)
	//
	{

	iWin=aWin;
	}

void RScreenDisplay::SetGraphicsDeviceL(CBitmapDevice *aGd)
	//
	{

	//DestroyContexts();
	//iGd=aGd;
	CreateContextL(aGd);
	}

void RScreenDisplay::SetInvalidRect(const TRect& aRect)
	//
	{

	iInvalidRect=aRect;
	}

void RScreenDisplay::SetInvalidRect(TInt aHeight)
	//
	//Set the clipping region to the top or bottom abs(aHeight) pixels of the screen
	//aHeight<0 ==> bottom of the screen
	//
	{

	iInvalidRect=iDrawTextLayoutContext->iViewRect;
	if (aHeight<0)
		iInvalidRect.iTl.iY=iInvalidRect.iBr.iY+aHeight;
	else
		iInvalidRect.iBr.iY=iInvalidRect.iTl.iY+aHeight;
	}

void RScreenDisplay::SetInvalidRectHorizontal(TInt aWidth)
	//
	//Set the clipping region to the left or right abs(aHeight) pixels of the screen
	//aWidth<0 ==> right of the screen
	//
	{

	iInvalidRect=iDrawTextLayoutContext->TextArea(); 
	if (aWidth<0)
		iInvalidRect.iTl.iX=iInvalidRect.iBr.iX+aWidth;
	else
		iInvalidRect.iBr.iX=iInvalidRect.iTl.iX+aWidth;
	}

void RScreenDisplay::SetRects(TUint aRects)
	//
	//Set the clipping region to the union of rectangles specified in the enum
	//
	{

	iRects=aRects;
	}

void RScreenDisplay::AddRects(TUint aRects)
	//
	{

	iRects|=aRects;
	}

void RScreenDisplay::SubRects(TUint aRects)
	//
	{
	
	iRects&=EFClipAllFlags-aRects;
	}


TRect RScreenDisplay::ClippingRect()
	//
	{

	TRect clipRect(iDrawTextLayoutContext->iViewRect);

	if (!(iRects&EFClipViewRect))
		{
		if (iRects&EFClipTextArea)
			clipRect=iDrawTextLayoutContext->TextArea();
		else if (iRects&EFClipLineCursor)
			clipRect=LineCursorMargin();
		if (iRects&EFClipExtendedViewRect && iTextLayout)
			iTextLayout->HighlightExtensions().ExtendRect(clipRect);
		if (iRects&EFClipExtendedTextArea)
			{
			clipRect=iDrawTextLayoutContext->TextArea();
			if (iTextLayout)
				iTextLayout->HighlightExtensions().ExtendRect(clipRect);
			}
		}
	if (iRects&EFClipInvalid)
		clipRect.Intersection(iInvalidRect);
	return clipRect;
	}


void RScreenDisplay::ResetClippingRect()
	{
	TRect clipRect=ClippingRect();
	iGc->SetClippingRect(clipRect);
	}

void RScreenDisplay::BlastBitmap(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aRect)
	//
	// Blast bitmap to screen
	//
	{
	iGc->BitBlt(aPoint,aBitmap,aRect);
	}

void RScreenDisplay::DrawPictureFrame(TFrameOverlay* aPictureFrame,const TRect& aLineRect)
	//
	// Draw a frame overlay, using XOR brush. Therefore if frame visible, the frame will disappear,
	// and vice versa.
	//
	{
	TRect clipRect=ClippingRect();
	clipRect.Intersection(aLineRect);
	iGc->SetClippingRect(clipRect);
	iGc->SetPenSize(TSize(1,1));
	aPictureFrame->XorDraw(*iGc);
	ResetClippingRect();
    }

void RScreenDisplay::Invalidate(TRect aRect)
	//
    //Invalidate the view rect
    //
    {
	if (iWin)
		iWin->Invalidate(aRect);
    }

void RScreenDisplay::SetTextCursor(TPoint aPos, const TTextCursor &aCursor)
	//
	//Display a cursor at position aPos
	//
	{
	if (iWin && iGroupWin)
		{
		aPos.iX+=iDrawTextLayoutContext->iTextStartX;
		iGroupWin->SetTextCursor(*iWin,aPos,aCursor,iDrawTextLayoutContext->TextArea());
		}
	}

void RScreenDisplay::RemoveTextCursor()
	//
	//Remove cursor from window
	//
	{

	if (iGroupWin)
		iGroupWin->CancelTextCursor();
	}

void RScreenDisplay::ClearRect(const TRect& aRect)
	//
	//Clear a rectangle on the screen
	//
	{
	if (iTextLayout)
		{
		TPoint top_left = iDrawTextLayoutContext->TopLeftText();
		iTextLayout->DrawBackground(*iGc,top_left,aRect,iBackground);
		ResetClippingRect();
		}
	else
		{
		iGc->SetBrushColor(iBackground);
		iGc->Clear(aRect);
		}
	}

void RScreenDisplay::InvertRect(TRect aRect,const TRgb aInvertColor)
	{
	aRect.Move(iDrawTextLayoutContext->iTextStartX,0);
	iGc->SetDrawMode(CGraphicsContext::EDrawModeXOR);
	iGc->SetBrushColor(aInvertColor);
	iGc->Clear(aRect);
	iGc->SetBrushColor(iBackground);
	iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	}

void RScreenDisplay::Scroll(TRect aRect,const TPoint& aBy,TBool aScrollBackground)
	{
	if (iWin)
		{
		if (!aScrollBackground || (iTextLayout && iTextLayout->CustomDraw()))
			{
			aRect.Move(aBy);
			iWin->Invalidate(aRect);
			}
		else
   			iWin->Scroll(aBy,aRect);
		}
	return;
	}

void RScreenDisplay::Flush()
	{
	if (iSession)
		iSession->Flush();
	}

TBool RScreenDisplay::IsLineCursor() const
	{
	return iDrawTextLayoutContext->IsGutterMargin();
	}

TRect RScreenDisplay::LineCursorMargin() const
	{
	return iDrawTextLayoutContext->GutterMargin();
	}

TPoint RScreenDisplay::TopLeftTextArea() const
	{
	return iDrawTextLayoutContext->TopLeftTextArea();
	}

void RScreenDisplay::SetLayout(CTextLayout* aLayout)
	{
	iTextLayout = aLayout;
	iTextLayout->SetWindow(iWin);
	}

CTextLayout* RScreenDisplay::Layout()
	{
	return iTextLayout;
	}

TBool RScreenDisplay::UseWindowGc() const
	{
	return iDrawTextLayoutContext->UseWindowGc();
	}
