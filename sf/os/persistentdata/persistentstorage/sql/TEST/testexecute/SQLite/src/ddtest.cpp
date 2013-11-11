// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ddtest.h"
#include "common.h"

CSQLDDT::~CSQLDDT()
    {
    }

CSQLDDT::CSQLDDT()
    {
    SetTestStepName(KSQLDDT);
    }

// Look at 'arg' and call whichever function is required.
// But we're data driven! We can't do this.
void CSQLDDT::ResolveTestFunctionL(const TDesC &, const TInt, const TPtrC & )
    {
    INFO_PRINTF1(HTML_RED);
    SetTestStepResult(EFail);
    INFO_PRINTF1(_L("Cannot call 'ResolveTestFunction' in data-driven test"));
    INFO_PRINTF1(HTML_COLOUR_OFF);
	User::Leave(999);
    }


