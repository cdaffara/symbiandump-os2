// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#if !defined(__U32FRAME_H__)
#define __U32FRAME_H__
#if !defined(__S32SHARE_H__)
#include <s32share.h>
#endif

#if defined(_DEBUG)&&!defined(__SMALL_FRAME)
//#define __SMALL_FRAME
#endif
//
enum TFrameType16
	{
	EFrameFree16=0x0000,
	EFrameData16=0x4000,
	EFrameDescriptive16=0x8000,
	EFrameContinuation16=0xc000
	};
typedef TUint16 TFrameDes16;
const TInt KSizeFrameDes16=sizeof(TFrameDes16);
const TInt KShiftFrameDes16=1;
//
const TInt KMaskFrame16=0xffff;
const TInt KMaskFrameType16=0xc000;
#if defined(__SMALL_FRAME)
const TInt KMaskFrameLength16=0xf;
const TInt KShiftFrameLength16=4;
#else
const TInt KMaskFrameLength16=0x3fff;
const TInt KShiftFrameLength16=14;
#endif
//
const TInt KFrameOpen16=0;
const TInt KFrameMinLength16=1;
const TInt KFrameMaxLength16=KMaskFrameLength16;
const TInt KFrameFullLength16=KMaskFrameLength16+1;
//
const TInt KFrameNonexistent16=-1;
//
NONSHARABLE_CLASS(RFrame16Buf) : public MStreamBuf
	{
public:
	static TInt Offset(TStreamPos aBase,TStreamPos aPos);
	static TStreamPos Position(TStreamPos aBase,TInt anOffset);
//
	RFrame16Buf(TStreamPos aBase);
	inline TStreamPos Base() const;
	inline TInt Offset(TStreamPos aPos) const;
	inline TStreamPos Position(TInt anOffset) const;
//
	TInt ExtendL(TStreamExchange& aHost,TInt anExtent,TInt aMode=ERead|EWrite);
	void OpenL(TStreamExchange& aHost,TInt anOffset,TInt aMode=ERead|EWrite);
	void Set(TStreamExchange& aHost,TInt anOffset,TInt anExtent,TInt aMode=ERead|EWrite);
//
	inline TInt Offset() const;
	inline TInt Extent() const;
protected:
	inline TBool IsCommitted() const;
	void CommitL();
//
	void DoRelease();
	void DoSynchL();
private:
	TInt DoReadL(TAny* aPtr,TInt aMaxLength);
	TInt DoReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	TStreamTransfer DoReadL(MStreamInput& anInput,TStreamTransfer aTransfer);
	void DoWriteL(const TAny* aPtr,TInt aLength);
	TInt DoWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	TStreamTransfer DoWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer);
	TStreamPos DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset);
//
	inline TStreamExchange& Host() const;
	TInt UnderflowL();
	void OverflowL();
	TBool ReachL(TInt anOffset);
	TInt EndL();
private:
	TStreamPos iBase;
	TStreamExchange* iHost;
	TInt iOff;
	TInt iExt;
	TStreamMark iRMark,iWMark;
	TInt iRAvail,iWAvail;
	TInt iRLim,iWLim;
	TInt iMode;
	};

#include "U32FRAME.INL"
#endif
