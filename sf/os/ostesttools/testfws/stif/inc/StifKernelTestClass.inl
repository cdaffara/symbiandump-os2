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


#ifndef STIFKERNELTESTCLASS_INL
#define STIFKERNELTESTCLASS_INL

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RStifKernelTestClass::Open
// Open driver.
// -----------------------------------------------------------------------------
//
TInt RStifKernelTestClass::Open( const TVersion& aVer, const TDesC& aDriverName )
    {       
    return DoCreate( aDriverName, aVer, KNullUnit, NULL, NULL );    
    }

// -----------------------------------------------------------------------------
// RStifKernelTestClass::VersionRequired
// Get version info.
// -----------------------------------------------------------------------------
//
TVersion RStifKernelTestClass::VersionRequired() const
    {
    return TVersion( EMajorVersionNumber, EMinorVersionNumber, EBuildVersionNumber );
    }
    
// -----------------------------------------------------------------------------
// RStifKernelTestClass::RunMethod
// DoControl call to the kernel side for test case execution
// -----------------------------------------------------------------------------
//
                                    
TInt RStifKernelTestClass::RunMethod( const TDesC8&  aMethodName,
                                      const TDesC8&  aMethodParams,
                                      TInt&          aResult,
                                      TDes8&         aResultDes )                                   
    {      
    TStifRunMethodInfo methodInfo;
    if( ( aMethodName.Length() > methodInfo.iMethodName.MaxLength() ) ||
        ( aMethodParams.Length() > methodInfo.iMethodParams.MaxLength() ) ) 
        {
        return KErrArgument;
        }
    methodInfo.iMethodName.Copy( aMethodName );
    methodInfo.iMethodParams.Copy( aMethodParams );
    
    TPckg<TStifRunMethodInfo> methodInfoPckg( methodInfo );
    TInt ret = DoControl( ERunMethod, ( TAny* )&methodInfoPckg );
    
    if( ret != KErrNone )
        {
        return ret;
        }
    
    aResult = methodInfo.iResult;
    aResultDes.Copy( methodInfo.iMethodResultDes.Left( aResultDes.MaxLength() ) );
    return KErrNone;
    }

#endif      // STIFKERNELTESTCLASS_INL

// End of File
