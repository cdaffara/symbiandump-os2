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


/**
@file
@internalComponent
*/

#include <e32base.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>

#ifndef OMXTHREADREQUEST_H
#define OMXTHREADREQUEST_H

_LIT(KXMLClientOMXThreadName, "XMLClientOMXThread");

NONSHARABLE_CLASS(COmxThreadRequest) : public CBase
    {
public:
    static COmxThreadRequest * NewL();
    ~COmxThreadRequest();
 
    OMX_ERRORTYPE GetHandle(
        OMX_OUT OMX_HANDLETYPE* pHandle, 
        OMX_IN  OMX_STRING cComponentName,
        OMX_IN  OMX_PTR pAppData,
        OMX_IN  OMX_CALLBACKTYPE* pCallBacks);

    OMX_ERRORTYPE FreeHandle(OMX_IN  OMX_HANDLETYPE hComponent);
    
private:
        enum TOMXCoreFunctionCode
            {
            EOMXFunctionCodeNone,
            EOMXFunctionCodeGetHandle,
            EOMXFunctionCodeFreeHandle,
            EOMXFunctionCodeTerminate
            
            // ... more to be added when required
            };
private:
    COmxThreadRequest();
    void ConstructL();
    void Terminate();
    void DoRequest();
    
    static TInt ThreadCreateFunction(TAny* aPtr);
    static void ThreadTrapFunctionL(TAny* aPtr);
    
private:
    
    TOMXCoreFunctionCode iFunction;
    
    RThread iRequestThread;
    TRequestStatus iRequestThreadStatus;
    //TThreadId iRequestThreadId;
    TBool iThreadCreated;
    
    TRequestStatus* iCallingStatus; // not owned
    TThreadId iCallingThreadId;
    
    OMX_ERRORTYPE iErrorType;  // not owned
    OMX_HANDLETYPE* ipHandle;  // not owned
    OMX_STRING icComponentName;  // not owned
    OMX_PTR ipAppData;   // not owned
    OMX_CALLBACKTYPE* ipCallBacks;  // not owned
    OMX_HANDLETYPE ihComponent;   // not owned
    };

/** Internal class for passing parameter to OMX thread */
class TThreadParam
    {
public:
    TThreadId iMainThreadId;
    TRequestStatus* iSetupComplete; //Not owned
    COmxThreadRequest* iThreadRequestObj; //Not owned
    };  

#endif //OMXTHREADREQUEST_H
