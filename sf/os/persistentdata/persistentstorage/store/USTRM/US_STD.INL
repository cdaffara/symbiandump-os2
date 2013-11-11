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

inline TSourceOutput::TSourceOutput(MStreamBuf* aSource)
	: iSrc(aSource)
	{
	__ASSERT_DEBUG(aSource!=NULL,Panic(EStreamNotOpen));
	}

inline TFilterInput::TFilterInput(TStreamFilter& aFilter,TAny* aPtr,TInt aMaxLength)
	: iFltr(&aFilter),iPtr((TUint8*)aPtr),iLeft(aMaxLength)
	{}
inline TBool TFilterInput::Done() const
	{return iLeft==0;}
inline TBool TFilterInput::Eof() const
	{return iPtr==NULL;}
inline TInt TFilterInput::Left() const
	{return iLeft;}

inline TFilterOutput::TFilterOutput(TStreamFilter& aFilter,const TAny* aPtr,TInt aLength)
	: iFltr(&aFilter),iFrom((TUint8*)aPtr),iEnd((TUint8*)aPtr+aLength)
	{}
inline TBool TFilterOutput::Done() const
	{return iFrom==iEnd;}

inline TUint8* TDelimitedInput8::Ptr() const
	{return iPtr;}
inline TInt TDelimitedInput8::Done() const
	{return iLeft==0;}

inline TUint16* TDelimitedInput16::Ptr() const
	{return iPtr;}
inline TInt TDelimitedInput16::Done() const
	{return iLeft==0;}

inline TBool TStreamMark::IsTracking(TStreamMark*const& __DEBUG(aRef)) const
	{
#if defined (_DEBUG)
	return iPos==KStreamBeginning-1-(TUint(&aRef)>>2);
#else
	return IsEmpty();
#endif
	}
inline void TStreamMark::Track(TStreamMark*const& __DEBUG(aRef))
	{
#if defined (_DEBUG)
	iPos=KStreamBeginning-1-(TUint(&aRef)>>2);
	__ASSERT_DEBUG(iPos<KStreamBeginning-1,User::Invariant());
#else
	Clear();
#endif
	}

inline MStreamBuf& TStreamExchange::BufL() const
	{
	if (iHost==NULL)
		User::Leave(KErrNotReady);
	return *iHost;
	}
inline MStreamBuf& TStreamExchange::Buf() const
	{
	__ASSERT_DEBUG(iHost!=NULL,User::Invariant());
	return *iHost;
	}

inline TStreamExchange& RShareBuf::Host() const
	{
	__ASSERT_DEBUG(iHost!=NULL,User::Invariant());
	return *iHost;
	}

