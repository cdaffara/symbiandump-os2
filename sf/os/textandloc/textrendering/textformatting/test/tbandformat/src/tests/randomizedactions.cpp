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
#include "bandtestfileoutput.h"
#include <txtrich.h>
#include "randomizedactions.h"
#include "randomizedtests.h"
#include "testpicture.h"
#include <frmtlay.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

enum TActionType
	{
	ERandomizedMove = 0,
	ERandomizedDelete = 1,
	ERandomizedInsert = 2,
	ERandomizedReformat = 3,
	ERandomizedInsertImage = 4,
	ERandomizedSetVisibility = 5

	};
const TInt KTestPictureMaxDimensions = 2000; //width & height in twips
	
//base class ctor
CRandomizedAction::CRandomizedAction(CBandMaintainer& aTextViews, 
	CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput)
	:iTextViews(aTextViews), iRichText(aRichText), iValidator(aValidator), iOutput(aOutput)
	{
	iStop = EFalse;
	}
	
CRandomizedAction* CRandomizedAction::NewLC(	CBandMaintainer& aTextViews, CRichText& aRichText, 
			CBandValidator& aValidator, CFbsScreenDevice& aDevice, RFile& aOutput, RReadStream& aStream)
	{
	TInt32 actionType = aStream.ReadInt32L();
	switch (actionType)
		{
		case 0:
			{
			return CRandomizedMove::NewLC(aTextViews, aRichText, aValidator, aOutput, aStream);
			}	
		case 1:
			{
			return CRandomizedDeleteText::NewLC(aTextViews, aRichText, aValidator, aOutput, aStream);
			}
		case 2:
			{
			return CRandomizedInsertText::NewLC(aTextViews, aRichText, aValidator, aOutput, aStream);
			}
		case 3:
			{
			return CRandomizedReformatText::NewLC(aTextViews, aRichText, aValidator, aDevice, aOutput, aStream);
			}
		case 4:
			{
			return CRandomizedInsertImage::NewLC(aTextViews, aRichText, aValidator, aOutput, aStream);
			}
		case 5:
			{
			return CRandomizedSetVisibility::NewLC(aTextViews, aRichText, aValidator, aOutput, aStream);
			}

		default:
			{
			__ASSERT_ALWAYS(EFalse, User::Invariant());
			}
		}
	return NULL;
	}
	
CRandomizedAction* CRandomizedAction::NewLC(	CBandMaintainer& aTextViews, CRichText& aRichText, 
			CBandValidator& aValidator, CFbsScreenDevice& aDevice, RFile& aOutput)
	{
	TInt32 actionType = BoundedRandom(0,5);
	switch (actionType)
		{
		case 0:
			{
			return CRandomizedMove::NewLC(aTextViews, aRichText, aValidator, aOutput);
			}
		case 1:
			{
			return CRandomizedDeleteText::NewLC(aTextViews, aRichText, aValidator, aOutput);
			}
		case 2:
			{
			return CRandomizedInsertText::NewLC(aTextViews, aRichText, aValidator, aOutput);
			}
		case 3:
			{
			return CRandomizedReformatText::NewLC(aTextViews, aRichText, aValidator, aDevice, aOutput);
			}
		case 4:
			{
			return CRandomizedInsertImage::NewLC(aTextViews, aRichText, aValidator, aOutput);
			}
		case 5:
			{
			return CRandomizedSetVisibility::NewLC(aTextViews, aRichText, aValidator, aOutput);
			}

		default:
			{
			__ASSERT_ALWAYS(EFalse, User::Invariant());
			}
		}
	return NULL;
	}
	
//move	
CRandomizedMove* CRandomizedMove::NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, 
	CBandValidator& aValidator, RFile& aOutput)
	{
	CRandomizedMove* self = new(ELeave) CRandomizedMove(aTextViews, aRichText, aValidator, aOutput);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CRandomizedMove* CRandomizedMove::NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, 
	CBandValidator& aValidator, RFile& aOutput, RReadStream& aStream)
	{
	CRandomizedMove* self = new(ELeave) CRandomizedMove(aTextViews, aRichText, aValidator, aOutput);
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

CRandomizedMove::CRandomizedMove(CBandMaintainer& aTextViews, CRichText& aRichText, 
	CBandValidator& aValidator, RFile& aOutput)
	:CRandomizedAction(aTextViews, aRichText, aValidator, aOutput)
	{
	}
	
void CRandomizedMove::ConstructL()
	{
/*possible moves:
TCursorPosition::TMovementType
0		EFNoMovement,
1		EFLeft,
2		EFRight,
3		EFLineUp,
4		EFLineDown,
5		EFPageUp,
6		EFPageDown,
7		EFLineBeg,
8		EFLineEnd
*/	
	TInt moveIndex = BoundedRandom(0, 8);
	iMovement = static_cast<TCursorPosition::TMovementType>(moveIndex);	
	}
	
void CRandomizedMove::ConstructL(RReadStream& aStream)
	{
	TInt32 movement = aStream.ReadInt32L();
	iMovement = static_cast<TCursorPosition::TMovementType>(movement);
	}
	
void CRandomizedMove::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(ERandomizedMove);
	aStream.WriteInt32L(iMovement);
	}

TBool CRandomizedMove::ExecuteL()
	{
	_LIT(KDes,"Move cursor: %d.");
	TBuf<128> actionDesc;
	actionDesc.Format(KDes,iMovement);
	iOutput.Write(actionDesc.Collapse());

	iTextViews.ScrollL(iMovement);
	return iValidator.ValidateL(actionDesc);	
	}
	
//delete text
CRandomizedDeleteText* CRandomizedDeleteText::NewLC(CBandMaintainer& aTextViews, 
	CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput)
	{
	CRandomizedDeleteText* self = new(ELeave) CRandomizedDeleteText(aTextViews, aRichText, aValidator, aOutput);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CRandomizedDeleteText* CRandomizedDeleteText::NewLC(CBandMaintainer& aTextViews, 
	CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput, RReadStream& aStream)
	{
	CRandomizedDeleteText* self = new(ELeave) CRandomizedDeleteText(aTextViews, aRichText, aValidator, aOutput);
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

void CRandomizedDeleteText::ConstructL()
	{	
	TInt startChar = iTextViews.TmLayoutUnderTest().StartChar();
	TInt endChar = iTextViews.TmLayoutUnderTest().EndChar();
	
	if(endChar > startChar)
		{
		endChar--;
		}
		
	iPos = BoundedRandom(startChar, endChar);
	if(iPos == endChar)
		{
		iLength = 0;
		}
	else
		{
		iLength = BoundedRandom(1, endChar - iPos); 
		}
	}
	
void CRandomizedDeleteText::ConstructL(RReadStream& aStream)
	{
	iPos = aStream.ReadInt32L();
	iLength = aStream.ReadInt32L();
	}	

void CRandomizedDeleteText::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(ERandomizedDelete);
	aStream.WriteInt32L(iPos);
	aStream.WriteInt32L(iLength);
	}
	
CRandomizedDeleteText::CRandomizedDeleteText(CBandMaintainer& aTextViews, CRichText& aRichText, 
	CBandValidator& aValidator, RFile& aOutput)
	:CRandomizedAction(aTextViews, aRichText, aValidator, aOutput)
	{
	}

TBool CRandomizedDeleteText::ExecuteL()
	{
//two cases which we can't execute, so just return ETrue and continue
	if(iLength == 0)
		{
		return ETrue;
		}		
	
	if ((iPos + iLength) > iTextViews.TmLayoutUnderTest().EndChar())
		{
		return ETrue;
		}	

	TCursorSelection cursor(iPos,iPos);
 	CTextView& testView = iTextViews.ViewUnderTest();
 	testView.SetSelectionL(cursor);
 	CTextView& referenceView = iTextViews.ViewReference();
 	referenceView.SetSelectionL(cursor);	
	
	iRichText.DeleteL(iPos, iLength); 
	iTextViews.HandleDeleteL(iPos, iLength);
	
	_LIT(KDes,"Delete Text at pos %d and length %d.");
	TBuf<128> actionDesc;
	actionDesc.Format(KDes,iPos,iLength); 
	iOutput.Write(actionDesc.Collapse());

	TBool success = iValidator.ValidateL(actionDesc);
	if (!success)
		{
		iStop = ETrue;
		TInt failLineNumInPara = iValidator.FailLineNumberInPara();
		if (failLineNumInPara==0)
			{
			return ETrue;
			}
		}
	return success;
	}

//insert text
CRandomizedInsertText* CRandomizedInsertText::NewLC(CBandMaintainer& aTextViews, 
	CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput)
	{
	CRandomizedInsertText* self = new(ELeave) CRandomizedInsertText(aTextViews, aRichText, aValidator, aOutput);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CRandomizedInsertText* CRandomizedInsertText::NewLC(CBandMaintainer& aTextViews, 
	CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput, RReadStream& aStream)
	{
	CRandomizedInsertText* self = new(ELeave) CRandomizedInsertText(aTextViews, aRichText, aValidator, aOutput);
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

CRandomizedInsertText::~CRandomizedInsertText()
	{
	iText.Close();
	}

void CRandomizedInsertText::ConstructL(RReadStream& aStream)
	{
	iPos = aStream.ReadInt32L();
	iText.CreateL(KMaxStringToInsert);
	aStream >> iText; 
	}
	
void CRandomizedInsertText::ConstructL()
	{
	TInt startChar = iTextViews.TmLayoutUnderTest().StartChar();
	TInt endChar = iTextViews.TmLayoutUnderTest().EndChar();
	if(endChar > startChar)
		{
		endChar--;
		}
		
	iPos = BoundedRandom(startChar, endChar);	
	
	RandomStringLC(1, KMaxStringToInsert, iText);
	CleanupStack::Pop();
	}	

void CRandomizedInsertText::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(ERandomizedInsert);
	aStream.WriteInt32L(iPos);
	aStream << iText;
	}
		
CRandomizedInsertText::CRandomizedInsertText(CBandMaintainer& aTextViews, CRichText& aRichText, 
	CBandValidator& aValidator, RFile& aOutput)
	:CRandomizedAction(aTextViews, aRichText, aValidator, aOutput)
	{
	}

TBool CRandomizedInsertText::ExecuteL()
	{
	if(iPos > iTextViews.TmLayoutUnderTest().EndChar())
		{
		return ETrue;
		}

 	TCursorSelection cursor(iPos, iPos);
 	CTextView& testView = iTextViews.ViewUnderTest();
 	testView.SetSelectionL(cursor);
 	CTextView& referenceView = iTextViews.ViewReference();
 	referenceView.SetSelectionL(cursor);
 
	iRichText.InsertL(iPos, iText);
	TCursorSelection insertCursor(iPos, iText.Length() + iPos);
 	iTextViews.HandleInsertL(insertCursor);	
	
	_LIT(KDes,"Insert Text at pos %d of length %d.");
	TBuf<128> actionDesc;
	actionDesc.Format(KDes, iPos, iText.Length());
	iOutput.Write(actionDesc.Collapse());

	TBool success = iValidator.ValidateL(actionDesc);
	if (!success)
		{
		iStop = ETrue;
		TInt failLineNumInPara = iValidator.FailLineNumberInPara();
		if (failLineNumInPara==0)
			{
			return ETrue;
			}
		}
	return success;
	}

//reformat text


CRandomizedReformatText* CRandomizedReformatText::NewLC(CBandMaintainer& aTextViews, 
	CRichText& aRichText, CBandValidator& aValidator, CFbsScreenDevice& aDevice, RFile& aOutput)
	{
	CRandomizedReformatText* self = new(ELeave) CRandomizedReformatText(aTextViews, aRichText, aValidator, aDevice, aOutput);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CRandomizedReformatText* CRandomizedReformatText::NewLC(CBandMaintainer& aTextViews, 
	CRichText& aRichText, CBandValidator& aValidator, CFbsScreenDevice& aDevice, RFile& aOutput, RReadStream& aStream)
	{
	CRandomizedReformatText* self = new(ELeave) CRandomizedReformatText(aTextViews, aRichText, aValidator, aDevice, aOutput);
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

void CRandomizedReformatText::ConstructL(RReadStream& aStream)
	{
	iPos = aStream.ReadInt32L();
	iLength = aStream.ReadInt32L();
	TInt32 actionType = aStream.ReadInt32L();
	iReformatAction = static_cast<CRandomizedReformatText::TReformatAction> (actionType);
	aStream >> iCharFormat.iFontSpec;
	TTmCharFormatMask tmCharFormatMask;
	tmCharFormatMask.iFlags = aStream.ReadUint32L();
	tmCharFormatMask.GetTCharFormatMask(iCharFormatMask);
	}
	
void CRandomizedReformatText::ConstructL()
	{
	TInt startChar = iTextViews.TmLayoutUnderTest().StartChar();
	TInt endChar = iTextViews.TmLayoutUnderTest().EndChar();
	
	if(endChar > startChar)
		{
		endChar--;
		}
		
	iPos = BoundedRandom(startChar, endChar);
	if(iPos == endChar)
		{
		iLength = 0;
		}
	else
		{
		iLength = BoundedRandom(1, endChar - iPos); 
		}
	
	TInt action = BoundedRandom(0, 2);
	
	iReformatAction = static_cast<CRandomizedReformatText::TReformatAction> (action);
	
	switch(iReformatAction)
		{
		case 0:
			{
			GetNewFontAttributes(iCharFormat, iCharFormatMask);
			break;
			}	
		case 1:
			{
			GetNewFont(iCharFormat, iCharFormatMask);
			break;
			}	
		case 2:
			{
			GetNewFontSize(iCharFormat, iCharFormatMask);
			break;
			}
		default:
			{
			__ASSERT_ALWAYS(EFalse, User::Invariant());
			}
		}
	
	}	

void CRandomizedReformatText::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(ERandomizedReformat);
	aStream.WriteInt32L(iPos);
	aStream.WriteInt32L(iLength);
	aStream.WriteInt32L(iReformatAction);
	aStream << iCharFormat.iFontSpec;
	TTmCharFormatMask tmCharFormatMask(iCharFormatMask);
	aStream.WriteUint32L(tmCharFormatMask.iFlags);
	}

CRandomizedReformatText::CRandomizedReformatText(CBandMaintainer& aTextViews, 
	CRichText& aRichText, CBandValidator& aValidator, 
	CFbsScreenDevice& aDevice, RFile& aOutput)
	:CRandomizedAction(aTextViews, aRichText, aValidator, aOutput), iDevice(aDevice)
	{
	}

TBool CRandomizedReformatText::ExecuteL()
	{
//two cases which we can't execute, so just return ETrue and continue
	if(iLength == 0)
		{
		return ETrue;
		}		
	
	if ((iPos + iLength) > iTextViews.TmLayoutUnderTest().EndChar())
		{
		return ETrue;
		}
	
	_LIT(KDes,"Reformat Text Action %d at pos %d and length %d.");
	TBuf<128> actionDesc;
	actionDesc.Format(KDes,iReformatAction,iPos,iLength);
	iOutput.Write(actionDesc.Collapse());	
	
 	TCursorSelection cursor(iPos, iPos+iLength);
 	CTextView& testView = iTextViews.ViewUnderTest();
 	testView.SetSelectionL(cursor);	
  	CTextView& referenceView = iTextViews.ViewReference();
 	referenceView.SetSelectionL(cursor);

	iRichText.ApplyCharFormatL(iCharFormat, iCharFormatMask, iPos, iLength);	
	iTextViews.HandleFormatChangeL(cursor); 

	TBool success = iValidator.ValidateL(actionDesc);
	if (!success)
		{
		iStop = ETrue;
		TInt failLineNumInPara = iValidator.FailLineNumberInPara();
		if (failLineNumInPara==0)
			{
			return ETrue;
			}
		}
	return success;
	}
	
void CRandomizedReformatText::GetNewFontAttributes(TCharFormat& aCharFormat, TCharFormatMask& aCharFormatMask)
	{
	TBool boldAttribute =  BoundedRandom(0,1);	
	
	TTmCharFormatLayer boldFormat;
	boldFormat.iMask.iFlags = 0;
	boldFormat.iMask.Set(TTmCharFormatMask::EBold);
	boldFormat.iFormat.iFontSpec.SetBold(boldAttribute);
		
	boldFormat.iFormat.GetTCharFormat(aCharFormat);
	TTmCharFormatMask tmCharFormatMask = boldFormat.iMask;
	tmCharFormatMask.GetTCharFormatMask(aCharFormatMask);
	}

void CRandomizedReformatText::GetNewFont(TCharFormat& aCharFormat, TCharFormatMask& aCharFormatMask)
	{
	CFbsFont* newFont;
	TTmCharFormat charFormat;	
	
	TBool coinToss = BoundedRandom(0,1);	
	
	if(coinToss)
		{
		TFontSpec fsNewCourier(_L("New Courier"),17);
		//we should calculate this 17 from the current height, but I can only find that in 
		//twips not pixels, and the conversion doesn't seem any too successful
		TInt error = iDevice.GetNearestFontInPixels((CFont*&)newFont,fsNewCourier);
		if (error)
			User::Panic(_L("Could not create 17-pixel New Courier"),error);	
		}
	else
		{
		TFontSpec fsClearlyu(_L("ClearlyU"),17);
		//we should calculate this 17 from the current height, but I can only find that in 
		//twips not pixels, and the conversion doesn't seem any too successful
		TInt error = iDevice.GetNearestFontInPixels((CFont*&)newFont,fsClearlyu);
		if (error)
			User::Panic(_L("Could not create 17-pixel ClearlyU"),error);	
		}
	
	charFormat.iFontSpec = newFont->FontSpecInTwips();
	charFormat.GetTCharFormat(aCharFormat);
	aCharFormatMask.SetAll();
	iDevice.ReleaseFont(newFont);
	}

void CRandomizedReformatText::GetNewFontSize(TCharFormat& aCharFormat, TCharFormatMask& aCharFormatMask)
	{	
	TInt coinToss = BoundedRandom(0,1);	
	TInt newSize = (coinToss > 0)? 24:8;
	
	CFbsFont* newFont;
	TTmCharFormat newCharFormat;
	TFontSpec fs(_L("Arial"),newSize);
	TInt error = iDevice.GetNearestFontInPixels((CFont*&)newFont,fs);
	if (error)
		User::Panic(_L("Could not create %d Arial"),newSize);	
	newCharFormat.iFontSpec = newFont->FontSpecInTwips();
	newCharFormat.GetTCharFormat(aCharFormat);
	aCharFormatMask.SetAll();
	iDevice.ReleaseFont(newFont);
	}

/* 
 * Randomized insert picture
 */
CRandomizedInsertImage* CRandomizedInsertImage::NewLC(
		CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput)
	{
	CRandomizedInsertImage* self = new(ELeave) CRandomizedInsertImage(aTextViews, aRichText, aValidator, aOutput);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
		
CRandomizedInsertImage* CRandomizedInsertImage::NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, 
				CBandValidator& aValidator, RFile& aOutput, RReadStream& aStream)
	{
	CRandomizedInsertImage* self = new(ELeave) CRandomizedInsertImage(aTextViews, aRichText, aValidator, aOutput);
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}
	
CRandomizedInsertImage::CRandomizedInsertImage(
		CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput)
	:CRandomizedAction(aTextViews, aRichText, aValidator, aOutput)
	{
	}

void CRandomizedInsertImage::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(ERandomizedInsertImage);
	aStream << iPictureSize;
	}
	
TBool CRandomizedInsertImage::ExecuteL()
	{
	CTestPicture* pic = CTestPicture::NewLC(iPictureSize);
	TPictureHeader header;
   	header.iPictureType = KUidTestPictureType;
   	header.iPicture=pic;
 		
   	TCursorSelection selection = iTextViews.ViewUnderTest().Selection();
   	TInt pos = selection.iCursorPos;
    iRichText.InsertL(pos, header); 
//CRichText::InsertL takes ownership of the picture via TPictureHeader 	
 	CleanupStack::Pop(pic); 
 	
	iTextViews.HandleGlobalChangeL(); 
 	iTextViews.ViewUnderTest().SetDocPosL(pos+1, EFalse);
 	iTextViews.ViewReference().SetDocPosL(pos+1, EFalse);	

	_LIT(KDes,"Insert picture at %d.");
	TBuf<128> actionDesc;
	actionDesc.Format(KDes,pos);
	iOutput.Write(actionDesc.Collapse());		

	TBool success = iValidator.ValidateL(actionDesc);
	if (!success)
		{
		iStop = ETrue;
		TInt failLineNumInPara = iValidator.FailLineNumberInPara();
		if (failLineNumInPara==0)
			{
			return ETrue;
			}
		}
	return success;
	}

CRandomizedInsertImage::~CRandomizedInsertImage()
	{
	}
	
void CRandomizedInsertImage::ConstructL()
	{
	iPictureSize.iHeight = BoundedRandom(1, KTestPictureMaxDimensions);
	iPictureSize.iWidth = BoundedRandom(1, KTestPictureMaxDimensions);
	}
	
void CRandomizedInsertImage::ConstructL(RReadStream& aStream)
	{
	aStream >> iPictureSize;
	}

/* 
 * Randomized set visibility
 */
CRandomizedSetVisibility* CRandomizedSetVisibility::NewLC(
		CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput)
	{
	CRandomizedSetVisibility* self = new(ELeave) CRandomizedSetVisibility(aTextViews, aRichText, aValidator, aOutput);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
		
CRandomizedSetVisibility* CRandomizedSetVisibility::NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, 
				CBandValidator& aValidator, RFile& aOutput, RReadStream& aStream)
	{
	CRandomizedSetVisibility* self = new(ELeave) CRandomizedSetVisibility(aTextViews, aRichText, aValidator, aOutput);
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}
	
CRandomizedSetVisibility::CRandomizedSetVisibility(
		CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput)
	:CRandomizedAction(aTextViews, aRichText, aValidator, aOutput)
	{
	}

void CRandomizedSetVisibility::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(ERandomizedSetVisibility);
	aStream.WriteInt32L(iVisibility);
	}
	
TBool CRandomizedSetVisibility::ExecuteL()
	{	
	iTextViews.ViewReference().MakeVisible(iVisibility);
	iTextViews.ViewUnderTest().MakeVisible(iVisibility);

	_LIT(KDes,"Set visibility to %d.");
	TBuf<128> actionDesc;
	actionDesc.Format(KDes,iVisibility);
	iOutput.Write(actionDesc.Collapse());		
 	
	return iValidator.ValidateL(actionDesc);
	}

CRandomizedSetVisibility::~CRandomizedSetVisibility()
	{
	}
	
void CRandomizedSetVisibility::ConstructL()
	{
	iVisibility = BoundedRandom(0,1);	
	}
	
void CRandomizedSetVisibility::ConstructL(RReadStream& aStream)
	{
	iVisibility = aStream.ReadInt32L();
	}

