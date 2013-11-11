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
* Description:  Definitions for the class CLibraryEventHandler.
*
*/


// INCLUDE FILES
#include "atlog.h"
#include "analyzetooleventhandler.h"
#include "analyzetooleventhandlernotifier.h"
#include "analyzetoolmemoryallocator.h"
#include "analyzetoolfastlog.h"

// -----------------------------------------------------------------------------
// CLibraryEventHandler::~CLibraryEventHandler()
// Destructor.
// -----------------------------------------------------------------------------
//
CLibraryEventHandler::~CLibraryEventHandler()
    {
    LOGSTR1( "ATMH CLibraryEventHandler::~CLibraryEventHandler()" );
    Cancel();
    }

// -----------------------------------------------------------------------------
// CLibraryEventHandler::RunL()
// Process active object's task
// -----------------------------------------------------------------------------
//
void CLibraryEventHandler::RunL()
    {
    LOGSTR1( "ATMH CLibraryEventHandler::RunL()" );

    if ( KErrNone != iStatus.Int() )
        {
        LOGSTR2( "ATMH RunL error: %i",  iStatus.Int() );
        return;
        }
    // Acquire the mutex
    iMutex.Wait();

    // Check that this is observed process 
    if ( iLibraryInfo.iEventType == TLibraryEventInfo::ELibraryAdded )
        {
        LOGSTR1( "ATMH CLibraryEventHandler::RunL() - TLibraryEventInfo::ELibraryAdded" );
        if ( iLibraryInfo.iProcessId == iProcessId )
            {
            TInt error( KErrNone );

            // Log library load event.
            if ( iLogOption == EATLogToTraceFast )
                {
                LOGSTR1( "ATMH CLibraryEventHandler::RunL() - ATFastLog.LogDllLoaded() " );
                ATFastLogDllLoaded( iProcessId,
                        iLibraryInfo.iLibraryName, 
                        iLibraryInfo.iRunAddress,
                        iLibraryInfo.iRunAddress + iLibraryInfo.iSize );
                }
            else if( iLogOption == EATLogToFile )
            	{
				iLogFile.ATFileLogDllLoaded( iLibraryInfo.iLibraryName, 
						iLibraryInfo.iRunAddress,
						iLibraryInfo.iRunAddress + iLibraryInfo.iSize );
            	}
            
            if ( KErrNone == error )
                {
                iCodeblocks.Append( TCodeblock( iLibraryInfo.iRunAddress, 
                        iLibraryInfo.iSize, 
                        iLibraryInfo.iLibraryName ) );
                }
            }
        }
    else if ( iLibraryInfo.iEventType == TLibraryEventInfo::ELibraryRemoved )
        {
        LOGSTR1( "ATMH CLibraryEventHandler::RunL() - TLibraryEventInfo::ELibraryRemoved " );
        TInt count = iCodeblocks.Count();
        LOGSTR2( "ATMH count of code blocks: %i",  count );
        for ( TInt i = 0; i < count; i++ )
            {
            if ( iCodeblocks[ i ].Match( iLibraryInfo.iLibraryName ) )
                {
                TBuf8<KMaxLibraryName> libraryName;
                libraryName.Copy( iLibraryInfo.iLibraryName );
                
                // Log library unloaded event
                if ( iLogOption == EATLogToTraceFast )
                    {
                    LOGSTR1( "ATMH CLibraryEventHandler::RunL() - ATFastLogDllUnloaded() " );
                    ATFastLogDllUnloaded( iProcessId,
                            libraryName,
                            iLibraryInfo.iRunAddress, 
                            iLibraryInfo.iRunAddress + iLibraryInfo.iSize );
                    }
                else if( iLogOption == EATLogToFile )
					{
					iLogFile.ATFileLogDllUnloaded( iLibraryInfo.iLibraryName, 
							iLibraryInfo.iRunAddress,
							iLibraryInfo.iRunAddress + iLibraryInfo.iSize );
					}
                iCodeblocks.Remove( i );
                break;
                }
            }
        }
    else if ( iLibraryInfo.iEventType == TLibraryEventInfo::EKillThread )
		{
		LOGSTR1( "ATMH CLibraryEventHandler::RunL() - TLibraryEventInfo::EKillThread" );
	  
		iNotifier.RemoveKilledThread( iLibraryInfo.iThreadId );
    	}
    
    Start();
    
    // Release the mutex
    iMutex.Signal();
    }

// -----------------------------------------------------------------------------
// CLibraryEventHandler::DoCancel()
// Cancels active object's task
// -----------------------------------------------------------------------------
//
void CLibraryEventHandler::DoCancel()
    {
    LOGSTR1( "ATMH CLibraryEventHandler::DoCancel()" );
    // Cancel the subscription of the library events
    if ( IsActive() )
        {
        iStarted = EFalse;
        
        iAnalyzeTool.CancelLibraryEvent();
        }
    }

// -----------------------------------------------------------------------------
// CLibraryEventHandler::Start()
// Creates CActiveScheduler and variables for the child thread
// -----------------------------------------------------------------------------
//
void CLibraryEventHandler::Start()
    {
    LOGSTR1( "ATMH CLibraryEventHandler::Start()" );
    
    // Acquire the mutex
    iMutex.Wait();

    if ( !IsAdded() )
        {
        CActiveScheduler::Add( this );
        }

    // Cancel current subscribetion
    if ( IsActive() )
        {
        iStarted = EFalse;
        Cancel();
        }

    iStatus = KErrNone;
    iAnalyzeTool.LibraryEvent( iStatus, iLibraryInfo );
    SetActive();
    
    iStarted = ETrue;
    // Release the mutex
    iMutex.Signal();
    }

// -----------------------------------------------------------------------------
// CLibraryEventHandler::CLibraryEventHandler()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLibraryEventHandler::CLibraryEventHandler( RAnalyzeTool& aAnalyzeTool, 
	RArray<TCodeblock>& aCodeblocks,
	TUint aProcessId, RMutex& aMutex, 
	MAnalyzeToolEventhandlerNotifier& aNotifier,
	TUint32 aLogOption,
	RATFileLog& aFileLog ) 
 :	CActive( EPriorityNormal ),
    iAnalyzeTool( aAnalyzeTool ),
    iCodeblocks( aCodeblocks ),
    iProcessId( aProcessId ),
    iMutex( aMutex ),
    iStarted( EFalse ),
    iNotifier( aNotifier ),
    iLogOption( aLogOption ),
    iLogFile( aFileLog )
    {
    LOGSTR1( "ATMH CLibraryEventHandler::CLibraryEventHandler()" );
    }

// -----------------------------------------------------------------------------
// CLibraryEventHandler::IsStarted()
// Returns eventhandler's state.
// -----------------------------------------------------------------------------
//
TBool CLibraryEventHandler::IsStarted()
    {
    LOGSTR2( "ATMH CLibraryEventHandler::IsStarted(%i)", iStarted );
    
    return iStarted;
    }
    
// End of File
