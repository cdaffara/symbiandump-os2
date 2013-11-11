// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Signal data structure that supports marshalling
// 
//

#include <e32base.h>

#ifndef TSIGNALMESSAGE_H
#define TSIGNALMESSAGE_H

const TInt KSigMsgLength = 32;
const TUint8 KSigMsgVersion = 1;

/*
@internalComponent
*/
class TSignalMessage
	{
public:
	TUint8	mType;
	union
		{
		struct
			{
			TUint8	mSignal;
			TUint32	mValue;
			} mSigVal;
		struct
			{
			TUint8 	mSignal;	
			TInt32	mTimerId;
			} mRtSignal;
		TUint8 	mSignal;		
		TUint32 mTimeOut;
		TUint32 mData;
		TUint64	mPid;
		};

	enum {
		ESignal = 0,
		ESignalValuePair,
		EAlarmRegistration,
		EDequeueSignal,
		EWaitOnChild,
		ERtTimerSignal,
	} TSignalMessageType;

	virtual ~TSignalMessage() {}

	static TBool SignalMatchBySigNum(const TSignalMessage& aMsg1, const TSignalMessage& aMsg2)
		{
		if(aMsg1.mType != ESignal || aMsg2.mType != ESignal)
			return EFalse;

		if (aMsg1.mSignal == aMsg2.mSignal)
			{
			return ETrue;
			}
		return EFalse;
		}

	static TBool SigValMatchBySigNum(const TSignalMessage& aMsg1, const TSignalMessage& aMsg2)
		{
		if(aMsg1.mType != ESignalValuePair || aMsg2.mType != ESignalValuePair)
			return EFalse;

		if (aMsg1.mSigVal.mSignal == aMsg2.mSigVal.mSignal)
			{
			return ETrue;
			}
		return EFalse;
		}

	inline TInt Marshall(TDes8& aBuffer)
		{
		TUint8	lBufPtr[KSigMsgLength] = {0};

		if(aBuffer.MaxSize() != KSigMsgLength)
			return KErrArgument;

		lBufPtr[0] = KSigMsgVersion;
		lBufPtr[1] = mType;

		switch(mType)
			{
			case ESignal:
				lBufPtr[2] = mSignal;
				break;
			case ESignalValuePair:
				lBufPtr[2] = mSigVal.mSignal;

				lBufPtr[3] = (TUint8)(mSigVal.mValue & 0x000000FF);
				lBufPtr[4] = (TUint8)((mSigVal.mValue & 0x0000FF00) >> 8);
				lBufPtr[5] = (TUint8)((mSigVal.mValue & 0x00FF0000) >> 16);
				lBufPtr[6] = (TUint8)((mSigVal.mValue & 0xFF000000) >> 24);
				break;
			case EAlarmRegistration:
				lBufPtr[2] = (TUint8)(mTimeOut & 0x000000FF);
				lBufPtr[3] = (TUint8)((mTimeOut & 0x0000FF00) >> 8);
				lBufPtr[4] = (TUint8)((mTimeOut & 0x00FF0000) >> 16);
				lBufPtr[5] = (TUint8)((mTimeOut & 0xFF000000) >> 24);
				break;
			case EDequeueSignal:
				lBufPtr[2] = mSignal;
				break;
			case EWaitOnChild:
				memcpy(&lBufPtr[2],&mPid,sizeof(mPid));
				break;
			case ERtTimerSignal:
				Mem::Copy(&lBufPtr[2], &mRtSignal.mSignal, sizeof(mRtSignal.mSignal));
				Mem::Copy(&lBufPtr[2+sizeof(mRtSignal.mSignal)], &mRtSignal.mTimerId, sizeof(mRtSignal.mTimerId));
				break;
			default:
				// do nothing
				return KErrGeneral;
				break;
			}
		aBuffer.Zero();
		aBuffer.Append(lBufPtr,KSigMsgLength);
		return KErrNone;
		}

	inline TInt Unmarshall(TDes8& aBuffer)
		{
		const TUint8* lBufPtr = aBuffer.Ptr();

		if(aBuffer.MaxSize() != KSigMsgLength)
			return KErrArgument;

		if(lBufPtr[0] != KSigMsgVersion)
			return KErrGeneral;

		mType = lBufPtr[1];

		switch(mType)
			{
			case ESignal:
				mSignal = lBufPtr[2];
				break;
			case ESignalValuePair:
				mSigVal.mSignal = lBufPtr[2];
				mSigVal.mValue = 0;
				mSigVal.mValue |= (TUint32)lBufPtr[3];
				mSigVal.mValue |= (((TUint32)lBufPtr[4]) << 8);
				mSigVal.mValue |= (((TUint32)lBufPtr[5]) << 16);
				mSigVal.mValue |= (((TUint32)lBufPtr[6]) << 24);
				break;
			case EAlarmRegistration:
				mTimeOut = 0;
				mTimeOut |= (TUint32)lBufPtr[2];
				mTimeOut |= (((TUint32)lBufPtr[3]) << 8);
				mTimeOut |= (((TUint32)lBufPtr[4]) << 16);
				mTimeOut |= (((TUint32)lBufPtr[5]) << 24);
				break;
			case EDequeueSignal:
				mSignal = lBufPtr[2];
				break;
			case EWaitOnChild:
				mPid = 0;
				memcpy(&mPid,&lBufPtr[2],sizeof(mPid));
				break;
			case ERtTimerSignal:
				Mem::Copy(&mRtSignal.mSignal, &lBufPtr[2], sizeof(mRtSignal.mSignal));
				Mem::Copy(&mRtSignal.mTimerId,&lBufPtr[2+sizeof(mRtSignal.mSignal)], sizeof(mRtSignal.mTimerId));
				break;
			default:
				// do nothing
				return KErrGeneral;
				break;
			}

		return KErrNone;
		}
	};

#endif // TSIGNALMESSAGE_H
