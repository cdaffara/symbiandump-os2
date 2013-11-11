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


#include "baseprofilehandler.h"

_LIT(KErrDialogInvalidPortIndex, "Base Profile support command called on port not configured for base profile comms, component: %S, port: %d");
_LIT(KErrDialogBufferNumber, "Test script specified %d buffers, less than the port's minimum of %d, component: %S, port: %d");
_LIT(KErrDialogFillBufferInvalidBufIndex, "Test script specified invalid buffer index on a FillThisBuffer call, component: %S, port: %d, buffer index passed: %d");
_LIT(KErrDialogEmptyBufferInvalidBufIndex, "Test script specified invalid buffer index on a EmptyThisBuffer call, component: %S, port: %d, buffer index passed: %d");


CBaseProfileHandler::CBaseProfileHandler(ROmxScriptTest& aTestOwner, RASBreakEventHandler& aParentEventHandler)
: iErrorCallbacks(aTestOwner), iEventHandlerCallbacks(aParentEventHandler)
    {
    iOmxCallbacks.EmptyBufferDone = EmptyBufferDone;
    iOmxCallbacks.EventHandler = EventHandler;
    iOmxCallbacks.FillBufferDone = FillBufferDone;
    iMutex.CreateLocal();
    }

CBaseProfileHandler::~CBaseProfileHandler()
    {
    iMutex.Close();
    delete iXmlName;
    iBufferHeaders.ResetAndDestroy();
    iPortsUsingBaseProfile.Close();
    }

void CBaseProfileHandler::AddPortSupportL(TInt aPortIndex)
    {
    TPortInfo portInfo;
    portInfo.iPortIndex = aPortIndex;
    portInfo.iAutoMode = EFalse;
    portInfo.iWeAreBufferSupplier = ETrue;
    iPortsUsingBaseProfile.AppendL(portInfo);
    }

void CBaseProfileHandler::SetBufferSupplier(TInt aPortIndex, TBool aComponentBufferSupplier)
    {
    TInt portIndex = iPortsUsingBaseProfile.Find(aPortIndex, CBaseProfileHandler::PortIndexMatchComparison);
    if (portIndex == KErrNotFound)
        {
        TBuf<140> errorString;
        errorString.Format(KErrDialogInvalidPortIndex, iXmlName, aPortIndex);
        iErrorCallbacks.FailTest(errorString);
        return;
        }
    iPortsUsingBaseProfile[portIndex].iWeAreBufferSupplier = !aComponentBufferSupplier;
    }

void CBaseProfileHandler::SetAutoMode(TInt aPortIndex, TBool aAutoMode)
    {
    TInt portIndex = iPortsUsingBaseProfile.Find(aPortIndex, CBaseProfileHandler::PortIndexMatchComparison);
    if (portIndex == KErrNotFound)
        {
        TBuf<140> errorString;
        errorString.Format(KErrDialogInvalidPortIndex, iXmlName, aPortIndex);
        iErrorCallbacks.FailTest(errorString);
        return;
        }
    iPortsUsingBaseProfile[portIndex].iAutoMode = aAutoMode;
    }

TInt CBaseProfileHandler::LocateBufferForPort(TInt aPortIndex, TInt aBufferIndexInPort)
    {
    //Implementation based on the fact that:
    //-All the buffers for a port are allocated and appended to iBufferHeaders
    //in a single contiguous block.
    //-So once the first relevant buffer is found this index marks the zero index
    //for the set of indexed buffers relevant to that port.
    
    TInt result = -1;
    TInt bufferCount =  iBufferHeaders.Count();
    for (TInt bufferIndex = 0; bufferIndex < bufferCount; bufferIndex++)
        {
        if (iBufferHeaders[bufferIndex]->iPortIndex != aPortIndex)
            {
            continue;
            }
        
        //Safety check that the script hasn't made a manual Fill/Empty call on 
        //an invalid buffer index
        if (iBufferHeaders[bufferIndex]->iPortIndex == iBufferHeaders[(bufferIndex + aBufferIndexInPort)]->iPortIndex)
            {
            result = bufferIndex + aBufferIndexInPort;
            }
        break;
        }
    return result;
    }

void CBaseProfileHandler::FillThisBuffer(TInt aPortIndex, TInt aBufferIndexInPort)
    {
    TInt bufHeaderIndex = LocateBufferForPort(aPortIndex, aBufferIndexInPort);
    if (bufHeaderIndex >= 0)
        {
        iBufferHeaders[bufHeaderIndex]->iBufferAvailable = EFalse;
        iOmxComponent->FillThisBuffer(iOmxComponent, iBufferHeaders[bufHeaderIndex]->iBufferHeader);
        }
    else
        {
        TBuf<140> errorString;
        errorString.Format(KErrDialogFillBufferInvalidBufIndex, iXmlName, aPortIndex, aBufferIndexInPort);
        iErrorCallbacks.FailTest(errorString);
        }
    }

void CBaseProfileHandler::EmptyThisBuffer(TInt aPortIndex, TInt aBufferIndexInPort)
    {
    TInt bufHeaderIndex = LocateBufferForPort(aPortIndex, aBufferIndexInPort);
    if (bufHeaderIndex >= 0)
        {
        iBufferHeaders[bufHeaderIndex]->iBufferAvailable = EFalse;
        iOmxComponent->EmptyThisBuffer(iOmxComponent, iBufferHeaders[bufHeaderIndex]->iBufferHeader);
        }
    else
        {
        TBuf<140> errorString;
        errorString.Format(KErrDialogEmptyBufferInvalidBufIndex, iXmlName, aPortIndex, aBufferIndexInPort);
        iErrorCallbacks.FailTest(errorString);
        }    
    }

void CBaseProfileHandler::WaitForBufferCompletion(TInt aPortIndex, TInt aBufferIndexInPort)
    {
    TInt bufHeaderIndex = LocateBufferForPort(aPortIndex, aBufferIndexInPort);
    iMutex.Wait();
    if (iBufferHeaders[bufHeaderIndex]->iBufferAvailable)
        {
        //Buffer has already completed either while waiting on another buffer or as a result of test command processing delay, no need to wait
        iMutex.Signal();
        }
    else
        {
        iWaitingOnBuffer = iBufferHeaders[bufHeaderIndex]->iBufferHeader;
        iMutex.Signal();
        iErrorCallbacks.BeginWait();
        }

    }

OMX_ERRORTYPE CBaseProfileHandler::EventHandler(OMX_HANDLETYPE /*hComponent*/, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
    {
    reinterpret_cast<CBaseProfileHandler*>(pAppData)->HandleEventReceived(eEvent, nData1, nData2, pEventData);
    return OMX_ErrorNone;
    }

OMX_ERRORTYPE CBaseProfileHandler::FillBufferDone(OMX_IN OMX_HANDLETYPE /*hComponent*/, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
    {
    reinterpret_cast<CBaseProfileHandler*>(pAppData)->HandleFillBufferDone(pBuffer);
    return OMX_ErrorNone;
    }

OMX_ERRORTYPE CBaseProfileHandler::EmptyBufferDone(OMX_IN OMX_HANDLETYPE /*hComponent*/, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
    {
    reinterpret_cast<CBaseProfileHandler*>(pAppData)->HandleEmptyBufferDone(pBuffer);
    return OMX_ErrorNone;
    }

void CBaseProfileHandler::HandleEventReceived(OMX_EVENTTYPE aeEvent, OMX_U32 anData1, OMX_U32 anData2, OMX_PTR apEventData)
    {
    DoEventReceived(aeEvent, anData1, anData2, apEventData);
    iEventHandlerCallbacks.EventHandler(iOmxComponent, &iEventHandlerCallbacks, aeEvent, anData1, anData2, apEventData);
    }

void CBaseProfileHandler::HandleFillBufferDone(OMX_IN OMX_BUFFERHEADERTYPE* aFilledBuffer)
    {
    iMutex.Wait();
    DoFillBufferDone(aFilledBuffer);
    TInt bufferIndex = iBufferHeaders.Find(*aFilledBuffer, CBaseProfileHandler::BufferHeaderMatchComparison);
    iBufferHeaders[bufferIndex]->iBufferAvailable = ETrue;
    TInt portIndex = iPortsUsingBaseProfile.Find((*iBufferHeaders[bufferIndex]).iPortIndex, CBaseProfileHandler::PortIndexMatchComparison);
    
    if (iPortsUsingBaseProfile[portIndex].iAutoMode)
        {
        iBufferHeaders[bufferIndex]->iBufferAvailable = EFalse;
        iOmxComponent->FillThisBuffer(iOmxComponent, aFilledBuffer);
        }
    
    if (iWaitingOnBuffer == iBufferHeaders[bufferIndex]->iBufferHeader)
        {
        iWaitingOnBuffer = NULL;
        iErrorCallbacks.EndWait();
        }
    
    iMutex.Signal();
    }

void CBaseProfileHandler::HandleEmptyBufferDone(OMX_IN OMX_BUFFERHEADERTYPE* aEmptiedBuffer)
    {
    iMutex.Wait();
    DoEmptyBufferDone(aEmptiedBuffer);
    TInt bufferIndex = iBufferHeaders.Find(*aEmptiedBuffer, CBaseProfileHandler::BufferHeaderMatchComparison);
    iBufferHeaders[bufferIndex]->iBufferAvailable = ETrue;
    TInt portIndex = iPortsUsingBaseProfile.Find((*iBufferHeaders[bufferIndex]).iPortIndex, CBaseProfileHandler::PortIndexMatchComparison);
    
    if (iPortsUsingBaseProfile[portIndex].iAutoMode)
        {
        iBufferHeaders[bufferIndex]->iBufferAvailable = EFalse;
        iOmxComponent->EmptyThisBuffer(iOmxComponent, aEmptiedBuffer);
        }
    
    if (iWaitingOnBuffer == iBufferHeaders[bufferIndex]->iBufferHeader)
        {
        iWaitingOnBuffer = NULL;
        iErrorCallbacks.EndWait();
        }
    
    iMutex.Signal();
    }
   
OMX_COMPONENTTYPE* CBaseProfileHandler::LoadComponentL(const TDesC8& aTestSpecificName, const TDesC8& aOmxName)
    {
    
    iXmlName = HBufC::NewL(aTestSpecificName.Length());
	iXmlName->Des().Copy(aTestSpecificName);
    // allow room for the '\0' used in call to OMX_GetHandle
    HBufC8* omxNameToLoad = HBufC8::NewL(aOmxName.Length() + 1);
    *omxNameToLoad = aOmxName;
    
    OMX_ERRORTYPE error = OMX_GetHandle((TAny**) &iOmxComponent, (OMX_STRING) omxNameToLoad->Des().PtrZ(), this, &iOmxCallbacks);
    delete omxNameToLoad;
    if (error != OMX_ErrorNone)
        {
        iErrorCallbacks.FailWithOmxError(_L("OMX_GetHandle()"), error);
        }
    
    return iOmxComponent;
    }

void CBaseProfileHandler::FreeAllocatedBuffersL()	
	{
	for (TInt i = 0; i < iBufferHeaders.Count(); ++i)
		{
		delete iBufferHeaders[i];
		}
	iBufferHeaders.Reset();
	}
	
void CBaseProfileHandler::SetupBuffersL(TInt aPortIndex, TInt aNumberBuffers)
    {
    TInt portInfoIndex = iPortsUsingBaseProfile.Find(aPortIndex, CBaseProfileHandler::PortIndexMatchComparison);
    if (portInfoIndex == KErrNotFound)
        {
        TBuf<140> errorString;
        errorString.Format(KErrDialogInvalidPortIndex, iXmlName, aPortIndex);
        iErrorCallbacks.FailTest(errorString);
        return;   
        }
    
    OMX_PARAM_PORTDEFINITIONTYPE portDef;
    portDef.nSize = sizeof(portDef);
    portDef.nVersion = KOmxVersion;
    portDef.nPortIndex = aPortIndex;
    OMX_ERRORTYPE error = iOmxComponent->GetParameter(iOmxComponent, OMX_IndexParamPortDefinition, &portDef);
    if(error != OMX_ErrorNone)
        {
        iErrorCallbacks.FailWithOmxError(_L("GetParameter"), error);
        return;
        }

    if (aNumberBuffers < portDef.nBufferCountMin)
        {
        TBuf<140> errorString;
        errorString.Format(KErrDialogBufferNumber, aNumberBuffers, portDef.nBufferCountMin, iXmlName, aPortIndex);
        iErrorCallbacks.FailTest(errorString);
        return;
        }
    
    TInt bufSize = portDef.nBufferSize;    
    
    for (TInt bufIndex=0; bufIndex < aNumberBuffers; bufIndex++)
        {
        CBufferHeaderInfo* headerInfo = new (ELeave) CBufferHeaderInfo(*this);
        iBufferHeaders.AppendL(headerInfo);
        headerInfo->iPortIndex = aPortIndex;
             
        if (iPortsUsingBaseProfile[portInfoIndex].iWeAreBufferSupplier)
            {
            OMX_U8* buffer = new (ELeave) OMX_U8[bufSize];
            iOmxComponent->UseBuffer(iOmxComponent, &(headerInfo->iBufferHeader), aPortIndex, NULL, bufSize, buffer);
            }
        else
            {
            iOmxComponent->AllocateBuffer(iOmxComponent, &(headerInfo->iBufferHeader), aPortIndex, NULL, bufSize);
            }
        }
    }

void CBaseProfileHandler::DoEventReceived(OMX_EVENTTYPE /*aeEvent*/, OMX_U32 /*anData1*/, OMX_U32 /*anData2*/, OMX_PTR /*apEventData*/)
    {
    //Uninterested
    }

void CBaseProfileHandler::DoFillBufferDone(OMX_BUFFERHEADERTYPE* /*aFilledBuffer*/)
    {
    //Uninterested
    }

void CBaseProfileHandler::DoEmptyBufferDone(OMX_BUFFERHEADERTYPE* /*aEmptiedBuffer*/)
    {
    //Uninterested
    }

CBaseProfileHandler::CBufferHeaderInfo::CBufferHeaderInfo(CBaseProfileHandler& aParent)
: iBufferAvailable(ETrue), iParent(aParent)
    {
    }
    
CBaseProfileHandler::CBufferHeaderInfo::~CBufferHeaderInfo()
    {
    OMX_U8* buffer = iBufferHeader->pBuffer;
    OMX_ERRORTYPE error = iParent.iOmxComponent->FreeBuffer(iParent.iOmxComponent, iPortIndex, iBufferHeader);
    if (error != OMX_ErrorNone)
        {
        iParent.iErrorCallbacks.FailWithOmxError(_L("OMX_FreeBuffer()"), error);
        return;
        }
    
    TInt portInfoIndex = iParent.iPortsUsingBaseProfile.Find(iPortIndex, CBaseProfileHandler::PortIndexMatchComparison);
    if (iParent.iPortsUsingBaseProfile[portInfoIndex].iWeAreBufferSupplier)
        {
        delete[] buffer; 
        }
    }
        
TBool CBaseProfileHandler::PortIndexMatchComparison(const TInt* aPortIndex, const TPortInfo& aPortInfo)
    {
    if (*aPortIndex == aPortInfo.iPortIndex)
        {
        return ETrue;
        }
    return EFalse;
    }

TBool CBaseProfileHandler::BufferHeaderMatchComparison(const OMX_BUFFERHEADERTYPE* aBufferHeader, const CBufferHeaderInfo& aBufferHeaderInfo)
    {
    if (aBufferHeader == aBufferHeaderInfo.iBufferHeader)
        {
        return ETrue;
        }
    return EFalse;
    }

