// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef EXPAT_CONFIG_H
#define EXPAT_CONFIG_H

#define HAVE_MEMMOVE

#ifndef NULL
#define NULL 0
#endif

#define XML_MIN_SIZE
#define SYMBIAN_MIN_SIZE

// Enable namespace processing
#define XML_NS

#ifdef _UNICODE
#ifndef XML_UNICODE
#define XML_UNICODE
#endif
#ifdef XML_UNICODE_WCHAR_T
#define XML_UNICODE_WCHAR_T
#endif
#endif // _UNICODE

#endif // EXPAT_CONFIG_H
