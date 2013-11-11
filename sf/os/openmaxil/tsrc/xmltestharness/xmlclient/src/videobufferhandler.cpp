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
#include "videobufferhandler.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>

const TInt KDefaultTimeInterval = 100000;

CVideoBufferHandler* CVideoBufferHandler::NewL(
    TDesC& aFilename, 
    OMX_SYMBIAN_PARAM_3PLANE_CHUNK_DATA& aChunkDataMsg,
    const OMX_SYMBIAN_PARAM_BUFFER_SIZE& aBufferSize, 
    OMX_PARAM_PORTDEFINITIONTYPE& portDef)
	{
	CVideoBufferHandler* handler = new (ELeave) CVideoBufferHandler(aFilename);
	CleanupStack::PushL(handler);
	handler->ConstructL(aChunkDataMsg, aBufferSize, portDef);
	CleanupStack::Pop(handler);

	return handler;
	}
	
CVideoBufferHandler::~CVideoBufferHandler()
	{
	iReceiveBuffer.Close();
	iSendBuffer.Close();
	iBufferChunk.Close();
	iFileSource.Close();
	iFs.Close();
	}

void CVideoBufferHandler::DoCancel()
	{
	iReceiveBuffer.CancelDataAvailable();
	}

CVideoBufferHandler::CVideoBufferHandler(TDesC& aFilename):
	CActive(EPriorityStandard),
	iReadDes(NULL, 0, 0),
	iFirstBuffer(ETrue)
	{
	CActiveScheduler::Add(this);
	iFileName.Append(aFilename);
	}

void CVideoBufferHandler::ChangeFilledBufferLength(TInt aLength)
    {
    if (!iChangeFileBufferLength)
        {
        iChangeFileBufferLength = ETrue;
        }
    iFileBufferLengthTestValue = aLength;
    }
    
    
void CVideoBufferHandler::SetSendInvalidBufferId(TInt aSendInvalidBufferId)    
	{
	iSendInvalidBufferId = aSendInvalidBufferId;
	}

void CVideoBufferHandler::RunL()
	{
	// Timer has expired check for new messages...
	TFilledBufferHeaderV2 header;
	User::LeaveIfError(iReceiveBuffer.Receive(header));
	TUint8* chunkBase = iBufferChunk.Base();

	TUint8* bufBase = chunkBase + header.iOffset;
	
	iReadDes.Set(bufBase, 0, iBufConfig.iBufferSizeInBytes);
	iFileSource.Read(iReadDes);
		
	if (iChangeFileBufferLength)
	    {
	    // this is negative test case. COmxILCallbackManager::BufferDoneNotification() will panic
	    header.iFilledLength = iFileBufferLengthTestValue;
	    }
	else
	    {
	    header.iFilledLength = iReadDes.Length();
	    }

	iTimeStamp += KDefaultTimeInterval;
	header.iTimeStamp = iTimeStamp;	
	
	if (iReadDes.Length() != 0)
		{
		// there is more to read from file source, request for more buffer
		header.iFlags = iFirstBuffer ? OMX_BUFFERFLAG_STARTTIME : 0;
		iReceiveBuffer.NotifyDataAvailable(iStatus);
		SetActive();
		}
	else
		{
		header.iFlags = OMX_BUFFERFLAG_EOS;
		}
		
	if (iSendInvalidBufferId)
		{
		header.iOffset=KMaxTUint32;
		User::LeaveIfError(iSendBuffer.Send(header));
		iSendInvalidBufferId = 0;
		}
	else
		User::LeaveIfError(iSendBuffer.Send(header));
	}
	
void CVideoBufferHandler::ConstructL(OMX_SYMBIAN_PARAM_3PLANE_CHUNK_DATA& aChunkDataMsg, const OMX_SYMBIAN_PARAM_BUFFER_SIZE& aBufferSize, OMX_PARAM_PORTDEFINITIONTYPE& portDef)

    {
    iBufferChunk.SetHandle(aChunkDataMsg.nChunk);
    User::LeaveIfError(iBufferChunk.Duplicate(RThread()));
    
    iReceiveBuffer.SetHandle(aChunkDataMsg.nAvailableBufferQueue );
    User::LeaveIfError(iReceiveBuffer.Duplicate(RThread()));
    
    iSendBuffer.SetHandle(aChunkDataMsg.nFilledBufferQueue);
    User::LeaveIfError(iSendBuffer.Duplicate(RThread()));
    
   	if (portDef.nBufferCountActual <= 0 || portDef.nBufferSize <= 0)
		{
		User::Leave(KErrArgument);
		}
	
	iBufConfig.iNumBuffers = portDef.nBufferCountActual;
	iBufConfig.iBufferSizeInBytes = aBufferSize.nBufferSize;
	
	// Open datafile
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFileSource.Open(iFs, iFileName, EFileShareExclusive|EFileRead));
	}

void CVideoBufferHandler::Start()
	{
	// Start streaming
	iFirstBuffer = ETrue;
	iReceiveBuffer.NotifyDataAvailable(iStatus);
	iTimeStamp = KDefaultTimeInterval;
	SetActive();
	}
