/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TFREETYPEDEFECTS_H__
#define __TFREETYPEDEFECTS_H__


#include <test/testexecutestepbase.h>
#include <w32std.h>
#include <test/ttmsgraphicsstep.h>

// The test step to verify the support of supplementary characters
class CTSurrogateRasterizedStep : public CTTMSGraphicsStep
	{
public:
	CTSurrogateRasterizedStep ();
protected:	
	TVerdict doTestStepL();
	};

_LIT(KTSurrogateRasterizedStep,"TSurrogateRasterized");

#endif
