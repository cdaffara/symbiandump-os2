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

// Class RFrame16Buf
inline TStreamPos RFrame16Buf::Base() const
	{return iBase;}
inline TInt RFrame16Buf::Offset(TStreamPos aPos) const
	{return Offset(iBase,aPos);}
inline TStreamPos RFrame16Buf::Position(TInt anOffset) const
	{return Position(iBase,anOffset);}
inline TInt RFrame16Buf::Offset() const
	{return iOff;}
inline TInt RFrame16Buf::Extent() const
	{return iExt;}
inline TBool RFrame16Buf::IsCommitted() const
	{return iExt!=0;}
inline TStreamExchange& RFrame16Buf::Host() const
	{
	__ASSERT_DEBUG(iHost!=NULL,User::Invariant());
	return *iHost;
	}

