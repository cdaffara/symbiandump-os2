/*
* Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32test.h>
#include <fbs.h>
#include <graphics/fbsdefs.h>

#include "bdfharn.h"

CTrapCleanup* TrapCleanup;


TInt E32Main()
    {
	FbsStartup();
	TrapCleanup = CTrapCleanup::New();

	TInt error = 0;

	__UHEAP_MARK;
	
	CBDFHarness* bdfHarness;

	TRAP(error,
	bdfHarness = CBDFHarness::NewL(_L("MAKEBDF"),_L("TTF2BDF v2 - Make bitmap BDF font from scaleable font"));
	bdfHarness->MakeBDFFontL();	
	delete bdfHarness;
	__UHEAP_MARKEND;
	);

	delete TrapCleanup;
	return error;
    }
