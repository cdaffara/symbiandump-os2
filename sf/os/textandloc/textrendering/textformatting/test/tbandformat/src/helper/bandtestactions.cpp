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
#include "bandmaintainer.h"
#include "bandvalidator.h"
#include <txtrich.h>
#include "bandtestactions.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

//base class ctor
TBandTestAction::TBandTestAction(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator)
	:iTextViews(aTextViews), iRichText(aRichText), iValidator(aValidator)
	{
	}	
	
//delete text
EXPORT_C TDeleteText::TDeleteText(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator)
	:TBandTestAction(aTextViews, aRichText, aValidator)
	{
	}

EXPORT_C TBandTestAction::TTestResult TDeleteText::ExecuteL(TInt aDocPos, TInt aEditLength)
	{
	_LIT(KDes,"Delete Text at pos %d and length %d.");
	TBuf<128> actionDesc;
	actionDesc.Format(KDes,aDocPos,aEditLength);
	if ((aDocPos + aEditLength) > iTextViews.TmLayoutUnderTest().EndChar())
		{
		return ENotExecuted;
		}
	iRichText.DeleteL(aDocPos, aEditLength); 
	iTextViews.HandleDeleteL(aDocPos, aEditLength); 
	return iValidator.ValidateL(actionDesc)? EPassed : EFailed;
	}

//insert text
		
EXPORT_C TInsertText::TInsertText(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, const TDesC& aTextToInsert)
	:TBandTestAction(aTextViews, aRichText, aValidator), iTextToInsert(aTextToInsert)
	{
	}

EXPORT_C TBandTestAction::TTestResult TInsertText::ExecuteL(TInt aDocPos, TInt /*aEditLength*/)
	{
	_LIT(KDes,"Insert Text at pos %d of length %d.");
	TBuf<128> actionDesc;
	actionDesc.Format(KDes, aDocPos, iTextToInsert.Length());
	if(aDocPos > iTextViews.TmLayoutUnderTest().EndChar())
		{
		return ENotExecuted;
		}
	iRichText.InsertL(aDocPos, iTextToInsert);
	TCursorSelection cursor(aDocPos, iTextToInsert.Length() + aDocPos);
	iTextViews.HandleInsertL(cursor);
	return iValidator.ValidateL(actionDesc)? EPassed : EFailed;
	}

//reformat text
EXPORT_C TReformatText::TReformatText(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, 
	TReformatAction aReformatAction, CFbsScreenDevice& aDevice)
	:TBandTestAction(aTextViews, aRichText, aValidator), iReformatAction(aReformatAction), iDevice(aDevice)
	{
	}

EXPORT_C TBandTestAction::TTestResult TReformatText::ExecuteL(TInt aDocPos, TInt aEditLength)
	{
	_LIT(KDes,"Reformat Text Action %d at pos %d and length %d.");
	TBuf<128> actionDesc;
	actionDesc.Format(KDes,iReformatAction,aDocPos,aEditLength);
	if ((aDocPos + aEditLength) > iTextViews.TmLayoutUnderTest().EndChar())
		{
		return ENotExecuted;
		}	
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;

	switch (iReformatAction)
		{
		case EMakeBold:
			{
			TTmCharFormatLayer boldFormat;
			boldFormat.iMask.iFlags = 0;
			boldFormat.iMask.Set(TTmCharFormatMask::EBold);
			boldFormat.iFormat.iFontSpec.SetBold(ETrue);
			
			boldFormat.iFormat.GetTCharFormat(charFormat);
			boldFormat.iMask.GetTCharFormatMask(charFormatMask);
			break;
			}
		case EIncreaseFontSize:
			{
			CFbsFont* bigFont;
			TTmCharFormat charFormatBig;	

			TFontSpec fsBig(_L("Arial"),24);
			TInt error = iDevice.GetNearestFontInPixels((CFont*&)bigFont,fsBig);
			if (error)
				User::Panic(_L("Could not create 24-pixel Arial"),error);	
			charFormatBig.iFontSpec = bigFont->FontSpecInTwips();
			charFormatBig.GetTCharFormat(charFormat);
			charFormatMask.SetAll();
			iDevice.ReleaseFont(bigFont);
			break;
			}
		case EDecreaseFontSize:
			{
			CFbsFont* smallFont;
			TTmCharFormat charFormatSmall;	

			TFontSpec fsSmall(_L("Arial"),8);
			TInt error = iDevice.GetNearestFontInPixels((CFont*&)smallFont,fsSmall);
			if (error)
				User::Panic(_L("Could not create 8-pixel Arial"),error);	
			charFormatSmall.iFontSpec = smallFont->FontSpecInTwips();
			charFormatSmall.GetTCharFormat(charFormat);
			charFormatMask.SetAll();
			iDevice.ReleaseFont(smallFont);
			break;
			}
		case EChangeFont:
			{
			CFbsFont* newCourierFont;
			TTmCharFormat charFormatNewCourier;	
			TFontSpec fsNewCourier(_L("New Courier"),17);
			//we should calculate this 17 from the current height, but I can only find that in 
			//twips not pixels, and the conversion doesn't seem any too successful
			TInt error = iDevice.GetNearestFontInPixels((CFont*&)newCourierFont,fsNewCourier);
			if (error)
				User::Panic(_L("Could not create 17-pixel New Courier"),error);	
			charFormatNewCourier.iFontSpec = newCourierFont->FontSpecInTwips();
			charFormatNewCourier.GetTCharFormat(charFormat);
			charFormatMask.SetAll();
			iDevice.ReleaseFont(newCourierFont);
			break;
			}
		default:
			return ENotExecuted;
		}
	iRichText.ApplyCharFormatL(charFormat, charFormatMask, aDocPos, aEditLength);
	TCursorSelection cursor(aDocPos, aDocPos+aEditLength);	
	iTextViews.HandleFormatChangeL(cursor); 

	return iValidator.ValidateL(actionDesc)? EPassed : EFailed;
	}


