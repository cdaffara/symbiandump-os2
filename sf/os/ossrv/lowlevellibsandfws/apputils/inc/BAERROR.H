// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Started by DWW, March 1997
// Error numbers
// 
//

#if !defined(__BAERROR_H__)
#define __BAERROR_H__

/**
@internalComponent
*/
#ifdef _UNICODE
#define KUidBaflDll KUidBaflDll16
#else
#define KUidBaflDll KUidBaflDll8
#endif

#define KUidBaflDllValue8 0x1000004e

/**
@internalComponent
*/
const TUid KUidBaflDll8={KUidBaflDllValue8};
const TUid KUidBaflDll16={0x10003A0F};

#endif
