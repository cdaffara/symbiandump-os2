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
* Description: This class is provides performance trace wrappers around OpenMax IL APIs.
* This class shall not be instantiated.
*
*/


#ifndef  	__OMXILCOMPONENTAPIPERFORMANCETRACE_H
#define 	__OMXILCOMPONENTAPIPERFORMANCETRACE_H

#include <openmax/il/khronos/v1_x/OMX_Types.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <e32def.h>
#include <e32std.h>
#include "OSTOMXILFrameworkTrace.h"


struct COMPONENT_REF
    {
    OMX_COMPONENTTYPE*  pComponentIF;
    OMX_CALLBACKTYPE*   pCallbacks;
    };

/**
@publishedAll
@released

This class is provides performance trace wrappers around OpenMax IL APIs.
This class shall not be instantiated.
*/
class OMXILAPITraceWrapper
    {
private:
    // This class shall not be instantiated or inherited
    OMXILAPITraceWrapper ();
    OMXILAPITraceWrapper(OMXILAPITraceWrapper& tracewrapper);
    
public:
    static OMX_ERRORTYPE Trace_ComponentDeInit(OMX_IN  OMX_HANDLETYPE hComponent);
    static OMX_ERRORTYPE Trace_SetCallbacks(OMX_IN  OMX_HANDLETYPE ahComponent,OMX_IN  OMX_CALLBACKTYPE* apCallbacks,OMX_IN  OMX_PTR apAppData);
    static OMX_ERRORTYPE Trace_GetExtensionIndexL(OMX_IN  OMX_HANDLETYPE ahComponent,OMX_IN  OMX_STRING acParameterName,OMX_OUT OMX_INDEXTYPE* apIndexType);
    static OMX_ERRORTYPE Trace_GetComponentVersionL(OMX_IN  OMX_HANDLETYPE ahComponent,OMX_OUT OMX_STRING apComponentName,OMX_OUT OMX_VERSIONTYPE* apComponentVersion,OMX_OUT OMX_VERSIONTYPE* apSpecVersion, OMX_OUT OMX_UUIDTYPE* apComponentUUID);
    static OMX_ERRORTYPE Trace_UseBuffer(OMX_IN OMX_HANDLETYPE ahComponent,  OMX_INOUT OMX_BUFFERHEADERTYPE** appBufferHdr,OMX_IN OMX_U32 anPortIndex, OMX_IN OMX_PTR apAppPrivate, OMX_IN OMX_U32 anSizeBytes,OMX_IN OMX_U8* apBuffer);
    static OMX_ERRORTYPE Trace_FreeBuffer(OMX_IN  OMX_HANDLETYPE ahComponent, OMX_IN OMX_U32 anPortIndex,OMX_IN  OMX_BUFFERHEADERTYPE* apBuffer);
    static OMX_ERRORTYPE Trace_FillThisBuffer(OMX_IN  OMX_HANDLETYPE ahComponent,OMX_IN  OMX_BUFFERHEADERTYPE* apBuffer);
    static OMX_ERRORTYPE Trace_ComponentTunnelRequest(OMX_IN  OMX_HANDLETYPE ahComp,OMX_IN  OMX_U32 anPort,OMX_IN  OMX_HANDLETYPE ahTunneledComp,OMX_IN  OMX_U32 anTunneledPort,OMX_INOUT  OMX_TUNNELSETUPTYPE* apTunnelSetup);
    static OMX_ERRORTYPE Trace_EmptyThisBuffer( OMX_IN  OMX_HANDLETYPE ahComponent,OMX_IN  OMX_BUFFERHEADERTYPE* apBuffer);
    static OMX_ERRORTYPE Trace_AllocateBuffer(OMX_IN OMX_HANDLETYPE ahComponent,OMX_INOUT OMX_BUFFERHEADERTYPE** apBuffer,OMX_IN OMX_U32 anPortIndex,OMX_IN OMX_PTR apAppPrivate,OMX_IN OMX_U32 anSizeBytes);
    static OMX_ERRORTYPE Trace_FillBufferDone(OMX_OUT OMX_HANDLETYPE ahComponent,OMX_OUT OMX_PTR apAppData,OMX_OUT OMX_BUFFERHEADERTYPE* apBuffer);
    static OMX_ERRORTYPE Trace_GetConfig(OMX_IN  OMX_HANDLETYPE ahComponent,OMX_IN  OMX_INDEXTYPE anIndex,OMX_INOUT OMX_PTR apComponentConfigStructure);
    static OMX_ERRORTYPE Trace_SetConfig(OMX_IN  OMX_HANDLETYPE ahComponent,OMX_IN  OMX_INDEXTYPE anIndex,OMX_IN  OMX_PTR apComponentConfigStructure);
    static OMX_ERRORTYPE Trace_GetState(OMX_IN  OMX_HANDLETYPE ahComponent,OMX_OUT OMX_STATETYPE* apState);
    static OMX_ERRORTYPE Trace_SendCommand(OMX_IN  OMX_HANDLETYPE ahComponent,OMX_IN  OMX_COMMANDTYPE aCmd,OMX_IN  OMX_U32 anParam1,OMX_IN  OMX_PTR apCmdData);
    static OMX_ERRORTYPE Trace_EmptyBufferDone(OMX_OUT OMX_HANDLETYPE ahComponent,OMX_OUT OMX_PTR pAppData,OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);
    static OMX_ERRORTYPE Trace_EventHandler(OMX_IN OMX_HANDLETYPE ahComponent,OMX_IN OMX_PTR apAppData,OMX_IN OMX_EVENTTYPE aeEvent,OMX_IN OMX_U32 anData1,OMX_IN OMX_U32 anData2,OMX_IN OMX_PTR apEventData);
    static OMX_ERRORTYPE Trace_SetParameter( OMX_IN  OMX_HANDLETYPE ahComponent, OMX_IN  OMX_INDEXTYPE anIndex, OMX_IN  OMX_PTR aComponentParameterStructure);
    static OMX_ERRORTYPE Trace_GetParameter( OMX_IN  OMX_HANDLETYPE ahComponent, OMX_IN  OMX_INDEXTYPE anParamIndex,OMX_INOUT OMX_PTR aComponentParameterStructure);
    static OMX_ERRORTYPE Trace_UseEGLImage( OMX_IN OMX_HANDLETYPE ahComponent, OMX_INOUT OMX_BUFFERHEADERTYPE** appBufferHdr,OMX_IN OMX_U32 aPortIndex,OMX_IN OMX_PTR apAppPrivate,OMX_IN void* aeglImage);
    static OMX_ERRORTYPE Trace_ComponentRoleEnum (OMX_IN OMX_HANDLETYPE ahComponent, OMX_OUT OMX_U8 *aRole, OMX_IN OMX_U32 aIndex);
    
    /**
     Creates all the objects  required to make the tracetrampoline work        
    
    @param aClientHandle  Trace component handle will be passed passed to the client
    @param aTraceCallBack Trace callback structure that is passed to the component
    @param aCompRefToClient Keeps track of the real callback and component handle structure
    @param aCompRefToTrace  Keeps track of the trace callback and component handle structure           
    */
    
    static OMX_ERRORTYPE PrepareTraceStructuresL( OMX_INOUT OMX_HANDLETYPE& aClientHandle, OMX_INOUT OMX_CALLBACKTYPE*& aTraceCallBack, COMPONENT_REF*& aCompRefToClient, COMPONENT_REF*& aCompRefToTrace);
    
    /**
    Cleans up all the objects created for the trace trampoline        
    
    @param aClientHandle  Trace component handle that is passed to the client
    @param aTraceCallBack Trace callback structure that is passed to the component
    @param aCompRefToClient Keeps track of the real callback and component handle structure
    @param aCompRefToTrace  Keeps track of the trace callback and component handle structure
    */
    static OMX_ERRORTYPE UndoPrepareTraceStructures( OMX_INOUT OMX_HANDLETYPE& aClientHandle,  OMX_INOUT OMX_CALLBACKTYPE*& aTraceCallBack, COMPONENT_REF*& aCompRefToClient,  COMPONENT_REF*& aCompRefToTrace);
    
    /**
    Create a map of the Trace wrapper interface OMX_COMPONENTTYPE handle + OMX_CALLBACKTYPE and the client's
            OMX_COMPONENTTYPE handle + OMX_CALLBACKTYPE
            
    OMX_COMPONENTTYPE:pComponentPrivate is required by the component
    OMX_COMPONENTTYPE:pApplicationPrivate is required by the application
    This function stores the reference(COMPONENT_REF) to the real OMX_COMPONENTTYPE handle + OMX_CALLBACKTYPE
    in the client's OMX_COMPONENTTYPE:pComponentPrivate.
    It also stores the clients OMX_COMPONENTTYPE and OMX_CALLBACKTYPE structures which point to the Trace wrapper function pointers
    in the Trace Interface's pApplicationPrivate. 
    
    @param ahClientComp original component handle from client
    @param apClientCallBack original callback structure from Client
    @param ahTraceComp Trace component handle that is passed to the client
    @param apTraceCallBack Trace callback structure that is passed to the component
    @param aCompRefToClient Keeps track of the real callback and component handle structure
    @param aCompRefToTrace  Keeps track of the trace callback and component handle structure           
    */
    static TInt PatchStructures( OMX_IN OMX_HANDLETYPE ahClientComp, OMX_IN OMX_CALLBACKTYPE* apClientCallBack, OMX_IN OMX_HANDLETYPE ahTraceComp, OMX_INOUT OMX_CALLBACKTYPE* apTraceCallBack,COMPONENT_REF*& aCompRefToClient, COMPONENT_REF*& aCompRefToTrace);
    
   
    /**
     
    The function destroys the extra object that were created for the function tracing. Before the destruction
    it has to resolve the resolve the real component handle from the trace component handle.
    
    @param ahClientComp ahTraceComp Trace component handle that was passed to the client
    */
    static TInt ComponentTracerDestroy(OMX_IN OMX_HANDLETYPE ahClientComp);
    
    /**
     This function resolves the bridge between the client's handle to a trace wrapper OMX_COMPONENTTYPE 
     structure and the real  OMX_HANDLETYPE structure populated by the component.
      
     @param  ahComponent Trace component handle that was passed to the client
     */
    inline static OMX_COMPONENTTYPE* ResolveCallsToComponent(OMX_IN  OMX_HANDLETYPE ahComponent);
 
     /**
     This function resolves the bridge between the components's handle to a trace wrapper OMX_CALLBACKTYPE structure
      and the real clients OMX_CALLBACKTYPE structure 
     
     @param  ahComponent component handle that was passed to the component
     */
     inline static COMPONENT_REF* ResolveCallsFromComponent(OMX_IN  OMX_HANDLETYPE ahComponent);
    };

#include "omxilcomponentapiperformancetrace.inl"

#endif	//__OMXILCOMPONENTAPIPERFORMANCETRACE_H
