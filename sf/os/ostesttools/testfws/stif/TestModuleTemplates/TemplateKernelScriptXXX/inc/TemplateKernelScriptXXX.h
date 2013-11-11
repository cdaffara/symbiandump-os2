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
* Description: Kernel testclass declaration
*
*/

#ifndef TEMPLATEKERNELSCRIPTXXX_H
#define TEMPLATEKERNELSCRIPTXXX_H

//  INCLUDES
#include <StifKernelTestClassBase.h>

// CONSTANTS

// MACROS
_LIT( KDriverName, "TemplateKernelScriptXXX" );

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// DATA TYPES

// CLASS DECLARATION

// CLASS DECLARATION

/**
* DTemplateKernelScriptXXXDriver is the device driver factory that will
* instantiate the physical channel, which is the actual
* physical driver.
*
*  @lib ?library
*  @since ?Series60_version
*/
class DTemplateKernelScriptXXXDriver : public DStifKernelTestClassBaseDriver
    {

    public:
        DTemplateKernelScriptXXXDriver( const TDesC& aName ) :
            DStifKernelTestClassBaseDriver( aName ){};

    public:

        // Logical Channel creation
        TInt Create(DLogicalChannelBase*& aChannel);
    };


// CLASS DECLARATION

/**
*  DTemplateKernelScriptXXX is a kernel test class 
*  for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
class DTemplateKernelScriptXXX : public DStifKernelTestClassBase
    {

    public:
        DTemplateKernelScriptXXX( DLogicalDevice* aDevice ) : 
            DStifKernelTestClassBase( aDevice ){};
        ~DTemplateKernelScriptXXX(){};

    public:
        // Derived class must implement, runs a script line        
        TInt RunMethodL( const TDesC& aMethod, const TDesC& aParams );  

        // Second phase constructor for DLogicalChannelBase object in EKA2
        virtual TInt DoCreate( TInt aUnit, const TDesC8* aInfo, const TVersion& aVer );

        // Called from destructor.
        void Delete();

    protected: 

    private:
        /**
        * Test methods.
        */
        TInt ExampleL( const TDesC& aParams );

    private:

    };

#endif      // TEMPLATEKERNELSCRIPTXXX_H

// End of File
