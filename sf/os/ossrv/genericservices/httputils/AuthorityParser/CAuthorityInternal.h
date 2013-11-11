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
// This file provides an internal API used by the class CAuthority?? for 
// setting the value of authority components in a descriptor.
// 
//

/**
 @file CAuthorityInternal.h
 @see Authority8.h, Authority16.h
*/

#ifndef __CAUTHORITYINTERNAL_H__
#define __CAUTHORITYINTERNAL_H__

// System includes
//
#include <e32base.h>

template<class TPtrCType>
TInt CalculateAuthorityLength(const TPtrCType aComponent[], TBool& aIsIPv6Host);

template<class TPtrType, class TPtrCType>
void DoFormAuthority(TPtrType& aAuthority, TPtrCType aComponent[], const TBool& aIsIPv6Host);

template<class TPtrType, class TPtrCType> 
void SetUserinfo(TPtrType& aAuthority, TPtrCType& aUserinfo);

template<class TPtrType, class TPtrCType> 
void SetHost(TPtrType& aAuthority, TPtrCType& aHost);

template<class TPtrType, class TPtrCType> 
void SetPort(TPtrType& aAuthority, TPtrCType& aPort);

#endif	// __CAUTHORITYINTERNAL_H__
