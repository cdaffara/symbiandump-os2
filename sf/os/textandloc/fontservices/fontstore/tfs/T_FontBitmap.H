/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_FONTBITMAP_H__
#define __T_FONTBITMAP_H__

#include <fntstore.h>
#include "TGraphicsHarness.h"


class CTFontBitmapStep : public CTGraphicsStep
	{
public:
	CTFontBitmapStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTFontBitmapStep,"TFontBitmap");

#endif

