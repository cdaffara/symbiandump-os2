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
* Description: This file contains declaration of STIF interference
* classes.
*
*/

#ifndef TESTINTERFERENCEIMPLEMENTATION_H
#define TESTINTERFERENCEIMPLEMENTATION_H

// INCLUDES
#include <StifTestInterference.h>
#include <StifLogger.h>
#include <e32std.h>
#include <e32base.h>
#include <e32svr.h>

// Needed by interference test cases
#include <f32file.h>
// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
class CExecuteInterference;

// CLASS DECLARATION

/**
*  This is a CSTIFInterferenceAO class.
*  This class is inherited from CActive and class implements MSTIFTestInterference.
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS( CSTIFInterferenceAO ) : public CActive, 
										   public MSTIFTestInterference
    {

     public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSTIFInterferenceAO* NewL();

        /**
        * Destructor.
        */
        virtual ~CSTIFInterferenceAO();

    public: // New functions

        /**
        * StartL method starts test interference.
        */
        TInt StartL( TStifTestInterferenceType aType,
                    TInt aIdleTime,
                    TInt aActiveTime );

        /**
        * Stop method stops test interference.
        */
        TInt Stop();

        /**
        * Sets thread or active object priority. This should use before
        * test interference is started otherwise error code will return.
        */
        TInt SetPriority( TInt aPriority );

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CActive RunL handles request completion.
        */
        void RunL();

        /**
        * From CActive DoCancel handles request cancellation.
        */
        void DoCancel();

        /**
        * From CActive RunError handles error situations.
        */
        TInt RunError( TInt aError );

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CSTIFInterferenceAO();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

        // 
        TStifTestInterferenceType   iInterferenceType;
        // Test interference idle time
        TInt                        iIdleTime;
        // Test interference active time
        TInt                        iActiveTime;

        // Timer for executing idle time
        RTimer                      iAOIdleTimer;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data

        // CExecuteInterference object
        CExecuteInterference*       iExecuteInterference;

        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

/**
*  This a CSTIFInterferenceThread class.
*  This class is inherited from CBase and class implements MSTIFTestInterference.
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS( CSTIFInterferenceThread ) : public CBase,
											   public MSTIFTestInterference
    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSTIFInterferenceThread* NewL( CTestModuleBase* aTestModuleBase );

        /**
        * Destructor.
        */
        virtual ~CSTIFInterferenceThread();

    public: // New functions

        /**
        * StartL method starts test interference.
        */
        TInt StartL( TStifTestInterferenceType aType,
                    TInt aIdleTime,
                    TInt aActiveTime );

        /**
        * Stop method stops test interference.
        */
        TInt Stop();

        /**
        * Sets thread or active object priority. This should use before
        * test interference is started otherwise error code will return.
        */
        TInt SetPriority( TInt aPriority );

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

        static TInt ThreadFunction( TAny* aStarted );

        /**
        * Executes interference.
        */
        static void ExecuteInterferenceL( TStifTestInterferenceType aType, TInt aIdleTime, TInt aActiveTime );
    public: // Functions from base classes

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CSTIFInterferenceThread();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CTestModuleBase* aTestModuleBase );

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data

        // Struct that keeps thread related informations.
        struct TThreadParam
            {
            RSemaphore                  iStarted;
            TStifTestInterferenceType   iType;
            TInt                        iIdleTime;
            TInt                        iActiveTime;
            // Note: Be carefully if adding pointers to struct
            };

        // RThread object
        RThread                         iThread;

        // TThreadParam object(Struct that keeps thread related informations)
        TThreadParam*                   iThreadParam;

        // Pointer to STIF size
        CTestModuleBase*                iTestModuleBase;

        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

/**
*  This is a CExecuteInterference class.
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS( CExecuteInterference ) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CExecuteInterference* NewL();

        /**
        * Destructor.
        */
        virtual ~CExecuteInterference();

    public: // New functions

        /**
        * Start implement the test interference according to aType parameter.
        */
        TInt InterferenceL( 
                    MSTIFTestInterference::TStifTestInterferenceType aType,
                    TInt aActiveTime );

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CExecuteInterference();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Searches a file from given drive
        */        
        TInt SearchAFileForReading( TDes &aDriveName, RFs &aFileserver );

        /**
        * Prepares file system for test cases
        */        
        TInt PrepareFileSystemL( TDesC &aDriveName,
                                 TDesC &aFileName );

        /**
        * Deletes files recursively starting from aStartDirectory
        */        
        TInt DeleteFilesRecursivelyL( RFs &aFileserver, 
                                      TDes &aStartDirectory,
                                      const TTime &aEndTime );

        /**
        * Empties file system using DeleteFilesRecursivelyL method
        */        
        TInt EmptyTheFileSystemL( TDes &aFilePath,
                                 const TTime &aEndTime );

        /**
        * Creates CPU load
        */        
        TInt CPULoad( const TTimeIntervalMicroSeconds32 &aActiveTime );
       
        /**
        * Writes data to file asynchronously
        */                   
        TInt WriteToFileAsynchL( TDes &aFilePath,
                                RFs &aFileserver,
                                const TTime & aEndTime );
        /**
        * Repeats write until aActiveTime has expired
        */                                       
        TInt RepeatWriteToFileL( 
                    TDes &aFilePath,
                    const TTime &aActiveTime );
        
        /**
        * Repeats read until aActiveTime has expired
        */              
        TInt RepeatReadFromFileL( TDes &aFilePath,
                                 const TTimeIntervalMicroSeconds32 &aActiveTime );
                
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data

        // RTimer object
        RTimer                          iTimer;

        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // TESTINTERFERENCEIMPLEMENTATION_H

// End of File
