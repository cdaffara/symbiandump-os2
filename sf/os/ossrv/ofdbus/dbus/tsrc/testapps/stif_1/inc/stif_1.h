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



#ifndef stif_1_H
#define stif_1_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <pthread.h>
#include "test-utils.h"
 
#if defined(__WINSCW__) || defined(__WINS__)
extern "C" void _dbus_wsd_reset();
#endif


// Logging path
_LIT( Kstif_1LogPath, "\\logs\\testframework\\stif_1\\" ); 
// Log file
_LIT( Kstif_1LogFile, "stif_1.txt" ); 
_LIT( Kstif_1LogFileWithTitle, "stif_1_[%S].txt" );
 
// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Cstif_1;


typedef struct{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	DBusConnection* connection;
	int ret; 
}threadData;

typedef struct{
pthread_mutex_t mutex;
pthread_cond_t cond;
int ret; 
}threadData1;


// CLASS DECLARATION

/**
*  Cstif_1 test class for stif Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Cstif_1) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Cstif_1* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Cstif_1();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
    	int handle_error(DBusError*);
    	int send_message(DBusConnection* connection, char* path, char* iface, char* member); 
    	void TestName(DBusConnection *connection, const char *name, int expectedSuccess, int* result);
    	int send_file(DBusConnection* connection, char* file_name);
        //?type ?member_function( ?type ?arg1 );

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
        Cstif_1( CTestModuleIf& aTestModuleIf );

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
        virtual TInt stif_Test_Names( CStifItemParser& aItem );
        virtual TInt stif_Method_Call( CStifItemParser& aItem );
        virtual TInt stif_N_Connections( CStifItemParser& aItem );
        virtual TInt stif_Variable_Args( CStifItemParser& aItem );
        virtual TInt stif_N_Messages( CStifItemParser& aItem );
        virtual TInt stif_N_Messages1( CStifItemParser& aItem );
        virtual TInt stif_Signal( CStifItemParser& aItem );
        virtual TInt stif_File_Send( CStifItemParser& aItem );
        virtual TInt stif_File_Send_Whole( CStifItemParser& aItem );
        virtual TInt stif_Match_Rule( CStifItemParser& aItem );
        virtual TInt stif_Sign_Test( CStifItemParser& aItem );
        virtual TInt stif_N_Separate_Connections( CStifItemParser& aItem );
        virtual TInt stif_Pending_Call_Data_Slot0( CStifItemParser& aItem );
        virtual TInt stif_Time0( CStifItemParser& aItem ); 
        virtual TInt stif_Daemon_Run_Time0( CStifItemParser& aItem ); 
        virtual TInt stif_Simultaneous_Connections( CStifItemParser& aItem ); 
        
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

#endif      // stif_1_H

// End of File
