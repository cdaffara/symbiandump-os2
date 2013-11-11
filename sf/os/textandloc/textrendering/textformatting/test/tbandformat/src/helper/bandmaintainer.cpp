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


/**
 @file
 @internalComponent 
*/
#include <e32std.h>
#include <e32base.h>
#include <badesca.h>
#include <txtglobl.h>
#include <frmtview.h>
#include <txtfmlyr.h>
#include <txtrich.h>
#include <frmtlay.h>

#include <coemain.h>
#include <coeaui.h>

#include <e32test.h>
#include "bandmaintainer.h"
#include "testenvironment.h"
#include "richcontrol.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

EXPORT_C CBandMaintainer* CBandMaintainer::NewLC(CRichText& aRichText, TRect aDrawRect, TBool aDisplayTextBox)
	{
	CBandMaintainer* self = new(ELeave) CBandMaintainer();
	CleanupStack::PushL(self);
	self->ConstructL(aRichText, aDrawRect, aDisplayTextBox);
	return self;
	}

CBandMaintainer::CBandMaintainer()
	{
	}
	
void CBandMaintainer::ConstructL(CRichText& aRichText, TRect aDrawRect, TBool aDisplayTextBox)
	{
	iTestControl = CRichControl::NewL(aRichText, aDrawRect, aDisplayTextBox);
	iViewUnderTest = iTestControl->View();
	CTextLayout* testLayout = const_cast<CTextLayout*>(iViewUnderTest->Layout());
	// set the test layout to use band formatting
	testLayout->SetAmountToFormat(CTextLayout::EFFormatBand);
	
	iReferenceControl = CRichControl::NewL(aRichText, aDrawRect, EFalse);//don't activate the reference
	iViewReference = iReferenceControl->View();
	}

EXPORT_C CBandMaintainer::~CBandMaintainer()
	{
	delete iTestControl;
	delete iReferenceControl;
	}

EXPORT_C void CBandMaintainer::ScrollDisplayPixelsL(TInt& aPixels)
	{
	iViewUnderTest->ScrollDisplayPixelsL(aPixels);
	TTmDocPos pos(TmLayoutUnderTest().StartChar(), ETrue);
	iViewUnderTest->SetDocPosL(pos);
	}

EXPORT_C TBool CBandMaintainer::ScrollL(TCursorPosition::TMovementType& aMovement)
	{
	TCursorPosition::TMovementType referenceMovement = aMovement;
	TPoint test = iViewUnderTest->MoveCursorL(aMovement, EFalse);
	TPoint reference = iViewReference->MoveCursorL(referenceMovement, EFalse);
	
	TTmDocPos testCursorPos;
	TTmDocPos referenceCursorPos;
	
	iViewUnderTest->GetCursorPos(testCursorPos);
	iViewReference->GetCursorPos(referenceCursorPos);
	iTestControl->DrawNow();
	
	if(	(testCursorPos.iPos==referenceCursorPos.iPos) &&
		(test.iY == reference.iY) &&
		(aMovement == referenceMovement))
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

EXPORT_C void CBandMaintainer::InitialiseViewL()
	{
	iViewUnderTest->FormatTextL();
	iViewReference->FormatTextL();
	iViewUnderTest->SetDocPosL(0);
	iViewReference->SetDocPosL(0);
	iTestControl->DrawNow();
	}

EXPORT_C void CBandMaintainer::HandleGlobalChangeL()
	{
	iViewUnderTest->HandleGlobalChangeL();
	iViewReference->HandleGlobalChangeL();
	}
	
EXPORT_C void CBandMaintainer::HandleFormatChangeL(TCursorSelection aSelection)
	{
	iViewUnderTest->HandleRangeFormatChangeL(aSelection,EFalse);
	iViewReference->FormatTextL();	
	iTestControl->DrawNow();
	}

EXPORT_C void CBandMaintainer::HandleInsertL(TCursorSelection aSelection)
	{
	iViewUnderTest->HandleInsertDeleteL(aSelection, 0, EFalse);	
	iViewReference->FormatTextL();
	iTestControl->DrawNow();
	}

EXPORT_C void CBandMaintainer::HandleDeleteL(TInt aStartPos, TInt aDeletedChars)
	{
	TCursorSelection cursor(aStartPos,aStartPos);
	iViewUnderTest->HandleInsertDeleteL(cursor, aDeletedChars, EFalse);
	iViewReference->FormatTextL();	
	iTestControl->DrawNow();
	}
	
EXPORT_C CTextView& CBandMaintainer::ViewUnderTest() const
	{
	return *iViewUnderTest;
	}
	
EXPORT_C CTextView& CBandMaintainer::ViewReference() const
	{
	return *iViewReference;	
	}
	
EXPORT_C const CTextLayout& CBandMaintainer::LayoutUnderTest() const
	{
	return *(iViewUnderTest->Layout());
	}
	
EXPORT_C const CTextLayout& CBandMaintainer::LayoutReference() const
	{
	return *(iViewReference->Layout());	
	}
	
EXPORT_C const CTmTextLayout& CBandMaintainer::TmLayoutUnderTest() const
	{
	return iViewUnderTest->Layout()->TagmaTextLayout();
	}
	
EXPORT_C const CTmTextLayout& CBandMaintainer::TmLayoutReference() const
	{
	return iViewReference->Layout()->TagmaTextLayout();	
	}

