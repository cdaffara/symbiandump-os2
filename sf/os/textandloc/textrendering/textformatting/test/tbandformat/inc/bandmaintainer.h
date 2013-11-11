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


#ifndef __BANDMAINTAINER_H
#define __BANDMAINTAINER_H

#include <badesca.h>
#include <txtglobl.h>
#include <frmtview.h>
#include <txtfmlyr.h>
#include <txtrich.h>
#include <frmtlay.h>

/**
@file
@internalComponent 
*/	
class CRichText;
class CRichControl;
class CTextView;

/**
This class is responsible for maintaining two CTextView/CTextLayout objects.
It also provides a simplified version of the CTextView interface for handling
inserts, deletes and format changes.
The two CTextView/CTextLayout objects that are maintained are:
1) iViewUnderTest is the view/layout we are testing
2) iViewReference is the view/layout we testing against
The test view iViewUnderTest has band formatting enabled and is compared to the 
reference view iViewReference which does not have band formatting enabled. The reference
view formats all of its document again from scratch each time a change is made so that it
does not invoke any of the reformmating code being tested by the test view.
*/
class CBandMaintainer : public CBase
	{
public:
	IMPORT_C static CBandMaintainer* NewLC(CRichText& aRichText, TRect aDrawRect, TBool aDisplayTextBox);
	IMPORT_C ~CBandMaintainer();
	/**
	Scroll the test display (negative = downwards)
	@param aPixels The number of pixels to scroll; may be a positive or negative value. On return, contains 
	the number of pixels actually scrolled. Positive values move the text down, negative move it up. 
	*/
	IMPORT_C void ScrollDisplayPixelsL(TInt& aPixels);
	/**
	Update the view after a change to the document
	*/
	IMPORT_C void InitialiseViewL();
	/**
	Scroll both views in the direction and amount specified by the TCursorPosition::TMovementType parameter
	@param aMovement The direction and manner in which to scroll. On return set to the actual cursor movement.
	@return ETrue if the resulting scroll gave the same results for both test and reference views, EFalse otherwise.
	*/
	IMPORT_C TBool ScrollL(TCursorPosition::TMovementType& aMovement);
	/**
	Causes the view objects to be reformatted after a change to the text or paragraph formatting
	The reference view formats the whole document again using CTextView::FormatTextL, so the test view 
	can be tested against a view which does not invoke the reformatting logic at all
	@param aSelection The start and new length of the changed block.
	*/
	IMPORT_C void HandleFormatChangeL(TCursorSelection aSelection);
	/**
	Reformats and redraws both views after a global change has been made to the layout.
	*/ 
	IMPORT_C void HandleGlobalChangeL();
	/**
	Reformats and redraws both views after a deleting a block of text.
	The reference view formats the whole document again using CTextView::FormatTextL, so the test view 
	can be tested against a view which does not invoke the reformatting logic at all.
	@param aStartPos The start position of the delete to be performed
	@param aDeletedChars The amount of characters to be deleted
	*/
	IMPORT_C void HandleDeleteL(TInt aStartPos, TInt aDeletedChars);
	/**
	Reformats and redraws both views after a inserting a block of text.
	The reference view formats the whole document again using CTextView::FormatTextL, so the test view 
	can be tested against a view which does not invoke the reformatting logic at all.
	@param aSelection The start and new length of the changed block.
	*/
	IMPORT_C void HandleInsertL(TCursorSelection aSelection);
//some accessors
	IMPORT_C CTextView& ViewUnderTest() const;
	IMPORT_C CTextView& ViewReference() const;
	IMPORT_C const CTextLayout& LayoutUnderTest() const;
	IMPORT_C const CTextLayout& LayoutReference() const;
	IMPORT_C const CTmTextLayout& TmLayoutUnderTest() const;
	IMPORT_C const CTmTextLayout& TmLayoutReference() const;
private:
	CBandMaintainer();
	void ConstructL(CRichText& aRichText, TRect aDrawRect, TBool aDisplayTextBox);

	CRichControl* iReferenceControl;
	CRichControl* iTestControl;
	//does not own these 2
	CTextView* iViewUnderTest; // view/layout we are testing
	CTextView* iViewReference; // view/layout we testing against
	};
	
#endif
