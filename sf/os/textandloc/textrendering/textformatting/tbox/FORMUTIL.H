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
* Header for private FORM stuff.
*
*/


#ifndef __FORMUTIL__
#define __FORMUTIL__

#include <frmtlay.h>

/**
@internalComponent
*/
class FormUtil

	{
	public:
	static void LogicalToActualColor(const MFormParam* aParam,TLogicalRgb& aColor)
			{
			if (aParam)
				{
				TUint index = aColor.SystemColorIndex();
				if (index)
					{
					aColor &= 0xFFFFFF;
					aColor = aParam->SystemColor(index,aColor);
					}
				}
			}
	static void SetPenColor(const MFormParam* aParam,CGraphicsContext* aGc,TLogicalRgb aColor);
	static void SetBrushColor(const MFormParam* aParam,CGraphicsContext* aGc,TLogicalRgb aColor);
	};

#endif //__FORMUTIL__
