/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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
////////////////////////////////////////////////////////////////////////
//
// Source file for the implementation of the 
// application view class - TForm18030_AppView
//
////////////////////////////////////////////////////////////////////////

#include "TForm18030.h"
#include <barsread.h>

//
//             Constructor for the view.
//
TForm18030_AppView::TForm18030_AppView()
	{
	}


//             Static NewL() function to start the standard two
//             phase construction.
//
TForm18030_AppView* TForm18030_AppView::NewL(const TRect& aRect)
	{
	TForm18030_AppView* self = new(ELeave) TForm18030_AppView();
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop();
	return self;
	}


//
//             Destructor for the view.
//
TForm18030_AppView::~TForm18030_AppView()
	{
	delete iTForm18030Text;
	delete iEditor;
	}


//             Second phase construction.
//
void TForm18030_AppView::ConstructL(const TRect& aRect)
    {
			   // Fetch the text from the resource file.
	iTForm18030Text = iEikonEnv->AllocReadResourceL(R_TFORM18030_TEXT_HELLO);
	           // Control is a window owning control
	CreateWindowL();
	
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, R_OUTPUT_VIEW_RTEXTED );
    iEditor = new ( ELeave ) CEikRichTextEditor();

    iEditor->SetContainerWindowL( *this );
    iEditor->ConstructFromResourceL( reader );
    CleanupStack::PopAndDestroy();  // Resource reader
    iEditor->SetFocus( ETrue );

    iEditor->SetExtent( TPoint( 0, 0 ),
                              TSize( aRect.Width(), aRect.Height() ) );

    iEditor->CreateScrollBarFrameL();
    iEditor->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOn, CEikScrollBarFrame::EOn);
    iEditor->ScrollBarFrame()->DrawScrollBarsNow();

    // Extent of the control. This is
	           // the whole rectangle available to application.
	           // The rectangle is passed to us from the application UI.
	SetRect(aRect);

			   // At this stage, the control is ready to draw so
	           // we tell the UI framework by activating it.
	ActivateL();
	}

void TForm18030_AppView::ChangeEditorText(const TDesC &fontName, const TDesC &text)
	{
		TCharFormat format(fontName, 280);
		TCharFormatMask mask;
		//mask.SetAttrib(EAttFontTypeface);
		//mask.SetAttrib(EAttFontHeight);
		mask.SetAll();
		HBufC * origText = iEditor->GetTextInHBufL();
		if (origText) iEditor->SetSelectionL(0,origText->Length());
		iEditor->ApplyCharFormatL(format, mask);
		iEditor->SetTextL(&text);
		iEditor->SetSelectionL(0,0);
		User::Free(origText);
		iEditor->DrawNow();
	}

TKeyResponse TForm18030_AppView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	if (iEditor)
		return iEditor->OfferKeyEventL (aKeyEvent, aType);
	else
		return EKeyWasNotConsumed;
}

CCoeControl* TForm18030_AppView::ComponentControl(TInt aIndex) const
{
	switch (aIndex)
	{
	case 0:
		return iEditor;

	default:
		return NULL;
	}
}

void TForm18030_AppView::SizeChanged()
{
}

TInt TForm18030_AppView::CountComponentControls() const
{
	return 1; // return number of controls inside this container
}

