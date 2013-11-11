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
* Description: This file contains the header file of the STIF 
* kernel testclass declaration
*
*/

#ifndef STIFKERNELTESTCLASSBASE_H
#define STIFKERNELTESTCLASSBASE_H

//  INCLUDES
#include <kernel.h>    


// CONSTANTS

// MACROS


// In EKA2 kernel only 8bit descriptors are allowed, because of that C++'s 
// L makro cannot be used.

// ENTRY macro is modified and now it stores data as 8bit descriptor.
// In EKA2 kernel TText is 8bit descriptor.

#define ENTRY(A,B) \
	{ (TText*) A, (StifTestFunction) &B }


// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class DStifKernelTestClassBase;

// DATA TYPES
typedef TInt (DStifKernelTestClassBase::* StifTestFunction)( const TDesC& );    

// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class TStifFunctionInfo
    {
    public:            
        const TText*           iFunctionName;
        StifTestFunction       iMethod;    
    };
    
// CLASS DECLARATION

/**
* DStifKernelTestClassBaseDriver is the device driver factory that will
* instantiate the physical channel, which is the actual
* physical driver.
*
*  @lib ?library
*  @since ?Series60_version
*/
class DStifKernelTestClassBaseDriver : public DLogicalDevice
    {

    public:
        IMPORT_C DStifKernelTestClassBaseDriver( const TDesC& aName );
        IMPORT_C virtual ~DStifKernelTestClassBaseDriver();

        IMPORT_C virtual TInt Install();
        IMPORT_C virtual void GetCaps(TDes8 &aDes) const;

        virtual TInt Create(DLogicalChannelBase*& aChannel)=0;
  
    private:
        TName   iName;
    
    };


// CLASS DECLARATION

/**
*  DStifKernelTestClassBase is a base class for kernel test class 
*  for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
class DStifKernelTestClassBase : public DLogicalChannel
    {

    public:
        IMPORT_C DStifKernelTestClassBase( DLogicalDevice* aDevice );
        IMPORT_C virtual ~DStifKernelTestClassBase();
        
    public:     
        /**
        * Runs a script line. Derived class must implement
        */
        virtual TInt RunMethodL( const TDesC& aMethod, const TDesC& aParams ) = 0;  
        
        /**
        * Called from destructor. Derived class may implement.
        */
        virtual void Delete(){};  
        
        // Processes a message for this logical channel
        IMPORT_C virtual void HandleMsg( TMessageBase* aMsg );
    protected: // Derived class uses
        
        // Internal fuction to run specified method.              
        IMPORT_C virtual TInt RunInternalL( 
                        const TStifFunctionInfo* const aFunctions, 
                        TInt aCount, 
                        const TDesC& aMethod, 
                        const TDesC& aParams );

    private: 

        // No need to export DoControl in EKA2, because it is called
        // from HandleMsg
        TInt DoControl( TInt aFunction,TAny* a1,TAny* a2 );
     
        // Asynchronous command are not supported
        void DoCancel( TInt ){};
                  
    private:
     
        DThread*         iThread;                
     
    };

#endif      // STIFKERNELTESTCLASSBASE_H

// End of File
