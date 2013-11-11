// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include "xmlengtester.h"
#include "xmlengtesterdef.h"
#include <xml/utils/xmlengxestd.h>
#include <Stiftestinterface.h>

#include <sys/reent.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLibxml2Tester::CLibxml2Tester
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLibxml2Tester::CLibxml2Tester( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CLibxml2Tester::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLibxml2Tester::ConstructL()
    {
    iLog = CStifLogger::NewL( KXML_TestLogPath, 
                          KXML_TestLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
                          
    XmlEngineAttachL();
		
	DOM_impl.OpenL( );
	
	parser.Open( DOM_impl );
	
	infoNum = 0;
	iLastError = KErrNone;
    }

// -----------------------------------------------------------------------------
// CLibxml2Tester::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLibxml2Tester* CLibxml2Tester::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CLibxml2Tester* self = new (ELeave) CLibxml2Tester( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

	return self;
    
    }
    
// Destructor
CLibxml2Tester::~CLibxml2Tester()
    { 
    
    if(iDoc.NotNull())
    	{
    	iDoc.Close();
    	}
    parser.Close();
    DOM_impl.Close();
    XmlEngineCleanup();
    CloseSTDLIB();  // Close EstLib here
    // Delete resources allocated from test methods
    Delete();
    
    // Delete logger
    delete iLog;  
   
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
    
    return ( CScriptBase* ) CLibxml2Tester::NewL( aTestModuleIf );
        
    }
    
//  End of File
