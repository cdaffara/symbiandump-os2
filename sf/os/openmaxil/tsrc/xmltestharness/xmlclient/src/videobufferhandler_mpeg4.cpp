/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include <gdi.h>
#include "videobufferhandler_mpeg4.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <e32debug.h>


CVideoBufferHandlerMPEG4* CVideoBufferHandlerMPEG4::NewL(
    TDesC& aFilename,
    OMX_SYMBIAN_PARAM_3PLANE_CHUNK_DATA& aChunkDataMsg,
    const OMX_SYMBIAN_PARAM_BUFFER_SIZE& aBufferSize,
    OMX_PARAM_PORTDEFINITIONTYPE& portDef)
	{
	CVideoBufferHandlerMPEG4* handler = new (ELeave) CVideoBufferHandlerMPEG4(aFilename);
	CleanupStack::PushL(handler);
	handler->ConstructL(aChunkDataMsg, aBufferSize, portDef);
	CleanupStack::Pop(handler);

	return handler;
	}
	
CVideoBufferHandlerMPEG4::~CVideoBufferHandlerMPEG4()
	{
	delete iFrameSupplier;
	}

CVideoBufferHandlerMPEG4::CVideoBufferHandlerMPEG4(TDesC& aFilename):
	CVideoBufferHandler(aFilename)
	{
	}

void CVideoBufferHandlerMPEG4::RunL()
	{
	// Timer has expired check for new messages...
	TFilledBufferHeaderV2 header;
	
	User::LeaveIfError(iReceiveBuffer.Receive(header));
	TUint8* chunkBase = iBufferChunk.Base();
	//__ASSERT_ALWAYS(message < iBufConfig.iNumBuffers, User::Invariant());

	TUint8* bufBase = chunkBase + header.iOffset;

	TInt frameLength = 0;
	TInt err = iFrameSupplier->NextFrameData(bufBase, frameLength);
	if (err != KErrNone)
	    {
	    User::Leave(err);
	    }
	
	if (iChangeFileBufferLength)
	    {
	    // this is negative test case. COmxILCallbackManager::BufferDoneNotification() will panic
	    header.iFilledLength = iFileBufferLengthTestValue;
	    }
	else
	    {
	    header.iFilledLength = frameLength;
	    }
	    
	if (!iFrameSupplier->IsLastFrame())
		{
		// there is more to read from file source, request for more buffer
		header.iTimeStamp = 0;
		header.iFlags = (iFrameSupplier->CurrentFrame() == 0 ? OMX_BUFFERFLAG_STARTTIME : 0);
		iReceiveBuffer.NotifyDataAvailable(iStatus);
		SetActive();
		}
	else
		{
		header.iFlags = OMX_BUFFERFLAG_EOS;
		}
		
	if (iSendInvalidBufferId)
		{
		User::LeaveIfError(iSendBuffer.Send(header));
		iSendInvalidBufferId = 0;
		}
	else
	    {
		User::LeaveIfError(iSendBuffer.Send(header));
	    }
	}
	

void CVideoBufferHandlerMPEG4::ConstructL(OMX_SYMBIAN_PARAM_3PLANE_CHUNK_DATA& aChunkDataMsg, const OMX_SYMBIAN_PARAM_BUFFER_SIZE& aBufferSize, OMX_PARAM_PORTDEFINITIONTYPE& portDef)
	{
	RThread chunkOwnerThread;
	
    TInt error = chunkOwnerThread.Open(TThreadId(aChunkDataMsg.nThreadId));    
	iBufferChunk.SetHandle(aChunkDataMsg.nChunk);
	User::LeaveIfError(iBufferChunk.Duplicate(chunkOwnerThread));
	
	iReceiveBuffer.SetHandle(aChunkDataMsg.nAvailableBufferQueue);
	User::LeaveIfError(iReceiveBuffer.Duplicate(RThread()));
	
	iSendBuffer.SetHandle(aChunkDataMsg.nFilledBufferQueue);
	User::LeaveIfError(iSendBuffer.Duplicate(RThread()));
	
	if ((portDef.nBufferCountActual <= 0) || (portDef.nBufferSize <= 0))
		{
		User::Leave(KErrArgument);
		}
	
	iFrameSupplier = CVideoFrameSupplier::NewL(iFileName, CVideoFrameSupplier::EVideoFileTypeMPEG4);
	
    iBufConfig.iNumBuffers = portDef.nBufferCountActual;
    iBufConfig.iBufferSizeInBytes = aBufferSize.nBufferSize;
	}


void CVideoBufferHandlerMPEG4::SetChunkDetailAndOpenL(TInt aChunkHandleId, TUint64 aThreadId)
    {
    RThread chunkOwnerThread;
    
    TInt err = chunkOwnerThread.Open(TThreadId(aThreadId));
    if (err != KErrNone)
        {
        User::LeaveIfError(err);
        }
    
    CleanupClosePushL(chunkOwnerThread);
    
    iBufferChunk.SetHandle(aChunkHandleId);
    err = iBufferChunk.Duplicate(chunkOwnerThread);
    if (err != KErrNone)
        {
        User::LeaveIfError(err);
        }
    
    CleanupStack::PopAndDestroy(&chunkOwnerThread);
    }



