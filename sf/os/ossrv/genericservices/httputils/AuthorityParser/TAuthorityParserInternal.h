// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// File contains local functions for use by the Authority parser class defined in
// AuthorityParser.h.
// 
//

/**
 @file TAuthorityParserInternal.h
 @see Authority8.h, Authority16.h
*/

#ifndef __TAUTHORITYPARSERINTERNAL_H__
#define __TAUTHORITYPARSERINTERNAL_H__

// System includes
//
#include <e32base.h>

template<class TPtrCType> 
void DoParseAuthority(const TPtrCType& aAuthority, TPtrCType aComponent[]);

template<class TPtrCType> 
TInt ParseUserInfo(const TPtrCType& aAuthority, TPtrCType& aComponent);

template<class TPtrCType> 
TInt ParseHost(const TPtrCType& aAuthority, TPtrCType& aComponent);

template<class TPtrCType> 
TInt ParsePort(const TPtrCType& aAuthority, TPtrCType& aComponent);

#endif	// __TAUTHORITYPARSERINTERNAL_H__
