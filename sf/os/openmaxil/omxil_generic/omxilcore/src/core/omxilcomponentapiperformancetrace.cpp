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
* Description: This class is provides performance trace wrappers around OpenMax IL APIs.
* This class shall not be instantiated.
*
*/

#include "omxilcomponentapiperformancetrace.h"
#include <e32base.h>

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
#include "..\..\traces\OstTraceDefinitions.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "omxilComponentapiperformancetraceTraces.h"
#endif

#endif




OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_GetConfig(
        OMX_IN  OMX_HANDLETYPE ahComponent,
        OMX_IN  OMX_INDEXTYPE anIndex,
        OMX_INOUT OMX_PTR apComponentConfigStructure)
    {
    OMX_ERRORTYPE error = OMX_ErrorBadParameter;
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _GetConfig1, "GetConfig >" );
#endif
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
              Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_GetConfig) );
#endif
        
    if(ahComponent)
        {
        OMX_COMPONENTTYPE* pComp = ResolveCallsToComponent(ahComponent);
        error = pComp->GetConfig(pComp, anIndex, apComponentConfigStructure);
        }
        
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    TBuf8<ADDITIONAL_INFO_SIZE> GetConfigInfo;
     // uses the shares same additional information format as the GetParameter measurement End format.
     GetConfigInfo.Format(Ost_OMXIL_Performance::K_GetParameterInfo , ahComponent, anIndex);
     OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
             Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_GetConfig), error ,&GetConfigInfo);
#endif     
     
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
     OstTraceExt4(TRACE_API, _GetConfig2, "GetConfig < ReturnVal=%{OMX_ERRORTYPE} Component=%p Index=%{OMX_INDEXTYPE} pComponentConfigStructure=%p",
             (TUint)error, ahComponent, (TUint)anIndex, apComponentConfigStructure);
	if (apComponentConfigStructure)
		{
		OMX_U32* const pStructSize =
			reinterpret_cast<OMX_U32*>(apComponentConfigStructure);
		OstTraceData(TRACE_API_DUMP_STRUCT, _GetConfig3, "Config Struct: %{hex8[]}", apComponentConfigStructure, (TUint)(*pStructSize)) ;
		}
#endif
     
    return error;
    }


OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_SetConfig(
        OMX_IN  OMX_HANDLETYPE ahComponent,
        OMX_IN  OMX_INDEXTYPE anIndex,
        OMX_IN  OMX_PTR apComponentConfigStructure)
    {
    OMX_ERRORTYPE error = OMX_ErrorBadParameter;

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _SetConfig1, "SetConfig >" );
#endif
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
            Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_SetConfig) );
#endif 
    if(ahComponent)
        {
        OMX_COMPONENTTYPE* pComp = ResolveCallsToComponent(ahComponent);
        error = pComp->SetConfig(pComp, anIndex, apComponentConfigStructure);
        }
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL    
    TBuf8<ADDITIONAL_INFO_SIZE> SetConfigInfo;
        // uses the shares same additional information format as the GetParameter measurement End format.
        SetConfigInfo.Format(Ost_OMXIL_Performance::K_GetParameterInfo , ahComponent, anIndex);
        OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
                Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_SetConfig), error ,&SetConfigInfo);
#endif 

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
     OstTraceExt4(TRACE_API, _SetConfig2, "SetConfig < ReturnVal=%{OMX_ERRORTYPE} Component=%p Index=%{OMX_INDEXTYPE} pComponentConfigStructure=%p",
             (TUint)error, ahComponent, (TUint)anIndex, apComponentConfigStructure) ;
	if (apComponentConfigStructure)
		{
		OMX_U32* const pStructSize =
			reinterpret_cast<OMX_U32*>(apComponentConfigStructure);
     
		OstTraceData(TRACE_API_DUMP_STRUCT, _SetConfig3, "Config Struct: %{hex8[]}", apComponentConfigStructure, (TUint)(*pStructSize)) ;
		}
     
#endif
        
    return error;
}


OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_GetState(
        OMX_IN  OMX_HANDLETYPE ahComponent,
        OMX_OUT OMX_STATETYPE* apState)
    {
    OMX_ERRORTYPE error = OMX_ErrorBadParameter;
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _GetState1, "GetState >" );
#endif
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
            Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_GetState));
#endif
    if(ahComponent)
       {
        OMX_COMPONENTTYPE* pComp = ResolveCallsToComponent(ahComponent);
        error = pComp->GetState(pComp, apState);
       }

#ifdef SYMBIAN_PERF_TRACE_OMX_IL    
    TBuf8<ADDITIONAL_INFO_SIZE> SetStateInfo;
    // uses the shares same additional information format as the GetParameter measurement End format.
    SetStateInfo.Format(Ost_OMXIL_Performance::K_GetParameterInfo , ahComponent, apState);
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
            Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_GetState), error ,&SetStateInfo);
#endif
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTraceExt3( TRACE_API, _GetState2, "GetState < ReturnVal=%{OMX_ERRORTYPE} Component=%p pState=%{OMX_STATETYPE}", 
            (TUint)error, ahComponent, (TUint)(apState)? *apState:0 );
#endif
    
    return error;
    }


OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_GetParameter(
        OMX_IN  OMX_HANDLETYPE ahComponent,
        OMX_IN  OMX_INDEXTYPE anParamIndex,
        OMX_INOUT OMX_PTR aComponentParameterStructure)
{
    OMX_ERRORTYPE error = OMX_ErrorBadParameter;
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _GetParameter1, "GetParameter >" );
#endif
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
            Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_GetParam));
#endif  
    if(ahComponent)
        { 
        OMX_COMPONENTTYPE* pComp = ResolveCallsToComponent(ahComponent);
        error = pComp->GetParameter(pComp, anParamIndex, aComponentParameterStructure);
        }

#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    // uses the shares same additional information format as the GetParameter measurement End format.
    TBuf8<ADDITIONAL_INFO_SIZE> GetParamerInfo;
    GetParamerInfo.Format(Ost_OMXIL_Performance::K_GetParameterInfo , ahComponent, anParamIndex);
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
                Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_GetParam), error ,&GetParamerInfo);
#endif   
 
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    if (anParamIndex == OMX_IndexParamPortDefinition)
        {
        OMX_PARAM_PORTDEFINITIONTYPE* portPtr =  static_cast<OMX_PARAM_PORTDEFINITIONTYPE*>(aComponentParameterStructure);
        
		OstTraceExt5( TRACE_API, _GetParameter2, "GetParameter(OMX_IndexParamPortDefinition) < ReturnVal=%{OMX_ERRORTYPE} Component=%p nPortIndex=%u nBufferCountActual=%u nBufferSize=%u",
                   (TUint)error, ahComponent, (TUint)(portPtr->nPortIndex), (TUint)(portPtr->nBufferCountActual), (TUint)(portPtr->nBufferSize));
        }
    else if (anParamIndex == OMX_IndexParamCompBufferSupplier)
        {
        OMX_PARAM_BUFFERSUPPLIERTYPE* bufPtr = static_cast<OMX_PARAM_BUFFERSUPPLIERTYPE*>(aComponentParameterStructure);
        OstTraceExt4( TRACE_API, _GetParameter3, "GetParameter(OMX_IndexParamCompBufferSupplier) < ReturnVal=%{OMX_ERRORTYPE} Component=%p nPortIndex=%u eBufferSupplier=%{OMX_BUFFERSUPPLIERTYPE}", 
                   (TUint)error, ahComponent, (TUint)(bufPtr->nPortIndex), (TUint)(bufPtr->eBufferSupplier));         
        }
    else if (anParamIndex > OMX_IndexKhronosExtensions)
        {
        OstTraceExt4( TRACE_API, _GetParameter4, "GetParameter(Vendor specific index)< ReturnVal=%{OMX_ERRORTYPE} Component=%p nParamIndex=%u aComponentParameterStructure=%p", 
                (TUint)error, ahComponent, (TUint)(anParamIndex), aComponentParameterStructure );        
        }
    else
        {
        OstTraceExt4( TRACE_API, _GetParameter5, "GetParameter < ReturnVal=%{OMX_ERRORTYPE} Component=%p nParamIndex=%{OMX_INDEXTYPE} aComponentParameterStructure=%p", 
                (TUint)error, ahComponent, (TUint)(anParamIndex), aComponentParameterStructure );
        }
	if (aComponentParameterStructure)
		{
		OMX_U32* const pStructSize =
			reinterpret_cast<OMX_U32*>(aComponentParameterStructure);
		OstTraceData(TRACE_API_DUMP_STRUCT, _GetParameter6, "Param Struct: %{hex8[]}", aComponentParameterStructure, (TUint)(*pStructSize)) ;
		}
#endif
    
    return error;
}


OMX_ERRORTYPE OMXILAPITraceWrapper::PrepareTraceStructuresL( OMX_INOUT OMX_HANDLETYPE& aClientHandle, 
                                                             OMX_INOUT OMX_CALLBACKTYPE*& aTraceCallBack,
                                                             COMPONENT_REF*& aCompRefToClient, 
                                                             COMPONENT_REF*& aCompRefToTrace)
    {
    
    // create a new OMX_COMPONENTTYPE that will be handed to the client
    aClientHandle = reinterpret_cast<OMX_HANDLETYPE>( new (ELeave) OMX_COMPONENTTYPE );
    
    CleanupStack::PushL(aClientHandle);
    aTraceCallBack      = new (ELeave)OMX_CALLBACKTYPE;
    CleanupStack::PushL(aTraceCallBack);
    //  create the structure to glue the clients Handle and the callback
    aCompRefToClient = new (ELeave) COMPONENT_REF;
    CleanupStack::PushL(aCompRefToClient);
    aCompRefToTrace = new (ELeave) COMPONENT_REF;
    
    
    CleanupStack::Pop(3, aClientHandle);

    //populate the callback structure with the Trace functions
    aTraceCallBack->EventHandler        = Trace_EventHandler;
    aTraceCallBack->EmptyBufferDone     = Trace_EmptyBufferDone;
    aTraceCallBack->FillBufferDone      = Trace_FillBufferDone;

    return OMX_ErrorNone;
    }


OMX_ERRORTYPE OMXILAPITraceWrapper::UndoPrepareTraceStructures( OMX_INOUT OMX_HANDLETYPE& aClientHandle, 
                                                             OMX_INOUT OMX_CALLBACKTYPE*& aTraceCallBack,
                                                             COMPONENT_REF*& aCompRefToClient, 
                                                             COMPONENT_REF*& aCompRefToTrace)
    {
    // 
    
    delete aTraceCallBack;
    aTraceCallBack= NULL;
    delete aClientHandle;
    aClientHandle= NULL;
    delete aCompRefToClient;
    aCompRefToClient=NULL;
    delete aCompRefToTrace ;
    aCompRefToTrace=NULL;
    return OMX_ErrorNone;
    }

TInt OMXILAPITraceWrapper::PatchStructures( OMX_IN OMX_HANDLETYPE ahClientComp,
                                         OMX_IN OMX_CALLBACKTYPE* apClientCallBack,
                                         OMX_IN OMX_HANDLETYPE ahTraceComp,
                                         OMX_INOUT OMX_CALLBACKTYPE* apTraceCallBack,
                                         COMPONENT_REF*& aCompRefToClient, 
                                         COMPONENT_REF*& aCompRefToTrace)
    {

    OMX_COMPONENTTYPE *pClientComp = reinterpret_cast< OMX_COMPONENTTYPE *>(ahClientComp);
    OMX_COMPONENTTYPE *pTraceComp = reinterpret_cast< OMX_COMPONENTTYPE *>(ahTraceComp);

    aCompRefToClient->pComponentIF = pClientComp;
    aCompRefToClient->pCallbacks = apClientCallBack;
    
    aCompRefToTrace->pComponentIF = pTraceComp;
    aCompRefToTrace->pCallbacks = apTraceCallBack;

    // the client component reference is stored in the Trace Handle's pApplicationPrivate
    pTraceComp->pApplicationPrivate = reinterpret_cast<OMX_PTR>(aCompRefToClient);

    pClientComp->nSize = sizeof(OMX_COMPONENTTYPE);
    pClientComp->nVersion = pTraceComp->nVersion;
    // the trace component reference is stored in the client's Handle pComponentPrivate
    pClientComp->pComponentPrivate = reinterpret_cast<OMX_PTR>(aCompRefToTrace);


    /* Set function pointers to wrapper functions */
    pClientComp->AllocateBuffer           = OMXILAPITraceWrapper::Trace_AllocateBuffer;
    pClientComp->ComponentTunnelRequest   = OMXILAPITraceWrapper::Trace_ComponentTunnelRequest;
    pClientComp->EmptyThisBuffer          = OMXILAPITraceWrapper::Trace_EmptyThisBuffer;
    pClientComp->FillThisBuffer           = OMXILAPITraceWrapper::Trace_FillThisBuffer;
    pClientComp->FreeBuffer               = OMXILAPITraceWrapper::Trace_FreeBuffer;
    pClientComp->GetComponentVersion      = OMXILAPITraceWrapper::Trace_GetComponentVersionL;
    pClientComp->GetExtensionIndex        = OMXILAPITraceWrapper::Trace_GetExtensionIndexL;
    pClientComp->ComponentDeInit          = OMXILAPITraceWrapper::Trace_ComponentDeInit;
    pClientComp->SetCallbacks             = OMXILAPITraceWrapper::Trace_SetCallbacks;
    pClientComp->UseBuffer                = OMXILAPITraceWrapper::Trace_UseBuffer;

    pClientComp->GetConfig                = OMXILAPITraceWrapper::Trace_GetConfig;
    pClientComp->GetParameter             = OMXILAPITraceWrapper::Trace_GetParameter;
    pClientComp->GetState                 = OMXILAPITraceWrapper::Trace_GetState;
    pClientComp->SendCommand              = OMXILAPITraceWrapper::Trace_SendCommand;
    pClientComp->SetConfig                = OMXILAPITraceWrapper::Trace_SetConfig;
    pClientComp->SetParameter             = OMXILAPITraceWrapper::Trace_SetParameter;
    
    pClientComp->UseEGLImage                = OMXILAPITraceWrapper::Trace_UseEGLImage;
    pClientComp->ComponentRoleEnum          = OMXILAPITraceWrapper::Trace_ComponentRoleEnum;

    return KErrNone;
    }

TInt OMXILAPITraceWrapper::ComponentTracerDestroy(OMX_IN OMX_HANDLETYPE ahClientComp)
    {
    // resolve the wrapper callbacks used by the component and delete it
    COMPONENT_REF* pTraceCompRef = reinterpret_cast<COMPONENT_REF*> ((reinterpret_cast<OMX_COMPONENTTYPE*>(ahClientComp))->pComponentPrivate );
    delete pTraceCompRef->pCallbacks;

    //resolve and delete the OMX_COMPONENTTYPE structure passed to the client
    COMPONENT_REF* pClientCompRef = reinterpret_cast<COMPONENT_REF*> ((pTraceCompRef->pComponentIF)->pApplicationPrivate);
    delete ahClientComp;

    // delete the glues
    delete pClientCompRef;
    pClientCompRef=NULL;
    delete pTraceCompRef;
    pTraceCompRef=NULL;

    return KErrNone;
    }


OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_SetParameter(
        OMX_IN  OMX_HANDLETYPE ahComponent,
        OMX_IN  OMX_INDEXTYPE anIndex,
        OMX_IN  OMX_PTR aComponentParameterStructure)
    {
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _SetParameter1, "SetParameter >" );
#endif
    
    OMX_ERRORTYPE omxError = OMX_ErrorBadParameter;
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
                Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_SetParam) );
#endif
     if(ahComponent)
        {
        OMX_COMPONENTTYPE *pComp = ResolveCallsToComponent(ahComponent);
        omxError = pComp->SetParameter(pComp, anIndex, aComponentParameterStructure);
        }
     
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    TBuf8<ADDITIONAL_INFO_SIZE> setParamerInfo;
    setParamerInfo.Format(Ost_OMXIL_Performance::K_GetParameterInfo , ahComponent, anIndex);
     OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
                Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_SetParam), omxError ,&setParamerInfo);
     
#endif

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
     if (anIndex == OMX_IndexParamPortDefinition)
         {
         OMX_PARAM_PORTDEFINITIONTYPE* portPtr =  static_cast<OMX_PARAM_PORTDEFINITIONTYPE*>(aComponentParameterStructure);
         
		OstTraceExt5( TRACE_API, _SetParameter2, "SetParameter(OMX_IndexParamPortDefinition) < ReturnVal=%{OMX_ERRORTYPE} Component=%p nPortIndex=%u nBufferCountActual=%u nBufferSize=%u",
                    (TUint)omxError, ahComponent, (TUint)(portPtr->nPortIndex), (TUint)(portPtr->nBufferCountActual), (TUint)(portPtr->nBufferSize));
         }
     else if (anIndex == OMX_IndexParamCompBufferSupplier)
         {
         OMX_PARAM_BUFFERSUPPLIERTYPE* bufPtr = static_cast<OMX_PARAM_BUFFERSUPPLIERTYPE*>(aComponentParameterStructure);
         OstTraceExt4( TRACE_API, _SetParameter3, "SetParameter(OMX_IndexParamCompBufferSupplier) < ReturnVal=%{OMX_ERRORTYPE} Component=%p nPortIndex=%u eBufferSupplier=%{OMX_BUFFERSUPPLIERTYPE}", 
                    (TUint)omxError, ahComponent, (TUint)(bufPtr->nPortIndex), (TUint)(bufPtr->eBufferSupplier));         
         }
     else if (anIndex > OMX_IndexKhronosExtensions)
         {
         OstTraceExt4( TRACE_API, _SetParameter4, "SetParameter(Vendor specific index) < ReturnVal=%u Component=%p nParamIndex=%u aComponentParameterStructure=%p", 
            (TUint)omxError, ahComponent, (TUint)(anIndex), aComponentParameterStructure );
         }
     else
         {
         OstTraceExt4( TRACE_API, _SetParameter5, "SetParameter < ReturnVal=%u Component=%p nParamIndex=%{OMX_INDEXTYPE} aComponentParameterStructure=%p", 
            (TUint)omxError, ahComponent, (TUint)(anIndex), aComponentParameterStructure );
         }

	if (aComponentParameterStructure)
		{
		OMX_U32* const pStructSize =
			reinterpret_cast<OMX_U32*>(aComponentParameterStructure);
		OstTraceData(TRACE_API_DUMP_STRUCT, _SetParameter6, "Param Struct: %{hex8[]}", aComponentParameterStructure, (TUint)(*pStructSize)) ;
		}
#endif
    
    return omxError;
    }

OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_SendCommand(
        OMX_IN  OMX_HANDLETYPE ahComponent,
        OMX_IN  OMX_COMMANDTYPE aCmd,
        OMX_IN  OMX_U32 anParam1,
        OMX_IN  OMX_PTR apCmdData)
    {
    OMX_ERRORTYPE omxError = OMX_ErrorBadParameter;
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _SendCommand1, "SendCommand >" );
#endif   

#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
      Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_SendCommand));
#endif
    if(ahComponent)
       {
       OMX_COMPONENTTYPE *pComp = ResolveCallsToComponent(ahComponent);
       omxError = pComp->SendCommand(pComp, aCmd, anParam1, apCmdData);
       }
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    TBuf8<ADDITIONAL_INFO_SIZE> SendCmdInfo;
    SendCmdInfo.Format(Ost_OMXIL_Performance::K_OMXSendCommandInfo, ahComponent, aCmd, anParam1);
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
            Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_SendCommand), omxError ,&SendCmdInfo);
#endif
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
	switch (aCmd)
		{
	case OMX_CommandStateSet:
		{
		OstTraceExt5( TRACE_API, _SendCommand2,
					  "SendCommand < ReturnVal=%{OMX_ERRORTYPE} Component=%p aCmd=%{OMX_COMMANDTYPE} anParam1=%{OMX_STATETYPE} apCmdData=%p",
            (TUint)omxError, ahComponent, (TUint)aCmd, (TUint)anParam1, apCmdData);
		}
		break;
	case OMX_CommandFlush:
	case OMX_CommandPortDisable:
	case OMX_CommandPortEnable:
	case OMX_CommandMarkBuffer:
		{
		if (anParam1 == OMX_ALL)
			{
			OstTraceExt4( TRACE_API, _SendCommand3,
						  "SendCommand < ReturnVal=%{OMX_ERRORTYPE} Component=%p aCmd=%{OMX_COMMANDTYPE} anParam1=OMX_ALL apCmdData=%p",
						  (TUint)omxError, ahComponent, (TUint)aCmd, apCmdData);
			}
		else
			{
			OstTraceExt5( TRACE_API, _SendCommand4,
						  "SendCommand < ReturnVal=%{OMX_ERRORTYPE} Component=%p aCmd=%{OMX_COMMANDTYPE} anParam1=%u apCmdData=%p",
						  (TUint)omxError, ahComponent, (TUint)aCmd, (TUint)anParam1, apCmdData);
			}
		}
		break;
	default:
		{
		OstTraceExt5( TRACE_API, _SendCommand6,
					  "SendCommand < ReturnVal=%{OMX_ERRORTYPE} Component=%p aCmd=%{OMX_COMMANDTYPE} anParam1=%u apCmdData=%p",
					  (TUint)omxError, ahComponent, (TUint)aCmd, (TUint)anParam1, apCmdData);
		}
		};
#endif
    
    return omxError;
    }


OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_AllocateBuffer(
        OMX_IN OMX_HANDLETYPE ahComponent,
        OMX_INOUT OMX_BUFFERHEADERTYPE** apBuffer,
        OMX_IN OMX_U32 anPortIndex,
        OMX_IN OMX_PTR apAppPrivate,
        OMX_IN OMX_U32 anSizeBytes)
    {
    OMX_ERRORTYPE omxError = OMX_ErrorBadParameter;
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API_BUFFER, _AllocateBuffer1, "AllocateBuffer >" );
#endif  
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
    Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_AllocateBuffer));
#endif
    
    if(ahComponent)
        {
        OMX_COMPONENTTYPE *pComp = ResolveCallsToComponent(ahComponent);
        omxError = pComp->AllocateBuffer(pComp, apBuffer, anPortIndex, apAppPrivate, anSizeBytes);
        }
 
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    TBuf8<ADDITIONAL_INFO_SIZE> Info;
    Info.Format(Ost_OMXIL_Performance::K_HandleOnlyInfo, ahComponent );
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
          Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_AllocateBuffer), omxError ,&Info);
#endif

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTraceExt5( TRACE_API_BUFFER, _AllocateBuffer2, "AllocateBuffer < ReturnVal=%{OMX_ERRORTYPE} Component=%p nPortIndex=%u  pBuffer=%p nSizeBytes=%u ", 
            (TUint)omxError, ahComponent, (TUint)anPortIndex, (apBuffer)? *apBuffer:0, (TUint)anSizeBytes );
#endif
    return omxError;
    }

OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_EmptyThisBuffer(
        OMX_IN  OMX_HANDLETYPE ahComponent,
        OMX_IN  OMX_BUFFERHEADERTYPE* apBuffer)
    {
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API_BUFFER, _EmptyThisBuffer1, "EmptyThisBuffer >" );
#endif  
    OMX_ERRORTYPE omxError =OMX_ErrorBadParameter;
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
    Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_EmptyThisBuffer));
#endif
    if(ahComponent)
        {
        OMX_COMPONENTTYPE *pComp = ResolveCallsToComponent(ahComponent);
        omxError = pComp->EmptyThisBuffer(pComp, apBuffer);
        }
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    TBuf8<ADDITIONAL_INFO_SIZE> Info;
    Info.Format(Ost_OMXIL_Performance::K_HandleOnlyInfo, ahComponent );
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
              Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_EmptyThisBuffer), omxError ,&Info);
#endif
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
	if (apBuffer)
		{
		OstTraceExt5( TRACE_API_BUFFER, _EmptyThisBuffer2,
					  "EmptyThisBuffer < ReturnVal=%{OMX_ERRORTYPE} Component=%p pBufferHeader=%p nPortIndex=%u nFilledLen=%u",
					  (TUint)omxError, ahComponent, apBuffer, (TUint)apBuffer->nOutputPortIndex, (TUint)apBuffer->nFilledLen  );
		OstTraceData(TRACE_API_DUMP_STRUCT, _EmptyThisBuffer3,
					 "Buffer Header Struct: %{hex8[]}",
					 apBuffer, (TUint)(apBuffer->nSize)) ;
		}
#endif   
    
     return omxError;
     }

OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_ComponentTunnelRequest(
    OMX_IN  OMX_HANDLETYPE ahComp,
    OMX_IN  OMX_U32 anPort,
    OMX_IN  OMX_HANDLETYPE ahTunneledComp,
    OMX_IN  OMX_U32 anTunneledPort,
    OMX_INOUT  OMX_TUNNELSETUPTYPE* apTunnelSetup)
    {
    OMX_ERRORTYPE omxError =OMX_ErrorBadParameter;

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _ComponentTunnelRequest1, "ComponentTunnelRequest >" );
#endif 
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
    Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_ComponentTunnelRequest));
#endif
    if(ahComp)
        {    
        OMX_COMPONENTTYPE *pComp = ResolveCallsToComponent(ahComp);
        omxError = pComp->ComponentTunnelRequest(pComp, anPort, ahTunneledComp, anTunneledPort, apTunnelSetup);
        }
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    TBuf8<ADDITIONAL_INFO_SIZE> Info;
    Info.Format(Ost_OMXIL_Performance::K_HandleOnlyInfo, ahComp );
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
         Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_ComponentTunnelRequest), omxError ,&Info);
#endif
   
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTraceExt5( TRACE_API, _ComponentTunnelRequest2, "ComponentTunnelRequest < ReturnVal=%{OMX_ERRORTYPE} Component=%p nPort=%u hTunneledComp=%u nTunneledPort=%u", 
            (TUint)omxError, ahComp, (TUint)anPort, ahTunneledComp, (TUint)anTunneledPort );
#endif
    
    return omxError;
    }

OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_FillThisBuffer(
        OMX_IN  OMX_HANDLETYPE ahComponent,
        OMX_IN  OMX_BUFFERHEADERTYPE* apBuffer)
    {

    OMX_ERRORTYPE omxError =OMX_ErrorBadParameter;
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API_BUFFER, _FillThisBuffer1, "FillThisBuffer >" );
#endif 
     
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
    Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_FillThisBuffer));
#endif
     
    if(ahComponent)
        {
        OMX_COMPONENTTYPE *pComp = ResolveCallsToComponent(ahComponent);
        omxError = pComp->FillThisBuffer(pComp, apBuffer);
        }

#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    TBuf8<ADDITIONAL_INFO_SIZE> Info;
    Info.Format(Ost_OMXIL_Performance::K_HandleOnlyInfo, ahComponent );
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
         Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_FillThisBuffer), omxError ,&Info);
#endif 

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
	if (apBuffer)
		{
		OstTraceExt5( TRACE_API_BUFFER, _FillThisBuffer2,
					  "FillThisBuffer < ReturnVal=%{OMX_ERRORTYPE} Component=%p pBufferHeader=%p nPortIndex=%u nFilledLen=%u",
					  (TUint)omxError, ahComponent, apBuffer, (TUint)apBuffer->nOutputPortIndex, (TUint)apBuffer->nFilledLen );
		OstTraceData(TRACE_API_DUMP_STRUCT, _FillThisBuffer3,
					 "Buffer Header Struct: %{hex8[]}",
					 apBuffer, (TUint)(apBuffer->nSize)) ;
		}
#endif     
    return omxError;
    }


OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_FreeBuffer(
        OMX_IN  OMX_HANDLETYPE ahComponent,
        OMX_IN OMX_U32 anPortIndex,
        OMX_IN  OMX_BUFFERHEADERTYPE* apBuffer)
    {
    OMX_ERRORTYPE omxError = OMX_ErrorBadParameter;
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API_BUFFER, _FreeBuffer1, "FreeBuffer >" );
#endif 
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
            Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_FreeBuffer));
#endif
       
    if(ahComponent)
        {
        OMX_COMPONENTTYPE *pComp = ResolveCallsToComponent(ahComponent);
        omxError = pComp->FreeBuffer(pComp, anPortIndex, apBuffer);
        }
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    TBuf8<ADDITIONAL_INFO_SIZE> Info;
    Info.Format(Ost_OMXIL_Performance::K_HandleOnlyInfo, ahComponent );
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
        Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_FreeBuffer), omxError ,&Info);
#endif    
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
	if (apBuffer)
		{
		OstTraceExt4( TRACE_API_BUFFER, _FreeBuffer2, "FreeBuffer < ReturnVal=%{OMX_ERRORTYPE} Component=%p nPortIndex=%u pBufferHeader=%p",
            (TUint)omxError,ahComponent, (TUint)anPortIndex ,apBuffer );
		OstTraceData(TRACE_API_DUMP_STRUCT, _FreeBuffer3,
					 "Buffer Header Struct: %{hex8[]}",
					 apBuffer, (TUint)(apBuffer->nSize)) ;
		}
#endif 
    
    return omxError;
    }


OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_UseBuffer(
        OMX_IN OMX_HANDLETYPE ahComponent,
        OMX_INOUT OMX_BUFFERHEADERTYPE** appBufferHdr,
        OMX_IN OMX_U32 anPortIndex,
        OMX_IN OMX_PTR apAppPrivate,
        OMX_IN OMX_U32 anSizeBytes,
        OMX_IN OMX_U8* apBuffer)
    {
    OMX_ERRORTYPE omxError = OMX_ErrorBadParameter;

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API_BUFFER, _UseBuffer1, "UseBuffer >" );
#endif     
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
               Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_UseBuffer));
#endif
    if(ahComponent)
        {
        OMX_COMPONENTTYPE *pComp = ResolveCallsToComponent(ahComponent);
        omxError = pComp->UseBuffer(pComp, appBufferHdr, anPortIndex, apAppPrivate, anSizeBytes, apBuffer);
        }
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    TBuf8<ADDITIONAL_INFO_SIZE> Info;
    Info.Format(Ost_OMXIL_Performance::K_HandleOnlyInfo, ahComponent );
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
            Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_UseBuffer), omxError ,&Info);
#endif
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTraceExt5( TRACE_API_BUFFER, _UseBuffer2, "UseBuffer < ReturnVal=%{OMX_ERRORTYPE} Component=%p nPortIndex=%u  pBuffer=%p nSizeBytes=%u ", 
            (TUint)omxError, ahComponent, (TUint)anPortIndex, (apBuffer)? *apBuffer:0, (TUint)anSizeBytes );
#endif    
    
    return omxError;
    }

OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_GetComponentVersionL(
        OMX_IN  OMX_HANDLETYPE ahComponent,
        OMX_OUT OMX_STRING apComponentName,
        OMX_OUT OMX_VERSIONTYPE* apComponentVersion,
        OMX_OUT OMX_VERSIONTYPE* apSpecVersion,
        OMX_OUT OMX_UUIDTYPE* apComponentUUID)
    {
    OMX_ERRORTYPE omxError = OMX_ErrorBadParameter;
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _GetComponentVersion1, "GetComponentVersion >" );
#endif 
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
      OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
              Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_GetComponentVersion));
#endif  
      if(ahComponent)
          {
          OMX_COMPONENTTYPE *pComp = ResolveCallsToComponent(ahComponent);
          omxError = pComp->GetComponentVersion(pComp, apComponentName, apComponentVersion, apSpecVersion, apComponentUUID);
          }
      
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    TBuf8<ADDITIONAL_INFO_SIZE> Info;
    Info.Format(Ost_OMXIL_Performance::K_HandleOnlyInfo, ahComponent );
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
           Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_GetComponentVersion), omxError ,&Info);
#endif   

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    RBuf8 ComponentInfo;
    ComponentInfo.CleanupClosePushL();
    
    TPtrC8 componentNamePtr(const_cast<const TUint8*>(reinterpret_cast<TUint8*>(apComponentName)));
    
    TInt len = componentNamePtr.Length();

    if (len < 0)
        {
        CleanupStack::PopAndDestroy(&ComponentInfo);
        return OMX_ErrorInvalidComponent;
        }
    
    if (ComponentInfo.Create(len))
        {
        CleanupStack::PopAndDestroy(&ComponentInfo);
        return OMX_ErrorInsufficientResources;
        }
    
    ComponentInfo = reinterpret_cast<TUint8 *>(apComponentName);
    OstTraceExt5( TRACE_API, _GetComponentVersion2, "GetComponentVersion < ReturnVal=%u Component=%p pComponentName=%s pComponentVersion=%u, pSpecVersion=%u", 
            (TUint)omxError, ahComponent, ComponentInfo, (TUint)((apComponentVersion)? apComponentVersion->nVersion:0), (TUint)((apSpecVersion)?apSpecVersion->nVersion:0) );
    CleanupStack::PopAndDestroy(&ComponentInfo);
#endif  
    return omxError;
    }

OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_ComponentDeInit(
        OMX_IN  OMX_HANDLETYPE ahComponent)
    {
    OMX_ERRORTYPE omxError = OMX_ErrorBadParameter;
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _ComponentDeInit1, "ComponentDeInit >" );
#endif     
       
#ifdef SYMBIAN_PERF_TRACE_OMX_IL      
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
                   Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_ComponentDeInit));
#endif
    if(ahComponent)
        {
        OMX_COMPONENTTYPE *pComp = ResolveCallsToComponent(ahComponent);
        omxError = pComp->ComponentDeInit(pComp);
        }

#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    TBuf8<ADDITIONAL_INFO_SIZE> Info;
    Info.Format(Ost_OMXIL_Performance::K_HandleOnlyInfo, ahComponent );
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
                Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_ComponentDeInit), omxError ,&Info);
#endif

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTraceExt1( TRACE_API, _ComponentDeInit2, "ComponentDeInit < hComponent=%p", ahComponent );
#endif
    return omxError;
    }


OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_GetExtensionIndexL(
        OMX_IN  OMX_HANDLETYPE ahComponent,
        OMX_IN  OMX_STRING acParameterName,
        OMX_OUT OMX_INDEXTYPE* apIndexType)
    {
    OMX_ERRORTYPE omxError = OMX_ErrorBadParameter;
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _GetExtensionIndex1, "GetExtensionIndex >" );
#endif     

#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
               Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_GetExtensionIndex));
#endif
    
    if(ahComponent)
        {
        OMX_COMPONENTTYPE *pComp = ResolveCallsToComponent(ahComponent);
        omxError = pComp-> GetExtensionIndex(pComp, acParameterName, apIndexType);
        }
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    TBuf8<ADDITIONAL_INFO_SIZE> Info;
    Info.Format(Ost_OMXIL_Performance::K_HandleOnlyInfo, ahComponent );
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
              Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_GetExtensionIndex), omxError ,&Info);
#endif

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    RBuf8 ParamInfo;

    ParamInfo.CleanupClosePushL();

    TPtrC8 parameterNamePtr(const_cast<const TUint8*>(reinterpret_cast<TUint8*>(acParameterName)));
    
    TInt len = parameterNamePtr.Length();

    if (len < 0)
        {
        CleanupStack::PopAndDestroy(&ParamInfo);
        return OMX_ErrorInvalidComponent;
        }
    
    if (ParamInfo.Create(len))
        {
        CleanupStack::PopAndDestroy(&ParamInfo);
        return OMX_ErrorInsufficientResources;
        }
    
    ParamInfo = reinterpret_cast<TUint8 *>(acParameterName);
    
    OstTraceExt4( TRACE_API, _GetExtensionIndex2, "GetComponentVersion < ReturnVal=%x Component=%p pComponentName=%s pIndexType=%{OMX_INDEXTYPE}", 
            (TUint)omxError, ahComponent, ParamInfo, (TUint)((apIndexType)? apIndexType:0) );
    
    CleanupStack::PopAndDestroy(&ParamInfo); 
#endif
    
    return omxError;
    }

OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_SetCallbacks(
        OMX_IN  OMX_HANDLETYPE ahComponent,
        OMX_IN  OMX_CALLBACKTYPE* apCallbacks,
        OMX_IN  OMX_PTR apAppData)
    {
    OMX_ERRORTYPE omxError = OMX_ErrorBadParameter;

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _SetCallbacks1, "SetCallbacks >" );
#endif 
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
               Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_SetCallbacks));
#endif
    
    if(ahComponent)
        {
        // need to access the reference to the internal trace inteface
        COMPONENT_REF* pTraceCompRef = reinterpret_cast<COMPONENT_REF*>(reinterpret_cast<OMX_COMPONENTTYPE*>(ahComponent)->pComponentPrivate ) ;
    
        // now access the client's interface and repopulate its callback
        COMPONENT_REF* pClientCompRef = reinterpret_cast<COMPONENT_REF*>(pTraceCompRef->pComponentIF->pApplicationPrivate);
    
        pClientCompRef->pCallbacks->EmptyBufferDone =  apCallbacks->EmptyBufferDone;
        pClientCompRef->pCallbacks->EventHandler =  apCallbacks->EventHandler;
        pClientCompRef->pCallbacks->FillBufferDone =  apCallbacks->FillBufferDone;
    
        // send pAppData to the component. The callback sent to the component does not change as this is internally
        // maintained , the client has no visibility of this trace wrapper interface callback
        omxError = pTraceCompRef->pComponentIF->SetCallbacks(pTraceCompRef->pComponentIF, pTraceCompRef->pCallbacks, apAppData);
        }
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    TBuf8<ADDITIONAL_INFO_SIZE> Info;
    Info.Format(Ost_OMXIL_Performance::K_HandleOnlyInfo, ahComponent );
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
         Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_SetCallbacks), 0 ,&Info);
#endif
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTraceExt4( TRACE_API, _SetCallbacks2, "SetCallbacks < ReturnVal=%{OMX_ERRORTYPE} Component=%p pCallbacks=%p pAppData=%p", 
            (TUint)omxError,ahComponent, apCallbacks ,apAppData );
#endif 
    
    return omxError;
    }


OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_EventHandler(
    OMX_IN OMX_HANDLETYPE ahComponent,
    OMX_IN OMX_PTR apAppData,
    OMX_IN OMX_EVENTTYPE aeEvent,
    OMX_IN OMX_U32 anData1,
    OMX_IN OMX_U32 anData2,
    OMX_IN OMX_PTR apEventData)
    {
    OMX_ERRORTYPE omxError = OMX_ErrorBadParameter;
    COMPONENT_REF* pCompRef = NULL;
    OMX_COMPONENTTYPE* hComponent = NULL;
    
    if(ahComponent)
        {
        pCompRef = ResolveCallsFromComponent(ahComponent);
        
        if (pCompRef)
            {
            hComponent = pCompRef->pComponentIF;
        	omxError = OMX_ErrorNone;
            
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
        TBuf8<ADDITIONAL_INFO_SIZE> CmdNotificationInfo;
        CmdNotificationInfo.Format(Ost_OMXIL_Performance::K_OMXEventNotificationInfo, hComponent, aeEvent, anData1, anData2 );
        OstPrintf(TTraceContext(KTracePerformanceTraceUID,ESystemCharacteristicMetrics), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
            Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_Event),omxError ,&CmdNotificationInfo);
#endif    

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
			switch(aeEvent)
				{
			case OMX_EventCmdComplete:
				{
				if (anData1 == OMX_CommandStateSet)
					{
					OstTraceExt5( TRACE_API, _EventHandler2,
								  "EventHandler < ReturnVal=%{OMX_ERRORTYPE} Component=%p eEvent=%{OMX_EVENTTYPE} anData1=%{OMX_COMMANDTYPE} nData2=%{OMX_STATETYPE}",
								  (TUint)omxError,hComponent,(TUint)aeEvent,(TUint)anData1, (TUint)anData2 );
					}
				else if (anData1 == OMX_CommandPortDisable || anData1 == OMX_CommandPortEnable || anData1 == OMX_CommandFlush)
					{
					if (anData2 == OMX_ALL)
						{
						OstTraceExt4( TRACE_API, _EventHandler3,
									  "EventHandler < ReturnVal=%{OMX_ERRORTYPE} Component=%p eEvent=%{OMX_EVENTTYPE} anData1=%{OMX_COMMANDTYPE} nData2=OMX_ALL",
									  (TUint)omxError,hComponent,(TUint)aeEvent,(TUint)anData1 );
						}
					else
						{
						OstTraceExt5( TRACE_API, _EventHandler4,
									  "EventHandler < ReturnVal=%{OMX_ERRORTYPE} Component=%p eEvent=%{OMX_EVENTTYPE} anData1=%{OMX_COMMANDTYPE} nData2=%u",
									  (TUint)omxError,hComponent,(TUint)aeEvent,(TUint)anData1, (TUint)anData2 );
						}
					}
				else
					{
					OstTraceExt5( TRACE_API, _EventHandler5,
								  "EventHandler < ReturnVal=%{OMX_ERRORTYPE} Component=%p eEvent=%{OMX_EVENTTYPE} anData1=%{OMX_COMMANDTYPE} nData2=%u",
								  (TUint)omxError,hComponent,(TUint)aeEvent,(TUint)anData1, (TUint)anData2 );
					}
				}
				break;
			case OMX_EventError:
				{
				OstTraceExt5( TRACE_API, _EventHandler6,
							  "EventHandler < ReturnVal=%{OMX_ERRORTYPE} Component=%p eEvent=%{OMX_EVENTTYPE} anData1=%{OMX_ERRORTYPE} nData2=%u",
							  (TUint)omxError,hComponent,(TUint)aeEvent,(TUint)anData1, (TUint)anData2 );
				}
				break;
			case OMX_EventMark:
				{
				OstTraceExt4( TRACE_API, _EventHandler7,
							  "EventHandler < ReturnVal=%{OMX_ERRORTYPE} Component=%p eEvent=%{OMX_EVENTTYPE} pEventData=%p",
							  (TUint)omxError,hComponent,(TUint)aeEvent, apEventData );
				}
				break;
			case OMX_EventPortSettingsChanged:
				{
				OstTraceExt5( TRACE_API, _EventHandler8,
							  "EventHandler < ReturnVal=%{OMX_ERRORTYPE} Component=%p eEvent=%{OMX_EVENTTYPE} anData1=%u nData2=%u",
            (TUint)omxError,hComponent,(TUint)aeEvent,(TUint)anData1, (TUint)anData2 );
				}
				break;
			case OMX_EventBufferFlag:
				{
				OstTraceExt5( TRACE_API, _EventHandler9,
							  "EventHandler < ReturnVal=%{OMX_ERRORTYPE} Component=%p eEvent=%{OMX_EVENTTYPE} anData1=%u nData2=%x",
							  (TUint)omxError,hComponent,(TUint)aeEvent,(TUint)anData1, (TUint)anData2 );
				}
			break;
			default:
				{
				OstTraceExt5( TRACE_API, _EventHandler10,
							  "EventHandler < ReturnVal=%{OMX_ERRORTYPE} Component=%p eEvent=%{OMX_EVENTTYPE} anData1=%u nData2=%u",
							  (TUint)omxError,hComponent,(TUint)aeEvent,(TUint)anData1, (TUint)anData2 );
				}
				};
#endif 
            }
        }
            
     if(ahComponent)
        {
        omxError = pCompRef->pCallbacks->EventHandler(hComponent, apAppData, aeEvent, anData1, anData2, apEventData);
        }
    return omxError;
    }

OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_EmptyBufferDone(
    OMX_OUT OMX_HANDLETYPE ahComponent,
    OMX_OUT OMX_PTR apAppData,
    OMX_OUT OMX_BUFFERHEADERTYPE* apBuffer)
    {
    OMX_ERRORTYPE omxError = OMX_ErrorBadParameter;
    COMPONENT_REF* pCompRef = NULL;
    OMX_COMPONENTTYPE* hComponent = NULL;
    
    if(ahComponent)
        {
        pCompRef = ResolveCallsFromComponent(ahComponent);
        
        if (pCompRef)
            {
            hComponent = pCompRef->pComponentIF;
        	omxError = OMX_ErrorNone;
            
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
			OstTraceExt4( TRACE_API_BUFFER, _EmptyBufferDone2,
						  "EmptyBufferDone < ReturnVal=%{OMX_ERRORTYPE} Component=%p apAppData=%p apBufferHeader=%p",
            (TUint)omxError,hComponent,apAppData, apBuffer );
			if (apBuffer)
				{
				OstTraceData(TRACE_API_DUMP_STRUCT, _EmptyBufferDone3,
							 "Buffer Header Struct: %{hex8[]}",
							 apBuffer, (TUint)(apBuffer->nSize)) ;
				}
#endif
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL  
    TBuf8<ADDITIONAL_INFO_SIZE> Info;
    Info.Format(Ost_OMXIL_Performance::K_HandleOnlyInfo, hComponent);
    OstPrintf(TTraceContext(KTracePerformanceTraceUID,ESystemCharacteristicMetrics), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
            Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_EmptyBufferDone),omxError ,&Info);
#endif
            }

        }
    
    if(ahComponent)
        {
        omxError = pCompRef->pCallbacks->EmptyBufferDone(hComponent, apAppData, apBuffer);
        }
    
    return omxError;
    }

OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_FillBufferDone(
    OMX_OUT OMX_HANDLETYPE ahComponent,
    OMX_OUT OMX_PTR apAppData,
    OMX_OUT OMX_BUFFERHEADERTYPE* apBuffer)
{
    OMX_ERRORTYPE omxError = OMX_ErrorBadParameter;
    COMPONENT_REF* pCompRef = NULL;
    OMX_COMPONENTTYPE* hComponent = NULL;
    
    if(ahComponent)
        {
        pCompRef = ResolveCallsFromComponent(ahComponent);

        if (pCompRef)
            {
            hComponent = pCompRef->pComponentIF;
        	omxError = OMX_ErrorNone;
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    
    TBuf8<ADDITIONAL_INFO_SIZE> Info;
       Info.Format(Ost_OMXIL_Performance::K_HandleOnlyInfo, hComponent);
       OstPrintf(TTraceContext(KTracePerformanceTraceUID,ESystemCharacteristicMetrics), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
               Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_FillBufferDone),omxError ,&Info);
#endif
       
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
			OstTraceExt4( TRACE_API_BUFFER, _FillBufferDone2,
						  "FillBufferDone < ReturnVal=%{OMX_ERRORTYPE} Component=%p apAppData=%p apBufferHeader=%p",
                    (TUint)omxError, hComponent,apAppData, apBuffer );
			if (apBuffer)
				{
				OstTraceData(TRACE_API_DUMP_STRUCT, _FillBufferDone3,
							 "Buffer Header Struct: %{hex8[]}",
							 apBuffer, (TUint)(apBuffer->nSize)) ;
				}
#endif
            }
        }
      
       if(ahComponent)
           {
           omxError = pCompRef->pCallbacks->FillBufferDone(pCompRef->pComponentIF, apAppData, apBuffer);
           }
       return omxError;
}

OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_UseEGLImage(
        OMX_IN OMX_HANDLETYPE ahComponent,
        OMX_INOUT OMX_BUFFERHEADERTYPE** appBufferHdr,
        OMX_IN OMX_U32 aPortIndex,
        OMX_IN OMX_PTR apAppPrivate,
        OMX_IN void* aeglImage)
    {
    OMX_ERRORTYPE omxError = OMX_ErrorBadParameter;
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _UseEGLImage1, "UseEGLImage >" );
#endif 
    
    if(ahComponent)
        {
        OMX_COMPONENTTYPE *pComp = ResolveCallsToComponent(ahComponent);
        omxError = pComp->UseEGLImage(ahComponent, appBufferHdr, aPortIndex, apAppPrivate, aeglImage );
        }  
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTraceExt5( TRACE_API, _UseEGLImage2, "UseEGLImage < ReturnVal=%{OMX_ERRORTYPE} Component=%p appBufferHdr=%p nIndex=%u aeglImage=%p", 
            (TUint)omxError, ahComponent, (appBufferHdr)?*appBufferHdr: 0, (TUint)aPortIndex, aeglImage );
#endif  
    return omxError;
    }

OMX_ERRORTYPE OMXILAPITraceWrapper::Trace_ComponentRoleEnum(
    OMX_IN OMX_HANDLETYPE ahComponent,
    OMX_OUT OMX_U8 *aRole,
    OMX_IN OMX_U32 aIndex)
    {
    OMX_ERRORTYPE omxError = OMX_ErrorBadParameter;
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _ComponentRoleEnum1, "ComponentRoleEnum >" );
#endif 
    
    if(ahComponent)
        {
        OMX_COMPONENTTYPE *pComp = ResolveCallsToComponent(ahComponent);
        omxError = pComp->ComponentRoleEnum(pComp, aRole, aIndex);
        }
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTraceExt4( TRACE_API, _ComponentRoleEnum2, "ComponentRoleEnum < ReturnVal=%{OMX_ERRORTYPE} Component=%p cRole=%p nIndex=%u ", 
            (TUint)omxError, ahComponent, aRole, (TUint)aIndex );
#endif  
    return omxError;
    }
