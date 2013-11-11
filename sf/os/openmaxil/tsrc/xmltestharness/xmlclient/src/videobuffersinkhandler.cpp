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
#include "videobuffersinkhandler.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>

CVideoBufferSinkHandler* CVideoBufferSinkHandler::NewL(
    TDesC& aFilename, 
    OMX_SYMBIAN_PARAM_3PLANE_CHUNK_DATA& aChunkDataMsg,
    const OMX_SYMBIAN_PARAM_BUFFER_SIZE& aBufferSize, 
    OMX_PARAM_PORTDEFINITIONTYPE& portDef)
	{
	CVideoBufferSinkHandler* handler = new (ELeave) CVideoBufferSinkHandler(aFilename);
	CleanupStack::PushL(handler);
	handler->ConstructL(aChunkDataMsg, aBufferSize, portDef);
	CleanupStack::Pop(handler);

	return handler;
	}
	
CVideoBufferSinkHandler::~CVideoBufferSinkHandler()
	{
	iReceiveBuffer.Close();
	iSendBuffer.Close();
	iBufferChunk.Close();
	iFileSink.Close();
	iFs.Close();
	}

CVideoBufferSinkHandler::CVideoBufferSinkHandler(TDesC& aFilename):
	CActive(EPriorityStandard),
	iWriteDes(NULL, 0, 0)
	{
	CActiveScheduler::Add(this);
	iFileName.Append(aFilename);
	}

void CVideoBufferSinkHandler::ConstructL(OMX_SYMBIAN_PARAM_3PLANE_CHUNK_DATA& aChunkDataMsg, const OMX_SYMBIAN_PARAM_BUFFER_SIZE& aBufferSize, OMX_PARAM_PORTDEFINITIONTYPE& portDef)
    {
    iBufferChunk.SetHandle(aChunkDataMsg.nChunk);
    User::LeaveIfError(iBufferChunk.Duplicate(RThread()));
       
    iReceiveBuffer.SetHandle(aChunkDataMsg.nFilledBufferQueue );
    User::LeaveIfError(iReceiveBuffer.Duplicate(RThread()));
       
    iSendBuffer.SetHandle(aChunkDataMsg.nAvailableBufferQueue); 
    User::LeaveIfError(iSendBuffer.Duplicate(RThread()));
    
   	if (portDef.nBufferCountActual <= 0 || portDef.nBufferSize <= 0)
		{
		User::Leave(KErrArgument);
		}
	
	iBufConfig.iNumBuffers = portDef.nBufferCountActual;
	iBufConfig.iBufferSizeInBytes = aBufferSize.nBufferSize;
	
	// Open datafile
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFileSink.Replace(iFs, iFileName, EFileShareExclusive|EFileWrite));
	}

void CVideoBufferSinkHandler::Start()
	{
	// Start streaming
	iReceiveBuffer.NotifyDataAvailable(iStatus);
	SetActive();
	}

void CVideoBufferSinkHandler::RunL()
	{
	TFilledBufferHeaderV2 header;
	User::LeaveIfError(iReceiveBuffer.Receive(header));

	TUint8* chunkBase = iBufferChunk.Base();

	TUint8* bufBase = chunkBase + header.iOffset;

	iWriteDes.Set(bufBase, header.iFilledLength, iBufConfig.iBufferSizeInBytes);

	__ASSERT_ALWAYS(header.iFilledLength <= iWriteDes.Length(), User::Invariant());
	
	iFileSink.Write(iWriteDes);
	
	if (header.iFlags != OMX_BUFFERFLAG_EOS)
		{
		// there is no more to read from file source, request for more buffer
		iReceiveBuffer.NotifyDataAvailable(iStatus);
		SetActive();
		}
	
	User::LeaveIfError(iSendBuffer.Send(header));
	}

void CVideoBufferSinkHandler::DoCancel()
	{
	iReceiveBuffer.CancelDataAvailable();
	}



