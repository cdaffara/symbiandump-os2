/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


 
#ifndef __GLIBBACKENDINTERNAL_H__
#define __GLIBBACKENDINTERNAL_H__

#include <e32std.h>

#ifdef __WINSCW__
RHeap* PrivateHeap();
#define privateHeap (PrivateHeap())
#else
extern RHeap* privateHeap;
#endif

void InitGLIBHeap(RHeap*& );

#endif /* __GLIBBACKENDINTERNAL_H__ */
