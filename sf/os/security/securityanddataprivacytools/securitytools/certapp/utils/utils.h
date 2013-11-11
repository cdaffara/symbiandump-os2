#ifndef __UTILS_H__
#define __UTILS_H__/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @internalComponent
*/
void OpenUtf8FStreamForRead(std::fstream &aFStream, const char *aUtf8FileName);
void OpenUtf8FStreamForWrite(std::fstream &aFStream, const char *aUtf8FileName);


#ifndef __LINUX__
#include <windows.h>
WCHAR *WindowsUtf8ToUtf16(const char *aUtf8);
CHAR *WindowsUtf16ToUtf8(const WCHAR *aUtf16);
WCHAR *WindowsRetrieveDosName(const WCHAR *aLongName);
#endif

#endif
