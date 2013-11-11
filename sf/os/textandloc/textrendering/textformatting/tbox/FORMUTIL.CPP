/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "FORMUTIL.H"

// Set the pen colour using a logical colour if necessary.
void FormUtil::SetPenColor(const MFormParam* aParam,CGraphicsContext* aGc,TLogicalRgb aColor)
	{
	LogicalToActualColor(aParam,aColor);
	aGc->SetPenColor(aColor);
	}

// Set the brush colour using a logical colour if necessary.
void FormUtil::SetBrushColor(const MFormParam* aParam,CGraphicsContext* aGc,TLogicalRgb aColor)
	{
	LogicalToActualColor(aParam,aColor);
	aGc->SetBrushColor(aColor);
	}
