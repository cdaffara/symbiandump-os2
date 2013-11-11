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


//  Include Files  

#include "DiskWriterPlugin.h"	// CDiskWriterPlugin
#include <e32base.h>
#include <sysutil.h>
//#include <piprofiler/EngineUIDs.h>

// constants
const TUid KDiskWriterPluginUid = { 0x2001E5BB };   // own UID

// engine properties
const TUid KEngineStatusPropertyCat={0x2001E5AD};
enum TEnginePropertyKeys
    {
    EProfilerEngineStatus = 8,
    EProfilerErrorStatus
    };

/*
 *
 *	Class CDiskWriterPlugin implementation
 *
 */

//  Member Functions
CDiskWriterPlugin* CDiskWriterPlugin::NewL(const TUid /*aImplementationUid*/, TAny* /*aInitParams*/)
    {
	LOGTEXT(_L("CDiskWriterPlugin::NewL() - entry"));
	CDiskWriterPlugin* self = new (ELeave) CDiskWriterPlugin(KDiskWriterPluginUid);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
	LOGTEXT(_L("CDiskWriterPlugin::NewL() - exit"));
    return self;
    }

CDiskWriterPlugin::CDiskWriterPlugin(const TUid aImplementationUid) :
	iWriterType(aImplementationUid.iUid)
    {
    LOGTEXT(_L("CDiskWriterPlugin::CDiskWriterPlugin - entry"));
    isEnabled = EFalse;
    iWriterId = Id().iUid;
    LOGTEXT(_L("CDiskWriterPlugin::CDiskWriterPlugin - exit"));
    }

CDiskWriterPlugin::~CDiskWriterPlugin()
    {
    LOGTEXT(_L("CDiskWriterPlugin::~CDiskWriterPlugin - entry"));

    iErrorStatus.Close();
    
    if(iWriterHandler)
        {
        iWriterHandler->Cancel();
        delete iWriterHandler;
        }
    LOGTEXT(_L("CDiskWriterPlugin::~CDiskWriterPlugin - exit"));
    }

void CDiskWriterPlugin::ConstructL()
	{
	// second phase constructor, anything that may leave must be constructed here
	LOGTEXT(_L("CDiskWriterPlugin::ConstructL() - entry"));
	iWriterHandler = CDiskWriterHandler::NewL(this);
	User::LeaveIfError(iErrorStatus.Attach(KEngineStatusPropertyCat, EProfilerErrorStatus));
	LOGTEXT(_L("CDiskWriterPlugin::ConstructL() - exit"));
	}

TUid CDiskWriterPlugin::Id() const 
	{
    LOGSTRING2("CDiskWriterPlugin::Id():0x%X", KDiskWriterPluginUid.iUid );
    return KDiskWriterPluginUid;
	}
	 
void CDiskWriterPlugin::GetWriterVersion(TDes* aDes)
	{
	_LIT(KDiskWriterVersion, "1.0.0");
	aDes->Append(KDiskWriterVersion);
	}

TInt CDiskWriterPlugin::Start()
	{
//	if(isEnabled)
//		{
//		TRAPD(err, iWriterHandler->StartL());
//		if( err != KErrNone)
//		    {
//		    LOGTEXT(_L("Could not start writer plugin"));
//		    return err;
//		    }
//		}
	return KErrNone;
	}

void CDiskWriterPlugin::Stop()
	{
	// stop writer handler normally
	iWriterHandler->Stop();
	}

TBool CDiskWriterPlugin::GetEnabled()
	{
	return isEnabled;
	}

TUint32 CDiskWriterPlugin::GetWriterType()
	{
	return iWriterType;
	}


void CDiskWriterPlugin::SetValue( const TWriterPluginValueKeys aKey,
                                    TDes& aValue )
    {
    TRAP_IGNORE( SetValueL( aKey, aValue ) );
    }


void CDiskWriterPlugin::GetValue( const TWriterPluginValueKeys aKey,
                                    TDes& aValue )
    {
    TRAP_IGNORE( GetValueL( aKey, aValue ) );
    }

void CDiskWriterPlugin::SetValueL( const TWriterPluginValueKeys aKey, TDes& aValue )
    {
    TInt error(KErrNone);
    
    switch( aKey )
        {
        case EWriterPluginEnabled:
            isEnabled = ETrue;
        	LOGTEXT(_L("CDebOutWriterPlugin::SetValueL - plugin enabled"));
        	break;
        case EWriterPluginDisabled:
            isEnabled = EFalse;
        	LOGTEXT(_L("CDebOutWriterPlugin::SetValueL - plugin disabled"));	
            break;
        case EWriterPluginSettings:	// file name in case of disk writer plugin
        	iFileName.Zero();
        	iFileName.Append(aValue);
        	error = iWriterHandler->TestFile(iFileName);
        		User::LeaveIfError(error);
        	break;
        default:
        	break;
        }
    }

void CDiskWriterPlugin::GetValueL( const TWriterPluginValueKeys aKey, TDes& aValue )
    {
    switch( aKey )
        {
        case EWriterPluginVersion:
        	GetWriterVersion(&aValue);
        	break;
        case EWriterPluginType:
        	break;
        case EWriterPluginSettings:	// file name in disk writer case
        	aValue.Copy(iFileName);
           default:
                break;
        }
    }

void CDiskWriterPlugin::DoCancel()
    {
	LOGTEXT(_L("CDebOutWriterPlugin::DoCancel - entry"));
    }

void CDiskWriterPlugin::WriteData()
    {
    // Activate handler to write data from buffer to output
    LOGTEXT(_L("CDiskWriterPlugin::WriteData() - entry"));
    TRAP_IGNORE(iWriterHandler->StartL());
    LOGTEXT(_L("CDiskWriterPlugin::WriteData() - exit"));
    }

void CDiskWriterPlugin::HandleError(TInt aError)
    {
    TInt err(KErrNone);
    RDebug::Print(_L("CDiskWriterPlugin::HandleError() - error received: %d"), aError);
    err = iErrorStatus.Set(aError);
    if(err != KErrNone)
        {
        RDebug::Print(_L("CDiskWriterPlugin::HandleError() - error in updating property: %d"), err);
        }
    }

/*
 * 
 * Implementation of class CDiskWriterHandler
 * 
 */

CDiskWriterHandler* CDiskWriterHandler::NewL(CDiskWriterPlugin* aWriter)
    {
	LOGTEXT(_L("CDiskWriterHandler::NewL() - entry"));
	CDiskWriterHandler* self = new (ELeave) CDiskWriterHandler(aWriter);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
	LOGTEXT(_L("CDiskWriterHandler::NewL() - exit"));
    return self;
    }

CDiskWriterHandler::CDiskWriterHandler(CDiskWriterPlugin* aWriter)  :
    CActive(EPriorityStandard)
    {
    LOGTEXT(_L("CDiskWriterHandler::CDiskWriterHandler - entry"));

    iWriter = aWriter;
    
    // set initial mode to non-stopping
    iStopping = EFalse;
    
    // add the handler to the active scheduler
    CActiveScheduler::Add(this);
    
    LOGTEXT(_L("CDiskWriterHandler::CDiskWriterHandler - exit"));
    }


CDiskWriterHandler::~CDiskWriterHandler()
    {
	LOGTEXT(_L("CDiskWriterHandler::~CDiskWriterHandler - entry"));

	LOGTEXT(_L("CDiskWriterHandler::~CDiskWriterHandler - exit"));
    }    
    
void CDiskWriterHandler::ConstructL()
	{
	}

TInt CDiskWriterHandler::TestFile(const TDesC& totalPrefix) 
    {
    TParse parse;

    TInt err(KErrNone);
    if((err = parse.Set(totalPrefix, NULL, NULL)) != KErrNone)
        return err;

    err = iFs.Connect();
    if(err != KErrNone)
        {
        LOGTEXT(_L("CDiskWriterHandler::TestFile() - Failed to open a session to file server"));
        return KErrNotFound;
        }
    
    iFs.MkDirAll(parse.FullName());
    
    err = iFile.Replace(iFs,parse.FullName(),EFileWrite);
    if(err != KErrNone)
        {
        iFs.Close();
        return KErrNotFound;
        }
    
    iFileName.Copy(parse.FullName());

    return KErrNone;
    }

void CDiskWriterHandler::Reset()
    {
    // cancel active object
    Cancel();
    
    // start writing new buffer if there is one available
    TBapBuf* nextBuf = iWriter->iStream->GetNextFilledBuffer();
    
    // empty the rest of the buffers synchronously
    while(nextBuf != 0)
        {
        LOGTEXT(_L("CDiskWriterHandler::Reset - writing to file"));
        if(nextBuf->iDataSize != 0)
            {
            LOGTEXT(_L("CDiskWriterHandler::Reset - writing to file"));
            WriteMemBufferToFile(*(nextBuf->iBufDes),iStatus);
            }
        
        // empty buffers when profiling stopped
        iWriter->iStream->AddToFreeBuffers(nextBuf);

        LOGTEXT(_L("CDiskWriterHandler::Reset - get next full buffer"));
        // start writing new buffer if there is one available
        nextBuf = iWriter->iStream->GetNextFilledBuffer();
        LOGSTRING2("CDiskWriterHandler::Reset - got next filled 0x%x",nextBuf);
        }
    }

void CDiskWriterHandler::HandleFullBuffers()
    {
    LOGTEXT(_L("CDiskWriterHandler::HandleFullBuffers - entry"));
    // previous write operation has finished
    // release the previous buffer 
    iWriter->iStream->AddToFreeBuffers(iBufferBeingWritten);

    LOGTEXT(_L("CDiskWriterHandler::HandleFullBuffers - get next full buffer"));
    // start writing new buffer if there is one available
    TBapBuf* nextBuf = iWriter->iStream->GetNextFilledBuffer();

    if(nextBuf != 0)
        {
        LOGTEXT(_L("CDiskWriterHandler::HandleFullBuffers - writing to file"));
        if(nextBuf->iDataSize != 0)
            {
            WriteBufferToOutput(nextBuf);
            }
        } 
    LOGTEXT(_L("CDiskWriterHandler::HandleFullBuffers - exit"));
    }

void CDiskWriterHandler::RunL()
    {
    LOGTEXT(_L("CDiskWriterHandler::RunL - entry"));
    // call function to complete full buffer handling
    HandleFullBuffers();
    LOGTEXT(_L("CDiskWriterHandler::RunL - exit"));
    }

void CDiskWriterHandler::DoCancel()
    {
    
    }

//-----------------------------------------------------------------------------
// CPIProfilerTraceCoreLauncher::RunError(TInt aError)
// Handle leaves from RunL().
//-----------------------------------------------------------------------------
TInt CDiskWriterHandler::RunError(TInt aError)
    {
    // no reason to continue if disk full or removed
    iFile.Close();
    // close handle to file server too
    iFs.Close();
    iFileName.Zero();
    
    iWriter->HandleError(KErrDiskFull);
    return aError;
    }

void CDiskWriterHandler::WriteMemBufferToFile(TDesC8& aDes, TRequestStatus& aStatus)
    {   
    LOGTEXT(_L("CDiskWriterPlugin::WriteMemBufferToFile - entry"));

    TUint sampleSize(aDes.Length());
    TInt err(KErrNone);
    TInt drive(0);
    TDriveInfo info;
    TBool noDiskSpace(EFalse);
    
    err = iFile.Drive(drive,info);

    // test available disk space 
    TRAP_IGNORE((noDiskSpace = SysUtil::DiskSpaceBelowCriticalLevelL(&iFs, sampleSize, drive))); 
    // check first if still space on disk
    if(noDiskSpace)
        {
        // set error to disk full
        err = KErrDiskFull;
        LOGTEXT(_L("CDiskWriterPlugin::WriteMemBufferToFile - disk full, cannot write"));
        }
    else
        {
        // check if profiling in stopping mode
        if(iStopping)
            {
            // RDebug::Print(_L("CDiskWriterPlugin::WriteMemBufferToFile - data written, length %d, stopping"), aDes.Length());
            // write to file without status
            err = iFile.Write(aDes);
            }
        else
            {
            // RDebug::Print(_L("CDiskWriterPlugin::WriteMemBufferToFile - data written, length %d"), aDes.Length());
            // write to file with status
            iFile.Write(aDes,aStatus);
            }
        }
    
    // check if error in write
    if(err != KErrNone)
        {
        // stop writer handler (and its timer) immediately, DO NOT try write data!
        Cancel();
        
        // no reason to continue if disk full or removed
        // end of stream detected, file can be closed
        iFile.Close();
        // close handle to file server too
        iFs.Close();
        iFileName.Zero();
        
        // set error status for engine to read
        iWriter->HandleError(err);
        }
    LOGTEXT(_L("CDiskWriterPlugin::WriteMemBufferToFile - exit"));
    }

void CDiskWriterHandler::WriteBufferToOutput(TBapBuf* aBuf)
    {
    LOGTEXT(_L("CDiskWriterPlugin::WriteBufferToOutput - entry"));
    iBufferBeingWritten = aBuf;

    // set the data length just to be sure
    iBufferBeingWritten->iBufDes->SetLength(aBuf->iDataSize);

    LOGTEXT(_L("CDiskWriterPlugin::WriteBufferToOutput - writing to file"));
    WriteMemBufferToFile(*(iBufferBeingWritten->iBufDes), iStatus);

    // set AO back to active, until filled buffers are emptied 
    SetActive();
    
    LOGTEXT(_L("CDiskWriterPlugin::WriteBufferToOutput - exit"));
    }

void CDiskWriterHandler::StartL()
	{
	LOGTEXT(_L("CDiskWriterHandler::StartL - entry"));
    if(!IsActive())
        {
        LOGTEXT(_L("CDiskWriterHandler::StartL - is not active"));
    
        TBapBuf* nextBuf = iWriter->iStream->GetNextFilledBuffer();
        LOGSTRING2("CDiskWriterHandler::StartL - got next filled 0x%x",nextBuf);
    
        if(nextBuf != 0)
            {
            LOGTEXT(_L("CDiskWriterHandler::StartL - writing to file"));
            WriteBufferToOutput(nextBuf);
            }
        }
    LOGTEXT(_L("CDiskWriterHandler::StartL - exit"));
	}

void CDiskWriterHandler::Stop()
	{
	LOGTEXT(_L("CDiskWriterHandler::Stop - entry"));
	
	// set to stopping mode, needed for emptying the remaining full buffers
	iStopping = ETrue;
	
	// stop the timer
	Reset();

    // end of stream detected, file can be closed
    iFile.Close();
    // close handle to file server too
    iFs.Close();
    iFileName.Zero();
    
    // set mode back to non-stopping
    iStopping = EFalse;
    
    LOGTEXT(_L("CDiskWriterHandler::Stop - exit"));
	}

// end of file
