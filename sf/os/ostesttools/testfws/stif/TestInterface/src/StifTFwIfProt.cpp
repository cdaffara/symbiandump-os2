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
* Description: This file conatins StifTfIfProt implementation.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
#include "StifTFwIfProt.h"

#include <StifLogger.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define DEBUG(a) RDebug::Print(a)
//#define DEBUG2(a,b) RDebug::Print(a,b)
//#define DEBUG3(a,b,c) RDebug::Print(a,b,c)
#define DEBUG(a)
#define DEBUG2(a,b)
#define DEBUG3(a,b,c)
#define ERROR RDebug::Print

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ==================== LOCAL FUNCTIONS =======================================

/*
-------------------------------------------------------------------------------

    Function: <function name>

    Description: <one line description>

    <Description of the functionality 
    description continues and...
    continues>

    Parameters:	<arg1>: <in/out/inout>: <accepted values>: <Description>
    
    Return Values: <value_1: Description
		            value_n: Description line 1
			                 description line 2>

    Errors/Exceptions: <description how errors and exceptions are handled>

    Status: Draft

-------------------------------------------------------------------------------
*/
/*
<type function_name(
    <arg_type arg,>  
    <arg_type arg >)  	
    {
    // <comment>
    <code> 
    
    // <comment>
    <code>
    }
*/


/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CStifTFwIfProt class 
	member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: CStifTFwIfProt

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.
    
    Parameters:	none
    
    Return Values: None
    
    Errors/Exceptions: None
    
    Status: Draft
	
-------------------------------------------------------------------------------
*/
CStifTFwIfProt::CStifTFwIfProt(): iMessage(0,0)
    { 
    };
    
/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.

     Parameters:    None

     Return Values: None

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CStifTFwIfProt::ConstructL()
    {        

    }

/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: NewL

     Description: Two-phased constructor.
          
     Parameters:    None

     Return Values: CStifTFwIfProt*: new object

     Errors/Exceptions: Leaves if new or ConstructL leaves and in oom.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C CStifTFwIfProt* CStifTFwIfProt::NewL()
    {
     
    CStifTFwIfProt* self = new (ELeave) CStifTFwIfProt();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
     
    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: ~CStifTFwIfProt

    Description: Destructor

    Parameters:	None
    
    Return Values: None
    
    Errors/Exceptions: None
    
    Status: Draft
	
-------------------------------------------------------------------------------
*/    

EXPORT_C CStifTFwIfProt::~CStifTFwIfProt()
    {
    
    delete iItem;
    delete iMessageBuf;
    iMessageBuf = NULL;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: CreateL

    Description: Create new empty protocol message.

    Parameters:	TInt aLength: in: protocol message length
    
    Return Values: None
    
    Errors/Exceptions: Leave if oom.
    
    Status: Draft
	
-------------------------------------------------------------------------------
*/
EXPORT_C void CStifTFwIfProt::CreateL( TInt aLength )
    {
    
    // Delete previous if exists
    delete iMessageBuf;
    iMessageBuf = NULL;
    iMessage.Set( 0, 0, 0 );
    
    iMessageBuf = HBufC::NewL( aLength );
    iMessage.Set( iMessageBuf->Des() );
            
    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: Append

    Description: Append string to protocol message.

    Parameters:	TInt aLength: in: protocol message length
    
    Return Values: None
    
    Errors/Exceptions: Leave if oom.
    
    Status: Draft
	
-------------------------------------------------------------------------------
*/   
EXPORT_C TInt CStifTFwIfProt::Append( const TDesC& aStr )
    {
    
    if( ( aStr.Length() + 1 ) >
        ( iMessage.MaxLength() - iMessage.Length() ) )
        {
        ERROR( _L("No space left for string") );
        return KErrOverflow;   
        }
    iMessage.Append( aStr );
    iMessage.Append( _L(" ") );
    
    return KErrNone;

    }    

/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: Append

    Description: Append hexadecimal value to protocol message.

    Parameters:	TInt aLength: in: protocol message length
    
    Return Values: None
    
    Errors/Exceptions: Leave if oom.
    
    Status: Draft
	
-------------------------------------------------------------------------------
*/   
EXPORT_C TInt CStifTFwIfProt::AppendId( TUint32 aId )
    {
    
     if( ( KMaxValueLength + 1 ) >
        ( iMessage.MaxLength() - iMessage.Length() ) )
        {
        ERROR( _L("No space left for string") );
        return KErrOverflow;   
        }
        
    iMessage.AppendNumFixedWidth( aId, EHex, KProtocolIdLength );
    iMessage.Append( _L(" ") );
    
    return KErrNone;

    }
        
/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: Append

    Description: Append string to protocol message.

    Parameters:	TInt aLength: in: protocol message length
    
    Return Values: None
    
    Errors/Exceptions: Leave if oom.

    Status: Draft
	
-------------------------------------------------------------------------------
*/   
EXPORT_C TInt CStifTFwIfProt::Append( KeywordFunc aFunc, TInt aKeyword )
    {
    
    if( ( aFunc( aKeyword ).Length() + 1 ) >
        ( iMessage.MaxLength() - iMessage.Length() ) )
        {
        ERROR( _L("No space left for string") );
        return KErrOverflow;   
        }
    iMessage.Append( aFunc( aKeyword ) );
    iMessage.Append( _L(" ") );
    
    return KErrNone;

    }
    
/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: Append

    Description: Append type-value string to protocol message.

    Parameters:	TInt aLength: in: protocol message length
    
    Return Values: None
    
    Errors/Exceptions: Leave if oom.
    
    Status: Draft
	
-------------------------------------------------------------------------------
*/   
EXPORT_C TInt CStifTFwIfProt::Append( KeywordFunc aFunc, 
                                      TInt aKeyword, 
                                      const TDesC& aStr )
    {
    
     if( ( aFunc( aKeyword ).Length() + 2 +  aStr.Length() ) >
        ( iMessage.MaxLength() - iMessage.Length() ) )
        {
        ERROR( _L("No space left for string") );
        return KErrOverflow;
        }
        
    iMessage.Append( aFunc( aKeyword ) );
    iMessage.Append( _L("=") );
    iMessage.Append( aStr );
    iMessage.Append( _L(" ") );
    
    return KErrNone;

    }
    
/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: Append

    Description: Append type-value string to protocol message.

    Parameters:	TInt aLength: in: protocol message length
    
    Return Values: None
    
    Errors/Exceptions: Leave if oom.
    
    Status: Draft
	
-------------------------------------------------------------------------------
*/   
EXPORT_C TInt CStifTFwIfProt::Append( KeywordFunc aFunc, 
                                      TInt aKeyword, 
                                      KeywordFunc aValueFunc, 
                                      TInt aValue )
    {
    
     if( ( aFunc( aKeyword ).Length() + 2 +  aValueFunc( aValue ).Length() ) >
        ( iMessage.MaxLength() - iMessage.Length() ) )
        {
        ERROR( _L("No space left for string") );
        return KErrOverflow;   
        }
        
    iMessage.Append( aFunc( aKeyword ) );
    iMessage.Append( _L("=") );
    iMessage.Append( aValueFunc( aValue ) );
    iMessage.Append( _L(" ") );
    
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: Append

    Description: Append type-value string to protocol message.

    Parameters:	TInt aLength: in: protocol message length
    
    Return Values: None
    
    Errors/Exceptions: Leave if oom.
    
    Status: Draft
	
-------------------------------------------------------------------------------
*/   
EXPORT_C TInt CStifTFwIfProt::Append( KeywordFunc aFunc, 
                                       TInt aKeyword, 
                                       TInt aValue )
    {
    
     if( ( aFunc( aKeyword ).Length() + 2 + KMaxValueLength ) >
        ( iMessage.MaxLength() - iMessage.Length() ) )
        {
        ERROR( _L("No space left for string") );
        return KErrOverflow;   
        }
        
    iMessage.Append( aFunc( aKeyword ) );
    iMessage.Append( _L("=") );
    iMessage.AppendNum( aValue );
    iMessage.Append( _L(" ") );
    
    return KErrNone;

    }
        
/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: SetL

    Description: Set and parse protocol message.

    Parameters:	TDesC& aMessage: in: protocol message
    
    Return Values: Symbian OS error code: If protocol message parsing fails, 
        i.e. message prsing after header <msg type> <sdcid> <dstid>
    
    Errors/Exceptions: Leaves if protocol header parsing fails and oom.
    
    Status: Draft
	
-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIfProt::SetL( const TDesC& aMessage )
    {
    
    // Delete previous if exists
    delete iMessageBuf;
    iMessageBuf = NULL;
    iMessage.Set( 0,0,0 );
    
    iMessageBuf = aMessage.AllocL();
    iMessage.Set( iMessageBuf->Des() );
    
    return ParseMessageL();
        
    }
            
/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: SetL

    Description: Get protocol source identifier.

    Parameters:	None
    
    Return Values: Source identifier.
    
    Errors/Exceptions: None.

    Status: Draft
	
-------------------------------------------------------------------------------
*/
EXPORT_C TUint32 CStifTFwIfProt::SrcId()
    {
    
    return iSrcId;    
    
    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: SetL

    Description: Get protocol source device identifier.

    Parameters:	None
    
    Return Values: Source device identifier.
    
    Errors/Exceptions: None.
    
    Status: Draft
	
-------------------------------------------------------------------------------
*/
EXPORT_C TUint16 CStifTFwIfProt::SrcDevId()
    {
    
    return DEVID( iSrcId );  
        
    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: SetL

    Description: Get protocol source test identifier.

    Parameters:	None
    
    Return Values: Source test identifier.
    
    Errors/Exceptions: None.

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TUint16 CStifTFwIfProt::SrcTestId()
    {

    return TESTID( iSrcId );  
    
    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: SetL

    Description: Get protocol destination identifier.

    Parameters:	None
    
    Return Values: Destination identifier.
    
    Errors/Exceptions: None.

    Status: Draft
	
-------------------------------------------------------------------------------
*/
EXPORT_C TUint32 CStifTFwIfProt::DstId()
    {
    
    return iDstId;  

    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: SetL

    Description: Get protocol destination device identifier.

    Parameters:	None
    
    Return Values: Destination device identifier.
    
    Errors/Exceptions: None.

    Status: Draft
	
-------------------------------------------------------------------------------
*/
EXPORT_C TUint16 CStifTFwIfProt::DstDevId()
    {

    return DEVID( iDstId );  

    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: SetL

    Description: Get protocol destination test identifier.

    Parameters:	None
    
    Return Values: Destination test identifier.
    
    Errors/Exceptions: None.

    Status: Draft
	
-------------------------------------------------------------------------------
*/
EXPORT_C TUint16 CStifTFwIfProt::DstTestId()
    {

    return TESTID( iDstId );  
    
    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: ParseMessageL

    Description: Parse protocol message.

    Parameters:	None

    Return Values: None.

    Errors/Exceptions: None.

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CStifTFwIfProt::ParseMessageL()
    {
    RDebug::Print(_L("CStifTFwIfProt::ParseMessageL start"));

    RDebug::Print(_L("CStifTFwIfProt::ParseMessageL message content: (next line)"));
    RDebug::Print(iMessage);

    iItem = CStifItemParser::NewL( iMessage, 0, iMessage.Length() );

    ParseHeaderL();

    TRAPD( err,
    switch( iMsgType )
        {
        case EMsgReserve:
            RDebug::Print(_L("CStifTFwIfProt::ParseMessageL EMsgReserve"));
            ParseReserveL();
            break;
        case EMsgRelease:
            RDebug::Print(_L("CStifTFwIfProt::ParseMessageL EMsgRelease"));
            break;
        case EMsgRemote:
            RDebug::Print(_L("CStifTFwIfProt::ParseMessageL EMsgRemote"));
            ParseRemoteL();
            break;
        case EMsgResponse:
            RDebug::Print(_L("CStifTFwIfProt::ParseMessageL EMsgResponse"));
            ParseResponseL();
            break;
        default:
            RDebug::Print(_L("CStifTFwIfProt::ParseMessageL ERROR invalid message type. Leaving!!!"));
            ERROR( _L("Invalid message type") );        
            User::Leave( KErrArgument );
        }
    );
    
    delete iItem;
    iItem = 0;
    
    return err;
           
    }
    
/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: ParseHeaderL

    Description: Parse protocol header.

    Parameters:	None
    
    Return Values: None.
    
    Errors/Exceptions: None.
    
    Status: Draft
	
-------------------------------------------------------------------------------
*/
void CStifTFwIfProt::ParseHeaderL()
    {
    
    TPtrC tmp;
    TInt ret;
    TInt tmpMsgType;
    
    // Get and parse message type    
    ret = iItem->GetString( _L(""), tmp );
    if( ret != KErrNone )
        {
        ERROR( _L("No message type given") );        
        User::Leave( KErrNotFound );
        }
        
    tmpMsgType = Parse( tmp, MsgType );
    if( tmpMsgType < 0 )
        {
        ERROR( _L("Invalid message type given") );        
        User::Leave( KErrArgument );
        }
	iMsgType = ( TMsgType )tmpMsgType;

    // Get and parse srcid    
    ret = iItem->GetNextString( tmp );
    if( ret != KErrNone )
        {
        ERROR( _L("No srcid given") );        
        User::Leave( KErrNotFound );
        }
    // Check id length
    if( tmp.Length() != KProtocolIdLength )
        {
        ERROR( _L("Invalid srcid length [%d]"), tmp.Length() );        
        User::Leave( KErrArgument );
        }    
    TUint32 id;
    TLex lex( tmp );
    if( lex.Val( id, EHex ) != KErrNone )
        {
        ERROR( _L("Invalid srcid given") );        
        User::Leave( KErrArgument );
        }        
    iSrcId = id;


    // Get and parse dstid    
    ret = iItem->GetNextString( tmp );
    if( ret != KErrNone )
        {
        ERROR( _L("No dstid given") );        
        User::Leave( KErrNotFound );
        }
    // Check id length
    if( tmp.Length() != KProtocolIdLength )
        {
        ERROR( _L("Invalid srcid length [%d]"), tmp.Length() );        
        User::Leave( KErrArgument );
        }                    
    lex.Assign( tmp );
    if( lex.Val( id, EHex ) != KErrNone )
        {
        ERROR( _L("Invalid dstid given") );        
        User::Leave( KErrArgument );
        }        
    iDstId = id;
    
    }
    
    
/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: ParseReserveL

    Description: Parse protocol reserve message.

    Parameters:	None
    
    Return Values: None.
    
    Errors/Exceptions: None.
    
    Status: Draft
	
-------------------------------------------------------------------------------
*/
void CStifTFwIfProt::ParseReserveL()
    {
    
    TPtrC tmp;
    TInt ret;
	TInt tmpRemoteType;
    
    // Get and parse remote type    
    ret = iItem->GetNextString( tmp );
    if( ret != KErrNone )
        {
        ERROR( _L("No remote type given") );        
        User::Leave( KErrNotFound );
        }
            
    tmpRemoteType = Parse( tmp, RemoteType );
    if( tmpRemoteType < 0 )
        {
        iRemoteType = ERemoteUnknown;
        }
    else 
    	{
    	iRemoteType = ( TRemoteType ) tmpRemoteType;
    	}
        
    }
    
/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: ParseRemoteL

    Description: Parse protocol remote message.

    Parameters:	None
    
    Return Values: None.
    
    Errors/Exceptions: None.
    
    Status: Draft
	
-------------------------------------------------------------------------------
*/
void CStifTFwIfProt::ParseRemoteL()
    {
    RDebug::Print(_L("CStifTFwIfProt::ParseRemoteL start"));

    TPtrC tmp;
    TInt ret;
    TInt tmpCmdType;

    // Set mode of item parser to be able to read titles with spaces inside
    iItem->SetParsingType(CStifItemParser::EQuoteStyleParsing);

    // Get and parse command
    ret = iItem->GetNextString( tmp );
    if( ret != KErrNone )
        {
        ERROR( _L("No command given") );
        User::Leave( KErrNotFound );
        }

    iCmdDes.Set( tmp );
    tmpCmdType = Parse( tmp, CmdType );
    RDebug::Print(_L("CStifTFwIfProt::ParseRemoteL readed command %d"), tmpCmdType);
    if( tmpCmdType < 0 )
        {
        iCmdType = ECmdUnknown;
        DEBUG( _L("Unknown command given") );        
        }
    else
    	{
    	iCmdType = ( TCmdType ) tmpCmdType;
    	}
        
    switch( iCmdType )
        {
        case ECmdRun:
            {
            RDebug::Print(_L("CStifTFwIfProt::ParseRemoteL iCmdType ECmdRun"));
            // Parse run parameters
            TPtrC arg; 
            TPtrC val;
            iTestCaseNumber = KErrNotFound;
            while( iItem->GetNextString( tmp ) == KErrNone )
                {
                if( ParseOptArg( tmp, arg, val ) == KErrNone )
                    {              
                    TInt param = Parse( arg, RunParams );
                    if( param < 0 )
                        {
                        ERROR( _L("Invalid run parameter given") );        
                        User::Leave( KErrArgument );
                        }
                    
                    switch( param )
                        {
                        case ERunModule:
                            iModule.Set( val );
                            break;
                        case ERunInifile:
                            iIniFile.Set( val );
                            break;
                        case ERunTestcasefile:
                            iTestCaseFile.Set( val );
                            break;
                        case ERunTestcasenum:
                            {
                            TLex ptr( val );
                            if( ptr.Val( iTestCaseNumber ) != KErrNone )
                                {
                                ERROR( _L("Invalid test case number given") );        
                                User::Leave( KErrArgument );
                                }  
                            }
                            break;
                        case ERunTitle:
                            iTitle.Set(val);
                            break;
                        default:
                            ERROR( _L("Invalid run parameter given") );        
                            User::Leave( KErrArgument );
                        }
                    }
                else
                    {
                    ERROR( _L("Invalid run parameter given") );        
                    User::Leave( KErrArgument );
                    }
                }
            if( iModule.Length() == 0 )
                {
                ERROR( _L("No mandatory test module name given as run parameter") );        
                //User::Leave( KErrNotFound );
                }    
            if(iTestCaseNumber < 0 && iTitle.Length() == 0) //No test case number and no title
                {
                ERROR( _L("No mandatory test case number given as run parameter") );        
                //User::Leave( KErrNotFound );
                }    
            }   
            break;
        case ECmdPause:
        case ECmdResume:
        case ECmdCancel:
            RDebug::Print(_L("CStifTFwIfProt::ParseRemoteL iCmdType ECmdPause,Resume,Cancel"));
            break;
        case ECmdRequest:
        case ECmdRelease:
            RDebug::Print(_L("CStifTFwIfProt::ParseRemoteL iCmdType ECmdRequest,Release"));
            ret = iItem->GetNextString( tmp );
            if( ret != KErrNone )
                {
                ERROR( _L("No event name given") );        
                //User::Leave( KErrNotFound );
                iEventName.Set( 0, 0 );
                }
            else
                {
                iEventName.Set( tmp );
                }
            break;
        case ECmdSendReceive:
            {
            RDebug::Print(_L("CStifTFwIfProt::ParseRemoteL iCmdType ECmdSendReceive"));
            DEBUG( _L("sendreceive") );
            break;
            }
        default:
            RDebug::Print(_L("CStifTFwIfProt::ParseRemoteL iCmdType UNKNOWN!!!"));
            DEBUG( _L("Unknown command") );
            break;
        }
    }    
    
/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: ParseResponseL

    Description: Parse protocol response message.

    Parameters:	None
    
    Return Values: None.
    
    Errors/Exceptions: None.
    
    Status: Draft
	
-------------------------------------------------------------------------------
*/
void CStifTFwIfProt::ParseResponseL()
    {

    TPtrC tmp;
    TInt ret;
    TInt tmpRespType;
    TInt tmpCmdType;

    // Get and parse request response type
    ret = iItem->GetNextString( tmp );
    if( ret != KErrNone )
        {
        ERROR( _L("No request type given") );
        User::Leave( KErrNotFound );
        }

    tmpRespType = Parse( tmp, MsgType );
    if( tmpRespType < 0 )
        {
        ERROR( _L("Invalid request type given") );
        User::Leave( KErrArgument );
        }
    else
    	{
    	iRespType = ( TMsgType ) tmpRespType;
    	}

    ret = iItem->GetNextString( tmp );

    // First check if this is a response to a remote command
    if( iRespType == EMsgRemote )
        {
        if( ret != KErrNone )
            {
            ERROR( _L("No arguments for remote response given") );
            User::Leave( KErrNotFound );
            }

        iCmdDes.Set( tmp );
        tmpCmdType = Parse( tmp, CmdType );;
        if( tmpCmdType >= 0 )
            {
			iCmdType = ( TCmdType ) tmpCmdType;
            DEBUG2( _L("Remote response for %S"), &tmp );
            ParseCmdResponseL();
            // Get and parse general response parameters
            ret = iItem->GetNextString( tmp );
            }
        else
            {
            iCmdType = (TCmdType)KErrNotFound;
            }
        }
    while( ret == KErrNone )
        {
        TPtrC arg;
        TPtrC val;
        if( ParseOptArg( tmp, arg, val ) == KErrNone )
            {              
            TInt param = Parse( arg, RespParam );
            if( param < 0 )
                {
                ERROR( _L("Invalid parameter given") );        
                User::Leave( KErrArgument );
                }
            
            switch( param )
                {
                case ERespResult:
                    {
                    TLex ptr( val );
                    if( ptr.Val( iResult ) != KErrNone )
                        {
                        ERROR( _L("Invalid error code given") );        
                        User::Leave( KErrArgument );
                        }   
                    }
                    break;
                default:
                    ERROR( _L("Invalid parameter given") );        
                    User::Leave( KErrArgument );
                }       
            }
#if 0 // Check all parameters anyway
        else 
            {
            ERROR( _L("Invalid parameter given") );        
            User::Leave( KErrArgument ); 
            }
#endif
        ret = iItem->GetNextString( tmp );

        }
    
    }    
    
    
/*
-------------------------------------------------------------------------------

    Class: CStifTFwIfProt

    Method: ParseCmdResponseL

    Description: Parse protocol command response parameters.

    Parameters:	None
    
    Return Values: None.
    
    Errors/Exceptions: None.
    
    Status: Draft
	
-------------------------------------------------------------------------------
*/
void CStifTFwIfProt::ParseCmdResponseL()
    {
    
    TPtrC tmp;
    TPtrC arg; 
    TPtrC val;
    TInt ret;
    TInt tmpRunStatus;
    TInt tmpResultCategory;
    TInt tmpEventStatus;
    TInt tmpEventType;	
    
    RDebug::Print(_L("CStifTFwIfProt::ParseCmdResponseL start"));
    switch( iCmdType )
        {
        case ECmdRun:
            {
            RDebug::Print(_L("CStifTFwIfProt::ParseCmdResponseL ECmdRun"));
            // Parse run response status 
            ret = iItem->GetNextString( tmp );
            if( ret != KErrNone )
                {
                ERROR( _L("No run response status given") );        
                User::Leave( KErrNotFound );
                }
            
            tmpRunStatus = Parse( tmp, RunStatus );;
            if( tmpRunStatus < 0 )
                {
                ERROR( _L("Invalid run status in response given") );        
                User::Leave( KErrArgument );
                }
            else
            	{
            	iRunStatus = (TRunStatus) tmpRunStatus;
            	}
            
            // Parse runs status parameters
            while( iItem->GetNextString( tmp ) == KErrNone )
                {
                if( ParseOptArg( tmp, arg, val ) == KErrNone )
                    {              
                    TInt param = Parse( arg, RunStatusParams );
                    if( param < 0 )
                        {
                        ERROR( _L("Invalid run status parameter given") );        
                        User::Leave( KErrArgument );
                        }
                    
                    switch( param )
                        {
                        case ERunResult:
                            {
                            TLex ptr( val );
                            if( ptr.Val( iResult ) != KErrNone )
                                {
                                ERROR( _L("Invalid run result given") );        
                                User::Leave( KErrArgument );
                                }  
                            }
                            break;
                        case ERunCategory:
                            tmpResultCategory = Parse( val, ResultCategory );
                            if( tmpResultCategory < 0 )
                                {
                                ERROR( _L("Invalid run result category given") );        
                                User::Leave( KErrArgument );
                                }
                            else 
                            	{
                            	iResultCategory = ( TResultCategory ) tmpResultCategory;
                            	}
                            break;
                        default:
                            ERROR( _L("Invalid run status parameter given") );        
                            User::Leave( KErrArgument );
                        }
                    }
                else
                    {
                    ERROR( _L("Invalid run status parameter given") );        
                    User::Leave( KErrArgument );                    
                    }
                }
            }           
            break;
        case ECmdPause:
        case ECmdResume:
        case ECmdCancel:
            RDebug::Print(_L("CStifTFwIfProt::ParseCmdResponseL ECmdPause,Resume,Cancel"));
            break;
        case ECmdRequest:
            {
            RDebug::Print(_L("CStifTFwIfProt::ParseCmdResponseL ECmdRequest"));
            // Parse event request response status
            ret = iItem->GetNextString( tmp );
            if( ret != KErrNone )
                {
                ERROR( _L("No request response status given") );        
                User::Leave( KErrNotFound );
                }
            
            tmpEventStatus = Parse( tmp, EventStatus );
            if( tmpEventStatus < 0 )
                {
                ERROR( _L("Invalid request status in response given") );        
                User::Leave( KErrArgument );
                }
            else
            	{
            	iEventStatus = (TEventStatus) tmpEventStatus;
            	}
            
            // Parse request response event name
            ret = iItem->GetNextString( tmp );
            if( ret != KErrNone )
                {
                ERROR( _L("No request response event name given") );        
                User::Leave( KErrNotFound );
                }
            iEventName.Set( tmp );    
            
            // Parse request response status parameters
            while( iItem->GetNextString( tmp ) == KErrNone )
                {
                if( ParseOptArg( tmp, arg, val ) == KErrNone )
                    {              
                    TInt param = Parse( arg, EventStatusParams );
                    if( param < 0 )
                        {
                        ERROR( _L("Invalid request response status parameter given") );        
                        User::Leave( KErrArgument );
                        }
                    
                    switch( param )
                        {
                        case EEventResult:
                            {
                            TLex ptr( val );
                            if( ptr.Val( iResult ) != KErrNone )
                                {
                                ERROR( _L("Invalid request response status parameter result given") );        
                                User::Leave( KErrArgument );
                                }  
                            }
                            break;
                        case EEventType:
                            tmpEventType = Parse( val, EventType );;
                            if( tmpEventType < 0 )
                                {
                                ERROR( _L("Invalid request response status parameter event type given") );        
                                User::Leave( KErrArgument );
                                }
                            else
                            	{
                            	iEventType = ( TEventIf::TEventType ) tmpEventType;
                            	}
                            break;
                        default:
                            ERROR( _L("Invalid request response status parameter given") );        
                            User::Leave( KErrArgument );
                        }
                    }
                else
                    {
                    ERROR( _L("Invalid request response status parameter given") );        
                    User::Leave( KErrArgument );            
                    }    
                }
            }
            break;
        case ECmdRelease:
            RDebug::Print(_L("CStifTFwIfProt::ParseCmdResponseL ECmdRelease"));
            // Parse release response event name
            ret = iItem->GetNextString( tmp );
            if( ret != KErrNone )
                {
                ERROR( _L("No release response event name given") );        
                User::Leave( KErrNotFound );
                }
            iEventName.Set( tmp );    
            break;
        case ECmdSetEvent:
        case ECmdUnsetEvent:
            RDebug::Print(_L("CStifTFwIfProt::ParseCmdResponseL ECmdSetEvent,UnsetEvent"));
            // Parse release response event name
            ret = iItem->GetNextString( tmp );
            if( ret != KErrNone )
                {
                ERROR( _L("No set/unset response event name given") );
                User::Leave( KErrNotFound );
                }
            iEventName.Set( tmp );
            break;
        case ECmdSendReceive:
            {
            RDebug::Print(_L("CStifTFwIfProt::ParseCmdResponseL ECmdSendReceive"));
            // Parse sendreceive response status
            ret = iItem->GetNextString( tmp );
            if( ret != KErrNone )
                {
                ERROR( _L("No run response status given") );        
                User::Leave( KErrNotFound );
                }
            
            tmpRunStatus = Parse( tmp, RunStatus );
            if( tmpRunStatus < 0 )
                {
                ERROR( _L("Invalid run status in response given") );        
                User::Leave( KErrArgument );
                }
            else
            	{
            	iRunStatus = (TRunStatus) tmpRunStatus;
            	}
            break;
            }
        default:
            ERROR( _L("Invalid command response") );        
            User::Leave( KErrArgument );
        
        }      
    }
    
    
/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: MsgType

     Description: Returns a string desrciptor corresponding 
        to message type number. 

     Parameters:    TInt aKeyword: in: keyword index.
     
     Return Values: TPtrC: keyword descriptor

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/      
EXPORT_C TPtrC CStifTFwIfProt::MsgType( TInt aKeyword )
    {
    static TText* const keywords[] =
        {
        (TText*)L"reserve",
        (TText*)L"release",
        (TText*)L"remote",
        (TText*)L"response",
        };
    
    if( (TUint)aKeyword >= (sizeof( keywords )/sizeof(TText*)) )
          {
          TPtrC null;
          return null;
          } 
    
    TPtrC keyword( keywords[ aKeyword ] ); 
    return keyword;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: ResultCategory

     Description: Returns a string desrciptor corresponding 
        to result category number. 

     Parameters:    TInt aKeyword: in: keyword index.
     
     Return Values: TPtrC: keyword descriptor

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/      
EXPORT_C TPtrC CStifTFwIfProt::CmdType( TInt aKeyword )
    {
    static TText* const keywords[] =
        {
        (TText*)L"run",
        (TText*)L"pause",
        (TText*)L"resume",
        (TText*)L"cancel",
        (TText*)L"request",
        (TText*)L"release",
        (TText*)L"sendreceive",
        (TText*)L"set",
        (TText*)L"unset",
        };
    
    if( (TUint)aKeyword >= (sizeof( keywords )/sizeof(TText*)) )
          {
          TPtrC null;
          return null;
          } 
    
    TPtrC keyword( keywords[ aKeyword ] ); 
    return keyword;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: RemoteType

     Description: Returns a string desrciptor corresponding 
        to remote type number. 

     Parameters:    TInt aKeyword: in: keyword index.
     
     Return Values: TPtrC: keyword descriptor

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/      
EXPORT_C TPtrC CStifTFwIfProt::RemoteType( TInt aKeyword )
    {
    static TText* const keywords[] =
        {
        (TText*)L"phone",
        };
    
    if( (TUint)aKeyword >= (sizeof( keywords )/sizeof(TText*)) )
          {
          TPtrC null;
          return null;
          } 
    
    TPtrC keyword( keywords[ aKeyword ] ); 
    return keyword;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: RunParams

     Description: Returns a string desrciptor corresponding 
        to run parameter number. 

     Parameters:    TInt aKeyword: in: keyword index.
     
     Return Values: TPtrC: keyword descriptor

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/      
EXPORT_C TPtrC CStifTFwIfProt::RunParams( TInt aKeyword )
    {
    static TText* const keywords[] =
        {
        (TText*)L"module",
        (TText*)L"inifile",
        (TText*)L"testcasefile",
        (TText*)L"testcasenum",
        (TText*)L"title",
        };
    
    if( (TUint)aKeyword >= (sizeof( keywords )/sizeof(TText*)) )
          {
          TPtrC null;
          return null;
          } 
    
    TPtrC keyword( keywords[ aKeyword ] ); 
    return keyword;
    
    }
  
/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: RunStatus

     Description: Returns a string desrciptor corresponding 
        to run status number. 

     Parameters:    TInt aKeyword: in: keyword index.
     
     Return Values: TPtrC: keyword descriptor

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/      
EXPORT_C TPtrC CStifTFwIfProt::RunStatus( TInt aKeyword )
    {
    static TText* const keywords[] =
        {
        (TText*)L"started",
        (TText*)L"error",
        (TText*)L"ready",
        };
    
    if( (TUint)aKeyword >= (sizeof( keywords )/sizeof(TText*)) )
          {
          TPtrC null;
          return null;
          } 
    
    TPtrC keyword( keywords[ aKeyword ] ); 
    return keyword;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: RunStatusParams

     Description: Returns a string desrciptor corresponding 
        to run status parameter number. 

     Parameters:    TInt aKeyword: in: keyword index.
     
     Return Values: TPtrC: keyword descriptor

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/      
EXPORT_C TPtrC CStifTFwIfProt::RunStatusParams( TInt aKeyword )
    {
    static TText* const keywords[] =
        {
        (TText*)L"result",
        (TText*)L"category",
        };
    
    if( (TUint)aKeyword >= (sizeof( keywords )/sizeof(TText*)) )
          {
          TPtrC null;
          return null;
          } 
    
    TPtrC keyword( keywords[ aKeyword ] ); 
    return keyword;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: ResultCategory

     Description: Returns a string desrciptor corresponding 
        to command type number. 

     Parameters:    TInt aKeyword: in: keyword index.
     
     Return Values: TPtrC: keyword descriptor

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/      
EXPORT_C TPtrC CStifTFwIfProt::ResultCategory( TInt aKeyword )
    {
    static TText* const keywords[] =
        {
        (TText*)L"normal",
        (TText*)L"panic",
        (TText*)L"exception",
        (TText*)L"timeout",
        (TText*)L"leave",
        };
    
    if( (TUint)aKeyword >= (sizeof( keywords )/sizeof(TText*)) )
          {
          TPtrC null;
          return null;
          } 
    
    TPtrC keyword( keywords[ aKeyword ] ); 
    return keyword;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: EventStatus

     Description: Returns a string desrciptor corresponding 
        to command type number. 

     Parameters:    TInt aKeyword: in: keyword index.
     
     Return Values: TPtrC: keyword descriptor

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/      
EXPORT_C TPtrC CStifTFwIfProt::EventStatus( TInt aKeyword )
    {
    static TText* const keywords[] =
        {
        (TText*)L"active",
        (TText*)L"set",
        (TText*)L"error",
        };
    
    if( (TUint)aKeyword >= (sizeof( keywords )/sizeof(TText*)) )
          {
          TPtrC null;
          return null;
          } 
    
    TPtrC keyword( keywords[ aKeyword ] ); 
    return keyword;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: EventStatusParams

     Description: Returns a string desrciptor corresponding 
        to command type number. 

     Parameters:    TInt aKeyword: in: keyword index.
     
     Return Values: TPtrC: keyword descriptor

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/      
EXPORT_C TPtrC CStifTFwIfProt::EventStatusParams( TInt aKeyword )
    {
    static TText* const keywords[] =
        {
        (TText*)L"result",
        (TText*)L"type",
        };
    
    if( (TUint)aKeyword >= (sizeof( keywords )/sizeof(TText*)) )
          {
          TPtrC null;
          return null;
          } 
    
    TPtrC keyword( keywords[ aKeyword ] ); 
    return keyword;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: EventType

     Description: Returns a string desrciptor corresponding 
        to event type number. 

     Parameters:    TInt aKeyword: in: keyword index.
     
     Return Values: TPtrC: keyword descriptor

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/      
EXPORT_C TPtrC CStifTFwIfProt::EventType( TInt aKeyword )
    {
    static TText* const keywords[] =
        {
        (TText*)L"indication",
        (TText*)L"state",
        };
    
    if( (TUint)aKeyword >= (sizeof( keywords )/sizeof(TText*)) )
          {
          TPtrC null;
          return null;
          } 
    
    TPtrC keyword( keywords[ aKeyword ] ); 
    return keyword;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: RespParam

     Description: Returns a string desrciptor corresponding 
        to response parameter number. 

     Parameters:    TInt aKeyword: in: keyword index.
     
     Return Values: TPtrC: keyword descriptor

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/      
EXPORT_C TPtrC CStifTFwIfProt::RespParam( TInt aKeyword )
    {
    static TText* const keywords[] =
        {
        (TText*)L"result",
        };
    
    if( (TUint)aKeyword >= (sizeof( keywords )/sizeof(TText*)) )
          {
          TPtrC null;
          return null;
          } 
    
    TPtrC keyword( keywords[ aKeyword ] ); 
    return keyword;
    
    }


/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: SetSrcId

     Description: Set protocol source identifier.

     Parameters: TUint32 aSrcId: in: source identifier
     
     Return Values: Symbian OS error code

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIfProt::SetSrcId( TUint32 aSrcId )
    {
    
    iSrcId = aSrcId;
    return AppendId( iSrcId );
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: SetDstId

     Description: Set protocol destination identifier.

     Parameters: TUint32 aDstId: in: destination identifier
     
     Return Values: Symbian OS error code

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/ 
EXPORT_C TInt CStifTFwIfProt::SetDstId( TUint32 aDstId )
    {
    
    iDstId = aDstId;
    return AppendId( iDstId );
    
    }
    

/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: SetMsgType

     Description: Set message type.

     Parameters: TMsgType iMsgType: in: message type
     
     Return Values: Symbian OS error code

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/ 
EXPORT_C TInt CStifTFwIfProt::SetMsgType( TMsgType aMsgType )
    {
    
    iMsgType = aMsgType;
    return Append( CStifTFwIfProt::MsgType, iMsgType );
    
    }
    

/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: SetRespType

     Description: Set response type.

     Parameters:    TMsgType iMsgType : in: set response type
     
     Return Values: Symbian OS error code
     
     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/ 
EXPORT_C TInt CStifTFwIfProt::SetRespType( TMsgType aRespType )
    {
    
    iRespType = aRespType;
    return Append( CStifTFwIfProt::MsgType, iRespType );
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: SetCmdType

     Description: Set command type.

     Parameters: TCmdType iCmdType: in: command type
     
     Return Values: None

     Errors/Exceptions: None
     
     Status: Draft 
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIfProt::SetCmdType( TCmdType aCmdType )
    {
    
    iCmdType = aCmdType;
    return Append( CStifTFwIfProt::CmdType, iCmdType );
    
    }

    
/*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: Parse

     Description: Returns a keyword enum corresponding to keyword 
                  string descriptor.

     Parameters:    TPtrC aKeyword: in: keyword descriptor.
                    KeywordFunc aFunc: in: Function pointer to keyword parser 
     
     Return Values: TInt: keyword index
                    KErrNotFound: Keyword does not exists

     Errors/Exceptions: None
     
     Status: Draft 
    
------------------------------------------------------------------------------
*/
TInt CStifTFwIfProt::Parse( TDesC& aKeyword, KeywordFunc aFunc )
    {
    TInt ind;
    for( ind = 0; aFunc( ind ).Length() > 0; ind++ )
        {
        if( aFunc( ind ) == aKeyword )
            {
            return ind;
            }
        }
    return KErrNotFound;
    };
    
    
    /*
-------------------------------------------------------------------------------

     Class: CStifTFwIfProt

     Method: ParseOptArgL

     Description: Parses optional argument 
  
     Parameters: const TDesC& aOptArg: in: 
                    argument-value pair (format arg=value)
                 TPtrC& aArg: out: parsed argument  
                 TPtrC& aVal: out: parsed value
     
     Return Values: KErrNone: Everything ok
                    Symbian OS error code: Not a valid optarg

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt CStifTFwIfProt::ParseOptArg( const TDesC& aOptArg, 
                                   TPtrC& aArg, 
                                   TPtrC& aVal )
    { 
    TInt length = aOptArg.Length();
    for( TInt i=0; i < length; i++) 
        {
        // find the '=' sign 
        if( aOptArg[i] == '=' )
            {
            if( i+1 >= length )
                {
                return KErrArgument;
                }
            aArg.Set( aOptArg.Left( i ) );
            aVal.Set( aOptArg.Mid( i+1 ) );
            DEBUG3(  _L( "arg '%S', val '%S'" ),
                &aArg, &aVal );        
            return KErrNone;
            }
        }
    return KErrArgument;
    
    }     

// ================= OTHER EXPORTED FUNCTIONS =================================
// None

// End of File
