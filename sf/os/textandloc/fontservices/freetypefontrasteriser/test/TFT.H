/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TFT_H__
#define __TFT_H__

#include <test/testexecutestepbase.h>
#include <w32std.h>
#include <test/ttmsgraphicsstep.h>

class CTFtStep : public CTTMSGraphicsStep
	{
public:
	CTFtStep();
protected:	
	TVerdict doTestStepL();
	};

_LIT(KTFtStep,"TFt");

#endif
