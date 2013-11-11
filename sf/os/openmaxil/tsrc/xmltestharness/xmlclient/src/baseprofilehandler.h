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


#ifndef BASEPROFILEHANDLER_H_
#define BASEPROFILEHANDLER_H_

#include <e32base.h>

#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>

#include "asbreakeventhandler.h"
#include "omxscripttest.h"


NONSHARABLE_CLASS(CBaseProfileHandler) : public CBase
    {
public:
    CBaseProfileHandler(ROmxScriptTest& aTestOwner, RASBreakEventHandler& aParentEventHandler);
    ~CBaseProfileHandler();
    
    OMX_COMPONENTTYPE* LoadComponentL(const TDesC8& aTestSpecificName, const TDesC8& aOmxName);
    
    void AddPortSupportL(TInt aPortIndex);
    void SetBufferSupplier(TInt aPortIndex, TBool aComponentBufferSupplier);
    void SetAutoMode(TInt aPortIndex, TBool aAutoMode);
    void SetupBuffersL(TInt aPortIndex, TInt aNumberBuffers);
	void FreeAllocatedBuffersL();
    void FillThisBuffer(TInt aPortIndex, TInt aBufferIndexInPort);
    void EmptyThisBuffer(TInt aPortIndex, TInt aBufferIndexInPort);
    void WaitForBufferCompletion(TInt aPortIndex, TInt aBufferIndexInPort);
    
    //OpenMAX IL callbacks
    static OMX_ERRORTYPE EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
    static OMX_ERRORTYPE FillBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
    static OMX_ERRORTYPE EmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
    
    void HandleEventReceived(OMX_EVENTTYPE aeEvent, OMX_U32 anData1, OMX_U32 anData2, OMX_PTR apEventData);
    void HandleFillBufferDone(OMX_IN OMX_BUFFERHEADERTYPE* aFilledBuffer);
    void HandleEmptyBufferDone(OMX_IN OMX_BUFFERHEADERTYPE* aEmptiedBuffer);
    
protected:
    
    NONSHARABLE_CLASS(CBufferHeaderInfo) : public CBase
        {
    public:
        CBufferHeaderInfo(CBaseProfileHandler& aParent);
        ~CBufferHeaderInfo();
        
    public:
        OMX_BUFFERHEADERTYPE* iBufferHeader;    
        TBool iBufferAvailable;     //EFalse if buffer currently with IL Component
        TInt iPortIndex;    //Port the buffer is assigned to
        CBaseProfileHandler& iParent;
        };
    
    struct TPortInfo
        {
        TInt iPortIndex;
        TBool iWeAreBufferSupplier;
        TBool iAutoMode;
        };
    
    virtual void DoEventReceived(OMX_EVENTTYPE aeEvent, OMX_U32 anData1, OMX_U32 anData2, OMX_PTR apEventData);
    virtual void DoFillBufferDone(OMX_IN OMX_BUFFERHEADERTYPE* aFilledBuffer);
    virtual void DoEmptyBufferDone(OMX_IN OMX_BUFFERHEADERTYPE* aEmptiedBuffer); 

    static TBool PortIndexMatchComparison(const TInt* aPortIndex, const TPortInfo& aPortInfo);
    static TBool BufferHeaderMatchComparison(const OMX_BUFFERHEADERTYPE* aBufferHeader, const CBufferHeaderInfo& aBufferHeaderInfo);

    //aBufferIndexInPort starts at 0 for the first buffer
    TInt LocateBufferForPort(TInt aPortIndex, TInt aBufferIndexInPort);
    
protected:
    OMX_CALLBACKTYPE iOmxCallbacks;
    OMX_COMPONENTTYPE* iOmxComponent;   //Not owned    

    RPointerArray<CBufferHeaderInfo> iBufferHeaders;

    RArray<TPortInfo> iPortsUsingBaseProfile;
     
    HBufC* iXmlName;
    
    ROmxScriptTest& iErrorCallbacks;
    RASBreakEventHandler& iEventHandlerCallbacks;
 
    OMX_BUFFERHEADERTYPE* iWaitingOnBuffer; //Not owned, only ever one wait request pending at any one time so this suffices
    
    RMutex iMutex;  //Used to ensure serialised execution of buffer completions and calls to wait on buffer completions
    };

#endif /* BASEPROFILEHANDLER_H_ */
