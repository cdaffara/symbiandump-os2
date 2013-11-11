/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Series 60 version of Hindi Demo application
* Implementation of CMainViewContainer
*
*/


#include <aknutils.h>
#include <eikgted.h>
#include <HindiDemo.rsg>
#include <uikon.hrh>
#include "MainViewcontainer.h"

// for font dialog
#include <eikenv.h>
#include <aknnotewrappers.h>

// hindi sample text 
// sample 1
_LIT16(KNile1,"\x0915\x094D\x0937\x0924\x094D\x0930\x093F\x092F\x0020\x0909\x0926\x094D\x0926\x0947\x0936\x094D\x0020\x0915\x094D\x0937\x093F\x092A\x094D\x0930");
_LIT16(KNile2,"\x092d\x0942\x0930\x094d\x092d\x0941\x0935\x0903\x0020\x0938\x094d\x0935\x0903\x0020\x0924\x0924\x094d\x0938\x0935\x093f\x0924\x0941\x0930\x094d\x0935\x0930\x0947\x0923\x094d\x092f\x0902");
_LIT16(KNile3,"\x092d\x0930\x094d\x0917\x094b\x0020\x0926\x0947\x0935\x0938\x094d\x092f\x0020\x0927\x0940\x092e\x0939\x093f\x0020\x0927\x093f\x092f\x094b\x0020\x092f\x094b\x0928\x0903\x0020\x092a\x094d\x0930\x091a\x094b\x0926\x092f\x093e\x0924\x094d");

// sample 2
_LIT16(KNile4,"\x092f\x0924\x094d\x0930\x0020\x092f\x094b\x0917\x0947\x0936\x094d\x0935\x0930\x0020\x0915\x0943\x0937\x094d\x0923\x094b\x0020\x092f\x0924\x094d\x0930\x0020\x092a\x093e\x0930\x094d\x0925\x094b\x0020\x0927\x0930\x094d\x0928\x0941\x0927\x0930\x0903");
_LIT16(KNile5,"\x0924\x0924\x094d\x0930\x0020\x0936\x094d\x0930\x0940\x0930\x094d\x0935\x093f\x091c\x092f\x094b\x0020\x092d\x0942\x0924\x093f\x0930\x094d\x0927\x0941\x0935\x093e\x0020\x0928\x0940\x0924\x093f\x0930\x094d\x092e\x0924\x093f\x0930\x094d\x092e\x092e");
_LIT16(KNile6,"\x0915\x0941\x0924\x0938\x094d\x0924\x094d\x0935\x093e\x0020\x0915\x0936\x094d\x092e\x0932\x092e\x093f\x0926\x0902\x0020\x0935\x093f\x0936\x092e\x0947\x0020\x0938\x092e\x0941\x092a\x0938\x094d\x0925\x093f\x0924\x092e\x094d");

// sample 3
_LIT16(KNile7,"\x0905\x0928\x093e\x0930\x094d\x092f\x091c\x0941\x0937\x094d\x091f\x092e\x0938\x094d\x0935\x0930\x094d\x0917\x094d\x092f\x092e\x0915\x0940\x0930\x094d\x0924\x093f\x0915\x0930\x092e\x0930\x094d\x091c\x0941\x0928");
_LIT16(KNile8,"\x0915\x094d\x0932\x0948\x092c\x094d\x092f\x0902\x0020\x092e\x0020\x0938\x094d\x092e\x0020\x0917\x092e\x0903\x0020\x092a\x093e\x0930\x094d\x0925\x0928\x0948\x0924\x0924\x094d\x0924\x094d\x0935\x092f\x094d\x092f\x0941\x092a\x092a\x0926\x094d\x092f\x0924\x0947");
_LIT16(KNile9,"\x0915\x094d\x0937\x0941\x0926\x094d\x0930\x0902\x0020\x0939\x0943\x0926\x092f\x0926\x094c\x0930\x094d\x092c\x0932\x094d\x092f\x0902\x0020\x0924\x094d\x092f\x0915\x094d\x0924\x094d\x0935\x094b\x0924\x094d\x0924\x093f\x0937\x094d\x091f\x0020\x092a\x0930\x0902\x0924\x092a");

// test 4 -  assorted sample data
_LIT16(KTest1,"\x091c\x092e\x094d\x0918\x0940\x0902\x091a\x094d\x092f\x0940\x0020\x0920\x094d\x0920\x094d\x0920\x094d\x0920\x094d");
_LIT16(KTest2,"'\x091A\x092A\x0940' '\x0915\x092E\x0932' '\x092D\x0930\x0924'");
_LIT16(KTest3,"\x091A\x092A\x42 \x0915\x092A\x42 \x0915\x092A\x44 \x0930\x094D\x0930\x094D\x0930\x094D\x0930\x093e\x0903\x0020");

CMainViewContainer* CMainViewContainer::NewL(const TRect& aRect)
	{
	CMainViewContainer* self = CMainViewContainer::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

CMainViewContainer* CMainViewContainer::NewLC(const TRect& aRect)
	{
  	CMainViewContainer* self = new (ELeave) CMainViewContainer;
  	CleanupStack::PushL(self);
  	self->ConstructL(aRect);
  	return self;
	}

CMainViewContainer::CMainViewContainer()
	{
	}

CMainViewContainer::~CMainViewContainer()
	{
  	CleanupComponents();
  	iCtrlArray.Reset();
	}

// -----------------------------------------------------------------------
// Routine that creates and initializes designed components
//
// -----------------------------------------------------------------------
void CMainViewContainer::InitComponentsL()
	{
  	CEikStatusPane * sp = iEikonEnv->AppUiFactory()->StatusPane();
	  	{
	    iContextPane = ( CAknContextPane * ) sp->ControlL( TUid::Uid( EEikStatusPaneUidContext ) );
	    TResourceReader reader;
	    iCoeEnv->CreateResourceReaderLC( reader, R_MAINVIEW_IAKNCONTEXTPANE1 );
	    iContextPane->SetFromResourceL( reader );
	    CleanupStack::PopAndDestroy();
	  	}
	  	{
	    iTitlePane = ( CAknTitlePane * ) sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) );
	    TResourceReader reader;
	    iCoeEnv->CreateResourceReaderLC( reader, R_MAINVIEW_IAKNTITLEPANE1 );
	    iTitlePane->SetFromResourceL( reader );
	    CleanupStack::PopAndDestroy();
	  	}
  	iNaviPane = ( CAknNavigationControlContainer * ) sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) );
  	iEikonEnv->AppUiFactory()->StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );
  	iBackgroundColor = iEikonEnv->Color( EColorControlBackground );
	}

// -----------------------------------------------------------------------
// Routine that cleans up designed components
//
// -----------------------------------------------------------------------
void CMainViewContainer::CleanupComponents()
	{
  	delete iNaviDecorator;
	}

void CMainViewContainer::ConstructL(const TRect& aRect)
	{
	CreateWindowL();
	SetRect(aRect);
	InitComponentsL();
	ActivateL();

  	ChangeSample(1);
 	SetFontSize(200);
 	ChangeFont();
	}

void CMainViewContainer::Draw(const TRect& /* aRect */) const
	{
  	CWindowGc& gc = SystemGc();
	DrawHindiText( gc );
	}

void CMainViewContainer::ChangeSample( TInt aSample)
	{
	iSample = aSample;
	if (iSample > 4) 
		iSample =1; 
	if (iSample < 1) 
		iSample =4; 
	}

void CMainViewContainer::ChangeFont()
	{
	iSelectedFont++;
	int iNumTypefaces = iCoeEnv->ScreenDevice()->NumTypefaces();
	if (iSelectedFont >= iNumTypefaces || iSelectedFont < 0)
		iSelectedFont = 0;
	}

void CMainViewContainer::ChangePreviousFont()
	{
	iSelectedFont--;
	int iNumTypefaces = iCoeEnv->ScreenDevice()->NumTypefaces();
	if (iSelectedFont >= iNumTypefaces || iSelectedFont < 0)
		iSelectedFont = iNumTypefaces-1;
	}
	
void CMainViewContainer::SetHindiFont( TFontSpec& aFontSpec )
	{
	iFontSpec = aFontSpec;
	}

void CMainViewContainer::SetFontSize( TInt aFontSize )
	{
	iFontSize = aFontSize;
	}

void CMainViewContainer::SetUnderLine( enum TFontUnderline aUnderLine )
	{
	iUnderLine = aUnderLine;
	}
 
enum TFontUnderline CMainViewContainer::GetUnderLine()
	{
	return 	iUnderLine;
	}
	
TInt CMainViewContainer::GetFontSize()
	{
	return iFontSize;
	}

TInt CMainViewContainer::GetSample()
	{
	return iSample;
	}
	
void CMainViewContainer::DisplayAvailableFonts()
	{
	TBuf<500> listofFonts;
	
	TInt iNumTypefaces = iCoeEnv->ScreenDevice()->NumTypefaces();
	TTypefaceSupport myTypefaceSupport;
	for(TInt i = 0; i < iNumTypefaces-1; i++)
		{
		// get the details of the next available typeface
		iCoeEnv->ScreenDevice()->TypefaceSupport(myTypefaceSupport,i);
		listofFonts.Append( myTypefaceSupport.iTypeface.iName.Des());
		listofFonts.Append( _L("\n"));
		}

    CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
    informationNote->ExecuteLD(listofFonts);			
	}
		
const TInt UnicodeZeroWidthNonJoiner = 0x200C;

/* Display Hindi without shaping by inserting a Unicode Zero Width Non Joiner
character bewteen each character
aInput the Hindi text to be displayed
aOutput the same text with the ZWNJ characters added, This buffer must be big enough
*/
void CMainViewContainer::DisableShaping(const TDesC* aInput, TDes& aOutput ) const
	{
	aOutput.Zero();
	for (TInt i = 0; i < aInput->Length(); i++ )
		{
		aOutput.Append( aInput->Mid(i,1) );
		aOutput.Append( UnicodeZeroWidthNonJoiner );
		}
	}
	
void CMainViewContainer::DrawHindiText(CWindowGc& aGc) const
	{

	// start with test font
	CFont* testFont;
	_LIT(KTestFontName,"DejaVu Sans Condensed");
	TFontSpec testFontSpec(KTestFontName,100); 
	CGraphicsDevice* screenDevice=iCoeEnv->ScreenDevice();
	screenDevice->GetNearestFontInTwips(testFont,testFontSpec);

	// set the selected font
	CFont* IndicFont;
	TBuf<200> textFontName;

	TTypefaceSupport myTypefaceSupport;
	screenDevice->TypefaceSupport(myTypefaceSupport,iSelectedFont);
		
	textFontName.Copy(myTypefaceSupport.iTypeface.iName.Des());

	TFontSpec indicFontSpec(textFontName,iFontSize); 
	screenDevice->GetNearestFontInTwips(IndicFont,indicFontSpec);

	// set the required attributes
	aGc.UseFont(testFont);
	aGc.SetPenColor(KRgbBlack);
	aGc.SetBrushColor(KRgbWhite);
	aGc.CancelClippingRect();
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush); 

	// Set the text drawing position & draw
	TRect posRect(10,10, 600, 40);
	TBuf<100> textLine;
	TBuf<100> textLine2;
	TBuf<20> fontLine;
	textLine.Append(_L("Sample:"));
	
	switch( iSample )
		{
		case 1:
			textLine.Append(_L("1 "));
			break;
		case 2:
			textLine.Append(_L("2 "));
			break;
		case 3:
			textLine.Append(_L("3 "));
			break;
		case 4:
			textLine.Append(_L("test "));
			break;
		default:
			textLine.Append(_L("what? "));
			break;
	 	}
	fontLine.Format(_L("font %d:"), iSelectedFont );	
	textLine.Append(fontLine);
	textLine.Append(indicFontSpec.iTypeface.iName);
	aGc.DrawText(textLine,posRect, 14);

	TRect pos2rect(10,30, 600, 60);
	textLine2.Format(_L("Size:%dpt black is not shaped, blue is"),iFontSize/20);
	aGc.DrawText(textLine2, pos2rect, 14);
	
	// Discard and destroy the test font
	aGc.DiscardFont();
	screenDevice->ReleaseFont(testFont);
	
	// set the required attributes
	aGc.UseFont(IndicFont);
	aGc.SetPenColor(KRgbBlack);
	aGc.SetBrushColor(KRgbWhite);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush); 
	aGc.CancelClippingRect();
	aGc.SetUnderlineStyle(iUnderLine);

	// some rectangles to fit the text in
	const TInt baseLineOffset  = 32;
	const TInt gap = 40;
	const TInt start = 50;
	TRect textRect( 10,start,          800, start+(gap) );
	TRect textRect2(10,start+(gap),    800, start+(gap*2));
	TRect textRect3(10,start+(gap*2),  800, start+(gap*3));
	TRect textRect4(10,start+(gap*3),  800, start+(gap*4));
	TRect textRect5(10,start+(gap*4),  800, start+(gap*5));
	TRect textRect6(10,start+(gap*5),  800, start+(gap*6));

	TBuf<200> textWithoutShaping;

	// Draw some Hindi in a rectangle
	switch (iSample)
	{
	default:
	case 1:
	
		// First draw in Black with shaping disabled
		DisableShaping(&KNile1, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, baseLineOffset, CGraphicsContext::ELeft);

		// Now in Blue with shaping
		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KNile1, textRect2, baseLineOffset, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		aGc.SetPenColor(KRgbBlack);
		DisableShaping(&KNile2, textWithoutShaping );
		aGc.DrawText(textWithoutShaping, textRect3, baseLineOffset, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KNile2, textRect4, baseLineOffset, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KNile3, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, baseLineOffset, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KNile3, textRect6, baseLineOffset, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		
	case 2:		
		DisableShaping(&KNile4, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, baseLineOffset, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KNile4, textRect2, baseLineOffset, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KNile5, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect3, baseLineOffset, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KNile5, textRect4, baseLineOffset, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KNile6, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, baseLineOffset, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KNile6, textRect6, baseLineOffset, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
	
	case 3:	
		DisableShaping(&KNile7, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect, baseLineOffset, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KNile7, textRect2, baseLineOffset, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KNile8, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect3, baseLineOffset, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KNile8, textRect4, baseLineOffset, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine
		DisableShaping(&KNile9, textWithoutShaping );
		aGc.SetPenColor(KRgbBlack);
		aGc.DrawText(textWithoutShaping, textRect5, baseLineOffset, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KNile9, textRect6, 100, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		break;
		
	case 4:
		// test 
		DisableShaping(&KTest1, textWithoutShaping );
		aGc.DrawText(textWithoutShaping, textRect, baseLineOffset, CGraphicsContext::ELeft);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTest1, textRect2, baseLineOffset, CGraphicsContext::ELeft);
	
		//--------------------------------------------------------------------------
		// Without the Layout Engine

		aGc.SetPenColor(KRgbBlack);
		DisableShaping(&KTest2, textWithoutShaping );
		aGc.DrawText(textWithoutShaping, textRect3, baseLineOffset, CGraphicsContext::ELeft);
		// With the Layout Engine

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTest2, textRect4, baseLineOffset, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		// Without the Layout Engine

		aGc.SetPenColor(KRgbBlack);
		DisableShaping(&KTest3, textWithoutShaping );
		aGc.DrawText(textWithoutShaping, textRect5, baseLineOffset, CGraphicsContext::ELeft);
		// With the Layout Engine

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(KTest3, textRect6, baseLineOffset, CGraphicsContext::ELeft);
		//--------------------------------------------------------------------------
		
		break;
	}
		
	// Discard and destroy the font
	aGc.DiscardFont();
	screenDevice->ReleaseFont(IndicFont);

	}


// -----------------------------------------------------------------------
// Returns the number of controls in the control group.
// -----------------------------------------------------------------------
TInt CMainViewContainer::CountComponentControls() const
	{
  	return iCtrlArray.Count();
	}

// -----------------------------------------------------------------------
// Returns the component control identified by the given index
// -----------------------------------------------------------------------
CCoeControl* CMainViewContainer::ComponentControl(TInt aIndex) const
	{
  	return (CCoeControl*)iCtrlArray[aIndex];
	}

// -----------------------------------------------------------------------
// Method invoked by control framework to handle key events
// -----------------------------------------------------------------------
TKeyResponse CMainViewContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
  	if (HandleKeyEvents(aKeyEvent, aType))
    	return EKeyWasConsumed;
  	else if(iFocusedControl)
    	return iFocusedControl->OfferKeyEventL(aKeyEvent, aType);
  	else
    	return EKeyWasNotConsumed;
	}

// -----------------------------------------------------------------------
// Method invoked to handle an event from an observed control
// -----------------------------------------------------------------------
void CMainViewContainer::HandleControlEventL(CCoeControl * aControl, TCoeEvent aEventType)
	{
  	DispatchControlEvents(aControl, aEventType);
	}

// -----------------------------------------------------------------------
// This method attemps to dispatch commands to individual handlers
//
// -----------------------------------------------------------------------
bool CMainViewContainer::DispatchViewCommandEvents(TInt aCommand)
	{
	//InfoMessage
    _LIT(message,"Copyright (c) 2006 Symbian Software Ltd.\n All rights reserved.");
  	switch (aCommand) 
  		{
		/* sample 1*/
		case 1009:
			ChangeSample(1);
			break;
		/* sample 2*/
		case 1010:
			ChangeSample(2);
			break;
		/* sample 3*/
		case 1011:
			ChangeSample(3);
			break;
		/* sample 4*/
		case 1012:
			ChangeSample(4);
			break;
		/* Font */
		case 1017:
			ChangeFont();
			break;
		case 1018:
			ChangePreviousFont();
			break;
		case 1020:
			DisplayAvailableFonts();
			break;	
		case 1014:
			// reduce font size
 			SetFontSize(GetFontSize() -20);
 			break;
		case 1016:
			// increase font size
 			SetFontSize(GetFontSize() +20);
 			break;
		case 1015:
    		CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
            informationNote->ExecuteLD(message);			
            break;
    	default:
    		// this command has not been processed by this view so..
			return false;
  		}
	DrawDeferred();
	return true;
	}

// -----------------------------------------------------------------------
// Routine that attempts to dispatch Control Events
//
// -----------------------------------------------------------------------
void CMainViewContainer::DispatchControlEvents(CCoeControl * /* aControl */, TCoeEvent /* aEventType */)
	{
	}

// -----------------------------------------------------------------------
// Routine where you can handle key events
// -----------------------------------------------------------------------
bool CMainViewContainer::HandleKeyEvents(const TKeyEvent& aKeyEvent, TEventCode /* aType */)
	{
  	switch (aKeyEvent.iCode) 
  		{
  	  	case EKeyUpArrow:
 			// increase font size
 			SetFontSize(GetFontSize() +20);
 			break;
  	  	case EKeyDownArrow:
 			// increase font size
 			SetFontSize(GetFontSize() -20);
 			break;
  	  	case EKeyLeftArrow:
 			// next sample
			ChangeSample(GetSample()-1);
 			break;
  	  	case EKeyRightArrow:
 			// last sample
			ChangeSample(GetSample()+1);
 			break;
  	  	case '8':
  			if (GetUnderLine() == EUnderlineOff)
  				SetUnderLine ( EUnderlineOn );
			else
  				SetUnderLine ( EUnderlineOff );
			break;
		case '3':
			ChangeFont();
			break;
		case '1':
			ChangePreviousFont();
			break;
 	  	default:
	   		return false;
 	  	}
 	  	
	DrawDeferred();
 	return true;  	
	}

// -----------------------------------------------------------------------
// Helper routine to manage Control Focus
// -----------------------------------------------------------------------
void CMainViewContainer::ChangeFocus(CCoeControl * aNewControl)
	{
  	if(iFocusedControl)
    	iFocusedControl->SetFocus(EFalse);
  	iFocusedControl = aNewControl;
  	if(iFocusedControl)
    	iFocusedControl->SetFocus(ETrue);
	}


