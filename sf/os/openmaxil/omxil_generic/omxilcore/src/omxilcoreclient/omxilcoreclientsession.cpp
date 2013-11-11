/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <openmax/il/khronos/v1_x/OMX_Core.h>

#include "log.h"
#include "omxilcoreclientsession.h"
#include "omxilcoreclientserver.h"
#include "omxilcore.h"

#ifdef __WINSCW__
#include <pls.h>
#include "../core/omxiluids.hrh"
#endif

#ifdef SYMBIAN_PERF_TRACE_OMX_IL
#include "OSTOMXILFrameworkTrace.h"
#endif


#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1

#include "../../traces/OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "omxilcoreclientsessionTraces.h"
#endif
#endif


_LIT(KOmxILCoreClientPanic, "OmxILCoreClient Panic");
const TInt KOpenSeverSessionMaxRetries = 3;

namespace
	{
/*
  This class is used to provide a global cache of a reference to the COmxILCore
  object.  A mutex is provided to serialize the access to the IL Core
  reference. A global object of this class is intended to be used only through
  the IL Core Client library which in turn is used by any thread in the process
  that needs to access the OpenMAX IL Core services.

*/
	class XGlobalILCoreCache
		{
	public:
		inline XGlobalILCoreCache();
		inline ~XGlobalILCoreCache();

		inline static XGlobalILCoreCache* IlCoreCache();
		inline void SetILCore(COmxILCore* aILCore);
		inline COmxILCore* ILCore();

		inline RServer2 &ServerHandle();
		inline TInt SetServerHandle(TUint32 aServerHandle);
		
		inline void Lock();
		inline void Unlock();

	private:

		RMutex iMutex;
		TInt iError;
		COmxILCore* iILCore;
		RServer2 iServerHandle;
		};

#ifndef __WINSCW__
XGlobalILCoreCache gGlobalILCoreCache;
#endif

	} // unnamed namespace

XGlobalILCoreCache* XGlobalILCoreCache::IlCoreCache()
	{
#ifdef __WINSCW__
	const TUid KUidOmxILCoreClientDllUid = { KUidOmxILCoreClientDll };
	XGlobalILCoreCache* pGlobalIlCoreCache = 0;
	if (NULL == (pGlobalIlCoreCache =
				 Pls<XGlobalILCoreCache>(KUidOmxILCoreClientDllUid, NULL)))
		{
		return NULL;
		}

	XGlobalILCoreCache& gGlobalILCoreCache = *pGlobalIlCoreCache;
#endif
	return (gGlobalILCoreCache.iError == KErrNone) ? &gGlobalILCoreCache : NULL;
	}

inline XGlobalILCoreCache::XGlobalILCoreCache()
	: iMutex(), iError(KErrNone), iILCore(NULL), iServerHandle()
	{
    DEBUG_PRINTF(_L8("XGlobalILCoreCache::XGlobalILCoreCache"));
	iError = iMutex.CreateLocal(EOwnerProcess);
	}

inline XGlobalILCoreCache::~XGlobalILCoreCache()
	{
    DEBUG_PRINTF(_L8("XGlobalILCoreCache::~XGlobalILCoreCache"));
	iMutex.Close();
	iServerHandle.Close();
	}

inline COmxILCore* XGlobalILCoreCache::ILCore()
	{
	__ASSERT_ALWAYS(iError == KErrNone,
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));

	return iILCore;
	}

inline void XGlobalILCoreCache::SetILCore(COmxILCore* aILCore)
	{
	__ASSERT_ALWAYS(iError == KErrNone,
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));
	__ASSERT_ALWAYS(iMutex.IsHeld(),
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));

	iILCore = aILCore;
	}

inline RServer2 &XGlobalILCoreCache::ServerHandle()
	{
	return iServerHandle;
	}

inline TInt XGlobalILCoreCache::SetServerHandle(TUint32 aServerHandle)
	{
	iServerHandle.Close();
	iServerHandle.SetHandle(aServerHandle);
	return KErrNone;
	}

inline void XGlobalILCoreCache::Lock()
	{
	__ASSERT_ALWAYS(iError == KErrNone,
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));
	iMutex.Wait();
	}

inline void XGlobalILCoreCache::Unlock()
	{
	__ASSERT_ALWAYS(iError == KErrNone,
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));
	__ASSERT_ALWAYS(iMutex.IsHeld(),
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));
	iMutex.Signal();
	}


ROmxILCoreClientSession::ROmxILCoreClientSession()
	{
    DEBUG_PRINTF(_L8("ROmxILCoreClientSession::ROmxILCoreClientSession"));
	}

/**
 *
 */
ROmxILCoreClientSession::~ROmxILCoreClientSession()
	{
    DEBUG_PRINTF(_L8("ROmxILCoreClientSession::~ROmxILCoreClientSession"));

	}

/**
   Starts the separate IL Core server thread if not already started

   @return KErrAlreadyExists if the IL Core server has already been
   started. System-wide error if the creation of the session with the IL Core
   server did not succeed.
*/
TInt ROmxILCoreClientSession::Connect()
	{
    DEBUG_PRINTF(_L8("ROmxILCoreClientSession::Connect"));
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
        OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
            Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_Init)); 
#endif

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _Connect1, "OMX_Init >" ); 
#endif 
    
    TInt err = KErrNotFound;
	XGlobalILCoreCache *glbCache = XGlobalILCoreCache::IlCoreCache();
	if(!glbCache)
	    {
	    err = KErrGeneral;
	    }
	else
	    {
        if(glbCache->ServerHandle().Handle() != KNullHandle)
            {
            err = CreateSession(glbCache->ServerHandle(), TVersion(1,0,0));
            }
        
           if(err == KErrNotFound)
                {
                // Server not running
                TUint32 serverHandle = KNullHandle;
                if (KErrNone == (err = StartOmxILCoreServer(&serverHandle)))
                    {
                    if (KErrNone == (err = glbCache->SetServerHandle(serverHandle) ))
                        {
                        if(glbCache->ServerHandle().Handle() == KNullHandle)
                            {
                            err = KErrNotFound;
                            }
                        else
                            {
                            err = CreateSession(glbCache->ServerHandle(), TVersion(1,0,0));
                            }
                        }
                    }
                }
            else
                {
                // The server exists already... close the session and return...
                RHandleBase::Close();
                err = KErrAlreadyExists;
                }
	    }

#if defined(SYMBIAN_PERF_TRACE_OMX_IL) || defined(SYMBIAN_PERF_TRACE_OMX_IL_OSTV1)
	OMX_ERRORTYPE omxError = OMX_ErrorNone;
	if(KErrNone != err)
	    {
	    omxError = OMX_ErrorUndefined;
	    }
#endif
	
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    TBufC8<1> InitStr;
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
            Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_Init),omxError,&InitStr); 
#endif
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace1( TRACE_API, _Connect2, "OMX_Init < ReturnVal=%u", omxError );
#endif     
    
    return err;	

	}

/**
   Opens a session to the IL Core server thread that should already be started

   @return KErrNone if successful, otherwise one of the other system-wide error
   codes
*/
TInt ROmxILCoreClientSession::Open()
	{
    DEBUG_PRINTF(_L8("ROmxILCoreClientSession::Open"));

	XGlobalILCoreCache *glbCache = XGlobalILCoreCache::IlCoreCache();
    if(glbCache->ServerHandle().Handle() == KNullHandle)
        {
        return KErrNotFound;
        }
	return CreateSession(glbCache->ServerHandle(), TVersion(1,0,0));

	}

/**
   Closes the session to the IL Core server thread.
*/
void ROmxILCoreClientSession::Close()
	{
    DEBUG_PRINTF(_L8("ROmxILCoreClientSession::Close"));

	RHandleBase::Close();

	}

/**
   Requests initialization of the OpenMAX IL Core loaders.

   @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE ROmxILCoreClientSession::ListLoaders()
	{
    DEBUG_PRINTF(_L8("ROmxILCoreClientSession::ListLoaders"));

	// Param 0
	OMX_ERRORTYPE err = OMX_ErrorNone;
    TPckgBuf<OMX_ERRORTYPE> pckg0;

	TIpcArgs arg(&pckg0);
	SendReceive(EOmxILCoreListLoaders, arg);

	// Extract the output values returned from the server.
	err = pckg0();
    return err;

	}

/**
   Requests de-initialization of the OpenMAX IL core. It closes the session to
   the IL Core server.

   @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE ROmxILCoreClientSession::DeinitAndClose()
	{
    DEBUG_PRINTF(_L8("ROmxILCoreClientSession::DeinitAndClose"));
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
        OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
            Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_DeInit)); 
#endif
   
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _DeinitAndClose1, "OMX_Deinit >" );
#endif
        
	// Param 0
	OMX_ERRORTYPE err = OMX_ErrorNone;
    TPckgBuf<OMX_ERRORTYPE> pckg0;

	// Param 1
	TUint64 serverThreadId = 0;
    TPckgBuf<TUint64> pckg1;

	TIpcArgs arg(&pckg0, &pckg1);
	SendReceive(EOmxILCoreDeinit, arg);

	// Extract the output values returned from the server.
	err = pckg0();
	serverThreadId = pckg1();

	RHandleBase::Close();

	// Release server handle so it will exit cleanly.
	XGlobalILCoreCache* pGlobalILCoreCache = XGlobalILCoreCache::IlCoreCache();

	__ASSERT_ALWAYS(pGlobalILCoreCache != NULL,
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));

	pGlobalILCoreCache->SetServerHandle(KNullHandle);
	
	RThread serverThread;
	TInt ret = serverThread.Open(TThreadId(serverThreadId));

	if ((KErrNone == ret) && (serverThread.Handle() != KNullHandle))
		{
		DEBUG_PRINTF2(_L8("ROmxILCoreClientSession::DeinitAndClose : serverThread.Handle =[%d]"), serverThread.Handle());
		TBool logoffFailed = EFalse;
		TRequestStatus logoffStatus;
		serverThread.Logon(logoffStatus);

		if (logoffStatus == KErrNoMemory)
			{
			logoffFailed = ETrue;
			}

		if (!logoffFailed)
			{
			if (logoffStatus == KRequestPending)
				{
				User::WaitForRequest(logoffStatus);
				}
			else
				{
				serverThread.LogonCancel(logoffStatus);
				User::WaitForRequest(logoffStatus);
				}
			}
		else
			{
			serverThread.Kill(KErrDied);
			}
		}

	serverThread.Close();

#if defined(SYMBIAN_PERF_TRACE_OMX_IL) || defined(SYMBIAN_PERF_TRACE_OMX_IL_OSTV1)
	OMX_ERRORTYPE omxError = OMX_ErrorNone;
    if(KErrNone != err)
        {
        omxError = OMX_ErrorUndefined;
        }	
#endif
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    TBuf8<1> DeInitStr;
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
            Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_DeInit), omxError,&DeInitStr); 
#endif

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace1( TRACE_API, _DeinitAndClose2, "OMX_Deinit < ReturnVal=%u", omxError );
#endif 
    
    return err;

	}


/**
   Obtains the IL Core pointer stored in the IL Core thread.

   @return COmxILCore&

 */
COmxILCore& ROmxILCoreClientSession::ILCore()
	{
    DEBUG_PRINTF(_L8("ROmxILCoreClientSession::ILCore"));

	// Param 0
    TPckgBuf<OMX_ERRORTYPE> pckg0;

    TPckgBuf<COmxILCore*> pckg1;

	TIpcArgs arg(&pckg0, &pckg1);
	SendReceive(EOmxILCoreGetILCore, arg);

	// Extract the output values returned from the server.
	// OMX error is not relevant...
	__ASSERT_ALWAYS(pckg1() != NULL,
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));

	return *pckg1();

	}


/**
   Requests OMX_ComponentNameEnum to the OpenMAX IL core.

   @return OMX_ERRORTYPE

 */
OMX_ERRORTYPE ROmxILCoreClientSession::ComponentNameEnum(
	    OMX_STRING cComponentName,
		OMX_U32 nNameLength,
		OMX_U32 nIndex)
	{
    DEBUG_PRINTF(_L8("ROmxILCoreClientSession::ComponentNameEnum"));

	// Param 0
	OMX_ERRORTYPE err = OMX_ErrorNone;
    TPckgBuf<OMX_ERRORTYPE> pckg0;

	// Param 1
	TComponentNameEnum arg1;
	arg1.cComponentName = cComponentName;
	arg1.nNameLength	= nNameLength;
	arg1.nIndex			= nIndex;

	TIpcArgs arg(&pckg0, &arg1);
	SendReceive(EOmxILCoreComponentNameEnum, arg);

	// Extract the output values returned from the server.
	err = pckg0();
    return err;

	}

/**
   Requests OMX_GetHandle to the OpenMAX IL core.

   @return OMX_ERRORTYPE

 */
OMX_ERRORTYPE ROmxILCoreClientSession::GetHandle(
	OMX_HANDLETYPE* pHandle,
	OMX_STRING cComponentName,
	OMX_PTR pAppData,
	OMX_CALLBACKTYPE* pCallBacks)
	{
    DEBUG_PRINTF(_L8("ROmxILCoreClientSession::GetHandle"));

	// Param 0
	OMX_ERRORTYPE err = OMX_ErrorNone;
    TPckgBuf<OMX_ERRORTYPE> pckg0;


	// Param 1
	TGetHandle arg1;
	arg1.pHandle		= pHandle;
	arg1.cComponentName = cComponentName;
	arg1.pAppData		= pAppData;
	arg1.pCallBacks		= pCallBacks;

	TIpcArgs arg(&pckg0, &arg1);
	SendReceive(EOmxILCoreGetHandle, arg);

	// Extract the output values returned from the server.
	err = pckg0();
	return err;

	}

/**
   Requests OMX_FreeHandle to the OpenMAX IL core.

   @return OMX_ERRORTYPE

 */
OMX_ERRORTYPE ROmxILCoreClientSession::FreeHandle(
	OMX_HANDLETYPE hComponent)
	{
	DEBUG_PRINTF(_L8("ROmxILCoreClientSession::FreeHandle"));

	// Param 0
	OMX_ERRORTYPE err = OMX_ErrorNone;
    TPckgBuf<OMX_ERRORTYPE> pckg0;

	TIpcArgs arg(&pckg0, hComponent);
	SendReceive(EOmxILCoreFreeHandle, arg);

	// Extract the output values returned from the server.
	err = pckg0();
	return err;

	}

/**
   Requests OMX_SetupTunnel to the OpenMAX IL core.

   @return OMX_ERRORTYPE

 */
OMX_ERRORTYPE ROmxILCoreClientSession::SetupTunnel(
	OMX_HANDLETYPE hOutput,
	OMX_U32 nPortOutput,
	OMX_HANDLETYPE hInput,
	OMX_U32 nPortInput)
	{
	DEBUG_PRINTF(_L8("ROmxILCoreClientSession::SetupTunnel"));

	// Param 0
	OMX_ERRORTYPE err = OMX_ErrorNone;
    TPckgBuf<OMX_ERRORTYPE> pckg0;

	TSetupTunnel arg1;
	arg1.hOutput	 = hOutput;
	arg1.nPortOutput = nPortOutput;
	arg1.hInput		 = hInput;
	arg1.nPortInput	 = nPortInput;

	TIpcArgs arg(&pckg0, &arg1);
	SendReceive(EOmxILCoreSetupTunnel, arg);

	// Extract the output values returned from the server.
	err = pckg0();
	return err;

	}

/**
   Requests OMX_GetContentPipe to the OpenMAX IL core.

   @return OMX_ERRORTYPE

 */
OMX_ERRORTYPE ROmxILCoreClientSession::GetContentPipe(
	OMX_HANDLETYPE* hPipe,
	OMX_STRING szURI)
	{
	DEBUG_PRINTF(_L8("ROmxILCoreClientSession::GetContentPipe"));

	// Param 0
	OMX_ERRORTYPE err = OMX_ErrorNone;
    TPckgBuf<OMX_ERRORTYPE> pckg0;

	// Param 1
	TGetContentPipe arg1;
	arg1.hPipe = hPipe;
	arg1.szURI = szURI;

	TIpcArgs arg(&pckg0, &arg1);
	SendReceive(EOmxILCoreGetContentPipe, arg);

	// Extract the output values returned from the server.
	err = pckg0();
	return err;

	}

/**
   Requests OMX_GetComponentsOfRole to the OpenMAX IL core.

   @return OMX_ERRORTYPE

 */
OMX_ERRORTYPE ROmxILCoreClientSession::GetComponentsOfRole(
	OMX_STRING role,
	OMX_U32* pNumComps,
	OMX_U8** compNames)
	{
	DEBUG_PRINTF(_L8("ROmxILCoreClientSession::GetComponentsOfRole"));

	// Param 0
	OMX_ERRORTYPE err = OMX_ErrorNone;
    TPckgBuf<OMX_ERRORTYPE> pckg0;

	TGetComponentsOfRole arg1;
	arg1.role	   = role;
	arg1.pNumComps = pNumComps;
	arg1.compNames = compNames;

	TIpcArgs arg(&pckg0, &arg1);
	SendReceive(EOmxILCoreGetComponentsOfRole, arg);

	// Extract the output values returned from the server.
	err = pckg0();
	return err;

	}

/**
   Requests OMX_GetRolesOfComponent to the OpenMAX IL core.

   @return OMX_ERRORTYPE

 */
OMX_ERRORTYPE ROmxILCoreClientSession::GetRolesOfComponent(
	OMX_STRING compName,
	OMX_U32 *pNumRoles,
	OMX_U8 **roles)
	{
	DEBUG_PRINTF(_L8("ROmxILCoreClientSession::GetRolesOfComponent"));

	// Param 0
	OMX_ERRORTYPE err = OMX_ErrorNone;
    TPckgBuf<OMX_ERRORTYPE> pckg0;

	TGetRolesOfComponent arg1;
	arg1.compName  = compName;
	arg1.pNumRoles = pNumRoles;
	arg1.roles	   = roles;

	TIpcArgs arg(&pckg0, &arg1);
	SendReceive(EOmxILCoreGetRolesOfComponent, arg);

	// Extract the output values returned from the server.
	err = pckg0();
	return err;

	}


//
// OMX IL Operations...
//

/**
   The OMX_Init method initializes the OpenMAX IL core. OMX_Init shall be the
   first call made into OpenMAX IL and should be executed only one time without
   an intervening OMX_Deinit call. If OMX_Init is called twice, OMX_ErrorNone
   is returned but the init request is ignored.

   @return OMX_ERRORTYPE
 */
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Init()
	{
    XGlobalILCoreCache* pGlobalILCoreCache = XGlobalILCoreCache::IlCoreCache();
	__ASSERT_ALWAYS(pGlobalILCoreCache != NULL,
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));
	pGlobalILCoreCache->Lock();

	// OMX_Init may be called several times, only the first time must have an
	// effect....
	OMX_ERRORTYPE omxError = OMX_ErrorNone;
	if (pGlobalILCoreCache->ILCore() != NULL)
		{
		omxError = OMX_ErrorNone;
		}
	else
		{
		// Connect will create the server side thread the first time it is
		// called... The IL Core object will be instantiated at the sever
		// side...
		ROmxILCoreClientSession coresession;
		TInt err = coresession.Connect();
		// There should not exist a client thread already created at this
		// point...
		__ASSERT_ALWAYS(err != KErrAlreadyExists,
						User::Panic(KOmxILCoreClientPanic,
									KErrNotReady));
		if (KErrNone == err)
			{
			// Obtain the ILCore from the server side...
			COmxILCore& ilCore = coresession.ILCore();
			//... and cache it globally so the client library doesn't need to
			// access the server every time the IL Core is needed...
			pGlobalILCoreCache->SetILCore(&ilCore);

			// This will call COmxILCore::ListLoaders()...We want to initialize
			// loader ecom plugins in the IL Core thread, so we can make sure
			// they get destroyed in the same thread, the IL Core thread.
			omxError = coresession.ListLoaders();
			}
		else
			{
			// This is to make sure we return something that conforms with
			// table 3-9 in the spec.
			switch(err)
				{
			case KErrTimedOut:
				{
				omxError = OMX_ErrorTimeout;
				}
				break;
			case KErrNoMemory:
			default:
				{
				omxError = OMX_ErrorInsufficientResources;
				}
				};
			}
		coresession.Close();

		if (omxError != OMX_ErrorNone)
			{
			ROmxILCoreClientSession coresession;
			TInt openRetryCounter = KOpenSeverSessionMaxRetries;
			TInt err = KErrNone;
			// Try to connect to the server several times...
			do
				{
				err = coresession.Open();
				}
			while(KErrNone != err && --openRetryCounter > 0);

			if (KErrNone == err)
				{
				// IL Core deinitialization in the server thread to make sure that
				// loader plugins are destroyed in the same thread...
				coresession.DeinitAndClose();
				// At this point, the IL Core has been successfully
				// deinitialized...remove the cached reference...
				pGlobalILCoreCache->SetILCore(NULL);
				}

			coresession.Close();
			}
		}

	pGlobalILCoreCache->Unlock();
	return omxError;

	}

/**
   The OMX_Deinit method de-initializes the OpenMAX core. OMX_Deinit should be
   the last call made into the OpenMAX core after all OpenMAX-related resources have
   been released

   @return OMX_ERRORTYPE
 */
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Deinit()
	{
    XGlobalILCoreCache* pGlobalILCoreCache = XGlobalILCoreCache::IlCoreCache();
	__ASSERT_ALWAYS(pGlobalILCoreCache != NULL,
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));
	pGlobalILCoreCache->Lock();

	// OMX_Deinit may be called several times.... only the first time will
	// remove the IL Core and the remaining ones must simply have no effect...
	OMX_ERRORTYPE omxError = OMX_ErrorNone;
	if (pGlobalILCoreCache->ILCore() == NULL)
		{
		omxError = OMX_ErrorNone;
		}
	else
		{
		ROmxILCoreClientSession coresession;
		TInt openRetryCounter = KOpenSeverSessionMaxRetries;
		TInt err = KErrNone;
		// Try to connect to the server several times...
		do
			{
			err = coresession.Open();
			__ASSERT_ALWAYS(KErrNotFound != err,
							User::Panic(KOmxILCoreClientPanic,
										KErrNotReady));
			}
		while(KErrNone != err && --openRetryCounter > 0);

		if (KErrNone == err)
			{
			// IL Core deinitialization in the server thread to make sure that
			// loader plugins are destroyed in the same thread...
			omxError = coresession.DeinitAndClose();
			// At this point, the IL Core has been successfully
			// deinitialized...remove the cached reference...
			if (omxError == OMX_ErrorNone)
				{
				pGlobalILCoreCache->SetILCore(NULL);
				}
			}
		else
			{
			// This is to make sure we return something that conforms with
			// table 3-9 in the spec.
			switch(err)
				{
			case KErrTimedOut:
				{
				omxError = OMX_ErrorTimeout;
				}
				break;
			default:
				{
				omxError = OMX_ErrorNone;
				}
				};
			}

		coresession.Close();
		}

	pGlobalILCoreCache->Unlock();
	return omxError;

	}

/**
   The OMX_ComponentNameEnum method will enumerate through all the names of
   recognized components in the system to detect all the components in the system
   run-time.

   @return OMX_ERRORTYPE
 */
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_ComponentNameEnum(
	    OMX_OUT OMX_STRING cComponentName,
	    OMX_IN  OMX_U32 nNameLength,
	    OMX_IN  OMX_U32 nIndex)
	{
    XGlobalILCoreCache* pGlobalILCoreCache = XGlobalILCoreCache::IlCoreCache();
	__ASSERT_ALWAYS(pGlobalILCoreCache != NULL,
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));
	pGlobalILCoreCache->Lock();

	// OpenMAX IL mandates that OMX_Init must be the first OMX call made into
	// the IL Core...
	__ASSERT_ALWAYS(pGlobalILCoreCache->ILCore() != NULL,
					User::Panic(KOmxILCoreClientPanic,
								KErrNotReady));


	ROmxILCoreClientSession coresession;
	OMX_ERRORTYPE omxError = OMX_ErrorNone;
	TInt err = coresession.Open();
	__ASSERT_ALWAYS(KErrNotFound != err,
					User::Panic(KOmxILCoreClientPanic,
								KErrNotReady));

	// As mandated by the spec, we list and initialize loaders if the index is
	// 0.
	if (0 == nIndex && KErrNone == err)
		{
		// Ignore if any error...
		coresession.ListLoaders();
		}

	if (CActiveScheduler::Current())
		{
		COmxILCore& ilCore = *pGlobalILCoreCache->ILCore();
		omxError = ilCore.ComponentNameEnum(
			cComponentName,
			nNameLength,
			nIndex);
		}
	else
		{
		if (KErrNone == err)
			{
			omxError = coresession.ComponentNameEnum(
				cComponentName,
				nNameLength,
				nIndex);
			}
		else
			{
			omxError = OMX_ErrorUndefined;
			}

		}

	coresession.Close();
	pGlobalILCoreCache->Unlock();
	return omxError;

	}

/**
   The OMX_GetHandle method will locate the component specified by the
   component name given, load that component into memory, and validate it.

   @return OMX_ERRORTYPE
 */
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_GetHandle(
	    OMX_OUT OMX_HANDLETYPE* pHandle,
	    OMX_IN  OMX_STRING cComponentName,
	    OMX_IN  OMX_PTR pAppData,
	    OMX_IN  OMX_CALLBACKTYPE* pCallBacks)
	{
    XGlobalILCoreCache* pGlobalILCoreCache = XGlobalILCoreCache::IlCoreCache();
	__ASSERT_ALWAYS(pGlobalILCoreCache != NULL,
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));
	pGlobalILCoreCache->Lock();

	// OpenMAX IL mandates that OMX_Init must be the first OMX call made into
	// the IL Core...
	__ASSERT_ALWAYS(pGlobalILCoreCache->ILCore() != NULL,
					User::Panic(KOmxILCoreClientPanic,
								KErrNotReady));

	OMX_ERRORTYPE omxError = OMX_ErrorNone;
	if (CActiveScheduler::Current())
		{
		COmxILCore& ilCore = *pGlobalILCoreCache->ILCore();
		omxError = ilCore.LoadComponent(
			cComponentName,
			pHandle,
			pAppData,
			pCallBacks);
		}
	else
		{
		ROmxILCoreClientSession coresession;
		TInt err = coresession.Open();
		// OMX_Init must be the first OMX call
		__ASSERT_ALWAYS(KErrNotFound != err,
						User::Panic(KOmxILCoreClientPanic, KErrNotReady));

		if (KErrNone == err)
			{
			omxError = coresession.GetHandle(
				pHandle,
				cComponentName,
				pAppData,
				pCallBacks);
			}
		else
			{
			omxError = OMX_ErrorUndefined;
			}

		coresession.Close();
		}

	pGlobalILCoreCache->Unlock();
	return omxError;

	}

/**
   The OMX_FreeHandle method will free a handle allocated by the OMX_GetHandle
   method.

   @return OMX_ERRORTYPE
*/
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_FreeHandle(
	OMX_IN  OMX_HANDLETYPE hComponent)
	{
    XGlobalILCoreCache* pGlobalILCoreCache = XGlobalILCoreCache::IlCoreCache();
	__ASSERT_ALWAYS(pGlobalILCoreCache != NULL,
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));
	pGlobalILCoreCache->Lock();

	// OpenMAX IL mandates that OMX_Init must be the first OMX call made into
	// the IL Core...
	__ASSERT_ALWAYS(pGlobalILCoreCache->ILCore() != NULL,
					User::Panic(KOmxILCoreClientPanic,
								KErrNotReady));

	OMX_ERRORTYPE omxError = OMX_ErrorNone;
	if (CActiveScheduler::Current())
		{
		COmxILCore& ilCore = *pGlobalILCoreCache->ILCore();
		omxError = ilCore.FreeHandle(hComponent);
		}
	else
		{
		ROmxILCoreClientSession coresession;
		TInt err = coresession.Open();
		__ASSERT_ALWAYS(KErrNotFound != err,
						User::Panic(KOmxILCoreClientPanic,
									KErrNotReady));
		if (KErrNone == err)
			{
			omxError = coresession.FreeHandle(hComponent);
			}
		else
			{
			omxError = OMX_ErrorUndefined;
			}

		coresession.Close();
		}

	pGlobalILCoreCache->Unlock();
	return omxError;

	}

/**
   The OMX_SetupTunnel method sets up tunneled communication between an output
   port and an input port.

   @return OMX_ERRORTYPE
 */
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_SetupTunnel(
	OMX_IN  OMX_HANDLETYPE hOutput,
	OMX_IN  OMX_U32 nPortOutput,
	OMX_IN  OMX_HANDLETYPE hInput,
	OMX_IN  OMX_U32 nPortInput)
	{
    XGlobalILCoreCache* pGlobalILCoreCache = XGlobalILCoreCache::IlCoreCache();
	__ASSERT_ALWAYS(pGlobalILCoreCache != NULL,
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));
	pGlobalILCoreCache->Lock();

	// OpenMAX IL mandates that OMX_Init must be the first OMX call made into
	// the IL Core...
	__ASSERT_ALWAYS(pGlobalILCoreCache->ILCore() != NULL,
					User::Panic(KOmxILCoreClientPanic,
								KErrNotReady));

	// SetupTunnel is reentrant and thus thread-safe, so no need to lock the
	// IL Core instance...
	pGlobalILCoreCache->Unlock();

	OMX_ERRORTYPE omxError = OMX_ErrorNone;
	if (CActiveScheduler::Current())
		{
		omxError = COmxILCore::SetupTunnel(
			hOutput,
			nPortOutput,
			hInput,
			nPortInput);
		}
	else
		{
		ROmxILCoreClientSession coresession;
		TInt err = coresession.Open();
		__ASSERT_ALWAYS(KErrNotFound != err,
						User::Panic(KOmxILCoreClientPanic,
									KErrNotReady));
		if (KErrNone == err)
			{
			omxError = coresession.SetupTunnel(
				hOutput,
				nPortOutput,
				hInput,
				nPortInput);
			}
		else
			{
			// This is to make sure we return something that conforms with
			// table 3-9 in the spec. Cannot return OMX_ErrorUndefined
			omxError = OMX_ErrorTimeout;
			}

		coresession.Close();
		}
	return omxError;

	}

/**
   The OMX_GetContentPipe method returns a content pipe capable of manipulating a
   given piece of content as (specified via URI)

   @return OMX_ERRORTYPE
 */
OMX_API OMX_ERRORTYPE   OMX_GetContentPipe(
	OMX_OUT OMX_HANDLETYPE* hPipe,
	OMX_IN OMX_STRING szURI)
	{
    XGlobalILCoreCache* pGlobalILCoreCache = XGlobalILCoreCache::IlCoreCache();
	__ASSERT_ALWAYS(pGlobalILCoreCache != NULL,
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));
	pGlobalILCoreCache->Lock();

	// OpenMAX IL mandates that OMX_Init must be the first OMX call made into
	// the IL Core...
	__ASSERT_ALWAYS(pGlobalILCoreCache->ILCore() != NULL,
					User::Panic(KOmxILCoreClientPanic,
								KErrNotReady));

	OMX_ERRORTYPE omxError = OMX_ErrorNone;
	if (CActiveScheduler::Current())
		{
		COmxILCore& ilCore = *pGlobalILCoreCache->ILCore();
		omxError = ilCore.GetContentPipe(
			hPipe,
			szURI);
		}
	else
		{
		ROmxILCoreClientSession coresession;
		TInt err = coresession.Open();
		// OMX_Init needs to be the first OMX call
		__ASSERT_ALWAYS(KErrNotFound != err,
						User::Panic(KOmxILCoreClientPanic,
									KErrNotReady));

		if (KErrNone == err)
			{
			omxError = coresession.GetContentPipe(
				hPipe,
				szURI);
			}
		else
			{
			omxError = OMX_ErrorUndefined;
			}

		coresession.Close();
		}

	pGlobalILCoreCache->Unlock();
	return omxError;

	}

/**
   The OMX_GetComponentsOfRole function that enables the IL client to query the
   names of all installed components that support a given role.

   @return OMX_ERRORTYPE
 */
OMX_API OMX_ERRORTYPE OMX_GetComponentsOfRole (
	OMX_IN      OMX_STRING role,
	OMX_INOUT   OMX_U32* pNumComps,
	OMX_INOUT   OMX_U8** compNames)
	{
	XGlobalILCoreCache* pGlobalILCoreCache = XGlobalILCoreCache::IlCoreCache();
	__ASSERT_ALWAYS(pGlobalILCoreCache != NULL,
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));
	pGlobalILCoreCache->Lock();

	// OpenMAX IL mandates that OMX_Init must be the first OMX call made into
	// the IL Core...
	__ASSERT_ALWAYS(pGlobalILCoreCache->ILCore() != NULL,
					User::Panic(KOmxILCoreClientPanic,
								KErrNotReady));

	ROmxILCoreClientSession coresession;
	OMX_ERRORTYPE omxError = OMX_ErrorNone;
	TInt err = coresession.Open();
	// OMX_Init needs to be the first OMX call
	__ASSERT_ALWAYS(KErrNotFound != err,
					User::Panic(KOmxILCoreClientPanic,
								KErrNotReady));

	// The OMX IL spec does not mandate this, but it probably should. We list
	// and initialize loaders here too, if we can.
	if (KErrNone == err)
		{
		// Ignore if any error...
		coresession.ListLoaders();
		}

	if (CActiveScheduler::Current())
		{
		COmxILCore& ilCore = *pGlobalILCoreCache->ILCore();
		omxError = ilCore.GetComponentsOfRole(
			role,
			pNumComps,
			compNames);
		}
	else
		{

		if (KErrNone == err)
			{
			omxError = coresession.GetComponentsOfRole(
				role,
				pNumComps,
				compNames);
			}
		else
			{
			omxError = OMX_ErrorUndefined;
			}

		}

	coresession.Close();
	pGlobalILCoreCache->Unlock();
	return omxError;

	}

/**
   The function that enables the IL client to query all the roles fulfilled by
   a given a component.

   @return OMX_ERRORTYPE
 */
OMX_API OMX_ERRORTYPE OMX_GetRolesOfComponent (
	OMX_IN      OMX_STRING compName,
	OMX_INOUT   OMX_U32* pNumRoles,
	OMX_OUT     OMX_U8** roles)
	{
    XGlobalILCoreCache* pGlobalILCoreCache = XGlobalILCoreCache::IlCoreCache();
	__ASSERT_ALWAYS(pGlobalILCoreCache != NULL,
					User::Panic(KOmxILCoreClientPanic, KErrNotReady));
	pGlobalILCoreCache->Lock();

	// OpenMAX IL mandates that OMX_Init must be the first OMX call made into
	// the IL Core...
	__ASSERT_ALWAYS(pGlobalILCoreCache->ILCore() != NULL,
					User::Panic(KOmxILCoreClientPanic,
								KErrNotReady));

	ROmxILCoreClientSession coresession;
	TInt err = coresession.Open();
	// OMX_Init needs to be the first OMX call
	__ASSERT_ALWAYS(KErrNotFound != err,
					User::Panic(KOmxILCoreClientPanic,
								KErrNotReady));

	// The OMX IL spec does not mandate this, but it probably should. We list
	// and initialize loaders here too, if we can.
	if (KErrNone == err)
		{
		// Ignore if any error...
		coresession.ListLoaders();
		}

	OMX_ERRORTYPE omxError = OMX_ErrorNone;
	if (CActiveScheduler::Current())
		{
		COmxILCore& ilCore = *pGlobalILCoreCache->ILCore();
		omxError = ilCore.GetRolesOfComponent(
			compName,
			pNumRoles,
			roles);
		}
	else
		{

		if (KErrNone == err)
			{
			omxError = coresession.GetRolesOfComponent(
				compName,
				pNumRoles,
				roles);
			}
		else
			{
			omxError = OMX_ErrorUndefined;
			}

		}

	coresession.Close();
	pGlobalILCoreCache->Unlock();
	return omxError;

	}
