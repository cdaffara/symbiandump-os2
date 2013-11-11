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


#ifndef __TMTEXTDRAWEXT_H__
#define __TMTEXTDRAWEXT_H__

#include <tagma.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

/**
A default implementation of MTmTextDrawExt interface, which has to be used only 
if there is no other MTmTextDrawExt implementation.
@internalComponent
*/
NONSHARABLE_CLASS(TTmTextDrawExt) : public MTmTextDrawExt
	{
public:
	virtual void DrawLine(CGraphicsContext& aGc, const TPoint& aPt1, const TPoint& aPt2) const;
	virtual void DrawText(CGraphicsContext& aGc, const TDesC& aText, const TPoint& aPt) const;
	virtual void DrawRect(CGraphicsContext& aGc, const TRect& aRc) const;
	};

#endif//__TMTEXTDRAWEXT_H__
