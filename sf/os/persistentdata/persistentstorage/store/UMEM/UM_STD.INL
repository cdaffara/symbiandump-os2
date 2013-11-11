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

inline TUint8* TMemBuf::Base() const
	{
	__ASSERT_DEBUG(iBase!=NULL,User::Invariant());
	return iBase;
	}
inline TUint8* TMemBuf::End() const
	{
	__ASSERT_DEBUG(TStreamBuf::End(ERead)==TStreamBuf::End(EWrite),User::Invariant());
	return TStreamBuf::End(EWrite);
	}

inline TDes8& TDesBuf::Des() const
	{
	__ASSERT_DEBUG(iDes!=NULL,User::Invariant());
	return *iDes;
	}
inline TUint8* TDesBuf::Base() const
	{
	__ASSERT_DEBUG(Des().Ptr()!=NULL,User::Invariant());
	return (TUint8*)Des().Ptr();
	}

inline CBufBase& TBufBuf::Buf() const
	{
	__ASSERT_DEBUG(iBuf!=NULL,User::Invariant());
	return *iBuf;
	}
inline void TBufBuf::SetPos(TRead,TInt aPos)
	{iRPos=aPos;}
inline void TBufBuf::SetPos(TWrite,TInt aPos)
	{iWPos=aPos;}
inline TInt TBufBuf::Pos(TRead) const
	{return iRPos;}
inline TInt TBufBuf::Pos(TWrite) const
	{return iWPos;}
inline TInt TBufBuf::MovePos(TRead,TInt anOffset)
	{return iRPos+=anOffset;}
inline TInt TBufBuf::MovePos(TWrite,TInt anOffset)
	{return iWPos+=anOffset;}
inline TInt TBufBuf::Mark(TRead) const
	{return Pos(ERead)-Avail(ERead);}
inline TInt TBufBuf::Mark(TWrite) const
	{return Pos(EWrite)-Avail(EWrite);}

