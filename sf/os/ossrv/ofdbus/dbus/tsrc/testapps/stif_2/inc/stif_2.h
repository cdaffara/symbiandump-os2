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



#ifndef STIF_2_H
#define STIF_2_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include "test-utils.h"

#if defined(__WINSCW__) || defined(__WINS__)
extern "C" void _dbus_wsd_reset();
#endif

// Logging path
_LIT( Kstif_2LogPath, "\\logs\\testframework\\stif_2\\" ); 
// Log file
_LIT( Kstif_2LogFile, "stif_2.txt" ); 
_LIT( Kstif_2LogFileWithTitle, "stif_2_[%S].txt" );

class Cstif_2;


// CLASS DECLARATION

/**
*  Cstif_2 test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Cstif_2) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Cstif_2* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Cstif_2();

    public: // New functions
 
    TBool CheckConnection(DBusConnection* conn, DBusError* error);
    TBool CheckMovedError(DBusError* src, DBusError* dest, char *err_name);
        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
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
        Cstif_2( CTestModuleIf& aTestModuleIf );

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
        virtual TInt DBus_Error_Init0( CStifItemParser& aItem );
        virtual TInt DBus_Set_Error0( CStifItemParser& aItem );
        virtual TInt DBus_Set_Error1( CStifItemParser& aItem );
        virtual TInt DBus_Set_Error2( CStifItemParser& aItem );
        virtual TInt DBus_Error_Has_Name0( CStifItemParser& aItem );
        virtual TInt DBus_Error_Has_Name1( CStifItemParser& aItem );
        virtual TInt DBus_Error_Is_Set0( CStifItemParser& aItem );
        virtual TInt DBus_Error_Is_Set1( CStifItemParser& aItem );
        virtual TInt DBus_Move_Error0( CStifItemParser& aItem );
        virtual TInt DBus_Move_Error1( CStifItemParser& aItem );
        virtual TInt DBus_Move_Error2( CStifItemParser& aItem );
        virtual TInt DBus_Bus_Get0( CStifItemParser& aItem );
        virtual TInt DBus_Bus_Get_Private0( CStifItemParser& aItem );
        virtual TInt DBus_Bus_Get_Unique_Name0( CStifItemParser& aItem );
        virtual TInt DBus_Connection_Get_Is_Authenticated0( CStifItemParser& aItem );
        virtual TInt DBus_Connection_Get_Is_Connected0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Connection_Data_Slot0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Connection_Data_Slot1( CStifItemParser& aItem ) ;
        virtual TInt DBus_Connection_Data_Slot2( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Path0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Path1( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Path2( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Path3( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Type0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Type1( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Type2( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Type3( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Data_Slot0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Data_Slot1( CStifItemParser& aItem ) ;
        virtual TInt DBus_Bus_Request_Name0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Bus_Name_Has_Owner0( CStifItemParser& aItem );
        virtual TInt DBus_Message_Copy0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Type_To_String0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Type_From_String0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Interface0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Interface1( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Error_Name0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Type_Basic0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Type_Container0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Type_Fixed0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Is_Error0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Message_Iter0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Bus_Start_Service_By_Name0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Server_Listen0( CStifItemParser& aItem ) ;
        virtual TInt DBus_Server_Data_Slot0( CStifItemParser& aItem ) ;
        virtual TInt Dbus_Connection_Message_Size0( CStifItemParser& aItem ) ;
        virtual TInt Dbus_Get_Local_Machine_Id( CStifItemParser& aItem );
        virtual TInt Dbus_Server_Get_Is_Connected( CStifItemParser& aItem );
        virtual TInt Dbus_Threads_Init_Default( CStifItemParser& aItem );
        virtual TInt Dbus_Maloc( CStifItemParser& aItem );
        virtual TInt Dbus_Server_Set_Auth_Mechanism( CStifItemParser& aItem );
        virtual TInt DBus_Message_Get_Path_Decompose0( CStifItemParser& aItem );
     
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

#endif      // STIF_2_H

// End of File
