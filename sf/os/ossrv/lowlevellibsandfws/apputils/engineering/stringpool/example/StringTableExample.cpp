// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <e32base.h>
#include <e32svr.h>
#include <stringpool.h>
#include "examplestringtable.h"

void StartL()
	{
	TBuf<100> buf;
	RStringPool pool;
	pool.OpenL(ExampleStringTable::Table);
	buf.Copy(pool.StringF(ExampleStringTable::EApple, ExampleStringTable::Table).DesC());
	RDebug::Print(buf);
	}

// main loop
//
GLDEF_C TInt E32Main()
    {
	// Install exception handler
	CTrapCleanup* theCleanup = CTrapCleanup::New(); 
	TRAPD(err,StartL());
	delete theCleanup;
    return(KErrNone);
	}
