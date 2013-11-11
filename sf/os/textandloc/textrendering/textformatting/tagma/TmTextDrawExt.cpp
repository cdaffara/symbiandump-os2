/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "TmTextDrawExt.h"

/**
A default implementation of MTmTextDrawExt::DrawLine().
*/
void TTmTextDrawExt::DrawLine(CGraphicsContext& aGc, const TPoint& aPt1, const TPoint& aPt2) const
	{
	aGc.DrawLine(aPt1, aPt2);
	}

/**
A default implementation of MTmTextDrawExt::DrawText().
*/
void TTmTextDrawExt::DrawText(CGraphicsContext& aGc, const TDesC& aText, const TPoint& aPt) const
	{
	aGc.DrawText(aText, aPt);
	}

/**
A default implementation of MTmTextDrawExt::DrawRect().
*/
void TTmTextDrawExt::DrawRect(CGraphicsContext& aGc, const TRect& aRc) const
	{
	aGc.DrawRect(aRc);
	}

