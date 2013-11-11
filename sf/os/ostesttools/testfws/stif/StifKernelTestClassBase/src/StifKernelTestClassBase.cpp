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
* Description: This file contains DStifKernelTestClassBaseDriver 
* implementation.
*
*/

// INCLUDE FILES
#include "StifKernelTestClass.h"
#include "StifKernelTestClassBase.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DStifKernelTestClassBaseDriver::DStifKernelTestClassBaseDriver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C DStifKernelTestClassBaseDriver::DStifKernelTestClassBaseDriver( 
    const TDesC& aName )
    {

    __KTRACE_OPT(KHARDWARE, 
       Kern::Printf("DStifKernelTestClassBaseDriver::DStifKernelTestClassBaseDriver()"));

    iVersion=TVersion( RStifKernelTestClass::EMajorVersionNumber,
                       RStifKernelTestClass::EMinorVersionNumber,
                       RStifKernelTestClass::EBuildVersionNumber );
 
    iName.Copy( aName.Left( KMaxName ) );

    }

// -----------------------------------------------------------------------------
// DStifKernelTestClassBaseDriver::Install
// Set name.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DStifKernelTestClassBaseDriver::Install()
    {

    __KTRACE_OPT(KHARDWARE, 
        Kern::Printf("DStifKernelTestClassBaseDriver::Install()"));
 

    return SetName( &iName );       // Set our name and return error code.

    }

// -----------------------------------------------------------------------------
// DStifKernelTestClassBaseDriver::~DStifKernelTestClassBaseDriver
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DStifKernelTestClassBaseDriver::~DStifKernelTestClassBaseDriver()
    {

    __KTRACE_OPT(KHARDWARE, 
        Kern::Printf("DStifKernelTestClassBaseDriver::~DStifKernelTestClassBaseDriver()"));

    }

// -----------------------------------------------------------------------------
// DStifKernelTestClassBaseDriver::GetCaps
// Returns the drivers capabilities, may be used by LDD
// -----------------------------------------------------------------------------
//
EXPORT_C void DStifKernelTestClassBaseDriver::GetCaps( TDes8& /* aDes */ ) const
    {

    __KTRACE_OPT(KHARDWARE, 
        Kern::Printf("DStifKernelTestClassBaseDriver::GetCaps()"));
  
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CStifKernelTestClassBase::CStifKernelTestClassBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

EXPORT_C DStifKernelTestClassBase::DStifKernelTestClassBase( DLogicalDevice* /* aDevice */ )
    {
    // Get pointer to client threads DThread object
    iThread=&Kern::CurrentThread();

    // Open a reference on client thread so it's control block can't dissapear until
    // this driver has finished with it
    ((DObject*)iThread)->Open();

    __KTRACE_OPT(KHARDWARE, 
        Kern::Printf("DStifKernelTestClassBase::DStifKernelTestClassBase()"));         
    }
            
        
// -----------------------------------------------------------------------------
// DStifKernelTestClassBase::~DStifKernelTestClassBase()
// Destructor
// -----------------------------------------------------------------------------
//

EXPORT_C DStifKernelTestClassBase::~DStifKernelTestClassBase()
    { 

    __KTRACE_OPT(KHARDWARE, 
        Kern::Printf("DStifKernelTestClassBase::~DStifKernelTestClassBase()"));

    // Close our reference on the client thread
    Kern::SafeClose((DObject*&)iThread,NULL);

            
    Delete();        
    }

// -----------------------------------------------------------------------------
// CStifKernelTestClassBase::DoControl
// Handle syncronous request,
// -----------------------------------------------------------------------------
//

TInt DStifKernelTestClassBase::DoControl( 
    TInt aFunction, 
    TAny* a1, 
    TAny* /* a2 */ )   
    {

    __KTRACE_OPT(KHARDWARE, 
        Kern::Printf("DStifKernelTestClassBase::DoControl()")); 
    
    TInt ret = KErrNone;
    switch( aFunction )
        {
        case RStifKernelTestClass::ERunMethod:
            {
            TStifRunMethodInfo methodInfo;
            TPckg<TStifRunMethodInfo> methodInfoPckg( methodInfo );

            ret = Kern::ThreadDesRead( iThread, a1, methodInfoPckg, 0, KChunkShiftBy0 );
            if( ret != KErrNone )
                {
                __KTRACE_OPT(KHARDWARE, 
                    Kern::Printf("DStifKernelTestClassBase::DoControl: ThreadDesRead fails! %d", 333));
                return ret;
                }                
                            
            // TRAP is not supported in EKA2 kernel, so we can't use it. However, it is not even needed 
            // any longer because kernel test cases cannot leave either. Although RunMethodL's name 
            // seems to allow leaves (L in the end of the name), it really doesn't!
            ret = RunMethodL( methodInfo.iMethodName, methodInfo.iMethodParams );                                           
                                     
            if( ret != KErrNone )
                {
                methodInfo.iResult = ret;
                methodInfo.iMethodResultDes.Copy( _L("RunMethodL returned error: ") );
                methodInfo.iMethodResultDes.AppendNum( ret );
                }
            else
                {
                methodInfo.iResult = KErrNone;
                }

            ret = Kern::ThreadDesWrite( iThread, a1, methodInfoPckg, 0, 0, &Kern::CurrentThread() );

            }
            break;
        default:
            ret = KErrNotFound;
        }
        
    return ret;
    } 


// -----------------------------------------------------------------------------
// void DStifKernelTestClassBase::HandleMsg(TMessageBase* aMsg)
// Processes the DoControl call in EKA2
// -----------------------------------------------------------------------------
//
       
EXPORT_C void DStifKernelTestClassBase::HandleMsg( TMessageBase* aMsg )
    {       
    __KTRACE_OPT(KHARDWARE, 
        Kern::Printf("DStifKernelTestClassBase::HandleMsg"));
    
    TThreadMessage& m=*(TThreadMessage*)aMsg;

    // Get the id
    TInt id=m.iValue;
   
    if(id == (TInt)ECloseMsg)
        {
        // Lets close the channel
        m.Complete( KErrNone, EFalse );
        return;
        }
        
    if (id == KMaxTInt)
        {
        // Cancel operations are not needed because we don't use async requests
        m.Complete( KErrNone, ETrue );
        return;
        }
   
    if( id > 0 ) 
        {
        // Process the DoControl call
        TInt r = DoControl( id, m.Ptr0(), m.Ptr1() );
        m.Complete( r, ETrue );
        return;
        }        
    }


// -----------------------------------------------------------------------------
// DStifKernelTestClassBase::RunInternalL( 
//                        const TStifKernelFunctionInfo* const aFunctions, 
//                        TInt aCount, 
//                        const TDesC& aMethod, 
//                        const TDesC& aParams )
//
// Executes the test case. In EKA2 this function cannot leave.
// -----------------------------------------------------------------------------
//

EXPORT_C TInt DStifKernelTestClassBase::RunInternalL( 
                        const TStifFunctionInfo* const aFunctions, 
                        TInt aCount, 
                        const TDesC& aMethod, 
                        const TDesC& aParams )
    {
    
    TInt ret = KErrNotFound;

    TInt i = 0;
    TName funcName;
    
    // Search function from table and call it
    for ( i = 0; i < aCount; i++ )
        {
        funcName = aFunctions[i].iFunctionName;      
        if ( aMethod == funcName )
            {
            ret  = ( this->*(aFunctions[i].iMethod) )( aParams );
            break;            
            }
    
        }

    return ret;
    
    }

// End of File
