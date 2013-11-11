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
 @test
 @internalComponent Internal Symbian test code
*/

#ifndef __T_CORRUPTFONTS_H__
#define __T_CORRUPTFONTS_H__

#include <fntstore.h>
#include "TGraphicsHarness.h"

_LIT(KTCorruptFontRejectionStep, "TCorruptFonts");

class CTCorruptFontRejectionStep : public CTGraphicsStep
	{
public:
	CTCorruptFontRejectionStep();
protected:	
	// from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};


#endif // __T_CORRUPTFONTS_H__
