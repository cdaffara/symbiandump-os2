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
// File contains local functions for use by the CUri classes defined in
// Uri8.h and Uri16.h.
// 
//

/**
 @file CUriInternal.h
 @see Uri8.h, Uri16.h
*/

#ifndef __CURIINTERNAL_H__
#define __CURIINTERNAL_H__

// System includes
//
#include <e32base.h>

/** 
	enum TInternalFileUriFlags
	Enum defining a flag used to indicate file type (public or private) 
	@internalComponent
	@released
	@since 9.1	
 */
enum TInternalFileUriFlags
	 {
	 /* Specifier for application private file on fix drive */
	 EPrivate   = 0x80000000
	 };

// Forward class declarations
//
class CDelimitedDataBase16;
class CDelimitedDataBase8;

template<class TPtrCType>
TInt CalculateUriLength(const TPtrCType aComponent[], TBool& aIsIPv6Host);

template<class TPtrType, class TPtrCType>
void DoFormUri(TPtrType& aUri, TPtrCType aComponent[], TBool& aIsIPv6Host);

template<class TPtrType, class TPtrCType> 
void SetScheme(TPtrType& aUri, TPtrCType& aScheme);

template<class TPtrType, class TPtrCType> 
void SetAuthority(TPtrType& aUri, TPtrCType& aUserinfo, TPtrCType& aHost, TPtrCType& aPort, TBool& aIsIPv6Host, TBool aUseNetworkDelimiter);

template<class TPtrType, class TPtrCType> 
void SetPath(TPtrType& aUri, TPtrCType& aPath);

template<class TPtrType, class TPtrCType> 
void SetQuery(TPtrType& aUri, TPtrCType& aQuery);

template<class TPtrType, class TPtrCType> 
void SetFragment(TPtrType& aUri, TPtrCType& aFragment);

template<class TPtrCType, class CDelimitedDataBaseType>
void CleanResolvedPathL(CDelimitedDataBaseType* aResolvedPath);

template<class HBufCType, class TUriCType>
HBufCType* FormResolvedPathLC(const TUriCType& aBaseUri, const TUriCType& aRefUri, TBool& aUseBaseQuery);

template<class TPtrCType, class TUriCType, class HBufCType>
void FormResolvedUri(TPtrCType aComponent[], const TUriCType& aBaseUri, const TUriCType& aRefUri, const HBufCType* aResolvedPath, TBool aUseBaseQuery);

HBufC* GenerateFileUriPathL(const TDesC& aFileName, TDriveNumber aDrive,  TUint aFlags);

HBufC8* ResolvePathsL(const TDesC8& aBasePath, const TDesC8& aRefPath);

HBufC16* ResolvePathsL(const TDesC16& aBasePath, const TDesC16& aRefPath);

TBool IsSameDir(const TDesC8& aSegment);

TBool IsSameDir(const TDesC16& aSegment);

TBool IsParentDir(const TDesC8& aSegment);

TBool IsParentDir(const TDesC16& aSegment);

void InsertParentDirL(CDelimitedDataBase8* aResolvedPath);

void InsertParentDirL(CDelimitedDataBase16* aResolvedPath);

#endif	// __CURIINTERNAL_H__
