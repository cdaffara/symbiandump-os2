/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name        : wcharcnv.h
* Part of     : MRT LIBC
* Header file for wcharcnv.cpp. It contains the prototype and defines required in
* wcharcnv.cpp
* Version     : 
* This material, including documentation and any related 
* computer programs, is protected by copyright controlled by 
* Nokia Corporation. All rights are reserved. Copying, 
* including reproducing, storing, adapting or translating, any 
* or all of this material requires the prior written consent of 
* Nokia Corporation. This material also contains confidential 
* information which may not be disclosed to others without the 
* prior written consent of Nokia Corporation.
*
*/


 
#ifndef _WCHARCNV_H_
#define	_WCHARCNV_H_

enum _UTF16State
{
	_EUTF16InitialState,
	_EUTF16_21BitExtensionState,
};

TInt _Utf16ToUtf8(char* dst, wchar_t aSrc, mbstate_t* ps, int aLen);
TInt ConvertToUnicodeFromUtf8(TDes16& aUnicode, const TDesC8& aUtf8, mbstate_t *state);

#endif //_WCHARCNV_H_


