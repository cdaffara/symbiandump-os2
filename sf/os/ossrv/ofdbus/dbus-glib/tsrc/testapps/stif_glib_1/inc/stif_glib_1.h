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





#ifndef STIF_GLIB_1_H
#define STIF_GLIB_1_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include <pthread.h>

#include "file_send-bindings.h"

#if defined(__WINSCW__) || defined(__WINS__)
extern "C" void _dbus_wsd_reset();
#endif

// Logging path
_LIT( Kstif_glib_1LogPath, "\\logs\\testframework\\stif_glib_1\\" ); 
// Log file
_LIT( Kstif_glib_1LogFile, "stif_glib_1.txt" ); 
_LIT( Kstif_glib_1LogFileWithTitle, "stif_glib_1_[%S].txt" );


// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Cstif_glib_1;

typedef struct{
pthread_mutex_t mutex;
pthread_cond_t cond;
DBusGProxy* proxy;
int ret; 
}threadData1;


// CLASS DECLARATION

/**
*  Cstif_glib_1 test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Cstif_glib_1) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Cstif_glib_1* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Cstif_glib_1();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
    int send_file(DBusGProxy* proxy, char* file_name);
    int send_text_file(DBusGProxy* proxy, char* file_name);
    int read_fifos(void);
    
    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

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
        Cstif_glib_1( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
   //     virtual TInt stif_File_SendL( CStifItemParser& aItem );
        virtual TInt stif_File_Send1( CStifItemParser& aItem );
        virtual TInt stif_Variable_Args( CStifItemParser& aItem );
        virtual TInt stif_Signal( CStifItemParser& aItem );
        virtual TInt stif_N_Messages( CStifItemParser& aItem );
        virtual TInt stif_N_Messages1( CStifItemParser& aItem );
        virtual TInt stif_Proxy_Begin_Call( CStifItemParser& aItem );
        virtual TInt stif_Proxy_Call_Timeout( CStifItemParser& aItem );
        virtual TInt stif_Proxy_Begin_Call_Timeout( CStifItemParser& aItem );
        virtual TInt stif_Struct_Send0( CStifItemParser& aItem );
        virtual TInt stif_Variant_Send0( CStifItemParser& aItem );
        virtual TInt stif_Dict_Send0( CStifItemParser& aItem );
        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
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

#endif      // STIF_GLIB_1_H

// End of File
