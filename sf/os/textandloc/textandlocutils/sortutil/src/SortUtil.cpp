/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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






// INCLUDE FILES
#include <e32std.h>
#include "SortUtil.h"

#ifdef __WINS__     
#include <featmgr.h>    
#ifndef KFeatureIdJapanese    
#define KFeatureIdJapanese 1080
#endif
#endif

// LOCAL CONSTANTS AND MACROS
typedef MSortUtil* (*TSortUtilFactoryFunctionL)();
_LIT( KSortUtilImpl, "SortUtilImpl.dll" );
#ifdef __WINS__
_LIT( KSortUtilImplJapan, "SortUtilImplJapan.dll" );
_LIT( KSortUtilImplKorean, "SortUtilImplKorean.dll" );
#endif // __WINS__

// -----------------------------------------------------------------------------
// CSortUtil::NewL()
// -----------------------------------------------------------------------------
//
EXPORT_C CSortUtil* CSortUtil::NewL()
    {
    CSortUtil* self = new( ELeave ) CSortUtil;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CSortUtil::~CSortUtil()
// -----------------------------------------------------------------------------
//
CSortUtil::~CSortUtil()
    {
    delete iInterface;
    iLib.Close();
    }

// -----------------------------------------------------------------------------
// CSortUtil::CSortUtil()
// -----------------------------------------------------------------------------
//
CSortUtil::CSortUtil()
    {
    }

// -----------------------------------------------------------------------------
// CSortUtil::ConstructL()
// -----------------------------------------------------------------------------
//
void CSortUtil::ConstructL()
    {
#ifdef __WINS__     
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdJapanese ) )
        {
        // load polymorphic implementation DLL
        User::LeaveIfError( iLib.Load( KSortUtilImplJapan ) );
        }
#ifdef __KOREAN_SORT__
	else
        {
        // load polymorphic implementation DLL
        User::LeaveIfError( iLib.Load( KSortUtilImplKorean ) );
        }
#else
    else
        {
        // load polymorphic implementation DLL
        User::LeaveIfError( iLib.Load( KSortUtilImpl ) );
        }
#endif
    FeatureManager::UnInitializeLib();
#else    

    // load polymorphic implementation DLL
    User::LeaveIfError( iLib.Load( KSortUtilImpl ) );

#endif  // __WINS__
    
    iInterface = ( *(TSortUtilFactoryFunctionL)iLib.Lookup( 1 ) )();
    User::LeaveIfNull( iInterface );
    }

// End of File
