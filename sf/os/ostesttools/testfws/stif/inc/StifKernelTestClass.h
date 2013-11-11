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
* Description: Kernel test class for STIF Test Framework TestScripter
*
*/


#ifndef STIFKERNELTESTCLASS_H
#define STIFKERNELTESTCLASS_H

// INCLUDES

#include <e32cmn.h>


// CONSTANTS

// MACROS

// DATA TYPES

typedef TBuf8<0x80>  TMethodResultDes;
typedef TBuf8<0x20>  TMethodName;
typedef TBuf8<0x100> TMethodParams;


// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class TStifRunMethodInfo
    {
    public:
        TStifRunMethodInfo() : 
            iResult( 0 )
            {
            iMethodName.Zero();
            iMethodParams.Zero();
            iMethodResultDes.Zero();
            }
    
    public:
        TMethodName         iMethodName;
        TMethodParams       iMethodParams;
        TMethodResultDes    iMethodResultDes;
        TInt                iResult;
        
    };

// CLASS DECLARATION

/**
*  RStifKernelTestClass kernel test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
class RStifKernelTestClass : public RBusLogicalChannel
    {
    public:
        // Synchronous operations
        enum TStifKernelTCOperation
            { 
            ERunMethod = 1,                       
            };
    
        // Version number
        enum TStifKernelTCVersion
            {
            EMajorVersionNumber=1,
            EMinorVersionNumber=0,
            EBuildVersionNumber=1
            };

    public: // Interface functions

          // Open
          inline TInt Open( const TVersion& aVer, const TDesC& aDriverName );

          // Return required version
          inline TVersion VersionRequired() const;
          
          // Run specific method
          
        
          inline TInt RunMethod( const TDesC8&  aMethodName,
                                 const TDesC8&  aParams,
                                 TInt&          aResult,
                                 TDes8&         aResultDes );
 
    };
    
// Include inline function implementations
#include "StifKernelTestClass.inl"

#endif      // STIFKERNELTESTCLASS_H

// End of File
