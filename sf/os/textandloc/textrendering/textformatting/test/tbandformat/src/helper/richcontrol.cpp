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
#include "richcontrol.h"
#include <coemain.h>
#include <coeaui.h>
#include <txtrich.h>
#include <frmtlay.h>

CRichControl* CRichControl::NewL(CRichText& aText, TRect& aRect, TBool aActivate)
	{
	CRichControl* self = new(ELeave) CRichControl(aText);
	CleanupStack::PushL(self);
	self->ConstructL(aRect, aActivate);
	CleanupStack::Pop(self);
	return self;
	}

CRichControl::CRichControl(CRichText& aText)
	:iRichText(aText)
	{
	}
	
CRichControl::~CRichControl()
	{
	delete iTextView; 
	delete iLayout; 
	}

void CRichControl::ConstructL(TRect& aRect, TBool aActivate)
	{
	CreateWindowL();
	SetRect(aRect);

	CWindowGc& gc=SystemGc(); 
	CBitmapDevice *device=(CBitmapDevice*) (gc.Device()); 
	
	TRect textRect = Rect();
	textRect.Shrink(1,1);	
	iLayout=CTextLayout::NewL(&iRichText,textRect.Width());
	iTextView=CTextView::NewL(iLayout, textRect, device, device, &Window(), 0, &iCoeEnv->WsSession());
	if (aActivate)
		{
		ActivateL();
		}
	}

void CRichControl::Draw(const TRect& aRect) const
	{
	CWindowGc& gc=SystemGc();
	gc.SetClippingRect(aRect);
	gc.DrawRect(Rect());
	TInt ignore;
	TRAP(ignore, iTextView->DrawL(aRect));
	}

CTextLayout* CRichControl::Layout()
	{
	return iLayout;
	}
	
CTextView* CRichControl::View()
	{
	return iTextView;
	}

TRect CRichControl::ViewRect()
	{
	return Rect();
	}

