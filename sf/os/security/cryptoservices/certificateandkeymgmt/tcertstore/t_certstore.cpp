/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include "t_certstoretests.h"
#include "t_certstoreactions.h"
#include <e32std.h>
#include <ecom/ecom.h>

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAP_IGNORE(DoTests());
	
	delete cleanup; // destroy clean-up stack
	REComSession::FinalClose();

	__UHEAP_MARKEND;
	return 0; // and return
    }
