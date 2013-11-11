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
* Description:  Definitions for the class CustomUser.
*
*/

#include <f32file.h>
#include <utf.h>
#include "customuser.h"
#include "analyzetoolmainallocator.h"
#include "analyzetoolallocator.h"
#include "atlog.h"
#include "analyzetoolmemoryallocator.h"
#include "analyzetoolpanics.pan"
#include "atdriveinfo.h"
#include <analyzetool/analyzetooltraceconstants.h>
#include "analyzetoolfastlog.h"

#ifdef USE_CLEANER_DLL
// Global variable to detect dll attach & detach in process.
// Note! This is initialized after SetupThreadHeap so its not usable there.
// This is used to store the main thread id and track when the process ends
// to load the cleaner dll with call back feature to cleanup allocator at the
// last possible phase.
#include <analyzetool/analyzetoolcleaner.h>

// CONSTANTS
const TInt KAToolCleanerOrdinal = 1;

class TAnalyzeToolGlobalTracker : public TAnalyzeToolCleanerBase
    {
public:
    /* Main thread id */
    TThreadId iMainId;
    
    /* Inform if panic occured */
    TBool iPanic;
    
    // -----------------------------------------------------------------------------
    // TAnalyzeToolGlobalTracker::TAnalyzeToolGlobalTracker()
    // C++ default constructor 
    // -----------------------------------------------------------------------------
    //
    TAnalyzeToolGlobalTracker()
        {
        LOGSTR1( "ATMH TAnalyzeToolGlobalTracker::TAnalyzeToolGlobalTracker()" );
        
        iPanic = EFalse; // no panic occured
        iMainId = RThread().Id(); // set main thread id
        LOGSTR2( "ATMH TAnalyzeToolGlobalTracker() > Main id set: %d", 
                iMainId.operator TUint() );
        }
    
    // -----------------------------------------------------------------------------
    // TAnalyzeToolGlobalTracker::~TAnalyzeToolGlobalTracker()
    // Destructor.
    // -----------------------------------------------------------------------------
    //
    ~TAnalyzeToolGlobalTracker()
        {
        LOGSTR1( "ATMH TAnalyzeToolGlobalTracker::~TAnalyzeToolGlobalTracker()" );
        
        // We dont load dll if panic has happened (uninstallation has been done).
        if ( iPanic )
            {
            LOGSTR1( "ATMH ~TAnalyzeToolGlobalTracker > Panic set not loading cleaner dll." );
            return;
            }
        
        LOGSTR1( "ATMH ~TAnalyzeToolGlobalTracker > about to load cleaner dll" );
        // Load cleaner library and set a call back to our cleanup
        RLibrary lib;
        TInt error( lib.Load( KATCleanerDllName ) );
        if ( error == KErrNone )
            {
            // Set address to point to ourself
            TLibraryFunction func = lib.Lookup( KAToolCleanerOrdinal ); // Ordinal 1 of the dll
            ATCLEANERTABLE* cleaner = (ATCLEANERTABLE*) func(); // Use function to get address
            cleaner->At( 0 ) = (TUint32) this; // Set address
            LOGSTR1( "ATMH ~TAnalyzeToolGlobalTracker() > cleaner dll loaded and call back set" );
            }
        else
            {
            // Error loading cleanup dll
            LOGSTR2( "ATMH ~TAnalyzeToolGlobalTracker() > cleaner dll load error(%i) uninstalling allocator now!", 
                    error );
            Cleanup();
            }
        }
    
    // -----------------------------------------------------------------------------
    // TAnalyzeToolGlobalTracker::Cleanup()
    // 
    // -----------------------------------------------------------------------------
    //
    void Cleanup()
        {
        LOGSTR1( "ATMH TAnalyzeToolGlobalTracker::Cleanup() - allocator uninstall" );
        
        // Uninstall allocator
        ( (RAnalyzeToolMemoryAllocator&) User::Allocator() ).Uninstall();
        }
    
    };

// Global variable definition.
TAnalyzeToolGlobalTracker gGlobalTracker;
#endif

// CONSTANTS
// When needed, update the version number directly inside _LIT macro.
// Constant for the atool API(staticlib) version.
_LIT( KAtoolApiVersion, "1.7.6" );

// Version number buffer length
const TInt KAtoolVersionNumberLength = 10;

// Wrong version error code
const TInt KAtoolVersionError = -1999;

// Version number separator
_LIT( KVersionSeparator, ";" );

// Incorrect version error strings 
_LIT( KIncorrectText, "ERR INCORRECT_ATOOL_VERSION [API v.%S][ATOOL v.%S]" );
_LIT( KIncorrectTextTrace, "<AT> " );

// -----------------------------------------------------------------------------
// CustomUser::Panic()
// Overloaded User::Panic() function
// -----------------------------------------------------------------------------
//
EXPORT_C void CustomUser::Panic( const TDesC& aCategory, TInt aReason )
    {
    LOGSTR3( "ATMH CustomUser::Panic() %S %i", &aCategory, aReason );
    
#ifdef USE_CLEANER_DLL
    // Set global tracker that panic has happened.
    gGlobalTracker.iPanic = ETrue;
#endif
    
    // Uninstall thread's RAllocator
    ( (RAnalyzeToolMemoryAllocator&) User::Allocator() ).Uninstall();
       
    // Call the "real" User::Panic()
    User::Panic( aCategory, aReason );
    }

// -----------------------------------------------------------------------------
// CustomUser::Exit()
// Overloaded User::Exit() function
// -----------------------------------------------------------------------------
//
EXPORT_C void CustomUser::Exit( TInt aReason )
    {
    LOGSTR3( "ATMH CustomUser::Exit() %i %i", aReason, RThread().Id().Id() );
    
    if ( aReason != KAtoolVersionError )
    	{
#ifdef USE_CLEANER_DLL
        // Only uninstall allocator if its not the process main/first thread.
        LOGSTR3( "ATMH CustomUser::Exit() - Thread id: %d - Main Id: %d",
                RThread().Id().operator TUint(), gGlobalTracker.iMainId.operator TUint() );
        
        if ( RThread().Id() != gGlobalTracker.iMainId )
            {
            LOGSTR2("ATMH CustomUser::Exit() - Calling allocator uninstall in thread: %d" , RThread().Id().operator TUint() );
            ( (RAnalyzeToolMemoryAllocator&) User::Allocator() ).Uninstall();
            }
#else
    	// Uninstall thread's RAllocator
    	( (RAnalyzeToolMemoryAllocator&) User::Allocator() ).Uninstall();
    	LOGSTR1( "ATMH CustomUser::Exit() - about to User::Exit" );
#endif
    	}
    
    // Call the "real" User::Exit()
    User::Exit( aReason );
    }

// -----------------------------------------------------------------------------
// CustomUser::SetCritical()
// Overloaded User::SetCritical() function which returns
// KErrNone, if successful; KErrArgument, if EAllThreadsCritical is 
// passed - this is a state associated with a process, and you use 
// User::SetProcessCritical() to set it.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CustomUser::SetCritical( User::TCritical aCritical )
    {
    LOGSTR1( "ATMH CustomUser::SetCritical()" );
    // Check the given User::TCritical type
    if ( aCritical == User::EAllThreadsCritical )
        {
        return KErrArgument;
        }
    else
        {
        return KErrNone;
        }
    }
  
// -----------------------------------------------------------------------------
// CustomUser::SetProcessCritical()
// Overloaded User::SetProcessCritical() function
// KErrNone, if successful; KErrArgument, if either EProcessCritical or 
// EProcessPermanent is passed - these are states associated with a 
// thread, and you use User::SetCritical() to set them.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CustomUser::SetProcessCritical( User::TCritical aCritical )
    {
    LOGSTR1( "ATMH CustomUser::SetProcessCritical()" );
     // Check the given User::TCritical type 
    if ( aCritical == User::EProcessCritical || 
         User::EProcessPermanent == aCritical )
        {
        return KErrArgument;
        }
    else
        {
        return KErrNone;
        }
    }

// -----------------------------------------------------------------------------
// CustomUser::SetupThreadHeap()
// Overloaded UserHeap::SetupThreadHeap function
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CustomUser::SetupThreadHeap( TBool aNotFirst, 
    SStdEpocThreadCreateInfo& aInfo, const TFileName& aFileName, const TPath& aFilePath,
    TUint32 aLogOption, TUint32 aIsDebug, const TATVersion& aVersion,
    TUint32 aAllocCallStackSize, TUint32 aFreeCallStackSize,
    TRefByValue<const TDesC> aFmt, ... )
    {
    LOGSTR1( "ATMH CustomUser::SetupThreadHeap()" );
    LOGSTR2( "ATMH > Thread id(%d)", RThread().Id().operator TUint() );
    
    // Add handling of the argument list here.
        
    TInt ret( KErrNone ); 
    
    // Check version number
    TBuf<KAtoolVersionNumberLength> atoolVer;
    TBuf<KApiVersionLength> apiVer;
    TInt err( KErrNone );
    err = CheckVersion( aVersion, atoolVer, apiVer );
    
    if ( err )
    	{
    	LOGSTR1( "ATMH > Wrong API version > Inform user and Exit." );
    	ReportIncorrectVersion( aLogOption, aFileName, aFilePath, atoolVer );
    	return KAtoolVersionError;
    	}
    
    // Check is this shared heap
    if ( aInfo.iAllocator == NULL )
        {
        LOGSTR1( "ATMH creating a new heap" );
        // RAllocator is NULL so heap is not shared, creating a new heap
        ret = UserHeap::SetupThreadHeap( aNotFirst, aInfo );
        __ASSERT_ALWAYS( KErrNone == ret, AssertPanic( EFailedToCreateHeap ) );
        
#if ( SYMBIAN_VERSION_SUPPORT >= SYMBIAN_3 )
    #ifndef __WINS__
        // Set dummy Tls value
        TAny* dummyPtr( NULL );
        TInt setErr( UserSvr::DllSetTls( KDummyHandle, dummyPtr ) );
        LOGSTR2( "ATMH > Set Tls err(%i)", setErr );
    #endif
#endif
        // Install the RAllocator
        TBuf8<KAtoolVersionLength> atoolVer8;
        atoolVer8.Copy(atoolVer);
        TBuf8<KApiVersionLength> apiVer8;
        apiVer8.Copy(apiVer);
        aInfo.iAllocator = &InstallAllocator( aNotFirst, aFileName, aFilePath, aLogOption, aIsDebug,
                aAllocCallStackSize, aFreeCallStackSize, atoolVer8, apiVer8 );
        }
    else
        {
        LOGSTR1( "ATMH sharing the heap" );
        // The heap is shared. Acquire pointer to the original heap
        RAnalyzeToolMemoryAllocator* allocator = 
			(RAnalyzeToolMemoryAllocator*) aInfo.iAllocator;
        // Share the heap
        allocator->ShareHeap();
        // Switch thread heap 
        User::SwitchAllocator( allocator );
        }
    

    return ret;
    }

// -----------------------------------------------------------------------------
// CustomUser::InstallAllocator
// Installs the RAllocator
// -----------------------------------------------------------------------------
//
//lint -e{429} suppress "Custodial pointer 'allocator' has not been freed or returned"
EXPORT_C RAllocator& CustomUser::InstallAllocator( TBool aNotFirst, 
	const TFileName& aFileName,  const TPath& aFilePath, TUint32 aLogOption, TUint32 aIsDebug,
	TUint32 aAllocCallStackSize, TUint32 aFreeCallStackSize, const TDesC8& aAtoolVersion, const TDesC8& aApiVersion )
    {
    LOGSTR1( "ATMH CustomUser::InstallAllocator()" );
    
    // Open handle to the device driver
    RAnalyzeTool analyzetool;
    TInt error = analyzetool.Open();
    
    // Check if the device driver has already loaded
    if ( KErrNone == error )
        {
        LOGSTR1( "ATMH CustomUser::InstallAllocator() - analyzetool.Open() returned KErrNone" );
        // The device driver has already loaded
        // Get pointer to the main thread allocator
        TMainThreadParamsBuf params;
        params().iProcessId = RProcess().Id().operator TUint();
        error = analyzetool.MainThreadAlloctor( params );

        __ASSERT_ALWAYS( KErrNone == error, AssertPanic( ECantOpenHandle ) );
        
        // Close handle to the device driver
        analyzetool.Close();
        
        // Is this the first thread of the program
        if ( params().iAlone )
            {
            LOGSTR1( "ATMH CustomUser::InstallAllocator() - first thread of the program" );
            // Only one thread in the program. Must be main thread
            RAnalyzeToolMainAllocator* allocator = 
				new RAnalyzeToolMainAllocator( aNotFirst, aFileName, aFilePath, aLogOption,
				                               aIsDebug, aAllocCallStackSize, aFreeCallStackSize, 
				                               aAtoolVersion, aApiVersion );
            
            __ASSERT_ALWAYS( allocator != NULL, AssertPanic( ENoMemory ) );
            
            // Change threads allocator
            User::SwitchAllocator( allocator );
            
            // Return reference to the RAllocator
            return *allocator;
            }
        // This is not the first thread. A new thread with a new heap created
        else
            {
            LOGSTR1( "ATMH CustomUser::InstallAllocator() - create a new allocator for the new thread" );
            // Create new RAllocator with handles from the main thread
            RAnalyzeToolAllocator* allocator = new RAnalyzeToolAllocator( 
                    aNotFirst,
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->Codeblocks(), 
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->Mutex(), 
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->ProcessId(), 
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->AnalyzeTool(),
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->LogOption(),
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->AllocMaxCallStack(),
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->FreeMaxCallStack(),
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->LogFile() );

            __ASSERT_ALWAYS( allocator != NULL, AssertPanic( ENoMemory ) );
            
            // Change threads allocator
            User::SwitchAllocator( allocator );
            
            // Return reference to the RAllocator
            return *allocator;
            }
        }
    // The device driver does not exists so this must be the first thread
    else
        {
        LOGSTR1( "ATMH CustomUser::InstallAllocator() - analyzetool.Open() returned error, creating DD" );
        RAnalyzeToolMainAllocator* allocator = 
			new RAnalyzeToolMainAllocator( aNotFirst, aFileName, aFilePath, aLogOption, aIsDebug,
			        aAllocCallStackSize, aFreeCallStackSize, aAtoolVersion, aApiVersion );
        
        __ASSERT_ALWAYS( allocator != NULL, AssertPanic( ENoMemory ) );
        
        // Change threads allocator
        User::SwitchAllocator( allocator );

        // Return reference to the RAllocator
        return *allocator;
        }
    } 
    
// -----------------------------------------------------------------------------
// CustomUser::CheckVersion
// Check atool version
// -----------------------------------------------------------------------------
//
TInt CustomUser::CheckVersion( const TATVersion& aVersion, TDes& aToolVersion, TDes& aApiVersion )
    { 
    LOGSTR2( "ATMH CustomUser::CheckVersion(), aVersion( %S )", &aVersion );
    
    TFileName version;
    version.Copy( aVersion );
    TBuf<KAtoolVersionNumberLength> apiVer;
    	
    // Find separator place
    TInt findplace( version.Find( KVersionSeparator() ) );
    // Parse API version first [x.x.x;x.x.x]
    if ( findplace >= 0 && findplace <= aApiVersion.MaxLength() )
		{
        aApiVersion.Copy( version.Mid( 0, findplace ) ); 
		version.Delete( 0, findplace + KVersionSeparator().Length() );
		}
 
    if ( version.Length() <= aToolVersion.MaxLength() )
    	{
    	aToolVersion.Copy( version );
    	if ( aToolVersion.Compare( KAtoolApiVersion ) == KErrNone &&
    	     aApiVersion.Length() == 0 )
    		{
    		// Support 1.5.0 version (Version info: [1.5.0])
    	    aApiVersion.Copy( version );
    		}
    	}
    
    LOGSTR3( "ATMH > API version( %S ), ATOOL version( %S )", 
    		&aApiVersion, &aToolVersion );
        
    // Check version numbers 
    if ( aApiVersion.Compare( KAtoolApiVersion ) == KErrNone )
    	{
    	return KErrNone;
    	}
    return KErrCancel;    
    }

// -----------------------------------------------------------------------------
// CustomUser::ReportIncorrectVersion
// Function for showing incorrect version information
// -----------------------------------------------------------------------------
//
void CustomUser::ReportIncorrectVersion( const TUint32 aLogOption,
	const TFileName& aFileName, const TPath& aFilePath, const TDes& aToolVersion )
	{
	LOGSTR2( "ATMH CustomUser::ReportIncorrectVersion(), aFileName( %S )", 
			&aFileName );

	switch ( aLogOption )
		{
		case EATLogToFile:
			{
			LOGSTR1( "ATMH ReportIncorrectVersion > EATLogToFile" );			
			
			// A handle to a file server session.
			RFs fs;
			// Creates and opens a file, 
			// and performs all operations on a single open file.
			RFile file;	
			// Create full path buffer
			TBuf<KMaxFileName> logFileBuf;
			// Connects a client to the file server.
			TInt err( fs.Connect() );
			
			if ( !err )
				{				
                err = TATDriveInfo::CreatePath( logFileBuf, aFileName, aFilePath, fs );
			    
				// Replace file if exists
				if ( err && err != KErrAlreadyExists )
					{
					LOGSTR2( "ATMH > TATDriveInfo::CreatePath() err( %i )", err );
					return;
					}
				
				// Replace file if exists (drive C)
				err = file.Replace( fs, logFileBuf, EFileWrite );
										
				// Write to file
				if ( !err )
					{
					err = file.Write( KDataFileVersion );
					// Error msg buffer
					TBuf8<KMaxFileName> msg;				     
					// Write the error code to the buffer  
					logFileBuf.Format( KIncorrectText, &KAtoolApiVersion, &aToolVersion );	
					CnvUtfConverter::ConvertFromUnicodeToUtf8( msg, logFileBuf );
					err = file.Write( msg );
					}
				// Closes the file.
				file.Close();
				}
			
			LOGSTR2( "ATMH > File err( %i )", err );			
			// Closes the handle.
			fs.Close();
			}
			break;
			
		case EATUseDefault:
		case EATLogToTraceFast:
			{
			LOGSTR1( "ATMH > ReportIncorrectVersion > EATLogToTraceFast" );
			// Error msg buffer
			TBuf<KMaxFileName> msg;	
			msg.Copy( KIncorrectTextTrace );
			msg.Append( KIncorrectText );
			TBuf<KMaxFileName> traceMsg;	
			// Write the error code to the buffer  
			traceMsg.Format( msg, &KAtoolApiVersion, &aToolVersion );
			RDebug::Print( traceMsg );
			}
			break;
		
		default:
			{
			LOGSTR1( "ATMH > ReportIncorrectVersion > default" );
			}
			break;
		}	
	}

// End of File
