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
* Description: This file contains the header file of the 
* CStifTFwIfProt.
*
*/

#ifndef STIF_TFW_IF_PROT_H 
#define STIF_TFW_IF_PROT_H 

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <StifTestEventInterface.h>
#include <StifParser.h>

// CONSTANTS
// Protocol identifier length
const TInt KProtocolIdLength=8;
// Default allocation size for message created with CreateL
const TInt KDefaultMsgSize=0x200;
// Maximum length for value added to type-value parameter
const TInt KMaxValueLength=12;

// MACROS
// Macros for identifier manipulation
#define DEVID(x) ((TUint16)( ( 0xffff0000 & ((TUint32)(x)) ) >> 16 ))
#define TESTID(x) ((TUint16)( 0x0000ffff & ((TUint32)(x)) ))
#define SETID(d,t) ((TUint32)(( ( 0x0000ffff & ((TUint32)(d)) ) << 16 )|( 0x0000ffff & ((TUint32)(t)) )))
#define GETDEVID(x) ((TUint32)( 0xffff0000 & ((TUint32)(x)) ))

// DATA TYPES
typedef TPtrC (*KeywordFunc)( TInt aKeyword );
// typedef TBuf<KDefaultMessageSize> TRemoteProtMsg;

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

// DESCRIPTION

// CStifTFwIfProt is the control protocol interface
class CStifTFwIfProt
        :public CBase
    {
    public: // Enumerations

        // Message type
        enum TMsgType
            {
            EMsgReserve,
            EMsgRelease,
            EMsgRemote,
            EMsgResponse,
            };
            
        // Command types
        enum TCmdType
            {
            ECmdRun,
            ECmdPause,
            ECmdResume,
            ECmdCancel,
            ECmdRequest,
            ECmdRelease,
            ECmdSendReceive,
            ECmdSetEvent,
            ECmdUnsetEvent,
            
            ECmdUnknown,
            };
            
        // Remote device type
        enum TRemoteType
            {
            ERemotePhone,
            //ERemoteCallBox, ...
            
            ERemoteUnknown,
            };
            
        // Run parameters
        enum TRunParams
            {
            ERunModule,
            ERunInifile,
            ERunTestcasefile,
            ERunTestcasenum,                        
            ERunTitle,
            };
            
        // Response parameters
        enum TRespParam
            {
            ERespResult,
            };
     
        // Run status
        enum TRunStatus
            {
            ERunStarted,
            ERunError,
            ERunReady,
            };
            
        // Run status parameters
        enum TRunStatusParams
            {
            ERunResult,
            ERunCategory,
            };
            
        // Execution result category
        enum TResultCategory
            {
            EResultNormal = 0,
            EResultPanic,
            EResultException,
            EResultTimeout,
            EResultLeave,
            };
            
        // Event status
        enum TEventStatus
            {
            EEventActive,
            EEventSet,
            EEventError,
            }; 
            
        // Event status
        enum TEventStatusParams
            {
            EEventResult,
            EEventType,
            };           
                                    
    private: // Enumerations

    public:  // Constructors and destructor
       
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CStifTFwIfProt* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CStifTFwIfProt();

    public: // New functions
    
        /*
        * Get message handle
        */ 
        virtual const TDesC& Message(){ return iMessage; };
        
        /**
        * Set message.
        */
        IMPORT_C TInt SetL( const TDesC& aMessage );

        /**
        * Create message.
        */
        IMPORT_C void CreateL( TInt aLength = KDefaultMsgSize );
        
        /**
        * Append string to message.
        */
        IMPORT_C TInt Append( const TDesC& aStr );
        
        /**
        * Append hexadecimal protocol identifier to message.
        */
        IMPORT_C TInt AppendId( TUint32 aId );

        /**
        * Append keyword string to message.
        */
        IMPORT_C TInt Append( KeywordFunc aFunc, TInt aKeyword );

        /**
        * Append keyword type-value string to message.
        */
        IMPORT_C TInt Append( KeywordFunc aFunc, TInt aKeyword, TInt aValue );
        
        /**
        * Append keyword type-value string to message.
        */
        IMPORT_C TInt Append( KeywordFunc aFunc, 
                              TInt aKeyword, 
                              const TDesC& aStr );
        
        /**
        * Append keyword type-value string to message.
        */
        IMPORT_C TInt Append( KeywordFunc aFunc, 
                              TInt aKeyword, 
                              KeywordFunc aValueFunc, 
                              TInt aValue );
        
        /**
        * Get protocol source identifier.
        */
        IMPORT_C TUint32 SrcId();

        /**
        * Get protocol source device identifier.
        */
        IMPORT_C TUint16 SrcDevId();

        /**
        * Get protocol source test identifier.
        */
        IMPORT_C TUint16 SrcTestId();

        /**
        * Get protocol destination identifier.
        */
        IMPORT_C TUint32 DstId();
        
        /**
        * Get protocol destination device identifier.
        */
        IMPORT_C TUint16 DstDevId();
        
        /**
        * Get protocol destination test identifier.
        */
        IMPORT_C TUint16 DstTestId();
        
        /**
        * Set protocol source identifier.
        */
        IMPORT_C TInt SetSrcId( TUint32 aSrcId );
            
        /**
        * Set protocol destination identifier.
        */
        IMPORT_C TInt SetDstId( TUint32 aDstId );

        /**
        * Set message type.
        */
        IMPORT_C TInt SetMsgType( TMsgType aMsgType );
        
        /**
        * Set response type.
        */
        IMPORT_C TInt SetRespType( TMsgType aRespType );

        /**
        * Set message type.
        */
        IMPORT_C TInt SetCmdType( TCmdType aCmdType );

        /*
        * String containers.
        */
        IMPORT_C static TPtrC MsgType( TInt aKeyword );
        
        IMPORT_C static TPtrC CmdType( TInt aKeyword );
        
        IMPORT_C static TPtrC RemoteType( TInt aKeyword );

        IMPORT_C static TPtrC RunParams( TInt aKeyword );

        IMPORT_C static TPtrC RunStatus( TInt aKeyword );

        IMPORT_C static TPtrC RunStatusParams( TInt aKeyword );

        IMPORT_C static TPtrC ResultCategory( TInt aKeyword );

        IMPORT_C static TPtrC EventStatus( TInt aKeyword );

        IMPORT_C static TPtrC EventStatusParams( TInt aKeyword );

        IMPORT_C static TPtrC EventType( TInt aKeyword );
        
        IMPORT_C static TPtrC RespParam( TInt aKeyword );
        
    public: // Functions from base classes

        /**
        * From <base_class member_description.
        */
        //<type member_function( type arg1 );>
        
    protected:  // New functions
        
        /**
        * <member_description.>
        */
        //<type member_function( type arg1 );>

    protected:  // Functions from base classes
        
        /**
        * From <base_class member_description>
        */
        //<type member_function();>

    private:
        /**
        * C++ default constructor.
        */
        CStifTFwIfProt();
        
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();
        
        /**
        * Parsing functions.
        */
        TInt ParseMessageL();
        
        void ParseHeaderL();
        
        void ParseReserveL();
        
        void ParseRemoteL();

        void ParseResponseL();     
        
        void ParseCmdResponseL();   

        static TInt Parse( TDesC& aKeyword, KeywordFunc aFunc );
        
        static TInt ParseOptArg( const TDesC& aOptArg, TPtrC& aArg, TPtrC& aVal );
        
	public:   //Data
	    // Message type
        TMsgType            iMsgType;
        // Remote type
        TRemoteType         iRemoteType;
        // Response request type
        TMsgType            iRespType;
        // Remote command type (valid only if TMsgType is EMsgRemote)
        TCmdType            iCmdType;
        TPtrC               iCmdDes;
        
        // Run parameters (valid only if run command)
        // Test module name
        TPtrC               iModule;
        // Test module initialization file
        TPtrC               iIniFile;
        // Test module initialization file
        TPtrC               iTestCaseFile;
        // Test case number
        TInt                iTestCaseNumber;
        // Test case title
        TPtrC               iTitle;
        
        // Event parameters
        // Event name
        TPtrC               iEventName;
      
        // Result from request or command
        TInt            iResult;

        // Response status 
        union 
            {
            TRunStatus      iRunStatus;
            TEventStatus    iEventStatus;
            };
        // Result category
        TResultCategory     iResultCategory;
        // Event response parameters
        // Event type
        TEventIf::TEventType    iEventType;
    
	protected:  // Data
        //<data_declaration;>

    private:    // Data
        // Message buffer
        HBufC*              iMessageBuf;
        // Message ptr
        TPtr                iMessage;     
        
        // Message parser
        CStifItemParser*    iItem;
        
        // SrcId
        TInt32              iSrcId;
        // DstId
        TInt32              iDstId;
        
    public:     // Friend classes
        //<friend_class_declaration;>

    protected:  // Friend classes
        //<friend_class_declaration;>

    private:    // Friend classes
        //<friend_class_declaration;>
        
    };

#endif      // STIF_TFW_IF_PROT_H 

// End of File
