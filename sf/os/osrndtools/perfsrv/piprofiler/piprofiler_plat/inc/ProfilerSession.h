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


#ifndef __PROFILER_SESSION_H__
#define __PROFILER_SESSION_H__

#include <e32base.h>
#include <e32svr.h>
#include <piprofiler/ProfilerConfig.h>
#include <piprofiler/ProfilerVersion.h>
#include <piprofiler/ProfilerTraces.h>
#include <piprofiler/ProfilerAttributes.h>

_LIT(KProfilerName,"PIProfilerEngine");

/**
 * The RProfiler class provides a functional interface to the sampling profiler.
 *
 * The engine must already be running for this interface to work, this can be
 * achieved by executing PIProfilerEngine.exe. The control methods are all static, and 
 * require no other context.
 */
NONSHARABLE_CLASS( RProfiler ) : public RSessionBase
	{
public:
	enum TCommand {	
		// profiler controls
		EStartSampling = 1,
		EStartTimedSampling,
		EStopSampling,
//		ELoadSettings,
		EExitProfiler,
		EFinalise,
		EAttachClient,
		ERemoveClient,

		// general attributes
		EGetGeneralAttributes,
		EGetSamplerAttributes,
		EGetSamplerAttributeCount,
		ESetGeneralAttributes,
		ESetSamplerAttributes,
		
		// generic settings
		ESetTraceFilePrefix, 
		ESetTraceFileDrive,
		EGetSamplerVersion,
		EGetFileName,
		EGetActiveWriter,
		ERefreshProfilerStatus
		};
	
	enum TSamplerState {
		EIdle = 0, 
		EInitializing, 
		ERunning, 
		EStopping, 
		EStopped
		};
	
	enum TProfilingMode
	    {
	    EProfilingModeNormal = 0,
	    EProfilingModeTimed
	    };
public:
	
	
	/*
	 * 
	 *  Methods for commanding Profiler Engine
	 *   
	 */
	
	// get general settings
	static inline TInt GetGeneralAttributes(TGeneralAttributes& aAttributes);
	
	// get samplers; names, settings, states, descriptions etc. in one array containing all sampler attributes
	static inline TInt GetSamplerAttributes(CArrayFixFlat<TSamplerAttributes>& aAttributeArray);
	
	// save settings back to engine and plugins
	static inline TInt SetGeneralAttributes(TGeneralAttributes aAttributes);

	// save settings back to engine and plugins
    static inline TInt SetSamplerAttributes(TSamplerAttributes aAttributes);
    
    // refresh profiler engine status
    static inline TInt RefreshProfilerStatus();	
	
    /** Start sampling */
	static inline TInt StartSampling(TProfilingMode aProfilingMode = EProfilingModeNormal);
	/** Stop sampling */
	static inline TInt StopSampling();
    /** Load settings */
//    static inline TInt LoadSettings(TDesC& aSettingsFile);
	/** Exit profiler */
	static inline TInt ExitProfiler();

	/** Get file name */
	static inline TInt GetFileName(TDes&);
    /** Get active writer */
    static inline TInt GetActiveWriter(TDes&);
	/** Perform a test case */
	static inline TInt Test(TUint32 testCase);
	/** Issue a control request to the engine without data*/
	static inline TInt ControlState(TCommand aRequest);
	/** Issue a control request to the engine with descriptor data to write there*/
	static inline TInt ControlWrite(TCommand aRequest,TDesC& data);
	/** Issue a control request to the engine with numeric and descriptor data to write there*/
	static inline TInt ControlWrite(TCommand aRequest,TInt numData, TDesC& data);
	/** Issue a control request to read descriptor data from the engine*/
	static inline TInt ControlRead(TCommand aRequest,TDes& data);
	/** Actually sends the message to profiler engine*/	
	static inline TInt PerformControl(	TCommand aRequest,
										TDesC* fromDescriptor = NULL,
										TUint32 numData1 = 0,
										TDes* toDescriptor = NULL,
										TUint32 numData2 = 0xffffffff);
	
	static inline TInt AttachClient();
	static inline TInt RemoveClient();

private:
	inline RProfiler();
	};

inline RProfiler::RProfiler()
	{}

//
// Connect to the profiler engine, and issue the control request if successful
//
inline TInt RProfiler::ControlState(TCommand aRequest)
    { return PerformControl(aRequest); }

//
// Connect to the profiler engine, and issue the control request if successful
//
inline TInt RProfiler::ControlWrite(TCommand aRequest,TDesC& data)
    { return PerformControl(aRequest,&data,0); }

//
// Connect to the profiler engine, and issue the control request if successful
//
inline TInt RProfiler::ControlWrite(TCommand aRequest,TInt numData, TDesC& data)
    { return PerformControl(aRequest,&data,numData); }

//
// Connect to the profiler engine, and issue the control request if successful
//
inline TInt RProfiler::ControlRead(TCommand aRequest,TDes& data)
    { return PerformControl(aRequest,0,0,&data); }

inline TInt RProfiler::PerformControl(TCommand aRequest,TDesC* fromDescriptor,TUint32 numData1,TDes* toDescriptor,TUint32 numData2)
	{
	LOGTEXT(_L("Creating a session to profiler"));
	RProfiler p;
	TUint count(0);
	TInt r(KErrNone);

	// in boot measurement mode, count until 30s
	#ifdef PROFILER_BOOT_MEASUREMENT
	while(count < 60)
	#else
	while(count < 6)
	#endif
		{
		r = p.CreateSession(KProfilerName, TVersion(), 0);
		if (r == KErrNone)
			{
			LOGSTRING2("Succeeded, sending a message %d", aRequest);
			LOGSTRING5(" - parameters 0x%x 0x%x 0x%x 0x%x",fromDescriptor,numData1,toDescriptor,numData2);
			TInt err = KErrNone;

			TIpcArgs a;
			a.Set(0,fromDescriptor);
			a.Set(1,numData1);
			a.Set(2,toDescriptor);
			a.Set(3,numData2);
			err = p.SendReceive(aRequest,a);

			p.RSessionBase::Close();
			
			if(err != KErrNone)
				{
				LOGSTRING2("Profiler responded with an error - code %d !!",err);		
				return err;
				}
			else
				{ 
				LOGTEXT(_L("OK, message sent, closing"));
				return KErrNone;
				}
			}

		LOGSTRING2("Error in opening session to profiler - code %d !!",r);
		//#ifdef PROFILER_BOOT_MEASUREMENT
		// there was an error contacting the Profiler
		// indicates that the server is most probably not up
		// however, since it should be(unless some resource is not yet ready)
		// we can just wait
		User::After(500000); // wait 1/2 s
		count++;
		//#else
		// exit immediately on error
		//return r;
		//#endif
		}
	return r;
	}

// the new UI access methods
inline TInt RProfiler::GetGeneralAttributes(TGeneralAttributes& aAttributes)
    {
#ifdef _TEST_
    _LIT(KDefaultTraceOutput,"debug_output");
    _LIT(KDefaultTraceFilePrefix,"PIProfiler_#");
    _LIT(KDefaultTraceFileSaveDrive,"E:\\");
    aAttributes.iTraceOutput.Copy(KDefaultTraceOutput);
    aAttributes.iTraceFilePrefix.Copy(KDefaultTraceFilePrefix);
    aAttributes.iSaveFileDrive.Copy(KDefaultTraceFileSaveDrive);
#else
    LOGTEXT(_L("Creating a session to profiler"));
    RProfiler p;
    TInt r(KErrNone);
    r = p.CreateSession(KProfilerName, TVersion(), 0);
    if (r == KErrNone)
        {
        LOGSTRING2("Succeeded, sending a message %d", EGetGeneralAttributes);

        TPckg<TGeneralAttributes> attrPckg(aAttributes);
        TIpcArgs a(&attrPckg);
        r = p.SendReceive(RProfiler::EGetGeneralAttributes,a);
        
        p.RSessionBase::Close();
        
        if(r != KErrNone)
            {
            LOGSTRING2("Profiler responded with an error - code %d !!",r);        
            return r;
            }
        else
            { 
            LOGTEXT(_L("OK, message sent, closing"));
            return KErrNone;
            }
        }
#endif
    return r;   // return error code  
    }
inline TInt RProfiler::GetSamplerAttributes(CArrayFixFlat<TSamplerAttributes>& aAttributes)
    {
#ifdef _TEST_
    _LIT(KDefaultTraceOutput,"debug_output");
    _LIT(KDefaultTraceFilePrefix,"PIProfiler_#");
    _LIT(KDefaultTraceFileSaveDrive,"E:\\");
    aAttributes.iTraceOutput.Copy(KDefaultTraceOutput);
    aAttributes.iTraceFilePrefix.Copy(KDefaultTraceFilePrefix);
    aAttributes.iSaveFileDrive.Copy(KDefaultTraceFileSaveDrive);
#else
    // do receive stream of TSamplerAttributes
    LOGTEXT(_L("Creating a session to profiler"));
    RProfiler p;
    TInt r(KErrNone);
    r = p.CreateSession(KProfilerName, TVersion(), 0);
    if (r == KErrNone)
        {
        TInt attrCount(0);
        TPckg<TInt> pckg(attrCount);
        TIpcArgs args(&pckg);
        
        r = p.SendReceive(RProfiler::EGetSamplerAttributeCount, args);
        
        HBufC8* buffer = HBufC8::NewL(attrCount*sizeof(TSamplerAttributes));
        TPtr8 ptr(buffer->Des());
        TIpcArgs args2(&ptr);
        r = p.SendReceive(RProfiler::EGetSamplerAttributes, args2);
        
        TInt len(ptr.Length());
        TInt pos(0);
        while (pos != len)
           {
           TPckgBuf<TSamplerAttributes> attrPckg;
           attrPckg.Copy(ptr.Mid(pos, attrPckg.Length()));
           pos += attrPckg.Length();
    
           aAttributes.AppendL(attrPckg());
           }
        
        p.RSessionBase::Close();
        
        if(r != KErrNone)
            {
            LOGSTRING2("Profiler responded with an error - code %d !!",r);        
            return r;
            }
        else
            { 
            LOGTEXT(_L("OK, message sent, closing"));
            return KErrNone;
            }
        }
#endif
    return KErrNone; 
    }

inline TInt RProfiler::SetGeneralAttributes(TGeneralAttributes aAttributes)
    {
    // do receive stream of TSamplerAttributes
    LOGTEXT(_L("Creating a session to profiler"));
    RProfiler p;
    TInt r(KErrNone);
    r = p.CreateSession(KProfilerName, TVersion(), 0);
    if (r == KErrNone)
        {

        TPckg<TGeneralAttributes> attrPckg(aAttributes);
        TIpcArgs a(&attrPckg);
        r = p.SendReceive(RProfiler::ESetGeneralAttributes,a);
        
        p.RSessionBase::Close();
        
        if(r != KErrNone)
            {
            LOGSTRING2("Profiler responded with an error - code %d !!",r);        
            return r;
            }
        else
            { 
            LOGTEXT(_L("OK, message sent, closing"));
            return KErrNone;
            }
        }
    return r; 
    }

// save settings back to engine and plugins
inline TInt RProfiler::SetSamplerAttributes(TSamplerAttributes aAttributes)
    {
    // do receive stream of TSamplerAttributes
    LOGTEXT(_L("Creating a session to profiler"));
    RProfiler p;
    TInt r(KErrNone);
    r = p.CreateSession(KProfilerName, TVersion(), 0);
    if (r == KErrNone)
        {

        TPckg<TSamplerAttributes> attrPckg(aAttributes);
        
        TIpcArgs a;
        a.Set(0,&attrPckg);

        r = p.SendReceive(RProfiler::ESetSamplerAttributes,a);
        
        p.RSessionBase::Close();
        
        if(r != KErrNone)
            {
            LOGSTRING2("Profiler responded with an error - code %d !!",r);        
            return r;
            }
        else
            { 
            LOGTEXT(_L("OK, message sent, closing"));
            return KErrNone;
            }
        }
    return r; 
    }

inline TInt RProfiler::RefreshProfilerStatus()
    {return ControlState(RProfiler::ERefreshProfilerStatus); }

inline TInt RProfiler::StartSampling(RProfiler::TProfilingMode aProfilingMode)
	{
    RProfiler::TCommand command = RProfiler::EStartSampling;
    if( aProfilingMode == RProfiler::EProfilingModeTimed )
        {
        command = RProfiler::EStartTimedSampling;
        }    
    return ControlState(command);
	}

inline TInt RProfiler::StopSampling()
	{return ControlState(RProfiler::EStopSampling);}

inline TInt RProfiler::ExitProfiler()
	{return ControlState(RProfiler::EExitProfiler);}

inline TInt RProfiler::AttachClient()
    {return ControlState(RProfiler::EAttachClient);}

inline TInt RProfiler::RemoveClient()
    {return ControlState(RProfiler::ERemoveClient);}

inline TInt RProfiler::GetFileName(TDes& fileName)
	{return ControlRead(EGetFileName,fileName);}

inline TInt RProfiler::GetActiveWriter(TDes& writerDes)
    {return ControlRead(EGetActiveWriter,writerDes);}

#endif // __PROFILER_SESSION_H__
