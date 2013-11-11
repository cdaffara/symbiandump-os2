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
* Description: This file contains TestInterface implementation.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32std.h>
#include "StifTestInterface.h"
#include "TestServerModuleIf.h"
#include "StifTestModule.h"
#include <stifinternal/TestServerClient.h>
//#include <bautils.h>
#include "version.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None
/*
-------------------------------------------------------------------------------

    Class: -

    Method: StifDriveLetters

    Description: This defines drive letters that is available in STIF TF.

    Parameters: TInt aDriveNumber: in: Number to indicate drive letter
                TInt& aCount: inout: Counts of drives that is available

    Return Values: const TChar: Returns a drive letter that is select by
                   aDriveNumber

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
//@spe
/*
EXPORT_C const TChar StifDriveLetters( TInt aDriveNumber, TInt& aCount )
    {
    static TChar* const EStifDriveLetters[] =
        {
        ( TChar* )L"a",
        ( TChar* )L"b",
        ( TChar* )L"c",
        ( TChar* )L"d",
        ( TChar* )L"e",
        ( TChar* )L"f",
        ( TChar* )L"g",
        ( TChar* )L"h",
        ( TChar* )L"i",
        ( TChar* )L"j",
        ( TChar* )L"k",
        ( TChar* )L"l",
        ( TChar* )L"m",
        ( TChar* )L"n",
        ( TChar* )L"o",
        ( TChar* )L"p",
        ( TChar* )L"q",
        ( TChar* )L"r",
        ( TChar* )L"s",
        ( TChar* )L"t",
        ( TChar* )L"u",
        ( TChar* )L"v",
        ( TChar* )L"w",
        ( TChar* )L"x",
        ( TChar* )L"y",
        ( TChar* )L"z",
        };
    aCount = (sizeof( EStifDriveLetters )/sizeof(TChar*));
    return TChar( *EStifDriveLetters[aDriveNumber] );

    }
*/

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================
// None

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of TTestCaseInfo class 
    member function.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: TTestCaseInfo

    Method: TTestCaseInfo

    Description: This method is the constructor of class TTestCaseInfo.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C TTestCaseInfo::TTestCaseInfo() 
    {
    iCaseNumber = -1;
    iTitle.Zero();
    iTimeout = 0;
    iPriority = EPriorityNormal;
    
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of TTestInfo class 
    member function.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: TTestInfo

    Method: TTestInfo

    Description: This method is the constructor of class TTestInfo.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C TTestInfo::TTestInfo()
    {

    iModuleName.Zero();
    iConfig.Zero();
    
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of TTestResult class 
    member function.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: TTestResult

    Method: TTestResult

    Description: This method is the constructor of class TTestResult.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C TTestResult::TTestResult()
    {
    iResult = 0;
    iResultDes.Zero();
    
    }

/*
-------------------------------------------------------------------------------

    Class: TTestResult

    Method: TTestResult

    Description: Sets result and result description of the test case.
    
    Parameters: TInt aResultCode: in: Symbian error code
                const TResultDes& aResultDes: in: Result description of the
                test case
    
    Return Values: None
    
    Errors/Exceptions: None
    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C void TTestResult::SetResult( TInt aResultCode,
                                        const TDesC& aResultDes )
    {
    iResult = aResultCode;
    // If description is too length then cut.
    if( aResultDes.Length() > KStifMaxResultDes )
        {
        iResultDes = aResultDes.Left( KStifMaxResultDes );
        }
    else
        {
        iResultDes = aResultDes;
        }

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of TFullTestResult class 
    member function.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: TFullTestResult

    Method: TFullTestResult

    Description: This method is the constructor of class TFullTestResult.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C TFullTestResult::TFullTestResult():
    iStartTime( 0 ),
    iEndTime( 0 )
    {    
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of TTestProgress class 
    member function.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: TTestProgress

    Method: TTestProgress

    Description: This method is the constructor of class TTestProgress.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C TTestProgress::TTestProgress()
    {
    iPosition = 0;
    iDescription.Zero();
    iText.Zero();

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of TErrorNotification class 
    member function.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: TErrorNotification

    Method: TErrorNotification

    Description: This method is the constructor of class TTestProgress.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C TErrorNotification::TErrorNotification()
    {
    iPriority = 0;
    iModule.Zero();
    iText.Zero();

    }

/*
-------------------------------------------------------------------------------

    Class: -

    Method: StartSession

    Description: This works and used only in EKA2 environment and used from
                 STIF TestFramework internally.
                 Executable module uses defined capabilities(PlatSec's
                 Capability model) to start session. Function for starting
                 the TestServer and Testmodule/TestClass.
    
                 Starts a new server. Server will be running its own
                 thread and this functions returns when server is up and
                 running or server start-up fails.

    Parameters: None
    
    Return Values: TInt Error code / KErrNone

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt StartSession()
    {
    //__UHEAP_MARK;

    // Get module name from command line
	const TInt length = User::CommandLineLength();
    HBufC* cmdLine = HBufC::New( length );
    
    if ( cmdLine == NULL )
        {
        //__UHEAP_MARKEND;
        return KErrNoMemory;
        }

    TPtr moduleName = cmdLine->Des();
    User::CommandLine( moduleName );

    RDebug::Print(_L("StartSession() Received data [%S]"), &moduleName);

    // Extract semaphore name passed in data    
    TInt index = moduleName.Find(_L(" "));
    RDebug::Print(_L("StartSession() Space separator found at position [%d]"), index);
    TPtrC semaphoreName = moduleName.Mid(index + 1);
    moduleName = moduleName.Left(index);

    RDebug::Print(_L("StartSession() Extracted module name [%S] and sempahore name [%S]"), &moduleName, &semaphoreName);
   
    // Open start-up synchronization semaphore
    RSemaphore startup;
    RDebug::Print(_L(" Opening start-up semaphore"));
//    TName semaphoreName = _L("startupSemaphore");
//    semaphoreName.Append( moduleName );
    
    TInt res = startup.OpenGlobal(semaphoreName);
    RDebug::Print(_L("Opening result %d"), res);    


    TFileName serverName;
    TInt r = StartNewServer ( moduleName, serverName, EFalse, startup);    

    if ( r ==   KErrAlreadyExists )
        {        
        // Ok, server was already started
        RDebug::Print(_L("Server already started, signaling semaphore and exiting"));
        startup.Signal();        
        //__UHEAP_MARKEND;
        
        delete cmdLine;
        
        return KErrNone;
        }
    else
        {       
        RDebug::Print(_L("Server is finished, code %d"), r);
        }

    //__UHEAP_MARKEND;

	// Program execution never comes here, because StartNewServer doesn't return
	// unless server is already started.
	
    delete cmdLine;
    
    // Return start-up result.
    return r;

    }

/*
-------------------------------------------------------------------------------

    Class: TStifUtil

    Method: CorrectFilePath

    Description: Checks if file path contains drive letter. If not file is serched
                 on all drives and first hit is added to file name.

    Parameters: TDes& aFilePath: in/out: file path to correct

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves

-------------------------------------------------------------------------------
*/
EXPORT_C void TStifUtil::CorrectFilePathL( TDes& aFilePath )
	{
	_LIT( KDriveSelector, ":\\" );
	_LIT( KDriveSelectorFormat_1, "%c:" );                                                                  
	_LIT( KDriveSelectorFormat_2, "%c:\\" );                                                                
	TChar KDriveZ = EDriveZ;//'Z';                                                                          
	                                                                                                              
	_LIT( KBackslash, "\\" );                                                                              
	                                                                                                                
	TInt length = aFilePath.Length();                                                                      
	                                                                                                                
	if (length == 0 )                                                                                     
	   {                                                                                                  
	   return;                                                                                            
	   }                                                                                                  
	if (length > 2 )                                                                                      
	   {                                                                                                  
	   // Check if file path contains drive selector                                                      
	   if ( aFilePath.Mid( 1, 2 ) == KDriveSelector )                                                     
	       {                                                                                               
	       // File path contains selector letter so we have nothing to do here                             
	       return;                                                                                         
	       }                                                                                               
	   }                                                                                                  
	                                                                                                                
	// Check if file path contains backslash at the begining and                                          
	// select proper drive selector format according to this information                                  
	TInt driveSelectorFormat = 2;                                                                         
	if ( aFilePath.Mid( 0, 1 ) == KBackslash )                                                            
	   {                                                                                               
	   driveSelectorFormat = 1;                                                                        
	   }                                                                                               
	                                                                                                                
	RFs rfs;                                                                                              
	if ( rfs.Connect() != KErrNone )                                                                      
	   {                                                                                                  
	   return;                                                                                            
	   }                                                                                                  
	                                                                                                                
	// Get available drives list, revers it order and move z drive at                                     
	// the end of the list.  
	TDriveList drivesList; 
	rfs.DriveList(drivesList); 

	// Set drive variable to last drive (except for Z, which will be checked at the end)
	char drive = 'Y' ;

	// Loop through all the drives in following order: YX..CBAZ
	while(drive >= 'A' && drive <= 'Z')
	     {
	     // Do further action only if drive exists
	     TInt driveint;
	     rfs.CharToDrive(drive, driveint);
	     if(drivesList[driveint])
	          {
	          //further checking (drive selector and file existence)
	          
	          // Prepare drive selector                                                                         
	          TBuf<3> driveSelector;                                                                            
	          if ( driveSelectorFormat == 1 )                                                                   
	              {                                                                                           
	              driveSelector.Format( KDriveSelectorFormat_1, drive );                                    
	              }                                                                                           
	          else if ( driveSelectorFormat == 2 )                                                              
	              {                                                                                           
	              driveSelector.Format( KDriveSelectorFormat_2, drive );                                    
	              }                                                                                           
	                                                                                                                
	          aFilePath.Insert( 0, driveSelector );                                                             
	                                                                                                                
	          TEntry entry;                                                                                     
	          if ( rfs.Entry(aFilePath, entry) == KErrNone )                                                    
	              {                                                                                         
	              rfs.Close();                                                                                
	              return;                                                                                     
	              }                                                                                           
	                                                                                                                
	          // File does not exists on selected drive. Restoring orginal file path                            
	          aFilePath.Delete( 0, driveSelector.Length() );    	         
	          }//if(drivesList[driveint])       
	            
	   // Select next drive
	   if(drive == 'Z')
	       break; // the last driver
	   else if(drive ==  'A' )
	       drive = 'Z'; //after checking A check Z
	   else
	       drive =  (TChar)(TInt(drive)-1) ; //after checking Y check X and so on in reverse alphabetical order
	   } //while 
	rfs.Close(); 
	}

/*
-------------------------------------------------------------------------------

    Class: TStifUtil

    Method: STIFVersion

    Description: Method used to retrieve version of STIF by both STIF and STIF UI 

    Parameters: TInt& aMajorV - parameter used to pass major STIF version
     			TInt& aMinorV - parameter used to pass minor STIF version
     			TInt& aBuildV - parameter used to pass build version of STIF
     			TDes& aRelDate - parameter used to pass information about release date

    Return Values: None

-------------------------------------------------------------------------------
*/
EXPORT_C void TStifUtil::STIFVersion(TInt& aMajorV, TInt& aMinorV, TInt& aBuildV, TDes& aRelDate)
	{
	aMajorV = STIF_MAJOR_VERSION;
	aMinorV = STIF_MINOR_VERSION;
	aBuildV = STIF_BUILD_VERSION;
	aRelDate = TO_UNICODE(STIF_REL_DATE);
	}

// End of File
