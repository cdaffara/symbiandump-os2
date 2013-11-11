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
* Description: This file contains TestScripterImp implementation.
*
*/

// INCLUDE FILES
#include <StifTestInterface.h>
#include <StifParser.h>
#include "TestScripterInternal.h"
#include "TestScripter.h"
// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// None


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CScriptBase::CScriptBase
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase::CScriptBase( CTestModuleIf& aTestModuleIf ):
    iTestModuleIf( aTestModuleIf )
    {
	// Handles multible 'waittestclass' given in test configure file.
    iSignalErrors.Reset();

    }

       
// -----------------------------------------------------------------------------
// CScriptBase::TestModuleIf
// Get reference to TestModuleIf API.
// -----------------------------------------------------------------------------
//
EXPORT_C CTestModuleIf& CScriptBase::TestModuleIf()
    { 
    
    return iTestModuleIf; 
    
    }
        
// -----------------------------------------------------------------------------
// CScriptBase::Signal
// Signal TestScripter to continue from waittestclass.
// -----------------------------------------------------------------------------
//
EXPORT_C void CScriptBase::Signal( TInt aError )
    { 
    
    if( iStatus ) 
        {
        User::RequestComplete( iStatus, aError ); 
        }
    else
    	{
		// Handles multible 'waittestclass' given in test configure file.
		// There is active object in CTestRunner also and it has higher
		// priority than CTestContinue(It must be higher that e.g.
		// 'allownextresult' is gotten right from configure file). That why we
		// have to use this array and take signal "count" to the array for
		// later handling.
    	iSignalErrors.Append( aError );
    	}
        
    }

// -----------------------------------------------------------------------------
// CScriptBase::CallTestClass
// Called from test class. TestScripter forwards
// operations to other test class object.
// aLine: in: <object name> <method name> <parameters>
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CScriptBase::CallTestClass( const TDesC& aLine )
    { 
    
    if( iFunc )
        {
        return iFunc( iTestScripter, EStifTSCallClass, aLine ); 
        }
    
    return KErrNotSupported;
    
    };

// -----------------------------------------------------------------------------
// CScriptBase::GetTestObject
// Get test class object address.
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* CScriptBase::GetTestObject( const TDesC& aObjectName )
    { 
    
    if( iFunc )
        {
        TInt ret = iFunc( iTestScripter, EStifTSGetObject, aObjectName );
        if( ret > 0 )
            {
            return( CScriptBase* )ret;
            } 
        }
    
    return NULL;
    };

// -----------------------------------------------------------------------------
// CScriptBase::EnableSignal
// Enable Signal().
// -----------------------------------------------------------------------------
//
EXPORT_C void CScriptBase::EnableSignal( TRequestStatus& aStatus )
    { 
    
    aStatus = KRequestPending; 
    iStatus = &aStatus; 

	// Handles multible 'waittestclass' given in test configure file.
    if( iSignalErrors.Count() > 0 )
    	{
		// Complete signal and remove it from array.
    	TInt error = iSignalErrors[0];
    	iSignalErrors.Remove( 0 );
    	User::RequestComplete( iStatus, error ); 
    	}
    
    }
   
// -----------------------------------------------------------------------------
// CScriptBase::CancelSignal
// Cancel Signal().
// -----------------------------------------------------------------------------
//
EXPORT_C void CScriptBase::CancelSignal()
    { 
    if(iStatus!=NULL)
        {
        User::RequestComplete( iStatus, KErrCancel ); 
        }
    }

// -----------------------------------------------------------------------------
// CScriptBase::SetScripter
// Set scripter callback.
// -----------------------------------------------------------------------------
//
EXPORT_C void CScriptBase::SetScripter( CallBackFunc aFunc, CTestScripter* aTestScripter )
    { 
    
    iTestScripter = aTestScripter; 
    iFunc = aFunc; 
    
    }

// -----------------------------------------------------------------------------
// CScriptBase::RunInternalL
// Run specified method from test class.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CScriptBase::RunInternalL ( 
    TStifFunctionInfo const aFunctions[], 
    TInt aCount, 
    CStifItemParser& aItem  ) 
    {

    TInt ret = KErrNotFound;
    
    TPtrC command; 
    User::LeaveIfError ( aItem.GetNextString( command ) );
    TInt i = 0;
    TName funcName;
    
    // Search function from table and call it
    for ( i = 0; i < aCount; i++ )
        {
        funcName = aFunctions[i].iFunctionName;      
        if ( command == funcName )
            {
            if( iLog )
                {
                iLog->Log(_L("Calling [%S]"), &command );
                }
            ret  = ( this->*(aFunctions[i].iMethod) )( aItem );    

            break;            
            }
    
        }

    if( i == aCount )
        {
        iLog->Log( _L("[%S] not found"), &command );
        }

    return ret;

    }
    
// -----------------------------------------------------------------------------
// CScriptBase::GetConstantValue
// Internal fuction to get const value defined in 
// [Define]...[Enddefine] section of script file
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CScriptBase::GetConstantValue( const TDesC& aName, TDes& aValue )
	{	
		return iTestScripter->GetConstantValue( aName, aValue );
	}

    
// -----------------------------------------------------------------------------
// CScriptBase::GetConstantValue
// Internal fuction to get const value defined in 
// [Define]...[Enddefine] section of script file
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CScriptBase::GetConstantValue( const TDesC& aName, TInt& aValue )
	{	
	TInt ret = KErrNone;
	TBuf<32> valStr;
	ret = iTestScripter->GetConstantValue( aName, valStr );
	if ( ret != KErrNone )
	{
		return ret;
	}

	TLex converter( valStr );
	ret = converter.Val( aValue );

	return ret;
	}
    
// -----------------------------------------------------------------------------
// CScriptBase::GetConstantValue
// Internal fuction to get const value defined in 
// [Define]...[Enddefine] section of script file
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CScriptBase::GetConstantValue( const TDesC& aName, TReal& aValue )
	{	
	TInt ret = KErrNone;
	TBuf<32> valStr;
	ret = iTestScripter->GetConstantValue( aName, valStr );
	if ( ret != KErrNone )
	{
		return ret;
	}

	TLex converter( valStr );
	ret = converter.Val( aValue );

	return ret;
	}

// -----------------------------------------------------------------------------
// CScriptBase::SetResultDescription
// Result description can be set from within the test class method 
// -----------------------------------------------------------------------------
//
EXPORT_C void CScriptBase::SetResultDescription(const TDesC& aDescription)
	{
	iTestScripter->SetResultDescription(aDescription);
	}

// -----------------------------------------------------------------------------
// CScriptBase::SetLocalValue
// Internal fuction to set local value
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CScriptBase::SetLocalValue(const TDesC& aName, const TDesC& aValue)
    {	
		return iTestScripter->SetLocalValue(aName, aValue);
    }
	
// -----------------------------------------------------------------------------
// CScriptBase::SetLocalValue
// Internal fuction to set local value
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CScriptBase::SetLocalValue(const TDesC& aName, const TInt aValue)
    {
    TBuf<20> buf;
    buf.Format(_L("%d"), aValue);
    return iTestScripter->SetLocalValue(aName, buf);
    }
	
// -----------------------------------------------------------------------------
// CScriptBase::SetLocalValue
// Internal fuction to set local value
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CScriptBase::SetLocalValue(const TDesC& aName, const TReal aValue )
    {	
    TBuf<50> buf;
    buf.Format(_L("%g"), aValue);
    return iTestScripter->SetLocalValue(aName, buf);
    }	

// -----------------------------------------------------------------------------
// CScriptBase::GetLocalValue
// Internal fuction to get local value
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CScriptBase::GetLocalValue(const TDesC& aName, TDes& aValue)
    {	
		return iTestScripter->GetLocalValue(aName, aValue);
    }
	
// -----------------------------------------------------------------------------
// CScriptBase::GetLocalValue
// Internal fuction to get local value
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CScriptBase::GetLocalValue(const TDesC& aName, TInt& aValue)
    {
    TInt ret = KErrNone;
    TBuf<50> valStr;
    ret = iTestScripter->GetLocalValue(aName, valStr);
    if(ret != KErrNone)
        {
        return ret;
        }

    TLex converter(valStr);
    ret = converter.Val(aValue);

    return ret;
    }
	
// -----------------------------------------------------------------------------
// CScriptBase::GetLocalValue
// Internal fuction to get local value
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CScriptBase::GetLocalValue(const TDesC& aName, TReal& aValue )
    {
    TInt ret = KErrNone;
    TBuf<50> valStr;
    ret = iTestScripter->GetLocalValue(aName, valStr);
    if(ret != KErrNone)
        {
        return ret;
        }

    TLex converter(valStr);
    ret = converter.Val(aValue);

    return ret;
    }

// End of File
