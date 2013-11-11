/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <eikstart.h>
#include <f32file.h>

#include "WPMAIN.H"
#include "WPPANIC.H"

// Panic the process with Word as the category.
//
GLDEF_C void Panic(TWordPanic aPanic)
	{
    User::Panic(_L("Word"),aPanic);
    }

//
// EXPORTed functions
//
////////////////////////////////////////////////////////////////////////////////////////////
//

	static CApaApplication* NewApplication()
		{
		return new CWordApplication;
		}

	TInt E32Main()
		{
		return EikStart::RunApplication(&NewApplication);
		}


