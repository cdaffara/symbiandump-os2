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


#ifndef NONTUNNELEDHANDLER_H
#define NONTUNNELEDHANDLER_H

#include <e32base.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>

NONSHARABLE_CLASS(CNonTunneledHandler) : public CBase
	{
public:
	CNonTunneledHandler();
	~CNonTunneledHandler();

	void AddLinkL(OMX_COMPONENTTYPE* aSourceComp, TInt aSourcePort,
	              OMX_COMPONENTTYPE* aSinkComp, TInt aSinkPort,
	              OMX_BUFFERSUPPLIERTYPE aSupplier, TInt aNumBufs, TInt aBufSize);
	void AllocateBuffersL(OMX_COMPONENTTYPE* aComp);
	void FillBuffers(OMX_COMPONENTTYPE* aComp);
	void BufferDone(OMX_COMPONENTTYPE* aComp, OMX_BUFFERHEADERTYPE* aBufHdr, TBool aSource);
	void HoldBuffers(OMX_COMPONENTTYPE* aComp);
	void FreeBuffers(OMX_COMPONENTTYPE* aComp);

private:
	NONSHARABLE_CLASS(CPortInfo) : public CBase
		{
	public:
		CPortInfo(OMX_COMPONENTTYPE* aComp, TInt aPortNum, TBool aSource, TInt aNumBufs, TInt aBufSize, OMX_BUFFERSUPPLIERTYPE aSupplier);
		~CPortInfo();
		void SetPeer(CPortInfo* aPeer);
		void AllocateBuffersL();
		void FillBuffers();
		void SendAllocatedBufferToPeerL(OMX_U8* aBuffer);
		void ReceiveAllocatedBufferFromPeerL(OMX_U8* aBuffer);
		void BufferDone(OMX_BUFFERHEADERTYPE* aBufHdr);
		void SendBufferToPeer(TInt aBufHdrOffset, OMX_BUFFERHEADERTYPE* aPeerBufHdr);
		void ReceiveBufferFromPeer(TInt aBufHdrOffset, OMX_BUFFERHEADERTYPE* aPeerBufHdr);
		void HoldBuffers();
		void FreeBuffers();
		void DeleteAllBuffers();
		static TBool ComparePortNum(const TInt* aPortNum, const CPortInfo& aPortInfo);

	private:
		OMX_COMPONENTTYPE* iComp;
		TInt iPortNum;
		TBool iSource;
		TInt iNumBufs;
		TInt iBufSize;
		OMX_BUFFERSUPPLIERTYPE iSupplier;
		RPointerArray<OMX_U8> iBuffers;
		RPointerArray<OMX_BUFFERHEADERTYPE> iBufferHeaders;
		CPortInfo* iPeer;
		TBool iWaitingForBuffers;
		TBool iHoldingBuffers;
		TBool iEosSignalledtoInput;
		};

	NONSHARABLE_CLASS(CComponentInfo) : public CBase
		{
	public:
		CComponentInfo(OMX_COMPONENTTYPE* aComp);
		~CComponentInfo();
		CPortInfo* AddPortL(TInt aPortNum, TBool aSource, TInt aNumBufs, TInt aBufSize, OMX_BUFFERSUPPLIERTYPE aSupplier);
		void AllocateBuffersL();
		void FillBuffers();
		void BufferDone(OMX_BUFFERHEADERTYPE* aBufHdr, TBool aSource);
		void HoldBuffers();
		void FreeBuffers();
		static TBool CompareComp(const OMX_COMPONENTTYPE* aSource, const CComponentInfo& aComponentInfo);

	private:
		OMX_COMPONENTTYPE* iComp;
		RPointerArray<CPortInfo> iPorts;
		};

	RPointerArray<CComponentInfo> iComponents;
	};

#endif // NONTUNNELEDHANDLER_H
