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


#include "nontunneledhandler.h"

CNonTunneledHandler::CNonTunneledHandler()
	{
	}

CNonTunneledHandler::~CNonTunneledHandler()
	{
	}

void CNonTunneledHandler::AddLinkL(OMX_COMPONENTTYPE* aSourceComp,
                                   TInt aSourcePort,
                                   OMX_COMPONENTTYPE* aSinkComp,
                                   TInt aSinkPort,
                                   OMX_BUFFERSUPPLIERTYPE aSupplier,
                                   TInt aNumBufs, TInt aBufSize)
	{
	CComponentInfo* compInfo = NULL;
	TInt pos = iComponents.Find(*aSourceComp, CComponentInfo::CompareComp);
	if (pos >= 0)
		{
		compInfo = iComponents[pos];
		}
	else
		{
		compInfo = new (ELeave) CComponentInfo(aSourceComp);
		CleanupStack::PushL(compInfo);
		iComponents.AppendL(compInfo);
		CleanupStack::Pop(compInfo);
		}

	CPortInfo* sourcePortInfo = compInfo->AddPortL(aSourcePort, ETrue, aNumBufs, aBufSize, aSupplier);

	pos = iComponents.Find(*aSinkComp, CComponentInfo::CompareComp);
	if (pos >= 0)
		{
		compInfo = iComponents[pos];
		}
	else
		{
		compInfo = new (ELeave) CComponentInfo(aSinkComp);
		CleanupStack::PushL(compInfo);
		iComponents.AppendL(compInfo);
		CleanupStack::Pop(compInfo);
		}

	CPortInfo* sinkPortInfo = compInfo->AddPortL(aSinkPort, EFalse, aNumBufs, aBufSize, aSupplier);

	sourcePortInfo->SetPeer(sinkPortInfo);
	sinkPortInfo->SetPeer(sourcePortInfo);
	}

void CNonTunneledHandler::AllocateBuffersL(OMX_COMPONENTTYPE* aComp)
	{
	TInt pos = iComponents.Find(*aComp, CComponentInfo::CompareComp);
	if (pos >= 0)
		{
		iComponents[pos]->AllocateBuffersL();
		}
	}

void CNonTunneledHandler::FillBuffers(OMX_COMPONENTTYPE* aComp)
	{
	TInt pos = iComponents.Find(*aComp, CComponentInfo::CompareComp);
	if (pos >= 0)
		{
		iComponents[pos]->FillBuffers();
		}	
	}

void CNonTunneledHandler::BufferDone(OMX_COMPONENTTYPE* aComp, OMX_BUFFERHEADERTYPE* aBufHdr, TBool aSource)
	{
	TInt pos = iComponents.Find(*aComp, CComponentInfo::CompareComp);
	if (pos >= 0)
		{
		iComponents[pos]->BufferDone(aBufHdr, aSource);
		}
	}

void CNonTunneledHandler::HoldBuffers(OMX_COMPONENTTYPE* aComp)
	{
	TInt pos = iComponents.Find(*aComp, CComponentInfo::CompareComp);
	if (pos >= 0)
		{
		iComponents[pos]->HoldBuffers();
		}	
	}

void CNonTunneledHandler::FreeBuffers(OMX_COMPONENTTYPE* aComp)
	{
	TInt pos = iComponents.Find(*aComp, CComponentInfo::CompareComp);
	if (pos >= 0)
		{
		iComponents[pos]->FreeBuffers();
		}	
	}

CNonTunneledHandler::CComponentInfo::CComponentInfo(OMX_COMPONENTTYPE* aComp) :
 iComp(aComp)
	{
	}

CNonTunneledHandler::CComponentInfo::~CComponentInfo()
	{
	iPorts.ResetAndDestroy();
	}

CNonTunneledHandler::CPortInfo* CNonTunneledHandler::CComponentInfo::AddPortL(TInt aPortNum, TBool aSource, TInt aNumBufs, TInt aBufSize, OMX_BUFFERSUPPLIERTYPE aSupplier)
	{
	__ASSERT_ALWAYS(iPorts.Find(aPortNum, CPortInfo::ComparePortNum) == KErrNotFound, User::Invariant());

	CPortInfo* portInfo = new (ELeave) CPortInfo(iComp, aPortNum, aSource, aNumBufs, aBufSize, aSupplier);
	CleanupStack::PushL(portInfo);
	iPorts.AppendL(portInfo);
	CleanupStack::Pop(portInfo);

	return portInfo;
	}

void CNonTunneledHandler::CComponentInfo::AllocateBuffersL()
	{
	TInt numPorts = iPorts.Count();
	
	for (TInt port = 0; port < numPorts; ++port)
		{
		iPorts[port]->AllocateBuffersL();
		}
	}

void CNonTunneledHandler::CComponentInfo::FillBuffers()
	{
	TInt numPorts = iPorts.Count();
	
	for (TInt port = 0; port < numPorts; ++port)
		{
		iPorts[port]->FillBuffers();
		}
	}

void CNonTunneledHandler::CComponentInfo::BufferDone(OMX_BUFFERHEADERTYPE* aBufHdr, TBool aSource)
	{
	TInt pos = KErrNotFound;

	if (aSource)
		{
		pos = iPorts.Find(static_cast<TInt>(aBufHdr->nOutputPortIndex), CPortInfo::ComparePortNum);
		}
	else
		{
		pos = iPorts.Find(static_cast<TInt>(aBufHdr->nInputPortIndex), CPortInfo::ComparePortNum);
		}

	__ASSERT_ALWAYS(pos != KErrNotFound, User::Invariant());

	iPorts[pos]->BufferDone(aBufHdr);
	}

void CNonTunneledHandler::CComponentInfo::HoldBuffers()
	{
	for (TInt port = 0; port < iPorts.Count(); ++port)
		{
		iPorts[port]->HoldBuffers();
		}
	}

void CNonTunneledHandler::CComponentInfo::FreeBuffers()
	{
	for (TInt port = 0; port < iPorts.Count(); ++port)
		{
		iPorts[port]->FreeBuffers();
		}
	}

TBool CNonTunneledHandler::CComponentInfo::CompareComp(const OMX_COMPONENTTYPE* aComp, const CComponentInfo& aComponentInfo)
	{
	return (aComp == aComponentInfo.iComp);
	}

CNonTunneledHandler::CPortInfo::CPortInfo(OMX_COMPONENTTYPE* aComp, TInt aPortNum, TBool aSource, TInt aNumBufs, TInt aBufSize, OMX_BUFFERSUPPLIERTYPE aSupplier) :
 iComp(aComp),
 iPortNum(aPortNum),
 iSource(aSource),
 iNumBufs(aNumBufs),
 iBufSize(aBufSize),
 iSupplier(aSupplier),
 iEosSignalledtoInput(FALSE)
	{
	}

CNonTunneledHandler::CPortInfo::~CPortInfo()
	{
	DeleteAllBuffers();
	}

void CNonTunneledHandler::CPortInfo::SetPeer(CPortInfo* aPeer)
	{
	iPeer = aPeer;
	}

void CNonTunneledHandler::CPortInfo::AllocateBuffersL()
	{
	OMX_BUFFERHEADERTYPE* bufHdr = NULL;

	if (iSource)
		{
		if (iSupplier == OMX_BufferSupplyUnspecified)
			{
			OMX_U8* newBuffer = NULL;

			for (TInt buf = 0; buf < iNumBufs; ++buf)
				{
				newBuffer = new (ELeave) OMX_U8[iBufSize];
				CleanupStack::PushL(newBuffer);
				iBuffers.AppendL(newBuffer);
				CleanupStack::Pop(newBuffer);
				iComp->UseBuffer(iComp, &bufHdr, iPortNum, NULL, iBufSize, newBuffer);
				iBufferHeaders.AppendL(bufHdr);
				SendAllocatedBufferToPeerL(newBuffer);
				}
			}
		else if (iSupplier == OMX_BufferSupplyOutput)
			{
			for (TInt buf = 0; buf < iNumBufs; ++buf)
				{			
				iComp->AllocateBuffer(iComp, &bufHdr, iPortNum, NULL, iBufSize);
				iBufferHeaders.AppendL(bufHdr);
				SendAllocatedBufferToPeerL(bufHdr->pBuffer);
				}
			}
		else
			{
			TInt bufCount = iBuffers.Count();

			for (TInt buf = 0; buf < bufCount; ++buf)
				{
				iComp->UseBuffer(iComp, &bufHdr, iPortNum, NULL, iBufSize, iBuffers[buf]);
				iBufferHeaders.AppendL(bufHdr);
				}

			if (bufCount < iNumBufs)
				{
				iWaitingForBuffers = ETrue;
				}
			}
		}
	else
		{
		if (iSupplier == OMX_BufferSupplyInput)
			{
			for (TInt buf = 0; buf < iNumBufs; ++buf)
				{
				iComp->AllocateBuffer(iComp, &bufHdr, iPortNum, NULL, iBufSize);
				iBufferHeaders.AppendL(bufHdr);
				SendAllocatedBufferToPeerL(bufHdr->pBuffer);
				}
			}
		else
			{
			TInt bufCount = iBuffers.Count();

			for (TInt buf = 0; buf < bufCount; ++buf)
				{
				iComp->UseBuffer(iComp, &bufHdr, iPortNum, NULL, iBufSize, iBuffers[buf]);
				iBufferHeaders.AppendL(bufHdr);
				}

			if (bufCount < iNumBufs)
				{
				iWaitingForBuffers = ETrue;
				}
			}
		}
	}

void CNonTunneledHandler::CPortInfo::SendAllocatedBufferToPeerL(OMX_U8* aBuffer)
	{
	__ASSERT_ALWAYS(iPeer, User::Invariant());

	iPeer->ReceiveAllocatedBufferFromPeerL(aBuffer);
	}

void CNonTunneledHandler::CPortInfo::ReceiveAllocatedBufferFromPeerL(OMX_U8* aBuffer)
	{
	if (iWaitingForBuffers)
		{
		OMX_BUFFERHEADERTYPE* bufHdr = NULL;

		iComp->UseBuffer(iComp, &bufHdr, iPortNum, NULL, iBufSize, aBuffer);
		iBufferHeaders.AppendL(bufHdr);
		
		if (iBufferHeaders.Count() == iNumBufs)
			{
			iWaitingForBuffers = EFalse;
			}
		}
	else
		{
		iBuffers.AppendL(aBuffer);
		}
	}

void CNonTunneledHandler::CPortInfo::FillBuffers()
	{
	iHoldingBuffers = EFalse;
	if (iSource)
		{
		for (TInt buf = 0; buf < iNumBufs; ++buf)
			{
			iComp->FillThisBuffer(iComp, iBufferHeaders[buf]);
			}
		}
	}

void CNonTunneledHandler::CPortInfo::BufferDone(OMX_BUFFERHEADERTYPE* aBufHdr)
	{
	for (TInt buf = 0; buf < iNumBufs; ++buf)
		{
		if (iBufferHeaders[buf] == aBufHdr)
			{
			iPeer->ReceiveBufferFromPeer(buf, aBufHdr);
			}
		}
	}

void CNonTunneledHandler::CPortInfo::SendBufferToPeer(TInt aBufHdrOffset, OMX_BUFFERHEADERTYPE* aPeerBufHdr)
	{
	__ASSERT_ALWAYS(iPeer, User::Invariant());

	iPeer->ReceiveBufferFromPeer(aBufHdrOffset, aPeerBufHdr);
	}

void CNonTunneledHandler::CPortInfo::ReceiveBufferFromPeer(TInt aBufHdrOffset, OMX_BUFFERHEADERTYPE* aPeerBufHdr)
	{
	__ASSERT_ALWAYS(aBufHdrOffset < iNumBufs, User::Invariant());

	if (!iHoldingBuffers)
		{
		iBufferHeaders[aBufHdrOffset]->nOffset = aPeerBufHdr->nOffset;
		iBufferHeaders[aBufHdrOffset]->nFilledLen = aPeerBufHdr->nFilledLen;

		if (iSource)
			{
			iComp->FillThisBuffer(iComp, iBufferHeaders[aBufHdrOffset]);
			}
		else
			{
			iBufferHeaders[aBufHdrOffset]->hMarkTargetComponent = aPeerBufHdr->hMarkTargetComponent;
			iBufferHeaders[aBufHdrOffset]->pMarkData = aPeerBufHdr->pMarkData;
			iBufferHeaders[aBufHdrOffset]->nTimeStamp = aPeerBufHdr->nTimeStamp;
			iBufferHeaders[aBufHdrOffset]->nFlags = aPeerBufHdr->nFlags;

			if(aPeerBufHdr->nFlags & OMX_BUFFERFLAG_EOS)
                {
                // TODO Logic for EOS
                if(iEosSignalledtoInput == FALSE)
                    {
                    iComp->EmptyThisBuffer(iComp, iBufferHeaders[aBufHdrOffset]);
                    iEosSignalledtoInput = TRUE;
                    }
                return;
                }
			iComp->EmptyThisBuffer(iComp, iBufferHeaders[aBufHdrOffset]);
			}
		}
	}

void CNonTunneledHandler::CPortInfo::HoldBuffers()
	{
	iHoldingBuffers = ETrue;
	}

void CNonTunneledHandler::CPortInfo::FreeBuffers()
	{
	for (TInt buf = 0; buf < iNumBufs; ++buf)
		{
		iComp->FreeBuffer(iComp, iPortNum, iBufferHeaders[buf]);
		}

	DeleteAllBuffers();
	}

void CNonTunneledHandler::CPortInfo::DeleteAllBuffers()
	{
	iBufferHeaders.Reset();

	if (iSource && iSupplier == OMX_BufferSupplyUnspecified)
		{
		iBuffers.ResetAndDestroy();
		}
	else
		{
		iBuffers.Reset();
		}
	}

TBool CNonTunneledHandler::CPortInfo::ComparePortNum(const TInt* aPortNum, const CPortInfo& aPortInfo)
	{
	return (*aPortNum == aPortInfo.iPortNum);
	}
