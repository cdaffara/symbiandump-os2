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
* Description:       Xmlsecurity utility tests
*
*/

//  CLASS HEADER
#include "xmlsecutilstester.h"

//  EXTERNAL INCLUDES
#include <StifParser.h>
#include <f32file.h>
#include <Stiftestinterface.h>
#include <s32file.h>
#include <e32svr.h>
#include <libc/sys/reent.h>

//  INTERNAL INCLUDES
#include "xmlsecwinit.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXmlSecTester::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CXmlSecUtilsTester::Delete() 
    {
    
    }
    
// -----------------------------------------------------------------------------
// CXmlSecTester::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CXmlSecUtilsTester::RunMethodL( 
    CStifItemParser& aItem ) 
    {
   
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
		
		
		// Test cases for encryption
		ENTRY( "TestXmlSecInitializeL_1L", CXmlSecUtilsTester::XmlSecInitializeL_1L),
		ENTRY( "TestXmlSecInitializeL_2L", CXmlSecUtilsTester::XmlSecInitializeL_2L),
		ENTRY( "TestXmlSecInitializeL_3L", CXmlSecUtilsTester::XmlSecInitializeL_3L),
		ENTRY( "TestXmlSecPushL_1L", CXmlSecUtilsTester::XmlSecPushL_1L),
		ENTRY( "TestXmlSecPushL_2L", CXmlSecUtilsTester::XmlSecPushL_2L),
		        
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    

// CONSTRUCTION
void CXmlSecUtilsTester::XmlSecInitializeL_1L(  )
    {
    XmlSecCleanup();
    }

void CXmlSecUtilsTester::XmlSecInitializeL_2L(  )
    {
    XmlSecInitializeL();
    XmlSecCleanup();
    }

void CXmlSecUtilsTester::XmlSecInitializeL_3L(  )
    {
    for(int i = 0; i<100; i++)
        XmlSecInitializeL();
    for(int i = 0; i<100; i++)
        XmlSecCleanup();
    }

void CXmlSecUtilsTester::XmlSecPushL_1L(  )
    {
    XmlSecPushL();
    XmlSecPopAndDestroy();
    CloseSTDLIB();
    }
    
void CXmlSecUtilsTester::XmlSecPushL_2L(  )
    {
    for(int i = 0; i<100; i++)
        XmlSecPushL();
    for(int i = 0; i<100; i++)
        {
        XmlSecPopAndDestroy();
        CloseSTDLIB();
        }
    }
//  END OF FILE
