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

#include "xmlsecencrypttester.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXmlSecEncryptTester::CXmlSecEncryptTester
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXmlSecEncryptTester::CXmlSecEncryptTester( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {

	
    }
    
// -----------------------------------------------------------------------------
// CXmlSecEncryptTester::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXmlSecEncryptTester::ConstructL()
    {
    iLog = CStifLogger::NewL( KXmlSecTesterLogPath, 
                          KXmlSecTesterLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    }

// -----------------------------------------------------------------------------
// CXmlSecEncryptTester::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXmlSecEncryptTester* CXmlSecEncryptTester::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CXmlSecEncryptTester* self = new (ELeave) CXmlSecEncryptTester( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
	
    return self;
    
    }
    
// Destructor
CXmlSecEncryptTester::~CXmlSecEncryptTester()
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
    
    return ( CScriptBase* ) CXmlSecEncryptTester::NewL( aTestModuleIf );
        
    }
//  End of File
