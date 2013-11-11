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


#ifndef __TESTENVIRONMENT_H
#define __TESTENVIRONMENT_H

#include <badesca.h>
#include <txtglobl.h>
#include <frmtview.h>
#include <txtfmlyr.h>
#include <txtrich.h>
#include <frmtlay.h>

/**
@file
@internalComponent 
*/
class CCoeEnv;

/**
The test environment
This manages the screen device and fonts
*/
class CBandTestEnvironment : public CBase
	{
public:
	IMPORT_C static CBandTestEnvironment* NewLC(CCoeEnv& aCoeEnv, const TDesC& aFont, TInt aFontSize);
	IMPORT_C ~CBandTestEnvironment();
	IMPORT_C CParaFormatLayer* ParaFormat();
	IMPORT_C CCharFormatLayer* CharFormat();
	IMPORT_C CFbsScreenDevice& Device();
private:
	CBandTestEnvironment(CCoeEnv& aCoeEnv);
	void ConstructL(const TDesC& aFont, TInt aFontSize);
	
	CCoeEnv& iCoeEnv;
	CParaFormatLayer* iParaFormatLayer;
	CCharFormatLayer* iCharFormatLayer; 	
	CFbsBitGc* iGc;
	CFbsScreenDevice* iDev;
	CFbsFont* iStandardFont;
	TTmCharFormat iStandardCharFormat;
	};
	
#endif


