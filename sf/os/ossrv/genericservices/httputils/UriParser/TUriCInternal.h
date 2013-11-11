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
// File contains local functions for use by the TUriC classes defined in
// Uri8.h and Uri16.h.
// 
//

/**
 @file TUriCInternal.h
 @see Uri8.h, Uri16.h
*/

#ifndef __TURICINTERNAL_H__
#define __TURICINTERNAL_H__

// System includes
//
#include <e32base.h>

// Constants
//
const TInt KPlusChar			= '+';
const TInt KMinusChar			= '-';
const TInt KPeriodChar			= '.';

//File URI Constants
//
_LIT(KPrivate, "private/");
_LIT(KExtMedia, "ext-media/");
_LIT8(KFileUriScheme8, "file");	
_LIT(KFileUriScheme16, "file");
_LIT(KDefaultPath,"C:\\");

const TInt KDriveSeparator = ':';
const TInt KUriPathSeparator = '/';
const TInt KFilePathSeparator = '\\';



template<class TPtrCType>
LOCAL_C TBool HasValidSchemeChars(const TPtrCType& aScheme);

HBufC8* CreateUri8LC(const TDesC16& aBuf16);

TInt DoValidate(const TUriC8& aUri);

TInt DoEquivalenceL(const TUriC8& aLhs, const TUriC8& aRhs);

void ChangePathSeparator(TDes& aDesPtr, TUint aPathSeperatorFrom, TUint aPathSeperatorTo);

HBufC* GetFullFileNameFromFileUriPathL(const TDesC& aFileUriPath);
HBufC* ResolveFileNameL(const TDesC8& aPath, TUriFileName aType, TBool aIsFileUri);
void GetFileComponent(TPtrC& aNewName, const TDesC& aOldName, TUriFileName aType );


#endif	// __TURICINTERNAL_H__
