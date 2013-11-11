// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : testabort.cpp
// 
//

// INCLUDE FILES
#include <e32def.h>
#include <e32std.h>
#include <stdio.h>
#include <e32base.h>

#include <unistd.h>
#include <stdlib.h>

void abortTest()
	{
	abort();
	}
	
TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error,abortTest());
	//Destroy cleanup stack
	delete cleanup;
	__UHEAP_MARKEND;
	return error;
	}

