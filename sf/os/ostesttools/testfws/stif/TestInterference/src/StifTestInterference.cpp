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
* Description: This module contains the implementation of 
* MSTIFTestInterference class member functions.
*
*/

// INCLUDE FILES
#include <StifTestInterference.h>
#include "TestInterferenceImplementation.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// For test interference thread's heap size
const TUint KMaxHeapSize        = 0x20000; // 128 K

// MACROS
//#define ?macro ?macro_def

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

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

/*
-------------------------------------------------------------------------------

    Class: MSTIFTestInterference

    Method: MSTIFTestInterference

    Description: Create test interference object according to the paramater.

    Parameters: CTestModuleBase* aTestModuleBase: inout: CTestModuleBase object
                for get test interference handle to STIF's side(Used if test
                case panic so test interference thread can be kill by STIF).
                TStifTestInterferenceCategory aCategory: in: Test interference
                category

    Return Values: MSTIFTestInterference*: pointer to MSTIFTestInterference 
                   object

    Errors/Exceptions: Leaves if object creation fails.

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C MSTIFTestInterference* MSTIFTestInterference::NewL(
                                    CTestModuleBase* aTestModuleBase,
                                    TStifTestInterferenceCategory aCategory )
    {
    if( aCategory == EActiveObject )        // Active object
        {
        CSTIFInterferenceAO* self = NULL;
        self = CSTIFInterferenceAO::NewL();
        return (MSTIFTestInterference*)self;
        }
    else                                    // EThread
        {
        CSTIFInterferenceThread* self = NULL;
        self = CSTIFInterferenceThread::NewL( aTestModuleBase );
        return (MSTIFTestInterference*)self;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: MSTIFTestInterference

    Method: MSTIFTestInterference

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
MSTIFTestInterference::~MSTIFTestInterference()
    {
    // None

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CSTIFInterferenceAO class 
    member functions.

-------------------------------------------------------------------------------
*/

// ============================ MEMBER FUNCTIONS ===============================

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceAO

    Method: CSTIFInterferenceAO

    Description: C++ default constructor can NOT contain any code, that
                 might leave 

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CSTIFInterferenceAO::CSTIFInterferenceAO() :
                                    CActive ( CActive::EPriorityStandard )
    {
    iInterferenceType = ENone;
    iIdleTime = 0;
    iActiveTime = 0;
    iExecuteInterference = NULL;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceAO

    Method: ConstructL

    Description: Symbian 2nd phase constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leave if CreateLocal fails

    Status: Approved

-------------------------------------------------------------------------------
*/
void CSTIFInterferenceAO::ConstructL()
    {
    CActiveScheduler::Add ( this );

    User::LeaveIfError( iAOIdleTimer.CreateLocal() );

    iExecuteInterference = CExecuteInterference::NewL();

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceAO

    Method: NewL

    Description: Two-phased constructor.

    Parameters: None

    Return Values: CSTIFInterferenceAO*: pointer to CSTIFInterferenceAO object

    Errors/Exceptions: Leaves if object creation fails.

    Status: Approved

-------------------------------------------------------------------------------
*/
CSTIFInterferenceAO* CSTIFInterferenceAO::NewL()
    {
    CSTIFInterferenceAO* self = new (ELeave) CSTIFInterferenceAO();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceAO

    Method: ~CSTIFInterferenceAO

    Description: Destructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CSTIFInterferenceAO::~CSTIFInterferenceAO()
    {
    Cancel(); // Cancel itself
    // If test case panic etc. do close operations here.
    delete iExecuteInterference;
    iExecuteInterference = NULL;

    iAOIdleTimer.Close();

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceAO

    Method: RunL

    Description: Derived from CActive, handles test interference execution.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if InterferenceL leaves.

    Status: Approved

-------------------------------------------------------------------------------
*/
void CSTIFInterferenceAO::RunL( )
    {
    iExecuteInterference->InterferenceL( iInterferenceType, iActiveTime );
    
    // Start idle timer
    iAOIdleTimer.After( iStatus, iIdleTime );
    SetActive();

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceAO

    Method: DoCancel

    Description: Derived from CActive handles the Cancel

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CSTIFInterferenceAO::DoCancel( )
    {
    iAOIdleTimer.Cancel();

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceAO

    Method: RunError

    Description: Derived from CActive handles errors from active handler.

    Parameters: TInt aError: in: error from CActive 

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CSTIFInterferenceAO::RunError( TInt aError )
    {
    RDebug::Print( _L( "CSTIFInterferenceAO::RunError() with [%d]" ), aError );

    return aError;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceAO

    Method: StartL

    Description: StartL method starts test interference.

    Parameters: TStifTestInterferenceType aType: in: Test interference type.
                TInt aIdleTime: in: Test interference idle time.
                TInt aActiveTime: in: Test interference active time.

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: Leaves if active object is active.

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CSTIFInterferenceAO::StartL( TStifTestInterferenceType aType,
                                    TInt aIdleTime,
                                    TInt aActiveTime )
    {
    if( IsActive() )
        {
        User::Leave( KErrAlreadyExists );
        }

    switch( aType )
    	{
    	case ENone:
    	case ECpuLoad:
    	case EFileSystemReadC:
    	case EFileSystemReadD:
    	case EFileSystemReadE:
    	case EFileSystemReadZ:
    	case EFileSystemWriteC:
    	case EFileSystemWriteD:
    	case EFileSystemWriteE:
    	case EFileSystemFillAndEmptyC:
    	case EFileSystemFillAndEmptyD:
    	case EFileSystemFillAndEmptyE:
    		aIdleTime = aIdleTime * 1000;
    		aActiveTime = aActiveTime * 1000;
    		break;
    	case ENoneMicroSeconds:
    		aType = ENone;
    		break;
    	case ECpuLoadMicroSeconds:
			aType = ECpuLoad;
			break;
    	case EFileSystemReadCMicroSeconds:
			aType = EFileSystemReadC;
			break;
    	case EFileSystemReadDMicroSeconds:
			aType = EFileSystemReadD;
			break;
    	case EFileSystemReadEMicroSeconds:
			aType = EFileSystemReadE;
			break;
    	case EFileSystemReadZMicroSeconds:
			aType = EFileSystemReadZ;
			break;
    	case EFileSystemWriteCMicroSeconds:
			aType = EFileSystemWriteC;
			break;
    	case EFileSystemWriteDMicroSeconds:
			aType = EFileSystemWriteD;
			break;
    	case EFileSystemWriteEMicroSeconds:
			aType = EFileSystemWriteE;
			break;
    	case EFileSystemFillAndEmptyCMicroSeconds:
			aType = EFileSystemFillAndEmptyC;
			break;
    	case EFileSystemFillAndEmptyDMicroSeconds:
			aType = EFileSystemFillAndEmptyD;
			break;
    	case EFileSystemFillAndEmptyEMicroSeconds:
			aType = EFileSystemFillAndEmptyE;
			break;
    	}    
    
    iInterferenceType = aType;
    iIdleTime = aIdleTime;
    iActiveTime = aActiveTime;

    // Set request to pending and active object to active
    iStatus = KRequestPending;
    SetActive();
    // Complete request immediately
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceAO

    Method: Stop

    Description: Stop method stops test interference.

    Parameters: None

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CSTIFInterferenceAO::Stop()
    {
    Cancel();

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceAO

    Method: SetPriority

    Description: Sets thread or active object priority. This should use before
                 test interference is started otherwise error code will return.

    Parameters: TInt aPriority: in: New priority for active object given by
                user.

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CSTIFInterferenceAO::SetPriority( TInt aPriority )
    {
    if( IsActive() )
        {
        RDebug::Print( _L( "STIF: Priority cannot set because active object is active" ) );
        return KErrGeneral;
        }

    CActive::SetPriority( (TPriority)aPriority );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CSTIFInterferenceThread class 
    member functions.

-------------------------------------------------------------------------------
*/


// ============================ MEMBER FUNCTIONS ===============================

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceThread

    Method: CSTIFInterferenceThread

    Description: C++ default constructor can NOT contain any code, that
                 might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CSTIFInterferenceThread::CSTIFInterferenceThread()
    {
    // None

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceThread

    Method: ConstructL

    Description: C++ default constructor can NOT contain any code, that
                 might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CSTIFInterferenceThread::ConstructL( CTestModuleBase* aTestModuleBase )
    {
    iTestModuleBase = aTestModuleBase;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceThread

    Method: NewL

    Description: Two-phased constructor.

    Parameters: None

    Return Values: CSTIFInterferenceThread*: pointer to CSTIFInterferenceThread
                   object

    Errors/Exceptions: Leaves if object creation fails.

    Status: Approved

-------------------------------------------------------------------------------
*/
CSTIFInterferenceThread* CSTIFInterferenceThread::NewL( 
                                            CTestModuleBase* aTestModuleBase )
    {
    CSTIFInterferenceThread* self = new (ELeave) CSTIFInterferenceThread();

    CleanupStack::PushL( self );
    self->ConstructL( aTestModuleBase );
    CleanupStack::Pop();

    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceThread

    Method: ~CSTIFInterferenceThread

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CSTIFInterferenceThread::~CSTIFInterferenceThread()
    {
    // If test case crash etc. do stop operations here also
    if( iThreadParam != NULL )
        {
        iThreadParam->iStarted.Close();
        }
    delete iThreadParam;
    iThreadParam = NULL;
    if( iThread.Handle() != NULL )
        {
        iThread.Kill( KErrNone );
        iThread.Close();
        }    

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceThread

    Method: StartL

    Description: StartL method starts test interference.

    Parameters: TStifTestInterferenceType aType: in: Test interference type.
                TInt aIdleTime: in: Test interference idle time.
                TInt aActiveTime: in: Test interference active time.

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: Leaves if iThreadParam exists.
                       Leaves if thread creation fails.

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CSTIFInterferenceThread::StartL( TStifTestInterferenceType aType,
                                        TInt aIdleTime,
                                        TInt aActiveTime )
    {
    //__UHEAP_MARK;

    if( iThreadParam )
        {
        User::Leave( KErrAlreadyExists );
        }

    // Thread related parameters
    iThreadParam = new TThreadParam;
    iThreadParam->iStarted.CreateLocal( 0 );
    iThreadParam->iType = aType;
    iThreadParam->iIdleTime = aIdleTime;
    iThreadParam->iActiveTime = aActiveTime;

    // Temporary thread unique name
    _LIT( KThreadFirstName, "STIFInterference_%x" );
     
    TBuf<32> tmpThreadName;
    TInt uniqueCounter = 1;
    
    TInt ret = KErrNone;
    do
    	{
    	tmpThreadName.Format( KThreadFirstName, uniqueCounter );
    	// Create thread
    	ret = iThread.Create(
    		    					tmpThreadName,       // thread name
    		                        ThreadFunction,         // thread function
    		                        KDefaultStackSize*4,    // stack
    		                        KMinHeapSize,           // Heap, min
    		                        KMaxHeapSize*2,         // Heap, max
    		                        (TAny*) iThreadParam    // parameter to thread function
    		                        );
    	uniqueCounter++;
    	}
    while( ret == KErrAlreadyExists );
    
    // If thread creation failed
    if( ret != KErrNone )        
        {
        iThreadParam->iStarted.Close();         // Close semaphore
        delete iThreadParam;
        iThreadParam = NULL;
        //__UHEAP_MARKEND;
        User::Leave( ret );
        }

    // Add thread pointer to STIF side for cases where need to kill
    // thread e.g. if test case is crashed etc. before Stop(give by user).
    iTestModuleBase->iTestModuleIf->AddInterferenceThread( iThread );

    // Create unique thread name
    const TInt name = 17; // Name parts
    const TInt id = 8;    // Unique id parts
    _LIT( KThreadUniqueName, "STIFInterference_" );
    TBuf<name+id> threadUniqueName;
    threadUniqueName.Copy( KThreadUniqueName );
    // Appends id in hexadesimal format 
    threadUniqueName.AppendFormat(  _L( "%x" ), (TInt)iThread.Id() );
    //RDebug::Print(threadUniqueName);

    // Reneme thread with unique name
    iThread.RenameMe( threadUniqueName );

    // Thread is currently in suspend state

    // Now start thread
    iThread.SetPriority( EPriorityMuchMore ); // set its priority
    iThread.Resume();                         // kick it into life in
                                              // sometimes(depend on scheduler)

    // This block execution here and continue when signal is said in thread
    // execution side.
    // Wait until the thread is started
    iThreadParam->iStarted.Wait();

    //__UHEAP_MARKEND;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceThread

    Method: Stop

    Description:  Stop method stops test interference.

    Parameters: None

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CSTIFInterferenceThread::Stop()
    {
    // Ensures that remove(Stop()) sequence won't do two times for one thread.
    if( iThread.Handle() != NULL )
        {
        // Remove pointer from array
        iTestModuleBase->iTestModuleIf->RemoveInterferenceThread( iThread );
        }

    // If test case crash etc. do stop operations here also
    if( iThreadParam != NULL )
        {
        iThreadParam->iStarted.Close();
        }
    delete iThreadParam;
    iThreadParam = NULL;
    if( iThread.Handle() != NULL )
        {
        iThread.Kill( KErrNone );
        iThread.Close();
        }    

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceThread

    Method: SetPriority

    Description: Sets thread or active object priority. This should use before
                 test interference is started otherwise error code will return.

    Parameters: TInt aPriority: in: New priority for active object given by
                user.

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CSTIFInterferenceThread::SetPriority( TInt aPriority )
    {
    // RThread priority can set during interference executions time. User
    // should be sure that given value is acceptable, otherwise SetPriority
    // panics.

    RThread thisThread;
    thisThread.SetPriority ( (TThreadPriority) aPriority );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceThread

    Method: ThreadFunction

    Description: Implements thread code

    Parameters: TAny* aThreadArg: in : Thread related informations

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CSTIFInterferenceThread::ThreadFunction( TAny* aThreadArg )
        {
        // Thread code here

        // Create clean-up stack
        CTrapCleanup* tc = CTrapCleanup::New();

        // Take local copy of incoming parameters. 
        // This object is in stack -> no manual deletion
        TThreadParam params = *(TThreadParam*)aThreadArg;
        
        // Signal to continue from CSTIFInterferenceThread::StartL
        params.iStarted.Signal();

        TInt ret = KErrNone;

        // Construct and install active scheduler
        CActiveScheduler* activeScheduler = new CActiveScheduler;
        CActiveScheduler::Install( activeScheduler );

        TRAP( ret, ExecuteInterferenceL( params.iType, params.iIdleTime, params.iActiveTime ) );

    	User::LeaveIfError( ret );
        
        delete activeScheduler;
        
        // Delete clean-up stack
        delete tc;
        tc = NULL;

        return KErrNone;
        }

/*
-------------------------------------------------------------------------------

    Class: CSTIFInterferenceThread

    Method: ExecuteInterferenceL

    Description: Executes interference.

    Parameters: aType		Interference type.
    			aIdleTime	Idle time.
    			aActiveTime Active time.

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CSTIFInterferenceThread::ExecuteInterferenceL( TStifTestInterferenceType aType, TInt aIdleTime, TInt aActiveTime )
	{
	CSTIFInterferenceAO* interferenceAO = CSTIFInterferenceAO::NewL();
	CleanupStack::PushL( interferenceAO );

	interferenceAO->StartL( aType, aIdleTime, aActiveTime );

	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy( interferenceAO );
	}

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CExecuteInterference class 
    member functions.

-------------------------------------------------------------------------------
*/


// ============================ MEMBER FUNCTIONS ===============================

/*
-------------------------------------------------------------------------------

    Class: CExecuteInterference

    Method: CExecuteInterference

    Description: C++ default constructor can NOT contain any code, that
                 might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CExecuteInterference::CExecuteInterference()
    {
    iTimer.CreateLocal();

    }

/*
-------------------------------------------------------------------------------

    Class: CExecuteInterference

    Method: ConstructL

    Description: C++ default constructor can NOT contain any code, that
                 might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CExecuteInterference::ConstructL()
    {
    // None

    }

/*
-------------------------------------------------------------------------------

    Class: CExecuteInterference

    Method: NewL

    Description: Two-phased constructor.

    Parameters: None

    Return Values: CExecuteInterference*: pointer to CExecuteInterference
                   object

    Errors/Exceptions: Leaves if object creation fails.

    Status: Approved

-------------------------------------------------------------------------------
*/
CExecuteInterference* CExecuteInterference::NewL()
    {
    CExecuteInterference* self = new (ELeave) CExecuteInterference();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CExecuteInterference

    Method: ~CExecuteInterference

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CExecuteInterference::~CExecuteInterference()
    {
    iTimer.Cancel();
    iTimer.Close();

    }

/*
-------------------------------------------------------------------------------

    Class: CExecuteInterference

    Method: Interference

    Description: Start implement the test interference according to aType
                 parameter.

    Parameters: MSTIFTestInterference::TStifTestInterferenceType aType: in:
                    Test interference type.
                TInt aActiveTime: in: Test interference active time

    Return Values: TInt: Symbian error code

    Errors/Exceptions: Leaves if CleanupClosePushL leave

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CExecuteInterference::InterferenceL( 
                    MSTIFTestInterference::TStifTestInterferenceType aType,
                    TInt aActiveTime )
    {
    TTimeIntervalMicroSeconds32 myActiveTime( aActiveTime );    
    
    TTime endTime;
    endTime.HomeTime();           
    endTime = endTime + myActiveTime;   
        
    TFileName myFile;    
        
    switch( aType )
        {
        case MSTIFTestInterference::ECpuLoad:
            {
            return CPULoad( myActiveTime );
            }
        case MSTIFTestInterference::EFileSystemReadC:
            {           
            RDebug::Print( _L( "STIFTestInterference::EFileSystemReadC" ) );
            
            _LIT( KDrive, "c:\\" );   
            myFile.Format( KDrive );
            
            RFs fileserver;            
            TInt err = fileserver.Connect(); 
            if( err != KErrNone )
                {
                RDebug::Print( _L( "RFs connection failed with error: %d" ), err );
                return err;
                }          
            CleanupClosePushL( fileserver );                                                                        

            err = fileserver.SetSessionPath( myFile );
            if( err != KErrNone )
                {
                RDebug::Print( _L( "Drive C is not ready!" ) );                                
                // R Classes have not created with New, no delete needed
                CleanupStack::Pop( &fileserver );
                fileserver.Close();
                return KErrNone;
                }                           
                
            RDebug::Print( _L( "SearchAFileForReading" ) );                  
            err = SearchAFileForReading( myFile, fileserver );                        
            // R Classes have not created with New, no delete needed
            CleanupStack::Pop( &fileserver );
            fileserver.Close();
            if( err != KErrNone )
                {
                RDebug::Print( _L( "File not found!" ) );                  
                return KErrNone;
                }                            

            RDebug::Print( _L( "File's name is: %S" ), &myFile );                     
                        
            return RepeatReadFromFileL( myFile, myActiveTime );
            }
        case MSTIFTestInterference::EFileSystemReadD:
            {
            RDebug::Print( _L( "STIFTestInterference::EFileSystemReadD" ) );            
            _LIT( KDrive, "D\x3a\\" );   
            myFile.Format( KDrive );
            
            RFs fileserver;            
            TInt err = fileserver.Connect(); 
            if( err != KErrNone )
                {
                RDebug::Print( _L( "RFs connection failed with error: %d" ), err );
                return err;
                }          
            CleanupClosePushL( fileserver );                                                                        
            err = fileserver.SetSessionPath( myFile );
            if( err != KErrNone )
                {
                RDebug::Print( _L( "Drive D is not ready!" ) );                                
                // R Classes have not created with New, no delete needed
                CleanupStack::Pop( &fileserver );
                fileserver.Close();
                return KErrNone;
                }                           
            RDebug::Print( _L( "Fileserver created" ) );                  
                        
            err = SearchAFileForReading( myFile, fileserver );                        
            // R Classes have not created with New, no delete needed
            CleanupStack::Pop( &fileserver );
            fileserver.Close();
            if( err != KErrNone )
                {
                RDebug::Print( _L( "File not found!" ) );                  
                return KErrNone;
                }                                                  
            return RepeatReadFromFileL( myFile, myActiveTime );
            }
        case MSTIFTestInterference::EFileSystemReadE:
            {
            RDebug::Print( _L( "STIFTestInterference::EFileSystemReadE" ) );            
            _LIT( KDrive, "E\x3a\\" );   
            myFile.Format( KDrive );
           
            RFs fileserver;            
            TInt err = fileserver.Connect(); 
            if( err != KErrNone )
                {
                RDebug::Print( _L( "RFs connection failed with error: %d" ), err );
                return err;
                }          
            CleanupClosePushL( fileserver );                                                                        
            RDebug::Print( _L( "Fileserver created" ) );                  
            err = fileserver.SetSessionPath( myFile );
            if( err != KErrNone )
                {
                RDebug::Print( _L( "Drive E is not ready!" ) );
                // R Classes have not created with New, no delete needed
                CleanupStack::Pop( &fileserver );
                fileserver.Close();
                return KErrNone;
                }                           
            
            err = SearchAFileForReading( myFile, fileserver );                        
            // R Classes have not created with New, no delete needed
            CleanupStack::Pop( &fileserver );
            fileserver.Close();
            if( err != KErrNone )
                {
                RDebug::Print( _L( "File not found!" ) );                  
                return KErrNone;
                }                                                  
            
            return RepeatReadFromFileL( myFile, myActiveTime );
            }
        case MSTIFTestInterference::EFileSystemReadZ:
            {
            RDebug::Print( _L( "STIFTestInterference::EFileSystemReadZ" ) );            
            _LIT( KDrive, "Z:\\" );   
            myFile.Format( KDrive );

            RFs fileserver;            
            TInt err = fileserver.Connect(); 
            if( err != KErrNone )
                {
                RDebug::Print( _L( "RFs connection failed with error: %d" ), err );
                return err;
                }          
            CleanupClosePushL( fileserver );                                                        
            RDebug::Print( _L( "Fileserver created" ) );                  
            err = fileserver.SetSessionPath( myFile );
            if( err != KErrNone )
                {
                RDebug::Print( _L( "Drive Z is not ready!" ) );                                
                // R Classes have not created with New, no delete needed
                CleanupStack::Pop( &fileserver );
                fileserver.Close();
                return KErrNone;
                }                           

            err = SearchAFileForReading( myFile, fileserver );                        
            // R Classes have not created with New, no delete needed
            CleanupStack::Pop( &fileserver );
            fileserver.Close();
            if( err != KErrNone )
                {
                RDebug::Print( _L( "File not found!" ) );                  
                return KErrNone;
                }                                                  
            
            return RepeatReadFromFileL( myFile, myActiveTime );
            }           
        case MSTIFTestInterference::EFileSystemWriteC:
            {            
            RDebug::Print( _L( "STIFTestInterference::EFileSystemWriteC" ) );            
            _LIT( KDrive, "C:\\" );
            _LIT( KFileName,  "FileSystemWriteC.txt" );                                    
            TBufC<20> myFileName( KFileName );            
            myFile.Format( KDrive );

            RFs fileserver;            
            TInt err = fileserver.Connect(); 
            if( err != KErrNone )
                {
                RDebug::Print( _L( "RFs connection failed with error: %d" ), err );
                return err;
                }          
            CleanupClosePushL( fileserver );                                                        
            RDebug::Print( _L( "Fileserver created" ) );                  
                        
            err = fileserver.SetSessionPath( myFile );                       
            // R Classes have not created with New, no delete needed
            CleanupStack::Pop( &fileserver );
            fileserver.Close();
            if( err != KErrNone )
                {
                RDebug::Print( _L( "Drive C is not ready!" ) );
                return KErrNone;
                }
            
            PrepareFileSystemL( myFile, myFileName );
            myFile.Append( myFileName );
            return RepeatWriteToFileL( myFile, endTime );
            }
        case MSTIFTestInterference::EFileSystemWriteD:
            {
            RDebug::Print( _L( "STIFTestInterference::EFileSystemWriteD" ) );            
            _LIT( KDrive, "D\x3a\\" );
            _LIT( KFileName,  "FileSystemWriteD.txt" );                                    
            TBufC<20> myFileName( KFileName );            
            myFile.Format( KDrive );

            RFs fileserver;            
            TInt err = fileserver.Connect(); 
            if( err != KErrNone )
                {
                RDebug::Print( _L( "RFs connection failed with error: %d" ), err );
                return err;
                }          
            CleanupClosePushL( fileserver );                                            
            RDebug::Print( _L( "Fileserver created" ) );                  
            err = fileserver.SetSessionPath( myFile );                       
                        
            // R Classes have not created with New, no delete needed
            CleanupStack::Pop( &fileserver );
            fileserver.Close();
            if( err != KErrNone )
                {
                RDebug::Print( _L( "Drive D is not ready!" ) );
                return KErrNone;
                }
                            
            PrepareFileSystemL( myFile, myFileName );
            myFile.Append( myFileName );
            return RepeatWriteToFileL( myFile, endTime );
            }
        case MSTIFTestInterference::EFileSystemWriteE:
            {            
            RDebug::Print( _L( "STIFTestInterference::EFileSystemWriteE" ) );            
            _LIT( KDrive, "E\x3a\\" );
            _LIT( KFileName,  "FileSystemWriteE.txt" );
            TBufC<20> myFileName( KFileName );            
            myFile.Format( KDrive );
            
            RFs fileserver;            
            TInt err = fileserver.Connect(); 
            if( err != KErrNone )
                {
                RDebug::Print( _L( "RFs connection failed with error: %d" ), err );
                return err;
                }          
            CleanupClosePushL( fileserver );                                
            RDebug::Print( _L( "Fileserver created" ) );                  
                        
            err = fileserver.SetSessionPath( myFile );                       
            // R Classes have not created with New, no delete needed
            CleanupStack::Pop( &fileserver );
            fileserver.Close();
            if( err != KErrNone )
                {
                RDebug::Print( _L( "Drive E is not ready!" ) );
                return KErrNone;
                }            
            PrepareFileSystemL( myFile, myFileName );
            myFile.Append( myFileName );

            return RepeatWriteToFileL( myFile, endTime );
            }
        case MSTIFTestInterference::EFileSystemFillAndEmptyC:
            {                       
            RDebug::Print( _L( "STIFTestInterference::EFileSystemFillAndEmptyC" ) );            
            _LIT( KDrive, "C:\\" );
            _LIT( KFileName,  "FillAndEmptyFile.txt" );                      
            TBufC<20> myFileName( KFileName );            
            myFile.Format( KDrive );
            
            RFs fileserver;            
            TInt err = fileserver.Connect();             
            if( err != KErrNone )
                {
                RDebug::Print( _L( "RFs connection failed with error: %d" ), err );
                return err;
                }          
            CleanupClosePushL( fileserver );                    
            RDebug::Print( _L( "Fileserver created" ) );                  
                        
            err = fileserver.SetSessionPath( myFile );                       
            // R Classes have not created with New, no delete needed
            CleanupStack::Pop( &fileserver );
            fileserver.Close();
            if( err != KErrNone )
                {
                RDebug::Print( _L( "Drive C is not ready!" ) );
                return KErrNone;
                }                
            PrepareFileSystemL( myFile, myFileName );
            myFile.Append( myFileName );

            RepeatWriteToFileL( myFile, endTime );                        
            EmptyTheFileSystemL( myFile, endTime );
            
            return KErrNone;
            }
        case MSTIFTestInterference::EFileSystemFillAndEmptyD:
            {
            RDebug::Print( _L( "STIFTestInterference::EFileSystemFillAndEmptyD" ) );            
            _LIT( KDrive, "D\x3a\\" );
            _LIT( KFileName,  "FillAndEmptyFile.txt" );                      
            TBufC<20> myFileName( KFileName );            
            myFile.Format( KDrive );
            
            RFs fileserver;            
            TInt err = fileserver.Connect(); 
            if( err != KErrNone )
                {
                RDebug::Print( _L( "RFs connection failed with error: %d" ), err );
                return err;
                }      
            CleanupClosePushL( fileserver );                    
            RDebug::Print( _L( "Fileserver created" ) );                  
                        
            err = fileserver.SetSessionPath( myFile );                       
            // R Classes have not created with New, no delete needed
            CleanupStack::Pop( &fileserver );
            fileserver.Close();
            if( err != KErrNone )
                {
                RDebug::Print( _L( "Drive D is not ready!" ) );
                return KErrNone;
                }                
            PrepareFileSystemL( myFile, myFileName );
            myFile.Append( myFileName );

            RepeatWriteToFileL( myFile, endTime );                        
            EmptyTheFileSystemL( myFile, endTime );
            
            return KErrNone;
            }
        case MSTIFTestInterference::EFileSystemFillAndEmptyE:
            {
            RDebug::Print( _L( "STIFTestInterference::EFileSystemFillAndEmptyE" ) );            
            _LIT( KDrive, "E\x3a\\" );
            _LIT( KFileName,  "FillAndEmptyFile.txt" );                      
            TBufC<20> myFileName( KFileName );            
            myFile.Format( KDrive );
            
            RFs fileserver;            
            TInt err = fileserver.Connect(); 
            if( err != KErrNone )
                {
                RDebug::Print( _L( "RFs connection failed with error: %d" ), err );
                return err;
                }          
            CleanupClosePushL( fileserver );
            RDebug::Print( _L( "Fileserver created" ) );                  
                        
            err = fileserver.SetSessionPath( myFile );                       
            // R Classes have not created with New, no delete needed
            CleanupStack::Pop( &fileserver );
            fileserver.Close();

            if( err != KErrNone )
                {
                RDebug::Print( _L( "Drive E is not ready!" ) );
                return KErrNone;
                }                
            PrepareFileSystemL( myFile, myFileName );
            myFile.Append( myFileName );

            RepeatWriteToFileL( myFile, endTime );                        
            EmptyTheFileSystemL( myFile, endTime );
            
            return KErrNone;
            }       
        default:
            {
            return KErrNone;
            }
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CExecuteInterference

    Method: SearchAFileForReading

    Description: Searches a file from given directory

    Parameters: TDes &aPath, TDes &aFileserver

    Return Values: TInt Symbian error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CExecuteInterference::SearchAFileForReading( TDes &aPath, RFs &aFileserver )
    {
    RDebug::Print( _L( "CExecuteInterference::SearchAFileForReading" ) );    
    
    CDir * filelist;
    CDir * dirlist;
    TInt ret = KErrNotFound;       
    aFileserver.SetSessionPath( aPath );        
    
    aFileserver.GetDir( aPath, KEntryAttNormal, ESortByName, filelist, dirlist );        
    
    if( !filelist || !dirlist )
        {
        return KErrArgument;
        }

    if( filelist->Count() > 0 )
        {
        RDebug::Print( _L( "Filelist > 0" ) );    
            
        // File found! 
        
        // Check file size and accept it only if the size is ~10kb
        // at miminum, or something like that
        aPath.Append( (*filelist)[0].iName );                        
        ret = KErrNone;
        
        RDebug::Print( _L( "File name: %S" ), &(*filelist)[0].iName );        
        }
    else
        {
        RDebug::Print( _L( "Filelist < 0, lets check other dirs" ) );            
        TFileName tmp;
        tmp.Append( aPath );
        for( TInt x=0; x<dirlist->Count(); x++ )
            {            
            aPath.Format( tmp );
            aPath.Append( (*dirlist)[0].iName );
            aPath.Append( _L("\\") );
            ret = SearchAFileForReading( aPath, aFileserver );
            if( ret == KErrNone )
                {
                break;
                }
            }
        }
    
    delete filelist;
    delete dirlist;    
       
    return ret;        
    }

/*
-------------------------------------------------------------------------------

    Class: CExecuteInterference

    Method: PrepareFileSystemLL

    Description: Creates directory and file for test cases, if necessary

    Parameters: TDes &aDriveName, TDes &aFileName

    Return Values: TInt Symbian error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CExecuteInterference::PrepareFileSystemL( TDesC &aDriveName,
                                              TDesC &aFileName )
    {
    // Note that aDriveName must be in format drivename:\\, e.g. "c:\\"
    
    RDebug::Print(_L("CExecuteInterference::PrepareFileSystemL"));
    TFileName tmp;
    tmp.Append( aDriveName );
    
    RFs fileserver;
    TInt err = fileserver.Connect(); 
    if( err != KErrNone )
        {
        RDebug::Print( _L( "RFs connection failed with error: %d" ), err );
        return err;
        }      
    CleanupClosePushL( fileserver );                    
            
    _LIT( KSTIFDirName, "STIFInterference\\" );                                          

    err = fileserver.SetSessionPath( tmp );                           
    if( err != KErrNone )
        {
        CleanupStack::Pop( &fileserver );
        fileserver.Close();
        return err;
        }
    
    tmp.Append( KSTIFDirName );
    err = fileserver.MkDir( tmp );
    if( err != KErrNone && err != KErrAlreadyExists )
        {
        RDebug::Print( _L( "MkDir failed with error: %d" ), err );
        // R Classes have not created with New, no delete needed
        CleanupStack::Pop( &fileserver );
        fileserver.Close();
        return err;
        }              
    
    tmp.Format( aDriveName );
    tmp.Append( KSTIFDirName );        
    fileserver.SetSessionPath( tmp );                               

    RFile newFile;
    CleanupClosePushL( newFile );                    
    TInt ret = newFile.Create( fileserver, aFileName, EFileWrite );
    if( ret != KErrNone && ret != KErrAlreadyExists )
        {
        RDebug::Print( _L( "File create failed with error: %d" ), err );            
        }
        
    // R Classes have not created with New, no delete needed
    CleanupStack::Pop( &newFile );
    CleanupStack::Pop( &fileserver );
    newFile.Close();
    fileserver.Close();
    
    return ret;      
    }

/*
-------------------------------------------------------------------------------

    Class: CExecuteInterference

    Method: CPULoad

    Description: Creates CPU load to the system

    Parameters: const TTimeIntervalMicroSeconds32 &aActiveTime

    Return Values: TInt Symbian error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CExecuteInterference::CPULoad( const TTimeIntervalMicroSeconds32 &aActiveTime )
    {
    RDebug::Print(_L("CExecuteInterference::CPULoad"));

    TTime endTime;
    TTime currentTime;

    currentTime.HomeTime();
    endTime.HomeTime();

    endTime = endTime + aActiveTime;  

    while ( currentTime < endTime )
       {
        currentTime.HomeTime();    
       }

    return KErrNone;    
    }

/*
-------------------------------------------------------------------------------

    Class: CExecuteInterference

    Method: DeleteFilesRecursivelyL

    Description: Delete files in current directory and its subdirectories

    Parameters: RFs &aFileserver, 
                TDes &aStartDirectory

    Return Values: TInt Symbian error code

    Errors/Exceptions: Leaves if GetDir fails

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CExecuteInterference::DeleteFilesRecursivelyL( RFs &aFileserver, 
                                                    TDes &aStartDirectory,
                                                    const TTime &aEndTime )
    { 
    TTime currentTime;
    currentTime.HomeTime();      
    if( currentTime >= aEndTime )
        {
        return KErrTimedOut;
        }
    
    RDebug::Print( _L( "CExecuteInterference::DeleteFilesRecursivelyL" ) );    
    
    TFileName currentDir;   
    CDir * filelist;
    CDir * dirlist;       
    
    aFileserver.SetSessionPath( aStartDirectory );    
    aFileserver.SessionPath( currentDir );
    RDebug::Print( _L( "Hakemistopolku = %S" ), &currentDir );       
    
    User::LeaveIfError( aFileserver.GetDir( 
            currentDir, KEntryAttNormal, ESortByName, filelist, dirlist ) );
               
    // Delete all files
    for( TInt x=0; x<filelist->Count(); x++ )
        {
        RDebug::Print( _L( "CExecuteInterference::DeleteFilesRecursivelyL: delete: %S" ), &(*filelist)[x].iName );                    
        aFileserver.Delete( (*filelist)[x].iName );    
        }
    
    // Looping all directories recursively
    for( TInt y= 0; y<dirlist->Count(); y++ )
        {        
        currentDir.Format( aStartDirectory );
        currentDir.Append( (*dirlist)[y].iName );
        currentDir.Append( _L("\\") );
        DeleteFilesRecursivelyL( aFileserver, currentDir, aEndTime );
        }    
     
    delete filelist;
    delete dirlist;

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CExecuteInterference

    Method: EmptyTheFileSystemL

    Description: Empty the file system using DeleteFilesRecursivelyL

    Parameters: TDes &aFilePath

    Return Values: TInt Symbian error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CExecuteInterference::EmptyTheFileSystemL( TDes &aFilePath, 
                                               const TTime &aEndTime )
    {
    TTime currentTime;
    currentTime.HomeTime();
    if( currentTime >= aEndTime )
        {
        // End of time
        return KErrTimedOut;
        }    
    
    RDebug::Print( _L( "CExecuteInterference::EmptyTheFileSystemL" ) );    
    RFs fileserver;
    TInt err = fileserver.Connect(); 
    if( err != KErrNone )
        {
        RDebug::Print( _L( "RFs connection failed with error: %d" ), err );
        return err;
        }      
    CleanupClosePushL( fileserver );                           
    TRAP( err, DeleteFilesRecursivelyL( fileserver, aFilePath, aEndTime ) );        
    if( err != KErrNone )
        {
        RDebug::Print( _L( "DeleteFilesRecursivelyL failed with error: %d" ), err );
        }    
    // R Classes have not created with New, no delete needed
    CleanupStack::Pop( &fileserver );   
    fileserver.Close();
    return err;
    }

/*
-------------------------------------------------------------------------------

    Class: CExecuteInterference

    Method: WriteToFileAsynchL

    Description: Writes data to a file asynchronously

    Parameters: RFs &aFileserver,
                const TTime &aEndTime 

    Return Values: TInt Symbian error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CExecuteInterference::WriteToFileAsynchL( TDes &aFilePath,
                                              RFs &aFileserver,
                                              const TTime &aEndTime )
    {
    RFile myFile;

    TInt err = myFile.Open( aFileserver, aFilePath, EFileWrite );
    if( err != KErrNone )
        {
        RDebug::Print( _L( "Open file failed with error: %d" ), err );
        return err;
        }    
    CleanupClosePushL( myFile );                      
    TInt position = 0;
    myFile.Seek( ESeekEnd, position );

    _LIT8( KMyBuffer, "STIF Interference module: WriteToFile");
    
    // NOTE: this part has previous asynch implementation, doesn't work
    // at the moment
    /* 
    TRequestStatus status;
    myFile.Write( position, KMyBuffer, status );    
    // Loop until either write operation is completed or the execution 
    // time has expired    
    while( doAlways ) 
        {
        if( status != KRequestPending )
            {
            return status.Int();
            }
        currentTime.HomeTime();                   
        if( currentTime >= aEndTime )
            {
            return KErrTimedOut;
            }
         
       }
    */            

    TTime currentTime;
    currentTime.HomeTime();
    
    while( currentTime <= aEndTime )
        {
        myFile.Write( position, KMyBuffer );
        currentTime.HomeTime();
        }
    // R Classes have not created with New, no delete needed
    CleanupStack::Pop( &myFile );          
    myFile.Close();
    return KErrNone;      
    }

/*
-------------------------------------------------------------------------------

    Class: CExecuteInterference

    Method: RepeatWriteToFileL

    Description: Repeats file writing using WriteToFileAsynchL method

    Parameters: const TTimeIntervalMicroSeconds32 &aActiveTime

    Return Values: TInt Symbian error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CExecuteInterference::RepeatWriteToFileL(
                    TDes &aFilePath, 
                    const TTime &aActiveTime )
    {
    RFs fileserver;
    TInt err = fileserver.Connect(); 
    if( err != KErrNone )
        {
        RDebug::Print( _L( "RFs connection failed with error: %d" ), err );
        return err;
        }      
    CleanupClosePushL( fileserver );                           
        
    TTime currentTime;
    currentTime.HomeTime();

    TVolumeInfo myVolume;
    fileserver.Volume( myVolume, EDriveC );   

    while( ( currentTime <= aActiveTime ) && ( myVolume.iFree > 0 ) )    
        {
        WriteToFileAsynchL( aFilePath, fileserver, aActiveTime );        
        currentTime.HomeTime();
        }
    
    // R Classes have not created with New, no delete needed
    CleanupStack::Pop( &fileserver );        
    fileserver.Close();
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CExecuteInterference

    Method: RepeatReadFromFileL

    Description: Repeats file reading using ReadFromFileAsynch method

    Parameters: const TTimeIntervalMicroSeconds32 &aActiveTime

    Return Values: TInt Symbian error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CExecuteInterference::RepeatReadFromFileL( 
                               TDes &aFilePath,
                               const TTimeIntervalMicroSeconds32 &aActiveTime )
    {
    RDebug::Print( _L( "CExecuteInterference::RepeatReadFromFileL" ) );
    
    RFs fileserver;
    TInt err = fileserver.Connect(); 
    if( err != KErrNone )
        {
        RDebug::Print( _L( "RFs connection failed with error: %d" ), err );
        return err;
        }      
    CleanupClosePushL( fileserver );                           
    
    TTime endTime;
    TTime currentTime;
    RFile myFile;
    TBuf8<20> myReadBuffer;

    RDebug::Print( _L( "RepeatReadFromFileL: Open" ) );                  

    // Open file
    err = myFile.Open( fileserver, aFilePath, EFileWrite );
    if( err != KErrNone )
        {
        RDebug::Print( _L( "Open file failed with error: %d" ), err );
        // R Classes have not created with New, no delete needed
        CleanupStack::Pop( &fileserver );
        fileserver.Close();
        return err;
        }        
            
    CleanupClosePushL( myFile );                                       
    // This part has previous async implementation, doesn't work at moment
    /*   
    RDebug::Print( _L( "ReadFromFileAsynch: trequeststatus" ) );                        
           
    TRequestStatus timer;
    TRequestStatus read;

    RDebug::Print( _L( "ReadFromFileAsynch: asynk timer" ) );                    

    RTimer clock;
    clock.CreateLocal();
    clock.At( timer, endTime);

    RDebug::Print( _L( "ReadFromFileAsynch: asynk read" ) );
    myFile.Read( myReadBuffer, 20, read );

    RDebug::Print( _L( "ReadFromFileAsynch: ennen whilelooppia" ) );
    while( timer == KRequestPending )
        {
        User::WaitForRequest( read, timer );
        if( read != KRequestPending)
            {
            RDebug::Print( _L( "ReadFromFileAsynch: uudelleen asynk read" ) );
            read = KRequestPending;                              
    	    myFile.Read( myReadBuffer, 20, read );    	    
            }
        else
            {  
            RDebug::Print( _L( "ReadFromFileAsynch: cancel luku, timer kompletoitui" ) );                  
            // Timer valmis         
            //read = KRequestPending;   
            myFile.ReadCancel( read );
            User::WaitForRequest( read );
            break;
            }
        }      
    */

    currentTime.HomeTime();
    endTime.HomeTime();
    endTime = endTime + aActiveTime;
    
    while( currentTime <= endTime )
        {
        myFile.Read( myReadBuffer );
        currentTime.HomeTime();    	    
        }
        
    //myFile.Close();
    //fileserver.Close();
    // R Classes have not created with New, no delete needed
    CleanupStack::Pop( &myFile );
    CleanupStack::Pop( &fileserver );
    myFile.Close();
    fileserver.Close();
    //CleanupStack::PopAndDestroy( 2 );       
   
    RDebug::Print( _L( "CExecuteInterference::RepeatReadFromFileL success" ) );
    return KErrNone;    
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

// End of File
