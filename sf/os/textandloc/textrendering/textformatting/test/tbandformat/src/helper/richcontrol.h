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
*
*/


#ifndef __RICHCONTROL_H
#define __RICHCONTROL_H

#include <coemain.h>
#include <coecntrl.h>
#include <s32file.h>
#include <txtglobl.h>
#include <frmtview.h>
#include <txtfmlyr.h>

/**
@file
@internalComponent 
*/
class CRichText;
class CParaFormatLayer;
class CCharFormatLayer;
class CTextLayout;
class CTextView;
class CStyleList;
class CParagraphStyle;

/**
Subclass of CCoeControl which displays the test text view in a window
*/
class CRichControl : public CCoeControl
	{
public:
	/**
	To activate the window aActivate must be set to ETrue
	*/
	static CRichControl* NewL(CRichText& aText, TRect& aRect, TBool aActivate);
	~CRichControl(); 	
	CTextLayout* Layout();
	CTextView* View();
	TRect ViewRect();
private:
	CRichControl(CRichText& aText);
	void ConstructL(TRect& aRect, TBool aActivate);
	void Draw(const TRect& aRect) const;
	TBool UpdateModelL();
	void CreateFontsL();
	void InitializeL();
	void DeleteSectionL(TInt aSectionNumber);
	
	CRichText& iRichText; 
	CTextLayout* iLayout;
	CTextView* iTextView; 	
	};

/*
class CRichControl : public CCoeControl
	{
public:
	static CRichControl* NewL(CRichText& aText, CCoeControl& aControl, TPoint aPos, TSize aSize);
	~CRichControl(); 	
	CTextLayout* Layout();
	CTextView* View();
	TRect ViewRect();
private:
	CRichControl(CRichText& aText);
	void ConstructL(CCoeControl& aControl, TPoint aPos, TSize aSize);
	void Draw(const TRect& aRect) const;
	TBool UpdateModelL();
	void CreateFontsL();
	void InitializeL();
	void DeleteSectionL(TInt aSectionNumber);
	
	CRichText& iRichText; 
	CTextLayout* iLayout;
	CTextView* iTextView; 	
	};
	
class CRichControlContainer : public CCoeControl
	{
public:
	static CRichControlContainer* NewL(TRect& aRect, TInt aBorder, CRichText& aText);
	~CRichControlContainer();
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	CRichControl& LeftSide();
	CRichControl& RightSide();
private:
	CRichControlContainer();
	void ConstructL(TRect& aRect, TInt aBorder, CRichText& aText);
	void Draw(const TRect& aRect) const;
	CRichControl* iLeftSide;
	CRichControl* iRightSide;
	};
*/
#endif
