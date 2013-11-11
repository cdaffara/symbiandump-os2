// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// #include <cwchar>
// #include <climits>
#include <cstdio>
#include <cwchar>
#include <climits>

#if defined(WCHAR_MIN) //&& (WCHAR_MIN == 0)
// do nothing, this is test for macro expansion
#endif

#if defined(WCHAR_MAX) //&& (WCHAR_MAX <= USHRT_MAX)
// do nothing, this is test for macro expansion
#endif
