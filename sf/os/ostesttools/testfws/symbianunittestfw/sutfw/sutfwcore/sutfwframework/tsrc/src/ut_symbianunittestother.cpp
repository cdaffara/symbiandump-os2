/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ut_symbianunittestother.h"
#include "symbianunittestfailure.h"
#include <symbianunittestmacros.h>

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestOther* UT_CSymbianUnitTestOther::NewL()
    {
    UT_CSymbianUnitTestOther* self = UT_CSymbianUnitTestOther::NewLC(); 
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestOther* UT_CSymbianUnitTestOther::NewLC()
    {
    UT_CSymbianUnitTestOther* self = 
        new( ELeave )UT_CSymbianUnitTestOther();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestOther::UT_CSymbianUnitTestOther()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestOther::ConstructL()
    {
    BASE_CONSTRUCT
    
    DEFINE_TEST_CLASS( UT_CSymbianUnitTestOther )
    
    ADD_SUT( UT_GettersL )
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestOther::~UT_CSymbianUnitTestOther()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestOther::UT_GettersL()
    {    
    }
