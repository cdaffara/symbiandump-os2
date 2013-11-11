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
#include <badesca.h>
#include <txtglobl.h>
#include <frmtview.h>
#include <txtfmlyr.h>
#include <txtrich.h>
#include <frmtlay.h>
#include <coemain.h>
#include <coeaui.h>

#include "testenvironment.h"


EXPORT_C CBandTestEnvironment* CBandTestEnvironment::NewLC(CCoeEnv& aCoeEnv, const TDesC& aFont, TInt aFontSize)
	{
	CBandTestEnvironment* self = new(ELeave) CBandTestEnvironment(aCoeEnv);
	CleanupStack::PushL(self);
	self->ConstructL(aFont, aFontSize);
	return self;
	}

EXPORT_C CBandTestEnvironment::~CBandTestEnvironment()
	{
	if (iDev)
		{
		iDev->ReleaseFont(iStandardFont);
		}
	delete iDev;
	delete iGc;
	delete iParaFormatLayer;
	delete iCharFormatLayer;
	}
	

EXPORT_C CParaFormatLayer* CBandTestEnvironment::ParaFormat()
	{
	return iParaFormatLayer;
	}
	
EXPORT_C CCharFormatLayer* CBandTestEnvironment::CharFormat()
	{
	return iCharFormatLayer;
	}
	
EXPORT_C CFbsScreenDevice& CBandTestEnvironment::Device()
	{
	return *iDev;
	}	
	
CBandTestEnvironment::CBandTestEnvironment(CCoeEnv& aCoeEnv)
	:iCoeEnv(aCoeEnv)
	{
	}
	
void CBandTestEnvironment::ConstructL(const TDesC& aFont, TInt aFontSize)
	{
//screen device
	TRAPD(error, iDev = CFbsScreenDevice::NewL(0,EColor16M));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(0,EColor16MA));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(0,EColor16MU));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(0,EColor64K));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(0,EColor4K));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(0,EColor256));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(0,EColor16));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(0,EGray256));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(0,EGray16));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(0,EGray4));
	if (error == KErrNotSupported)
		iDev = CFbsScreenDevice::NewL(0,EGray2);
		
	iDev->ChangeScreenDevice(NULL);
	iDev->SetAutoUpdate(ETrue);
	iDev->CreateContext(iGc);
//standard font
	TFontSpec fsMedium(aFont,aFontSize);
	error = iDev->GetNearestFontInPixels((CFont*&)iStandardFont,fsMedium);
	if (error)
		User::Panic((_L("Could not create %S"), aFont), error);
	iStandardCharFormat.iFontSpec = iStandardFont->FontSpecInTwips();
	if (iStandardCharFormat.iFontSpec.Name() != aFont)
		User::Panic((_L("Could not create %S"), aFont), KErrNotFound);

//rich text formatting layers
	iParaFormatLayer=CParaFormatLayer::NewL(); 
	TCharFormat charFormat;
	iStandardCharFormat.GetTCharFormat(charFormat);
	TCharFormatMask charFormatMask;
	charFormatMask.SetAll();
	iCharFormatLayer=CCharFormatLayer::NewL(charFormat, charFormatMask); 
	}

