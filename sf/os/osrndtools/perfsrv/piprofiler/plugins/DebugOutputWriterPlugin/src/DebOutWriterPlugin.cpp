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

#include "DebOutWriterPlugin.h"	
#include <e32base.h>
//#include <piprofiler/EngineUIDs.h>
#include <piprofiler/ProfilerTraces.h>
#ifdef OST_TRACE_COMPILER_IN_USE
#include <OpenSystemTrace.h>
#include "DebOutWriterPluginTraces.h"
#endif

// engine properties
const TUid KEngineStatusPropertyCat={0x2001E5AD};
enum TEnginePropertyKeys
    {
    EProfilerEngineStatus = 8,
    EProfilerErrorStatus
    };

// CONSTANTS
// own UID
const TUid KDebOutWriterPluginUid = { 0x2001E5BA };

//  Member Functions
/*
 *
 *	Class CDebOutWriterPlugin implementation
 *
 */

CDebOutWriterPlugin* CDebOutWriterPlugin::NewL(const TUid /*aImplementationUid*/, TAny* /*aInitParams*/)
{
	LOGTEXT(_L("CDebOutWriterPlugin::NewL() - entry"));
	CDebOutWriterPlugin* self = new (ELeave) CDebOutWriterPlugin(KDebOutWriterPluginUid);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
	LOGTEXT(_L("CDebOutWriterPlugin::NewL() - exit"));
    return self;
}

CDebOutWriterPlugin::CDebOutWriterPlugin(const TUid aImplementationUid) :
	iWriterType(aImplementationUid.iUid)
    {
    LOGTEXT(_L("CDebOutWriterPlugin::CDebOutWriterPlugin - entry"));
    isEnabled = EFalse;
    iWriterId = Id().iUid;
    LOGTEXT(_L("CDebOutWriterPlugin::CDebOutWriterPlugin - exit"));
    }

CDebOutWriterPlugin::~CDebOutWriterPlugin()
    {
    LOGTEXT(_L("CDebOutWriterPlugin::~CDebOutWriterPlugin - entry"));

    iErrorStatus.Close();
    
    if(iWriterHandler)
        {
        iWriterHandler->Cancel();
        delete iWriterHandler;
        }
    LOGTEXT(_L("CDebOutWriterPlugin::~CDebOutWriterPlugin - exit"));
    }    
    
void CDebOutWriterPlugin::ConstructL()
	{
	// second phase constructor, anything that may leave must be constructed here

	LOGTEXT(_L("CDebOutWriterPlugin::ConstructL() - entry"));
	iWriterHandler = CDebOutWriterHandler::NewL(this);
    User::LeaveIfError(iErrorStatus.Attach(KEngineStatusPropertyCat, EProfilerErrorStatus));
    
	LOGTEXT(_L("CDebOutWriterPlugin::ConstructL() - exit"));
	}

TInt CDebOutWriterPlugin::Start()
	{
	LOGTEXT(_L("CDebOutWriterPlugin::Start() - entry"));
	
#ifdef OST_TRACE_COMPILER_IN_USE
    TInt err(KErrNone);
	// activate traces on TraceCore
    RTcDriverParameters tcdp_ThreadIdentification;
    tcdp_ThreadIdentification.iComponentId = KOstTraceComponentID;
    tcdp_ThreadIdentification.iGroupId = CDEBOUTWRITERPLUGIN_PRINTDESCRIPTORASBASE64START;
    err = tcldd.ActivateTrace(tcdp_ThreadIdentification);

    tcdp_ThreadIdentification.iGroupId = CDEBOUTWRITERPLUGIN_PRINTDESCRIPTORASBASE64LINE;
    err = tcldd.ActivateTrace(tcdp_ThreadIdentification);

    tcdp_ThreadIdentification.iGroupId = CDEBOUTWRITERPLUGIN_PRINTDESCRIPTORASBASE64FIN;
    err = tcldd.ActivateTrace(tcdp_ThreadIdentification);

    tcdp_ThreadIdentification.iGroupId = CDEBOUTWRITERPLUGIN_PRINTDESCRIPTORASBASE64END;
    err = tcldd.ActivateTrace(tcdp_ThreadIdentification);

    RDebug::Print(_L("Debug output activated"));
    if(err != KErrNone)
        RDebug::Print(_L("TraceCore LDD API responded: %d"), err);
#endif

	LOGTEXT(_L("CDebOutWriterPlugin::Start() - exit"));
	return KErrNone;
	}

void CDebOutWriterPlugin::Stop()
	{
	LOGTEXT(_L("CDebOutWriterPlugin::Stop() - entry"));
	iWriterHandler->Stop();
#ifdef OST_TRACE_COMPILER_IN_USE
	TInt err(KErrNone);
    // activate traces on TraceCore
    RTcDriverParameters tcdp_ThreadIdentification;
    tcdp_ThreadIdentification.iComponentId = KOstTraceComponentID;
    tcdp_ThreadIdentification.iGroupId = CDEBOUTWRITERPLUGIN_PRINTDESCRIPTORASBASE64START;
    err = tcldd.DeactivateTrace(tcdp_ThreadIdentification);

    tcdp_ThreadIdentification.iGroupId = CDEBOUTWRITERPLUGIN_PRINTDESCRIPTORASBASE64LINE;
    err = tcldd.DeactivateTrace(tcdp_ThreadIdentification);

    tcdp_ThreadIdentification.iGroupId = CDEBOUTWRITERPLUGIN_PRINTDESCRIPTORASBASE64FIN;
    err = tcldd.DeactivateTrace(tcdp_ThreadIdentification);

    tcdp_ThreadIdentification.iGroupId = CDEBOUTWRITERPLUGIN_PRINTDESCRIPTORASBASE64END;
    err = tcldd.DeactivateTrace(tcdp_ThreadIdentification);

    RDebug::Print(_L("Debug output deactivated"));
    if(err != KErrNone)
        RDebug::Print(_L("TraceCore LDD API responded: %d"), err);
#endif
	LOGTEXT(_L("CDebOutWriterPlugin::Stop() - exit"));
	}

TUid CDebOutWriterPlugin::Id() const
	{
    LOGSTRING2("CDebOutWriterPlugin::Id(): 0x%X", KDebOutWriterPluginUid.iUid );
    return KDebOutWriterPluginUid;
	//return iDtor_ID_Key;
	}

TBool CDebOutWriterPlugin::GetEnabled()
	{
	return isEnabled;
	}

void CDebOutWriterPlugin::SetValue( const TWriterPluginValueKeys aKey,
                                    TDes& aValue )
    {
    TRAP_IGNORE( SetValueL( aKey, aValue ) );
    }


void CDebOutWriterPlugin::GetValue( const TWriterPluginValueKeys aKey,
                                    TDes& aValue )
    {
    TRAP_IGNORE( GetValueL( aKey, aValue ) );
    }



void CDebOutWriterPlugin::SetValueL( const TWriterPluginValueKeys aKey, TDes& /*aValue*/ )
{
	LOGTEXT(_L("CDebOutWriterPlugin::SetValueL - entry"));	
	
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
        case EWriterPluginSettings:
        	//result = StringLoader::LoadL(PROFILER_KERNEL_MODE_SAMPLER);
        	LOGTEXT(_L("CDebOutWriterPlugin::SetValueL - setting plugin settings"));	
        	break;
        default:
        	break;
        }
	LOGTEXT(_L("CDebOutWriterPlugin::SetValueL - exit"));	

}

TUint32 CDebOutWriterPlugin::GetWriterType()
	{
	return iWriterType;
	}


void CDebOutWriterPlugin::GetValueL( const TWriterPluginValueKeys aKey, TDes& aValue )
    {
    switch( aKey )
        {
        case EWriterPluginVersion:

        	GetWriterVersion(&aValue);
        	break;
        case EWriterPluginType:
        	break;
           default:
                break;
        }
    }

void CDebOutWriterPlugin::GetWriterVersion(TDes* aDes)
	{
	_LIT(KDebugOutputWriterVersion, "1.0.0");
	aDes->Append(KDebugOutputWriterVersion);
	}

void CDebOutWriterPlugin::DoCancel()
{
	LOGTEXT(_L("CDebOutWriterPlugin::DoCancel - entry"));
}

void CDebOutWriterPlugin::WriteData()
    {
    // Activate handler to write data from buffer to output
    LOGTEXT(_L("CDiskWriterPlugin::WriteData() - entry"));
    TRAP_IGNORE(iWriterHandler->StartL());
    LOGTEXT(_L("CDiskWriterPlugin::WriteData() - exit"));
    }

void CDebOutWriterPlugin::HandleError(TInt aError)
    {
    TInt err(KErrNone);
    err = iErrorStatus.Set(aError);
    if(err != KErrNone)
        {
        RDebug::Print(_L("CDiskWriterPlugin::HandleError() - error: %d"), err);
        }
    }

void CDebOutWriterPlugin::PrintDescriptorAsBase64(	TDesC8& aDes,
                                                    TRequestStatus* aStatus,
													TUint32 sampleTime,
													TBool aEmptying)
{
	LOGTEXT(_L("CDebOutWriterPlugin::PrintDescriptorAsBase64() - entry"));
	TUint len = aDes.Length();

	// we must wait for the sample tick to be printed, in case
	// prints are performed at user side, otherwise the kernel
	// prints will corrupt the data
	if(sampleTime != 0xffffffff)
	{
		TUint32 remains = sampleTime%1000;
	
		if(remains > 800) 
		{
			TTimeIntervalMicroSeconds32 timeToWait = ((1050-remains)*1000);
			User::After(timeToWait);
		}
	}
	
	TBuf16<75> buf;

	// Header
#ifdef OST_TRACE_COMPILER_IN_USE
    OstTrace0( PIPROFILER_TRACE_OUT, CDEBOUTWRITERPLUGIN_PRINTDESCRIPTORASBASE64START, 
            "<PIPROF>=================================================================" );
#else
	RDebug::Print(_L("<PIPROF>================================================================="));
#endif
	buf.Zero();

	// base64 encoding table
	const char uu_base64[64] =
	{
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', '0', '1', '2', '3',
		'4', '5', '6', '7', '8', '9', '+', '/'
	};

	TChar byte1, byte2, byte3, byte4;
	TUint8 count = 0x30;
	// base64 encoding 
	for(TUint i = 0, j = 0; i < len; i += 3, j = (j + 1) % 18) 
	{
    // byte 1
		byte1 = uu_base64[(aDes[i] >> 2) & 0x3F];
		
		// byte 2
		if(i+1 < len)
			byte2 = uu_base64[(aDes[i] << 4) & 0x3f | (aDes[i+1] >> 4)];
		else
			byte2 = uu_base64[(aDes[i] << 4) & 0x3f];

		// byte 3
		if(i+1 < len && i+2 < len)
			byte3 = uu_base64[(aDes[i+1] << 2) & 0x3f | (aDes[i+2] >> 6)];
		else if(i+1 < len)
			byte3 = uu_base64[(aDes[i+1] << 2) & 0x3f];
		else
			byte3 = '=';

		// byte 4
		if(i+2 < len) 
			byte4 = uu_base64[aDes[i+2] & 0x3f];
		else
			byte4 = '=';
	
		// append to buffer
		buf.Append(byte1);
		buf.Append(byte2);
		buf.Append(byte3);
		buf.Append(byte4);

		// output 72 chars / line
		if(j == 17) 
		{		
			// add check number at the end of line
			buf.Append(count);
#ifdef OST_TRACE_COMPILER_IN_USE
			OstTraceExt1( PIPROFILER_TRACE_OUT, CDEBOUTWRITERPLUGIN_PRINTDESCRIPTORASBASE64LINE, "<PIPROF>%S", &buf );
#else
			RDebug::Print(_L("<PIPROF>%S"),&buf);
#endif
			count++;
			if(count > 0x39)
				count = 0x30;
			buf.Zero();
		}
	}
	
#ifdef OST_TRACE_COMPILER_IN_USE
	OstTraceExt1( PIPROFILER_TRACE_OUT, CDEBOUTWRITERPLUGIN_PRINTDESCRIPTORASBASE64FIN, "<PIPROF>%S", &buf );
#else
	RDebug::Print(_L("<PIPROF>%S"),&buf);
#endif
	buf.Zero();

	// footer
#ifdef OST_TRACE_COMPILER_IN_USE
	OstTrace0( PIPROFILER_TRACE_OUT, CDEBOUTWRITERPLUGIN_PRINTDESCRIPTORASBASE64END, 
	        "<PIPROF>=================================================================" );
#else
	RDebug::Print(_L("<PIPROF>================================================================="));
#endif

	if(!aEmptying)
	    {
        if(aStatus != 0) 
            User::RequestComplete(aStatus,0);
	    }
	
	LOGTEXT(_L("CDebOutWriterPlugin::PrintDescriptorAsBase64() - exit"));
}



/*
 * 
 * Implementation of class CDebOutWriterHandler
 * 
 */
CDebOutWriterHandler::CDebOutWriterHandler(CDebOutWriterPlugin* aWriter) :
    CActive(EPriorityStandard)
    {
    LOGTEXT(_L("CDebOutWriterHandler::CDebOutWriterHandler - entry"));
    iWriter = aWriter;
    
    // set initial mode to non-stopping
    iStopping = EFalse;
    
    // add the handler to the active scheduler
    CActiveScheduler::Add(this);
    LOGTEXT(_L("CDebOutWriterHandler::CDebOutWriterHandler - exit"));
    }

CDebOutWriterHandler* CDebOutWriterHandler::NewL(CDebOutWriterPlugin* aWriter)
{
	LOGTEXT(_L("CDebOutWriterHandler::NewL() - entry"));
	CDebOutWriterHandler* self = new (ELeave) CDebOutWriterHandler(aWriter);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
	LOGTEXT(_L("CDebOutWriterHandler::NewL() - exit"));
    return self;
}

CDebOutWriterHandler::~CDebOutWriterHandler()
    {
	LOGTEXT(_L("CDebOutWriterHandler::~CDebOutWriterHandler - entry"));
    
    LOGTEXT(_L("CDebOutWriterHandler::~CDebOutWriterHandler - exit"));
    }    
    
void CDebOutWriterHandler::ConstructL()
	{

	}

void CDebOutWriterHandler::StartL()
	{
	LOGTEXT(_L("CDebOutWriterHandler::StartL - entry"));
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
	LOGTEXT(_L("CDebOutWriterHandler::StartL - entry"));
	}

void CDebOutWriterHandler::Stop()
	{
	LOGTEXT(_L("CDebOutWriterHandler::Stop - entry"));
   
	// do write once more to write the logged data to output
    // set to stopping mode, needed for emptying the remaining full buffers
    iStopping = ETrue;

    // stop the timer
    Reset();

    // set mode back to non-stopping
    iStopping = EFalse;
	LOGTEXT(_L("CDebOutWriterHandler::Stop - exit"));
	}

void CDebOutWriterHandler::Reset()
    {
  
    // start writing new buffer if there is one available
    TBapBuf* nextBuf = iWriter->iStream->GetNextFilledBuffer();
    
    // empty the rest of the buffers synchronously
    while(nextBuf != 0)
        {
        if(nextBuf->iDataSize != 0)
            {
            LOGTEXT(_L("CDiskWriterHandler::Reset - writing to file"));
            iWriter->PrintDescriptorAsBase64(*(nextBuf->iBufDes),&iStatus,0xffffffff, iStopping);
            }
        
        // empty buffers when profiling stopped
        iWriter->iStream->AddToFreeBuffers(nextBuf);

        LOGTEXT(_L("CDiskWriterHandler::Reset - get next full buffer"));
        // start writing new buffer if there is one available
        nextBuf = iWriter->iStream->GetNextFilledBuffer();
        LOGSTRING2("CDiskWriterHandler::Reset - got next filled 0x%x",nextBuf);
        }
    }

void CDebOutWriterHandler::HandleFullBuffers()
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

void CDebOutWriterHandler::RunL()
    {
    // call function to complete full buffer handling
    HandleFullBuffers();
    }

void CDebOutWriterHandler::DoCancel()
    {
    
    }

void CDebOutWriterHandler::WriteBufferToOutput(TBapBuf* aBuf)
    {
    LOGTEXT(_L("CDebOutWriterHandler::WriteBufferToOutput - entry"));
    iBufferBeingWritten = aBuf;

    // set the data length just to be sure
    iBufferBeingWritten->iBufDes->SetLength(aBuf->iDataSize);

    LOGTEXT(_L("CDiskWriterPlugin::WriteBufferToOutput - writing to file"));
//    PrintBufferToOutput(iBufferBeingWritten, iStatus);
    iWriter->PrintDescriptorAsBase64(*(iBufferBeingWritten->iBufDes),&iStatus,0xffffffff, iStopping);
    // set AO back to active, until filled buffers are emptied 
    SetActive();
    
    LOGTEXT(_L("CDebOutWriterHandler::WriteBufferToOutput - exit"));
    }

// private
void CDebOutWriterHandler::PrintBufferToOutput(TBapBuf* aBuffer, TRequestStatus& aStatus)
    {
    LOGTEXT(_L("CDebOutWriterHandler::WriteBufferToOutput() - debug out writer tick activated"));

    TPtrC8& aDes = (TPtrC8&)*(aBuffer->iBufDes);
#ifdef BAPPEA_SAMPLE_MARKS
    TUint32 time = iSampler->GetSampleTime();
#else
    TUint32 time = 0xffffffff;
#endif
    iWriter->PrintDescriptorAsBase64(aDes,&aStatus,time, iStopping);
    }

