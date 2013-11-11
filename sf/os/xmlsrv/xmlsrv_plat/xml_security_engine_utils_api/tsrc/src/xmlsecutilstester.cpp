/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:       Main file
*
*/

// INCLUDE FILES
#include <Stiftestinterface.h>
#include <e32svr.h>
#include <StifParser.h>
#include <libc/sys/reent.h>

#include "xmlsecutilstester.h"
#include "xmlsecwinit.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXmlSecUtilsTester::CXmlSecUtilsTester
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXmlSecUtilsTester::CXmlSecUtilsTester( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {

	
    }
    
// -----------------------------------------------------------------------------
// CXmlSecUtilsTester::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXmlSecUtilsTester::ConstructL()
    {
    iLog = CStifLogger::NewL( KXmlSecTesterLogPath, 
                          KXmlSecTesterLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    }

// -----------------------------------------------------------------------------
// CXmlSecUtilsTester::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXmlSecUtilsTester* CXmlSecUtilsTester::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CXmlSecUtilsTester* self = new (ELeave) CXmlSecUtilsTester( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
	
    return self;
    
    }
    
// Destructor
CXmlSecUtilsTester::~CXmlSecUtilsTester()
    { 
    
    // Delete resources allocated from test methods
    Delete();
     // Delete logger
    delete iLog;  
    
    CloseSTDLIB(); 
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {
    
    return ( CScriptBase* ) CXmlSecUtilsTester::NewL( aTestModuleIf );
        
    }
    

    
//  End of File
