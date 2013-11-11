/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef __MAINVIEW_CONTAINER_H__
#define __MAINVIEW_CONTAINER_H__

#include "HindiDemo.hrh"


#include <eikappui.h>
#include <coecntrl.h>
#include <akncontext.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <akntitle.h>
#include <avkon.rsg>
#include <barsread.h>
#include <eikspane.h>

const TInt KLengthOfNaviTempString = 100;

// -----------------------------------------------------------------------
// CMainViewContainer
// -----------------------------------------------------------------------
class CMainViewContainer : public CCoeControl, public MCoeControlObserver {

public:
  // -----------------------------------------------------------------------
  // Creates a CMainViewContainer object
  // -----------------------------------------------------------------------
  static CMainViewContainer * NewL(const TRect& aRect);

  // -----------------------------------------------------------------------
  // Creates a CMainViewView object
  // -----------------------------------------------------------------------
  static CMainViewContainer * NewLC(const TRect& aRect);

  // -----------------------------------------------------------------------
  // Returns the number of controls contained in this compound control
  // -----------------------------------------------------------------------
  TInt CountComponentControls() const;

  // -----------------------------------------------------------------------
  // Returns the component at the specified index
  // @param aIndex specifies index of component
  // @return Pointer to component control
  // -----------------------------------------------------------------------
  CCoeControl * ComponentControl(TInt aIndex) const;

  // -----------------------------------------------------------------------
  // Draws this container
  // -----------------------------------------------------------------------
  	void Draw(const TRect& aRect) const;
  	void DrawHindiText(CWindowGc& aGc) const;
	void DisableShaping(const TDesC* aInput, TDes& aOutput ) const;
	void SetHindiFont( TFontSpec& aFontSpec );
	void ChangeSample( TInt aSample);
	TInt GetSample();
	void SetFontSize( TInt aFontSize );
	TInt GetFontSize();
	void SetUnderLine( enum TFontUnderline aUnderLine );
	enum TFontUnderline GetUnderLine( );
	void ChangeFont();
	void ChangePreviousFont();
	void DisplayAvailableFonts();

  	// -----------------------------------------------------------------------
  	// Destroys container
  	// -----------------------------------------------------------------------
  	~CMainViewContainer();

	//
	// Handler for events sent by a control to this Observer
	//
	void HandleControlEventL(CCoeControl * aControl, TCoeEvent aEventType);

  // --------------------------------------------------------------------------------------
  // Overridden function used to pass key events to child controls owned by this container
  // --------------------------------------------------------------------------------------
  TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

  // -----------------------------------------------------------------------
  // Routine that dispatches Command events to individual handlers
  // -----------------------------------------------------------------------
  bool DispatchViewCommandEvents(TInt aCommand);

private:
  // -----------------------------------------------------------------------
  // Constructor
  // -----------------------------------------------------------------------
  CMainViewContainer();

  // -----------------------------------------------------------------------
  // Performs second phase construction of this Container
  // -----------------------------------------------------------------------
  void ConstructL(const TRect & aRect);

  // -----------------------------------------------------------------------
  // Routine that initializes components owned by this Container
  // -----------------------------------------------------------------------
  void InitComponentsL();

  // -----------------------------------------------------------------------
  // Routine that cleans up components owned by this container
  // -----------------------------------------------------------------------
  void CleanupComponents();

  // -----------------------------------------------------------------------
  // Routine that attempts to dispatch Control Events
  // -----------------------------------------------------------------------
  void DispatchControlEvents(CCoeControl * aControl, TCoeEvent aEventType);

  // -----------------------------------------------------------------------
  // Place holder routine to handle key events
  // -----------------------------------------------------------------------
  bool HandleKeyEvents(const TKeyEvent& aKeyEvent, TEventCode aType);

  // -----------------------------------------------------------------------
  // Helper method to manage control in focus
  // -----------------------------------------------------------------------
  void ChangeFocus(CCoeControl * aNewControl);

private:
  TRgb iBackgroundColor;
  RPointerArray<CCoeControl> iCtrlArray;
  CCoeControl* iFocusedControl;

private:
  // -----------------------------------------------------------------------------
  	CAknContextPane * iContextPane;
  	CAknTitlePane * iTitlePane;
  	CAknNavigationControlContainer * iNaviPane;
  	CAknNavigationDecorator * iNaviDecorator;

    TInt 		iSample;
    TFontSpec 	iFontSpec;
    TInt 		iFontSize;
    TInt		iSelectedFont;
	enum TFontUnderline iUnderLine;

};

#endif // __MAINVIEW_CONTAINER_H__
