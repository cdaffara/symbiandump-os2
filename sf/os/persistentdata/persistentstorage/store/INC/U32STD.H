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

#if !defined(STORE_INC_U32STD_H)
#define STORE_INC_U32STD_H
#if !defined(__S32STD_H__)
#include <s32std.h>
#endif
#if !defined(__S32SHARE_H__)
#include <s32share.h>
#endif

#if defined(_DEBUG)
#define __DEBUG(s) s
#define __LEAVE(r) Util::__DbgLeave(r,_L8(__FILE__),__LINE__)
#define __LEAVE_NO_MEMORY() Util::__DbgLeaveNoMemory(_L8(__FILE__),__LINE__)
#define __LEAVE_IF_ERROR(r) Util::__DbgLeaveIfError(r,_L8(__FILE__),__LINE__)
#define __LEAVE_IF_NULL(p) Util::__DbgLeaveIfNull(p,_L8(__FILE__),__LINE__)
#else
#define __DEBUG(s)
#define __LEAVE(r) User::Leave(r)
#define __LEAVE_NO_MEMORY() User::LeaveNoMemory()
#define __LEAVE_IF_ERROR(r) User::LeaveIfError(r)
#define __LEAVE_IF_NULL(p) User::LeaveIfNull(p)
#endif

class Util
	{
public:
#if defined (_DEBUG)
	static void __DbgLeave(TInt aReason,const TDesC8& aFileName,TInt aLineNo); 
	static void __DbgLeaveNoMemory(const TDesC8& aFileName,TInt aLineNo);
	static TInt __DbgLeaveIfError(TInt aReason,const TDesC8& aFileName,TInt aLineNo); 
	static TAny *__DbgLeaveIfNull(TAny *aPtr,const TDesC8& aFileName,TInt aLineNo);
#endif
	};
//
template <class T>
class TCapture
	{
public:
	inline TCapture(T& aRef);
	inline T& Object() const;
private:
	T* iPtr;
	};
//
class TDesHeader
	{
public:
	TDesHeader() {}
	inline TDesHeader(const TDesC8& aDes8);
	inline TDesHeader(const TDesC16& aDes16);
	inline TDesHeader& operator=(const TDesC8& aDes8);
	inline TDesHeader& operator=(const TDesC16& aDes16);
//
	inline TBool IsWidth8() const;
	inline TBool IsWidth16() const;
	inline TInt Length() const;
//
	inline void ExternalizeL(RWriteStream& aStream) const;
	inline void InternalizeL(RReadStream& aStream);
private:
	TCardinality iVal;
	};
class TDesInternalizer
	{
public:
	inline const TDesHeader& Header() const;
	inline TDesHeader& Header();
//
	void operator()(TDes8& aDes8,RReadStream& aStream) const;
	void operator()(TDes16& aDes16,RReadStream& aStream) const;
	void ReadDesc8L (TDes8& aDes8,RReadStream& aStream) const;
	void ReadDesc16L (TDes16& aDes16,RReadStream& aStream) const;
private:
	TDesHeader iHeader;
	};
//
NONSHARABLE_CLASS(TPtrInput) : public MStreamInput
	{
public:
	inline TPtrInput(TAny* aPtr);
//
	TInt PushL(const TAny* aPtr,TInt aMaxLength);
	TStreamTransfer ReadFromL(MStreamBuf& aSource,TStreamTransfer aTransfer);
private:
	TUint8* iPtr;
	};
//
NONSHARABLE_CLASS(TPtrOutput) : public MStreamOutput
	{
public:
	inline TPtrOutput(const TAny* aPtr);
//
	TInt PullL(TAny* aPtr,TInt aMaxLength);
	TStreamTransfer WriteToL(MStreamBuf& aSink,TStreamTransfer aTransfer);
private:
	const TUint8* iPtr;
	};
//
NONSHARABLE_CLASS(HDirectStoreBuf) : public RShareBuf
	{
public:
	static HDirectStoreBuf* CreateL(TStreamExchange& aHost,TStreamId& anId,TInt aMode=ERead|EWrite);
	static HDirectStoreBuf* OpenL(TStreamExchange& aHost,TStreamId anId,TInt aMode=ERead|EWrite);
//
	virtual inline ~HDirectStoreBuf();
private:
	static HDirectStoreBuf* NewL(TStreamExchange& aHost,TInt anOffset,TInt aMode);
	inline HDirectStoreBuf(TInt anOffset);
//
	void DoRelease();
	TStreamPos DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset);
private:
	TInt iOff;
	};
//
template <class T>
inline TCapture<T> Capture(T& aRef)
	{return TCapture<T>(aRef);}


#include "U32STD.INL"
#endif
