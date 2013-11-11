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

inline CStreamStore& CSecureStore::Host()
	{
	__ASSERT_DEBUG(iHost!=NULL,User::Invariant());
	return *iHost;
	}
inline const CStreamStore& CSecureStore::Host() const
	{
	__ASSERT_DEBUG(iHost!=NULL,User::Invariant());
	return *iHost;
	}
inline const CPBEncryptSet& CSecureStore::PBEKey() const
	{
    return iKey;
	}
inline HEncryptFilter::~HEncryptFilter()
	{TEncryptFilter::DoRelease();}

inline HDecryptFilter::~HDecryptFilter()
	{TDecryptFilter::DoRelease();}

